#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* responseBuffer = static_cast<std::string*>(userp);
    responseBuffer->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[ERROR] Не удалось инициализировать cURL." << std::endl;
        return 1;
    }

    std::string walletAddress = "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa";
    std::string url = "https://blockchain.info/rawaddr/" + walletAddress;
    std::string rawResponse;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rawResponse);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");

    std::cout << "[INFO] Отправка запроса к сети..." << std::endl;

    CURLcode result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        std::cerr << "[ERROR] Запрос завершился ошибкой: " 
                  << curl_easy_strerror(result) << std::endl;
    } else {
        std::cout << "[SUCCESS] Данные успешно получены!\n" << std::endl;

        try {
            json data = json::parse(rawResponse);

            long long satoshis = data["final_balance"].get<long long>();
            int txCount = data["n_tx"].get<int>();
            
            double btcBalance = static_cast<double>(satoshis) / 100000000.0;

            std::cout << "=== ИНФОРМАЦИЯ О КОШЕЛЬКЕ ===" << std::endl;
            std::cout << "Адрес:            " << walletAddress << std::endl;
            std::cout << "Всего транзакций: " << txCount << std::endl;
            std::cout << "Баланс (Сатоши):  " << satoshis << " SAT" << std::endl;
            std::cout << "Баланс (BTC):     " << btcBalance << " BTC" << std::endl;
            std::cout << "==============================" << std::endl;

        } catch (const json::exception& e) {
            std::cerr << "[ERROR] Ошибка парсинга JSON: " << e.what() << std::endl;
        }
    }

    curl_easy_cleanup(curl);
    return 0;
}

