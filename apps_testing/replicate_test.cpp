/**
* @file replicate_test.cpp
* @brief Testing replicate utility
* @author Nikhil Kapila
* @date 2025-06-26 13:51:07 Thursday
*/

// curl request from docs, https://replicate.com/cuuupid/idm-vton/api
// curl --silent --show-error https://api.replicate.com/v1/predictions \ --request POST \ --header "Authorization: Bearer $REPLICATE_API_TOKEN" \ --header "Content-Type: application/json" \ --header "Prefer: wait" \ --data @- <<-EOM { "version": "0513734a452173b8173e907e3a59d19a36266e55b48528559432bd21c7d7e985", "input": { "garm_img": "https://replicate.delivery/pbxt/KgwTlZyFx5aUU3gc5gMiKuD5nNPTgliMlLUWx160G4z99YjO/sweater.webp", "human_img": "https://replicate.delivery/pbxt/KgwTlhCMvDagRrcVzZJbuozNJ8esPqiNAIJS3eMgHrYuHmW4/KakaoTalk_Photo_2024-04-04-21-44-45.png", "garment_des": "cute pink top" } } EOM

#include <utils/replicate_inference.h>
#include <string>
#include <iostream>


int main() {
    std::string ver = "0513734a452173b8173e907e3a59d19a36266e55b48528559432bd21c7d7e985";

    std::string garm_img = "https://replicate.delivery/pbxt/KgwTlZyFx5aUU3gc5gMiKuD5nNPTgliMlLUWx160G4z99YjO/sweater.webp";

    std::string human_img = "https://replicate.delivery/pbxt/KgwTlhCMvDagRrcVzZJbuozNJ8esPqiNAIJS3eMgHrYuHmW4/KakaoTalk_Photo_2024-04-04-21-44-45.png";

    std::string garment_des = "cute pink top";

    ri::ReplicateInference styler(ver);

    styler.add_input("garm_img", garm_img);
    styler.add_input("human_img", human_img);
    styler.add_input("garment_des", garment_des);

    std::string res = styler.perform_inference("apikey");

    std::cout << res;

    return 0;
}