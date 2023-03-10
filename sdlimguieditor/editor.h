#pragma once

#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include "AssetsManager.h"
#include "InputHandler.h"
#include "statemachine.h"
#include "entity.h"

class Editor {
public:
	static Editor* Instance()
	{
		if (s_pInstance == 0)
		{
			s_pInstance = new Editor();
			return s_pInstance;
		}
		return s_pInstance;
	}

	SDL_Renderer* getRenderer() const { return m_pRenderer; }

	~Editor();

	bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	void render();
	void update();
	void handleEvents();
	//void clean();
	void quit();

	bool running() { return m_bRunning; }

	int getEditorWidth() const { return m_editorWidth; }
	int getEditorHeight() const { return m_editorHeight; }
	StateMachine* getStateMachine() { return m_pStateMachine; }
	void cleanStateMachine() { delete m_pStateMachine; }

	Entity* project;
	int getEntityID() {
		return entityID++;
	}
	void saveProject();

private:
	Editor();
	static Editor* s_pInstance;
	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;

	StateMachine* m_pStateMachine;

	bool m_bRunning;
	int m_editorWidth;
	int m_editorHeight;
	int entityID = 0;  //every entity has a different ID number
};
