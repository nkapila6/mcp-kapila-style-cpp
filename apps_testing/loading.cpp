/**
* @file loading.cpp
* @brief testing csv/jsonml loading
* @author Nikhil Kapila
* @date 2025-06-23 15:18:47 Monday
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <Eigen/Dense>

// overloading vector of smth to allow cout stream print
// https://stackoverflow.com/questions/10750057/how-do-i-print-out-the-contents-of-a-vector/23397700#23397700
template<typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
    out << "{";
    size_t last = v.size() - 1;
    for(size_t i = 0; i < v.size(); ++i) {
        out << v[i];
        if (i != last) 
            out << ", ";
    }
    out << "}";
    return out;
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

int main() {
    std::ifstream file("../data-prep/full_data_3k.csv");
    std::string line;
    
    std::getline(file, line);
    std::cout << "Header: " << line << std::endl;

    std::vector<std::string> row;
    
    // getting first row only
    while(std::getline(file, line)){
        row = line_parser(line);
        break;
    }

    std::cout << "\nFilename: " << row[0] << std::endl;
    std::cout << "Link: " << row[1] << std::endl;
    std::cout << "ID: " << row[2] << std::endl;
    std::cout << "Desc: " << row[3] << std::endl;
    std::cout << "Vector: " << row[5] << std::endl;
}
