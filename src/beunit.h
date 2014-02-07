#ifndef BEUNIT_H
#define BEUNIT_H
#include <View.h>
#include <Bitmap.h>
#include <Rect.h>
#include "effect.h"
#include <stdlib.h>
#include "convert.h"
//---------------------------------------------------------------------
class DView; //temp
//---------------------------------------------------------------------
class BeUnit
{
 protected :
	friend struct Item;
	friend class DView;
	int HP; //Hit point of this unit
	int Recovery; //time before he play again
	int Type; //which kind of unit
	int Player; //Owner of the unit
	BRect Position; //Current position on the board
	bool Speed;
	bool Slow;
	bool Stun;
	bool Poison;

	void Update(BRect old); //Redraw the unit (and erase previous frame)
	Effect* SEffect(); //Allow access to special effects

	//load a bitmap
	BBitmap* GetAnim(int unit, int anim, int frame, rgb_color color);
	
	//load a bitmap (flipped verticaly)
	BBitmap* GetAnimFlip(int unit, int anim, int frame, rgb_color color);

 public :
	rgb_color PlayerColor; //Color of the player
	BBitmap* CurrentImage; //Current Frame

	BeUnit();
	virtual ~BeUnit() {delete CurrentImage;}
	void Draw(BView*, bool);
	bool Contains(BPoint p);
	bool Intersect(BRect where); 
	BRect GetPosition() {return Position;}
	int GetType() {return Type;}
	int GetPlayer() {return Player;}
	void SetPlayer(int P) {Player = P;}
	int GetHP() {return HP;}
	void SetHP(int newhp);

	virtual void DoAction1(BeUnit*, BRect) = 0;
	virtual void DoAction2(BeUnit*, BRect) = 0;
	virtual void DoAction3(BeUnit*, BRect) = 0;
	virtual void DoAction4(BeUnit*, BRect) = 0;
	virtual void DoAction5(BeUnit*, BRect) = 0;
	virtual void DoAction6(BeUnit*, BRect) = 0;

	virtual bool OkTarget1(BeUnit*, bool, BRect) = 0;
	virtual bool OkTarget2(BeUnit*, bool, BRect) = 0;
	virtual bool OkTarget3(BeUnit*, bool, BRect) = 0;
	virtual bool OkTarget4(BeUnit*, bool, BRect) = 0;
	virtual bool OkTarget5(BeUnit*, bool, BRect) = 0;

	virtual void Hit(bool) = 0;
	virtual void Stupid() = 0;
	virtual void Kill() = 0;
	void ApplyEffect();

	void CastSpeed() {Speed = true;}
	void CastSlow() {Slow = true;}
	void CastStun() {Stun = true; Recovery += 20;}
	void CastPoison() {Poison = true;}
	bool IsSpeed() {return Speed;}
	bool IsSlow() {return Slow;}
	bool IsStun() {return Stun;}
	bool IsPoison() {return Poison;}
	void AddRecovery() {Recovery++;}

	void CastRemove() {Speed = false; Slow = false; Stun = false; Poison = false; Recovery = 3;}
	int Distance(BRect, BRect);
};
//---------------------------------------------------------------------
struct Item
{/*Each unit is in a container like this.  It allows me to put
different units in a same list*/
	BeUnit* Unit;
	
	bool operator<(const Item& I) const
	{return Unit->Recovery < I.Unit->Recovery;}
	
	void DestroyUnit()
	{delete Unit;}
};
//---------------------------------------------------------------------
#include "be1.h" //the normal "looking" guy
#include "be2.h" //the violent boxer
#include "be3.h" //the magician
#include "be4.h" //the mysctic
#include "be5.h" //the "don't underestimate me" unit
#endif