#include <string>

void openURL(const std::string& url) {
#ifdef _WIN32
    // Windows
    std::string command = "start " + url;
    system(command.c_str());
#elif __APPLE__
    // macOS
    std::string command = "open " + url;
    system(command.c_str());
#elif __linux__
    // Linux
    std::string command = "xdg-open " + url;
    system(command.c_str());
#else
    std::cout << "Platform not supported" << std::endl;
#endif
}

int main() {
    openURL("https://replicate.delivery/pbxt/KgwTlhCMvDagRrcVzZJbuozNJ8esPqiNAIJS3eMgHrYuHmW4/KakaoTalk_Photo_2024-04-04-21-44-45.png");
    return 0;
}