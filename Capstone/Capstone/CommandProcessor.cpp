#include "CommandProcessor.h"
#include "EditorWindow.h"
#include "StringFuncs.h"

namespace Capstone
{
	EditorWindow *CommandProcessor::s_pMyWindow = nullptr;

	bool CommandProcessor::Initialize(EditorWindow * pWindow)
	{
		s_pMyWindow = pWindow;
		return true;
	}
	
	bool CommandProcessor::ProcessCommand(const char * const command)
	{
		if (StringFuncs::StringBeginsWith(command, "exit"))
		{
			s_pMyWindow->CloseWindow();
		}
		return true;
	}

	bool CommandProcessor::Shutdown()
	{
		return true;
	}
}