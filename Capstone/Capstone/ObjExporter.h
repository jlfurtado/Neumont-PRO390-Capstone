#pragma once
#include <fstream>

namespace Capstone
{
	class ObjExporter
	{
	public:
		static bool WriteObj(const char *const filePath, const char *const format, const float *const pVertsToWrite, int vertexCount);

	private:
		static bool ProcessFile(const char *const filePath, const char *const format, const float *const pVertsToWrite, int vertexCount);
		static bool OpenFile();
		static bool CloseFile();
		static bool WriteMeshOutToFile(const float *const pVertsToWrite, int vertexCount);
		static bool WriteVertexPositions(const float *const pVertsToWrite, int vertexCount);
		static bool WriteVertexTextures(const float *const pVertsToWrite, int vertexCount);
		static bool WriteVertexNormals(const float *const pVertsToWrite, int vertexCount);
		static bool WriteVertexFaceIndices(int vertexCount);
		static bool GetFaceIndicesP(char *buffer, int bufferSize, int idx);
		static bool GetFaceIndicesPT(char *buffer, int bufferSize, int idx);
		static bool GetFaceIndicesPN(char *buffer, int bufferSize, int idx);
		static bool GetFaceIndicesPTN(char *buffer, int bufferSize, int idx);

		static bool CleanUp();
		static bool CalcProps(const char *const format);

		template<typename T>
		static bool Write(const T *pData);

		template<typename T>
		static bool WriteArray(const T *pArray, unsigned count);

		static const int MAX_CHARS = 256;
		static char s_outputFileName[MAX_CHARS];
		static std::ofstream s_outputFileStream;
		static int s_stride;
		static int s_floatsPerVertex;
		static bool s_hasColor;
		static bool s_hasTexture;
		static bool s_hasNormal;
		static int s_positionInStart;
		static int s_colorInStart;
		static int s_textureInStart;
		static int s_normalInStart;
	};

	template<typename T>
	inline bool ObjExporter::Write(const T * pData)
	{
		return WriteArray(pData, 1);
	}

	template<typename T>
	inline bool ObjExporter::WriteArray(const T * pArray, unsigned count)
	{
		s_outputFileStream.write(reinterpret_cast<const char *>(pArray), sizeof(T) * count);
		return !s_outputFileStream.fail();
	}
}

