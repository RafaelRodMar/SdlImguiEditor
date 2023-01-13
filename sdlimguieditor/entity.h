#pragma once
#include<string>

class Entity {
public:
	Entity(std::string n, std::string t) {
		name = n; type = t;
	}

	std::string name;
	std::string type;
};
