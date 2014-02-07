#include "be2.h"
#include <Rect.h>
#define SPEED 75000
//--------------------------------------------------------------------
Be2::Be2(int w, int p)
{
	Position.top = ((int)(w/14)) * 32;
	Position.left = (w % 14) * 32;
	Position.bottom = Position.top + 31;
	Position.right = Position.left + 31;
	HP = 10;
	Player = p;
	Type = 2;
	CastRemove();
	Recovery = (rand() % 3) + 4;

	switch(Player)
	{
		case 1:PlayerColor.red=254; PlayerColor.green=0; PlayerColor.blue=0; PlayerColor.alpha=255;break;
		case 2:PlayerColor.red=0; PlayerColor.green=0; PlayerColor.blue=254; PlayerColor.alpha=255;break;
	}

	CurrentImage = GetAnim(Type, 6, 0, PlayerColor);
}
//---------------------------------------------------------------------------------
void Be2::DoAction1(BeUnit* Unit, BRect dest)
{//move
	BBitmap* idle = CurrentImage;
	int CurrentFrame = -1; //idle
	BRect Invalidator;
	
	BBitmap* Frame[6];
	Frame[0] = GetAnim(Type, 0, 2, PlayerColor);
	Frame[1] = GetAnim(Type, 0, 1, PlayerColor);
	Frame[2] = GetAnimFlip(Type, 0, 2, PlayerColor);
	Frame[3] = GetAnimFlip(Type, 0, 1, PlayerColor);
	Frame[4] = GetAnim(Type, 0, 0, PlayerColor);
	Frame[5] = GetAnimFlip(Type, 0, 0, PlayerColor);

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

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
	delete Frame[4];
	delete Frame[5];

	Recovery += 3;
}
//--------------------------------------------------------------------------------------------------
void Be2::DoAction2(BeUnit* Unit, BRect dest)
{//punch
	BBitmap* Frame[4];
	BRect Origine = Position;
	int direction, decalage;
	
	Frame[0] = GetAnim(Type, 5, 0, PlayerColor); //swing
	Frame[1] = GetAnimFlip(Type, 5, 0, PlayerColor);
	Frame[2] = GetAnim(Type, 5, 1, PlayerColor); //punch
	Frame[3] = GetAnimFlip(Type, 5, 1, PlayerColor);

	//where do we go ?
	if(Position.left > dest.left || dest.left == 0)
	{dest.OffsetBy(32,0); direction = 1; decalage = -10;}
	
	else
	{dest.OffsetBy(-32,0); direction = 0; decalage = 10;}

	DoAction1(this, dest);

	BBitmap* idle = CurrentImage;
	BRect Invalidator;
	
	Invalidator = Position; 
	Position.OffsetBy(decalage, 0); //moving forward 1
	CurrentImage = Frame[0+direction]; 
	Update(Invalidator); 
	snooze(SPEED * 2);

	Invalidator = Position; 
	Position.OffsetBy(decalage, 0); //moving forward 2	
	Update(Invalidator); 
	snooze(SPEED);

	CurrentImage = Frame[2+direction]; //punch
	Update(Position); 
	Unit->Hit(!((bool) direction));
	Unit->SetHP(-3); //3 damage
	Recovery -= 3; //6 (2 walk) - 3 = 3
	
	Invalidator = Position; 
	Position.OffsetBy(-decalage, 0); //back off 1
	CurrentImage = Frame[0+direction]; 
	Update(Invalidator); snooze(SPEED);

	Invalidator = Position; 
	Position.OffsetBy(-decalage, 0); //back off 2
	Update(Invalidator); 
	snooze(SPEED);
	
	CurrentImage = idle; Update(Invalidator);

	DoAction1(this, Origine); //retourne a la case depart

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
} 
//---------------------------------------------------------------------------------------
void Be2::DoAction3(BeUnit* Unit, BRect dest) 
{//Punch Fury
	BBitmap* Frame[6];
	BRect Origine = Position;
	int direction, decalage;
	
	//Load les Images du move pour lui
	Frame[0] = GetAnim(Type, 2, 0, PlayerColor);
	Frame[1] = GetAnimFlip(Type, 2, 0, PlayerColor);
	Frame[2] = GetAnim(Type, 2, 1, PlayerColor);
	Frame[3] = GetAnimFlip(Type, 2, 1, PlayerColor);
	Frame[4] = GetAnim(Type, 2, 2, PlayerColor);
	Frame[5] = GetAnimFlip(Type, 2, 2, PlayerColor);

	if(Position.left > dest.left || dest.left == 0)
	{dest.OffsetBy(32,0); direction = 1; decalage = -16;}
	
	else
	{dest.OffsetBy(-32,0); direction = 0; decalage = 16;}
	DoAction1(this, dest);    //a cote de l'assaillit

	BBitmap* idle = CurrentImage;
	BRect Invalidator;
	
	CurrentImage = Frame[2+direction];
	Invalidator = Position;
	Position.OffsetBy(decalage, 0);
	Update(Invalidator); 
	Unit->Hit(!((bool) direction));
	snooze(SPEED);

	CurrentImage = Frame[0+direction];
	Update(Position); 
	Unit->Hit(!((bool) direction));
	snooze(SPEED);

	CurrentImage = Frame[2+direction];
	Update(Position); 
	Unit->Hit(!((bool) direction));
	snooze(SPEED);

	CurrentImage = Frame[0+direction];
	Update(Position); 
	Unit->Hit(!((bool) direction));
	snooze(SPEED);

	CurrentImage = Frame[2+direction];
	Update(Position); 
	Unit->Hit(!((bool) direction));
	snooze(SPEED);

	CurrentImage = Frame[4+direction];
	Update(Position); 
	Unit->Hit(!((bool) direction));
	snooze(SPEED*2);

	Unit->SetHP(-(rand()%8 + 3)); //3-10 dammage
	Recovery += 1; //6 (2 walk) + 1 = 7

	CurrentImage = idle; 
	Invalidator = Position;
	Position.OffsetBy(-decalage, 0);
	Update(Invalidator); 

	DoAction1(this, Origine); //retourne a la case depart

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
	delete Frame[3];
	delete Frame[4];
	delete Frame[5];
}
//--------------------------------------------------------------------------------------------------
void Be2::DoAction4(BeUnit* Unit, BRect dest)
{//shotgun
	BBitmap* Frame[7];
	
	//Load les Images du move pour lui
	Frame[0] = GetAnim(Type, 4, 0, PlayerColor);
	Frame[1] = GetAnimFlip(Type, 4, 0, PlayerColor);
	Frame[2] = GetAnim(Type, 4, 1, PlayerColor);
	Frame[3] = GetAnimFlip(Type, 4, 1, PlayerColor);
	Frame[4] = GetAnim(Type, 4, 2, PlayerColor);
	Frame[5] = GetAnimFlip(Type, 4, 2, PlayerColor);
	Frame[6] = GetAnim(Type, 4, 3, PlayerColor);

	//determine ou pitcher
	int direction = 0; //vers la droite
	if(Position.left > dest.left)
		direction = 1; //vers la gauche
	
	BBitmap* idle = CurrentImage;
	BRect Invalidator;
	
	Invalidator = Position;
	CurrentImage = Frame[0+direction]; 
	Update(Invalidator); 
	snooze(SPEED);

	Invalidator = Position;
	CurrentImage = Frame[2+direction]; 
	Update(Invalidator); 
	snooze(SPEED);

	Invalidator = Position;
	CurrentImage = Frame[0+direction]; 
	Update(Invalidator); 
	snooze(SPEED);

	Invalidator = Position; 
	CurrentImage = Frame[4+direction]; 
	Update(Invalidator); 
	snooze(SPEED);

	SEffect()->Position = Position;
	SEffect()->CurrentImage = Frame[6];
	SEffect()->Valid = true;

	while(SEffect()->Position != dest) //aller
	{
		Invalidator = SEffect()->Position;	

		if(SEffect()->Position.left < dest.left) SEffect()->Position.OffsetBy(16,0);		
		else if(SEffect()->Position.left > dest.left) SEffect()->Position.OffsetBy(-16,0);		

		if(SEffect()->Position.top < dest.top) SEffect()->Position.OffsetBy(0,16);		
		else if(SEffect()->Position.top > dest.top) SEffect()->Position.OffsetBy(0,-16);		

		SEffect()->Update(Invalidator);
		snooze((int)SPEED/2);
	}

	Unit->Hit(!((bool) direction));		
	Unit->SetHP(-(rand()%5 + 2)); //2-6 dammage
	Recovery += 5;
	SEffect()->Valid = false;
	Update(SEffect()->Position); //refresh

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
void Be2::DoAction5(BeUnit* Unit, BRect dest) 
{//slow
	BBitmap* Frame[3];
	
	//Load les Images du move pour lui
	Frame[0] = GetAnim(Type, 7, 0, PlayerColor);     
	Frame[1] = GetAnim(Type, 7, 1, PlayerColor); 	
	Frame[2] = GetAnim(Type, 7, 2, PlayerColor); 		

	SEffect()->Position = dest; //slow !
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
		SEffect()->CurrentImage = Frame[2];
		SEffect()->Update(dest);
		snooze((int)SPEED);
	}
	SEffect()->Valid = false;
	Update(dest); //remet la bonne image
	Unit->CastSlow();
	//on set le nouveau recovery
	Recovery += 5;

	//clean up
	delete Frame[0];
	delete Frame[1];
	delete Frame[2];
}
//--------------------------------------------------------------------------------------------------
void Be2::DoAction6(BeUnit* b, BRect r)
{
	BBitmap* Frame[2];
	
	Frame[0] = GetAnim(Type, 3, 0, PlayerColor);
	Frame[1] = GetAnim(Type, 3, 1, PlayerColor);

	BBitmap* idle = CurrentImage;

	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED * 4);

	CurrentImage = Frame[1]; 
	Update(Position); 
	snooze(SPEED * 4);

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
bool Be2::OkTarget1(BeUnit* Unit, bool ValidUnit, BRect dest)
{//move
	if(ValidUnit || (Distance(Position, dest) > 5))
		return false;
	
	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be2::OkTarget2(BeUnit* Unit, bool ValidUnit, BRect dest)
{//punch
	if(!ValidUnit || (Distance(dest, Position) > 3) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false;

	return true;
} 
//---------------------------------------------------------------------------------------
bool Be2::OkTarget3(BeUnit* Unit, bool ValidUnit, BRect dest) 
{//Punch Fury
	if(!ValidUnit || (Distance(dest, Position) > 3) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false;

	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be2::OkTarget4(BeUnit* Unit, bool ValidUnit, BRect dest)
{//shotgun
	if(!ValidUnit || (Distance(dest, Position) > 5) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false;

	return true;
}
//--------------------------------------------------------------------------------------------------
bool Be2::OkTarget5(BeUnit* Unit, bool ValidUnit, BRect dest) 
{//slow
	if(!ValidUnit || (Distance(dest, Position) > 7) || (Unit == this) || (Unit->GetPlayer() == Player))
		return false;
	
	return true;
}
//--------------------------------------------------------------------------------------------------
void Be2::Hit(bool direction)
{
	BBitmap* Ouch;
	if(direction) 	Ouch = GetAnim(Type, 6, 1, PlayerColor);
	else			Ouch = GetAnimFlip(Type, 6, 1, PlayerColor);
			
	BBitmap* idle = CurrentImage;
	
	CurrentImage = Ouch;
	Update(Position); 
	snooze(SPEED * 2);
	
	CurrentImage = idle; 
	Update(Position);

	delete Ouch; //clean up
}
//--------------------------------------------------------------------------------------------------
void Be2::Stupid()
{
	BBitmap* Stupid1 = GetAnim(Type, 1, 0, PlayerColor);
	BBitmap* Stupid2 = GetAnim(Type, 1, 1, PlayerColor);
			
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
void Be2::Kill()
{
	BBitmap* Frame[3];
	
	Frame[0] = GetAnim(Type, 6, 1, PlayerColor);
	Frame[1] = GetAnimFlip(Type, 6, 1, PlayerColor);
	Frame[2] = GetAnim(Type, 6, 2, PlayerColor);
	delete CurrentImage;

	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED * 2);
	CurrentImage = Frame[1]; 
	Update(Position); 
	snooze(SPEED * 2);
	CurrentImage = Frame[0]; 
	Update(Position); 
	snooze(SPEED * 2);
	CurrentImage = Frame[1]; 
	Update(Position); 
	snooze(SPEED * 2);
	CurrentImage = Frame[2]; 
	Update(Position); 
	snooze(SPEED * 4);

	//clean up
	delete Frame[0];
	delete Frame[1];
	//don't erase Frame[3].
}
//-----------------------------------------------------------------
