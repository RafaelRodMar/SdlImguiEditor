#include "dearimgui/imgui.h"
#include "dearimgui/imgui_stdlib.h"  //for using std::string in some imgui funcions.
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

void Entity::onInspector()
{
	if (type == "project")
		onInspectorProject();
	else if (type == "scene") 
		onInspectorScene();
	else if (type == "layer")
		onInspectorLayer();
	else if (type == "object")
		onInspectorObject();
}

void Entity::onInspectorProject()
{
	ImGui::Text("Project element name: %s", name);
	ImGui::Text("Type : %s", type);
	ImGui::Text("Position : %d, %d", pos.x, pos.y);
	ImGui::InputText("archive", &path);
}

void Entity::onInspectorScene()
{
	ImGui::Text("Project element name: %s", name);
	ImGui::Text("Type : %s", type);
	ImGui::Text("Position : %d, %d", pos.x, pos.y);
	ImGui::InputText("archive", &path);
}

void Entity::onInspectorLayer()
{
	ImGui::Text("Project element name: %s", name);
	ImGui::Text("Type : %s", type);
	ImGui::Text("Position : %d, %d", pos.x, pos.y);
	ImGui::InputText("archive", &path);
}

void Entity::onInspectorObject()
{
	ImGui::Text("Project element name: %s", name);
	ImGui::Text("Type : %s", type);
	ImGui::Text("Position : %d, %d", pos.x, pos.y);
	ImGui::InputText("archive", &path);
}
