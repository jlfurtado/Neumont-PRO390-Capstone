#pragma once
#include <windows.h>
#include "Editor.h"
#include "DebugConsole.h"

namespace Capstone
{
	class EditorWindow
	{
	public:
		typedef void(*InitFunc)(HWND);
		typedef void(*ShutFunc)();
		typedef void(*DrawFunc)();

		bool MakeWindow(HINSTANCE instanceHandle, int nCmdShow, const char *const windowName, int screenWidth, int screenHeights);
		int Run();
		void GetWindowSize(int& outWidth, int &outHeight);

	private:
		HWND m_windowHandle;
		HINSTANCE m_instanceHandle;
		Editor m_editor;
		DebugConsole m_console;

		const char *const WINDOW_CLASS_NAME = "WindowClass1";
	
		static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	};
}
