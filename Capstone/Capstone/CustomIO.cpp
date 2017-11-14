#include "CustomIO.h"
#include "DebugConsole.h"
#include "StringFuncs.h"
#include "Utils.h"

namespace Capstone
{
	const char CustomIO::FORMAT_CHECK_DATA[FORMAT_CHECK_DATA_SIZE] = "JUSTINS_FANCY_CUSTOM_CAPSTONE_FORMAT";
	std::ifstream CustomIO::s_inputFile;
	std::ofstream CustomIO::s_outputFile;

	/*
	Format:
	 - Mesh Header Info
	 - Mesh Data
	 - Object Level Variation Data
	 - Vertex Group Header Info
	 - Vertex Groups Data
	*/

	bool CustomIO::WriteMeshToFile(const char * const filePath, const float * const pBaseVerts, int numVerts, int floatsPerVertex, const std::vector<VertexGroup>* const pVertexGroups, const VariationController * const pObjectVariations)
	{
		if (!OpenOutputFile(filePath)) 
		{ 
			DebugConsole::Log("ERROR: Failed to WriteMeshToFile! Could not open output file [%s]!\n", filePath); 
			return false;
		}

		if (!WriteFormatData())
		{
			DebugConsole::Log("ERROR: Failed to WriteMeshToFile! Could not WriteFormatData!\n");
			CloseOutputFile();
			return false;
		}

		if (!WriteVersionData())
		{
			DebugConsole::Log("ERROR: Failed to WriteMeshToFile! Could not WriteVersionData!\n");
			CloseOutputFile();
			return false;
		}

		if (!WriteMeshInfo(pBaseVerts, numVerts, floatsPerVertex, pVertexGroups, pObjectVariations))
		{
			DebugConsole::Log("ERROR: Failed to WriteMeshToFile! Could not WriteMeshInfo!\n");
			CloseOutputFile();
			return false;
		}

		if (!CloseOutputFile())
		{
			DebugConsole::Log("ERROR: Failed to CloseOutputFile()! Continuing execution!\n");
		}

		return true;
	}

	bool CustomIO::ReadMeshFromFile(const char * const filePath, float ** outAllocatedVerts, int * outNumVerts, int * outfloatsPerVertex, std::vector<VertexGroup>* pVertexGroups, VariationController * pObjectVariations)
	{
		if (!OpenInputFile(filePath))
		{
			DebugConsole::Log("ERROR: Failed to ReadMeshFromFile! Could not open input file [%s]!\n", filePath);
			return false;
		}

		if (!CheckFormatData())
		{
			DebugConsole::Log("ERROR: Failed to ReadMeshFromFile! Failed to CheckFormatData! Ensure that file is of the correct format!\n");
			CloseInputFile();
			return false;
		}

		if (!CheckVersionData())
		{
			DebugConsole::Log("ERROR: Failed to ReadMeshFromFile! Failed to CheckVersionData! You are probably trying to load a file made using an older version of the CustomIO Exporter!\n");
			CloseInputFile();
			return false;
		}

		if (!ReadMeshInfo(outAllocatedVerts, outNumVerts, outfloatsPerVertex, pVertexGroups, pObjectVariations))
		{
			DebugConsole::Log("ERROR: Failed to ReadMeshFromFile! Failed to ReadMeshInfo!\n");
			CloseInputFile();
			return false;
		}

		if (!CloseInputFile())
		{
			DebugConsole::Log("ERROR: Failed to CloseInputFile! Continuing Execution!\n");	
		}

		return true;
	}

	bool CustomIO::OpenInputFile(const char * const filePath)
	{
		s_inputFile.open(filePath, std::ios::binary | std::ios::in);
		s_inputFile.clear();
		s_inputFile.seekg(0, std::ios::beg);
		return s_inputFile.is_open() && s_inputFile.good();
	}

	bool CustomIO::OpenOutputFile(const char * const filePath)
	{
		s_outputFile.open(filePath, std::ios::binary | std::ios::out);
		s_outputFile.clear();
		s_outputFile.seekp(0, std::ios::beg);
		return s_outputFile.is_open() && s_outputFile.good();
	}

	bool CustomIO::CloseInputFile()
	{
		s_inputFile.close();
		return !s_inputFile.is_open();
	}

	bool CustomIO::CloseOutputFile()
	{
		s_outputFile.close();
		return !s_outputFile.is_open();
	}

	bool CustomIO::WriteFormatData()
	{
		return WriteArray(FORMAT_CHECK_DATA, FORMAT_CHECK_DATA_SIZE);
	}

