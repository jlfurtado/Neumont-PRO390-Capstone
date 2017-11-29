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
			const char *const m_args;
			CommandProcessCallback m_callback;
			const char *const m_helpText;
		};

		static bool ProcessExitCommand(const char *const command);
		static bool ProcessLoadObjCommand(const char *const command);
		static bool ProcessCancelCommand(const char * const command);
		static bool ProcessSetPivotCommand(const char *const command);
		static bool ProcessSetVariationTypeCommand(const char *const command);
		static bool ProcessHelpCommand(const char *const command);
		static bool ProcessSaveCommand(const char *const command);
		static bool ProcessLoadCommand(const char *const command);
		static bool ProcessSetCameraSpeedCommand(const char *const command);
		static bool ProcessSetCameraRotateSpeedCommand(const char *const command);
		static bool ProcessSetVariationSpeedCommand(const char *const command);
		static bool ProcessDisplayVertexGroupsCommand(const char *const command);
		static bool ProcessSelectVertexGroupCommand(const char *const command);
		static bool ProcessRemoveVertexGroupCommand(const char *const command);
		static bool ProcessClearVertexGroupsCommand(const char *const command);
		static bool ProcessDisplayVariantsCommand(const char *const command);
		static bool ProcessDisplayVariants2DCommand(const char *const command);
		static bool ProcessResumeEditCommand(const char *const command);
		static bool ProcessExportCurrentMeshObjCommand(const char *const command);
		static bool ProcessExportVariedMeshesObjCommand(const char *const command);
		static bool ProcessToggleDebugFrustumCommand(const char *const command);

		static EditorWindow *s_pMyWindow;
		static Editor *s_pEditor;

		static const int NUM_COMMANDS = 22;
		static Command s_commands[NUM_COMMANDS];
	};

}

