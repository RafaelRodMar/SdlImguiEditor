#pragma once

#include <string>
#include <vector>
#include "state.h"

class EditorState :	public State
{
public:
	EditorState();
	~EditorState();

	virtual void update();
	virtual void render();

	virtual bool onEnter();
	virtual bool onExit();

	virtual std::string getStateID() const { return s_ID; }

private:
	static const std::string s_ID;
};
