#pragma once
#include <windows.h>
#include <stdio.h>

namespace Capstone
{
	class DebugConsole
	{
	public:
		static bool Initialize(bool hide);
		static bool Hide();
		static bool Show();
		static void GetCommand();

		static void ToggleKeep();

		template<typename...Args>
		static void Log(const char *const format, Args... args);

		static bool Shutdown();
		
	private:
		static bool ShowBackground();
		static void ProcessCommand();
		static void WriteLog(const char *const str);

		static const int MAX_LOG_SIZE = 1000;
		static HANDLE s_writeHandle;
		static HANDLE s_readHandle;
		static HWND s_console;
		static HWND s_lastActive;
		static bool s_hidden;
		static bool s_outputOnly;
	};

	template <typename...Args>
	inline void DebugConsole::Log(const char *const format, Args...args)
	{
		char buffer[MAX_LOG_SIZE]{ 0 };
		sprintf_s(buffer, MAX_LOG_SIZE, format, args...);
		WriteLog(buffer);
	}
}
