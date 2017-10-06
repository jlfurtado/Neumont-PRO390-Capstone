#include "DebugConsole.h"
#include <stdio.h>


namespace Capstone
{
	HANDLE DebugConsole::s_consoleHandle = NULL;

	bool DebugConsole::Initialize()
	{
		AllocConsole();
		s_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		return s_consoleHandle != NULL;
	}

	bool DebugConsole::Shutdown()
	{
		s_consoleHandle = NULL;
		return true;
	}

	void DebugConsole::WriteLog(const char * const str)
	{
		DWORD cCharsWritten;
		WriteConsole(s_consoleHandle, str, strlen(str), &cCharsWritten, NULL);
	}
}