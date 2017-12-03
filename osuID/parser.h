#pragma once
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>

class parser {
public:
	parser();
	int parseRankings();
	int parseProfiles();
	
	void writeUserID();
	void writeProfileData();
	void loadProfileData();

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
	
	template<class Archive>
	void serialize(Archive & ar, user& u, const unsigned int version) {		
		ar & u.id;
		ar & u.username;
		ar & u.age;
		ar & u.playstyle;
		ar & u.rank;
		ar & u.pp;
	}
};
