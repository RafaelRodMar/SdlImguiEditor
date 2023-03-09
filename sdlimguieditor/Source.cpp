#include<SDL.h>
#include<SDL_image.h>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<bitset>
#include "Vector2D.h"
#include "json.hpp"
#include "AssetsManager.h"
#include "InputHandler.h"
#include "IconsFontAwesome4.h" //header with names for every icon.
#include "dearimgui/imgui.h"
#include "dearimgui/imgui_impl_sdl.h"
#include "dearimgui/imgui_impl_sdlrenderer.h"
#include "dearimgui/imgui_stdlib.h"  //for using std::string in some imgui funcions.
#include "entity.h"
#include <time.h>
#include "editor.h"
#include "ProjectManagerState.h"
#include "EditorState.h"

//the Editor class
Editor* Editor::s_pInstance = 0;

Editor::Editor() {
	m_pRenderer = nullptr;
	m_pWindow = nullptr;
}

Editor::~Editor() {}

SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;

bool Editor::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
	m_editorWidth = width;
	m_editorHeight = height;

	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		int flags = SDL_WINDOW_SHOWN;
		if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
		
		// if succeeded create our window
		std::cout << "SDL init success\n";
		m_pWindow = SDL_CreateWindow(title,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height, flags);
		// if the window creation succeeded create our renderer
		if (m_pWindow != 0)
		{
			std::cout << "window creation success\n";
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
			if (m_pRenderer != 0)
			{
				std::cout << "renderer creation success\n";
				SDL_SetRenderDrawColor(m_pRenderer, 255, 255, 255, 255);
			}
			else
			{
				std::cout << "renderer init fail\n";
				return false;
			}
		}
		else
		{
			std::cout << "window init fail\n";
			return false;
		}
	}
	else
	{
		std::cout << "SDL init fail\n";
		return false; // sdl could not initialize
	}
	std::cout << "init success\n";
	m_bRunning = true;

	//pass the renderer to the assets manager
	AssetsManager::Instance()->renderer = m_pRenderer;
	//load all the assets in the json file
	AssetsManager::Instance()->loadAssetsJson();

	//***************************************ImGui
	//setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontDefault();

	//configuration flags
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; //navigate through controls by keyboard
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; //not with sdl renderer
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //download the imgui docking version

	io.ConfigWindowsMoveFromTitleBarOnly = true;

	//setup Dear ImGui style
	ImGui::StyleColorsDark();

	//merge icons into default tool font
	ImFontConfig config;
	config.MergeMode = true;
	config.GlyphMinAdvanceX = 13.0f; //use if you want to make the icon monospaced.
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	io.Fonts->AddFontFromFileTTF("assets\\fnt\\fontawesome-webfont.ttf", 13.0f, &config, icon_ranges);

	//setup platform/renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(m_pWindow, m_pRenderer);
	ImGui_ImplSDLRenderer_Init(m_pRenderer);
	//***************************************

	//init of StateMachine
	m_pStateMachine = new StateMachine();     //create the statemachine

	m_pStateMachine->m_projectManagerState = new ProjectManagerState();
	m_pStateMachine->m_editorState = new EditorState();

	m_pStateMachine->m_projectManagerState->onEnter();
	m_pStateMachine->m_editorState->onEnter();

	m_pStateMachine->changeState(PROJECTMANAGER); //assign a current state

	//create a new project
	//project = new Entity("testProject", "project", "");

	g_pRenderer = m_pRenderer;
	g_pWindow = m_pWindow;
	return true;
}

void Editor::handleEvents() {
	InputHandler::Instance()->update();
}

void Editor::update() {
	m_pStateMachine->m_currentState->update();
}

nlohmann::json to_json(Entity node)
{
	nlohmann::json json_node;
	std::string entityType = "";
	json_node["name"] = node.name;
	json_node["type"] = node.type;
	
	for (int i = 0; i < node.ventities.size(); i++) {
		json_node[node.ventities[i].type].push_back(to_json(node.ventities[i]));
	}

	return json_node;
}

void Editor::saveProject() {
	nlohmann::json json_tree = to_json(*project);

	std::ofstream output_file(project->name + "\\" + "entitytree.json");
	output_file << json_tree.dump(2); //two spaces of indentation.
	output_file.close();
}

void Editor::render() {
	// set to black // This function expects Red, Green, Blue and
	// Alpha as color values
	SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
	// clear the window to black
	SDL_RenderClear(g_pRenderer);

	//start the Dear ImGui frame
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	//docking
	ImGui::DockSpaceOverViewport();
	ImGui::DockSpace(ImGui::GetID("DockSpace"));

	//menu

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Project Manager")) {
				m_pStateMachine->changeState(PROJECTMANAGER);
			}
			if (m_pStateMachine->m_currentState->getStateID() == "EDITOR")
			{
				if (ImGui::MenuItem("Save Project")) {
					saveProject();
				}
			}
			if (ImGui::MenuItem("Exit")) {
				Editor::Instance()->quit();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			ImGui::MenuItem("Some Panel", nullptr);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	//render the current state of the editor
	m_pStateMachine->m_currentState->render();

	//ImGui rendering
	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

	// show the window
	SDL_RenderPresent(g_pRenderer);
}

void Editor::quit() {
	m_bRunning = false;
}

int main(int argc, char* args[])
{
	srand(time(nullptr));
	
	if (Editor::Instance()->init("SDL ImGui editor", 100, 100, 1024, 768,
		false))
	{
		while (Editor::Instance()->running()) {
			Editor::Instance()->handleEvents();
			Editor::Instance()->update();
			Editor::Instance()->render();
		}
	}
	else
	{
		std::cout << "editor init failure - " << SDL_GetError() << "\n";
		return -1;
	}

	std::cout << "editor closing...\n";

	// clean up imgui
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	// clean up SDL
	SDL_DestroyWindow(g_pWindow);
	SDL_DestroyRenderer(g_pRenderer);
	AssetsManager::Instance()->clearAllTextures();
	InputHandler::Instance()->clean();
	Editor::Instance()->getStateMachine()->clean();
	Editor::Instance()->cleanStateMachine();
	if (Editor::Instance()->project != nullptr) delete Editor::Instance()->project;
	SDL_Quit();
	return 0;
}