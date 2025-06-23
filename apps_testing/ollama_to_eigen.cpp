/**
* @file ollama_to_eigen.cpp
* @brief Testing Ollama Dot product
* @author Nikhil Kapila
* @date 2025-06-23 18:45:40 Monday
*/

#include <Eigen/Dense>
#include <iostream>
#include "ollama.hpp" // for ollama

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

int main() {

    std::cout << ollama::is_running() << std::endl;

    ollama::show_requests(true);
    // ollama::show_replies(true);

    ollama::response response = ollama::generate_embeddings("nomic-embed-text:latest", "Why is the sky blue?");

    nlohmann::json data = response.as_json();
    std::cout << data["embeddings"].type_name(); // is array
    std::cout << "Embeddings: " << data["embeddings"][0] << std::endl;

    std::vector<double> vec1 = data["embeddings"][0];
    // std::cout << vec1;

    // https://stackoverflow.com/questions/43907601/how-to-copy-data-from-c-array-to-eigen-matrix-or-vector
    Eigen::RowVectorXd eigen_vec1 = Eigen::Map<Eigen::RowVectorXd>(vec1.data(), vec1.size());
    // std::cout << eigen_vec1;

    // reinit the same var
    Eigen::RowVectorXd eigen_vec2 = Eigen::Map<Eigen::RowVectorXd>(vec1.data(), vec1.size());

    // https://eigen.tuxfamily.org/dox/group__TutorialMatrixArithmetic.html#title6
    std::cout << "Dot product: " << eigen_vec1.dot(eigen_vec2) << std::endl;

    return 0;
}