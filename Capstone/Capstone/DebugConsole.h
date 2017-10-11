#pragma once
#include <windows.h>
#include <stdio.h>

namespace Capstone
{
	class DebugConsole
	{
	public:
		static bool Initialize();

		template<typename...Args>
		static void Log(const char *const format, Args... args);

		static bool Shutdown();

	private:
		static void WriteLog(const char *const str);

		static const int MAX_LOG_SIZE = 1000;
		static HANDLE s_consoleHandle;
	};

	template <typename...Args>
	inline void DebugConsole::Log(const char *const format, Args...args)
	{
		char buffer[MAX_LOG_SIZE]{ 0 };
		sprintf_s(buffer, MAX_LOG_SIZE, format, args...);
		WriteLog(buffer);
	}
}
