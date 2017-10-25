#pragma once

#include <DirectXMath.h>

namespace Capstone
{
	class Frustum
	{
	public:
		Frustum(const DirectX::XMVECTOR& cameraPosition, const DirectX::XMVECTOR& cameraViewDir, const DirectX::XMVECTOR& cameraUp,
				float fovyRadians, float nearClipDistance, float screenWidth, float screenHeight, float farClipDistance);
		~Frustum();

		bool PointInFrustum(const DirectX::XMVECTOR& point);

	private:
		static const int PLANES_PER_FRUSTUM = 6;
		static const int TOP_PLANE = 0, BOTTOM_PLANE = 1, LEFT_PLANE = 2, RIGHT_PLANE = 3, NEAR_PLANE = 4, FAR_PLANE= 5;
		static const int POINTS_PER_FRUSTUM = 8;
		static const int FAR_TOP_LEFT = 0, FAR_TOP_RIGHT = 1, FAR_BOTTOM_LEFT = 2, FAR_BOTTOM_RIGHT = 3,
						 NEAR_TOP_LEFT = 4, NEAR_TOP_RIGHT = 5, NEAR_BOTTOM_LEFT = 6, NEAR_BOTTOM_RIGHT = 7;
		DirectX::XMVECTOR m_planes[PLANES_PER_FRUSTUM];
		DirectX::XMVECTOR m_points[POINTS_PER_FRUSTUM];
	};

}

