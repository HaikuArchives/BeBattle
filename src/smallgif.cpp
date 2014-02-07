#include "smallgif.h"
#include "convert.h"
//----------------------------------------------------------------------
SmallGif::SmallGif(BRect dest, BBitmap* i) 
	: BView(dest, "SmallGif", B_FOLLOW_NONE, B_WILL_DRAW)
{
	Image = i;
	MakeTransparent(Image);
	Invalidate();
}
//----------------------------------------------------------------------
void SmallGif::Draw(BRect R)
{
	DrawBitmap(Image);
}
//----------------------------------------------------------------------
