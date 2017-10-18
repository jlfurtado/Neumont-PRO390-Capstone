#pragma once

namespace Capstone
{
	class ObjLoader
	{
	public:
		static void LoadObj(const char *const filePath, float **outPVerts, int *outSize);
		static void LoadPreset(const char *const name, float **outPVerts, int *outSize, int *outStride);

	private:
		static const int CUBE_VERTEX_COUNT = 36;
		static const int FLOATS_PER_PC_VERTEX = 7;
		static const int CUBE_FLOAT_COUNT = CUBE_VERTEX_COUNT * FLOATS_PER_PC_VERTEX;
		static float s_PCCubeVerts[CUBE_FLOAT_COUNT];
	};
}


