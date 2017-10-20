#include "DebugConsole.h"
#include <stdio.h>
#include "CommandProcessor.h"


namespace Capstone
{
	HANDLE DebugConsole::s_writeHandle = NULL;
	HANDLE DebugConsole::s_readHandle = NULL;
	HWND DebugConsole::s_console = NULL;
	HWND DebugConsole::s_lastActive = NULL;
	bool DebugConsole::s_hidden = false;
	bool DebugConsole::s_outputOnly = false;

	bool DebugConsole::Initialize(bool hide)
	{
		AllocConsole();
		s_writeHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		s_readHandle = GetStdHandle(STD_INPUT_HANDLE);
		s_console = GetConsoleWindow();
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
			WriteLog("Enter command: ");
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
		char buffer[BUFFER_SIZE]{ '\0' };
		
		DWORD cCharsRead;
		ReadConsole(s_readHandle, &buffer[0], BUFFER_SIZE, &cCharsRead, NULL);

		buffer[cCharsRead - 1] = '\0'; // remove \n
		buffer[cCharsRead - 2] = '\0'; // remove \r

		CommandProcessor::ProcessCommand(&buffer[0]);
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