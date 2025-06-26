/**
 * @file mcp_openvto.cpp
 * @brief MCP Server for Couchbase/Local and Replicate APIs
 * @author Nikhil Kapila
 * @date 2025-06-22 00:03:35 Sunday
 * @version 1.0.0
 *
 * Follows the 2024-11-05 basic protocol specification.
 *
 */

#include "mcp_server.h"
#include "mcp_tool.h"
#include "mcp_resource.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <filesystem>
#include <algorithm>

// utils
#include "utils/csv_parser.h"
#include "utils/couchbase_search.h"
#include "utils/replicate_inference.h"
#include "utils/open_browser.h"

struct Config{
    // couchbase configs
    std::string user;
    std::string pass;
    std::string hostname;
    int port;
    std::string bucket;
    std::string scope;
    std::string search_index;

    // replicate configs
    std::string api_key;
    std::string version;

    // local file path for csv
    std::string csv_filepath;
    // local file path for base image
    std::string img_filepath;
} config;

enum FunctionalityAvailability{ //lol@name
    LOCAL,
    COUCHBASE,
    NO_REPLICATE_KEY,
    NEEDS_CONFIG,
    ALL
};

static Config parse_config(int argc, char* argv[]) {
    Config config;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--user") == 0) {
            if (i + 1 < argc) {
                config.user = argv[++i];
            } else {
                std::cerr << "Error: --user requires a value" << std::endl;
                exit(1);
            }
        } else if (strcmp(argv[i], "--pass") == 0) {
            if (i + 1 < argc) {
                config.pass = argv[++i];
            } else {
                std::cerr << "Error: --pass requires a value" << std::endl;
                exit(1);
            }
        } else if (strcmp(argv[i], "--hostname") == 0) {
            if (i + 1 < argc) {
                config.hostname = argv[++i];
            } else {
                std::cerr << "Error: --hostname requires a value" << std::endl;
                exit(1);
            }
        } else if (strcmp(argv[i], "--port") == 0) {
            if (i + 1 < argc) {
                try {
                    config.port = std::stoi(argv[++i]);
                    if (config.port < 1 || config.port > 65535) {
                        throw std::invalid_argument("Port must be between 1 and 65535");
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing port: " << e.what() << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: --port requires a value" << std::endl;
                exit(1);
            }
        } else if (strcmp(argv[i], "--bucket") == 0) {
            if (i + 1 < argc) {
                config.bucket = argv[++i];
            } else {
                std::cerr << "Error: --bucket requires a value" << std::endl;
                exit(1);
            }
        } else if (strcmp(argv[i], "--scope") == 0) {
            if (i + 1 < argc) {
                config.scope = argv[++i];
            } else {
                std::cerr << "Error: --scope requires a value" << std::endl;
                exit(1);
            }
        } else if (strcmp(argv[i], "--search-index") == 0) {
            if (i + 1 < argc) {
                config.search_index = argv[++i];
            } else {
                std::cerr << "Error: --search-index requires a value" << std::endl;
                exit(1);
            }
        } else if (strcmp(argv[i], "--api-key") == 0) {
            if (i + 1 < argc) {
                config.api_key = argv[++i];
            } else {
                std::cerr << "Error: --api-key requires a value" << std::endl;
                exit(1);
            }
        } else if (strcmp(argv[i], "--version") == 0) {
            if (i + 1 < argc) {
                config.version = argv[++i];
            } else {
                std::cerr << "Error: --version requires a value" << std::endl;
                exit(1);
            }
        } else if (strcmp(argv[i], "--csv-filepath") == 0) {
            if (i + 1 < argc) {
                config.csv_filepath = argv[++i];
            } else {
                std::cerr << "Error: --csv_filepath requires a value" << std::endl;
                exit(1);
            }
        } else if (strcmp(argv[i], "--img-filepath") == 0) {
            if (i + 1 < argc) {
                config.img_filepath = argv[++i];
            } else {
                std::cerr << "Error: --img-filepath requires a value" << std::endl;
                exit(1);
            }
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            std::cout << "Usage: " << argv[0] << " [OPTIONS]\n\n";
            std::cout << "Couchbase Options:\n";
            std::cout << "  --user <username>        Couchbase username\n";
            std::cout << "  --pass <password>        Couchbase password\n";
            std::cout << "  --hostname <hostname>    Couchbase hostname\n";
            std::cout << "  --port <port>            Couchbase port (default: 18094)\n";
            std::cout << "  --bucket <bucket>        Couchbase bucket name\n";
            std::cout << "  --scope <scope>          Couchbase scope name\n";
            std::cout << "  --search-index <index>   Couchbase search index name\n\n";
            std::cout << "Replicate Options:\n";
            std::cout << "  --api-key <key>          Replicate API key\n";
            std::cout << "  --version <version>      Replicate model version\n\n";
            std::cout << "File Options:\n";
            std::cout << "  --csv_filepath <path>        Path to CSV file\n\n";
            std::cout << "  --img_filepath <path>        Path to base img file\n\n";
            std::cout << "Other Options:\n";
            std::cout << "  --help, -h               Show this help message\n";
            exit(0);
        } else {
            std::cerr << "Unknown option: " << argv[i] << std::endl;
            std::cerr << "Use --help for usage information" << std::endl;
            exit(1);
        }
    }
    return config;
}

FunctionalityAvailability eval_availability(const Config& config){
    bool has_couchbase = !config.user.empty() && 
                        !config.pass.empty() && 
                        !config.hostname.empty() && 
                        config.port > 0 && 
                        !config.bucket.empty() && 
                        !config.scope.empty() && 
                        !config.search_index.empty();

    bool has_replicate = !config.api_key.empty() && !config.version.empty();

    bool has_local_files = !config.csv_filepath.empty() && !config.img_filepath.empty();

    // unusable if no access to idm-vton
    if (!has_replicate){
        return FunctionalityAvailability::NO_REPLICATE_KEY;
    }

    // if everything available, all gud
    if (has_couchbase && has_replicate && has_local_files){
        return FunctionalityAvailability::ALL;
    } else if (has_couchbase && has_replicate){ // if couchbase and replicate
        return FunctionalityAvailability::COUCHBASE;
    } else if (has_local_files && has_replicate){ // if local nad replicate
        return FunctionalityAvailability::LOCAL;    
    } else {
        return FunctionalityAvailability::NEEDS_CONFIG;
    }
}

// search locally using provided .CSV
std::string local_search(){


}

// couchbase vector search
mcp::json couchbase_vector_searcher(const mcp::json& params){

}

// inference using replicate
mcp::json replicate_inference(const mcp::json& params){


}

// TODO: need to add image resource

int main(int argc, char* argv[]){
    // parse config
    config = parse_config(argc, argv);

    // check what's available and make corresponding tools available
    FunctionalityAvailability check = eval_availability(config);
    if (check == FunctionalityAvailability::NEEDS_CONFIG){
        // throw unintialize config error
    } else if (check == FunctionalityAvailability::NO_REPLICATE_KEY){
        // cannot run server if replicate key is missing
    }


    mcp::server server("localhost", 8888);
    server.set_server_info("MCP OpenVTO in C++", "1.0.0");

    mcp::json capabilities = {
        {"tools", } // add tools here
    };
    server.set_capabilities(capabilities);

    // tool registry
    mcp::tool style_transfer = mcp::tool_builder("perform_style_transfer")
        .with_description("")
        .build(); // TODO need to add params

    server.register_tool(style_transfer, replicate_inference);

    // Start server
    std::cout << "Starting MCP server at localhost:8888..." << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;
    server.start(true);  // Blocking mode

    return 0;
}
