#ifndef BE5_H
#define BE5_H

#include <Bitmap.h>
#include <View.h>
#include "beunit.h"

class Be5 : public BeUnit
{
 public :
	Be5(int w, int p);
	void DoAction1(BeUnit*, BRect);
	void DoAction2(BeUnit*, BRect);
	void DoAction3(BeUnit*, BRect);
	void DoAction4(BeUnit*, BRect);
	void DoAction5(BeUnit*, BRect);
	void DoAction6(BeUnit*, BRect);

	bool OkTarget1(BeUnit*, bool, BRect);
	bool OkTarget2(BeUnit*, bool, BRect);
	bool OkTarget3(BeUnit*, bool, BRect);
	bool OkTarget4(BeUnit*, bool, BRect);
	bool OkTarget5(BeUnit*, bool, BRect);

	void Hit(bool);
	void Stupid();
	void Kill();
};

#endif