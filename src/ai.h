#ifndef AI_H
#define AI_H
#include <list> //list from Standard Template Library
#include "dview.h"
#include "beunit.h"
//---------------------------------------------------------------------
typedef	std::list<Item>::iterator ITER;
//---------------------------------------------------------------------
struct Action //A possible action
{
	ITER Target;
	int UnitAction; //which action that is
	int Rating; //[1, n] Rating of the action (1 being the worst)

	Action() {UnitAction=6; Rating=1;}
	void Set(ITER i, int u, int r) {Target=i; UnitAction=u; Rating=r;}
};
//---------------------------------------------------------------------
class MrAI //Mister AI :)
{
 private:
	Action BestAction[3];

 public:
	MrAI() {};
	void AddAction(Action&); //Add a possible action the the list
	Action GetChoosenOption(); //return the action choosed by the AI
};
//---------------------------------------------------------------------
#endif
