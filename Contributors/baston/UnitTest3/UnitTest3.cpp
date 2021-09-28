#include "pch.h"
#include "CppUnitTest.h"
#include "..\Project3\main.cpp"
#include "..\Project3\sum_integers.cpp"
#include "..\Project3\sum_integers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest3
{
	TEST_CLASS(UnitTest3)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			int a = 0;
			auto integers = { 1, 2, 3, 4, 5 };
			if (sum_integers(integers) == 15) {
				a = 1;
			}
			Assert::IsTrue(a == 1);
		}
	};
}
