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
		static void TripleVectorUniform(const DirectX::XMVECTOR& l1, const DirectX::XMVECTOR& h1, DirectX::XMVECTOR *pOut1,
										const DirectX::XMVECTOR& l2, const DirectX::XMVECTOR& h2, DirectX::XMVECTOR *pOut2,
										const DirectX::XMVECTOR& l3, const DirectX::XMVECTOR& h3, DirectX::XMVECTOR *pOut3);

		static float ScalarBellApproximation(float v1, float v2, int sampleSize);
		static DirectX::XMVECTOR VectorComponentBellApproximation(const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2, int sampleSize);
		static DirectX::XMVECTOR VectorBellApproximation(const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2, int sampleSize);
		static void TripleVectorBellApproximation(const DirectX::XMVECTOR& l1, const DirectX::XMVECTOR& h1, DirectX::XMVECTOR *pOut1,
												  const DirectX::XMVECTOR& l2, const DirectX::XMVECTOR& h2, DirectX::XMVECTOR *pOut2,
												  const DirectX::XMVECTOR& l3, const DirectX::XMVECTOR& h3, DirectX::XMVECTOR *pOut3,
												  int sampleSize);

	};
}
