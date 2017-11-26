#include "curl\curl.h"
#include "connect.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
connect::connect() {}

static size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *stream);

int connect::connector() {

	CURL *curl;
	file.open("profile.html", std::ios::out);
	rankingsFile.open("rankings.html", std::ios::out);

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:36.0) Gecko/20100101 Firefox/54.0");
		curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	
		// Write ranking pages to file
		for (int page = 1; page <= 3; page++) {
			std::stringstream URL;
			std::string base = "https://osu.ppy.sh/rankings/osu/performance?page=";
			URL << base << page;

			curl_easy_setopt(curl, CURLOPT_URL, URL.str().c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rankingsFile);
			curl_easy_perform(curl);
		}
		rankingsFile.close();
		rankingsFile.clear();

		// Parse file to prepare the ID array
		scrapeRankings();

		// Write profile pages to file
		for (int pid = 95; pid < 104; pid++) {
			std::stringstream profileURL;
			std::string pbase = "https://osu.ppy.sh/users/";
			profileURL << pbase << scrapedID[pid];

			curl_easy_setopt(curl, CURLOPT_URL, profileURL.str().c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
			curl_easy_perform(curl);
		}
		file.close();
		file.clear();
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	
	return 0;
}

// Scrape ranking pages for profile IDs and store into an array
int connect::scrapeRankings() {

	std::regex rRankingsID(R"(users\/(\d+))");
	std::smatch match;
	std::string line;
	std::string temp;

	int scrapedCounter = 0;

	rankingsFile.open("rankings.html", std::ios::in);

	while (std::getline(rankingsFile, line)) {

		if (regex_search(line, match, rRankingsID)) {
			temp = match.str(1);
			scrapedID[scrapedCounter] = std::stoi(temp);
			scrapedCounter++;
		}
	}
	// Write scraped IDs to file and print them
	if (scrapedCounter != 0) {
		for (int i = 0; i < 150; i++) {
			std::cout << "ID #" << i << ": " << scrapedID[i] << std::endl;
		}
	}

	rankingsFile.close();
	rankingsFile.clear();
	return 0;
}

// Parse HTML for player info
int connect::getProfileInfo() {

	std::regex rIdUserAgeStyle(R"(\"id\":(\d+),\"username\":\"([\w" "]+)\".+\"age\":(\w+),.+\"playstyle\":\[?([\w\",]+)\]?,)"); // Capture ID, Username, Age and Playstyle
	std::regex rRankPP(R"(\"pp\":(\d+).?\S+\"pp_rank\":(\d+))"); // Capture PP and Rank
	std::smatch match;
	std::string line;
	std::string temp;

	int userIndex = 0;

	//bool all values if true then inc counter

	bool fId, fUser, fAge, fStyle, fRank, fPp;
	fId = fUser = fAge = fStyle = fRank = fPp = false;
	
	file.open("profile.html", std::ios::in);
	
	while (std::getline(file, line)) {

		if (regex_search(line, match, rIdUserAgeStyle)) {
			// Loop through captured groups and store them
			for (int i = 1; i <= 4; i++) {			
				switch (i) {
				case 1:
					// Store result to a temp string
					temp = match.str(i);
					// Convert to int and store in struct
					u[userIndex].id = std::stoi(temp);
					fId = true;
					break;
				case 2:
					// Store directly since it's a string
					u[userIndex].username = match.str(i);
					fUser = true;
					break;
				case 3:
					if (match.str(i) == "null"){					
						u[userIndex].age = 0;
						fAge = true;
					}
					else {
						temp = match.str(i);
						u[userIndex].age = std::stoi(temp);
						fAge = true;
					}
					break;
				case 4:
					// Search result for input device and store into the bool array
					temp = match.str(i);
					if (temp.find("mouse") != std::string::npos) {
						u[userIndex].playstyle[0] = true;
					}
					if (temp.find("tablet") != std::string::npos) {
						u[userIndex].playstyle[1] = true;
					}
					if (temp.find("keyboard") != std::string::npos) {
						u[userIndex].playstyle[2] = true;
					}
					if (temp.find("touch") != std::string::npos) {
						u[userIndex].playstyle[3] = true;
					}
					fStyle = true;
					break;
				}
			}
		}
		if (regex_search(line, match, rRankPP)) {
			for (int i = 1; i <= 2; i++) {
				switch(i){
				case 1:
					temp = match.str(i);
					u[userIndex].pp = std::stoi(temp);
					fPp = true;
					break;
				case 2:
					temp = match.str(i);
					u[userIndex].rank = std::stoi(temp);
					fRank = true;
					break;
				}
			}
		}
		if (fId == true && fUser == true && fAge == true && fStyle == true && fRank == true && fPp == true){
			userIndex++;
			fId = fUser = fAge = fStyle = fRank = fPp = false;
		}

	}
	for (int c = 0; c < 10; c++) {
		
		std::cout << "Struct " << c << std::endl;
		std::cout << "ID: " << u[c].id << std::endl;
		std::cout << "Username: " << u[c].username << std::endl;
		std::cout << "PP: " << u[c].pp << std::endl;
		std::cout << "Rank: " << u[c].rank << std::endl;

		if (u[c].age != 0) {
		std::cout << "Age: " << u[c].age << std::endl;
		}
		else {
			std::cout << "Age: Not Set" << std::endl;
		}

		std::cout << "Playstyle: ";
		if (u[c].playstyle[0]) {
		std::cout << "Mouse ";
		}
		if (u[c].playstyle[1]) {
			std::cout << "Tablet ";
		}
		if (u[c].playstyle[2]) {
			std::cout << "Keyboard ";
		}
		if (u[c].playstyle[3]) {
			std::cout << "Touchscreen ";
		}
		if (!u[c].playstyle[0] && !u[c].playstyle[1] && !u[c].playstyle[2] && !u[c].playstyle[3]) {
			std::cout << "Not Set";
		}
		std::cout << std::endl << std::endl;
	}
	file.close();
	file.clear();
	return 0;
}

static size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *stream) {
	std::ostream *out = static_cast<std::ostream *>(stream);
	size_t nbytes = size * nmemb;
	out->write(contents, nbytes);

	return nbytes;
}