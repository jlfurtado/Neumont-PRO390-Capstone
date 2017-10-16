#pragma once

#include <DirectXMath.h>

namespace Capstone
{
	class Variations
	{
	public:
		static float ScalarUniform(float v1, float v2);
		static DirectX::XMVECTOR VectorUniform(const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2);
	};
}
