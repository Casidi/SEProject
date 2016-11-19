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

	TEST_CLASS(TestDataServer) {
	public:
		TEST_METHOD(TestAddStaff) {
			DataServer server;
			server.addStaff("BBB11");
			DataFrame result = server.makeQuery("SELECT * FROM staff WHERE id='BBB11';");
			if (result.isEmpty()) {
				Assert::Fail(L"add user failed");
			}
		}

		TEST_METHOD(TestDeleteStaff) {
			DataServer server;
			server.deleteStaff("BBB11");
			DataFrame result = server.makeQuery("SELECT * FROM staff WHERE id='BBB11';");
			if (result.isEmpty() == false) {
				Assert::Fail(L"delete user failed");
			}
		}

		/*TEST_METHOD(BuildDatabase) {
			DataServer server;
			char buffer[32];

			sprintf_s(buffer, 32, "A%03d", 4);
			server.addStaff(buffer);

			for (int i = 5; i <= 23; ++i) {
				sprintf_s(buffer, 32, "A%03d", i);
				server.addStaff(buffer);
			}
		}*/
	};
}