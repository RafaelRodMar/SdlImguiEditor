//compile in release mode.
#include<SDL.h>
#include "dearimgui/imgui.h"
#include "dearimgui/imgui_impl_sdl.h"
#include "dearimgui/imgui_impl_sdlrenderer.h"

SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;

int main(int argc, char* args[])
{
	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		// if succeeded create our window
		g_pWindow = SDL_CreateWindow("Chapter 1: Setting up SDL",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			640, 480,
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

	//***************************************ImGui
	//setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//configuration flags
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; //navigate through controls by keyboard
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; //download the imgui docking version
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	//setup Dear ImGui style
	ImGui::StyleColorsDark();

	//setup platform/renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(g_pWindow, g_pRenderer);
	ImGui_ImplSDLRenderer_Init(g_pRenderer);
	//***************************************

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

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();

			//rendering
			ImGui::Render();
			ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		}

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