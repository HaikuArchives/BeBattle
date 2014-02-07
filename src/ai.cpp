#include "ai.h"
#include "Window.h"
#include <stdlib.h> //rand
#include <stdio.h> //printf
/*----------------------------------------------------------------------
A.I. ROUTINES
Rating explication :
10-Kill target with short recovery
9-Kill target with medium recovery
8-May kill target with short recovery
7-Kill target with long recovery
6-May kill target with medium recovery
5-Weaken Target with short recovery
4-May kill target with long recovery
3-Weaken Target with medium recovery
2-Weaken Target with long recovery
1-Walking or doing Nothing
0-Don't do that !
----------------------------------------------------------------------*/
void MrAI::AddAction(Action& A)
{//Add a possible action if it is good enough
	for(int i=0; i<3; i++)
	{
		if(BestAction[i].UnitAction == 6 && A.Rating != 0)
		{//Place it there, because anything is better than doing nothing
			BestAction[i] = A;
			return;
		}

		else if(BestAction[i].Rating < A.Rating)
		{//replace it if better
			Action FlushedAction = BestAction[i];
			BestAction[i] = A;
			AddAction(FlushedAction); //try to replace it somewhere
			return;
		}
	}//Wasn't good enough for Mr.AI, so we discard it.
}
//----------------------------------------------------------------------
Action MrAI::GetChoosenOption()
{//return the action choosed by the AI
	int totalRating = 0;
	for(int i=0; i<3; i++)
		totalRating += BestAction[i].Rating;

	int choice = rand()%totalRating;

	if(choice < BestAction[0].Rating || (BestAction[1].Rating + BestAction[2].Rating == 2))
		return BestAction[0]; //Choosed action or the following actions are dumb. 

	else if(choice < (BestAction[0].Rating + BestAction[1].Rating) || (BestAction[2].Rating == 1))
		return BestAction[1]; //Choosed action or the last action is not appropriate
		
	return BestAction[2];
}
//----------------------------------------------------------------------
bool DView::GetAround(BRect self, BRect target, BRect& result)
{//Mr AI try to find a spot near the target.
	result = self;
	BRect test = self;
	int move = 5;
	
	while((test != target) && move)
	{
		//try to move horizontally toward target
		if(test.left < target.left)			test.OffsetBy(32, 0);
		else if(test.left > target.left)	test.OffsetBy(-32, 0);
		if(!Occupied(test))					result = test;

		//try to move vertically toward target
		if(test.top < target.top)		test.OffsetBy(0, 32);
		else if(test.top > target.top)	test.OffsetBy(0,-32);
		if(!Occupied(test))				result = test;

		move--; //move one more step
	}

	return (result != self);
}
//----------------------------------------------------------------------
void DView::PlayAI()
{
	switch((*Active).Unit->Type)
	{
		case 1: AI1();break;
		case 2: AI2();break;
		case 3: AI3();break;
		case 4: AI4();break;
		case 5: AI5();break;
	}
}
//----------------------------------------------------------------------
bool DView::Occupied(BRect target)
{//going toward target
	for(ITER checker = Liste.begin(); checker != Liste.end(); checker++)
		if((*checker).Unit->GetPosition() == target)
			return true;

	return false;
}
//----------------------------------------------------------------------
void DView::AI1()
{//normal guy AI
	MrAI NormalAI;
	Action A;
	int rating;
	int shortDist = 20; //distance to closest enemy
	BRect shortDest; //position of the closest enemy
	int currentDist; //distance from current target
	BRect Dest; //where to move...
		
	//search a target
	for(ITER checker = Liste.begin(); checker != Liste.end(); checker++)
	{
		if((*Active).Unit->OkTarget2((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//punch
			if((*checker).Unit->GetHP() < 3) rating = 10; //10-Kill target with short recovery
			else if((*checker).Unit->GetHP() == 10) rating = 1; //not the best move
			else rating = 5; //5-Weaken Target with short recovery

			if((*Active).Unit->IsSpeed()) rating+=2; //good action if on speed
			if((*Active).Unit->IsSlow()) rating-=2;

			A.Set(checker, 2, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget3((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Throw head
			if((*checker).Unit->GetHP() < 5) rating = 7; //7-Kill target with long recovery
			if((*checker).Unit->GetHP() < 7) rating = 4; //4-May kill target with long recovery
			else rating = 2;  //2-Weaken Target with long recovery

			A.Set(checker, 3, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget4((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//arm-smash
			if((*checker).Unit->GetHP() < 2) rating = 9; //9-Kill target with medium recovery
			else if((*checker).Unit->GetHP() < 5 ) rating = 6; //6-May kill target with medium recovery
			else rating = 3; //3-Weaken Target with medium recovery

			A.Set(checker, 4, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget5((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//heal
			if((*checker).Unit->GetHP() == 10) rating = 0; //don't do it...
			else rating = 11 - (*checker).Unit->GetHP(); //the less energy the more likely lo heal.
			if((*Active).Unit->IsPoison() && ((*Active).Unit->GetHP() < 3)) rating = 10; //our best action before we die

			A.Set(checker, 5, rating);
			NormalAI.AddAction(A); 
		}
		
		currentDist = (*Active).Unit->Distance((*Active).Unit->GetPosition(), (*checker).Unit->GetPosition());
		if((currentDist < shortDist) &&	((*Active).Unit->GetPlayer() != (*checker).Unit->GetPlayer()))
		{
			shortDist = currentDist;
			shortDest = (*checker).Unit->GetPosition();
			if(GetAround((*Active).Unit->GetPosition(), shortDest, Dest))
			{
				A.Set(checker, 1, 1);
				NormalAI.AddAction(A);
			}
		}
	}

	
	A = NormalAI.GetChoosenOption();
	//The choice is done, start the animation...
	Animating = true; //we start animating
	Invalidate(Target); //hide the spot
	Window()->UpdateIfNeeded();

	switch(A.UnitAction)
	{
		case 1: (*Active).Unit->DoAction1(NULL, Dest);break;
		case 2: (*Active).Unit->DoAction2((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 3: (*Active).Unit->DoAction3((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 4: (*Active).Unit->DoAction4((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 5: (*Active).Unit->DoAction5((*A.Target).Unit, (*A.Target).Unit->GetPosition());break; 
		case 6:	(*Active).Unit->DoAction6(NULL, BRect(0,0,0,0));break;
	}
	
	(*Active).Unit->ApplyEffect();
	GetNext();
}
//----------------------------------------------------------------------
void DView::AI2()
{//Boxer AI
	MrAI NormalAI;
	Action A;
	int rating;
	int shortDist = 20; //distance to closest enemy
	BRect shortDest; //position of the closest enemy
	int currentDist; //distance from current target
	BRect Dest; //where to move...
		
	//search a target
	for(ITER checker = Liste.begin(); checker != Liste.end(); checker++)
	{
		if((*Active).Unit->OkTarget2((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//punch
			if((*checker).Unit->GetHP() < 4) rating = 10; //10-Kill target with short recovery
			else if((*checker).Unit->GetHP() == 10) rating = 1; //not the best move
			else rating = 5; //5-Weaken Target with short recovery

			if((*Active).Unit->IsSpeed()) rating+=2; //good action if on speed
			if((*Active).Unit->IsSlow()) rating-=2;

			A.Set(checker, 2, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget3((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Punch Fury
			if((*checker).Unit->GetHP() < 4) rating = 7; //7-Kill target with long recovery
			else rating = 6; //4-May kill target with long recovery + 2(because we can kill a unit with full HP)

			if((*Active).Unit->IsPoison() && ((*Active).Unit->GetHP() < 3)) rating = 10; //our best action before we die

			A.Set(checker, 3, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget4((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Shotgun
			if((*checker).Unit->GetHP() < 3) rating = 9; //9-Kill target with medium recovery
			else if((*checker).Unit->GetHP() < 7 ) rating = 6; //6-May kill target with medium recovery
			else rating = 3; //3-Weaken Target with medium recovery

			A.Set(checker, 4, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget5((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Slow
			if((*checker).Unit->IsSlow() || (*checker).Unit->IsStun()) rating = 0; //not useful
			else if((*checker).Unit->GetHP() > 5) rating = 3; //3-Weaken Target with medium recovery
			else rating = 2; //Target may die soon, so...

			A.Set(checker, 5, rating);
			NormalAI.AddAction(A); 
		}
		
		currentDist = (*Active).Unit->Distance((*Active).Unit->GetPosition(), (*checker).Unit->GetPosition());
		if((currentDist < shortDist) &&	((*Active).Unit->GetPlayer() != (*checker).Unit->GetPlayer()))
		{
			shortDist = currentDist;
			shortDest = (*checker).Unit->GetPosition();
			if(GetAround((*Active).Unit->GetPosition(), shortDest, Dest))
			{
				A.Set(checker, 1, 1);
				NormalAI.AddAction(A);
			}
		}
	}

	A = NormalAI.GetChoosenOption();
	//The choice is done, start the animation...
	Animating = true; //we start animating
	Invalidate(Target); //hide the spot
	Window()->UpdateIfNeeded();

	switch(A.UnitAction)
	{
		case 1: (*Active).Unit->DoAction1(NULL, Dest);break;
		case 2: (*Active).Unit->DoAction2((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 3: (*Active).Unit->DoAction3((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 4: (*Active).Unit->DoAction4((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 5: (*Active).Unit->DoAction5((*A.Target).Unit, (*A.Target).Unit->GetPosition());break; 
		case 6:	(*Active).Unit->DoAction6(NULL, BRect(0,0,0,0));break;
	}
	
	(*Active).Unit->ApplyEffect();
	GetNext();
}
//----------------------------------------------------------------------
void DView::AI3()
{//Magician AI
	MrAI NormalAI;
	Action A;
	int rating;
	int shortDist = 20; //distance to closest enemy
	BRect shortDest; //position of the closest enemy
	int currentDist; //distance from current target
	BRect Dest; //where to move...

	//search a target
	for(ITER checker = Liste.begin(); checker != Liste.end(); checker++)
	{
		if((*Active).Unit->OkTarget2((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Zap
			if((*checker).Unit->GetHP() < 2) rating = 10; //10-Kill target with short recovery
			else if((*checker).Unit->GetHP() < 4) rating = 8;  //8-May kill target with short recovery
			else if((*checker).Unit->GetHP() == 10) rating = 1; //not the best move
			else rating = 5;  //5-Weaken Target with short recovery

			if((*Active).Unit->IsSpeed()) rating+=2; //good action if on speed
			if((*Active).Unit->IsSlow()) rating-=2;

			A.Set(checker, 2, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget3((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Fireball
			if((*checker).Unit->GetHP() < 4) rating = 9; //9-Kill target with medium recovery
			else if((*checker).Unit->GetHP() < 8) rating = 6; //6-May kill target with medium recovery
			else rating = 5; //3-Weaken Target with medium recovery

			if((*Active).Unit->IsPoison() && ((*Active).Unit->GetHP() < 3)) rating = 10; //our best action before we die

			A.Set(checker, 3, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget4((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Poison
			if((*checker).Unit->IsPoison()) rating = 0; //already poisonned 
			else if((*checker).Unit->GetHP() > 5) rating = 3; //3-Weaken Target with medium recovery
			else rating = 1; //Target unit may be dying soon...

			A.Set(checker, 4, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget5((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Color Change
			if((*checker).Unit->GetHP() == 10) rating = 0; //would not work
			else rating = 11 - (*checker).Unit->GetHP(); //the less the HP the more we convert :)

			A.Set(checker, 5, rating);
			NormalAI.AddAction(A); 
		}
		
		currentDist = (*Active).Unit->Distance((*Active).Unit->GetPosition(), (*checker).Unit->GetPosition());
		if((currentDist < shortDist) &&	((*Active).Unit->GetPlayer() != (*checker).Unit->GetPlayer()))
		{
			shortDist = currentDist;
			shortDest = (*checker).Unit->GetPosition();
			if(GetAround((*Active).Unit->GetPosition(), shortDest, Dest))
			{
				A.Set(checker, 1, 1);
				NormalAI.AddAction(A);
			}
		}
	}

	A = NormalAI.GetChoosenOption();
	//The choice is done, start the animation...
	Animating = true; //we start animating
	Invalidate(Target); //hide the spot
	Window()->UpdateIfNeeded();

	switch(A.UnitAction)
	{
		case 1: (*Active).Unit->DoAction1(NULL, Dest);break;
		case 2: (*Active).Unit->DoAction2((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 3: (*Active).Unit->DoAction3((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 4: (*Active).Unit->DoAction4((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 5: (*Active).Unit->DoAction5((*A.Target).Unit, (*A.Target).Unit->GetPosition());break; 
		case 6:	(*Active).Unit->DoAction6(NULL, BRect(0,0,0,0));break;
	}
	
	(*Active).Unit->ApplyEffect();
	GetNext();
}
//----------------------------------------------------------------------
void DView::AI4()
{//Medic AI
	MrAI NormalAI;
	Action A;
	int rating;
	int shortDist = 20; //distance to closest enemy
	BRect shortDest; //position of the closest enemy
	int currentDist; //distance from current target
	BRect Dest; //where to move...
		
	//search a target
	for(ITER checker = Liste.begin(); checker != Liste.end(); checker++)
	{
		if((*Active).Unit->OkTarget2((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//3D Attack
			if((*checker).Unit->GetHP() < 2) rating = 10; //10-Kill target with short recovery
			else if((*checker).Unit->GetHP() == 10) rating = 1; //not the best move
			else rating = 4;  //5-Weaken Target with short recovery - 1 (not really weakening)

			if((*Active).Unit->IsSpeed()) rating+=2; //good action if on speed
			if((*Active).Unit->IsSlow()) rating-=2;

			A.Set(checker, 2, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget3((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Heal
			if((*checker).Unit->GetHP() == 10) rating = 0;
			else rating = 11 - (*checker).Unit->GetHP();

			A.Set(checker, 3, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget4((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Purify
			if((*checker).Unit->GetPlayer() == 1)
			{//it's a human unit
				if((*checker).Unit->IsSpeed() && !(*checker).Unit->IsSlow() && !(*checker).Unit->IsStun() && !(*checker).Unit->IsPoison())
					rating = 3; //to remove the speed from opposing unit
				else
					rating = 0; //not a good idea, we might remove something that we put on him
			}

			else //cpu unit
			{//it's a cpu unit
				rating = 0;
				if((*checker).Unit->IsSpeed()) rating -= 3;
				if((*checker).Unit->IsSlow()) rating += 3;
				if((*checker).Unit->IsStun()) rating += 3;
				if((*checker).Unit->IsPoison()) rating += 3;
				if(rating < 0) rating = 0; //unit has no nasty spell on him
			}

			A.Set(checker, 4, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget5((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Drain
			if((*checker).Unit->GetHP() < 2) rating = 7; //7-Kill target with long recovery
			else if((*checker).Unit->GetHP() < 7) rating = 4; //4-May kill target with long recovery
			else rating = 11-(*Active).Unit->GetHP(); //do we need to heal ourself

			if((*Active).Unit->IsPoison() && ((*Active).Unit->GetHP() < 3)) rating = 10; //our best action before we die

			A.Set(checker, 5, rating);
			NormalAI.AddAction(A); 
		}
		
		currentDist = (*Active).Unit->Distance((*Active).Unit->GetPosition(), (*checker).Unit->GetPosition());
		if((currentDist < shortDist) &&	((*Active).Unit->GetPlayer() != (*checker).Unit->GetPlayer()))
		{
			shortDist = currentDist;
			shortDest = (*checker).Unit->GetPosition();
			if(GetAround((*Active).Unit->GetPosition(), shortDest, Dest))
			{
				A.Set(checker, 1, 1);
				NormalAI.AddAction(A);
			}
		}
	}

	A = NormalAI.GetChoosenOption();
	//The choice is done, start the animation...
	Animating = true; //we start animating
	Invalidate(Target); //hide the spot
	Window()->UpdateIfNeeded();

	switch(A.UnitAction)
	{
		case 1: (*Active).Unit->DoAction1(NULL, Dest);break;
		case 2: (*Active).Unit->DoAction2((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 3: (*Active).Unit->DoAction3((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 4: (*Active).Unit->DoAction4((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 5: (*Active).Unit->DoAction5((*A.Target).Unit, (*A.Target).Unit->GetPosition());break; 
		case 6:	(*Active).Unit->DoAction6(NULL, BRect(0,0,0,0));break;
	}
	
	(*Active).Unit->ApplyEffect();
	GetNext();
}
//----------------------------------------------------------------------
void DView::AI5()
{//Small one AI
	MrAI NormalAI;
	Action A;
	int rating;
	int shortDist = 20; //distance to closest enemy
	BRect shortDest; //position of the closest enemy
	int currentDist; //distance from current target
	BRect Dest; //where to move...
		
	//search a target
	for(ITER checker = Liste.begin(); checker != Liste.end(); checker++)
	{
		if((*Active).Unit->OkTarget2((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Neandethal club
			if((*checker).Unit->GetHP() < 5) rating = 10; //10-Kill target with short recovery
			else if((*checker).Unit->GetHP() < 9) rating = 8;  //8-May kill target with short recovery
			else if((*checker).Unit->GetHP() == 10) rating = 1; //not the best move
			else rating = 5;  //5-Weaken Target with short recovery

			A.Set(checker, 2, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget3((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Bazooka
			if((*checker).Unit->GetHP() < 5) rating = 9; //9-Kill target with medium recovery
			else rating = 3; //3-Weaken Target with medium recovery

			A.Set(checker, 3, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget4((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Dynamite
			if((*checker).Unit->GetHP() > (*Active).Unit->GetHP()) rating = 7;  //7-Kill target with long recovery
			else rating = 1; //not a good sacrifice

			if((*Active).Unit->IsPoison() && ((*Active).Unit->GetHP() < 3)) rating = 10; //our best action before we die

			A.Set(checker, 4, rating);
			NormalAI.AddAction(A); 
		}

		if((*Active).Unit->OkTarget5((*checker).Unit, true, (*checker).Unit->GetPosition()))
		{//Speed up !
			if((*checker).Unit->IsSpeed() || (*checker).Unit->IsStun()) rating = 0; //not useful
			else if((*checker).Unit->GetHP() > 5) rating = 3; //good investment
			else rating = 2; //Target may die soon, so...

			A.Set(checker, 5, rating);
			NormalAI.AddAction(A); 
		}
		
		currentDist = (*Active).Unit->Distance((*Active).Unit->GetPosition(), (*checker).Unit->GetPosition());
		if((currentDist < shortDist) &&	((*Active).Unit->GetPlayer() != (*checker).Unit->GetPlayer()))
		{
			shortDist = currentDist;
			shortDest = (*checker).Unit->GetPosition();
			if(GetAround((*Active).Unit->GetPosition(), shortDest, Dest))
			{
				A.Set(checker, 1, 1);
				NormalAI.AddAction(A);
			}
		}
	}

	A = NormalAI.GetChoosenOption();
	//The choice is done, start the animation...
	Animating = true; //we start animating
	Invalidate(Target); //hide the spot
	Window()->UpdateIfNeeded();

	switch(A.UnitAction)
	{
		case 1: (*Active).Unit->DoAction1(NULL, Dest);break;
		case 2: (*Active).Unit->DoAction2((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 3: (*Active).Unit->DoAction3((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 4: (*Active).Unit->DoAction4((*A.Target).Unit, (*A.Target).Unit->GetPosition());break;
		case 5: (*Active).Unit->DoAction5((*A.Target).Unit, (*A.Target).Unit->GetPosition());break; 
		case 6:	(*Active).Unit->DoAction6(NULL, BRect(0,0,0,0));break;
	}
	
	(*Active).Unit->ApplyEffect();
	GetNext();
}
//----------------------------------------------------------------------
