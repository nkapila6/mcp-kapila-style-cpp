/**
* @file replicate_inference.cpp
* @brief Replicate inference utility
* @author Nikhil Kapila
* @date 2025-06-26 13:20:33 Thursday
*/

#include "utils/replicate_inference.h"
#include <iostream>
#include "httplib.h"
#include "json.hpp"

// curl request from docs, https://replicate.com/cuuupid/idm-vton/api
// curl --silent --show-error https://api.replicate.com/v1/predictions \ --request POST \ --header "Authorization: Bearer $REPLICATE_API_TOKEN" \ --header "Content-Type: application/json" \ --header "Prefer: wait" \ --data @- <<-EOM { "version": "0513734a452173b8173e907e3a59d19a36266e55b48528559432bd21c7d7e985", "input": { "garm_img": "https://replicate.delivery/pbxt/KgwTlZyFx5aUU3gc5gMiKuD5nNPTgliMlLUWx160G4z99YjO/sweater.webp", "human_img": "https://replicate.delivery/pbxt/KgwTlhCMvDagRrcVzZJbuozNJ8esPqiNAIJS3eMgHrYuHmW4/KakaoTalk_Photo_2024-04-04-21-44-45.png", "garment_des": "cute pink top" } } EOM

namespace ri{
    // constructor
    ReplicateInference::ReplicateInference(const std::string& version): 
    version(version) {
    }

    // all em overloaded methods
    ReplicateInference& ReplicateInference::add_input(const std::string& key, const std::string& value){
        inputs[key] = value;
        return *this;
    }

    ReplicateInference& ReplicateInference::add_input(const std::string& key, int value){
        inputs[key] = value;
        return *this;
    }

    ReplicateInference& ReplicateInference::add_input(const std::string& key, double value){
        inputs[key] = value;
        return *this;
    }

    ReplicateInference& ReplicateInference::add_input(const std::string& key, bool value){
        inputs[key] = value;
        return *this;
    }

    ReplicateInference& ReplicateInference::add_input(const std::string& key, float value){
        inputs[key] = value;
        return *this;
    }

    void ReplicateInference::print_inputs(){
        for (const auto& [key, value] : inputs){          
            std::cout << key << " = ";
            // lambda function for value, handling thru auto
            std::visit([](const auto& v) {
                std::cout << v;
            }, value);
            std::cout << std::endl;
        }
    }

    // curl request from docs, https://replicate.com/cuuupid/idm-vton/api
    // curl --silent --show-error https://api.replicate.com/v1/predictions \ --request POST \ --header "Authorization: Bearer $REPLICATE_API_TOKEN" \ --header "Content-Type: application/json" \ --header "Prefer: wait" \ --data @- <<-EOM { "version": "0513734a452173b8173e907e3a59d19a36266e55b48528559432bd21c7d7e985", "input": { "garm_img": "https://replicate.delivery/pbxt/KgwTlZyFx5aUU3gc5gMiKuD5nNPTgliMlLUWx160G4z99YjO/sweater.webp", "human_img": "https://replicate.delivery/pbxt/KgwTlhCMvDagRrcVzZJbuozNJ8esPqiNAIJS3eMgHrYuHmW4/KakaoTalk_Photo_2024-04-04-21-44-45.png", "garment_des": "cute pink top" } } EOM
    std::string ReplicateInference::perform_inference(const std::string& api_key){
        std::string base_url = "https://api.replicate.com";
        std::string path = "/v1/predictions";
        
        httplib::Client client(base_url);
        client.set_connection_timeout(30, 0);
        client.set_read_timeout(60, 0 );
        
        // build json from input map
        nlohmann::json inputs_json;
        for (const auto& [k, v] : inputs) {
            inputs_json[k] = std::visit([](const auto& value) -> nlohmann::json {
                return nlohmann::json(value);
            }, v);
        }

        httplib::Headers headers = {
        {"Authorization", "Bearer " + api_key},
        {"Content-Type", "application/json"},
        {"Prefer", "wait"}
        };

        nlohmann::json payload = {
            {"version", version},
            {"input", inputs_json}
        };

        std::string json_payload = payload.dump();
        // std::cout << "Generated JSON: " << json_payload << std::endl;

        auto result = client.Post(path, headers, json_payload, "application/json");

        if (!result) {
        return "Error: Failed to connect to server";
        }
    
        if (result->status != 200) {
            return "Error: HTTP " + std::to_string(result->status) + " - " + result->body;
        }

        std::string jsonresp = result->body;
        std::cout << "Returned response:\n" << jsonresp;

        nlohmann::json res = nlohmann::json::parse(jsonresp);

        if (res.contains("status")){
            if (res["status"] == "succeeded"){
                return res["output"];
            }
        } else {
            return "Parsing failed. Returning raw response.\n" + jsonresp;
        }
    }
}