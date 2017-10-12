#pragma once

namespace Capstone
{
	class Mouse
	{
		struct MouseState
		{
			unsigned char v;

			void Clear()
			{
				v = 0;
			}

			bool Set(int btn)
			{
				return (v & (1 << (btn % 8))) != 0;
			}

			bool NotSet(int btn)
			{
				return (v & (1 << (btn % 8))) == 0;
			}

			void Press(int btn)
			{
				v |= (1 << (btn % 8));
			}

			void Release(int btn)
			{
				v &= ~(1 << (btn % 8));
			}
		};

	public:
		static bool Initialize();
		static void Update();
		static bool LeftMouseClicked();
		static bool LeftMouseReleased();
		static bool LeftMouseDown();
		static bool LeftMouseUp();
		static bool RightMouseClicked();
		static bool RightMouseReleased();
		static bool RightMouseDown();
		static bool RightMouseUp();
		static bool MiddleMouseClicked();
		static bool MiddleMouseReleased();
		static bool MiddleMouseDown();
		static bool MiddleMouseUp();

	private:
		friend class EditorWindow;
		static void Clicked(int btn);
		static void Released(int btn);

		static const int LEFT_MOUSE_INDEX = 0;
		static const int MIDDLE_MOUSE_INDEX = 1;
		static const int RIGHT_MOUSE_INDEX = 2;

		static MouseState s_last;
		static MouseState s_current;
		static MouseState s_next;
	};
}