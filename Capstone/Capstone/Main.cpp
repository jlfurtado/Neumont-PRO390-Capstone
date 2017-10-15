// include the basic windows header file
#include <windows.h>
#include "EditorWindow.h"
#include <time.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
const char *const WINDOW_TITLE = "Capstone";

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE /*hPrevInstance*/,
	LPSTR /*lpCmdLine*/,
	int nCmdShow)
{
	srand(static_cast<unsigned>(time(0)));

	// make an instance of the pretty class that manages all the window stuff on the stack
	Capstone::EditorWindow editorWindow;

	// try to make a window, if it fails, explode violently
	if (!editorWindow.MakeWindow(hInstance, nCmdShow, WINDOW_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT))
	{
		return EXIT_FAILURE;
	}
	
	// tell our window to do the everything
	return editorWindow.Run();
}