#include "beunit.h"
#include <Application.h>
#include "main.h"
#define SPEED 50000 //0,05 sec
//---------------------------------------------------------------------
BeUnit::BeUnit()
{
}
//---------------------------------------------------------------------
bool BeUnit::Contains(BPoint p) 
{
	return Position.Contains(p);
}
//---------------------------------------------------------------------
bool BeUnit::Intersect(BRect where) 
{
	return Position.Intersects(where);
}
//---------------------------------------------------------------------
void BeUnit::Draw(BView* aView, bool highlight)
{
	aView->DrawBitmap(CurrentImage, Position);
	if(highlight) 
	{//the transparent white square
		aView->FillRoundRect(Position, 5, 5);
		rgb_color temp = aView->HighColor();
		aView->SetHighColor(0,0,0,255);
		aView->StrokeRoundRect(Position, 5, 5);
		aView->SetHighColor(temp);
	}
}
//--------------------------------------------------------------------
void BeUnit::Update(BRect old)
{//erase old frame and draw the new one
	((Application*)be_app)->B->Damier->Invalidate(old); //erase
	((Application*)be_app)->B->Damier->Invalidate(Position); //redraw
	((Application*)be_app)->B->UpdateIfNeeded(); //NOW !!!
}
//---------------------------------------------------------------------
void BeUnit::SetHP(int mod)
{//Show the dammage on-screen
	//save current effect (ex :flying head)
	bool valid = SEffect()->Valid;
	BBitmap* currentEffect = SEffect()->CurrentImage;
	BRect position = SEffect()->Position;

	int row = mod > 0 ? 1 : 0;
	SEffect()->Position = Position;
	SEffect()->CurrentImage = GetAnim(6, abs(mod)-1, row, PlayerColor);
	SEffect()->Valid = true;
	BRect Invalidator;

	for(int up=0; up < 4; up++)
	{
		Invalidator = SEffect()->Position;
		SEffect()->Position.OffsetBy(0,-4);
		SEffect()->Update(Invalidator);
		snooze((int)(SPEED*0.3));
	}
	
	for(int down=0; down < 4; down++)
	{
		Invalidator = SEffect()->Position;
		SEffect()->Position.OffsetBy(0,4);
		SEffect()->Update(Invalidator);
		snooze((int)(SPEED*0.3));
	}
	
	snooze((int)(SPEED*4));
	SEffect()->Valid = false;
	SEffect()->Update(SEffect()->Position); 
	delete SEffect()->CurrentImage;

	//put the old effect in place
	SEffect()->Valid = valid;
	SEffect()->CurrentImage = currentEffect;
	SEffect()->Position = position;

	HP += mod; 
	if(HP > 10) HP=10;
	if(HP < 1) Kill();
}
//---------------------------------------------------------------------
int BeUnit::Distance(BRect a, BRect b)
{
	int ah = (int)(a.left/32);
	int bh = (int)(b.left/32);
	
	int av = (int)(a.top/32);
	int bv = (int)(b.top/32);
	
	return MAX(abs(ah-bh), abs(av-bv));
}
//---------------------------------------------------------------------
Effect* BeUnit::SEffect()
{
	return ((Application*)be_app)->B->Damier->SEffect; 
}
//---------------------------------------------------------------------
void BeUnit::ApplyEffect()
{//apply effect on units and tell if it can play or not
	if(Speed) Recovery -= 2;
	if(Slow)  Recovery += 2;
	if(Poison)SetHP(-2);

	if(Stun) Stun = false; //UnStun on next turn
}
//---------------------------------------------------------------------
BBitmap* BeUnit::GetAnim(int unit, int col, int row, rgb_color color)
{
	/*Load an image.  It will open 'unit'.png and fetch the image at the 
	specified colomn and row (32x32 images) and paint it to the player color*/
	BBitmap* temp = ((Application*)be_app)->B->Damier->Animation->GetAnim(unit, col, row);

	rgb_color c;
	for(int i = 0; i < 32; i++)
		for(int j = 0; j < 32; j++)
		{
			c = ((rgb_color *)temp->Bits())[(i*32 + j)];
			if(c.red == 128 && c.green == 128 && c.blue == 128)
				((rgb_color *)temp->Bits())[(i*32 + j)] = color;
		}

	return temp; 
}
//-----------------------------------------------------------------
BBitmap* BeUnit::GetAnimFlip(int unit, int anim, int frame, rgb_color color)
{
	BBitmap* temp = ((Application*)be_app)->B->Damier->Animation->GetAnim(unit, anim, frame);

	rgb_color c;
	for(int i = 0; i < 32; i++)
		for(int j = 0; j < 32; j++)
		{
			c = ((rgb_color *)temp->Bits())[(i*32 + j)];
			if(c.red == 128 && c.green == 128 && c.blue == 128)
				((rgb_color *)temp->Bits())[(i*32 + j)] = color;
		}

	//Flip the bitmap sideways
	BBitmap* flipped = new BBitmap(BRect(0,0,31,31), B_RGB32); 

	for(int colonne = 0; colonne < 32; colonne++)
		for(int rangee = 0; rangee <32; rangee++)
		{
			((rgb_color *)flipped->Bits())[(rangee*32 + colonne)] = 
				((rgb_color *)temp->Bits())[(rangee*32 + 31 - colonne)];
		}

	delete temp;
	return flipped; 
}
//-----------------------------------------------------------------
