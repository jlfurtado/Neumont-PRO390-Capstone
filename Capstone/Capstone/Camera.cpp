#include "Camera.h"
#include "DebugConsole.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "MathUtility.h"

namespace Capstone
{
	using namespace DirectX; // ITS UGLY BUT I DON'T GET OPERATORS WITHOUT IT

	Camera::Camera()
		: m_position(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f)), 
		  m_forward(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f)),
		  m_up(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f)),
		  m_speed(10.0f), m_rotateSpeed(0.5f)
	{
	}

	Camera::~Camera()
	{
	}

	DirectX::XMMATRIX *Camera::GetWorldToViewMatrixPointer()
	{
		return &m_worldToViewMatrix;
	}

	DirectX::XMMATRIX Camera::GetWorldToViewMatrix()
	{
		return m_worldToViewMatrix;
	}

	DirectX::XMVECTOR * Camera::GetPositionPointer()
	{
		return &m_position;
	}

	DirectX::XMVECTOR * Camera::GetViewDirPointer()
	{
		return &m_forward;
	}

	DirectX::XMVECTOR * Camera::GetUpPointer()
	{
		return &m_up;
	}

	void Camera::Update(float dt)
	{
		float moveAmount = dt * m_speed;
		XMVECTOR f = m_forward * moveAmount;
		XMVECTOR r = XMVector3Cross(m_up, m_forward) * moveAmount;
		XMVECTOR u = m_up * moveAmount;

		if (!Keyboard::IsKeyDown(VK_SHIFT))
		{
			if (Keyboard::IsKeyDown('W')) { m_position += f; }
			if (Keyboard::IsKeyDown('A')) { m_position -= r; }
			if (Keyboard::IsKeyDown('S')) { m_position -= f; }
			if (Keyboard::IsKeyDown('D')) { m_position += r; }
			if (Keyboard::IsKeyDown(VK_UP)) { m_position += u; }
			if (Keyboard::IsKeyDown(VK_DOWN)) { m_position -= u; }
		}

		if (Mouse::RightMouseDown())
		{
			int x = Mouse::GetMouseDeltaX();
			int y = Mouse::GetMouseDeltaY();
			float deltaY = y * m_rotateSpeed;
			float deltaX = x * m_rotateSpeed;
			m_pitch += deltaY;
			m_yaw += deltaX;

			if (m_pitch > 89.0f) { m_pitch = 89.0f; return; }
			if (m_pitch < -89.0f) { m_pitch = -89.0f; return; }

			XMMATRIX horiz = XMMatrixRotationNormal(m_up, MathUtility::ToRadians(deltaX));
			XMMATRIX vert = XMMatrixRotationNormal(XMVector3Cross(m_up, m_forward), MathUtility::ToRadians(deltaY));
			m_forward = XMVector3Normalize(XMVector3Transform(m_forward, vert * horiz));
			//m_up = XMVector3Normalize(XMVector3Transform(m_up, vert * horiz));
		}

		DirectX::XMMATRIX viewMat = DirectX::XMMatrixLookAtLH(m_position, m_position + m_forward, m_up);
		m_worldToViewMatrix = DirectX::XMMatrixTranspose(viewMat);
	}
}