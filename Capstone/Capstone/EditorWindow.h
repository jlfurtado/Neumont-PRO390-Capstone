#pragma once
#include "Editor.h"
#include "DebugConsole.h"
#include <windows.h>

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
		int GetWidth();
		int GetHeight();
		void CloseWindow();

	private:
		HWND m_windowHandle;
		HINSTANCE m_instanceHandle;
		Editor m_editor;
		DebugConsole m_console;
		int m_width;
		int m_height;

		const char *const WINDOW_CLASS_NAME = "WindowClass2";
		static EditorWindow *s_pFirst;
		static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	};
}
