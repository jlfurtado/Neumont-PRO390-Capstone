#include "ObjLoader.h"
#include "DebugConsole.h"
#include "StringFuncs.h"
#include <sstream>
#include <istream>

namespace Capstone
{
	std::ifstream ObjLoader::s_fileStream;
	int ObjLoader::s_numVertices = 0;
	float *ObjLoader::s_pMeshVertexPositions = nullptr;
	float *ObjLoader::s_pMeshVertexColors = nullptr;
	float *ObjLoader::s_pMeshVertexTextureCoords = nullptr;
	float *ObjLoader::s_pMeshVertexNormals = nullptr;
	char ObjLoader::s_inputFile[MAX_CHARS]{ '\0' };
	int *ObjLoader::s_pTempIndices = nullptr;
	int ObjLoader::s_stride = 0;
	bool ObjLoader::s_hasPosition = false;
	bool ObjLoader::s_hasColor = false;
	bool ObjLoader::s_hasTexture = false;
	bool ObjLoader::s_hasNormal = false;

	const int FLOATS_PER_COLOR = 4;
	float ObjLoader::s_PCCubeVerts[CUBE_FLOAT_COUNT] = {
		/*X      Y      Z      R      G      B      A */
		-1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f,
		-1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f,
		-1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f,

		+1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f,
		-1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f,

		-1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f,
		-1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f,
		-1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f,
		-1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f,

		+1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f,
		+1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f,
		+1.0f, +1.0f,  1.0f, +1.0f, +1.0f,  1.0f, +1.0f,
		+1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f,

		-1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f,
		+1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f,
		+1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f,
		-1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f,

		+1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f,
		-1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f,
		+1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f,
		-1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f,
		-1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f,
		+1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f
	};

	void ObjLoader::LoadObj(const char * const filePath, float ** outPVerts, int * outCount, int *outStride)
	{
		ProcessFile(filePath, "PCN", outPVerts, outCount, outStride);
	}

	void ObjLoader::LoadStatic(float ** outPVerts, int * outCount, int *outStride)
	{
		*outPVerts = s_PCCubeVerts;
		*outCount = CUBE_VERTEX_COUNT;
		*outStride = FLOATS_PER_PC_VERTEX * sizeof(float);
	}

	//====================================================================================================

	bool ObjLoader::ProcessFile(const char *const fileName, const char *const format, float **outAllocatedVerts, int *outCount, int *outStride)
	{
		StringFuncs::StringCopy(fileName, s_inputFile, MAX_CHARS);

		// try to open input file, if it fails, do not proceed - indicate failure
		if (!OpenFile())
		{
			DebugConsole::Log("ObjLoader failed to ProcessFile()! Failed to open input file [%s]!\n", s_inputFile);
			return false;
		}

		// if analyze mode only analyze
		if (!AnalyzeFile())
		{
			DebugConsole::Log("ObjLoader failed to ProcessFile()! Failed to analyze input file [%s]!\n", s_inputFile);
			return false;
		}

		// do format things
		if (!CalcProps(format))
		{
			DebugConsole::Log("ObjLoader failed to CalcProps()!\n");
			return false;
		}

		// try to read data from input file into a mesh, if it fails, do not proceed - indicate failure
		if (!ReadFileIntoMesh(outAllocatedVerts))
		{
			DebugConsole::Log("ObjLoader failed to ProcessFile()! Failed to read input file [%s] into mesh!\n", s_inputFile);
			return false;
		}

		// try to close input file, if it fails, mark a warning and proceed
		if (!CloseFile())
		{
			DebugConsole::Log("ObjLoader failed to close input file [%s]! ObjLoader will continue to ProcessFile()!\n", s_inputFile);
		}

		// After writing has finished clean up, no need to clean up if only analyzed because nothing new'd
		if (!CleanUp())
		{
			DebugConsole::Log("ObjLoader failed to clean up!\n");
		}

		*outCount = s_numVertices;
		*outStride = s_stride;

		// success was achieved 
		DebugConsole::Log("ObjLoader successfully processed input file [%s]!\n", s_inputFile);
		return true;
	}

