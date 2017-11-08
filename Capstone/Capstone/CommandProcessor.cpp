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
		{"exit", nullptr, CommandProcessor::ProcessExitCommand, "shuts down and closes the application."},
		{"loadObj", "(filePath)", CommandProcessor::ProcessLoadObjCommand, "loads the specified .obj file"},
		{"back", nullptr, CommandProcessor::ProcessCancelCommand, "closes the console and returns to the editor."},
		{"cancel", nullptr, CommandProcessor::ProcessCancelCommand, "closes the console and returns to the editor." },
		{"setPivot", "((x y z) || 'center' || 'camera')", CommandProcessor::ProcessSetPivotCommand, "sets the pivot location for the current vertex group. Currently not availiable for object level variations." },
		{"setVariationType", "('componentUniform' || 'vectorUniform' || 'smoothUniform' || 'componentBell' || 'vectorBell' || 'smoothBell')", CommandProcessor::ProcessSetVariationTypeCommand, "sets the variation type for the current vertex group or the whole object." },
		{"help", nullptr, CommandProcessor::ProcessHelpCommand, "displays the help text for every command."},
		{"save", "(filePath)", CommandProcessor::ProcessSaveCommand, "saves the base vertices, object level variations, and vertex groups to a custom file format."},
		{"load", "(filePath)", CommandProcessor::ProcessLoadCommand, "loads the base vertices, object level variations, and vertex groups from a custom file format."},
		{"setCameraSpeed", "(speed)", CommandProcessor::ProcessSetCameraSpeedCommand, "sets the speed of the editor camera so you can move faster or slower."},
		{"setCameraRotateSpeed", "(speed)", CommandProcessor::ProcessSetCameraRotateSpeedCommand, "sets the rotate speed of the editor camera so you can turn faster or slower." },
		{"setVariationSpeed", "(speed)", CommandProcessor::ProcessSetVariationSpeedCommand, "sets the speed of all variation controllers so you can edit faster or slower." },
		{"displayVertexGroups", nullptr, CommandProcessor::ProcessDisplayVertexGroupsCommand, "displays information about all the vertex groups."},
		{"selectVertexGroup", "(vertex group)", CommandProcessor::ProcessSelectVertexGroupCommand, "selects the specified vertex group, if it exists."},
		{"removeVertexGroup", "(vertex group)", CommandProcessor::ProcessRemoveVertexGroupCommand, "removes the specified vertex group, if it exists."}
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
			if (s_commands[i].m_args)
			{
				DebugConsole::Log(" - Command [%d][%s %s]: %s\n", i, s_commands[i].m_prefix, s_commands[i].m_args, s_commands[i].m_helpText);
			}
			else
			{
				DebugConsole::Log(" - Command [%d][%s]: %s\n", i, s_commands[i].m_prefix, s_commands[i].m_helpText);
			}
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

	bool CommandProcessor::ProcessSetCameraSpeedCommand(const char * const command)
	{
		const char *const args = command + StringFuncs::StringLen("setCameraSpeed ");
		
		float speed;
		if (!StringFuncs::GetSingleFloatFromString(args, speed))
		{
			DebugConsole::Log("Invalid args [%s] to SetCameraSpeed!\n", args);
			return false;
		}

		return s_pEditor->SetCameraSpeed(speed);
	}

	bool CommandProcessor::ProcessSetCameraRotateSpeedCommand(const char * const command)
	{
		const char *const args = command + StringFuncs::StringLen("setCameraRotateSpeed ");

		float speed;
		if (!StringFuncs::GetSingleFloatFromString(args, speed))
		{
			DebugConsole::Log("Invalid args [%s] to SetCameraRotateSpeed!\n", args);
			return false;
		}

		return s_pEditor->SetCameraRotateSpeed(speed);
	}

	bool CommandProcessor::ProcessSetVariationSpeedCommand(const char * const command)
	{
		const char *const args = command + StringFuncs::StringLen("setVariationSpeed ");

		float speed;
		if (!StringFuncs::GetSingleFloatFromString(args, speed))
		{
			DebugConsole::Log("Invalid args [%s] to SetVariationSpeed!\n", args);
			return false;
		}

		return VariationController::SetVariationSpeed(speed);
	}

	bool CommandProcessor::ProcessDisplayVertexGroupsCommand(const char * const /*command*/)
	{
		unsigned numGroups = s_pEditor->GetVertexGroupCount();

		DebugConsole::Log("Begin vertex group info for [%u] groups!\n", numGroups);
		
		unsigned size = 0;
		for (unsigned i = 0; i < numGroups; ++i)
		{
			if (!s_pEditor->GetVertexGroupInfo(i, size)) { DebugConsole::Log("Unable to get vertex group info for vertex group [%u]!\n", i); return false; }
			DebugConsole::Log("Vertex Group [%u]: Num Verts (%d)!\n", i, size);
		}

		DebugConsole::Log("End vertex group info for [%u] groups!\n", numGroups);
		return false;
	}

	bool CommandProcessor::ProcessSelectVertexGroupCommand(const char * const command)
	{
		const char *const args = command + StringFuncs::StringLen("selectVertexGroup ");
	
		unsigned numGroups = s_pEditor->GetVertexGroupCount();
		if (numGroups <= 0) { DebugConsole::Log("Cannot selectVertexGroup! No vertex groups exist for model!\n"); return false; }

		int groupIdx = -1;
		if (!StringFuncs::GetSingleIntFromString(args, groupIdx)) { DebugConsole::Log("Invalid args to SelectVertexGroup!\n"); return false; }
		if (groupIdx < 0 || (unsigned)groupIdx >= numGroups) { DebugConsole::Log("Invalid vertex group [%d]! Must be in range [0, %u]!\n", groupIdx, numGroups-1); return false; }
		return s_pEditor->SelectVertexGroup(groupIdx);
	}

	bool CommandProcessor::ProcessRemoveVertexGroupCommand(const char * const command)
	{
		const char *const args = command + StringFuncs::StringLen("removeVertexGroup ");

		unsigned numGroups = s_pEditor->GetVertexGroupCount();
		if (numGroups <= 0) { DebugConsole::Log("Cannot removeVertexGroup! No vertex groups exist for model!\n"); return false; }

		int groupIdx = -1;
		if (!StringFuncs::GetSingleIntFromString(args, groupIdx)) { DebugConsole::Log("Invalid args to RemoveVertexGroup!\n"); return false; }
		if (groupIdx < 0 || (unsigned)groupIdx >= numGroups) { DebugConsole::Log("Invalid vertex group [%d]! Must be in range [0, %u]!\n", groupIdx, numGroups - 1); return false; }
		return s_pEditor->RemoveVertexGroup(groupIdx);
	}
}