#include "statemachine.h"

StateMachine::StateMachine()
{
}

StateMachine::~StateMachine()
{
}

void StateMachine::changeState(STATES newState)
{
	if (newState == CREATEPROJECT) m_currentState = m_createProjectState;
	if (newState == EDITOR) m_currentState = m_editorState;
}

void StateMachine::update() {
	m_currentState->update();
}

void StateMachine::render()
{
	m_currentState->render();
}

void StateMachine::clean() {
	m_currentState = nullptr;

	m_createProjectState->onExit();
	delete m_createProjectState;

	m_editorState->onExit();
	delete m_editorState;
}