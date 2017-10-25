#include "MousePicker.h"
#include <math.h>

namespace Capstone
{
	using namespace DirectX;

	XMVECTOR MousePicker::s_cameraPosition;
	XMVECTOR MousePicker::s_horizontalDirection;
	XMVECTOR MousePicker::s_verticalDirection;
	XMVECTOR MousePicker::s_viewDirection;
	float MousePicker::s_nearHorizontalLength = 0.0f;
	float MousePicker::s_nearVerticalLength = 0.0f;
	float MousePicker::s_farHorizontalLength = 0.0f;
	float MousePicker::s_farVerticalLength = 0.0f;
	float MousePicker::s_nearClipDistance = 0.0f;
	float MousePicker::s_screenHeight = 0.0f;
	float MousePicker::s_screenWidth = 0.0f;

	void MousePicker::SetCameraInfo(const XMVECTOR& cameraPosition, const XMVECTOR& cameraViewDir, const XMVECTOR& cameraUp)
	{
		s_cameraPosition = cameraPosition;
		s_viewDirection = XMVector3Normalize(cameraViewDir);
		s_horizontalDirection = XMVector3Normalize(XMVector3Cross(s_viewDirection, cameraUp));
		s_verticalDirection = XMVector3Normalize(XMVector3Cross(s_horizontalDirection,s_viewDirection));
	}

	void MousePicker::SetPerspectiveInfo(float fovyRadians, float nearClipDist, float screenWidth, float screenHeight, float farClipDist)
	{
		s_screenWidth = screenWidth;
		s_screenHeight = screenHeight;
		s_nearClipDistance = nearClipDist;

		float tRatio = tanf(fovyRadians / 2.0f);
		float aRatio = screenWidth / screenHeight;

		s_nearVerticalLength = tRatio * nearClipDist;
		s_nearHorizontalLength = s_nearVerticalLength * aRatio;

		s_farVerticalLength = tRatio * farClipDist;
		s_farHorizontalLength = s_farVerticalLength * aRatio;
	}

	XMVECTOR MousePicker::GetOrigin(int pixelX, int pixelY)
	{
		float wOver2 = s_screenWidth / 2.0f, hOver2 = s_screenHeight / 2.0f;
		float x = (pixelX - wOver2) / (wOver2);
		float y = (hOver2 - pixelY) / (hOver2);
		return s_cameraPosition + s_viewDirection * s_nearClipDistance + (x*s_horizontalDirection*s_nearHorizontalLength) + (y*s_verticalDirection*s_nearVerticalLength);
	}

	XMVECTOR MousePicker::GetDirection(int pixelX, int pixelY)
	{
		return GetOrigin(pixelX, pixelY) - s_cameraPosition;
	}

}