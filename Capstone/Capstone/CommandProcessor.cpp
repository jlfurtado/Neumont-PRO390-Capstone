#include "CommandProcessor.h"
#include "EditorWindow.h"
#include "Editor.h"
#include "StringFuncs.h"

namespace Capstone
{
	EditorWindow *CommandProcessor::s_pMyWindow = nullptr;
	Editor *CommandProcessor::s_pEditor = nullptr;
	CommandProcessor::Command CommandProcessor::s_commands[NUM_COMMANDS] = {
		{"exit", CommandProcessor::ProcessExitCommand},
		{"loadObj", CommandProcessor::ProcessLoadObjCommand},
		{"back", CommandProcessor::ProcessCancelCommand},
		{"cancel", CommandProcessor::ProcessCancelCommand }
	};

	bool CommandProcessor::Initialize(EditorWindow * pWindow, Editor *pEditor)
	{
		s_pMyWindow = pWindow;
		s_pEditor = pEditor;
		return true;
	}
	
	bool CommandProcessor::ProcessCommand(const char * const command)
	{
		// iterate through every command we have specified
		for (int i = 0; i < NUM_COMMANDS; ++i)
		{
			// if match the start of the command 
			if (StringFuncs::StringBeginsWith(command, s_commands[i].m_prefix))
			{
				// return whether or not that command succeeds
				return s_commands[i].m_callback(command);
			}
		}
			
		// not a valid command
		DebugConsole::Log("Invalid Command [%s]! Ignoring!\n", command);
		return false;
	}

	bool CommandProcessor::Shutdown()
	{
		return true;
	}

	bool CommandProcessor::ProcessExitCommand(const char * const command)
	{
		s_pMyWindow->CloseWindow();
		return true;
	}

	bool CommandProcessor::ProcessLoadObjCommand(const char * const command)
	{
		s_pEditor->LoadObj(command + StringFuncs::StringLen("loadObj ")); // TOTALLY A HACK
		return true;
	}

	bool CommandProcessor::ProcessCancelCommand(const char * const /*command*/)
	{
		return true;
	}
}