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
	bool remove = false;

	//image
	SDL_Texture* texture;

	//constructors
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

	//Add a new entity based on the current entity type (scene, layer, object)
	bool addEntity() {
		if (ventities.size() >= 256) return false;
		switch (type[0]) {
		case 'p':
			ventities.push_back(Entity("scene", "scene", ""));
			break;
		case 's':
			ventities.push_back(Entity("layer", "layer", ""));
			break;
		case 'l':
			ventities.push_back(Entity("object", "object", ""));
			break;
		case 'o':
			//future feature: objects can have other objects attached.
			break;
		default:
			break;
		}

		return true;
	}

};
