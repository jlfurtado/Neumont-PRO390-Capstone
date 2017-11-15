#include "ObjExporter.h"
#include "DebugConsole.h"
#include "StringFuncs.h"
#include "Utils.h"

namespace Capstone
{
	const int FLOATS_PER_POSITION = 3;
	const int POSITION_BYTES = FLOATS_PER_POSITION * sizeof(float);
	const int FLOATS_PER_COLOR = 4;
	const int COLOR_BYTES = FLOATS_PER_COLOR * sizeof(float);
	const int FLOATS_PER_TEXTURE = 2;
	const int TEXTURE_BYTES = FLOATS_PER_TEXTURE * sizeof(float);
	const int FLOATS_PER_NORMAL = 3;
	const int NORMAL_BYTES = FLOATS_PER_NORMAL * sizeof(float);

	char ObjExporter::s_outputFileName[MAX_CHARS]{ '\0' };
	std::ofstream ObjExporter::s_outputFileStream;
	int ObjExporter::s_stride = 0;
	int ObjExporter::s_floatsPerVertex = 0;
	bool ObjExporter::s_hasColor = false;
	bool ObjExporter::s_hasTexture = false;
	bool ObjExporter::s_hasNormal = false;
	int ObjExporter::s_positionInStart = -1;
	int ObjExporter::s_colorInStart = -1;
	int ObjExporter::s_textureInStart = -1;
	int ObjExporter::s_normalInStart = -1;

	bool ObjExporter::WriteObj(const char * const filePath, const char * const format, const float * const pVertsToWrite, int vertexCount)
	{
		return ProcessFile(filePath, format, pVertsToWrite, vertexCount);
	}

	bool ObjExporter::ProcessFile(const char * const filePath, const char * const format, const float * const pVertsToWrite, int vertexCount)
	{
		StringFuncs::StringCopy(filePath, s_outputFileName, MAX_CHARS);

		if (!OpenFile())
		{
			DebugConsole::Log("ObjExporter failed to ProcessFile()! Failed to open output file [%s]!\n", s_outputFileName);
			return false;
		}

		// do format things
		if (!CalcProps(format))
		{
			DebugConsole::Log("ObjExporter failed to CalcProps()!\n");
			return false;
		}

		// try to read data from output file into a mesh, if it fails, do not proceed - indicate failure
		if (!WriteMeshOutToFile(pVertsToWrite, vertexCount))
		{
			CleanUp();
			DebugConsole::Log("ObjExporter failed to ProcessFile()! Failed to write output file [%s]!\n", s_outputFileName);
			return false;
		}

		// try to close output file, if it fails, mark a warning and proceed
		if (!CloseFile())
		{
			DebugConsole::Log("ObjExporter failed to close output file [%s]! ObjExporter will continue to ProcessFile()!\n", s_outputFileName);
		}

		// After writing has finished clean up, no need to clean up if only analyzed because nothing new'd
		if (!CleanUp())
		{
			DebugConsole::Log("ObjExporter failed to clean up!\n");
		}

		// success was achieved 
		DebugConsole::Log("ObjExporter successfully processed output file [%s]!\n", s_outputFileName);
		return true;
	}

	bool ObjExporter::OpenFile()
	{
		// open the output file, ensure and validate its state
		s_outputFileStream.open(s_outputFileName, std::ios::binary | std::ios::out);
		s_outputFileStream.clear();
		s_outputFileStream.seekp(0, std::ios::beg);
		return s_outputFileStream.is_open() && s_outputFileStream.good();
	}

	bool ObjExporter::CloseFile()
	{
		// close the output file and check that it is no longer open
		s_outputFileStream.close();
		return !s_outputFileStream.is_open();
	}

	bool ObjExporter::WriteMeshOutToFile(const float * const pVertsToWrite, int vertexCount)
	{
		return WriteVertexPositions(pVertsToWrite, vertexCount)
			&& (!s_hasTexture || WriteVertexTextures(pVertsToWrite, vertexCount))
			&& (!s_hasNormal || WriteVertexNormals(pVertsToWrite, vertexCount))
			&& WriteVertexFaceIndices(vertexCount);
	}

	//v float float float
	bool ObjExporter::WriteVertexPositions(const float * const pVertsToWrite, int vertexCount)
	{
		static const int MAX_LINE = 256;

		for (int i = 0; i < vertexCount; ++i)
		{
			char buffer[MAX_LINE]{ '\0' };
			const float *const pVert = pVertsToWrite + (i * s_floatsPerVertex) + s_positionInStart;
			sprintf_s(&buffer[0], MAX_LINE, "v %.6f %.6f %.6f\n", *(pVert + 0), *(pVert + 1), *(pVert + 2));
			if (!WriteArray(&buffer[0], StringFuncs::StringLen(&buffer[0]))) { DebugConsole::Log("Failed to WriteVertexPosition [%d]!\n", i); return false; }
		}

		return true;
	}

	//vt float float
	bool ObjExporter::WriteVertexTextures(const float * const pVertsToWrite, int vertexCount)
	{
		static const int MAX_LINE = 256;

		for (int i = 0; i < vertexCount; ++i)
		{
			char buffer[MAX_LINE]{ '\0' };
			const float *const pVert = pVertsToWrite + (i * s_floatsPerVertex) + s_textureInStart;
			sprintf_s(&buffer[0], MAX_LINE, "vt %.6f %.6f\n", *(pVert + 0), *(pVert + 1));
			if (!WriteArray(&buffer[0], StringFuncs::StringLen(&buffer[0]))) { DebugConsole::Log("Failed to WriteVertexTexture [%d]!\n", i); return false; }
		}

		return true;
	}