	const int MAX_LINE_SIZE = 512;
	bool ObjLoader::ReadFileIntoMesh(float **outAllocatedVerts)
	{
		// create space to place things into mesh properly
		// TODO: remove hard-coded GL_TRIANGLES, shader ID and Vertex format!!!
		int numFloats = s_numVertices*s_stride / sizeof(float);
		*outAllocatedVerts = new float[numFloats] { 0.0f };

		// make plenty of space for data to be filled in
		s_pMeshVertexPositions = new float[numFloats] {0.0f};
		s_pMeshVertexColors = new float[numFloats] {0.0f};
		s_pMeshVertexTextureCoords = new float[numFloats] {0.0f};
		s_pMeshVertexNormals = new float[numFloats] {0.0f};
		s_pTempIndices = new int[s_numVertices * 9]{ 0 };

		// create a buffer to hold the line and an integer to store the line number in
		char line[MAX_LINE_SIZE]{ 0 };
		int lineNumber = 0;

		// set colors if colors should be set
		if (s_hasColor)
		{
			if (!RandomizeVertexColors(numFloats))
			{
				DebugConsole::Log("Failed to set colors for mesh that should have colors set!\n");
				return false;
			}
		}

		// loop through each line in the file and process it, storing the recorded values 
		while (s_fileStream.getline(line, MAX_LINE_SIZE))
		{
			// Increment line number
			++lineNumber;

			// if processing fails for any line
			if (!ProcessLine(line, lineNumber))
			{
				// close the file and indicate failure
				CloseFile();
				DebugConsole::Log("ObjLoader failed to ReadFileIntoMesh()! Unable to ProcessLine() for line #%d: [%s]!\n", lineNumber, line);
				return false;
			}
		}

		// convert the recorded values into the vertices for a mesh
		if (!ProcessIntoVertices(outAllocatedVerts))
		{
			DebugConsole::Log("ObjLoader failed to ReadFileIntoMesh()! Read in data then failed to convert to mesh vertices!\n");
			return false;
		}

		// if the end of the file has been reached and no lines failed to process indicate success
		DebugConsole::Log("ObjLoader successfully read file [%s] into a mesh!\n", s_inputFile);
		return true;
	}

	bool ObjLoader::AnalyzeFile()
	{
		// variable to hold vertex count 
		int vertexCount = 0;

		// char array to hold line and int for line number
		char line[MAX_LINE_SIZE]{ 0 };
		int lineNumber = 0;

		// loop through each line of the file
		while (s_fileStream.getline(line, MAX_LINE_SIZE))
		{
			// increment line number
			++lineNumber;

			// increment count
			if (line[0] == 'f') { vertexCount += 3; } //TODO: replace if adding support for non-triangulated meshes
		}

		// output values and indicate success
		s_numVertices = vertexCount;

		s_fileStream.clear();
		s_fileStream.seekg(0, std::ios::beg);

		DebugConsole::Log("ObjLoader analyzed file [%s] and found [%d] vertices!\n", s_inputFile, vertexCount);
		return true;
	}

	bool ObjLoader::ProcessLine(const char *const line, int lineNumber)
	{
		// ignore comments!
		if (line[0] == '#') { return true; }

		// the line to be processed contains vertex data
		if (line[0] == 'v')
		{
			// try to add the vertex and respond accordingly
			if (!AddVertex(line, lineNumber))
			{
				DebugConsole::Log("ObjLoader failed to process line #%d: [%s]! AddVertex() failed!\n", lineNumber, line);
				return false;
			}

			// indicate success with no duplicate logging
			return true;
		}

		// the line to be processed contains index data
		if (line[0] == 'f')
		{
			// try to add the indices and respond accordingly
			if (!AddFaceIndices(line, lineNumber))
			{
				DebugConsole::Log("ObjLoader failed to process line #%d: [%s]! AddFaceIndices() failed!\n", lineNumber, line);
				return false;
			}

			// indicate success with no duplicate logging
			return true;
		}

		return true;
	}

