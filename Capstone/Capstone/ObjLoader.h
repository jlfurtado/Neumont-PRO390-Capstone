#pragma once
#include <fstream>

namespace Capstone
{
	class ObjLoader
	{
	public:
		static void LoadObj(const char *const filePath, float **outPVerts, int *outCount, int *outStride);
		static void LoadStatic(float **outPVerts, int *outCount, int *outStride);

	private:
		static bool ProcessFile(const char *const fileName, const char *const format, float **outAllocatedVerts, int *outCount, int *outStride);
		static bool ReadFileIntoMesh(float **outAllocatedVerts);
		static bool AnalyzeFile();
		static bool ProcessLine(const char *const line, int lineNumber);
		static bool OpenFile();
		static bool CloseFile();

		static bool AddVertex(const char *const line, int lineNumber);
		static bool AddVertexPosition(const char *const line, int lineNumber);
		static bool AddVertexTexture(const char *const line, int lineNumber);
		static bool AddVertexNormal(const char *const line, int lineNumber);
		static bool RandomizeVertexColors(int numFloats);
		static bool AddFaceIndices(const char *const line, int lineNumber);
		static bool ProcessIntoVertices(float **outAllocatedVerts);
		static bool CleanUp();
		static bool CalcProps(const char *const format);

		static const int MAX_CHARS = 256;
		static char s_inputFile[MAX_CHARS];
		static std::ifstream s_fileStream;
		static int s_numVertices;
		static float *s_pMeshVertexPositions;
		static float *s_pMeshVertexColors;
		static float *s_pMeshVertexTextureCoords;
		static float *s_pMeshVertexNormals;
		static int *s_pTempIndices;
		static int s_stride;
		static bool s_hasColor;
		static bool s_hasTexture;
		static bool s_hasNormal;

		static const int CUBE_VERTEX_COUNT = 36;
		static const int FLOATS_PER_PC_VERTEX = 7;
		static const int CUBE_FLOAT_COUNT = CUBE_VERTEX_COUNT * FLOATS_PER_PC_VERTEX;
		static float s_PCCubeVerts[CUBE_FLOAT_COUNT];
	};
}


