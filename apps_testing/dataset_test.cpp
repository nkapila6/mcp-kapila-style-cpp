/**
* @file dataset_test.cpp
* @brief checking dataset parser
* @author Nikhil Kapila
* @date 2025-06-24 00:03:10 Tuesday
*/

#include "ollama.hpp"
#include <iostream>
#include <Eigen/Dense>
#include "utils/csv_parser.h"

int main(){
    std::string query = "Blue Male TShirt!";
    ollama::response response = ollama::generate_embeddings("nomic-embed-text:latest", "Blue Male TShirt!");
    nlohmann::json data = response.as_json();
    std::vector<double> query_vec = data["embeddings"][0];

    auto dataset = csv::parse_csv_with_scores("../data-prep/full_data_3k.csv", query_vec);

    std::cout << "Dataset contains " << dataset.size() << " rows:" << std::endl;

    // for (const auto& row : dataset) {
    //     std::cout << "ID: " << row.id 
    //             << ", File: " << row.fname 
    //             << ", Score: " << row.score
    //             << ", Desc: " << row.desc 
    //             << std::endl;
    // }
    
    std::cout << "The query is " << query << std::endl;

    auto top_data = csv::get_top_k(dataset);
    for (const auto& row : top_data) {
        std::cout << "ID: " << row.id 
                << ", File: " << row.fname 
                << ", Score: " << row.score << std::endl
                << ", Desc: " << row.desc << std::endl
                << "Link: " << row.link << std::endl
                << std::endl;
    }

    return 0;
}

