#pragma once

#include <fstream>
#include <vector>
#include "VertexGroup.h"
#include "VariationController.h"

namespace Capstone
{
	class Mesh;
	class CustomIO
	{
	public:
		static bool WriteMeshToFile(const char *const filePath, const float *const pBaseVerts, int numVerts, int floatsPerVertex, const std::vector<VertexGroup> *const pVertexGroups, const VariationController *const pObjectVariations);
		static bool ReadMeshFromFile(const char *const filePath, float **outAllocatedVerts, int *outCount, int *outfloatsPerVertex, std::vector<VertexGroup> *pVertexGroups, VariationController *pObjectVariations);

	private:
		static bool OpenInputFile(const char *const filePath);
		static bool OpenOutputFile(const char *const filePath);

		static bool CloseInputFile();
		static bool CloseOutputFile();

		static bool WriteFormatData();
		static bool CheckFormatData();

		static bool WriteVersionData();
		static bool CheckVersionData();

		static bool WriteMeshInfo(const float * const pBaseVerts, int numVerts, int floatsPerVertex, const std::vector<VertexGroup>* const pVertexGroups, const VariationController * const pObjectVariations);
		static bool ReadMeshInfo(float **outAllocatedVerts, int *outCount, int *outfloatsPerVertex, std::vector<VertexGroup> *pVertexGroups, VariationController *pObjectVariations);

		static bool WriteMeshHeader(int numVerts, int floatsPerVertex);
		static bool ReadMeshHeader(int *outCount, int *outfloatsPerVertex);
		
		static bool WriteMeshVerts(const float * const pBaseVerts, int numFloats);
		static bool ReadMeshVerts(float **outAllocatedVerts, int numFloats);

		static bool WriteObjectLevelVariations(const VariationController * const pObjectVariations);
		static bool ReadObjectLevelVariations(VariationController *pObjectVariations);

		static bool WriteVertexGroupHeader(const std::vector<VertexGroup>* const pVertexGroups);
		static bool ReadVertexGroupHeader(std::vector<VertexGroup> *pVertexGroups, size_t *outNumGroups);
		
		static bool WriteVertexGroupData(const std::vector<VertexGroup>* const pVertexGroups);
		static bool ReadVertexGroupData(std::vector<VertexGroup> *pVertexGroups, size_t numGroups);

		static void ClearVertexData(float **outAllocatedVerts);
		static void ClearVertexGroups(std::vector<VertexGroup> *pVertexGroups);

		static bool WriteVariationData(const VariationController *const pVariation);
		static bool ReadVariationData(VariationController *pVariation);

		static bool WriteVertexGroup(const VertexGroup& vertexGroup);
		static bool ReadVertexGroup(VertexGroup& vertexGroup);

		static bool WriteVertexGroupIndices(const std::vector<int> *const pIndices);
		static bool ReadVertexGroupIndices(std::vector<int> *pIndices);

		template<typename T>
		static bool Write(const T *pData);

		template<typename T>
		static bool Read(T *pData);

		template<typename T>
		static bool WriteArray(const T *pArray, unsigned count);

		template<typename T>
		static bool ReadArray(T *pArray, unsigned count);

		static const int FORMAT_CHECK_DATA_SIZE = 37;
		static const char FORMAT_CHECK_DATA[FORMAT_CHECK_DATA_SIZE];
		static const int FORMAT_VERSION = 1;
		static std::ifstream s_inputFile;
		static std::ofstream s_outputFile;
	};

	template<typename T>
	inline bool CustomIO::Write(const T * pData)
	{
		return WriteArray(pData, 1);
	}

	template<typename T>
	inline bool CustomIO::Read(T * pData)
	{
		return ReadArray(pData, 1);
	}

	template<typename T>
	inline bool CustomIO::WriteArray(const T * pArray, unsigned count)
	{
		s_outputFile.write(reinterpret_cast<const char *>(pArray), sizeof(T) * count);
		return !s_outputFile.fail();
	}

	template<typename T>
	inline bool CustomIO::ReadArray(T * pArray, unsigned count)
	{
		s_inputFile.read(reinterpret_cast<char *>(pArray), sizeof(T) * count);
		return !s_inputFile.fail();
	}
}


