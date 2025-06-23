/**
* @file csv_parser.cpp
* @brief Custom CSV Parser Utility generalized out of apps_testing/loading.cpp
* @author Nikhil Kapila
* @date 2025-06-23 23:03:06 Monday
*/

#include "csv_parser.h"

namespace csv{
    std::vector<std::string> line_parser(const std::string& line){
        std::vector<std::string> row;
        std::string temp; // temp placeholder within loop
        bool quotes = false;

        for (int i=0; i<line.length(); i++){
            char c = line[i];

            if (c=='"'){
                quotes = !quotes;
            } else if (c==',' && !quotes){
                // add everything before this into row
                row.push_back(temp); // https://cplusplus.com/reference/vector/vector/push_back/
                // clear the string
                temp.clear(); // https://cplusplus.com/reference/string/string/clear/
            } else {
                temp +=c;
            }
        }

        // add the last field
        row.push_back(temp);
        return row;
    }

    std::vector<double> parse_str_to_vector(const std::string& vec_str){
        std::vector<double> result;
        std::string vecstring = vec_str;

        // https://cplusplus.com/reference/algorithm/remove/
        // https://cplusplus.com/reference/string/string/erase/
        // very confusing function, remove just gives the range of elements to remove
        //  remove gives the first iterator, last iterator we already know.
        // erase is what actually removes it
        vecstring.erase(std::remove(vecstring.begin(), vecstring.end(), '['), vecstring.end());
        vecstring.erase(std::remove(vecstring.begin(), vecstring.end(), ']'), vecstring.end());
        std::replace(vecstring.begin(), vecstring.end(), ',', ' ');

        std::stringstream stream; // https://cplusplus.com/reference/sstream/stringstream/
        stream << vecstring; // writing to string stream
        double value; // string stream out to double

        while (stream >> value){
            result.push_back(value);
        }

        return result;
    }
}