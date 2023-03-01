#include "EditorState.h"
#include "IconsFontAwesome4.h" //header with names for every icon.
#include "dearimgui/imgui.h"
#include "dearimgui/imgui_impl_sdl.h"
#include "dearimgui/imgui_impl_sdlrenderer.h"
#include "dearimgui/imgui_stdlib.h"  //for using std::string in some imgui funcions.
#include "editor.h"

const std::string EditorState::s_ID = "EDITOR";

EditorState::EditorState()
{
}


EditorState::~EditorState()
{
}

void EditorState::update()
{
}

void EditorState::render_tree_node(Entity& node) {
	bool node_selected = ImGui::Selectable(node.name.c_str(), node.is_selected);
	if (node_selected != node.is_selected) {
		node.is_selected = node_selected;
	}
	ImGui::SameLine();
	if (node.is_selected)
		ImGui::Text("is");
	else
		ImGui::Text("is not");

	if (!node.ventities.empty()) {
		ImGui::Indent();
		for (auto& child : node.ventities) {
			render_tree_node(child);
		}
		ImGui::Unindent();
	}
}

void EditorState::render()
{
	if (Editor::Instance()->project == nullptr) return;

	auto& vecScenes = Editor::Instance()->project->ventities;

	//imgui

	//static float f = 0.0f;
	//static int counter = 0;
	Entity entitySelected;

	//one window
	ImGui::Begin("Project");                          // Create a window with name and append into it.

	//show scenes
	ImGui::SetNextItemOpen(true, 0);
	if (ImGui::TreeNode(Editor::Instance()->project->name.c_str()))
	{
		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			if (ImGui::MenuItem("Add scene"))
			{
				if (vecScenes.size() < 256)
				{
					vecScenes.push_back(Entity("scene", "scene"));
				}
			}
			ImGui::EndPopup();
		}

		// 'selection_mask' is dumb representation of what may be user-side selection state.
		//  You may retain selection state inside or outside your objects in whatever format you see fit.
		// 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
		/// of the loop. May be a pointer to your own node type, etc.
		static std::vector<bool> selection_mask_scenes(256, false);
		int node_clicked_scenes = -1;
		for (int i = 0; i < vecScenes.size(); i++) {
			// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
			// To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
			ImGuiTreeNodeFlags node_flags_scenes = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			const bool is_selected_scenes = selection_mask_scenes[i] != false;
			if (is_selected_scenes)
				node_flags_scenes |= ImGuiTreeNodeFlags_Selected;

			bool node_open_scenes = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags_scenes, vecScenes[i].name.c_str());
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			{
				node_clicked_scenes = i;
				Editor::Instance()->project->selected = i;
			}

			//context menu
			if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
			{
				//ImGui::Text("test %d", ImGui::GetItemID());
				if (ImGui::MenuItem("Add Layer"))
				{
					Editor::Instance()->project->selected = i;
					vecScenes[Editor::Instance()->project->selected].ventities.push_back(Entity("layer", "layer"));
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Remove scene"))
				{
					vecScenes.erase(Editor::Instance()->project->ventities.begin() + i);
					Editor::Instance()->project->selected = -1;
				}
				ImGui::EndPopup();
			}

			if (node_open_scenes)
			{
				//show layers
				static std::vector<bool> selection_mask_layers(256, false);
				int node_clicked_layers = -1;
				//node_flags = ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; //leaf node
				for (int j = 0; j < vecScenes[i].ventities.size(); j++) {
					auto& vecLayers = vecScenes[i].ventities;
					ImGuiTreeNodeFlags node_flags_layers = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
					const bool is_selected_layers = selection_mask_layers[j] != false;
					if (is_selected_layers)
						node_flags_layers |= ImGuiTreeNodeFlags_Selected;

					bool node_open_layers = ImGui::TreeNodeEx((void*)(intptr_t)j, node_flags_layers, vecLayers[j].name.c_str());
					if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					{
						node_clicked_layers = j;
						Editor::Instance()->project->ventities[i].selected = j;
					}

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

					if (node_open_layers)
					{
						//show objects
						ImGui::TreePop();
					}
				}

				if (node_clicked_layers != -1)
				{
					//remove selections from scene level
					node_clicked_scenes = -1;
					std::fill(selection_mask_scenes.begin(), selection_mask_scenes.end(), 0);
					// Update selection state
					// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
					if (ImGui::GetIO().KeyCtrl)
						selection_mask_layers[node_clicked_layers] = !selection_mask_layers[node_clicked_layers];          // CTRL+click to toggle
					else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
					{
						std::fill(selection_mask_layers.begin(), selection_mask_layers.end(), 0);
						selection_mask_layers[node_clicked_layers] = true;           // Click to single-select
					}
				}
				ImGui::TreePop();
			}
		}

		if (node_clicked_scenes != -1)
		{
			// Update selection state
			// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
			if (ImGui::GetIO().KeyCtrl)
				selection_mask_scenes[node_clicked_scenes] = !selection_mask_scenes[node_clicked_scenes];          // CTRL+click to toggle
			else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
			{
				std::fill(selection_mask_scenes.begin(), selection_mask_scenes.end(), 0);
				selection_mask_scenes[node_clicked_scenes] = true;           // Click to single-select
			}
		}

		ImGui::TreePop();
	}

	ImGui::End();

	//tree test window
	ImGui::Begin("testWindow");
		render_tree_node(*Editor::Instance()->project);
	ImGui::End();

	//another window
	ImGui::Begin("Inspector");
	if (Editor::Instance()->project->selected != -1)
	{
		ImGui::Text("Project element : %d", Editor::Instance()->project->selected);
		ImGui::Text("Name : %s", vecScenes[Editor::Instance()->project->selected].name);
		ImGui::Text("Type : %s", vecScenes[Editor::Instance()->project->selected].type);
		ImGui::Text("Position : %d, %d", vecScenes[Editor::Instance()->project->selected].pos.x, vecScenes[Editor::Instance()->project->selected].pos.y);
		ImGui::InputText("archive", &vecScenes[Editor::Instance()->project->selected].path);
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
	ImGui::Text("last selected %d", Editor::Instance()->project->selected);
	ImGui::End();

	ImGui::ShowDemoWindow();
}

bool EditorState::onEnter()
{
	return false;
}

bool EditorState::onExit()
{
	return false;
}
