#pragma once

namespace Capstone
{
	class EditorWindow;
	class CommandProcessor
	{
	public:
		static bool Initialize(EditorWindow *pWindow);
		static bool ProcessCommand(const char *const command);
		static bool Shutdown();

	private:
		static EditorWindow *s_pMyWindow;
	};

}

