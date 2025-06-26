/**
* @file couchbase_test.cpp
* @brief Testing REST API request construction
* @author Nikhil Kapila
* @date 2025-06-24 21:36:07 Tuesday
*/

#include "utils/couchbase_search.h"
#include "ollama.hpp" 

// curl -X POST \
//   -H "Content-Type: application/json" \
//   -u "aitinkerers:AI-Tinkerers@Event11" \
//   "https://svc-dqis-node-001.s5tkr5bxtbeozql.cloud.couchbase.com:18094/api/bucket/clothes/scope/clothes/index/search_clothes/query" \
//   -d '{"fields":["*"],"knn":[{"field":"embedding_vector","k":3,"vector":[0.25,0.5]}]}'

int main (){
    // random data
    std::string hostname = "*.cloud.couchbase.com";
    std::string search = "search_index";

    // ollama::show_requests(true);
    // ollama::show_replies(true);

    ollama::response response = ollama::generate_embeddings("nomic-embed-text:latest", "Blue Jeans Men!");
    nlohmann::json data = response.as_json();
    // std::cout<<data["embeddings"];
    std::vector<double> vector = data["embeddings"][0];

    // usr, pass, host, port, bucket_name, scope_name, idx_name, vector
    CouchbaseVectorSearch request("user", "ayo", hostname, 18094,
    "bucket", "scope", search, vector); //{0.25, 0.5});

    std::string res = request.vector_search("vector", 3);
    std::cout << res;


    return 0;
}