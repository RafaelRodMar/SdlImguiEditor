#pragma once
#include<sdl.h>
#include<SDL_image.h>
#include<string>

class Entity {
public:
	std::string name;
	std::string type;
	std::string path;
	SDL_Point pos;
	std::vector<Entity> ventities;
	int selected = -1;
	bool is_selected = false;

	//image
	SDL_Texture* texture;

	Entity(){}

	//name, type
	Entity(std::string n, std::string t) {
		name = n; type = t;
		pos.x = pos.y = 0;
	}

	//name, type, filepath
	Entity(std::string n, std::string t, std::string p) {
		name = n; type = t; path = p;
		pos.x = pos.y = 0;
	}

};
