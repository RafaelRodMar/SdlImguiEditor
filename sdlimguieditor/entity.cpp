#include "entity.h"
#include "editor.h"

//Add a new entity based on the current entity type (scene, layer, object)
bool Entity::addEntity() {
	if (ventities.size() >= 256) return false;

	int entityID = Editor::Instance()->getEntityID();
	switch (type[0]) {
	case 'p':
		ventities.push_back(Entity("scene" + std::to_string(entityID), "scene", ""));
		break;
	case 's':
		ventities.push_back(Entity("layer" + std::to_string(entityID), "layer", ""));
		break;
	case 'l':
		ventities.push_back(Entity("object" + std::to_string(entityID), "object", ""));
		break;
	case 'o':
		//future feature: objects can have other objects attached.
		break;
	default:
		break;
	}

	return true;
}