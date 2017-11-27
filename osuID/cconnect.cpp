#include "curl\curl.h"
#include "cconnect.h"
#include <iostream>
#include <string>

cconnect::cconnect() {}
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *stream);

std::string cconnect::connector(std::string url) {

	CURL *curl = curl_easy_init();

	// cURL result
	std::string requestData;

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:55.0) Gecko/20100101 Firefox/55.0");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &requestData);
		curl_easy_perform(curl);
	}

	curl_easy_cleanup(curl);

	return requestData;
}

// Result to string
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *stream) {
	((std::string*)stream)->append((char*)contents, size * nmemb);
	return size * nmemb;
}