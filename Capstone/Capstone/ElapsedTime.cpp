#include "ElapsedTime.h"
#include <stdio.h>
#include <Windows.h>
#include "DebugConsole.h"

namespace Capstone
{
	// file globals!
	__int64 f_timerCountsPerSecond;     // high-precision clock tick frequency
	__int64 f_timerInitialCountValue;   // number of ticks at game-begin
	float   f_lastFrameTime = 0.0f;
	bool    f_isInitialized = false;

	bool LogWindowsError();

	bool ElapsedTime::Initialize()
	{
		if (f_isInitialized)
		{
			DebugConsole::Log("Programmer Error: ElapsedTime was already initialized!\n");
			return false;
		}
		f_isInitialized = true;

		// Get the frequency of the high-resolution performance counter
		if (QueryPerformanceFrequency((LARGE_INTEGER*)&f_timerCountsPerSecond) == 0)
		{
			LogWindowsError();
			return false;
		}

		// Make sure it's supported on this hardware
		if (f_timerCountsPerSecond != 0)
		{
			// Get the current count value
			if (QueryPerformanceCounter((LARGE_INTEGER*)&f_timerInitialCountValue) == 0)
			{
				LogWindowsError();
				return false;
			}
		}
		else
		{
			DebugConsole::Log("FATAL ERROR: ElapsedTime::Initialize(). Hardware doesn't support high-resolution performance counters");
			return false;
		}

		DebugConsole::Log("PROCESS: ElapsedTime::Initialize() ok\n");
		return true;
	}

	bool ElapsedTime::Shutdown()
	{
		DebugConsole::Log("PROCESS ElapsedTime::Shutdown() ok\n");
		return true;
	}

	float ElapsedTime::GetElapsedTime()
	{
		__int64 currentValue;
		if (0 == QueryPerformanceCounter((LARGE_INTEGER*)&currentValue)) return 0;
		return (float)(currentValue - f_timerInitialCountValue) / f_timerCountsPerSecond;
	}

	// Very important that this function is called exactly once per frame
	// or the return value will not be correct
	float ElapsedTime::GetLastFrameTime()
	{
		float ElapsedTime = GetElapsedTime();
		float frameTime = ElapsedTime - f_lastFrameTime;
		f_lastFrameTime = ElapsedTime;
		return frameTime;
	}

	bool LogWindowsError()
	{
		DWORD errorCode = GetLastError();
		void* messageBuffer = NULL;
		DWORD result = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |			// Let Windows allocate memory for the error message
			FORMAT_MESSAGE_FROM_SYSTEM,				    // We are passing in a Windows error code
			NULL,										// Only relevant if we are formatting the message ourselves
			errorCode,									// The error code
			0,											// Use default language
			reinterpret_cast<LPSTR>(&messageBuffer),	// Buffer to be allocated //LPWSTR!?!?!?!
			0,											// Minimum number of TCHARs to allocate
			NULL);										// Array of insertion strings

		if (result != 0)
		{
			DebugConsole::Log(reinterpret_cast<LPCSTR>(messageBuffer));
			if (LocalFree(messageBuffer) != NULL)
			{
				DebugConsole::Log("ERROR: ElapsedTime: Windows couldn't deallocate error message memory");
				return false;
			}
			return true;
		}

		DebugConsole::Log("ERROR: ElapsedTime: Couldn't format Windows error message");
		return true;
	}
}
