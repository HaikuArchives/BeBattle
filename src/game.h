#ifndef GAME_H
#define GAME_H

#include <Window.h>
#include "panel.h"
#include "dview.h"

class BeBattle : public BWindow
{
 public:
	ActivePanel *Panel1; //Active unit panel
	PassivePanel *Panel2; //Passive unit panel
	DView* Damier; //the board
	
	BeBattle();
	virtual	bool QuitRequested();
	virtual void MessageReceived(BMessage* message);
};

#endif