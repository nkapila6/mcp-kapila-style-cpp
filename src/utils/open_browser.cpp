/**
* @file open_browser.cpp
* @brief Opening the browser
* @author Nikhil Kapila
* @date 2025-06-26 18:46:09 Thursday
*/

#include "utils/open_browser.h"
#include <string>

namespace browser {
    void openURL(const std::string& url){
        #ifdef _WIN32_
            std::string command = "start " + url;
            system(command.c_str());
        #elif __APPLE__
            std::string command = "open " + url;
            system(command.c_str());
        #elif __linux__
            std::string command = "xdg-open " + url;
            system(command.c_str());
        #endif

    }
}