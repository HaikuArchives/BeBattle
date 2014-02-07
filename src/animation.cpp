#include "animation.h"
#include <TranslationUtils.h>
#include "convert.h"
//-------------------------------------------------------------------
TAnimation::TAnimation()
{
	BitmapKit[0] = BTranslationUtils::GetBitmap("bmp/animation/1.png"); 
	BitmapKit[1] = BTranslationUtils::GetBitmap("bmp/animation/2.png"); 
	BitmapKit[2] = BTranslationUtils::GetBitmap("bmp/animation/3.png"); 
	BitmapKit[3] = BTranslationUtils::GetBitmap("bmp/animation/4.png"); 
	BitmapKit[4] = BTranslationUtils::GetBitmap("bmp/animation/5.png");
	BitmapKit[5] = BTranslationUtils::GetBitmap("bmp/misc/number.png");

	//met de la transparence
	for(int B=0; B<6; B++)
		MakeTransparent(BitmapKit[B]);
}
//-------------------------------------------------------------------
BBitmap* TAnimation::GetAnim(int unit, int anim, int frame)
{
	unit--; //unit = type - 1
	anim*=32;
	frame*=32;
	BBitmap* Target = new BBitmap(BRect(0,0,31,31), B_RGB32);

	int width = (int)BitmapKit[unit]->Bounds().right+1;

	for(int row = 0; row < 32; row++)
		for(int col = 0; col < 32; col++)
		{
			((rgb_color *)Target->Bits())[(row*32 + col)] = 
			((rgb_color *)BitmapKit[unit]->Bits())[((frame+row)*width + col+anim)];
		}			

	return Target;
}
//-------------------------------------------------------------------