	//vn float float float
	bool ObjExporter::WriteVertexNormals(const float * const pVertsToWrite, int vertexCount)
	{
		static const int MAX_LINE = 256;

		for (int i = 0; i < vertexCount; ++i)
		{
			char buffer[MAX_LINE]{ '\0' };
			const float *const pVert = pVertsToWrite + (i * s_floatsPerVertex) + s_normalInStart;
			sprintf_s(&buffer[0], MAX_LINE, "vn %.6f %.6f %.6f\n", *(pVert + 0), *(pVert + 1), *(pVert + 2));
			if (!WriteArray(&buffer[0], StringFuncs::StringLen(&buffer[0]))) { DebugConsole::Log("Failed to WriteVertexNormal [%d]!\n", i); return false; }
		}

		return true;
	}

	//f #/#/# #/#/# #/#/#
	const int VERTICES_PER_TRI = 3;
	bool ObjExporter::WriteVertexFaceIndices(int vertexCount)
	{
		static const int MAX_LINE = 256;
		char buffer[MAX_LINE]{ '\0' };

		typedef bool(*GetIndicesFormatCallback)(char *, int, int);
		static const int NUM_FORMATS = 4;
		static const GetIndicesFormatCallback CALLBACKS[NUM_FORMATS] = {
			GetFaceIndicesP, GetFaceIndicesPT, GetFaceIndicesPN, GetFaceIndicesPTN
		};

		int callbackIdx = (s_hasTexture ? 1 : 0) + 2 * (s_hasNormal ? 1 : 0);
		GetIndicesFormatCallback callback = CALLBACKS[callbackIdx];

		for (int i = 0; i < vertexCount; i += VERTICES_PER_TRI)
		{
			MyUtils::MyClearFunc(buffer, MAX_LINE);
			// OBJ INDICES 1 BASED
			if (!callback(&buffer[0], MAX_LINE, i+1)) { DebugConsole::Log("Failed to make string for callback [%d] polygon/line [%d]!\n", callbackIdx, i); return false; }
			if (!WriteArray(&buffer[0], StringFuncs::StringLen(&buffer[0]))) { DebugConsole::Log("Failed to WriteFaceIndices [%d]!\n", i); return false; }
		}

		return true;
	}

	bool ObjExporter::GetFaceIndicesP(char * buffer, int bufferSize, int idx)
	{
		const char *const P_OUT_FORMAT = "f %d %d %d\n";
		sprintf_s(&buffer[0], bufferSize, P_OUT_FORMAT, idx, idx+1, idx+2);
		return true;
	}

	bool ObjExporter::GetFaceIndicesPT(char * buffer, int bufferSize, int idx)
	{
		const char *const PT_OUT_FORMAT = "f %d/%d %d/%d %d/%d\n";
		sprintf_s(&buffer[0], bufferSize, PT_OUT_FORMAT, idx, idx, idx+1, idx+1, idx+2, idx+2);
		return true;
	}

	bool ObjExporter::GetFaceIndicesPN(char * buffer, int bufferSize, int idx)
	{
		const char *const PN_OUT_FORMAT = "f %d//%d %d//%d %d//%d\n";
		sprintf_s(&buffer[0], bufferSize, PN_OUT_FORMAT, idx, idx, idx + 1, idx + 1, idx + 2, idx + 2);
		return true;
	}

	bool ObjExporter::GetFaceIndicesPTN(char * buffer, int bufferSize, int idx)
	{
		const char *const PTN_OUT_FORMAT = "f %d/%d/%d %d/%d/%d %d/%d/%d\n";
		sprintf_s(&buffer[0], bufferSize, PTN_OUT_FORMAT, idx, idx, idx, idx + 1, idx + 1, idx + 1, idx + 2, idx + 2, idx + 2);
		return true;
	}

	bool ObjExporter::CleanUp()
	{
		// nothing to clean up - yet???
		//MyUtils::SafeDeleteArray(nullptr);
		DebugConsole::Log("ObjExporter successfully cleaned up!\n");
		return true;
	}

	bool ObjExporter::CalcProps(const char * const format)
	{
		/// umm ugly bare minimum stuffs todo more error checks if needed
		int numChars = StringFuncs::StringLen(format);
		if (numChars <= 0 || numChars > 4) { DebugConsole::Log("Invalid props format!\n"); return false; }
		if (format[0] != 'P') { DebugConsole::Log("Invalid props format! Format must have position!\n"); return false; }

		s_positionInStart = 0;
		s_colorInStart = -1;
		s_textureInStart = -1;
		s_normalInStart = -1;

		s_floatsPerVertex = FLOATS_PER_POSITION;
		int idx = 1;
		if (format[idx] == 'C')
		{
			s_hasColor = true;
			s_colorInStart = s_floatsPerVertex;
			++idx;
			s_floatsPerVertex += FLOATS_PER_COLOR;
		}

		if (format[idx] == 'T')
		{ 
			s_hasTexture = true;
			s_textureInStart = s_floatsPerVertex;
			++idx; 
			s_floatsPerVertex += FLOATS_PER_TEXTURE;
		}

		if (format[idx] == 'N')
		{ 
			s_hasNormal = true;
			s_normalInStart = s_floatsPerVertex;
			++idx; 
			s_floatsPerVertex += FLOATS_PER_NORMAL;
		}

		s_stride = s_floatsPerVertex * sizeof(float);
		return true;
	}
}