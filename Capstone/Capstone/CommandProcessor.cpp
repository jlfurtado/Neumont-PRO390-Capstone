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
		{"cancel", CommandProcessor::ProcessCancelCommand },
		{"setPivot", CommandProcessor::ProcessSetPivotCommand }
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

	bool CommandProcessor::ProcessExitCommand(const char * const /*command*/)
	{
		s_pMyWindow->CloseWindow();
		return true;
	}

	bool CommandProcessor::ProcessLoadObjCommand(const char * const command)
	{
		return s_pEditor->LoadObj(command + StringFuncs::StringLen("loadObj "));
	}

	bool CommandProcessor::ProcessCancelCommand(const char * const /*command*/)
	{
		return true;
	}

	bool CommandProcessor::ProcessSetPivotCommand(const char * const command)
	{
		static const int EXPECTED_FLOATS = 3;
		float xyz[EXPECTED_FLOATS]{ 0.0f };
		const char *const args = command + StringFuncs::StringLen("setPivot ");

		if (StringFuncs::StringBeginsWith(args, "camera"))
		{
			return s_pEditor->SetPivotCamera();
		}

		if (StringFuncs::StringBeginsWith(args, "center"))
		{
			return s_pEditor->SetPivotCenter();
		}

		if (StringFuncs::GetFloatsFromString(args, EXPECTED_FLOATS, &xyz[0]))
		{
			return s_pEditor->SetPivotXYZ(xyz[0], xyz[1], xyz[2]);
		}


		return false;
	}
}