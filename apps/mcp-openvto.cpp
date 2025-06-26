/**
 * @file mcp_kapila_style.cpp
 * @brief MCP Server for Couchbase and Replicate APIs
 * @author Nikhil Kapila
 * @date 2025-06-22 00:03:35 Sunday
 * @version 1.0.0
 *
 * This example demonstrates how to create an MCP server, register tools and resources,
 * and handle client requests. Follows the 2024-11-05 basic protocol specification.
 *
 * Tools provided:
 * TBA
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



// perform ollama search using ollama.hpp
ollama::response ollama_embedder(const std::string& text){
    return ollama::generate_embeddings("nomic-embed-text", text);
}

// couchbase vector search
mcp::json couchbase_vector_searcher(const mcp::json& params){

}

// inference using replicate
mcp::json replicate_inference(const mcp::json &params){

}

// TODO: need to add image resource

int main () {
    mcp::server server("localhost", 8888);
    server.set_server_info("MCP Couchbase Capella Dresser", "0.0.1");

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
