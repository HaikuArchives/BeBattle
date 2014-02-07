#include "panel.h"
#include <Bitmap.h>
#include <Button.h>
#include <TranslationUtils.h>
#include <String.h>
#include "enum.h"
#include "convert.h"
//---------------------------------------------------------------------------------------
ActivePanel::ActivePanel() : BBox(BRect(448,0,583,297))
{
	Image = new Portrait(BRect(4, 4, 67, 67));
	AddChild(Image);

	Heart = new SmallGif(BRect(4, 68, 19, 83), BTranslationUtils::GetBitmap("bmp/misc/heart.png"));
	AddChild(Heart);

	Speed = new SmallGif(BRect(70, 4, 85, 19), BTranslationUtils::GetBitmap("bmp/misc/speed.png"));
	AddChild(Speed);

	Slow = new SmallGif(BRect(70, 20, 85, 35), BTranslationUtils::GetBitmap("bmp/misc/slow.png"));
	AddChild(Slow);

	Stun = new SmallGif(BRect(70, 36, 85, 51), BTranslationUtils::GetBitmap("bmp/misc/stun.png"));
	AddChild(Stun);

	Poison = new SmallGif(BRect(70, 52, 85, 67), BTranslationUtils::GetBitmap("bmp/misc/poison.png"));
	AddChild(Poison);

	ShowSpeed = true;
	ShowSlow = true;
	ShowStun = true;
	ShowPoison = true;

	HPBar = new BStatusBar(BRect(20, 56, 131, 0), "HP");
	HPBar->SetMaxValue(10);
	AddChild(HPBar);

	TempWindow = new BWindow(BRect(0, 0, 63, 63), "TempWindow", B_TITLED_WINDOW, B_NOT_RESIZABLE);
	
	Buttons = BTranslationUtils::GetBitmap("bmp/misc/button.png");
	MakeTransparent(Buttons);

	//Load button picture
	//OPTION
	LoadPicture(0,0,0,4); //5 vs 5
	LoadPicture(0,2,1,4); //10 vs 10
	LoadPicture(0,4,2,4); //15 vs 15
	LoadPicture(0,6,3,4); //20 vs 20
	LoadPicture(0,8,4,4); //25 vs 25
	LoadPicture(0,10,5,4);//30 vs 30

	//Unit 1
	LoadPicture(1,0,0,1); //Move
	LoadPicture(1,2,1,3); //Punch
	LoadPicture(1,4,3,2); //Throw Head
	LoadPicture(1,6,4,2); //Arm-Smash
	LoadPicture(1,8,5,2); //Self-Heal
	LoadPicture(1,10,5,1);//do nothing

	//Unit 2
	LoadPicture(2,0,0,1); //Move
	LoadPicture(2,2,1,0); //Punch
	LoadPicture(2,4,0,3); //Punch Fury
	LoadPicture(2,6,2,2); //Shotgun
	LoadPicture(2,8,2,0); //Slow
	LoadPicture(2,10,5,1);//do nothing

	//Unit 3
	LoadPicture(3,0,0,1); //Move
	LoadPicture(3,2,1,2); //Zap
	LoadPicture(3,4,0,2); //Fireball
	LoadPicture(3,6,3,0); //Poison
	LoadPicture(3,8,4,0); //Color change
	LoadPicture(3,10,5,1);//do nothing

	//Unit 4
	LoadPicture(4,0,0,1); //Move
	LoadPicture(4,2,2,1); //3d Attack
	LoadPicture(4,4,1,1); //Heal
	LoadPicture(4,6,2,3); //Remove Effect
	LoadPicture(4,8,4,3); //Drain
	LoadPicture(4,10,5,1);//do nothing

	//Unit 5
	LoadPicture(5,0,0,1); //Move
	LoadPicture(5,2,5,0); //Club Neanderthal
	LoadPicture(5,4,3,3); //Bazooka
	LoadPicture(5,6,3,1); //Dynamite
	LoadPicture(5,8,4,1); //Fast
	LoadPicture(5,10,5,1);//do nothing

	//the 6 action buttons
	Button1 = new BPictureButton(BRect(4, 100, 68, 163), "", 
		Action[0][0], Action[0][1], new BMessage(BUTTON1), B_TWO_STATE_BUTTON);

	Button2 = new BPictureButton(BRect(69, 100, 132, 163), "", 
		Action[0][2], Action[0][3], new BMessage(BUTTON2), B_TWO_STATE_BUTTON);

	Button3 = new BPictureButton(BRect(4, 165, 68, 228), "", 
		Action[0][4], Action[0][5], new BMessage(BUTTON3), B_TWO_STATE_BUTTON);

	Button4 = new BPictureButton(BRect(69, 165, 132, 228), "", 
		Action[0][6], Action[0][7], new BMessage(BUTTON4), B_TWO_STATE_BUTTON);

	Button5 = new BPictureButton(BRect(4, 230, 68, 293), "", 
		Action[0][8], Action[0][9], new BMessage(BUTTON5), B_TWO_STATE_BUTTON);

	Button6 = new BPictureButton(BRect(69, 230, 132, 293), "", 
		Action[0][10], Action[0][11], new BMessage(BUTTON6), B_TWO_STATE_BUTTON);

	AddChild(Button1);
	AddChild(Button2);
	AddChild(Button3);
	AddChild(Button4);
	AddChild(Button5);
	AddChild(Button6);

	delete Buttons; //don't need that

	AboutButton = new BButton(BRect(85, 5, 132, 5), "", "About", new BMessage(ABOUT));
	AddChild(AboutButton);

	RestartButton = new BButton(BRect(85, 30, 132, 30), "", "Restart", new BMessage(RESTART));
	AddChild(RestartButton);

	TempWindow->Quit(); //Quit the offscreen window
}
//---------------------------------------------------------------------------------------
void ActivePanel::LoadPicture(int unit, int action, int colonne, int rangee)
{//Extract a 32x32 bitmap from a bigger bitmap
//Express French course: colonne=colomn and rangee=row
	int C = colonne * 64;
	int R = rangee * 64;

	BBitmap* Target = new BBitmap(BRect(0,0,63,63), B_RGB32);
	BBitmap* FadeBitmap = new BBitmap(BRect(0,0,63,63), B_RGB32);

	int width = (int)Buttons->Bounds().right+1;
	for(int row = 0; row < 64; row++)
		for(int col = 0; col < 64; col++)
		{
			((rgb_color *)Target->Bits())[(row*64 + col)] = 
			((rgb_color *)Buttons->Bits())[((R+row)*width + col+C)];
		}			

	tempView = new BView(BRect(0,0, 63, 63), "temp", B_FOLLOW_NONE, B_WILL_DRAW );
	tempView->SetDrawingMode(B_OP_ALPHA);
	tempView->SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
	tempView->SetViewColor(B_TRANSPARENT_32_BIT); 
	TempWindow->AddChild(tempView);

   	tempView->BeginPicture(new BPicture); 
   	tempView->DrawBitmap(Target);
   	Action[unit][action] = tempView->EndPicture();

	//Darker image (when the button is pressed)
	rgb_color fade;
	fade.red=0; fade.green=0; fade.blue=0; fade.alpha=128;
	for(int row = 0; row < 64; row++)
		for(int col = 0; col < 64; col++)
			((rgb_color *)FadeBitmap->Bits())[(row*64 + col)] = fade; 

   	tempView->BeginPicture(new BPicture); 
   	tempView->DrawBitmap(Target);
   	tempView->DrawBitmap(FadeBitmap);
   	Action[unit][action+1] = tempView->EndPicture();

	TempWindow->RemoveChild(tempView);
	delete tempView; //delete the view
	delete Target; //delete the bitmap
}
//---------------------------------------------------------------------------------------
void ActivePanel::ChangeBe(int i)
{//change image + bouton set
	Image->ChangePortrait(i);
	Button1->SetEnabledOff(Action[i][0]);
	Button1->SetEnabledOn(Action[i][1]);
	Button2->SetEnabledOff(Action[i][2]);
	Button2->SetEnabledOn(Action[i][3]);
	Button3->SetEnabledOff(Action[i][4]);
	Button3->SetEnabledOn(Action[i][5]);
	Button4->SetEnabledOff(Action[i][6]);
	Button4->SetEnabledOn(Action[i][7]);
	Button5->SetEnabledOff(Action[i][8]);
	Button5->SetEnabledOn(Action[i][9]);
	Button6->SetEnabledOff(Action[i][10]);
	Button6->SetEnabledOn(Action[i][11]);
	RefreshButton();
}
//---------------------------------------------------------------------------------------
void ActivePanel::RefreshButton()
{
	Button1->Invalidate();
	Button2->Invalidate();
	Button3->Invalidate();
	Button4->Invalidate();
	Button5->Invalidate();
	Button6->Invalidate();
}
//---------------------------------------------------------------------------------------
void ActivePanel::UpdateStat(int HP, bool speed, bool slow, bool stun, bool poison)
{
	if(speed && !ShowSpeed)		{Speed->Show();  ShowSpeed = true;}
	if(!speed && ShowSpeed)		{Speed->Hide();  ShowSpeed = false;}
	if(slow && !ShowSlow)		{Slow->Show();   ShowSlow = true;}
	if(!slow && ShowSlow)		{Slow->Hide();   ShowSlow = false;}
	if(stun && !ShowStun)		{Stun->Show();   ShowStun = true;}
	if(!stun && ShowStun)		{Stun->Hide(); 	 ShowStun = false;}
	if(poison && !ShowPoison)	{Poison->Show(); ShowPoison = true;}
	if(!poison && ShowPoison) 	{Poison->Hide(); ShowPoison = false;}

	BString temp;
	temp << ((uint32)HP);
	HPBar->SetTrailingText(temp.String());
	HPBar->Update(HP - (int)HPBar->CurrentValue());
}
//---------------------------------------------------------------------------------------
void ActivePanel::PressButton(int B)
{//desactivate the last button and activate 'B'
	switch(CurrentButton)
	{
		case 1: {Button1->SetValue(B_CONTROL_OFF); Button1->Invalidate();}break;
		case 2: {Button2->SetValue(B_CONTROL_OFF); Button2->Invalidate();}break;
		case 3: {Button3->SetValue(B_CONTROL_OFF); Button3->Invalidate();}break;
		case 4: {Button4->SetValue(B_CONTROL_OFF); Button4->Invalidate();}break;
		case 5: {Button5->SetValue(B_CONTROL_OFF); Button5->Invalidate();}break;
		case 6: {Button6->SetValue(B_CONTROL_OFF); Button6->Invalidate();}break;
	}

	switch(B)
	{
		case 1: {Button1->SetValue(B_CONTROL_ON); Button1->Invalidate();}break;
		case 2: {Button2->SetValue(B_CONTROL_ON); Button1->Invalidate();}break;
		case 3: {Button3->SetValue(B_CONTROL_ON); Button1->Invalidate();}break;
		case 4: {Button4->SetValue(B_CONTROL_ON); Button1->Invalidate();}break;
		case 5: {Button5->SetValue(B_CONTROL_ON); Button1->Invalidate();}break;
		case 6: {Button6->SetValue(B_CONTROL_ON); Button1->Invalidate();}break;
	}
	
	CurrentButton = B;
}
//---------------------------------------------------------------------------------------
PassivePanel::PassivePanel() : BBox(BRect(448,298,583,447))
{
	Image = new Portrait(BRect(4, 4, 67, 67));
	AddChild(Image);
	
	Heart = new SmallGif(BRect(4, 68, 19, 83), BTranslationUtils::GetBitmap("bmp/misc/heart.png"));
	AddChild(Heart);

	Speed = new SmallGif(BRect(70, 4, 85, 19), BTranslationUtils::GetBitmap("bmp/misc/speed.png"));
	AddChild(Speed);

	Slow = new SmallGif(BRect(70, 20, 85, 35), BTranslationUtils::GetBitmap("bmp/misc/slow.png"));
	AddChild(Slow);

	Stun = new SmallGif(BRect(70, 36, 85, 51), BTranslationUtils::GetBitmap("bmp/misc/stun.png"));
	AddChild(Stun);

	Poison = new SmallGif(BRect(70, 52, 85, 67), BTranslationUtils::GetBitmap("bmp/misc/poison.png"));
	AddChild(Poison);

	ShowSpeed = true;
	ShowSlow = true;
	ShowStun = true;
	ShowPoison = true;

	HPBar = new BStatusBar(BRect(20, 56, 131, 0), "HP");
	HPBar->SetMaxValue(10);
	AddChild(HPBar);

	Line1 = new BStringView(BRect(5, 85, 128, 100), "", "");
	AddChild(Line1);

	Line2 = new BStringView(BRect(5, 100, 128, 115), "", "");
	AddChild(Line2);

	Line3 = new BStringView(BRect(5, 115, 128, 130), "", "");
	AddChild(Line3);

	Line4 = new BStringView(BRect(5, 130, 128, 145), "", "");
	AddChild(Line4);
}
//---------------------------------------------------------------------------------------
void PassivePanel::UpdateStat(int HP, bool speed, bool slow, bool stun, bool poison)
{
	if(speed && !ShowSpeed)		{Speed->Show();  ShowSpeed = true;}
	if(!speed && ShowSpeed)		{Speed->Hide();  ShowSpeed = false;}
	if(slow && !ShowSlow)		{Slow->Show();   ShowSlow = true;}
	if(!slow && ShowSlow)		{Slow->Hide();   ShowSlow = false;}
	if(stun && !ShowStun)		{Stun->Show();   ShowStun = true;}
	if(!stun && ShowStun)		{Stun->Hide(); 	 ShowStun = false;}
	if(poison && !ShowPoison)	{Poison->Show(); ShowPoison = true;}
	if(!poison && ShowPoison) 	{Poison->Hide(); ShowPoison = false;}

	BString temp;
	temp << ((uint32)HP);
	HPBar->SetTrailingText(temp.String());
	HPBar->Update(HP - (int)HPBar->CurrentValue());
}
//---------------------------------------------------------------------------------------
void PassivePanel::ShowInfo(int unit, int action)
{//info about current selected move
	switch(unit) 
	{
		case 1:
		switch(action)
		{
			case 1:
			{
				Line1->SetText("MOVE");
				Line2->SetText("Range: Medium");
				Line3->SetText("Recovery: Short");
				Line4->SetText("");
			}break;

			case 2:
			{
				Line1->SetText("PUNCH");
				Line2->SetText("Range: Short");
				Line3->SetText("Recovery: Short");
				Line4->SetText("Damage: 2 HP");
			}break;

			case 3:
			{
				Line1->SetText("THROW HEAD");
				Line2->SetText("Range: Long");
				Line3->SetText("Recovery: Long");
				Line4->SetText("Damage: 3-5 HP");
			}break;

			case 4:
			{
				Line1->SetText("ARM SMASH");
				Line2->SetText("Range: Short");
				Line3->SetText("Recovery: Medium");
				Line4->SetText("Damage:1-4HP & Stun(1/2)");
			}break;

			case 5:
			{
				Line1->SetText("SELF-HEAL");
				Line2->SetText("Range: Self");
				Line3->SetText("Recovery: Medium");
				Line4->SetText("Heal: +5 HP");
			}break;
		}break;

		case 2:
		switch(action)
		{
			case 1:
			{
				Line1->SetText("MOVE");
				Line2->SetText("Range: Medium");
				Line3->SetText("Recovery: Short");
				Line4->SetText("");
			}break;

			case 2:
			{
				Line1->SetText("PUNCH");
				Line2->SetText("Range: Short");
				Line3->SetText("Recovery: Short");
				Line4->SetText("Damage: 3 HP");
			}break;

			case 3:
			{
				Line1->SetText("PUNCH FURY");
				Line2->SetText("Range: Short");
				Line3->SetText("Recovery: Long");
				Line4->SetText("Damage: 3-10 HP");
			}break;

			case 4:
			{
				Line1->SetText("SHOTGUN");
				Line2->SetText("Range: Medium");
				Line3->SetText("Recovery: Medium");
				Line4->SetText("Damage: 2-6");
			}break;

			case 5:
			{
				Line1->SetText("SLOW");
				Line2->SetText("Range: Long");
				Line3->SetText("Recovery: Medium");
				Line4->SetText("Cast Slow on target");
			}break;
		}break;

		case 3:
		switch(action)
		{
			case 1:
			{
				Line1->SetText("TELEPORT");
				Line2->SetText("Range: Medium");
				Line3->SetText("Recovery: Short");
				Line4->SetText("");
			}break;

			case 2:
			{
				Line1->SetText("ZAP");
				Line2->SetText("Range: Medium");
				Line3->SetText("Recovery: Short");
				Line4->SetText("Damage:1-3 HP & shock");
			}break;

			case 3:
			{
				Line1->SetText("FIREBALL");
				Line2->SetText("Range: Medium");
				Line3->SetText("Recovery: Medium");
				Line4->SetText("Damage: 3-7");
			}break;

			case 4:
			{
				Line1->SetText("POISON");
				Line2->SetText("Range: Long");
				Line3->SetText("Recovery: Medium");
				Line4->SetText("Cast Poison on target");
			}break;

			case 5:
			{
				Line1->SetText("COLOR CHANGE");
				Line2->SetText("Range: Short");
				Line3->SetText("Recovery:Long");
				Line4->SetText("Convert a weaker unit");
			}break;
		}break;

		case 4:
		switch(action)
		{
			case 1:
			{
				Line1->SetText("SPIN");
				Line2->SetText("Range: Medium");
				Line3->SetText("Recovery: Short");
				Line4->SetText("");
			}break;

			case 2:
			{
				Line1->SetText("3D ATTACK");
				Line2->SetText("Range: Long");
				Line3->SetText("Recovery: Short");
				Line4->SetText("Damage: 1 HP");
			}break;

			case 3:
			{
				Line1->SetText("HEAL");
				Line2->SetText("Range: Long");
				Line3->SetText("Recovery: Medium");
				Line4->SetText("Heal 4-6 HP to target");
			}break;

			case 4:
			{
				Line1->SetText("PURIFY");
				Line2->SetText("Range: Long");
				Line3->SetText("Recovery: Medium");
				Line4->SetText("Remove all effects");
			}break;

			case 5:
			{
				Line1->SetText("DRAIN");
				Line2->SetText("Range: Short");
				Line3->SetText("Recovery: Long");
				Line4->SetText("Damage & Heal 1-6 HP");
			}break;
		}break;

		case 5:
		switch(action)
		{
			case 1:
			{
				Line1->SetText("MOVE");
				Line2->SetText("Range: Medium");
				Line3->SetText("Recovery: Short");
				Line4->SetText("");
			}break;

			case 2:
			{
				Line1->SetText("THE BIG BAT");
				Line2->SetText("Range: Short");
				Line3->SetText("Recovery: Medium");
				Line4->SetText("Damage: 4-8 HP");
			}break;

			case 3:
			{
				Line1->SetText("BAZOOKA");
				Line2->SetText("Range: Medium");
				Line3->SetText("Recovery: Medium");
				Line4->SetText("Damage: 4 HP");
			}break;

			case 4:
			{
				Line1->SetText("DYNAMITE");
				Line2->SetText("Range: Short");
				Line3->SetText("Recovery: -");
				Line4->SetText("Max HP target&self");
			}break;

			case 5:
			{
				Line1->SetText("SPEED");
				Line2->SetText("Range: Long");
				Line3->SetText("Recovery: Short");
				Line4->SetText("Cast Speed on target");
			}break;
		}break;
	}
}
//---------------------------------------------------------------------------------------

