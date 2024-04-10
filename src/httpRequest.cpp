#include "httpRequest.hpp"

const char* HttpRequest::defaultUserAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36";

HttpRequest::HttpRequest() : curl(curl_easy_init()) {
    if (!curl) {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }
}

HttpRequest::~HttpRequest() {
    if (curl) {
        curl_easy_cleanup(curl);
    }
}

std::string HttpRequest::get(const std::string& url, const std::string& userAgent) {
    std::string response_data;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.empty() ? defaultUserAgent : userAgent.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
    }
    // ce morceau de code aurait mérité quelques explications

    return response_data;
}

size_t HttpRequest::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    // Les pointeurs void* peuvent être convertis implicitement en n’importe quel autre type de pointeur.
    // être explicite est souvent la meilleureapproche, attention à ne pas donner l'impression qu'une opération
    // spéciale est en cours alors qu'une conversion implicite pourrait suffire (cela dépendra des contextes)
    return total_size;
}