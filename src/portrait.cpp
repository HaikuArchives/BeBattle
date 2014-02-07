#include "portrait.h"
#include <TranslationUtils.h>
#include "convert.h"
//----------------------------------------------------------------------
Portrait::Portrait(BRect dest) 
	: BView(dest, "Image", B_FOLLOW_NONE, B_WILL_DRAW)
{
	//load les portraits
	AllPortrait = BTranslationUtils::GetBitmap("bmp/misc/portrait.png"); 
	MakeTransparent(AllPortrait);

	Portraits[0] = Fetch(0);
	Portraits[1] = Fetch(1);
	Portraits[2] = Fetch(2);
	Portraits[3] = Fetch(3);
	Portraits[4] = Fetch(4);
	Portraits[5] = Fetch(5);
	Portraits[6] = Fetch(6);
	Portraits[7] = Fetch(7);
	ChangePortrait(0); //image par default
}
//----------------------------------------------------------------------
BBitmap* Portrait::Fetch(int index)
{
	index *= 64;
	BBitmap* Target = new BBitmap(BRect(0,0,63,63), B_RGB32);

	int width = (int)AllPortrait->Bounds().right+1;

	for(int row = 0; row < 64; row++)
		for(int col = 0; col < 64; col++)
		{
			((rgb_color *)Target->Bits())[(row*64 + col)] = 
			((rgb_color *)AllPortrait->Bits())[(row*width + col+index)];
		}			

	return Target;
}
//----------------------------------------------------------------------
void Portrait::Draw(BRect R)
{
	DrawBitmap(Image);
}
//----------------------------------------------------------------------
void Portrait::ChangePortrait(int i)
{
	Image = Portraits[i]; //nouvelle image
	Invalidate();
}
//----------------------------------------------------------------------
