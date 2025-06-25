/**
* @file couchbase_search.h
* @brief Header file to send POST requests to Couchbase for Vector Search
* @author Nikhil Kapila
* @date 2025-06-24 21:31:01 Tuesday
*/

#include <iostream>
#include "httplib.h"
#include "json.hpp"

class CouchbaseVectorSearch{
private:
    std::string username;
    std::string password;
    std::string hostname;
    int port;

    std::string bucket_name;
    std::string scope_name;
    std::string idx_name;
    std::vector<double> query;

public:
    CouchbaseVectorSearch(const std::string& usr, const std::string& pass, 
        const std::string& host, int prt, const std::string& bucket_name,
        const std::string& scope_name, const std::string& idx_name, const std::vector<double>& query);
        
    std::string performVectorSearch(const std::string& field, int k);
};