	bool CustomIO::CheckFormatData()
	{
		char buffer[FORMAT_CHECK_DATA_SIZE]{ '\0' };
		return ReadArray(&buffer[0], FORMAT_CHECK_DATA_SIZE) && StringFuncs::StringsAreEqual(&buffer[0], FORMAT_CHECK_DATA);
	}

	bool CustomIO::WriteVersionData()
	{
		return Write(&FORMAT_VERSION);
	}

	bool CustomIO::CheckVersionData()
	{
		int readVersion = -1;
		return Read(&readVersion) && (readVersion == FORMAT_VERSION);
	}

	bool CustomIO::WriteMeshInfo(const float * const pBaseVerts, int numVerts, int floatsPerVertex, const std::vector<VertexGroup>* const pVertexGroups, const VariationController * const pObjectVariations)
	{
		if (!WriteMeshHeader(numVerts, floatsPerVertex)) { DebugConsole::Log("ERROR: Failed to WriteMeshInfo! Failed to WriteMeshHeader!\n"); return false; }
		if (!WriteMeshVerts(pBaseVerts, numVerts * floatsPerVertex)) { DebugConsole::Log("ERROR: Failed to WriteMeshInfo! Failed to WriteMeshVerts!\n"); return false; }
		if (!WriteObjectLevelVariations(pObjectVariations)) { DebugConsole::Log("ERROR: Failed to WriteMeshInfo! Failed to WriteObjectLevelVariations!\n"); return false; }
		if (!WriteVertexGroupHeader(pVertexGroups)) { DebugConsole::Log("ERROR: Failed to WriteMeshInfo! Failed to WriteVertexGroupHeader!\n"); return false; }
		if (!WriteVertexGroupData(pVertexGroups)) { DebugConsole::Log("ERROR: Failed to WriteMeshInfo! Faled to WriteVertexGroupData!\n"); return false; }
		return true;
	}

	bool CustomIO::ReadMeshInfo(float ** outAllocatedVerts, int * outNumVerts, int * outfloatsPerVertex, std::vector<VertexGroup>* pVertexGroups, VariationController * pObjectVariations)
	{
		size_t numGroups = 0;
		if (!ReadMeshHeader(outNumVerts, outfloatsPerVertex)) { DebugConsole::Log("ERROR: Failed to ReadMeshInfo! Failed to ReadMeshHeader!\n"); return false; }
		if (!ReadMeshVerts(outAllocatedVerts, (*outNumVerts) * (*outfloatsPerVertex))) { DebugConsole::Log("ERROR: Failed to ReadMeshInfo! Failed to ReadMeshVerts!\n"); ClearVertexData(outAllocatedVerts); return false; }
		if (!ReadObjectLevelVariations(pObjectVariations)) { DebugConsole::Log("ERROR: Failed to ReadMeshInfo! Failed to ReadObjectLevelVariations!\n"); ClearVertexData(outAllocatedVerts); return false; }
		if (!ReadVertexGroupHeader(pVertexGroups, &numGroups)) { DebugConsole::Log("ERROR: Failed to ReadMeshInfo! Failed to ReadVertexGroupHeader!\n"); ClearVertexData(outAllocatedVerts); return false; }
		if (!ReadVertexGroupData(pVertexGroups, numGroups)) { DebugConsole::Log("ERROR: Failed to ReadMeshInfo! Failed to ReadVertexGroupData!\n"); ClearVertexData(outAllocatedVerts); ClearVertexGroups(pVertexGroups); return false; }
		return true;
	}

	bool CustomIO::WriteMeshHeader(int numVerts, int floatsPerVertex)
	{
		return Write(&numVerts) && Write(&floatsPerVertex);
	}

	bool CustomIO::ReadMeshHeader(int * outNumVerts, int * outfloatsPerVertex)
	{
		return Read(outNumVerts) && Read(outfloatsPerVertex);
	}

	bool CustomIO::WriteMeshVerts(const float * const pBaseVerts, int numFloats)
	{
		return WriteArray(pBaseVerts, numFloats);
	}

	bool CustomIO::ReadMeshVerts(float ** outAllocatedVerts, int numFloats)
	{
		*outAllocatedVerts = new float[numFloats] {0.0f};
		return ReadArray(*outAllocatedVerts, numFloats);
	}

	bool CustomIO::WriteObjectLevelVariations(const VariationController * const pObjectVariations)
	{
		return WriteVariationData(pObjectVariations);
	}

	bool CustomIO::ReadObjectLevelVariations(VariationController * pObjectVariations)
	{
		return ReadVariationData(pObjectVariations);
	}

	bool CustomIO::WriteVertexGroupHeader(const std::vector<VertexGroup>* const pVertexGroups)
	{
		size_t size = pVertexGroups->size();
		return Write(&size);
	}

