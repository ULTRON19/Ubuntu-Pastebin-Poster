#include "mainmodule.h"

int WINAPI WinMain (_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	MAINMODULE* MainModule = MAINMODULE::GetInstance ();
	
	if (!MainModule -> Initialize (hInstance))
		return -1;
		
	return MainModule -> EnterLoop ();
}
