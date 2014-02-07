#include "dview.h"
#include <TranslationUtils.h>
#include <Application.h>
#include "main.h"
#include "enum.h"
#include <stdio.h>
//----------------------------------------------------------------------
DView::DView() : BView(BRect(0, 0, 447, 447), "Board", B_FOLLOW_NONE, B_WILL_DRAW)
{
	srand((long) system_time()); //randomize
	Background = BTranslationUtils::GetBitmap("bmp/background/BeBattle.png"); //Intro background
	MakeTransparent(Background); //apply tranparency if needed

	Animation = new TAnimation; //animations
	SEffect = new Effect(); //special effects
	
	//The view is not automatically erased before updating
	SetViewColor(B_TRANSPARENT_32_BIT); 

	//Offscreen bitmap to avoid flickering
	OffscreenBitmap  = new BBitmap(BRect(0, 0, 447, 447), B_RGB_32_BIT, true); 
	OffscreenView = new BView(BRect(0, 0, 447, 447), "", B_FOLLOW_NONE, (uint32)NULL); //our canvas
	OffscreenView->SetHighColor(255, 255, 255, 96); //transparent white (for the highlight box)
	OffscreenView->SetDrawingMode(B_OP_ALPHA);
	OffscreenView->SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
	OffscreenView->SetViewColor(B_TRANSPARENT_32_BIT);
	OffscreenBitmap->AddChild(OffscreenView);

	Mode = Option; //option mode 
	Playing = false;
	AIPlayer = false;
	AILock = false;
	RedSpot();
}
//----------------------------------------------------------------------
void DView::Ini(int group, bool CPU)
{//Start a new game
	AIPlayer = CPU;
	delete Background;
	Background = BTranslationUtils::GetBitmap("bmp/background/board.png"); //board background
	MakeTransparent(Background); //apply transparency if needed
	
	Mode = Unit; //Unit mode (now pressing a button trigger a unit action)
	for(ITER clear = Liste.begin(); clear!=Liste.end(); clear++)
		(*clear).DestroyUnit(); //free the memory occupied by this unit
		
	Liste.clear(); //empty the list, pour (re)commencer une game
	Item temp;

	bool FreeList[196]; //List of unoccupied squares
	for(int i=0; i<196; i++)
		FreeList[i] = true;
	
	int ARandom;
	for(int g=0; g<group; g++)
		for(int u=0; u<5; u++)
			for(int p=1; p<=2; p++)
			{
				do 
				{ARandom = rand() % 196;}
				while(FreeList[ARandom] != true);

				//we found a spot
				FreeList[ARandom] = false;
			
				switch(u)
				{
					case 0: temp.Unit = new Be1(ARandom, p);break;
					case 1: temp.Unit = new Be2(ARandom, p);break;
					case 2: temp.Unit = new Be3(ARandom, p);break;
					case 3: temp.Unit = new Be4(ARandom, p);break;
					case 4: temp.Unit = new Be5(ARandom, p);break;
				}
				Liste.push_back(temp);
			}

	Invalidate(); //Draw the background and the units
	Active = Liste.end();
	GetNext(); //fetch the unit that play first
	Playing = true;
}
//----------------------------------------------------------------------
void DView::Restart()
{
	Playing = false;
	Mode = Option; //option mode 
	((Application*)be_app)->B->Panel1->ChangeBe(0);
	((Application*)be_app)->B->Panel1->UpdateStat(0, false, false, false, false); //stats
	((Application*)be_app)->B->Panel1->PressButton(1);
	CurrentAction = 1;
	((Application*)be_app)->B->Panel2->Image->ChangePortrait(0);
	((Application*)be_app)->B->Panel2->UpdateStat(0, false, false, false, false);
}
//----------------------------------------------------------------------
void DView::GetNext()
{
	AILock = false;
	bool Player1ok = false; //Player 1 has at least 1 unit.
	bool Player2ok = false; //Player 2 has at least 1 unit.
	ITER Terminator = Liste.begin();
	ITER Deleter;
	BRect temp; //coordinate of the dead unit

	//Terminator remove dead units from the list and from the screen
	while(Terminator != Liste.end())
	{
		if((*Terminator).Unit->HP < 1) //unit is dead
		{
			if(Terminator == Liste.begin()) Active = Liste.end(); //Active Unit died of poison
			temp = (*Terminator).Unit->Position;
			(*Terminator).DestroyUnit(); //free the memory occupied by this unit
			Deleter = Terminator;			
			Terminator++;
			Liste.erase(Deleter); //erase the unit container from the list
			Invalidate(temp); //erase the dead unit on-screen
		}

		else if((*Terminator).Unit->Player == 1) 
		{
			Player1ok = true;
			Terminator++;
		}
		
		else 
		{
			Player2ok = true;
			Terminator++;
		}
	}

	if(!Player1ok || !Player2ok) //A player has no unit left.
	{
		Restart(); //ready for another exciting game !
		return;
	}

	//take the unit unit that has finished playing and put it at the end of the list
	//that is to make sure that if there is another unit with the same recovery, 
	//it will play first

	if(Active != Liste.end())
	{
		Item temp;
		temp.Unit = (*Active).Unit;
		Liste.pop_front(); //remove it from the begining of the list
		Liste.push_back(temp); //and put it to the end
	}

	Liste.sort(); //sort units by recovery
	Active = Liste.begin(); //new active unit

	//lower the recovery of all units in the list
	int decrementation = (*Active).Unit->Recovery; //bring it to 0
	for(ITER Runner = Liste.begin(); Runner != Liste.end(); Runner++)
		(*Runner).Unit->Recovery -= decrementation;

	//Reset the selected target to none
	Passive = Liste.end(); 
	Target = BRect(0,0,0,0);
	((Application*)be_app)->B->Panel2->Image->ChangePortrait(0);
	((Application*)be_app)->B->Panel2->UpdateStat(0, false, false, false, false);

	//Activating active unit panel with all his info
	((Application*)be_app)->B->Panel1->ChangeBe((*Active).Unit->Type); //his image & buttons

	((Application*)be_app)->B->Panel1->UpdateStat((*Active).Unit->HP, (*Active).Unit->Speed, 
		(*Active).Unit->Slow, (*Active).Unit->Stun, (*Active).Unit->Poison); //stats

	((Application*)be_app)->B->Panel1->PressButton(1);
	((Application*)be_app)->B->Panel2->ShowInfo((*Active).Unit->Type, 1);
	CurrentAction = 1;
	Animating = false; //the preceding animation is over
	Invalidate((*Active).Unit->Position); //highlight the active unit
	Window()->UpdateIfNeeded(); //update now
	
	//AI
	if(AIPlayer && (*Active).Unit->GetPlayer() == 2)
	{
		AILock = true;
		PlayAI();
	}
}
//----------------------------------------------------------------------
void DView::Draw(BRect where) 
{
	/*Offscreen drawing is pretty fast...  We setup an offscreen bitmap where we draw
	the board background and each unit or effect that are in the 'where' rectangle.  
	Then, we copy only this part of the offscreen bitmap to the visible screen*/

	OffscreenBitmap->Lock(); //protect the OffscreenBitmap against intruders
	RedrawBackground(OffscreenView); //draw the background offscreen
	
	//draw all units that intersect 'where' from the end of the list to it's begining. 
	//(so that the active unit is always on top)
	bool highlight;
	RITER runner; //reverse iterator
	for(runner = Liste.rbegin(); runner != Liste.rend(); runner++)
		if((*runner).Unit->Intersect(where))
		{	//don't highlight the active unit if he is moving...
			highlight = ((*runner).Unit == (*Active).Unit) && (!Animating);
			(*runner).Unit->Draw(OffscreenView, highlight); //draw it
		}
		
	if(Target.Intersects(where) && !Animating) 
	{//the spot
		rgb_color temp = OffscreenView->HighColor(); //save current drawing color
		OffscreenView->SetHighColor(SpotColor); //transparent red or green
		OffscreenView->FillRoundRect(Target, 16, 16); //transparent spot
		OffscreenView->SetHighColor(temp); //reset the drawing color
	}
	
	if(SEffect->Valid) //if Special Effect is showing
		if(SEffect->Intersect(where))
			SEffect->Draw(OffscreenView);

	//Draw the invalidated portion of the offscreen bitmap into the onscreen view
	OffscreenView->Sync(); //Synchronize offscreen view with onscreen view
	DrawBitmap(OffscreenBitmap, where, where); //Copy the updated part to screen
	OffscreenBitmap->Unlock(); //Unprotect the Offscreen bitmap
}
//----------------------------------------------------------------------
void DView::RedrawBackground(BView* aView)
{//Redraw the entire background on 'aView'
	aView->DrawBitmap(Background, BRect(0, 0, 447, 447));
}
//----------------------------------------------------------------------
void DView::MessageReceived(BMessage* message)
{//quand on pese sur un bouton
	switch(message->what)
	{//trigger the action
		case BUTTON1: 
		{
			((Application*)be_app)->B->Panel1->PressButton(1);
			if(Mode == Option) Ini(1, true);
			else if(!AILock)
			{
				CurrentAction = 1;
				((Application*)be_app)->B->Panel2->ShowInfo((*Active).Unit->Type, CurrentAction);
			}
		}break;

		case BUTTON2:
		{
			((Application*)be_app)->B->Panel1->PressButton(2);
			if(Mode == Option) Ini(1, false);
			else if(!AILock)
			{
				CurrentAction = 2;
				((Application*)be_app)->B->Panel2->ShowInfo((*Active).Unit->Type, CurrentAction);
			}
		}break;
		
		case BUTTON3:
		{
			((Application*)be_app)->B->Panel1->PressButton(3);
			if(Mode == Option) Ini(2, true);
			else if(!AILock)
			{
				CurrentAction = 3;
				((Application*)be_app)->B->Panel2->ShowInfo((*Active).Unit->Type, CurrentAction);
			}
		}break;
		
		case BUTTON4: 
		{
			((Application*)be_app)->B->Panel1->PressButton(4);
			if(Mode == Option) Ini(2, false);
			else if(!AILock)
			{
				CurrentAction = 4;
				((Application*)be_app)->B->Panel2->ShowInfo((*Active).Unit->Type, CurrentAction);
			}
		}break;
		
		case BUTTON5:
		{
			((Application*)be_app)->B->Panel1->PressButton(5);
			if(Mode == Option) Ini(3, true);
			else if(!AILock)
			{
				CurrentAction = 5;
				((Application*)be_app)->B->Panel2->ShowInfo((*Active).Unit->Type, CurrentAction);
			}
		}break;
		
		case BUTTON6:
		{
			((Application*)be_app)->B->Panel1->PressButton(6);
			if(Mode == Option) Ini(3, false);
			else if(!AILock)
			{
				Animating = true; //we start an animation
				Invalidate(Target); //hide the spot
				Window()->UpdateIfNeeded();

			 	CurrentAction = 6;
				(*Active).Unit->DoAction6((*Passive).Unit, Target);
				GetNext();
			}
		}break;
		
		default: BView::MessageReceived(message);
	}
}
//--------------------------------------------------------------------------------
void DView::MouseMoved(BPoint where, uint32 transit, const BMessage *message)
{
	if((Target.Contains(where) && (transit == B_INSIDE_VIEW)) || !Playing || AILock)
		return; //nothing to update, same square or end game

	Passive = FindClickedOn(where); //check if we have clicked on a unit
	Invalidate(Target); //remove previous selection (previous spot)

	if(Passive != Liste.end()) //it's a unit
	{
		((Application*)be_app)->B->Panel2->Image->ChangePortrait((*Passive).Unit->Type); //target image

		((Application*)be_app)->B->Panel2->UpdateStat((*Passive).Unit->HP, (*Passive).Unit->Speed, 
			(*Passive).Unit->Slow, (*Passive).Unit->Stun, (*Passive).Unit->Poison); //stats

		Target = (*Passive).Unit->Position; //saves the coordinates of the target
	}
		
	else //we clicked on an empty square
	{
		((Application*)be_app)->B->Panel2->Image->ChangePortrait(0); //default image
		((Application*)be_app)->B->Panel2->UpdateStat(0, false, false, false, false); //no stats
		int destX = (int)(where.x / 32); //to get the exact bounds of the destination square
		int destY = (int)(where.y / 32);
		destX*=32;
		destY*=32;
		Target = BRect(destX, destY, destX+31, destY+31); //save coordinates of destination
	}

	switch(CurrentAction)
	{
		case 1:(*Active).Unit->OkTarget1((*Passive).Unit, (Passive != Liste.end()), Target) ? GreenSpot() : RedSpot();break;
		case 2:(*Active).Unit->OkTarget2((*Passive).Unit, (Passive != Liste.end()), Target) ? GreenSpot() : RedSpot();break;
		case 3:(*Active).Unit->OkTarget3((*Passive).Unit, (Passive != Liste.end()), Target) ? GreenSpot() : RedSpot();break;
		case 4:(*Active).Unit->OkTarget4((*Passive).Unit, (Passive != Liste.end()), Target) ? GreenSpot() : RedSpot();break;
		case 5:(*Active).Unit->OkTarget5((*Passive).Unit, (Passive != Liste.end()), Target) ? GreenSpot() : RedSpot();break;
	}

	Invalidate(Target); //draw a new spot
	Window()->UpdateIfNeeded();
}
//--------------------------------------------------------------------------------
void DView::MouseDown(BPoint where) 
{//when we click somewhere on the board, we basically repeat the MouseMoved Code + the action.
	if(!Playing || AILock) return;
	
	int32 buttons;
	Window()->CurrentMessage()->FindInt32("buttons", &buttons);

	if(buttons == B_PRIMARY_MOUSE_BUTTON) //Left-click
	{
		Passive = FindClickedOn(where); //make sure we have the good unit
		Invalidate(Target); //remove previous selection (previous spot)

		if(Passive != Liste.end()) //it's a unit
		{
			((Application*)be_app)->B->Panel2->Image->ChangePortrait((*Passive).Unit->Type); //target image

			((Application*)be_app)->B->Panel2->UpdateStat((*Passive).Unit->HP, (*Passive).Unit->Speed, 
				(*Passive).Unit->Slow, (*Passive).Unit->Stun, (*Passive).Unit->Poison); //stats

			Target = (*Passive).Unit->Position; //saves the coordinates of the target
		}
		
		else //we clicked on an empty square
		{
			((Application*)be_app)->B->Panel2->Image->ChangePortrait(0); //default image
			((Application*)be_app)->B->Panel2->UpdateStat(0, false, false, false, false); //no stats
			int destX = (int)(where.x / 32); //to get the exact bounds of the destination square
			int destY = (int)(where.y / 32);
			destX*=32;
			destY*=32;
			Target = BRect(destX, destY, destX+31, destY+31); //save coordinates of destination
		}
	}

	Animating = true; //we start an animation (maybe not, but don't take a chance)
	Invalidate(Target); //hide the red spot
	Window()->UpdateIfNeeded();

	BRect U = (*Active).Unit->Position; //save unit position
	switch(CurrentAction)
	{
		case 1:
		{
			if((*Active).Unit->OkTarget1((*Passive).Unit, (Passive != Liste.end()), Target))
			{
				(*Active).Unit->DoAction1((*Passive).Unit, Target); 
				(*Active).Unit->ApplyEffect();
				U = (*Active).Unit->Position; //position has changed
				GetNext();
			}
			else (*Active).Unit->Stupid();
		}break;

		case 2:
		{
			if((*Active).Unit->OkTarget2((*Passive).Unit, (Passive != Liste.end()), Target))
			{
				(*Active).Unit->DoAction2((*Passive).Unit, Target);
				(*Active).Unit->ApplyEffect();
				GetNext();
			}
			
			else (*Active).Unit->Stupid();
		}break;

		case 3:
		{
			if((*Active).Unit->OkTarget3((*Passive).Unit, (Passive != Liste.end()), Target))
			{
				(*Active).Unit->DoAction3((*Passive).Unit, Target);
				(*Active).Unit->ApplyEffect();
				GetNext();
			}
			
			else (*Active).Unit->Stupid();
		}break;

		case 4:
		{
			if((*Active).Unit->OkTarget4((*Passive).Unit, (Passive != Liste.end()), Target))
			{
				(*Active).Unit->DoAction4((*Passive).Unit, Target);
				(*Active).Unit->ApplyEffect();
				GetNext();
			}
			
			else (*Active).Unit->Stupid();
		}break;
		
		case 5:
		{
			if((*Active).Unit->OkTarget5((*Passive).Unit, (Passive != Liste.end()), Target))
			{
				(*Active).Unit->DoAction5((*Passive).Unit, Target);
				(*Active).Unit->ApplyEffect();
				GetNext();
			}
			
			else (*Active).Unit->Stupid();
		}break;
	}
	Animating = false; //end of animation
	Invalidate(Target);
	Invalidate(U);
}
//--------------------------------------------------------------------------------
ITER DView::FindClickedOn(BPoint where) 
{
	ITER select = Liste.begin();
	while(select != Liste.end())
	{
		if((*select).Unit->Contains(where))		
			return select; //we clicked on this unit

		select++; //check next next unit
	}

	return select; //we clicked elsewhere
}
//--------------------------------------------------------------------------------
void DView::RedSpot()
{
	SpotColor.red = 255;
	SpotColor.green = 0;
	SpotColor.blue = 0;
	SpotColor.alpha = 64;
}
//--------------------------------------------------------------------------------
void DView::GreenSpot()
{
	SpotColor.red = 0;
	SpotColor.green = 255;
	SpotColor.blue = 0;
	SpotColor.alpha = 64;
}
//--------------------------------------------------------------------------------



