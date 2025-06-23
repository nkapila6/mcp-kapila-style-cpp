/**
* @file csv_parser.h
* @brief Custom CSV Parser Utility generalized out of apps_testing/loading.cpp
* @author Nikhil Kapila
* @date 2025-06-23 17:04:26 Monday
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

namespace csv {

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

}