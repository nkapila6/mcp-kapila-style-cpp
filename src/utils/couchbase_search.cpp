/**
* @file filename.cpp
* @brief Brief description of what this file does
* @author Nikhil Kapila
* @date 2025-06-24 16:59:36 Tuesday
*/

#include <iostream>
#include "httplib.h"
#include "json.hpp"


// from https://docs.couchbase.com/server/current/vector-search/run-vector-search-rest-api.html
// To run a Vector search with the REST API:
// In your command-line tool, enter a curl command with the XPOST verb.
// Set your header content to include 
// "Content-Type: application/json".
// Enter your username, password, and the 
// Search Service endpoint on port 8094 with the 
// name of the Vector Search index you want to query.

class CouchbaseVectorSearch{
private:
    std::string username;
    std::string password;
    std::string hostname;
    int port;

    std::string bucket_name;
    std::string scope_name;
    std::string idx_name;
    std::vector<double> query;

public:
    CouchbaseVectorSearch(const std::string& usr, const std::string& pass, 
        const std::string& host, int prt=18094, const std::string& bucket_name,
        const std::string& scope_name, const std::string& idx_name, const std::vector<double>& query)
        : username(usr), password(pass), hostname(host), port(prt), bucket_name(bucket_name), 
        scope_name(scope_name), idx_name(idx_name), query(query) {
        }

    std::string performVectorSearch(int k=3, const std::string& field) { // field to search on Couchbase DB
        std::string base_url = (port == 18094 ? "https://" : "http://") + hostname + ":" + std::to_string(port);

        
        httplib::Client client(base_url);
        client.set_basic_auth(username, password);
        client.set_connection_timeout(30, 0);
        client.set_read_timeout(60, 0);
        client.set_write_timeout(60, 0);
        
        // curl -s -XPUT -H "Content-Type: application/json" \
        // -u ${CB_USERNAME}:${CB_PASSWORD} http://${CB_HOSTNAME}:8094/api/bucket/${BUCKET_NAME}/scope/${SCOPE_NAME}/index/${INDEX_NAME}/query -d
        std::string path = "/api/bucket/" + bucket_name + "/scopre/" + scope_name + "/index/" + idx_name + "/query";

        httplib::Headers header = {
            {"Content-Type", "application/json"}
        };

        // WITH JSON PAYLOAD
        // curl -XPOST -H "Content-Type: application/json" \
        // -u ${CB_USERNAME}:${CB_PASSWORD} http://${CB_HOSTNAME}:8094/api/bucket/vector-sample/scope/color/index/color-index/query \
        // -d '{
        //     "fields": ["*"], 
        //     "query": { 
        //         "min": 70, 
        //         "max": 80,  
        //         "inclusive_min": false,  
        //         "inclusive_max": true,  
        //         "field": "brightness" 
        //     }, 
        //     "knn": [
        //         {
        //         "k": 3, 
        //         "field": "colorvect_l2", 
        //         "vector": [ 176, 0, 176 ]
        //         }
        //     ]
        //     }'

        // https://json.nlohmann.me/api/json/#examples
        nlohmann::json payload = {
            {"fields", {"*"}},
            {"knn", {
                {
                    {"k", k},
                    {"field", field},
                    {"vector", query}
                }
            }}
        };
        
        // https://json.nlohmann.me/api/basic_json/dump/#examples
        std::string json_payload = payload.dump();

        auto result = client.Post(path, header, json_payload, "application/json");
        if (!result) {
            return "Error: Failed to connect to server";
        }
        
        if (result->status != 200) {
            return "Error: HTTP " + std::to_string(result->status) + " - " + result->body;
        }

        return result->body;
    }
};

