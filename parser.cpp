#include <iostream>
#include <string>
#include <curl/curl.h>

// Функция-клиент для cURL: принимает куски данных из сети и записывает их в строку
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* responseBuffer = static_cast<std::string*>(userp);
    
    // Дописываем полученные байты в конец нашей строки
    responseBuffer->append(static_cast<char*>(contents), totalSize);
    
    return totalSize;
}

int main() {
    // Инициализация cURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[ERROR] Не удалось инициализировать cURL." << std::endl;
        return 1;
    }

    // Публичный API адреса Bitcoin (в формате JSON)
    std::string url = "https://blockchain.info/rawaddr/1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa";
    std::string rawResponse;

    // Настройка параметров cURL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rawResponse);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");

    std::cout << "[INFO] Отправка запроса к сети..." << std::endl;

    // Выполнение сетевого запроса
    CURLcode result = curl_easy_perform(curl);

    // Проверка на ошибки сети
    if (result != CURLE_OK) {
        std::cerr << "[ERROR] Запрос завершился ошибкой: " 
                  << curl_easy_strerror(result) << std::endl;
    } else {
        std::cout << "[SUCCESS] Данные успешно получены!" << std::endl;
        std::cout << "\nСырой ответ сервера (JSON):\n" << rawResponse.substr(0, 300) << "...\n" << std::endl;
    }

    // Освобождаем ресурсы
    curl_easy_cleanup(curl);

    return 0;
}