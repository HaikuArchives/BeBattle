#include "effect.h"
#include <Application.h>
#include "main.h"
//---------------------------------------------------------------------
bool Effect::Contains(BPoint p) 
{
	return Position.Contains(p);
}
//---------------------------------------------------------------------
bool Effect::Intersect(BRect where) 
{
	return Position.Intersects(where);
}
//---------------------------------------------------------------------
void Effect::Draw(BView* aView)
{
	if(Valid) aView->DrawBitmap(CurrentImage, Position);
}
//--------------------------------------------------------------------
void Effect::Update(BRect old)
{
	((Application*)be_app)->B->Damier->Invalidate(old); //efface 
	((Application*)be_app)->B->Damier->Invalidate(Position); //redessine
	((Application*)be_app)->B->UpdateIfNeeded(); //NOW !!!
}
