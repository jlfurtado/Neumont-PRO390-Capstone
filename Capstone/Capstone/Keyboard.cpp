#include "Keyboard.h"
#include "DebugConsole.h"

namespace Capstone
{
	Keyboard::KeyState Keyboard::s_next;
	Keyboard::KeyState Keyboard::s_current;
	Keyboard::KeyState Keyboard::s_previous;

	bool Keyboard::Initialize()
	{
		s_next.Clear();
		s_current.Clear();
		s_previous.Clear();

		DebugConsole::Log("Keyboard Initialized Successfully\n");
		return true;
	}
	
	void Keyboard::Update()
	{
		s_previous = s_current;
		s_current = s_next;
	}

	bool Keyboard::IsKeyDown(int vKey)
	{
		return s_current.Set(vKey);
	}

	bool Keyboard::IsKeyUp(int vKey)
	{
		return s_current.NotSet(vKey);
	}

	bool Keyboard::IsKeyPressed(int vKey)
	{
		return s_current.Set(vKey) && s_previous.NotSet(vKey);
	}

	bool Keyboard::IsKeyReleased(int vKey)
	{
		return s_current.NotSet(vKey) && s_previous.Set(vKey);
	}

	void Keyboard::KeyHit(int vKey)
	{
		s_next.Press(vKey);
	}

	void Keyboard::KeyRelease(int vKey)
	{
		s_next.Release(vKey);
	}

}