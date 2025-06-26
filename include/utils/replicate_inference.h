/**
* @file replicate_inference.h
* @brief Replicate inference utility
* @author Nikhil Kapila
* @date 2025-06-25 14:35:34 Wednesday
*/

// curl request from docs, https://replicate.com/cuuupid/idm-vton/api
// curl --silent --show-error https://api.replicate.com/v1/predictions \ --request POST \ --header "Authorization: Bearer $REPLICATE_API_TOKEN" \ --header "Content-Type: application/json" \ --header "Prefer: wait" \ --data @- <<-EOM { "version": "0513734a452173b8173e907e3a59d19a36266e55b48528559432bd21c7d7e985", "input": { "garm_img": "https://replicate.delivery/pbxt/KgwTlZyFx5aUU3gc5gMiKuD5nNPTgliMlLUWx160G4z99YjO/sweater.webp", "human_img": "https://replicate.delivery/pbxt/KgwTlhCMvDagRrcVzZJbuozNJ8esPqiNAIJS3eMgHrYuHmW4/KakaoTalk_Photo_2024-04-04-21-44-45.png", "garment_des": "cute pink top" } } EOM

// making this generalizable/modular: https://en.cppreference.com/w/cpp/container/unordered_map.html

#include <variant>
#include <unordered_map>
#include "json.hpp"

namespace ri {
    // https://www.geeksforgeeks.org/cpp/std-variant-in-cpp-17/
    using inp = std::variant<std::string, int, double, bool, float, nlohmann::json>;
    using inpmap = std::unordered_map<std::string, inp>;

    class ReplicateInference{
    private:
        std::string version;
        inpmap inputs;

    public:
        // constructor
        ReplicateInference(const std::string& version);

        // overloading methods to add inputs
        ReplicateInference& add_input(const std::string& key, const std::string& value);
        ReplicateInference& add_input(const std::string& key, int value);
        ReplicateInference& add_input(const std::string& key, double value);
        ReplicateInference& add_input(const std::string& key, bool value);
        ReplicateInference& add_input(const std::string& key, float value);
        
        // print inputs -- for debugging
        void print_inputs();

        // send request to replicate
        std::string perform_inference(const std::string& api_key);
    };

}
