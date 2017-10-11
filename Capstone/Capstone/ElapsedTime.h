#ifndef GAME_TIME_H_
#define GAME_TIME_H_


namespace Capstone
{
	class ElapsedTime
	{
	public:
		static bool  Initialize();
		static bool  Shutdown();
		static float GetElapsedTime();      // returns num of seconds game has been running
		static float GetLastFrameTime(); // Only call this ONCE per frame!!!
	};
}

#endif // ndef GAME_TIME_H_
