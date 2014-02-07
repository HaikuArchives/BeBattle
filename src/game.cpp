#include "game.h"
#include "enum.h"
#include <Application.h>
#include "main.h"
#include <stdio.h>
//-------------------------------------------------------------------
BeBattle::BeBattle()
 : BWindow(BRect(5, 22, 585, 470) , "BeBattle v2.1", B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE )
{//640x480 window
	//active unit panel
	Panel1 = new ActivePanel;
	AddChild(Panel1);

	//passive unit panel
	Panel2 = new PassivePanel;
	AddChild(Panel2);
	
	//the board
	Damier = new DView();
	AddChild(Damier);
	
	Show();
}
//--------------------------------------------------------------------------------
bool BeBattle::QuitRequested()
{//Who would like to quit this marvelous game ?
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}
//--------------------------------------------------------------------------------
void BeBattle::MessageReceived(BMessage* message)
{
	switch(message->what)
	{ //redirect button action to the board...
		case BUTTON1: case BUTTON2: case BUTTON3:
		case BUTTON4: case BUTTON5: case BUTTON6:
			Damier->MessageReceived(message); break;
			
		case ABOUT:
			((Application*)be_app)->About->MessageReceived(message);
		break;

		case RESTART:
			Damier->Restart();
		break;

		default:
			BWindow::MessageReceived(message);
	}
}
//--------------------------------------------------------------------------------
