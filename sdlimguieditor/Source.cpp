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

SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;

int main(int argc, char* args[])
{
	srand(time(nullptr));

	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		// if succeeded create our window
		g_pWindow = SDL_CreateWindow("SDL imGui editor",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			1024, 768,
			SDL_WINDOW_SHOWN);
		// if the window creation succeeded create our renderer
		if (g_pWindow != 0)
		{
			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
		}
	}
	else
	{
		return 1; // sdl could not initialize
	}

	//pass the renderer to the assets manager
	AssetsManager::Instance()->renderer = g_pRenderer;
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
	ImGui_ImplSDL2_InitForSDLRenderer(g_pWindow, g_pRenderer);
	ImGui_ImplSDLRenderer_Init(g_pRenderer);
	//***************************************

	//create a new project
	Entity* project = new Entity("testProject", "project", "");

	/*for (int i = 0; i < 10; i++) {
		project->ventities.push_back(Entity("scene", "scene", ""));
	}*/

	bool quit = false;
	SDL_Event e;

	while (!quit) {
		// everything succeeded lets draw the window
		// set to black // This function expects Red, Green, Blue and
		// Alpha as color values
		SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
		// clear the window to black
		SDL_RenderClear(g_pRenderer);

		while (SDL_PollEvent(&e) != 0) {
			ImGui_ImplSDL2_ProcessEvent(&e);
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else
			{
				if (e.type == SDL_TEXTINPUT)
				{
					int x = 0, y = 0;
					SDL_GetMouseState(&x, &y);
					if (e.text.text[0] == 'q')
					{
						quit = true;
					}
				}
			}
		}

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
						quit = true;
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
							//node_open = true;
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
			ImGui::Image((ImTextureID)AssetsManager::Instance()->getTexture("warrior"), ImVec2(33,33));
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

		//draw(warrior, 40, 40, 33, 33, g_pRenderer, SDL_FLIP_NONE);

		// show the window
		SDL_RenderPresent(g_pRenderer);
	}

	// clean up imgui
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	// clean up SDL
	SDL_DestroyWindow(g_pWindow);
	SDL_DestroyRenderer(g_pRenderer);
	AssetsManager::Instance()->clearAllTextures();
	SDL_Quit();
	return 0;
}