#include "Mouse.h"
#include "DebugConsole.h"

namespace Capstone
{
	Mouse::MouseState Mouse::s_last;
	Mouse::MouseState Mouse::s_current;
	Mouse::MouseState Mouse::s_next;
	int Mouse::s_xPos = 0;
	int Mouse::s_yPos = 0;

	void Mouse::Clicked(int btn)
	{
		s_next.Press(btn);
	}

	void Mouse::Released(int btn)
	{
		s_next.Release(btn);
	}

	bool Mouse::Initialize()
	{
		s_next.Clear();
		s_current.Clear();
		s_last.Clear();

		DebugConsole::Log("Mouse Initialized!\n");
		return true;
	}

	void Mouse::Update(int x, int y)
	{
		s_last = s_current;
		s_current = s_next;
		s_xPos = x;
		s_yPos = y;
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
}