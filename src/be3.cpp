#include "be3.h"
#include <Rect.h>
#define SPEED 75000
//--------------------------------------------------------------------
Be3::Be3(int w, int p)
{
	//propriete de l'unit
	Position.top = ((int)(w/14)) * 32;
	Position.left = (w % 14) * 32;
	Position.bottom = Position.top + 31;
	Position.right = Position.left + 31;
	HP = 10;
	Player = p;
	Type = 3;
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
void Be3::DoAction1(BeUnit* Unit, BRect dest)
{//teleportation 
	BBitmap* idle = CurrentImage;
	
	BBitmap* Frame[2];
	Frame[0] = GetAnim(Type, 0, 1, PlayerColor);
	Frame[1] = GetAnim(Type, 0, 2, PlayerColor);

	CurrentImage = Frame[0];
	Update(Position);
	snooze(SPEED);
		
	CurrentImage = Frame[1];
	Update(Position);
	snooze(SPEED);

	CurrentImage = Frame[0];
	Update(Position);
	snooze(SPEED*2);

	//Fade Out
	rgb_color c;
	for(int a = 240; a >= 0; a-=16)
	{
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)CurrentImage->Bits())[(i*32 + j)].alpha = a;
			}
		Update(Position);
		snooze((int)SPEED/2);
	}

	Position = dest;

	//Fade In
	for(int a = 15; a <= 255; a+=16)
	{
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)CurrentImage->Bits())[(i*32 + j)].alpha = a;
			}
		Update(Position);
		snooze((int)SPEED/2);
	}

	CurrentImage = Frame[1];
	Update(Position);
	snooze(SPEED*2);

	CurrentImage = Frame[0];
	Update(Position);
	snooze(SPEED);

	CurrentImage = idle;
	Update(Position);

	//on set le nouveau recovery
	Recovery += 3;

	//clean up
	delete Frame[0];
	delete Frame[1];
}
//--------------------------------------------------------------------------------------------------
void Be3::DoAction2(BeUnit* Unit, BRect dest)
{//ZAP
	BBitmap* Frame[8];
	Frame[0] = GetAnim(Type, 1, 0, PlayerColor); //charge1
	Frame[1] = GetAnimFlip(Type, 1, 0, PlayerColor);
	Frame[2] = GetAnim(Type, 1, 1, PlayerColor); //charge2
	Frame[3] = GetAnimFlip(Type, 1, 1, PlayerColor);
	Frame[4] = GetAnim(Type, 1, 2, PlayerColor); //charge3
	Frame[5] = GetAnimFlip(Type, 1, 2, PlayerColor);
	Frame[6] = GetAnim(Type, 2, 2, PlayerColor); //Zap
	Frame[7] = GetAnimFlip(Type, 2, 2, PlayerColor);

	BRect Origine = Position;
	BBitmap* idle = CurrentImage;
	int direction;
	BRect target = dest;
	
	if(Position.left > dest.left || dest.left == 0)
	{dest.OffsetBy(32,0); direction = 1;}
	
	else
	{dest.OffsetBy(-32,0); direction = 0;}

	DoAction1(this, dest);
	
	//Staff charging
	CurrentImage = Frame[0+direction]; 
	Update(Position); 
	snooze(SPEED);

	CurrentImage = Frame[2+direction]; 
	Update(Position); 
	snooze(SPEED);

	CurrentImage = Frame[4+direction]; 
	Update(Position); 
	snooze(SPEED);

	//Zap
	SEffect()->Position = target;
	SEffect()->CurrentImage = Frame[6+direction];

	for(int i = 0; i < 5; i++)
	{
		SEffect()->Valid = true;
		Unit->Hit(!((bool) direction));		
		SEffect()->Valid = false;
		Update(SEffect()->Position);
		snooze((int)SPEED/2);
	}

	Unit->SetHP(-(rand()%3 +1)); //1-3 damage
	Unit->AddRecovery(); //target is shocked !
	Recovery -= 3; //6(2 teleport) - 3 = 3

	//Staff uncharging
	CurrentImage = Frame[2+direction]; 
	Update(Position); 
	snooze(SPEED);

	CurrentImage = Frame[0+direction]; 
	Update(Position); 
	snooze(SPEED);

	CurrentImage = idle; 
	Update(Position);

	DoAction1(this, Origine);

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
	delete Frame[4];
	delete Frame[5];
	delete Frame[6];
	delete Frame[7];
} 
//--------------------------------------------------------------------------------------------------
void Be3::DoAction3(BeUnit* Unit, BRect dest) 
{//fireball
	BBitmap* Frame[11];
	Frame[0] = GetAnim(Type, 1, 0, PlayerColor); //charge1
	Frame[1] = GetAnimFlip(Type, 1, 0, PlayerColor);
	Frame[2] = GetAnim(Type, 1, 1, PlayerColor); //charge2
	Frame[3] = GetAnimFlip(Type, 1, 1, PlayerColor);
	Frame[4] = GetAnim(Type, 1, 2, PlayerColor); //charge3
	Frame[5] = GetAnimFlip(Type, 1, 2, PlayerColor);
	Frame[6] = GetAnim(Type, 2, 0, PlayerColor); //fireball 1
	Frame[7] = GetAnimFlip(Type, 2, 0, PlayerColor);
	Frame[8] = GetAnim(Type, 2, 3, PlayerColor); //fireball 2
	Frame[9] = GetAnimFlip(Type, 2, 3, PlayerColor);
	Frame[10] = GetAnim(Type, 2, 1, PlayerColor); //explosion

	//Make the fireball and explosion semi-transparent
	rgb_color c;
	for(int a = 6; a < 11; a++)
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)Frame[a]->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)Frame[a]->Bits())[(i*32 + j)].alpha = 128;
			}

	int direction = 0; //right
	if(Position.left > dest.left)
		direction = 1; //left
	
	BBitmap* idle = CurrentImage;
	BRect Invalidator;
	
	CurrentImage = Frame[0+direction]; 
	Update(Position); 
	snooze(SPEED);

	CurrentImage = Frame[2+direction]; 
	Update(Position); 
	snooze(SPEED);

	CurrentImage = Frame[4+direction]; 
	Update(Position); 
	snooze(SPEED);

	SEffect()->Position = Position; //fireball !
	SEffect()->Valid = true;
	bool flip = true;

	while(SEffect()->Position != dest)
	{
		if(flip) 	SEffect()->CurrentImage = Frame[6+direction];
		else		SEffect()->CurrentImage = Frame[8+direction];
		flip = !flip;

		Invalidator = SEffect()->Position;	

		if(SEffect()->Position.left < dest.left) 		SEffect()->Position.OffsetBy(8,0);		
		else if(SEffect()->Position.left > dest.left) 	SEffect()->Position.OffsetBy(-8,0);		

		if(SEffect()->Position.top < dest.top) 			SEffect()->Position.OffsetBy(0,8);		
		else if(SEffect()->Position.top > dest.top) 	SEffect()->Position.OffsetBy(0,-8);		

		SEffect()->Update(Invalidator);	
		snooze((int)SPEED/2);
	}

	SEffect()->CurrentImage = Frame[8];
	Unit->Hit(!((bool) direction));		
	SEffect()->Valid = false;
	Update(SEffect()->Position); //refresh

	Unit->SetHP(-(rand()%5 + 3)); //3-7 dammage
	Recovery += 5;

	CurrentImage = Frame[2+direction]; 
	Update(Position); 
	snooze(SPEED);

	CurrentImage = Frame[0+direction]; 
	Update(Position); 
	snooze(SPEED);
	
	CurrentImage = idle; 
	Update(Position);

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
	delete Frame[9];
	delete Frame[10];
}
//--------------------------------------------------------------------------------------------------
void Be3::DoAction4(BeUnit* Unit, BRect dest)
{//poison
	BBitmap* Frame[8];
	
	Frame[0] = GetAnim(Type, 3, 2, PlayerColor); //Ugly poison effect
	Frame[1] = GetAnimFlip(Type, 3, 2, PlayerColor); 	
	Frame[2] = GetAnim(Type, 1, 0, PlayerColor); //charge1
	Frame[3] = GetAnimFlip(Type, 1, 0, PlayerColor);
	Frame[4] = GetAnim(Type, 1, 1, PlayerColor); //charge2
	Frame[5] = GetAnimFlip(Type, 1, 1, PlayerColor);
	Frame[6] = GetAnim(Type, 1, 2, PlayerColor); //charge3
	Frame[7] = GetAnimFlip(Type, 1, 2, PlayerColor);

	int direction = 0; //right
	if(Position.left > dest.left)
		direction = 1; //left
	
	BBitmap* idle = CurrentImage;
	
	CurrentImage = Frame[2+direction]; 
	Update(Position); 
	snooze(SPEED);

	CurrentImage = Frame[4+direction]; 
	Update(Position); 
	snooze(SPEED);

	CurrentImage = Frame[6+direction]; 
	Update(Position); 
	snooze(SPEED);

	SEffect()->Position = dest; //poison !
	SEffect()->CurrentImage = Frame[0];
	SEffect()->Valid = true;

	for(int i=0; i<4; i++)
	{
		SEffect()->CurrentImage = Frame[0];
		SEffect()->Update(dest);
		snooze((int)SPEED);
		SEffect()->CurrentImage = Frame[1];
		SEffect()->Update(dest);
		snooze((int)SPEED);
	}
	SEffect()->Valid = false;
	Update(dest);

	Unit->CastPoison();
	Recovery += 7;
	
	CurrentImage = idle;
	Update(Position);
	
	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
	delete Frame[4];
	delete Frame[5];
	delete Frame[6];
	delete Frame[7];
}
//--------------------------------------------------------------------------------------------------
void Be3::DoAction5(BeUnit* Unit, BRect dest) 
{//Color Fuzz
	BBitmap* Frame[6];
	Frame[0] = GetAnim(Type, 1, 0, PlayerColor); //charge1
	Frame[1] = GetAnimFlip(Type, 1, 0, PlayerColor);
	Frame[2] = GetAnim(Type, 1, 1, PlayerColor); //charge2
	Frame[3] = GetAnimFlip(Type, 1, 1, PlayerColor);
	Frame[4] = GetAnim(Type, 1, 2, PlayerColor); //charge3
	Frame[5] = GetAnimFlip(Type, 1, 2, PlayerColor);

	int direction = 0; //right
	if(Position.left > dest.left)
		direction = 1; //left
	
	BBitmap* idle = CurrentImage;
	
	CurrentImage = Frame[0+direction]; 
	Update(Position); 
	snooze(SPEED);

	CurrentImage = Frame[2+direction]; 
	Update(Position); 
	snooze(SPEED);

	CurrentImage = Frame[4+direction]; 
	Update(Position); 
	snooze(SPEED);

	SEffect()->Position = dest; //Color him !
	SEffect()->CurrentImage = new BBitmap(Unit->CurrentImage);
	SEffect()->Valid = true;
	
	rgb_color c;
	//initialisation
	for(int i = 0; i < 32; i++)
		for(int j = 0; j < 32; j++)
		{
			c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
			if(c.red == Unit->PlayerColor.red && c.green == Unit->PlayerColor.green 
			&& c.blue == Unit->PlayerColor.blue)
				((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = 254;
		}

	//flashing
	for(int red = 64; red < 255; red+=64)
		for(int green = 64; green < 255; green+=64)
			for(int blue = 64; blue < 255; blue+=64)
			{
				for(int i = 0; i < 32; i++)
					for(int j = 0; j < 32; j++)
					{
						c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
						if(c.alpha == 254)
						{
							((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].red = red;
							((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].green = green;
							((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].blue = blue;
						}
					}
					Update(dest);
					snooze((int)SPEED/2);
			}

	SEffect()->Valid = false;
	delete SEffect()->CurrentImage; //delete temporary bitmap

	//Can convert unit having 1-9 HP
	if(rand()%11 > Unit->GetHP())
	{
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)Unit->CurrentImage->Bits())[(i*32 + j)];
				if(c.red == Unit->PlayerColor.red && c.green == Unit->PlayerColor.green &&
				c.blue == Unit->PlayerColor.blue)
					((rgb_color *)Unit->CurrentImage->Bits())[(i*32 + j)] = PlayerColor;
			}

		Unit->SetPlayer(Player);
		Unit->PlayerColor = PlayerColor;
	}

	Recovery += 5;
	Update(dest);

	CurrentImage = Frame[2+direction]; 
	Update(Position); 
	snooze(SPEED);

	CurrentImage = Frame[0+direction]; 
	Update(Position); 
	snooze(SPEED);
	
	CurrentImage = idle; 
	Update(Position);

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
	delete Frame[4];
	delete Frame[5];
}
//--------------------------------------------------------------------------------------------------
void Be3::DoAction6(BeUnit* b, BRect r)
{
	BBitmap* Frame[3];
	Frame[0] = GetAnim(Type, 4, 0, PlayerColor);
	Frame[1] = GetAnim(Type, 4, 1, PlayerColor);
	Frame[2] = GetAnim(Type, 4, 2, PlayerColor);

	BBitmap* idle = CurrentImage;

	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED * 2);

	for(int i=0; i < 3; i++)
	{
		CurrentImage = Frame[1]; 
		Update(Position); 
		snooze(SPEED);

		CurrentImage = Frame[2]; 
		Update(Position); 
		snooze(SPEED);
	}	

	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED * 2);

	CurrentImage = idle; 
	Update(Position);

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
}
//--------------------------------------------------------------------------------------------------
bool Be3::OkTarget1(BeUnit* Unit, bool ValidUnit, BRect dest)
{//teleportation
	if(ValidUnit || (Distance(dest, Position) > 5)) 
		return false;
	
	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be3::OkTarget2(BeUnit* Unit, bool ValidUnit, BRect dest)
{//ZAP
	if(!ValidUnit || (Distance(dest, Position) > 5) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false;

	return true;
} 
//--------------------------------------------------------------------------------------------------
bool Be3::OkTarget3(BeUnit* Unit, bool ValidUnit, BRect dest) 
{//fireball
	if(!ValidUnit || (Distance(dest, Position) > 5) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false; 

	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be3::OkTarget4(BeUnit* Unit, bool ValidUnit, BRect dest)
{//poison
	if(!ValidUnit || (Distance(dest, Position) > 7) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false; 

	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be3::OkTarget5(BeUnit* Unit, bool ValidUnit, BRect dest) 
{//Color Fuzz
	if(!ValidUnit || (Distance(dest, Position) > 3) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false; 

	return true;
}
//--------------------------------------------------------------------------------------------------
void Be3::Hit(bool direction)
{
	BBitmap* Ouch;
	if(direction) 	Ouch = GetAnim(Type, 3, 0, PlayerColor);
	else			Ouch = GetAnimFlip(Type, 3, 0, PlayerColor);
			
	BBitmap* idle = CurrentImage;
	
	CurrentImage = Ouch;
	Update(Position); 
	snooze(SPEED * 2);
	
	CurrentImage = idle; 
	Update(Position);
	delete Ouch; //clean up
}
//--------------------------------------------------------------------------------------------------
void Be3::Stupid()
{
	BBitmap* Stupid1 = GetAnim(Type, 5, 0, PlayerColor);
	BBitmap* Stupid2 = GetAnim(Type, 5, 1, PlayerColor);
			
	BBitmap* idle = CurrentImage;
	
	for(int i=0; i<3; i++)
	{
		CurrentImage = Stupid1;
		Update(Position); 
		snooze(SPEED * 2);
	
		CurrentImage = Stupid2;
		Update(Position); 
		snooze(SPEED * 2);
	}
	
	CurrentImage = idle; 
	Update(Position);

	delete Stupid1;
	delete Stupid2;
}
//-----------------------------------------------------------------
void Be3::Kill()
{
	delete CurrentImage;
	CurrentImage = GetAnim(Type, 3, 1, PlayerColor);

	rgb_color c;
	for(int a = 240; a >= 0; a-=16)
	{
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == PlayerColor.red && c.green == PlayerColor.green && c.blue == PlayerColor.blue) 
				&& !(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)CurrentImage->Bits())[(i*32 + j)].alpha = a;
			}
		Update(Position);
		snooze((int)SPEED/2);
	}
	//don't delete CurrentImage...
}
//-----------------------------------------------------------------
