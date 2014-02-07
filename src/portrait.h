#ifndef PORTRAIT_H
#define PORTRAIT_H
#include <Bitmap.h>
#include <View.h>

class Portrait : public BView
{
	BBitmap* Portraits[8];
	BBitmap* AllPortrait;

 public:
	BBitmap* Image; //private

	Portrait(BRect dest); 
	virtual void Draw(BRect);
	void ChangePortrait(int);
	BBitmap* Fetch(int);
};

#endif