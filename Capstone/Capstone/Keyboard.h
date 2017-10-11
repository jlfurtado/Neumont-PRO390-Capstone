#pragma once

namespace Capstone
{
	class Keyboard
	{
		struct KeyState
		{
			static const unsigned long long NUM_KEYS = 256;
			static const unsigned long long VAL_SIZE = sizeof(unsigned long long);
			static const unsigned long long NUM_VALS = NUM_KEYS / VAL_SIZE;

			unsigned long long v[NUM_VALS];

			void Clear()
			{
				for (int i = 0; i < NUM_VALS; ++i) { v[i] = 0; }
			}

			inline bool Set(int n)
			{
				return (v[n / NUM_VALS] & (1ULL << (n % VAL_SIZE))) != 0;
			}

			inline bool NotSet(int n)
			{
				return (v[n / NUM_VALS] & (1ULL << (n % VAL_SIZE))) == 0;
			}

			inline void Press(int n)
			{
				v[n / NUM_VALS] |= (1ULL << (n % VAL_SIZE));
			}

			inline void Release(int n)
			{
				v[n / NUM_VALS] &= ~(1ULL << (n % VAL_SIZE));
			}
		};

	public:
		static bool Initialize();
		static void Update();
		static bool IsKeyDown(int vKey);
		static bool IsKeyUp(int vKey);
		static bool IsKeyPressed(int vKey);
		static bool IsKeyReleased(int vKey);

	private:
		friend class EditorWindow;
		static void KeyHit(int vKey);
		static void KeyRelease(int vKey);

		static KeyState s_next;
		static KeyState s_current;
		static KeyState s_previous;
	};
}
