#include <filesystem>
#include <fstream>
#include <cstdlib>
#include "json.hpp"
#include "ProjectManagerState.h"
#include "IconsFontAwesome4.h" //header with names for every icon.
#include "dearimgui/imgui.h"
#include "dearimgui/imgui_impl_sdl.h"
#include "dearimgui/imgui_impl_sdlrenderer.h"
#include "dearimgui/imgui_stdlib.h"  //for using std::string in some imgui funcions.
#include "editor.h"

const std::string ProjectManagerState::s_ID = "PROJECTMANAGER";

ProjectManagerState::ProjectManagerState()
{
}


ProjectManagerState::~ProjectManagerState()
{
}

void ProjectManagerState::update()
{
}

Entity createNode(const nlohmann::json& nodeData) {
	Entity node;

	node.name = nodeData["name"].get<std::string>();
	node.type = nodeData["type"].get<std::string>();

	if (nodeData.find("scene") != nodeData.end()) {
		for (const auto& sceneData : nodeData["scene"]) {
			node.ventities.push_back(createNode(sceneData));
		}
	}

	if (nodeData.find("layer") != nodeData.end()) {
		for (const auto& layerData : nodeData["layer"]) {
			node.ventities.push_back(createNode(layerData));
		}
	}

	if (nodeData.find("object") != nodeData.end()) {
		for (const auto& objectData : nodeData["object"]) {
			node.ventities.push_back(createNode(objectData));
		}
	}

	return node;
}

void ProjectManagerState::render()
{
	ImGui::SetNextWindowFocus();
	ImGui::SetNextItemOpen(true, 0);
	ImGui::Begin("Project Manager");
		static std::string txt = " ";
		static char str0[128] = "";
		if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
			ImGui::SetKeyboardFocusHere(0);
		ImGui::InputText("Project name", str0, IM_ARRAYSIZE(str0));
		if (ImGui::Button("Create Project"))
		{
			if (strlen(str0) > 0 && !std::filesystem::exists(str0))
			{
				txt = "  ";
				Editor::Instance()->project = new Entity(str0, "project", str0);
				Editor::Instance()->project->addEntity();
				Editor::Instance()->project->addEntity();
				Editor::Instance()->project->addEntity();
				Editor::Instance()->project->ventities[1].addEntity();
				Editor::Instance()->project->ventities[1].addEntity();
				if (std::filesystem::create_directory(str0))
				{
					
					Editor::Instance()->getStateMachine()->changeState(EDITOR);
				}
				else
				{
					txt = "Error creating project";
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Project"))
		{
			txt = "  ";
			if (!std::filesystem::exists(str0))
			{
				txt = "couldn't find project ";
			}
			else
			{
				/*Editor::Instance()->project = new Entity(str0, "project", str0);
				Editor::Instance()->project->addEntity();
				Editor::Instance()->project->addEntity();
				Editor::Instance()->project->addEntity();
				Editor::Instance()->project->ventities[1].addEntity();
				Editor::Instance()->project->ventities[1].addEntity();*/
				nlohmann::json jsonData;
				std::string f = str0;
				f += "\\"; f += "entitytree"; f += ".json";

				std::ifstream jsonFile(f);
				if (jsonFile.is_open())
				{
					jsonFile >> jsonData;  //read the file

					Entity e = createNode(jsonData);
					Editor::Instance()->project = new Entity("", "", "");
					Editor::Instance()->project->name = e.name;
					Editor::Instance()->project->type = e.type;
					Editor::Instance()->project->ventities = e.ventities;

					Editor::Instance()->getStateMachine()->changeState(EDITOR);
				}
			}
		}
		if (ImGui::Button("Return To Editor"))
		{
			if(Editor::Instance()->project != nullptr) Editor::Instance()->getStateMachine()->changeState(EDITOR);
		}
		ImGui::Text(txt.c_str());
	ImGui::End();

	ImGui::ShowDemoWindow();
}

bool ProjectManagerState::onEnter()
{
	return false;
}

bool ProjectManagerState::onExit()
{
	return false;
}
