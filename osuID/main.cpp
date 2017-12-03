#include "curl\curl.h"
#include "parser.h"
#include <iostream>
#include <regex>

int main() {

	curl_global_init(CURL_GLOBAL_DEFAULT);

	parser p;

	//p.parseProfiles();
	//p.writeUserID();
	//p.writeProfileData();
	p.loadProfileData();
	p.printStruct();

	system("Pause");

	curl_global_cleanup();
	return 0;
}