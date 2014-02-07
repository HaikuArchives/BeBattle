#ifndef MAIN_H
#define MAIN_H

#include <Application.h>
#include "game.h"
#include "about.h"

class Application : public BApplication 
{
 public:
	BeBattle *B; //the window
	AboutBox* About; //about box
	Application();
};

#endif

