#include "DebugConsole.h"
#include <stdio.h>
#include "CommandProcessor.h"
#include "Utils.h"
#include "EditorWindow.h"

namespace Capstone
{
	EditorWindow *DebugConsole::s_pWindow = nullptr;
	HANDLE DebugConsole::s_writeHandle = NULL;
	HANDLE DebugConsole::s_readHandle = NULL;
	HWND DebugConsole::s_console = NULL;
	HWND DebugConsole::s_lastActive = NULL;
	bool DebugConsole::s_hidden = false;
	bool DebugConsole::s_outputOnly = false;

	// Handler function will be called on separate thread!
	BOOL WINAPI DebugConsole::MyConsoleControlHandler(DWORD controlType)
	{
		switch (controlType)
		{
		case CTRL_C_EVENT: // Ctrl+C
		case CTRL_BREAK_EVENT: // Ctrl+Break
		case CTRL_CLOSE_EVENT: // Closing the console window
		case CTRL_LOGOFF_EVENT: // User logs off. Passed only to services!
		case CTRL_SHUTDOWN_EVENT: // System is shutting down. Passed only to services!
			s_pWindow->ShutdownAll();
		}

		// Return TRUE if handled this message, further handler functions won't be called.
		// Return FALSE to pass this message to further handlers until default handler calls ExitProcess().
		return FALSE;
	}

	bool DebugConsole::Initialize(bool hide, EditorWindow *pWindow)
	{
		AllocConsole();
		s_writeHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		s_readHandle = GetStdHandle(STD_INPUT_HANDLE);
		s_console = GetConsoleWindow();
		s_pWindow = pWindow;

		if (!SetConsoleCtrlHandler(DebugConsole::MyConsoleControlHandler, TRUE)) { DebugConsole::Log("FAILED TO SET HANDLER!!\n"); return false; }

		if (hide) { Hide(); }
		return s_writeHandle != NULL && s_console != NULL && s_readHandle != NULL;
	}

	bool DebugConsole::Hide()
	{
		if (!s_hidden)
		{
			FlushConsoleInputBuffer(s_readHandle);
			ShowWindow(s_console, SW_HIDE);
			if (s_lastActive != NULL) { SetActiveWindow(s_lastActive); s_lastActive = NULL; }
			s_hidden = true;
		}
		return true;
	}

	bool DebugConsole::Show()
	{
		if (s_hidden)
		{
			s_lastActive = GetActiveWindow();
			ShowWindow(s_console, SW_SHOW);
			s_hidden = false;
		}
		return true;
	}

	void DebugConsole::GetCommand()
	{
		if (!s_outputOnly)
		{
			Show();
			ProcessCommand();
			Hide();
		}
	}

	void DebugConsole::ToggleKeep()
	{
		s_outputOnly = !s_outputOnly;

		if (s_outputOnly) { ShowBackground(); }
		else { Hide(); }
	}

	bool DebugConsole::Shutdown()
	{
		s_writeHandle = NULL;
		s_readHandle = NULL;
		s_console = NULL;
		return true;
	}

	bool DebugConsole::ShowBackground()
	{
		if (s_hidden)
		{
			s_lastActive = GetActiveWindow();
			ShowWindow(s_console, SW_SHOW);
			SetActiveWindow(s_lastActive);
			s_hidden = false;
		}
		return true;
	}

	void DebugConsole::ProcessCommand()
	{
		static const int BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE];
		DWORD cCharsRead;
		bool failed;

		do
		{
			MyUtils::MyClearFunc(&buffer[0], BUFFER_SIZE);
			MyUtils::MyClearFunc(&cCharsRead);
			
			WriteLog("Enter command: ");
			ReadConsole(s_readHandle, &buffer[0], BUFFER_SIZE, &cCharsRead, NULL);

			if (cCharsRead < 2) { failed = true; }
			else
			{
				buffer[cCharsRead - 1] = '\0'; // remove \n
				buffer[cCharsRead - 2] = '\0'; // remove \r

				failed = !CommandProcessor::ProcessCommand(&buffer[0]);
			}
			
			if (failed) { FlushConsoleInputBuffer(s_readHandle); }
		} while (failed);
	}

	void DebugConsole::WriteLog(const char * const str)
	{
		if (!s_hidden)
		{
			DWORD cCharsWritten;
			WriteConsole(s_writeHandle, str, strlen(str), &cCharsWritten, NULL);
		}
	}
}