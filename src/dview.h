#ifndef DVIEW_H
#define DVIEW_H
#include <View.h>
#include <Bitmap.h>
#include <Point.h>
#include <list> //list from Standard Template Library
#include "beunit.h"
#include "effect.h"
#include "animation.h"
#include "convert.h"
//---------------------------------------------------------------------
typedef	std::list<Item>::iterator ITER;
typedef	std::list<Item>::reverse_iterator RITER;
enum BUTTONMODE{Option, Unit};
//---------------------------------------------------------------------
class DView : public BView
{
 private:
	BBitmap* Background; //the board background (448x448)
	BBitmap* OffscreenBitmap; //to avoid flicker while drawing
	BView* OffscreenView; //used by the offscreen bitmap
	
	std::list<Item> Liste; //All units are in this list
	ITER Active; //iterator to active unit
	ITER Passive; //iterator to current target (passive unit)
	BUTTONMODE Mode; //current button set
	BRect Target; //destination coordinate
	bool Animating; //true if we are currently animating something
	bool Playing; //Currently playing the game
	bool AIPlayer; //is the cpu playing against the player ?
	bool AILock; //Computer turn.
	int CurrentAction;
	rgb_color SpotColor;
		
	void Ini(int group, bool CPU); //Initialisation before the game begins
	void GetNext(); //Next turn (active the next unit in the list)
	void RedrawBackground(BView*); //redraw the entire background
	ITER FindClickedOn(BPoint where); //find what the user clicked on
	void RedSpot();
	void GreenSpot();

	/* Those AI related functions are in 'ai.cpp' */
	void PlayAI();
	void AI1(); //play using the normal guy AI
	void AI2(); //play using the boxer AI
	void AI3(); //play using magician AI
	void AI4(); //play using the medic AI
	void AI5(); //play using the small one AI
	bool GetAround(BRect self, BRect target, BRect& result); //find a free spot near target
	bool Occupied(BRect target);
	
 public:
	TAnimation* Animation; //the animations (available to everyone)
	Effect* SEffect; //the special fx (available to everyone)

	DView(); 
	void Restart();
	virtual void Draw(BRect where);
	virtual void MessageReceived(BMessage* message);
	virtual void MouseDown(BPoint where);
	virtual void MouseMoved(BPoint where, uint32 transit, const BMessage *message);
};
//---------------------------------------------------------------------
#endif
