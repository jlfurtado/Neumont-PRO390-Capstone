#pragma once

#include <DirectXMath.h>

namespace Capstone
{
	class Variations
	{
	public:
		static float ScalarUniform(float v1, float v2);
		static DirectX::XMVECTOR VectorComponentUniform(const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2);
		static DirectX::XMVECTOR VectorUniform(const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2);

		static float ScalarBellApproximation(float v1, float v2, int sampleSize);
		static DirectX::XMVECTOR VectorComponentBellApproximation(const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2, int sampleSize);
		static DirectX::XMVECTOR VectorBellApproximation(const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2, int sampleSize);

	};
}
