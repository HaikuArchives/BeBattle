#ifndef EFFECT_H
#define EFFECT_H

#include <View.h>
#include <Bitmap.h>
#include <Rect.h>
//---------------------------------------------------------------------
class Effect
{
 public :
	BRect Position;
	BBitmap* CurrentImage;
	bool Valid;

	Effect() {Position = BRect(0,0,0,0); CurrentImage = NULL; Valid=false;}
	void Update(BRect old);
	void Draw(BView*);
	bool Contains(BPoint p);
	bool Intersect(BRect where); 
};
//---------------------------------------------------------------------
#endif