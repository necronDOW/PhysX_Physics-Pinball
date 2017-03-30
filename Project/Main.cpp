#include <iostream>
#include "VisualDebugger.h"

using namespace std;

int main()
{
	try 
	{
		VisualDebugger::Init("Liam Wilson (13458211) - Physics Demo - 30/03/2017", 800, 800); 
	}
	catch (Exception exc) 
	{ 
		cerr << exc.what() << endl;
		return 0; 
	}

	VisualDebugger::Start();

	return 0;
}