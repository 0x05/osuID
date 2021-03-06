#include "cconnect.h"
#include "curl\curl.h"
#include "parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>


using std::cout;
using std::endl;

parser::parser() {}

// Parse ranking pages for profile IDs and store into an array
int parser::parseRankings() {

	cconnect c;

	std::regex rUserID(R"(users\/(\d+))");
	std::smatch match;

	std::string temp;

	int IDcounter = 0;

	for (int page = 1; page <= 2; page++) {
		
		cout << "Retrieving rankings page " << page << "... ";
		
		std::stringstream URL;
		std::string base = "https://osu.ppy.sh/rankings/osu/performance?page=";
		URL << base << page;

		std::string html = c.connector(URL.str());
		
		cout << "Done" << endl;
		
		while (regex_search(html, match, rUserID)) {
			temp = match.str(1);
			userID[IDcounter] = std::stoi(temp);
			
			IDcounter++;
			html = match.suffix();
		}
	}

	return 0;
}

// Parse profile pages and store into struct
int parser::parseProfiles() {

	cconnect c;

	std::regex rIdUserAgeStyle(R"(\"id\":(\d+),\"username\":\"([\w" "-]+)\".+\"age\":(\w+),.+\"playstyle\":\[?([\w\",]+)\]?,)"); // Capture ID, Username, Age and Playstyle
	std::regex rRankPP(R"(\"pp\":(\d+).?\S+\"pp_rank\":(\d+))"); // Capture PP and Rank
	std::smatch match;
	
	std::string temp;

	int profileCounter = 0;

	parseRankings();
	
	for (int pid = MIN_USER; pid < MAX_USER; pid++) {

		cout << "Retrieving profile " << pid << " [" << userID[pid] << "]";

		std::stringstream URL;
		std::string base = "https://osu.ppy.sh/users/";
		URL << base << userID[pid];

		std::string html = c.connector(URL.str());

		cout << "... Done\nParsing profile...";

		while (regex_search(html, match, rRankPP)) {
			for (int i = 1; i <= 2; i++) {
				switch (i) {
				case 1:
					temp = match.str(i);
					u[profileCounter].pp = std::stoi(temp);
					break;
				case 2:
					temp = match.str(i);
					u[profileCounter].rank = std::stoi(temp);
					break;
				}
			}

			html = match.suffix();
		}
		while (regex_search(html, match, rIdUserAgeStyle)) {
			// Loop through captured groups and store them
			for (int i = 1; i <= 4; i++) {
				switch (i) {
				case 1:
					// Store result to a temp string
					temp = match.str(i);
					// Convert to int and store in struct
					u[profileCounter].id = std::stoi(temp);
					break;
				case 2:
					// Store directly since it's a string
					u[profileCounter].username = match.str(i);
					break;
				case 3:
					if (match.str(i) == "null") {
						u[profileCounter].age = 0;
					}
					else {
						temp = match.str(i);
						u[profileCounter].age = std::stoi(temp);
					}
					break;
				case 4:
					// Search result for input device and store into the bool array
					temp = match.str(i);
					if (temp.find("mouse") != std::string::npos) {
						u[profileCounter].playstyle[0] = true;
					}
					if (temp.find("tablet") != std::string::npos) {
						u[profileCounter].playstyle[1] = true;
					}
					if (temp.find("keyboard") != std::string::npos) {
						u[profileCounter].playstyle[2] = true;
					}
					if (temp.find("touch") != std::string::npos) {
						u[profileCounter].playstyle[3] = true;
					}
					break;
				}
			}

			html = match.suffix();
		}

		profileCounter++;

		cout << "Done" << endl;
	}

	return 0;
}

void parser::writeUserID() {
	std::ofstream ofs("uid.dat");
	for (int i = 0; i < 100; i++) {
		ofs << userID[i] << endl;
	}
}
/*
void parser::writeProfileData() {
	std::ofstream ofs("profile.dat"); 
	for (int i = 0; i < 10; i++) {
		ofs << u[i].id << "," <<
			u[i].username << "," <<
			u[i].rank << "," <<
			u[i].pp << "," <<
			u[i].age << ",";

		for (int p = 0; p < 4; p++) {
			ofs << u[i].playstyle[p];
			if (p == 3) {
				ofs << endl;
			}
		}	
	}
}*/

void parser::writeProfileData() {
	std::ofstream ofs("profile.dat");
	boost::archive::text_oarchive oa(ofs);
	for (int i = 0; i < 10;i++) {
		oa << u[i].id;
		oa << u[i].username;
		oa << u[i].age;
		oa << u[i].playstyle;
		oa << u[i].rank;
		oa << u[i].pp;
	}
}

void parser::loadProfileData() {
	std::ifstream ifs("profile.dat");
	boost::archive::text_iarchive ia(ifs);
	for (int i = 0; i < 10; i++) {
		ia >> u[i].id;
		ia >> u[i].username;
		ia >> u[i].age;
		ia >> u[i].playstyle;
		ia >> u[i].rank;
		ia >> u[i].pp;
	}
}

// not needed
void parser::printStruct() {

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
}