/**
* @file csv_parser.h
* @brief Custom CSV Parser Utility generalized out of apps_testing/loading.cpp
* @author Nikhil Kapila
* @date 2025-06-23 17:04:26 Monday
*/

#include <iostream>

namespace csv {

    struct CSVRow{
        // filename, link, id, desc, embedding_model, vector
        std::string fname;
        std::string link;
        int id;
        std::string desc;
        std::string embedding_model;
        std::vector<double> vector;
        double score;


        // Struct constructor
        CSVRow(const std::string& file, const std::string& link, int id,
            const std::string& desc, const std::string& model,
            const std::vector<double>& vec, double scr=0.0):
            fname(file), link(link), id(id), desc(desc), embedding_model(model),
            vector(vec), score(scr) {}
    };

    /**
    * @brief Parse full CSV and add scoring
    * @param filepath The string input to the path of the file
    * @param query_vector Query vector for the input query from ollama
    * @return Full dataset with scores
    */
    std::vector<CSVRow> parse_csv_with_scores(const std::string& filepath, const std::vector<double>& query_vector);

    /**
    * @brief Parses full CSV
    * @param filepath The string input to the path of the file
    * @return The full dataset
    */
    std::vector<CSVRow> parse_csv(const std::string& filepath);

    /**
    * @brief Reads a line in a CSV
    * @param line The string input
    * @return A vector of strings
    */
   std::vector<std::string> line_parser(const std::string& line);

   /**
   * @brief Converts string of numbers into vector of doubles
   * @param vecstr The string input
   * @return A vector of doubles
   */
   std::vector<double> parse_str_to_vector(const std::string& vec_str);

   /**
   * @brief Converts a row to a string
   * @param row a row in the dataset
   * @return returns a string with all the row info for the LLM
   */
   std::string row_to_string(const CSVRow& row);

    /**
    * @brief Converts dataset to a json string
    * @param dataset the incoming dataset (most likely going to be a subset from the get_top_k func)
    * @return returns a json string
    */
    std::string dataset_to_json(const std::vector<CSVRow>& dataset);

   /**
   * @brief Gets top-k sorted results
   * @param dataset loaded full dataset
   * @param k number of results to return
   * @return returned results
   */
   std::vector<CSVRow> get_top_k(std::vector<CSVRow>& dataset, int k=5);

}