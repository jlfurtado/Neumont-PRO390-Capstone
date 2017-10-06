#pragma once
#include <windows.h>

namespace Capstone
{
	class DebugConsole
	{
	public:
		static bool Initialize();
		static void Log(const char *const str);
		static bool Shutdown();

	private:
		static HANDLE s_consoleHandle;
	};


}
