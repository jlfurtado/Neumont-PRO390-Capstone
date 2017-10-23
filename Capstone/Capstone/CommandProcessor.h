#pragma once

namespace Capstone
{
	class EditorWindow;
	class Editor;
	class CommandProcessor
	{
	public:
		static bool Initialize(EditorWindow *pWindow, Editor *pEditor);
		static bool ProcessCommand(const char *const command);
		static bool Shutdown();

	private:
		typedef bool(*CommandProcessCallback)(const char *const);
		struct Command
		{
			const char *const m_prefix;
			CommandProcessCallback m_callback;
		};
		static bool ProcessExitCommand(const char *const command);
		static bool ProcessLoadObjCommand(const char *const command);
		static bool ProcessCancelCommand(const char * const command);
		static EditorWindow *s_pMyWindow;
		static Editor *s_pEditor;

		static const int NUM_COMMANDS = 4;
		static Command s_commands[NUM_COMMANDS];
	};

}