	bool CustomIO::ReadVertexGroupHeader(std::vector<VertexGroup>* /*pVertexGroups*/, size_t *outNumGroups)
	{
		return Read(outNumGroups);
	}

	bool CustomIO::WriteVertexGroupData(const std::vector<VertexGroup>* const pVertexGroups)
	{
		for (size_t i = 0; i < pVertexGroups->size(); ++i)
		{
			if (!WriteVertexGroup(pVertexGroups->at(i))) { DebugConsole::Log("Failed to WriteVertexGroupData! Failed to WriteVertexGroup(%u)!\n", i); return false; }
		}

		return true;
	}

	bool CustomIO::ReadVertexGroupData(std::vector<VertexGroup>* pVertexGroups, size_t numGroups)
	{
		pVertexGroups->resize(numGroups);

		for (size_t i = 0; i < numGroups; ++i)
		{
			if (!ReadVertexGroup(pVertexGroups->at(i))) { DebugConsole::Log("Failed to ReadVertexGroupData! Failed to ReadVertexGroup(%u)!\n", i); return false; }
		}

		return true;
	}

	void CustomIO::ClearVertexData(float ** outAllocatedVerts)
	{
		MyUtils::SafeDeleteArray(*outAllocatedVerts);
	}

	void CustomIO::ClearVertexGroups(std::vector<VertexGroup>* pVertexGroups)
	{
		for (size_t i = 0; i < pVertexGroups->size(); ++i)
		{
			pVertexGroups->at(i).Clear();
		}

		pVertexGroups->clear();
	}

	bool CustomIO::WriteVariationData(const VariationController * const pVariation)
	{
		static const void *NEEDS_POINTER_HOOKUP = nullptr;
		return Write(&pVariation->m_lowScale)
			&& Write(&pVariation->m_lowTranslation)
			&& Write(&pVariation->m_lowRotation)
			&& Write(&pVariation->m_highScale)
			&& Write(&pVariation->m_highTranslation)
			&& Write(&pVariation->m_highRotation)
			&& Write(&NEEDS_POINTER_HOOKUP)
			&& Write(&NEEDS_POINTER_HOOKUP)
			&& Write(&NEEDS_POINTER_HOOKUP)
			&& Write(&pVariation->m_variationType)
			&& Write(&NEEDS_POINTER_HOOKUP)
			&& Write(&NEEDS_POINTER_HOOKUP);
	}

	bool CustomIO::ReadVariationData(VariationController * pVariation)
	{
		return Read(&pVariation->m_lowScale)
			&& Read(&pVariation->m_lowTranslation)
			&& Read(&pVariation->m_lowRotation)
			&& Read(&pVariation->m_highScale)
			&& Read(&pVariation->m_highTranslation)
			&& Read(&pVariation->m_highRotation)
			&& Read(&pVariation->m_pScale) // should be nullptr after read
			&& Read(&pVariation->m_pTranslation) // should be nullptr after read
			&& Read(&pVariation->m_pRotation) // should be nullptr after read
			&& Read(&pVariation->m_variationType)
			&& Read(&pVariation->m_pInstance) // should be nullptr after read
			&& Read(&pVariation->m_onVariationChanged); // should be nullptr after read
	}

	bool CustomIO::WriteVertexGroup(const VertexGroup & vertexGroup)
	{
		return Write(&vertexGroup.m_scale)
			&& Write(&vertexGroup.m_translation)
			&& Write(&vertexGroup.m_rotation)
			&& Write(&vertexGroup.m_pivot)
			&& WriteVariationData(&vertexGroup.m_variation)
			&& WriteVertexGroupIndices(&vertexGroup.m_vertexIndices);
	}

	bool CustomIO::ReadVertexGroup(VertexGroup & vertexGroup)
	{
		return Read(&vertexGroup.m_scale)
			&& Read(&vertexGroup.m_translation)
			&& Read(&vertexGroup.m_rotation)
			&& Read(&vertexGroup.m_pivot)
			&& ReadVariationData(&vertexGroup.m_variation)
			&& ReadVertexGroupIndices(&vertexGroup.m_vertexIndices);
	}

	bool CustomIO::WriteVertexGroupIndices(const std::vector<int>* const pIndices)
	{
		size_t numIndices = pIndices->size();
		return Write(&numIndices) && WriteArray(pIndices->data(), numIndices);
	}

	bool CustomIO::ReadVertexGroupIndices(std::vector<int>* pIndices)
	{
		size_t indexCount = 0;
		if (!Read(&indexCount)) { return false; }

		pIndices->resize(indexCount, 0);
		return ReadArray(pIndices->data(), indexCount);
	}

}