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
		{"exit", CommandProcessor::ProcessExitCommand, "shuts down and closes the application."},
		{"loadObj", CommandProcessor::ProcessLoadObjCommand, "loads an .obj file, argument is file path to obj file."},
		{"back", CommandProcessor::ProcessCancelCommand, "closes the console and returns to the editor."},
		{"cancel", CommandProcessor::ProcessCancelCommand, "closes the console and returns to the editor." },
		{"setPivot", CommandProcessor::ProcessSetPivotCommand, "sets the pivot for the current vertex group. Args: [x y z] xor 'center' xor 'camera'." },
		{"setVariationType", CommandProcessor::ProcessSetVariationTypeCommand, "sets the variation type for the current vertex group or the whole object. Args: 'componentUniform', 'vectorUniform', 'smoothUniform', 'componentBell', 'vectorBell', or 'smoothBell'." },
		{"help", CommandProcessor::ProcessHelpCommand, "displays the help text for every command."},
		{"save", CommandProcessor::ProcessSaveCommand, "saves the base vertices, object level variations, and vertex groups to a custom file format. Args: filePath."},
		{"load", CommandProcessor::ProcessLoadCommand, "loads the base vertices, object level variations, and vertex groups from a custom file format. Args: filePath."}
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

	bool CommandProcessor::ProcessHelpCommand(const char * const /*command*/)
	{
		DebugConsole::Log("Begin help for [%d] commands\n", NUM_COMMANDS);

		for (int i = 0; i < NUM_COMMANDS; ++i)
		{
			DebugConsole::Log(" - Command [%d][%s]: %s\n", i, s_commands[i].m_prefix, s_commands[i].m_helpText);
		}

		DebugConsole::Log("End help for [%d] commands\n", NUM_COMMANDS);
		return false; // don't close after help
	}

	bool CommandProcessor::ProcessSaveCommand(const char * const command)
	{
		const char *const args = command + StringFuncs::StringLen("save ");
		return s_pEditor->WriteMeshToFile(args);
	}

	bool CommandProcessor::ProcessLoadCommand(const char * const command)
	{
		const char *const args = command + StringFuncs::StringLen("load ");
		return s_pEditor->ReadMeshFromFile(args);
	}
}