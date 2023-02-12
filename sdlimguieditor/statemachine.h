#pragma once

#include "state.h"
#include <vector>

enum STATES { PROJECTMANAGER, EDITOR };

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void changeState(STATES st);

	void update();
	void render();
	void clean();

	State* m_projectManagerState;
	State* m_editorState;

	State* m_currentState;

};