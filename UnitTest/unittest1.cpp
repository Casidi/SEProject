#include "CppUnitTest.h"
#include "../SEProject/Database.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{		
	TEST_CLASS(TestDataFrame)
	{
	public:
		TEST_METHOD(TestEmptyFrame)
		{
			DataFrame frame;

			Assert::IsTrue(frame.isEmpty());
			Assert::AreEqual(frame.getHeight(), 0);
			Assert::AreEqual(frame.getWidth(), 0);
			Assert::AreEqual(frame.getItem(0, 0), string(""));
		}
	};
}