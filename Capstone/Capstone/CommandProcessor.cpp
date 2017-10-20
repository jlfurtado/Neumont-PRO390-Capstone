#include "CommandProcessor.h"
#include "EditorWindow.h"
#include "Editor.h"
#include "StringFuncs.h"

namespace Capstone
{
	EditorWindow *CommandProcessor::s_pMyWindow = nullptr;
	Editor *CommandProcessor::s_pEditor = nullptr;

	bool CommandProcessor::Initialize(EditorWindow * pWindow, Editor *pEditor)
	{
		s_pMyWindow = pWindow;
		s_pEditor = pEditor;
		return true;
	}
	
	bool CommandProcessor::ProcessCommand(const char * const command)
	{
		if (StringFuncs::StringBeginsWith(command, "exit"))
		{
			s_pMyWindow->CloseWindow();
		}

		if (StringFuncs::StringBeginsWith(command, "loadObj"))
		{
			s_pEditor->LoadObj(command + StringFuncs::StringLen("loadObj ")); // TOTALLY A HACK
		}
		return true;
	}

	bool CommandProcessor::Shutdown()
	{
		return true;
	}
}