	bool ObjLoader::OpenFile()
	{
		// open the .obj file
		s_fileStream.open(s_inputFile, std::ios::in);

		// if opened stream is bad indicate failure
		if (!s_fileStream)
		{
			DebugConsole::Log("ObjLoader failed to open input file [%s]! File stream is no good!\n", s_inputFile);
			return false;
		}

		// indicate success
		DebugConsole::Log("ObjLoader successfully opened file [%s]!\n", s_inputFile);
		return true;
	}

	bool ObjLoader::CloseFile()
	{
		// close file stream only if it is open
		if (s_fileStream.is_open())
		{
			s_fileStream.close();
		}

		// if file stream is still open after trying to close it indicate failure
		if (s_fileStream.is_open())
		{
			DebugConsole::Log("ObjLoader failed to close input file stream [%s]!\n", s_inputFile);
			return false;
		}

		// indicate success
		DebugConsole::Log("ObjLoader successfully closed input file [%s]!\n", s_inputFile);
		return true;
	}

	bool ObjLoader::AddVertex(const char *const line, int lineNumber)
	{
		// add vertex based on which part of the vertex it is and the vertex format
		if (StringFuncs::IsWhiteSpace(line[1]) && s_hasPosition) { return AddVertexPosition(line, lineNumber); }
		else if (line[1] == 't' && s_hasTexture) { return AddVertexTexture(line, lineNumber); }
		else if (line[1] == 'n' && s_hasNormal) { return AddVertexNormal(line, lineNumber); }
		else { return true; }
	}

	const int FLOATS_PER_POSITION = 3;
	bool ObjLoader::AddVertexPosition(const char * const line, int /*lineNumber*/)
	{
		// keep track of the location to add the next vertex
		static int s_nextVertex = 0;

		// line should look like v [float] [float] [float]
		// create std::things to parse the line
		std::string word;
		std::istringstream parse(line);

		// grab the letter v
		parse >> word;

		// grab the first float
		parse >> *(s_nextVertex*FLOATS_PER_POSITION + s_pMeshVertexPositions);

		// grab the second float
		parse >> *(s_nextVertex*FLOATS_PER_POSITION + s_pMeshVertexPositions + 1);

		// grab the third float
		parse >> *(s_nextVertex*FLOATS_PER_POSITION + s_pMeshVertexPositions + 2);

		// increment vertex number
		++s_nextVertex;

		return true;
	}

	const int FLOATS_PER_TEXTURE = 2;
	bool ObjLoader::AddVertexTexture(const char * const line, int /*lineNumber*/)
	{
		// keep track of the location to add the next vertex
		static int s_nextVertex = 0;

		// line should look like vt [float] [float]
		// create std::things to parse the line
		std::string word;
		std::istringstream parse(line);

		// grab the letters v and t
		parse >> word;

		// grab the first float
		parse >> *(s_nextVertex*FLOATS_PER_TEXTURE + s_pMeshVertexTextureCoords);

		// grab the second float
		parse >> *(s_nextVertex*FLOATS_PER_TEXTURE + s_pMeshVertexTextureCoords + 1);

		// increment vertex number
		++s_nextVertex;

		return true;
	}

