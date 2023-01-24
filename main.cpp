#include <windows.h>

int CALLBACK
WinMain(HINSTANCE hInstance,
		HINSTANCE hPrevInstance, 
		LPSTR hlpCmdLine, 
		int nCmdShow)
{
	MessageBoxW(0, L"This is message box", L"Basdsf", MB_OK|MB_ICONINFORMATION);
	return(0);
}