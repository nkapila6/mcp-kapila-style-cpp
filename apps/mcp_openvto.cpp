/**
 * @file mcp_openvto.cpp
 * @brief MCP Server for Couchbase/Local and Replicate APIs
 * @author Nikhil Kapila
 * @date 2025-06-22 00:03:35 Sunday
 * @version 0.0.1
 *
 * Follows the 2024-11-05 basic protocol specification.
 *
 */

// mcp requirements
#include "mcp_server.h"
#include "mcp_tool.h"

// standard headers
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

// 3rd party headers
#include <Eigen/Dense>
#include "ollama.hpp"

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
    std::string search_field;

    // replicate configs
    std::string api_key;
    std::string version;

    // local file path for csv
    std::string csv_filepath;
    // uploaded human img link for base image
    std::string img_link;

    // verbosity
    bool verbose;
} config;

enum FunctionalityAvailability{ //lol@name
    LOCAL,
    COUCHBASE,
    NO_REPLICATE_KEY,
    NEEDS_CONFIG,
    ALL
};

bool parse_bool(const std::string& str) {
    if (str == "true" || str == "1" || str == "yes") return true;
    if (str == "false" || str == "0" || str == "no") return false;
    throw std::invalid_argument("Invalid boolean: " + str);
}

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
        } else if (strcmp(argv[i], "--search-field") == 0) {
            if (i + 1 < argc) {
                config.search_field = argv[++i];
            } else {
                std::cerr << "Error: --search-field requires a value" << std::endl;
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
        } else if (strcmp(argv[i], "--img-link") == 0) {
            if (i + 1 < argc) {
                config.img_link = argv[++i];
            } else {
                std::cerr << "Error: --img-filepath requires a value" << std::endl;
                exit(1);
            }
        } else if (strcmp(argv[i], "--verbose") == 0) {
            if (i + 1 < argc) {
                config.verbose = parse_bool(argv[++i]);
            } else {
                std::cerr << "Error: --verbose should be either 0/1 or true/false" << std::endl;
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
            std::cout << "  --search-field <field>   Couchbase search field name\n\n";
            std::cout << "Replicate Options:\n";
            std::cout << "  --api-key <key>          Replicate API key\n";
            std::cout << "  --version <version>      Replicate model version\n\n";
            std::cout << "File Options:\n";
            std::cout << "  --csv_filepath <path>        Path to CSV file\n\n";
            std::cout << "  --img_link <url>                Public URL to img\n\n";
            std::cout << "  --verbose <bool>             Boolean value (0/false or 1/true)\n\n";
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

    bool has_local_reqs = !config.csv_filepath.empty() && !config.img_link.empty();

    // unusable if no access to idm-vton
    if (!has_replicate){
        return FunctionalityAvailability::NO_REPLICATE_KEY;
    }

    // if everything available, all gud
    if (has_couchbase && has_replicate && has_local_reqs){
        return FunctionalityAvailability::ALL;
    } else if (has_couchbase && has_replicate){ // if couchbase and replicate
        return FunctionalityAvailability::COUCHBASE;
    } else if (has_local_reqs && has_replicate){ // if local nad replicate
        return FunctionalityAvailability::LOCAL;    
    } else {
        return FunctionalityAvailability::NEEDS_CONFIG;
    }
}

// convert query to embedding
std::vector<double> fetch_embedding_from_query(std::string& query, bool verbose){
    if (!ollama::is_running()){
        throw std::runtime_error("Ollama service is not running. Please start Ollama before using this functionality.");
    }

    if (verbose){
        ollama::show_requests(true);
        ollama::show_replies(true);
    }

    ollama::response response = ollama::generate_embeddings("nomic-embed-text:latest", query);
    nlohmann::json data = response.as_json();

    if (verbose){
        std::cout << data["embeddings"].type_name(); // is array
        std::cout << "Embeddings: " << data["embeddings"][0] << std::endl;
    }

    std::vector<double> vec = data["embeddings"][0];
    // https://stackoverflow.com/questions/43907601/how-to-copy-data-from-c-array-to-eigen-matrix-or-vector
    // Eigen::RowVectorXd eigen = Eigen::Map<Eigen::RowVectorXd>(vec.data(), vec.size());

    return vec;
}

// search locally using provided .CSV
auto local_search(std::string& query, bool verbose=false){
    // convert query to embedding
    std::vector<double> query_vec = fetch_embedding_from_query(query, verbose);
    // get datset
    auto dataset = csv::parse_csv_with_scores(config.csv_filepath, query_vec);

    return csv::get_top_k(dataset);
}

// couchbase vector search
std::string couchbase_vector_searcher(std::string& query, int k=5, bool verbose=false){
    std::vector<double> query_vec = fetch_embedding_from_query(query, verbose);

    CouchbaseVectorSearch couchbase(config.user, 
        config.pass, config.hostname, config.port,
        config.bucket, config.scope, config.search_index, 
        query_vec);

    std::string res = couchbase.vector_search(config.search_field, k);
    if (verbose){
        std::cout << "Received from server:\n" << res << std::endl;
    }

    return res;
}

// inference using replicate
std::string replicate_inference(std::string& garm_img_link, std::string& garm_des, bool verbose){
    ri::ReplicateInference styler(config.version);

    styler.add_input("garm_img", garm_img_link);
    styler.add_input("human_img", config.img_link);
    styler.add_input("garment_des", garm_des);

    std::string res = styler.perform_inference(config.api_key);
    return res;
}

void open_browser(std::string& link){
    browser::openURL(link);
}

mcp::json local_search_handler(const mcp::json& params, const std::string& session_id){
    std::string query = params["query"].get<std::string>();
    int k = params["k"].get<int>();
    
    std::cout << "Session ID: " << session_id << " Received query: " << query << std::endl;
    std::cout << "Session ID: " << session_id << " Received k: " << k << std::endl;
    
    auto results = local_search(query, config.verbose);
    
    return csv::dataset_to_json(results);
}

mcp::json couchbase_search_handler(const mcp::json& params, const std::string& session_id){
    std::string query = params["query"].get<std::string>();
    int k = params["k"].get<int>();
    
    std::cout << "Session ID: " << session_id << " Received query: " << query << std::endl;
    std::cout << "Session ID: " << session_id << " Received k: " << k << std::endl;
    
    auto results = couchbase_vector_searcher(query, k, config.verbose);
    
    return results;
}

mcp::json replicate_handler(const mcp::json& params, const std::string& session_id){
    std::cout << "Session ID: " << session_id << "\t Starting Replicate Inference..." << std::endl;
    std::string garm_img = params["garm_img"].get<std::string>();
    // std::string human_img = config.img_link;
    std::string garment_des = params["garment_des"].get<std::string>();
    
    std::cout << "Received data\n" << "Garment img: " << garm_img << "\nHuman img: " << config.img_link << "\nGarment des: " << garment_des;

    std::string res = replicate_inference(garm_img, garment_des, config.verbose);
    
    // open output in browser
    open_browser(res);

    return res;
}

int main(int argc, char* argv[]){
    // parse config
    config = parse_config(argc, argv);

    // check what's available and make corresponding tools available
    FunctionalityAvailability check = eval_availability(config);
    if (check == FunctionalityAvailability::NEEDS_CONFIG){
        // throw unintialize config error
        throw std::runtime_error("\nMissing config. Please add either Local or Couchbase config to continue.");

    } else if (check == FunctionalityAvailability::NO_REPLICATE_KEY){
        // cannot run server if replicate key is missing
        throw std::runtime_error("\nApp is unusable with missing Replicate key for idm-vton. If you are able to run this heavy model locally, let me know or contribute for local inference. :-)");
    }

    mcp::server server("localhost", 8888);
    server.set_server_info("MCP OpenVTO in C++", "0.0.1");

    mcp::json capabilities = {
        {"tools", mcp::json::object()} // add tools here
    };
    server.set_capabilities(capabilities);

    mcp::tool couchbase_search = mcp::tool_builder("couchbase_search")
    .with_description("Only to be performed if the user asks to perform a search on the Cloud/Couchbase. Performs a vector search on Couchbase for a given query to find the most suitable clothes. Your job is to return the results in a readable format so the user can select which clothes to perform Virtual Try-On on.")
    .with_string_param("query", "The refined query of the user. If it's something like Blue Jeans, ask the user for more detail and refine the query so that a more richer embedding can be used to perform a semantic search.", true)
    .with_number_param("k", "The top-k results to fetch from semantic search (default: 5).", true)
    .build();
    
    mcp::tool local_search = mcp::tool_builder("local_search")
    .with_description("This is the default search tool to search for relevant clothes from a database of CSV file. Performs a vector search over a CSV files for a given query to find the most suitable clothes. Your job is to return the results in a readable format so the user can select which clothes to perform Virtual Try-On on.")
    .with_string_param("query", "The refined query of the user. If it's something like Blue Jeans, ask the user for more detail and refine the query so that a more richer embedding can be used to perform a semantic search.", true)
    .with_number_param("k", "The top-k results to fetch from semantic search (default: 5).", true)
    .build();

    mcp::tool perform_vton = mcp::tool_builder("perform_vton")
    .with_description("Perform Virtual Try-On using IDM-VTON Deep Learning model. This tool is only to be called once the user has selected a garment/item to Virtual Try-On. If the user asks to call this directly without selecting a garment, kindly reject the request asking them to use either `local_search` or `couchbase_search`.")
    .with_string_param("garm_img", "The image link of the selected garment from `local_search` or `couchbase_search`", true)
    .with_string_param("garment_des", "Description of garment e.g. Short Sleeve Round Neck T-shirt from the `local_search` or `couchbase_search` selection", true)
    .build();

    // tool registry
    if (check == FunctionalityAvailability::ALL){
        server.register_tool(local_search, local_search_handler);
        server.register_tool(couchbase_search, couchbase_search_handler);
        server.register_tool(perform_vton, replicate_handler);
    }
    
    if (check == FunctionalityAvailability::COUCHBASE){
        server.register_tool(couchbase_search, couchbase_search_handler);
        server.register_tool(perform_vton, replicate_handler);        
    }
    
    if (check == FunctionalityAvailability::LOCAL){
        server.register_tool(local_search, local_search_handler);
        server.register_tool(perform_vton, replicate_handler);
    }

    // Start server
    // std::cout << "Starting MCP server at localhost:8888..." << std::endl;
    // std::cout << "Press Ctrl+C to stop the server" << std::endl;
    server.start(true);  // Blocking mode

    return 0;
}
