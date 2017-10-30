#include "CommandProcessor.h"
#include "EditorWindow.h"
#include "Editor.h"
#include "StringFuncs.h"
#include "VariationType.h"

namespace Capstone
{
	EditorWindow *CommandProcessor::s_pMyWindow = nullptr;
	Editor *CommandProcessor::s_pEditor = nullptr;
	CommandProcessor::Command CommandProcessor::s_commands[NUM_COMMANDS] = {
		{"exit", CommandProcessor::ProcessExitCommand},
		{"loadObj", CommandProcessor::ProcessLoadObjCommand},
		{"back", CommandProcessor::ProcessCancelCommand},
		{"cancel", CommandProcessor::ProcessCancelCommand },
		{"setPivot", CommandProcessor::ProcessSetPivotCommand },
		{"setVariationType", CommandProcessor::ProcessSetVariationTypeCommand }
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

		DebugConsole::Log("Invalid args to setPivot!\n");
		return false;
	}

	bool CommandProcessor::ProcessSetVariationTypeCommand(const char * const command)
	{
		static const int NUM_TYPES = static_cast<int>(VariationType::USED_TYPES);
		static const VariationType s_types[NUM_TYPES]{ VariationType::COMPONENT_UNIFORM, VariationType::VECTOR_UNIFORM, VariationType::SMOOTH_UNIFORM, VariationType::COMPONENT_BELL, VariationType::VECTOR_BELL, VariationType::SMOOTH_BELL};
		static const char *const s_prefixes[NUM_TYPES]{ "componentUniform", "vectorUniform", "smoothUniform", "componentBell", "vectorBell", "smoothBell" };

		const char *const args = command + StringFuncs::StringLen("setVariationType ");
		int outInt = -1;

		for (int i = 0; i < NUM_TYPES; ++i)
		{
			if (StringFuncs::StringBeginsWith(args, s_prefixes[i]))
			{
				return s_pEditor->SetVariationType(s_types[i]);
			}
		}

		if (StringFuncs::GetSingleIntFromString(args, outInt))
		{
			if (outInt < 0 || outInt >= NUM_TYPES) { DebugConsole::Log("Invalid args to setVariationType! type must be between [0] and [%d]\n", NUM_TYPES); return false; }
			return s_pEditor->SetVariationType(static_cast<VariationType>(outInt));
		}

		DebugConsole::Log("Invalid args to setVariationType!\n");
		return false;
	}
}