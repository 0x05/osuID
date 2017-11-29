#pragma once
#include <fstream>
class parser {
public:
	parser();	
	int parseRankings();
	int parseProfiles();
	
	void writeUserID();
	void writeProfileData();

	void printStruct();

private:
	int MIN_USER = 80;
	int MAX_USER = 90;

	struct user {
		int id;
		std::string username;
		int age;
		bool playstyle[4] = { false };
		int rank;
		int pp;
	};
	user u[10000];
	int userID[10000] = { 0 };
	
};
