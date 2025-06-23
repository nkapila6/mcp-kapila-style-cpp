/**
* @file ollama_embedder.cpp
* @brief testing ollama resp
* @author Nikhil Kapila
* @date 2025-06-23 18:27:25 Monday
*/

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
    ollama::show_replies(true);

    ollama::response response = ollama::generate_embeddings("nomic-embed-text:latest", "Why is the sky blue?");

    nlohmann::json data = response.as_json();
    std::cout << "Embeddings: " << data["embeddings"] << std::endl;

    std::cout << data["embeddings"].type_name();

    return 0;
}