	const int FLOATS_PER_NORMAL = 3;
	bool ObjLoader::AddVertexNormal(const char * const line, int /*lineNumber*/)
	{
		// keep track of the location to add the next vertex
		static int s_nextVertex = 0;

		// line should look like vn [float] [float] [float]
		// create std::things to parse the line
		std::string word;
		std::istringstream parse(line);

		// grab the letter v
		parse >> word;

		// grab the first float
		parse >> *(s_nextVertex*FLOATS_PER_NORMAL + s_pMeshVertexNormals);

		// grab the second float
		parse >> *(s_nextVertex*FLOATS_PER_NORMAL + s_pMeshVertexNormals + 1);

		// grab the third float
		parse >> *(s_nextVertex*FLOATS_PER_NORMAL + s_pMeshVertexNormals + 2);

		// increment vertex number
		++s_nextVertex;

		return true;
	}

	bool ObjLoader::RandomizeVertexColors(int numFloats)
	{
		for (int i = 0; i < numFloats - FLOATS_PER_COLOR; i += FLOATS_PER_COLOR)
		{
			// randomize colors
			*(s_pMeshVertexColors + i) = (1.0f*rand()) / RAND_MAX;
			*(s_pMeshVertexColors + i + 1) = (1.0f*rand()) / RAND_MAX;
			*(s_pMeshVertexColors + i + 2) = (1.0f*rand()) / RAND_MAX;
			*(s_pMeshVertexColors + i + 3) = 1.0f;
		}

		return true;
	}

	bool ObjLoader::AddFaceIndices(const char *const line, int /*lineNumber*/)
	{
		// keep track of the next index location
		static int s_nextIndex = 0;

		// std::things to parse line
		std::istringstream parse(line);
		std::string word;

		// eat the f and the space after it
		parse >> word;
		parse.get();

		// array to hold indices for x/x/x x/x/x x/x/x and ints for locations
		int indices[9]; // 1-based
		int type = 0;
		int index = 0;

		// while the line is not yet ended
		while (parse.peek() > 0)
		{
			// if it is a number store the number
			if (parse.peek() >= '0' && parse.peek() <= '9')
			{
				// grab and store number at proper location
				parse >> indices[index * 3 + type];
			}

			// if it is a slash move index
			if (parse.peek() == '/')
			{
				// eat the '/'
				parse.get();

				// indicate we have changed the index
				type++;
			}

			// if it is whitespace eat it and move to the next index
			if (StringFuncs::IsWhiteSpace(static_cast<char>(parse.peek())))
			{
				// eat the whitespace
				parse.get();

				// indicate we have changed index
				index++;
				type = 0;
			}
		}

		// add indices from array of one-based indices to mesh and convert to zero-based indices
		if (s_hasPosition) { s_pTempIndices[s_nextIndex++] = indices[0] - 1; }
		if (s_hasTexture) { s_pTempIndices[s_nextIndex++] = indices[1] - 1; }
		if (s_hasNormal) { s_pTempIndices[s_nextIndex++] = indices[2] - 1; }
		if (s_hasPosition) { s_pTempIndices[s_nextIndex++] = indices[3] - 1; }
		if (s_hasTexture) { s_pTempIndices[s_nextIndex++] = indices[4] - 1; }
		if (s_hasNormal) { s_pTempIndices[s_nextIndex++] = indices[5] - 1; }
		if (s_hasPosition) { s_pTempIndices[s_nextIndex++] = indices[6] - 1; }
		if (s_hasTexture) { s_pTempIndices[s_nextIndex++] = indices[7] - 1; }
		if (s_hasNormal) { s_pTempIndices[s_nextIndex++] = indices[8] - 1; }

		return true;
	}

