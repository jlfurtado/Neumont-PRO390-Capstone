#include "Variations.h"
#include "MathUtility.h"

namespace Capstone
{
	using namespace DirectX;

	float Variations::ScalarUniform(float v1, float v2)
	{
		return MathUtility::Rand(MathUtility::Min(v1, v2), MathUtility::Max(v1, v2));
	}

	XMVECTOR Variations::VectorComponentUniform(const XMVECTOR & v1, const XMVECTOR & v2)
	{
		XMFLOAT4 f1, f2;
		XMStoreFloat4(&f1, v1);
		XMStoreFloat4(&f2, v2);
		return XMVectorSet(ScalarUniform(f1.x, f2.x),
						   ScalarUniform(f1.y, f2.y),
						   ScalarUniform(f1.z, f2.z),
						   ScalarUniform(f1.w, f2.w));
	}

	XMVECTOR Variations::VectorUniform(const XMVECTOR & v1, const XMVECTOR & v2)
	{
		float t = ScalarUniform(0.0f, 1.0f);
		return (v1 * t) + ((1.0f - t) * v2);
	}
	
	float Variations::ScalarBellApproximation(float v1, float v2, int sampleSize)
	{
		float low = MathUtility::Min(v1, v2);
		float max = MathUtility::Max(v1, v2);
		float avg = MathUtility::Rand(low, max);
		for (int i = 1; i < sampleSize; ++i) { avg += MathUtility::Rand(low, max); }
		return avg / sampleSize;
	}

	XMVECTOR Variations::VectorComponentBellApproximation(const XMVECTOR & v1, const XMVECTOR & v2, int sampleSize)
	{
		XMFLOAT4 f1, f2;
		XMStoreFloat4(&f1, v1);
		XMStoreFloat4(&f2, v2);
		return XMVectorSet(ScalarBellApproximation(f1.x, f2.x, sampleSize),
						   ScalarBellApproximation(f1.y, f2.y, sampleSize),
						   ScalarBellApproximation(f1.z, f2.z, sampleSize),
						   ScalarBellApproximation(f1.w, f2.w, sampleSize));
	}

	XMVECTOR Variations::VectorBellApproximation(const XMVECTOR & v1, const XMVECTOR & v2, int sampleSize)
	{
		float t = ScalarBellApproximation(0.0f, 1.0f, sampleSize);
		return (v1 * t) + ((1.0f - t) * v2);
	}
}