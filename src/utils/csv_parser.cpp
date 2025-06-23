/**
* @file csv_parser.cpp
* @brief Custom CSV Parser Utility generalized out of apps_testing/loading.cpp
* @author Nikhil Kapila
* @date 2025-06-23 23:03:06 Monday
*/

#include "utils/csv_parser.h"
#include <algorithm>
#include <Eigen/Dense>
#include <iostream>
#include <fstream>

namespace csv{

    std::vector<CSVRow> parse_csv_with_scores(const std::string& filepath, const std::vector<double>& query_vector){
        std::vector<CSVRow> dataset;
        std::ifstream file(filepath);
        std::string line;

        // TODO: add Error checking
        // skipping header file
        std::getline(file, line);

        int row_num = 0;
        while(std::getline(file,line)) {
            auto row = line_parser(line);
            
            int id = std::stoi(row[2]);
            std::vector<double> vec = parse_str_to_vector(row[5]);
            Eigen::RowVectorXd evec1 = Eigen::Map<Eigen::RowVectorXd>(vec.data(), vec.size());
            Eigen::Map<const Eigen::VectorXd> evec2(query_vector.data(), query_vector.size());
            // Eigen::RowVectorXd evec2 = Eigen::Map<Eigen::RowVectorXd>(query_vector.data(), query_vector.size());
            double scr = evec1.dot(evec2);

            dataset.emplace_back(row[0], row[1], id, row[3], row[4], vec, scr);
            row_num++;
        }
        file.close();

        return dataset;
    }

    std::vector<CSVRow> parse_csv(const std::string& filepath){
        std::vector<CSVRow> dataset;
        std::ifstream file(filepath);
        std::string line;
        
        // TODO: add Error checking
        // skipping header file
        std::getline(file, line);

        int row_num = 0;
        while(std::getline(file,line)) {
            auto row = line_parser(line);
            int id = std::stoi(row[2]);
            std::vector<double> vec = parse_str_to_vector(row[5]);
            dataset.emplace_back(row[0], row[1], id, row[3], row[4], vec);
            row_num++;
        }
        file.close();

        return dataset;
    }

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

    std::vector<CSVRow> get_top_k(std::vector<CSVRow>& dataset, int k){
        // sorting the dataset
        std::sort(dataset.begin(), dataset.end(),
            [](const CSVRow& a, const CSVRow& b){
                return a.score>b.score;
            });
        
        return std::vector<CSVRow>(dataset.begin(), dataset.begin()+k);
   }

}