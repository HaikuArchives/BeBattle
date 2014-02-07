#include "be5.h"
#include <Rect.h>
#define SPEED 75000
//--------------------------------------------------------------------
Be5::Be5(int w, int p)
{
	//propriete de l'unit
	Position.top = ((int)(w/14)) * 32;
	Position.left = (w % 14) * 32;
	Position.bottom = Position.top + 31;
	Position.right = Position.left + 31;
	HP = 10;
	Player = p;
	Type = 5;
	CastRemove();
	Recovery = (rand() % 3) + 4;

	switch(Player)
	{
		case 1:PlayerColor.red=254; PlayerColor.green=0; PlayerColor.blue=0; PlayerColor.alpha=255;break;
		case 2:PlayerColor.red=0; PlayerColor.green=0; PlayerColor.blue=254; PlayerColor.alpha=255;break;
	}

	CurrentImage = GetAnim(Type, 0, 0, PlayerColor);
}
//---------------------------------------------------------------------------------
void Be5::DoAction1(BeUnit* Unit, BRect dest)
{//walk
	BBitmap* Frame[6];
	Frame[0] = GetAnim(Type, 1, 0, PlayerColor); //right
	Frame[1] = GetAnim(Type, 1, 1, PlayerColor); 
	Frame[2] = GetAnimFlip(Type, 1, 0, PlayerColor); //left
	Frame[3] = GetAnimFlip(Type, 1, 1, PlayerColor);
	Frame[4] = GetAnim(Type, 0, 1, PlayerColor); //up or down
	Frame[5] = GetAnimFlip(Type, 0, 1, PlayerColor);

	BBitmap* idle = CurrentImage;
	int CurrentFrame = -1; //idle
	BRect Invalidator;
	
	while(Position != dest)
	{
		Invalidator = Position;
		if(Position.top < dest.top) //down
		{
			Position.OffsetBy(0, 8);	
			if(CurrentFrame == -1 || CurrentFrame == 4) CurrentFrame = 5;
			else CurrentFrame = 4;

			CurrentImage = Frame[CurrentFrame]; 
			Update(Invalidator);
		}
		
		else if(Position.top > dest.top) //up
		{
			Position.OffsetBy(0, -8);	
			if(CurrentFrame == -1 || CurrentFrame == 4) CurrentFrame = 5;
			else CurrentFrame = 4;

			CurrentImage = Frame[CurrentFrame]; 
			Update(Invalidator);
		}

		else if(Position.left < dest.left) //right
		{
			Position.OffsetBy(8, 0);	
			if(CurrentFrame == -1 || CurrentFrame == 0)	CurrentFrame = 1;
			else CurrentFrame = 0;

			CurrentImage = Frame[CurrentFrame]; 
			Update(Invalidator);
		}
		
		else if(Position.left > dest.left) //left
		{
			Position.OffsetBy(-8, 0);	
			if(CurrentFrame == -1 || CurrentFrame == 2) CurrentFrame = 3;
			else CurrentFrame = 2;

			CurrentImage = Frame[CurrentFrame]; 
			Update(Invalidator);
		}
		snooze(SPEED);
	}

	CurrentImage = idle;
	Update(Position);

	Recovery += 3;

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
	delete Frame[4];
	delete Frame[5];
}
//--------------------------------------------------------------------------------------------------
void Be5::DoAction2(BeUnit* Unit, BRect dest)
{//The BIG BAT (a favorite of mine)
	BBitmap* Frame[6];
	Frame[0] = GetAnim(Type, 3, 0, PlayerColor); //big swing
	Frame[1] = GetAnimFlip(Type, 3, 0, PlayerColor);
	Frame[2] = GetAnim(Type, 4, 0, PlayerColor); //huge impact
	Frame[3] = GetAnimFlip(Type, 4, 0, PlayerColor);
	Frame[4] = GetAnim(Type, 2, 0, PlayerColor);
	Frame[5] = GetAnim(Type, 3, 1, PlayerColor);

	BRect Origine = Position;
	int direction, decalage;
	
	if(Position.left > dest.left || dest.left == 0)
	{dest.OffsetBy(32,0); direction = 1; decalage = -10;}
	
	else
	{dest.OffsetBy(-32,0); direction = 0; decalage = 10;}

	DoAction1(this, dest);
	BBitmap* idle = CurrentImage;
	
	CurrentImage = Frame[4]; //casting
	Update(Position); 
	snooze(SPEED * 2);

	//the BIG BAT appears
	SEffect()->Position = Position; 
	SEffect()->CurrentImage = Frame[5];
	SEffect()->Valid = true;

	rgb_color c;
	for(int a = 15; a != 255; a+=16)
	{//fading in
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = a;
			}
		Update(Position);
		snooze((int)(SPEED/2));
	}

	SEffect()->Valid = false;
	Update(Position);

	BRect Invalidator;
	Invalidator = Position; 
	Position.OffsetBy(decalage, 0);
	CurrentImage = Frame[0+direction]; 
	Update(Invalidator); 
	snooze(SPEED * 2);

	Invalidator = Position; 
	Position.OffsetBy(decalage, 0);
	Update(Invalidator);
	snooze(SPEED);

	CurrentImage = Frame[2+direction];
	Update(Position); 
	Unit->Hit(!((bool) direction));
	
	Invalidator = Position; 
	Position.OffsetBy(-decalage, 0);
	Update(Invalidator); 
	snooze(SPEED);

	Invalidator = Position; 
	Position.OffsetBy(-decalage, 0);
	Update(Invalidator); 
	snooze(SPEED);
	
	CurrentImage = idle; 
	Update(Invalidator);

	Unit->SetHP(-(rand()%5 + 4)); //4-8 dammage
	Recovery -= 1; //6(walk) - 1 = 5;

	DoAction1(this, Origine);

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
	delete Frame[4];
	delete Frame[5];
} 
//--------------------------------------------------------------------------------------------------
void Be5::DoAction3(BeUnit* Unit, BRect dest) 
{//bazooka (Big badaboum)
	BBitmap* Frame[7];
	Frame[0] = GetAnim(Type, 5, 0, PlayerColor); //aiming ->
	Frame[1] = GetAnimFlip(Type, 5, 0, PlayerColor); //aiming <-
	Frame[2] = GetAnim(Type, 6, 0, PlayerColor); //missile ->
	Frame[3] = GetAnimFlip(Type, 6, 0, PlayerColor); //missile <-
	Frame[4] = GetAnim(Type, 10, 0, PlayerColor); //ouch !
	Frame[5] = GetAnim(Type, 2, 1, PlayerColor); //bazooka
	Frame[6] = GetAnim(Type, 2, 0, PlayerColor); //casting

	int direction = 0; //right
	if(Position.left > dest.left)
		direction = 1; //left
	
	BBitmap* idle = CurrentImage;
	BRect Invalidator;

	CurrentImage = Frame[6];
	Update(Position); 
	snooze(SPEED * 2);

	SEffect()->Position = Position; 
	SEffect()->CurrentImage = Frame[5];
	SEffect()->Valid = true;

	rgb_color c;
	for(int a = 15; a != 255; a+=16)
	{//fade in
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = a;
			}
		Update(Position);
		snooze((int)(SPEED/2));
	}

	SEffect()->Valid = false;
	Update(Position);

	CurrentImage = Frame[0+direction]; 
	Update(Position); 
	snooze(SPEED*2);

	SEffect()->Position = Position; //Firing Bazooka !
	SEffect()->CurrentImage = Frame[2+direction];
	SEffect()->Valid = true;

	while(SEffect()->Position != dest)
	{
		Invalidator = SEffect()->Position;	

		if(SEffect()->Position.left < dest.left) SEffect()->Position.OffsetBy(8,0);		
		else if(SEffect()->Position.left > dest.left) SEffect()->Position.OffsetBy(-8,0);		

		if(SEffect()->Position.top < dest.top) SEffect()->Position.OffsetBy(0,8);		
		else if(SEffect()->Position.top > dest.top) SEffect()->Position.OffsetBy(0,-8);		

		SEffect()->Update(Invalidator);	
		snooze((int)SPEED/2);
	}

	SEffect()->CurrentImage = Frame[4];
	Unit->Hit(!((bool) direction));		
	SEffect()->Valid = false;
	Update(SEffect()->Position);

	CurrentImage = idle; 
	Update(Position);

	Unit->SetHP(-4); //4 pts of dammage
	Recovery += 5;

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
	delete Frame[4];
	delete Frame[5];
	delete Frame[6];
}
//--------------------------------------------------------------------------------------------------
void Be5::DoAction4(BeUnit* Unit, BRect dest)
{//dynamite
	BBitmap* Frame[9];
	Frame[0] = GetAnim(Type, 2, 0, PlayerColor); //placement ->
	Frame[1] = GetAnimFlip(Type, 2, 0, PlayerColor); //placement <-
	Frame[2] = GetAnim(Type, 0, 2, PlayerColor); //protect himself
	Frame[3] = GetAnimFlip(Type, 7, 0, PlayerColor); //dynamite 1
	Frame[4] = GetAnimFlip(Type, 8, 0, PlayerColor); //dynamite 2
	Frame[5] = GetAnimFlip(Type, 9, 0, PlayerColor); //dynamyte 3
	Frame[6] = GetAnimFlip(Type, 10, 0, PlayerColor); //explosion
	Frame[7] = GetAnim(Type, 2, 0, PlayerColor);
	Frame[8] = GetAnim(Type, 4, 1, PlayerColor);
	
	int direction;
	BRect Target = dest;

	if(Position.left > dest.left || dest.left == 0)
	{dest.OffsetBy(32,0); direction = 1;}
	
	else
	{dest.OffsetBy(-32,0); direction = 0;}

	DoAction1(this, dest);
	BBitmap* idle = CurrentImage;
	CurrentImage = Frame[7];
	Update(Position); 
	snooze(SPEED * 2);

	SEffect()->Position = Position; 
	SEffect()->CurrentImage = Frame[8];
	SEffect()->Valid = true;

	rgb_color c;
	for(int a = 15; a != 255; a+=16)
	{
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = a;
			}
		Update(Position);
		snooze((int)(SPEED/2));
	}

	//place dynamite
	SEffect()->CurrentImage = Frame[3];
	SEffect()->Position = Target;
	SEffect()->Update(Target);

	CurrentImage = Frame[0+direction];
	Update(Position); //back idle
	snooze(SPEED*2); 

	SEffect()->CurrentImage = Frame[4];
	SEffect()->Update(Target);
	snooze(SPEED*4);

	//protect himself
	CurrentImage = Frame[2];
	Update(Position); 
	snooze(SPEED*2); 

	SEffect()->CurrentImage = Frame[5];
	SEffect()->Update(Target);
	snooze(SPEED*4);

	SEffect()->CurrentImage = Frame[6];
	Unit->Hit(!((bool) direction));
	Update(Target); 
	snooze(SPEED*4);

	SEffect()->Position = Position;
	Hit(!((bool) direction));
	Update(Position); 
	snooze(SPEED*2);

	SEffect()->Valid = false;
	CurrentImage = idle; 
	Update(Target);
	Update(Position);

	Unit->SetHP(-Unit->GetHP()); //Max dammage 
	SetHP(-GetHP());

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
	delete Frame[4];
	delete Frame[5];
	delete Frame[6];
	delete Frame[7];
	delete Frame[8];
}
//--------------------------------------------------------------------------------------------------
void Be5::DoAction5(BeUnit* Unit, BRect dest) 
{//speed up
	BBitmap* Frame[3];
	Frame[0] = GetAnim(Type, 8, 1, PlayerColor); //speed arrow
	Frame[1] = GetAnim(Type, 2, 0, PlayerColor); //casting
	Frame[2] = GetAnim(Type, 5, 1, PlayerColor); //speed ball
	
	BBitmap* idle = CurrentImage;
	CurrentImage = Frame[1];
	Update(Position); 
	snooze(SPEED * 2);

	SEffect()->Position = Position; 
	SEffect()->CurrentImage = Frame[2];
	SEffect()->Valid = true;

	rgb_color c;
	for(int a = 15; a != 255; a+=16)
	{
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = a;
			}
		Update(Position);
		snooze((int)(SPEED/2));
	}

	SEffect()->Valid = false;
	Update(Position);

	//speed-up
	SEffect()->Position = dest; 
	SEffect()->CurrentImage = Frame[0];
	SEffect()->Valid = true;

	for(int b=0; b<26; b+=2)
	{
		for(int i = 0; i < 32; i++)
		{
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					if(i==b)
						((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 200;
					else if(i==b+1)
						((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 150;
					else if(i==b+2)
						((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 100;
					else if(i==b+3)
						((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 150;
					else if(i==b+4)
						((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 200;
					else
						((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 255;
			}
		}
		SEffect()->Update(dest);
		snooze((int)(SPEED/4));
	}

	for(int b=26; b>0; b-=2)
	{
		for(int i = 0; i < 32; i++)
		{
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					if(i==b)
						((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 200;
					else if(i==b+1)
						((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 150;
					else if(i==b+2)
						((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 100;
					else if(i==b+3)
						((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 150;
					else if(i==b+4)
						((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 200;
					else
						((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 255;
			}
		}
		SEffect()->Update(dest);
		snooze((int)(SPEED/4));
	}

	SEffect()->Valid = false;
	Update(dest);
	CurrentImage = idle; 
	Update(Position);

	Unit->CastSpeed();
	Recovery += 3;

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
}
//--------------------------------------------------------------------------------------------------
void Be5::DoAction6(BeUnit* b, BRect r)
{
	BBitmap* casting = GetAnim(Type, 2, 0, PlayerColor);
	BBitmap* beSign = GetAnim(Type, 6, 1, PlayerColor);
	BBitmap* idle = CurrentImage;
	CurrentImage = casting;
	Update(Position); 
	snooze(SPEED * 2);

	SEffect()->Position = Position; 
	SEffect()->CurrentImage = beSign;
	SEffect()->Valid = true;

	rgb_color c;
	for(int a = 15; a != 255; a+=16)
	{
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = a;
			}
		Update(Position);
		snooze((int)(SPEED/2));
	}

	SEffect()->Valid = false;
	Update(Position); //enleve la trace

	CurrentImage = idle; 
	Update(Position);
	delete casting;
	delete beSign;
}
//--------------------------------------------------------------------------------------------------
bool Be5::OkTarget1(BeUnit* Unit, bool ValidUnit, BRect dest)
{//walking by
	if(ValidUnit || (Distance(dest, Position) > 5)) 
		return false;

	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be5::OkTarget2(BeUnit* Unit, bool ValidUnit, BRect dest)
{//The BIG BAT !
	if(!ValidUnit || (Distance(dest, Position) > 3) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false;
	
	return true;
} 
//--------------------------------------------------------------------------------------------------
bool Be5::OkTarget3(BeUnit* Unit, bool ValidUnit, BRect dest) 
{//bazooka !
	if(!ValidUnit || (Distance(dest, Position) > 5) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false; 

	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be5::OkTarget4(BeUnit* Unit, bool ValidUnit, BRect dest)
{//dynamite
	if(!ValidUnit || (Distance(dest, Position) > 3) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false;
	
	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be5::OkTarget5(BeUnit* Unit, bool ValidUnit, BRect dest) 
{//speed up
	if(!ValidUnit || (Distance(dest, Position) > 7) || (Unit->GetPlayer() != Player))
		return false; 

	return true;
}
//--------------------------------------------------------------------------------------------------
void Be5::Hit(bool direction)
{
	BBitmap* Ouch = GetAnim(Type, 0, 2, PlayerColor);
	BBitmap* idle = CurrentImage;
	
	CurrentImage = Ouch;
	Update(Position); 
	snooze(SPEED * 2);
	
	CurrentImage = idle; 
	Update(Position);

	delete Ouch;
}
//--------------------------------------------------------------------------------------------------
void Be5::Stupid()
{
	BBitmap* interogation = GetAnim(Type, 7, 1, PlayerColor);
	BBitmap* casting = GetAnim(Type, 2, 0, PlayerColor);
	BBitmap* idle = CurrentImage;
	
	CurrentImage = casting;
	Update(Position); 
	snooze(SPEED * 2);

	SEffect()->Position = Position; 
	SEffect()->CurrentImage = interogation;
	SEffect()->Valid = true;

	rgb_color c;
	for(int a = 15; a != 255; a+=16)
	{
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = a;
			}
		Update(Position);
		snooze((int)(SPEED/2));
	}

	SEffect()->Valid = false;
	Update(Position);

	CurrentImage = idle; 
	Update(Position);

	//clean up
	delete casting;
	delete interogation;
}
//-----------------------------------------------------------------
void Be5::Kill()
{
	//save current effect (ex :flying head)
	bool valid = SEffect()->Valid;
	BBitmap* currentEffect = SEffect()->CurrentImage;
	BRect position = SEffect()->Position;

	SEffect()->Position = Position;
	SEffect()->CurrentImage = GetAnim(Type, 1, 2, PlayerColor); //Adieu !
	SEffect()->Valid = true;

	//Transparent angel
	rgb_color c;
	for(int i = 0; i < 32; i++)
		for(int j = 0; j < 32; j++)
		{
			c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
			if(!(c.red == 255 && c.green == 0 && c.blue == 255))
				((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 128;
		}

	BRect Invalidator;
	BRect dest = Position;
	dest.OffsetBy(0,-32);

	while(SEffect()->Position != dest) //bye bye !
	{
		Invalidator = SEffect()->Position;
		SEffect()->Position.OffsetBy(0,-2);
		SEffect()->Update(Invalidator);
		snooze(SPEED);
	}

	SEffect()->Valid = false;
	SEffect()->Update(SEffect()->Position); 
	delete SEffect()->CurrentImage;
	
	//put the old effect in place
	SEffect()->Valid = valid;
	SEffect()->CurrentImage = currentEffect;
	SEffect()->Position = position;
}
//-----------------------------------------------------------------
