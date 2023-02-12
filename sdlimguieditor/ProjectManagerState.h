#pragma once

#include <string>
#include <vector>
#include "state.h"

class ProjectManagerState : public State
{
public:
	ProjectManagerState();
	~ProjectManagerState();

	virtual void update();
	virtual void render();

	virtual bool onEnter();
	virtual bool onExit();

	virtual std::string getStateID() const { return s_ID; }

private:
	static const std::string s_ID;
};