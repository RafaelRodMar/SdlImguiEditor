#include<SDL.h>
#include<SDL_image.h>
#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include "dearimgui/imgui.h"
#include "dearimgui/imgui_impl_sdl.h"
#include "dearimgui/imgui_impl_sdlrenderer.h"
#include "entity.h"
#include <time.h>

SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;

SDL_Texture* loadTexture(std::string fileName, SDL_Renderer* pRenderer)
{
	SDL_Surface* pTempSurface = IMG_Load(fileName.c_str()); //create Surface with contents of fileName

	if (pTempSurface == 0) //exception handle load error
	{
		std::cout << "error loading file: " << fileName << std::endl;
		std::cout << SDL_GetError() << " " << fileName << std::endl;
		return false;
	}

	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pTempSurface); //Pass Surface to Texture
	SDL_FreeSurface(pTempSurface); //Delete Surface

	if (pTexture == 0) //exception handle transiction Surfacte to Texture
	{
		std::cout << "error creating Texture of file: " << fileName << std::endl;
		return false;
	}

	return pTexture; //save texture.
}

void draw(SDL_Texture* texture, int x, int y, int width, int height, SDL_Renderer* pRenderer, SDL_RendererFlip flip)
{
	SDL_Rect srcRect; //source rectangle
	SDL_Rect destRect; //destination rectangle

	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x; //select x axis on game window
	destRect.y = y; //select y axis on game window

	SDL_RenderCopyEx(pRenderer, texture, &srcRect, &destRect, 0, 0, flip); //Load image to render buffer.
}

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

	SDL_Texture* warrior = loadTexture("files/warrior.png", g_pRenderer);

	//***************************************ImGui
	//setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//configuration flags
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; //navigate through controls by keyboard
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; //not with sdl renderer
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //download the imgui docking version

	//setup Dear ImGui style
	ImGui::StyleColorsDark();

	//setup platform/renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(g_pWindow, g_pRenderer);
	ImGui_ImplSDLRenderer_Init(g_pRenderer);
	//***************************************

	std::vector<Entity> ventities;

	std::string types[] = {"sonido", "imagen", "fondo"};
	std::string names = "nombre";
	int num = 0;
	for (int i = 0; i < 100; i++) {
		ventities.push_back(Entity(names + std::to_string(num), types[rand() % 3]));
		num++;
	}

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
			static int last_selected = -1;
			ImGui::Begin("Project");                          // Create a window with name and append into it.
			//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too.
			//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			//	counter++;
			//ImGui::SameLine();
			//ImGui::Text("counter = %d", counter);
			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			if (ImGui::TreeNode("project name"))
			{
				// 'selection_mask' is dumb representation of what may be user-side selection state.
				//  You may retain selection state inside or outside your objects in whatever format you see fit.
				// 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
				/// of the loop. May be a pointer to your own node type, etc.
				static std::vector<bool> selection_mask(ventities.size(), false);
				int node_clicked = -1;
				for (int i = 0; i < ventities.size(); i++) {
					// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
					// To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
					ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
					const bool is_selected = selection_mask[i] != false;
					if (is_selected)
						node_flags |= ImGuiTreeNodeFlags_Selected;

					bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, ventities[i].name.c_str());
					if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					{
						node_clicked = i;
						last_selected = i;
					}
					
					if (node_open)
					{
						ImGui::BulletText("Blah blah\nBlah Blah");
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
			ImGui::Text("this is another window");
			if (last_selected != -1)
			{
				ImGui::Text("Project element : %d", last_selected);
				ImGui::Text("Name : %s", ventities[last_selected].name);
				ImGui::Text("Type : %s", ventities[last_selected].type);
			}
			ImGui::End();

			ImGui::Begin("Assets");
			ImGui::End();

			ImGui::Begin("View");
			ImGui::Image((ImTextureID)warrior, ImVec2(33, 33));
			ImGui::End();


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
	SDL_Quit();
	return 0;
}