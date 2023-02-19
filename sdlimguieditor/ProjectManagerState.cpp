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

void ProjectManagerState::render()
{
	ImGui::Begin("Project Manager");
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