	bool ObjLoader::ProcessIntoVertices(float **outAllocatedVerts)
	{
		int indexCount = (s_hasPosition ? 1 : 0) + (s_hasTexture ? 1 : 0) + (s_hasNormal ? 1 : 0);
		int floatsPerVertex = s_stride / sizeof(float);

		for (int i = 0; i < s_numVertices; ++i)
		{
			int offset = 0;
			int ioffset = 0;

			if (s_hasPosition)
			{
				float *pToStart = (*outAllocatedVerts) + (i*floatsPerVertex) + offset;
				float *pFromStart = s_pMeshVertexPositions + 3 * (*(s_pTempIndices + (i*indexCount) + ioffset));

				*(pToStart + 0) = *(pFromStart + 0);
				*(pToStart + 1) = *(pFromStart + 1);
				*(pToStart + 2) = *(pFromStart + 2);

				offset += FLOATS_PER_POSITION;
				ioffset++;
			}

			if (s_hasColor)
			{
				float *pToStart = (*outAllocatedVerts) + (i*floatsPerVertex) + offset;
				float *pFromStart = s_pMeshVertexPositions + 3 * (*(s_pTempIndices + (i*indexCount) + ioffset));

				*(pToStart + 0) = *(pFromStart + 0);
				*(pToStart + 1) = *(pFromStart + 1);
				*(pToStart + 2) = *(pFromStart + 2);
				
				offset += FLOATS_PER_COLOR;
			}

			if (s_hasTexture)
			{
				float *pToStart = (*outAllocatedVerts) + (i*floatsPerVertex) + offset;
				float *pFromStart = s_pMeshVertexPositions + 3 * (*(s_pTempIndices + (i*indexCount) + ioffset));

				*(pToStart + 0) = *(pFromStart + 0);
				*(pToStart + 1) = *(pFromStart + 1);
				*(pToStart + 2) = *(pFromStart + 2);
				
				offset += FLOATS_PER_TEXTURE;
				ioffset++;
			}

			if (s_hasNormal)
			{
				float *pToStart = (*outAllocatedVerts) + (i*floatsPerVertex) + offset;
				float *pFromStart = s_pMeshVertexPositions + 3 * (*(s_pTempIndices + (i*indexCount) + ioffset));

				*(pToStart + 0) = *(pFromStart + 0);
				*(pToStart + 1) = *(pFromStart + 1);
				*(pToStart + 2) = *(pFromStart + 2);
				
				offset += FLOATS_PER_NORMAL;
				ioffset++;
			}
		}

		return true;
	}

	bool ObjLoader::CleanUp()
	{
		// clean up after new'd vertices and indices
		if (s_pMeshVertexPositions) { delete[] s_pMeshVertexPositions; }
		if (s_pMeshVertexColors) { delete[] s_pMeshVertexColors; }
		if (s_pMeshVertexTextureCoords) { delete[] s_pMeshVertexTextureCoords; }
		if (s_pMeshVertexNormals) { delete[] s_pMeshVertexNormals; }
		if (s_pTempIndices) { delete[] s_pTempIndices; }

		// safe clean - only delete if they exist and mark as no longer existing after death
		s_pMeshVertexPositions = nullptr;
		s_pMeshVertexColors = nullptr;
		s_pMeshVertexTextureCoords = nullptr;
		s_pMeshVertexNormals = nullptr;
		s_pTempIndices = nullptr;

		DebugConsole::Log("ObjLoader successfully cleaned up!\n");
		return true;
	}

	bool ObjLoader::CalcProps(const char * const format)
	{
		/// umm ugly bare minimum stuffs todo more error checks if needed
		int numChars = StringFuncs::StringLen(format);
		if (numChars <= 0 || numChars > 4) { DebugConsole::Log("Invalid props format!\n"); return false; }
		if (format[0] != 'P') { DebugConsole::Log("Invalid props format! Format must have position!\n"); return false; }
		
		s_stride = FLOATS_PER_POSITION;
		s_hasPosition = true;
		int idx = 1;
		if (format[idx] == 'C') { s_hasColor = true; ++idx; s_stride += FLOATS_PER_COLOR; }
		if (format[idx] == 'T') { s_hasTexture = true; ++idx; s_stride += FLOATS_PER_TEXTURE; }
		if (format[idx] == 'N') { s_hasNormal = true; ++idx; s_stride += FLOATS_PER_NORMAL; }

		s_stride *= sizeof(float);
		return true;
	}

}