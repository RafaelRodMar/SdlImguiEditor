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

void EditorState::remove_node(Entity& node) {
	
	if (!node.ventities.empty()) {
		for (int i = 0; i < node.ventities.size();i++) {
			if (node.ventities[i].remove == true) {
				node.ventities.erase(node.ventities.begin() + i);
			}
			else
			{
				remove_node(node.ventities[i]);
			}
		}
	}
}

void EditorState::render_tree_node2(Entity& node) {
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (entitySelected.name == node.name)
		base_flags |= ImGuiTreeNodeFlags_Selected;

	ImGui::SetNextItemOpen(true, 0);
	if (ImGui::TreeNodeEx(node.name.c_str(), base_flags))
	{
		if (ImGui::IsItemClicked())
		{
			entitySelected = node;
		}

		//context menu
		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			std::string add = "Add ";
			std::string remove = "Remove ";

			switch (node.type[0]) {
			case 'p':
				add += "scene";
				break;
			case 's':
				add += "layer";
				remove += "scene";
				break;
			case 'l':
				add += "object";
				remove += "layer";
				break;
			case 'o':
				remove += "object";
				break;
			default:
				break;
			}

			if (add != "Add ")
			{
				if (ImGui::MenuItem(add.c_str()))
				{
					node.addEntity();
				}
			}
			if (remove != "Remove ")
			{
				if(add != "Add ") ImGui::Separator();
				if (ImGui::MenuItem(remove.c_str()))
				{
					node.remove = true;
				}
			}
			ImGui::EndPopup();
		}

		if (!node.ventities.empty()) {
			for (auto& child : node.ventities) {
				render_tree_node2(child);
			}
		}

		ImGui::TreePop();
	}
}

void EditorState::render_tree(Entity& node) {
	//render the tree
	render_tree_node2(*Editor::Instance()->project);
	//remove nodes marked for deletion
	remove_node(*Editor::Instance()->project);
}

void EditorState::render()
{
	if (Editor::Instance()->project == nullptr) return;

	auto& vecScenes = Editor::Instance()->project->ventities;

	//imgui

	//project window
	ImGui::Begin("Project");
		render_tree(*Editor::Instance()->project);
	ImGui::End();

	//another window
	ImGui::Begin("Inspector");
	if (entitySelected.name != "")
	{
		entitySelected.onInspector();
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
		ImGui::Text("last selected %s", entitySelected);
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
