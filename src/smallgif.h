#ifndef SMALLGIF_H
#define SMALLGIF_H
#include <Bitmap.h>
#include <View.h>

/*This project was started before Unisys gif-related
announcement.  Now, this class should be named SmallPng.*/

class SmallGif : public BView
{
	BBitmap* Image; //private

 public:
	SmallGif(BRect dest, BBitmap* i); 
	~SmallGif() {delete Image;}
	virtual void Draw(BRect);
};

#endif