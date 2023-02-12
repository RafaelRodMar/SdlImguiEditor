#include "statemachine.h"

StateMachine::StateMachine()
{
}

StateMachine::~StateMachine()
{
}

void StateMachine::changeState(STATES newState)
{
	if (newState == PROJECTMANAGER) m_currentState = m_projectManagerState;
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

	m_projectManagerState->onExit();
	delete m_projectManagerState;

	m_editorState->onExit();
	delete m_editorState;
}