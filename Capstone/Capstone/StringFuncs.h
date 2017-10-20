#ifndef STRINGFUNCS_H
#define STRINGFUNCS_H

namespace Capstone
{
	class StringFuncs
	{
	public:
		// methods that test/measure/compare strings
		static bool StringsAreEqual(const char *const str1, const char *const str2);
		static bool IsPalindrome(const char *const str);
		static int StringCompare(const char *const str1, const char *const str2);
		static int StringLen(const char *const str);
		static int FindSubString(const char *const source, const char *const target);
		static bool StringEndsWith(const char *const source, const char *const target);
		static bool StringBeginsWith(const char *const source, const char *const target);

		// methods that modify strings
		static char *StringReverse(char *const str);
		static char *ToLower(char *const str);
		static char *ToUpper(char *const str);
		static int StringCopy(const char *const source, char *const target, int targetSize);
		static int StringConcatIntoBuffer(const char *const str1, const char *const str2, const char *const sep, char *const target, int targetSize);

		// methods that parse strings
		static bool GetSingleFloatFromString(const char *const string, float& outValue);
		static bool GetFloatsFromString(const char *const string, int numFloats, float *outValues);
		static bool GetIntsFromString(const char *const string, int numInts, int *outValues);
		static bool GetSingleIntFromString(const char *const string, int& outValue);
		static bool IsWhiteSpace(char c);
		static bool IsDigit(char c);

		// methods that I wrote to make something work
		static bool CountUp(char *numBuffer, unsigned bufferSize);
		static bool CountDown(char *numBuffer, unsigned bufferSize);
		static const char *GetPtrToInCharArray(const char *const strToLookFor);
		static const char *AddToCharArray(const char *const str);

	private:
		static const int PLENTY = 1024 * 1024;
		static char s_buffer[PLENTY];
		static int s_nextBufferSlot;
	};
}

#endif // ndef STRINGFUNCS_H