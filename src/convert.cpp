#include "convert.h"
void MakeTransparent(BBitmap* Image)
{
	//met de la transparence
	rgb_color c;
	int height, width;
	width = (int)Image->Bounds().right+1;
	height = (int)Image->Bounds().bottom+1;

	for(int row = 0; row < height; row++)
		for(int col = 0; col < width; col++)
		{
			c = ((rgb_color *)Image->Bits())[(row*width + col)];
			if(c.red == 255 && c.green == 0 && c.blue == 255)
				((rgb_color *)Image->Bits())[(row*width + col)].alpha = 0;
			else
				((rgb_color *)Image->Bits())[(row*width + col)].alpha = 255;
		}
}
