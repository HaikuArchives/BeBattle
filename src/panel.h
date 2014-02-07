#ifndef PANEL_H
#define PANEL_H

#include <Window.h> 
#include <Button.h>
#include <PictureButton.h>
#include <Box.h>
#include <StatusBar.h>
#include <StringView.h>
#include "portrait.h"
#include "smallgif.h"

class ActivePanel : public BBox
{
 private:
	BPicture* Action[6][12]; //8 button set (1 set = 6 'on' and 6 'off' images )
	BBitmap* Buttons;
	BWindow* TempWindow; //used to create the BPictures
	BView *tempView; //also used to create BPictures

	BPictureButton *Button1;
	BPictureButton *Button2;
	BPictureButton *Button3;
	BPictureButton *Button4;
	BPictureButton *Button5;
	BPictureButton *Button6;
	BButton* AboutButton;
	BButton* RestartButton;
	BStatusBar* HPBar; //HP bar
	SmallGif* Heart;
	SmallGif* Speed;
	SmallGif* Slow;
	SmallGif* Stun;
	SmallGif* Poison;
	bool ShowSpeed;
	bool ShowSlow;
	bool ShowStun;
	bool ShowPoison;

	Portrait *Image; //of the current active unit
	int CurrentButton;
	
	void LoadPicture(int, int, int, int); //Extract a 32x32 bitmap from a bigger bitmap
	void RefreshButton(); //Refresh each Buttons

 public:
	ActivePanel();
	void ChangeBe(int i); //change the button set and image
	void UpdateStat(int HP, bool speed, bool slow, bool stun, bool poison);
	void PressButton(int B);
};
//-----------------------------------------------------------------
class PassivePanel : public BBox
{ //PassivePanel = ActivePanel - ButtonSet
 private:
	BStatusBar* HPBar;
	BStringView* Line1;
	BStringView* Line2;
	BStringView* Line3;
	BStringView* Line4;
	SmallGif* Heart;
	SmallGif* Speed;
	SmallGif* Slow;
	SmallGif* Stun;
	SmallGif* Poison;
	bool ShowSpeed;
	bool ShowSlow;
	bool ShowStun;
	bool ShowPoison;

 public:
	Portrait *Image;	
	PassivePanel();
	void UpdateStat(int HP, bool speed, bool slow, bool stun, bool poison);
	void ShowInfo(int unit, int action);
};
//-----------------------------------------------------------------
#endif