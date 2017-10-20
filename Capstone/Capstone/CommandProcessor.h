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
		static EditorWindow *s_pMyWindow;
		static Editor *s_pEditor;
	};

}

