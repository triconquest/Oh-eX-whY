#pragma once

class IEngineVGui
{
public:
	virtual					~IEngineVGui(void) = 0;
	virtual int				GetPanel(int type) = 0;
	virtual bool			IsGameUIVisible() = 0;
};