#include "be4.h"
#include <Rect.h>
#define SPEED 75000
//--------------------------------------------------------------------
Be4::Be4(int w, int p)
{
	//propriete de l'unit
	Position.top = ((int)(w/14)) * 32;
	Position.left = (w % 14) * 32;
	Position.bottom = Position.top + 31;
	Position.right = Position.left + 31;
	HP = 10;
	Player = p;
	Type = 4;
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
void Be4::DoAction1(BeUnit* Unit, BRect dest)
{//Spinning out of control !
	BBitmap* idle = CurrentImage;
	BRect Invalidator;
	bool spin = true;	

	BBitmap* Frame[2];
	Frame[0] = GetAnim(Type, 0, 1, PlayerColor); //spin 1
	Frame[1] = GetAnim(Type, 0, 2, PlayerColor); //spin 2

	while(Position != dest)
	{
		if(spin) CurrentImage = Frame[0];
		else CurrentImage = Frame[1];
		spin = !spin;		
		snooze(SPEED/2);
		Invalidator = Position;
		if(Position.top < dest.top) Position.OffsetBy(0, 8); //down
		else if(Position.top > dest.top) Position.OffsetBy(0, -8); //up
		else if(Position.left < dest.left) Position.OffsetBy(8, 0); //right
		else if(Position.left > dest.left) Position.OffsetBy(-8, 0); //left
		Update(Invalidator);
	}

	CurrentImage = idle;
	Update(Position);
	Recovery += 3;

	//clean up
	delete Frame[0];
	delete Frame[1];
}
//--------------------------------------------------------------------------------------------------
void Be4::DoAction2(BeUnit* Unit, BRect dest)
{//3D Attack
	BBitmap* Frame[6];
	
	//Load les Images du move pour lui
	Frame[0] = GetAnim(Type, 1, 0, PlayerColor); //charge1
	Frame[1] = GetAnim(Type, 1, 1, PlayerColor); //charge2
	Frame[2] = GetAnim(Type, 2, 1, PlayerColor); //3D 1
	Frame[3] = GetAnim(Type, 2, 2, PlayerColor); //3D 2
	Frame[4] = GetAnimFlip(Type, 2, 1, PlayerColor); //3D 3
	Frame[5] = GetAnimFlip(Type, 2, 2, PlayerColor); //3D 4

	int direction = 0; //right
	if(Position.left > dest.left)
		direction = 1; //left
	
	BBitmap* idle = CurrentImage;
	BRect Invalidator;
	
	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED*2);

	CurrentImage = Frame[1]; 
	Update(Position); 
	snooze(SPEED*2);

	int CurrentFrame = 2;
	SEffect()->Position = Position; //3D Now !!!!!
	SEffect()->Valid = true;

	while(SEffect()->Position != dest)
	{
		CurrentFrame++;
		if(CurrentFrame > 5)
			CurrentFrame = 2;

		SEffect()->CurrentImage = Frame[CurrentFrame];
		Invalidator = SEffect()->Position;	

		if(SEffect()->Position.left < dest.left) 		SEffect()->Position.OffsetBy(8,0);		
		else if(SEffect()->Position.left > dest.left) 	SEffect()->Position.OffsetBy(-8,0);		

		if(SEffect()->Position.top < dest.top) 			SEffect()->Position.OffsetBy(0,8);		
		else if(SEffect()->Position.top > dest.top) 	SEffect()->Position.OffsetBy(0,-8);		

		SEffect()->Update(Invalidator);	
		snooze((int)SPEED/2);
	}

	Unit->Hit(!((bool) direction));		
	SEffect()->Valid = false;
	Update(SEffect()->Position);

	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED*2);
	
	CurrentImage = idle; 
	Update(Position);

	Unit->SetHP(-1); //1 pts de degat
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
void Be4::DoAction3(BeUnit* Unit, BRect dest) 
{//Heal
	BBitmap* Frame[3];
	
	//Load les Images du move pour lui
	Frame[0] = GetAnim(Type, 1, 0, PlayerColor); //charge 1
	Frame[1] = GetAnimFlip(Type, 1, 1, PlayerColor); //charge 2
	Frame[2] = GetAnim(Type, 2, 0, PlayerColor); //Heal

	BBitmap* idle = CurrentImage;
	
	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED*2);

	CurrentImage = Frame[1]; 
	Update(Position);
	snooze(SPEED*2);

	SEffect()->Position = dest;
	SEffect()->CurrentImage = Frame[2];
	SEffect()->Valid = true;
	
	//Fade In the healing sign
	rgb_color c;
	for(int a = 15; a <= 255; a+=16)
	{
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = a;
			}
		Update(dest);
		snooze((int)SPEED/2);
	}

	//Fade Out the healing sign
	for(int a = 240; a >= 0; a-=16)
	{
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = a;
			}
		Update(dest);
		snooze((int)SPEED/2);
	}

	SEffect()->Valid = false;
	Update(dest);

	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED*2);
	
	CurrentImage = idle; 
	Update(Position);

	//Heal
	Unit->SetHP(rand()%2 + 4); //4-6 heal
	Recovery = 5;

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
}
//--------------------------------------------------------------------------------------------------
void Be4::DoAction4(BeUnit* Unit, BRect dest)
{//Purify : remove special effect
	BBitmap* Frame[4];
	
	Frame[0] = GetAnim(Type, 1, 0, PlayerColor); //charge 1
	Frame[1] = GetAnimFlip(Type, 1, 1, PlayerColor); //charge 2
	Frame[2] = GetAnim(Type, 5, 1, PlayerColor); //spinning square 1
	Frame[3] = GetAnim(Type, 6, 1, PlayerColor); //spinning square 2

	BBitmap* idle = CurrentImage;
	
	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED*2);

	CurrentImage = Frame[1]; 
	Update(Position); 
	snooze(SPEED*2);

	SEffect()->Position = dest;
	SEffect()->CurrentImage = Frame[2];;
	SEffect()->Valid = true;
	bool square = true;
	double acceleration = SPEED*2;

	rgb_color c;
	for(int a = 240; a >= 0; a-=8)
	{//accelarate the rotation while fading
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)];
				if(!(c.red == 255 && c.green == 0 && c.blue == 255))
					((rgb_color *)SEffect()->CurrentImage->Bits())[(i*32 + j)].alpha = a;
			}
		Update(dest);
		snooze((int)acceleration);

		acceleration /= 1.2;
		SEffect()->CurrentImage = square ? Frame[3] : Frame[2];
		square = !square;
	}

	SEffect()->Valid = false;
	Update(dest);

	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED*2);
	
	CurrentImage = idle; 
	Update(Position);
	Unit->CastRemove();
	Recovery += 5;

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
}
//--------------------------------------------------------------------------------------------------
void Be4::DoAction5(BeUnit* Unit, BRect dest) 
{//Drain
	BBitmap* Frame[4];
	Frame[0] = GetAnim(Type, 5, 0, PlayerColor); //casting
	Frame[1] = GetAnimFlip(Type, 5, 0, PlayerColor);
	Frame[2] = GetAnim(Type, 6, 0, PlayerColor); //drain
	Frame[3] = GetAnimFlip(Type, 6, 0, PlayerColor);

	BRect Origine = Position;
	BBitmap* idle = CurrentImage;
	int direction;
	BRect target = dest;
	
	if(Position.left > dest.left || dest.left == 0)
	{dest.OffsetBy(32,0); direction = 1;}
	
	else
	{dest.OffsetBy(-32,0); direction = 0;}

	DoAction1(this, dest);
	
	//on decharge
	CurrentImage = Frame[0+direction];
	Update(Position);
	snooze(SPEED);

	//Zap
	SEffect()->Position = target;
	SEffect()->CurrentImage = Frame[2+direction];
	SEffect()->Valid = true;

	for(int i = 0; i < 5; i++)
	{
		Unit->Hit(!((bool) direction));		
		Update(SEffect()->Position);
		snooze((int)SPEED/2);
	}

	int dammage = rand()%6 + 1;
	Unit->SetHP(-dammage); //sucking 1-6 HP
	SetHP(dammage);
	Recovery += 1; //6(2 walks) + 1 = 7

	SEffect()->Valid = false;
	Update(SEffect()->Position);

	CurrentImage = idle; 
	Update(Position);

	DoAction1(this, Origine);

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
}
//--------------------------------------------------------------------------------------------------
void Be4::DoAction6(BeUnit* b, BRect r)
{//pass
	BBitmap* Frame[2];
	Frame[0] = GetAnim(Type, 3, 0, PlayerColor);
	Frame[1] = GetAnim(Type, 3, 1, PlayerColor);

	BBitmap* idle = CurrentImage;

	CurrentImage = Frame[0]; 
	Update(Position);
	snooze(SPEED * 2);

	CurrentImage = Frame[1]; 
	Update(Position); 
	snooze(SPEED);

	//Doing a transparency show
	rgb_color c;
	rgb_color ton;
	ton.alpha = 254;
	for(int i = 0; i < 32; i++)
		for(int j = 0; j < 32; j++)
		{
			c = ((rgb_color *)CurrentImage->Bits())[(i*32 + j)];
			if((c.red == PlayerColor.red && c.green == PlayerColor.green && c.blue == PlayerColor.blue))
				((rgb_color *)CurrentImage->Bits())[(i*32 + j)].alpha = 254;
		}

	for(int a = 0; a < 255; a+=32)
	{
		ton.red = a; ton.green = a; ton.blue = a;
		for(int i = 0; i < 32; i++)
			for(int j = 0; j < 32; j++)
			{
				c = ((rgb_color *)CurrentImage->Bits())[(i*32 + j)];
				if(c.alpha == 254)
					((rgb_color *)CurrentImage->Bits())[(i*32 + j)] = ton;
			}
		Update(Position);
		snooze((int)SPEED/2);
	}
	
	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED * 2);

	CurrentImage = idle; 
	Update(Position);

	//clean up
	delete Frame[0];
	delete Frame[1];
}
//--------------------------------------------------------------------------------------------------
bool Be4::OkTarget1(BeUnit* Unit, bool ValidUnit, BRect dest)
{//Spin
	if(ValidUnit || (Distance(dest, Position) > 5)) 
		return false;
	
	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be4::OkTarget2(BeUnit* Unit, bool ValidUnit, BRect dest)
{//3D Attack
	if(!ValidUnit || (Distance(dest, Position) > 7) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false; 

	return true;
} 
//--------------------------------------------------------------------------------------------------
bool Be4::OkTarget3(BeUnit* Unit, bool ValidUnit, BRect dest) 
{//Heal
	if(!ValidUnit || (Distance(dest, Position) > 7) || (Unit == this) || (Unit->GetPlayer() != Player))
		return false; 

	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be4::OkTarget4(BeUnit* Unit, bool ValidUnit, BRect dest)
{//Remove special effect
	if(!ValidUnit || (Distance(dest, Position) > 7))
		return false;

	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be4::OkTarget5(BeUnit* Unit, bool ValidUnit, BRect dest) 
{//Drain
	if(!ValidUnit || (Distance(dest, Position) > 3) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false;
	
	return true;
}
//--------------------------------------------------------------------------------------------------
void Be4::Hit(bool direction)
{
	BBitmap* Ouch;
	if(direction) 	Ouch = GetAnimFlip(Type, 4, 0, PlayerColor);
	else			Ouch = GetAnim(Type, 4, 0, PlayerColor);
			
	BBitmap* idle = CurrentImage;
	
	CurrentImage = Ouch;
	Update(Position); 
	snooze(SPEED * 4);
	
	CurrentImage = idle; 
	Update(Position);

	delete Ouch;
}
//--------------------------------------------------------------------------------------------------
void Be4::Stupid()
{
	BBitmap* Frame[4];
	Frame[0] = GetAnim(Type, 4, 1, PlayerColor);
	Frame[1] = GetAnim(Type, 4, 2, PlayerColor);
	Frame[2] = GetAnimFlip(Type, 4, 1, PlayerColor);
	Frame[3] = GetAnimFlip(Type, 4, 2, PlayerColor);
			
	BBitmap* idle = CurrentImage;
	
	CurrentImage = Frame[0];
	Update(Position); 
	snooze(SPEED * 2);
	
	CurrentImage = Frame[1];
	Update(Position); 
	snooze(SPEED * 2);

	CurrentImage = Frame[0];
	Update(Position); 
	snooze(SPEED * 2);

	CurrentImage = idle;
	Update(Position); 
	snooze(SPEED * 2);

	CurrentImage = Frame[2];
	Update(Position); 
	snooze(SPEED * 2);

	CurrentImage = Frame[3];
	Update(Position); 
	snooze(SPEED * 2);

	CurrentImage = Frame[2];
	Update(Position); 
	snooze(SPEED * 2);

	CurrentImage = idle; 
	Update(Position);

	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
}
//-----------------------------------------------------------------
void Be4::Kill()
{//Tombstone
  	delete CurrentImage;
	BBitmap* Frame[3];
 	Frame[0] = GetAnim(Type, 7, 2, PlayerColor);
 	Frame[1] = GetAnim(Type, 6, 2, PlayerColor);
 	Frame[2] = GetAnim(Type, 5, 2, PlayerColor);

	CurrentImage = Frame[0];
	Update(Position);
	snooze(SPEED*2);

	CurrentImage = Frame[1];
	Update(Position);
	snooze(SPEED*2);

	CurrentImage = Frame[2];
	Update(Position);
	snooze(SPEED*10);

	delete Frame[0];
	delete Frame[1];
	//don't delete Frame[3]
}
//-----------------------------------------------------------------

