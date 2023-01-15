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

	//image
	SDL_Texture* texture;

	Entity(std::string n, std::string t, std::string p) {
		name = n; type = t; path = p;
		pos.x = pos.y = 0;
	}

};
