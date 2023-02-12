#pragma once

#include "state.h"
#include <vector>

enum STATES { CREATEPROJECT, EDITOR };

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void changeState(STATES st);

	void update();
	void render();
	void clean();

	State* m_createProjectState;
	State* m_editorState;

	State* m_currentState;

};