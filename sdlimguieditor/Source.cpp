#include<SDL.h>
#include<SDL_image.h>
#include<iostream>
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

	m_pStateMachine->changeState(EDITOR); //assign a current state

	//create a new project
	project = new Entity("testProject", "project", "");

	g_pRenderer = m_pRenderer;
	g_pWindow = m_pWindow;
	return true;
}

void Editor::handleEvents() {
	InputHandler::Instance()->update();
}

void Editor::update() {

}

void Editor::render() {
	// set to black // This function expects Red, Green, Blue and
	// Alpha as color values
	SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
	// clear the window to black
	SDL_RenderClear(g_pRenderer);

	//imgui
	{
		//start the Dear ImGui frame
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//docking
		ImGui::DockSpaceOverViewport();

		//menu
		// ...
		ImGui::DockSpace(ImGui::GetID("DockSpace"));

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit")) {
					Editor::quit();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {
				ImGui::MenuItem("Some Panel", nullptr);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		static float f = 0.0f;
		static int counter = 0;
		Entity entitySelected;

		//one window
		ImGui::Begin("Project");                          // Create a window with name and append into it.

		//show scenes
		ImGui::SetNextItemOpen(true, 0);
		if (ImGui::TreeNode(project->name.c_str()))
		{
			if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
			{
				if (ImGui::MenuItem("Add scene"))
				{
					if (project->ventities.size() < 256)
					{
						project->ventities.push_back(Entity("scene", "scene"));
					}
				}
				ImGui::EndPopup();
			}

			// 'selection_mask' is dumb representation of what may be user-side selection state.
			//  You may retain selection state inside or outside your objects in whatever format you see fit.
			// 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
			/// of the loop. May be a pointer to your own node type, etc.
			static std::vector<bool> selection_mask(256, false);
			int node_clicked = -1;
			for (int i = 0; i < project->ventities.size(); i++) {
				// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
				// To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
				const bool is_selected = selection_mask[i] != false;
				if (is_selected)
					node_flags |= ImGuiTreeNodeFlags_Selected;

				bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, project->ventities[i].name.c_str());
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				{
					node_clicked = i;
					project->selected = i;
				}

				//context menu
				if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
				{
					//ImGui::Text("test %d", ImGui::GetItemID());
					if (ImGui::MenuItem("Add Layer"))
					{
						project->selected = i;
						project->ventities[project->selected].ventities.push_back(Entity("layer", "layer"));
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Remove scene"))
					{
						project->ventities.erase(project->ventities.begin() + i);
						project->selected = -1;
					}
					ImGui::EndPopup();
				}

				if (node_open)
				{
					//show layers
					//node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
					node_flags = ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; //leaf node
					for (int j = 0; j < project->ventities[i].ventities.size(); j++) {
						ImGui::TreeNodeEx((void*)(intptr_t)j, node_flags, project->ventities[i].ventities[j].name.c_str());

						//context menu
						if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
						{
							if (ImGui::MenuItem("Add Object"))
							{
							}
							ImGui::Separator();
							if (ImGui::MenuItem("Remove Layer"))
							{
							}
							ImGui::EndPopup();
						}
					}
					ImGui::TreePop();
				}
			}

			if (node_clicked != -1)
			{
				// Update selection state
				// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
				if (ImGui::GetIO().KeyCtrl)
					selection_mask[node_clicked] = !selection_mask[node_clicked];          // CTRL+click to toggle
				else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
				{
					std::fill(selection_mask.begin(), selection_mask.end(), 0);
					selection_mask[node_clicked] = true;           // Click to single-select
				}
			}

			ImGui::TreePop();
		}

		ImGui::End();

		//another window
		ImGui::Begin("Inspector");
		if (project->selected != -1)
		{
			ImGui::Text("Project element : %d", project->selected);
			ImGui::Text("Name : %s", project->ventities[project->selected].name);
			ImGui::Text("Type : %s", project->ventities[project->selected].type);
			ImGui::Text("Position : %d, %d", project->ventities[project->selected].pos.x, project->ventities[project->selected].pos.y);
			ImGui::InputText("archive", &project->ventities[project->selected].path);
		}
		ImGui::End();

		ImGui::Begin("Assets");
		//icon example
		ImGui::Text("%s among %d items", ICON_FA_SEARCH, 54);
		ImGui::Button(ICON_FA_SEARCH " Search");
		ImGui::End();

		ImGui::Begin("View");
		ImGui::Image((ImTextureID)AssetsManager::Instance()->getTexture("warrior"), ImVec2(33, 33));
		ImGui::SetCursorPos(ImVec2(100, 100));
		ImGui::Image((ImTextureID)AssetsManager::Instance()->getTexture("warrior"), ImVec2(33, 33));
		ImGui::End();

		ImGui::Begin("Console");
		ImGui::Text("last selected %d", project->selected);
		ImGui::End();

		ImGui::ShowDemoWindow();


		//rendering
		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	}

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
	SDL_Quit();
	return 0;
}