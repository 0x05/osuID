#pragma once
#include <fstream>
class parser {
public:
	parser();	
	void parseRankings();
	void parseProfiles();
	void printStruct();

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
	
	int userID[10000] = { 0 };
	
};
