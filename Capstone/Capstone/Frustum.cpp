#include "Frustum.h"

namespace Capstone
{
	using namespace DirectX;

	Frustum::Frustum(const XMVECTOR & cameraPosition, const XMVECTOR & cameraViewDir, const XMVECTOR & cameraUp, float fovyRadians, float nearClipDistance, float screenWidth, float screenHeight, float farClipDistance)
	{
		// calc camera stuff
		XMVECTOR viewDir = XMVector3Normalize(cameraViewDir);
		XMVECTOR horizontalDirection = XMVector3Normalize(XMVector3Cross(viewDir, cameraUp));
		XMVECTOR verticalDirection = XMVector3Normalize(XMVector3Cross(horizontalDirection, viewDir));

		// calc perspective stuff
		float tRatio = tanf(fovyRadians / 2.0f);
		float aRatio = screenWidth / screenHeight;

		float nearVerticalLength = tRatio * nearClipDistance;
		float nearHorizontalLength = nearVerticalLength * aRatio;

		float farVerticalLength = tRatio * farClipDistance;
		float farHorizontalLength = farVerticalLength * aRatio;
		
		// Calculate points
		XMVECTOR farCenter = cameraPosition + viewDir * farClipDistance;
		XMVECTOR farUp = farVerticalLength * verticalDirection;
		XMVECTOR farRight = farHorizontalLength * horizontalDirection;
		m_points[FAR_TOP_LEFT]     = farCenter + farUp - farRight;
		m_points[FAR_TOP_RIGHT]    = farCenter + farUp + farRight;
		m_points[FAR_BOTTOM_LEFT]  = farCenter - farUp - farRight;
		m_points[FAR_BOTTOM_RIGHT] = farCenter - farUp + farRight;

		XMVECTOR nearCenter = cameraPosition + viewDir * nearClipDistance;
		XMVECTOR nearUp = nearVerticalLength * verticalDirection;
		XMVECTOR nearRight = nearHorizontalLength * horizontalDirection;
		m_points[NEAR_TOP_LEFT]     = nearCenter + nearUp - nearRight;
		m_points[NEAR_TOP_RIGHT]    = nearCenter + nearUp + nearRight;
		m_points[NEAR_BOTTOM_LEFT]  = nearCenter - nearUp - nearRight;
		m_points[NEAR_BOTTOM_RIGHT] = nearCenter - nearUp + nearRight;

		// calculate planes, normals to center
		m_planes[NEAR_PLANE]   = XMPlaneFromPoints(m_points[NEAR_TOP_RIGHT], m_points[NEAR_BOTTOM_RIGHT], m_points[NEAR_BOTTOM_LEFT]);
		m_planes[FAR_PLANE]	   = XMPlaneFromPoints(m_points[FAR_TOP_LEFT], m_points[FAR_BOTTOM_LEFT], m_points[FAR_BOTTOM_RIGHT]);
		m_planes[TOP_PLANE]	   = XMPlaneFromPoints(m_points[NEAR_TOP_LEFT], m_points[FAR_TOP_LEFT], m_points[FAR_TOP_RIGHT]);
		m_planes[BOTTOM_PLANE] = XMPlaneFromPoints(m_points[FAR_BOTTOM_LEFT], m_points[NEAR_BOTTOM_LEFT], m_points[NEAR_BOTTOM_RIGHT]);
		m_planes[LEFT_PLANE]   = XMPlaneFromPoints(m_points[NEAR_TOP_LEFT], m_points[NEAR_BOTTOM_LEFT], m_points[FAR_BOTTOM_LEFT]);
		m_planes[RIGHT_PLANE]  = XMPlaneFromPoints(m_points[FAR_TOP_RIGHT], m_points[FAR_BOTTOM_RIGHT], m_points[NEAR_BOTTOM_RIGHT]);
	}

	Frustum::~Frustum()
	{
	}

	bool Frustum::PointInFrustum(const DirectX::XMVECTOR & point)
	{
		for (int i = 0; i < PLANES_PER_FRUSTUM; ++i)
		{
			if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], point)) < 0.0f)
			{
				return false;
			}
		}

		return true;
	}
}
