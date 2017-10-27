#include "Mouse.h"
#include "DebugConsole.h"
#include "EditorWindow.h"

namespace Capstone
{
	Mouse::MouseScrollCallback Mouse::s_onScroll = nullptr;
	void *Mouse::s_pInstance = nullptr;
	EditorWindow *Mouse::s_pMyWindow = nullptr;
	Mouse::MouseState Mouse::s_last;
	Mouse::MouseState Mouse::s_current;
	Mouse::MouseState Mouse::s_next;
	int Mouse::s_xPos = 0;
	int Mouse::s_yPos = 0;
	int Mouse::s_lastXPos = 0;
	int Mouse::s_lastYPos = 0;

	void Mouse::Clicked(int btn)
	{
		s_next.Press(btn);
	}

	void Mouse::Released(int btn)
	{
		s_next.Release(btn);
	}

	void Mouse::Scroll(int degrees)
	{
		if (s_onScroll) { s_onScroll(degrees, s_pInstance); }
	}

	void Mouse::ClearStateOutOfBounds()
	{
		if (s_xPos < 0 || s_xPos > s_pMyWindow->GetWidth() || s_yPos < 0 || s_yPos > s_pMyWindow->GetHeight())
		{
			ClearAll();
		}
	}

	bool Mouse::Initialize(MouseScrollCallback onScroll, void *pInstance, EditorWindow *pWindow)
	{
		ClearAll();

		s_onScroll = onScroll;
		s_pInstance = pInstance;
		s_pMyWindow = pWindow;

		DebugConsole::Log("Mouse Initialized!\n");
		return true;
	}

	void Mouse::Update(int x, int y)
	{
		s_last = s_current;
		s_current = s_next;
		
		s_lastXPos = s_xPos;
		s_lastYPos = s_yPos;

		s_xPos = x;
		s_yPos = y;

		ClearStateOutOfBounds();
	}

	bool Mouse::LeftMouseClicked()
	{
		return s_current.Set(LEFT_MOUSE_INDEX) && s_last.NotSet(LEFT_MOUSE_INDEX);
	}

	bool Mouse::LeftMouseReleased()
	{
		return s_current.NotSet(LEFT_MOUSE_INDEX) && s_last.Set(LEFT_MOUSE_INDEX);
	}

	bool Mouse::LeftMouseDown()
	{
		return s_current.Set(LEFT_MOUSE_INDEX);
	}

	bool Mouse::LeftMouseUp()
	{
		return s_current.NotSet(LEFT_MOUSE_INDEX);
	}

	bool Mouse::RightMouseClicked()
	{
		return s_current.Set(RIGHT_MOUSE_INDEX) && s_last.NotSet(RIGHT_MOUSE_INDEX);
	}

	bool Mouse::RightMouseReleased()
	{
		return s_current.NotSet(RIGHT_MOUSE_INDEX) && s_last.Set(RIGHT_MOUSE_INDEX);
	}

	bool Mouse::RightMouseDown()
	{
		return s_current.Set(RIGHT_MOUSE_INDEX);
	}

	bool Mouse::RightMouseUp()
	{
		return s_current.NotSet(RIGHT_MOUSE_INDEX);
	}

	bool Mouse::MiddleMouseClicked()
	{
		return s_current.Set(MIDDLE_MOUSE_INDEX) && s_last.NotSet(MIDDLE_MOUSE_INDEX);
	}

	bool Mouse::MiddleMouseReleased()
	{
		return s_current.NotSet(MIDDLE_MOUSE_INDEX) && s_last.Set(MIDDLE_MOUSE_INDEX);
	}

	bool Mouse::MiddleMouseDown()
	{
		return s_current.Set(MIDDLE_MOUSE_INDEX);
	}

	bool Mouse::MiddleMouseUp()
	{
		return s_current.NotSet(MIDDLE_MOUSE_INDEX);
	}
	
	int Mouse::GetMouseX()
	{
		return s_xPos;
	}

	int Mouse::GetMouseY()
	{
		return s_yPos;
	}

	int Mouse::GetMouseDeltaX()
	{
		return s_xPos - s_lastXPos;
	}

	int Mouse::GetMouseDeltaY()
	{
		return s_yPos - s_lastYPos;
	}

	void Mouse::ClearAll()
	{
		s_next.Clear();
		s_current.Clear();
		s_last.Clear();
	}
}