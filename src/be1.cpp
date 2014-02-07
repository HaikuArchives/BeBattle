#include "be1.h"
#include <Rect.h>
#define SPEED 75000
//--------------------------------------------------------------------
Be1::Be1(int w, int p)
{
	Position.top = ((int)(w/14)) * 32;
	Position.left = (w % 14) * 32;
	Position.bottom = Position.top + 31;
	Position.right = Position.left + 31;
	HP = 10;
	Player = p;
	Type = 1;
	CastRemove();
	Recovery = (rand() % 3) + 4;

	switch(Player)
	{
		case 1:PlayerColor.red=254; PlayerColor.green=0; PlayerColor.blue=0; PlayerColor.alpha=255;break;
		case 2:PlayerColor.red=0; PlayerColor.green=0; PlayerColor.blue=254; PlayerColor.alpha=255;break;
	}

	CurrentImage = GetAnim(Type, 4, 0, PlayerColor);
}
//---------------------------------------------------------------------------------
void Be1::DoAction1(BeUnit* Unit, BRect dest)
{//move
	BBitmap* idle = CurrentImage;
	int CurrentFrame = -1; //idle
	BRect Invalidator;
	
	BBitmap* Frame[6];
	Frame[0] = GetAnim(Type, 9, 1, PlayerColor);
	Frame[1] = GetAnim(Type, 9, 2, PlayerColor);
	Frame[2] = GetAnimFlip(Type, 9, 1, PlayerColor);
	Frame[3] = GetAnimFlip(Type, 9, 2, PlayerColor);
	Frame[4] = GetAnim(Type, 9, 0, PlayerColor);
	Frame[5] = GetAnimFlip(Type, 9, 0, PlayerColor);

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

	//original image (before the animation started)
	CurrentImage = idle;
	Update(Position);

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
	delete Frame[4];
	delete Frame[5];

	//new recovery
	Recovery += 3;
}
//--------------------------------------------------------------------------------------------------
void Be1::DoAction2(BeUnit* Unit, BRect dest)
{//punch
	BBitmap* Frame[4];
	BRect Origine = Position;
	int direction, decalage;
	
	Frame[0] = GetAnim(Type, 5, 0, PlayerColor); //swing
	Frame[1] = GetAnimFlip(Type, 5, 0, PlayerColor);
	Frame[2] = GetAnim(Type, 5, 1, PlayerColor); //punch 
	Frame[3] = GetAnimFlip(Type, 5, 1, PlayerColor);

	if(Position.left > dest.left || dest.left == 0)
	{dest.OffsetBy(32,0); direction = 1; decalage = -10;}
	
	else
	{dest.OffsetBy(-32,0); direction = 0; decalage = 10;}

	DoAction1(this, dest);

	BBitmap* idle = CurrentImage;
	BRect Invalidator;
	
	Invalidator = Position; 
	Position.OffsetBy(decalage, 0); //move forward 1
	CurrentImage = Frame[0+direction]; 
	Update(Invalidator); 
	snooze(SPEED * 2);

	Invalidator = Position; 
	Position.OffsetBy(decalage, 0); //move forward 2	
	Update(Invalidator); 
	snooze(SPEED);

	CurrentImage = Frame[2+direction]; //punch
	Update(Position); 
	Unit->Hit(!((bool) direction));
	Unit->SetHP(-2);
	Recovery += -3; //2 calls to Action1 = 6 recovery.  Total recovery = 3.

	Invalidator = Position; 
	Position.OffsetBy(-decalage, 0); //back off 1
	CurrentImage = Frame[0+direction]; 
	Update(Invalidator); 
	snooze(SPEED);

	Invalidator = Position; 
	Position.OffsetBy(-decalage, 0); //back off 2
	Update(Invalidator); 
	snooze(SPEED);
	
	CurrentImage = idle; 
	Update(Invalidator);

	DoAction1(this, Origine); //go back to origin

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
} 
//--------------------------------------------------------------------------------------------------
void Be1::DoAction3(BeUnit* Unit, BRect dest) 
{//loosing head ?
	BBitmap* Frame[7];
	
	Frame[0] = GetAnim(Type, 2, 0, PlayerColor); //take head
	Frame[1] = GetAnimFlip(Type, 2, 0, PlayerColor);
	Frame[2] = GetAnim(Type, 2, 1, PlayerColor); //swing head
	Frame[3] = GetAnimFlip(Type, 2, 1, PlayerColor);
	Frame[4] = GetAnim(Type, 2, 2, PlayerColor); //throw head
	Frame[5] = GetAnimFlip(Type, 2, 2, PlayerColor);
	Frame[6] = GetAnim(Type, 2, 3, PlayerColor); //the head

	//where to throw
	int direction = 0; //right
	if(Position.left > dest.left)
		direction = 1; //left
	
	BBitmap* idle = CurrentImage;
	BRect Invalidator;
	
	Invalidator = Position; //take head
	CurrentImage = Frame[0+direction]; 
	Update(Invalidator); 
	snooze(SPEED);

	Invalidator = Position; //swing head
	CurrentImage = Frame[2+direction]; 
	Update(Invalidator); 
	snooze(SPEED);

	Invalidator = Position; //throw head
	CurrentImage = Frame[4+direction]; 
	Update(Invalidator); 
	snooze(SPEED);

	BRect Origine = Position; //so the head knows where to get back
	SEffect()->Position = Origine;
	SEffect()->CurrentImage = Frame[6];
	SEffect()->Valid = true;

	while(SEffect()->Position != dest) //head seeking target
	{
		Invalidator = SEffect()->Position;	

		if(SEffect()->Position.left < dest.left) 		SEffect()->Position.OffsetBy(8,0);		
		else if(SEffect()->Position.left > dest.left) 	SEffect()->Position.OffsetBy(-8,0);		

		if(SEffect()->Position.top < dest.top) 			SEffect()->Position.OffsetBy(0,8);		
		else if(SEffect()->Position.top > dest.top) 	SEffect()->Position.OffsetBy(0,-8);		

		SEffect()->Update(Invalidator);	
		snooze((int)SPEED/2);
	}

	Unit->Hit(!((bool) direction));		
	Unit->SetHP(-(rand()%3 + 3)); //3-5 damage
	Recovery += 7;

	while(SEffect()->Position != Origine) //head coming back
	{
		Invalidator = SEffect()->Position;	

		if(SEffect()->Position.left < Origine.left) 		SEffect()->Position.OffsetBy(8,0);		
		else if(SEffect()->Position.left > Origine.left) 	SEffect()->Position.OffsetBy(-8,0);		

		if(SEffect()->Position.top < Origine.top) 			SEffect()->Position.OffsetBy(0,8);		
		else if(SEffect()->Position.top > Origine.top) 		SEffect()->Position.OffsetBy(0,-8);		

		SEffect()->Update(Invalidator);	
		snooze((int)SPEED/2);
	}

	SEffect()->Valid = false;

	Invalidator = Position; //catch it
	CurrentImage = Frame[2+direction]; 
	Update(Invalidator); 
	snooze(SPEED);

	Invalidator = Position; //fix it on shoulder
	CurrentImage = Frame[0+direction]; 
	Update(Invalidator); 
	snooze(SPEED);
	
	CurrentImage = idle; 
	Update(Invalidator);

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
void Be1::DoAction4(BeUnit* Unit, BRect dest)
{//that should hurt
	BRect target = dest;
	BBitmap* Frame[9];
	BRect Origine = Position;
	int direction, decalage;
	
	Frame[0] = GetAnim(Type, 0, 0, PlayerColor); //take arm
	Frame[1] = GetAnimFlip(Type, 0, 0, PlayerColor);
	Frame[2] = GetAnim(Type, 0, 1, PlayerColor); //rip off arm
	Frame[3] = GetAnimFlip(Type, 0, 1, PlayerColor);
	Frame[4] = GetAnim(Type, 0, 2, PlayerColor); //swin arm
	Frame[5] = GetAnimFlip(Type, 0, 2, PlayerColor);
	Frame[6] = GetAnim(Type, 0, 3, PlayerColor); //Boum !
	Frame[7] = GetAnimFlip(Type, 0, 3, PlayerColor);
	Frame[8] = GetAnim(Type, 3, 1, PlayerColor); //X

	if(Position.left > dest.left || dest.left == 0)
	{dest.OffsetBy(32,0); direction = 1; decalage = -16;}
	
	else
	{dest.OffsetBy(-32,0); direction = 0; decalage = 16;}
	DoAction1(this, dest);

	BBitmap* idle = CurrentImage;
	BRect Invalidator;
	
	CurrentImage = Frame[0+direction]; //take arm
	Update(Position); 
	snooze(SPEED*2);

	CurrentImage = Frame[2+direction]; //rip off arm
	Update(Position); 
	snooze(SPEED*2);

	CurrentImage = Frame[4+direction]; //swing arm
	Update(Position); 
	snooze(SPEED);

	Invalidator = Position;
	Position.OffsetBy(decalage, 0);
	CurrentImage = Frame[6+direction]; 
	Update(Invalidator); 
	Unit->Hit(!((bool) direction));
	Unit->SetHP(-(rand()%4 + 1)); //1-4 dammage
	Recovery -= 1; //6 (2 walks) - 1 = 5
	if(!(rand()%2))
	{
		Unit->CastStun(); //1/2 chance to stun the ennemy unit
		SEffect()->Position = target; //poison !
		SEffect()->CurrentImage = Frame[8];

		for(int i=0; i<4; i++)
		{
			SEffect()->Valid = true;
			SEffect()->Update(target);
			snooze((int)SPEED);
			SEffect()->Valid = false;
			SEffect()->Update(target);
			snooze((int)SPEED);
		}
		Update(target);
	}

	Invalidator = Position; //back off
	Position.OffsetBy(-decalage, 0);
	CurrentImage = Frame[4+direction]; 
	Update(Invalidator); 
	snooze(SPEED);

	CurrentImage = Frame[2+direction]; //put arm back in place
	Update(Position); 
	snooze(SPEED*2);

	CurrentImage = Frame[0+direction]; //ending animation
	Update(Position); 
	snooze(SPEED*2);

	CurrentImage = idle; 
	Update(Invalidator);

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
	delete Frame[8];
}
//--------------------------------------------------------------------------------------------------
void Be1::DoAction5(BeUnit* Unit, BRect dest) 
{//energize
	BBitmap* Frame[4];

	Frame[0] = GetAnim(Type, 6, 0, PlayerColor);
	Frame[1] = GetAnim(Type, 6, 1, PlayerColor);
	Frame[2] = GetAnim(Type, 6, 2, PlayerColor);

	BBitmap* idle = CurrentImage;

	CurrentImage = Frame[0]; Update(Position); snooze((int)(SPEED * 1.5));
	CurrentImage = Frame[1]; Update(Position); snooze((int)(SPEED * 1.5));
	CurrentImage = Frame[2]; Update(Position); snooze((int)(SPEED * 1.5));
	CurrentImage = Frame[1]; Update(Position); snooze((int)(SPEED * 1.5));
	CurrentImage = Frame[0]; Update(Position); snooze((int)(SPEED * 1.5));
	
	CurrentImage = idle; 
	Update(Position);

	//degat a l'unit
	SetHP(5);
	Recovery += 5;

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
}
//--------------------------------------------------------------------------------------------------
void Be1::DoAction6(BeUnit* b, BRect r)
{
	BBitmap* Frame[2];
	Frame[0] = GetAnim(Type, 7, 0, PlayerColor);
	Frame[1] = GetAnim(Type, 7, 1, PlayerColor);

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

	//clean up
	delete Frame[0];
	delete Frame[1];
}
//--------------------------------------------------------------------------------------------------
bool Be1::OkTarget1(BeUnit* Unit, bool ValidUnit, BRect dest)
{//move
	//there is already someone there || too far away
	if(ValidUnit || (Distance(dest, Position) > 5)) 
		return false; 
	
	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be1::OkTarget2(BeUnit* Unit, bool ValidUnit, BRect dest)
{//punch
	//No target || to far || hurting himself || hurting a friend
	if(!ValidUnit || (Distance(dest, Position) > 3) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false;

	return true;
} 
//--------------------------------------------------------------------------------------------------
bool Be1::OkTarget3(BeUnit* Unit, bool ValidUnit, BRect dest) 
{//Flying head
	//No target || to far || hurting himself || hurting a friend
	if(!ValidUnit || (Distance(dest, Position) > 7) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false;

	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be1::OkTarget4(BeUnit* Unit, bool ValidUnit, BRect dest)
{//smashing arm
	//No target || to far || hurting himself || hurting a friend
	if(!ValidUnit || (Distance(dest, Position) > 3) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false;

	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be1::OkTarget5(BeUnit* Unit, bool ValidUnit, BRect dest) 
{//energize
	//Not healing himself
	if(Unit != this)
		return false; 

	return true;
}
//--------------------------------------------------------------------------------------------------
void Be1::Hit(bool direction)
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
void Be1::Stupid()
{
	BBitmap* Stupid1 = GetAnim(Type, 8, 0, PlayerColor);
	BBitmap* Stupid2 = GetAnim(Type, 8, 1, PlayerColor);
			
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

	delete Stupid1; //clean up
	delete Stupid2;
}
//-----------------------------------------------------------------
void Be1::Kill()
{
	BBitmap* Frame[4];
	
	Frame[0] = GetAnim(Type, 1, 1, PlayerColor);
	Frame[1] = GetAnim(Type, 1, 1, PlayerColor);
	Frame[2] = GetAnim(Type, 1, 2, PlayerColor);
	Frame[3] = GetAnim(Type, 1, 3, PlayerColor);
	delete CurrentImage; //won't need that anymore

	//"Crumbling to bits" animation
	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED * 2);

	CurrentImage = Frame[1]; 
	Update(Position); 
	snooze(SPEED * 2);

	CurrentImage = Frame[2]; 
	Update(Position); 
	snooze(SPEED * 2);

	CurrentImage = Frame[3]; 
	Update(Position); 
	snooze(SPEED * 4);
	
	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	//don't erase frame[3] because it's our current frame
}
//-----------------------------------------------------------------



