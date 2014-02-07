#ifndef ANIMATION_H
#define ANIMATION_H

#include <Bitmap.h>

class TAnimation
{
 private:
	BBitmap* BitmapKit[6];

 public:
	TAnimation();
	BBitmap* GetAnim(int unit, int anim, int frame);
};
//-----------------------------------------------------------------
#endif