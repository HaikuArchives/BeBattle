#include "main.h"
//----------------------------------------------------------------------
int main()
{
	Application *App = new Application();
	App->Run();
	delete App;
	return 0;
}
//----------------------------------------------------------------------
Application::Application() : BApplication("application/x-vnd.BeBattle")
{
	B = new BeBattle(); //main window
	About = new AboutBox();
	About->Run();
}
//----------------------------------------------------------------------
