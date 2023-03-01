#include <iostream>
#include <vector>
#include "SDL.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

struct TreeNode {
    std::string name;
    bool is_selected;
    std::vector<TreeNode> children;

    TreeNode(const std::string& _name) : name(_name), is_selected(false) {}
};

void render_tree_node(TreeNode& node) {
    bool node_selected = ImGui::Selectable(node.name.c_str(), node.is_selected);
    if (node_selected != node.is_selected) {
        node.is_selected = node_selected;
    }

    if (!node.children.empty()) {
        ImGui::Indent();
        for (auto& child : node.children) {
            render_tree_node(child);
        }
        ImGui::Unindent();
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow("Dear ImGui Nested Tree with Selectable Nodes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        std::cerr << "Error creating SDL window: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create SDL OpenGL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        std::cerr << "Error creating SDL OpenGL context: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Create the nested tree
    TreeNode root("Root");
    root.children.push_back(TreeNode("Child 1"));
    root.children.push_back(TreeNode("Child 2"));
    root.children[1].children.push_back(TreeNode("Grandchild 1"));
    root.children[1].children.push_back(TreeNode("Grandchild 2"));

    // Main loop
    bool quit = false;
    while (!quit) {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Start new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // Render the tree
        render_tree_node(root);

        // Render ImGui
        ImGui::Render();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

