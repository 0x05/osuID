#pragma once
#include <fstream>
class connect {
public:
	connect();
	int connector();
	int getProfileInfo();
	int scrapeRankings();

private:
	struct user {
		int id;
		std::string username;
		int age;
		bool playstyle[4] = { false };
		int rank;
		int pp;
	};
	user u[10000];
	
	int scrapedID[10000] = { 0 };

	std::fstream file;
	std::fstream rankingsFile;
};

//question
//let's say I wanted to parse 10000 html pages
//does it make sense to output into a file and then parse the file?

//also can you recommend any source for good practices when handling that much data?