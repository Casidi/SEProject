#include "CppUnitTest.h"
#include "../SEProject/Database.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{		
	TEST_CLASS(TestDate) {
	public:
		TEST_METHOD(TestAddDays) {
			Date date(2016, 11, 25);
			date.addDays(6);
			Assert::AreEqual(2016, date.getYear());
			Assert::AreEqual(12, date.getMonth());
			Assert::AreEqual(1, date.getDay());

			Date date1(2016, 11, 25);
			date1.addDays(37);
			Assert::AreEqual(2017, date1.getYear());
			Assert::AreEqual(1, date1.getMonth());
			Assert::AreEqual(1, date1.getDay());

			Date date2(2016, 11, 25);
			date2.addDays(68);
			Assert::AreEqual(2017, date2.getYear());
			Assert::AreEqual(2, date2.getMonth());
			Assert::AreEqual(1, date2.getDay());

			Date date3(2016, 11, 25);
			date3.addDays(96);
			Assert::AreEqual(2017, date3.getYear());
			Assert::AreEqual(3, date3.getMonth());
			Assert::AreEqual(1, date3.getDay());
		}
		TEST_METHOD(TestSubDays) {
			Date date(2016, 12, 1);
			date.subDays(6);
			Assert::AreEqual(2016, date.getYear());
			Assert::AreEqual(11, date.getMonth());
			Assert::AreEqual(25, date.getDay());

			Date date1(2017, 1, 1);
			date1.subDays(37);
			Assert::AreEqual(2016, date1.getYear());
			Assert::AreEqual(11, date1.getMonth());
			Assert::AreEqual(25, date1.getDay());

			Date date2(2017, 2, 1);
			date2.subDays(68);
			Assert::AreEqual(2016, date2.getYear());
			Assert::AreEqual(11, date2.getMonth());
			Assert::AreEqual(25, date2.getDay());
		}
		TEST_METHOD(TestDiffDays) {
			Date first(2016, 11, 25);
			Date second(2016, 12, 1);
			Assert::AreEqual(-6, first.diffDays(second));
			Assert::AreEqual(6, second.diffDays(first));

			Date first1(2017, 1, 1);
			Date second1(2016, 11, 25);
			Assert::AreEqual(37, first1.diffDays(second1));
			Assert::AreEqual(-37, second1.diffDays(first1));

			Date first2(2017, 2, 1);
			Date second2(2016, 11, 25);
			Assert::AreEqual(68, first2.diffDays(second2));
			Assert::AreEqual(-68, second2.diffDays(first2));

			Date first3(2017, 3,1);
			Date second3(2016, 11, 25);
			Assert::AreEqual(96, first3.diffDays(second3));
			Assert::AreEqual(-96, second3.diffDays(first3));

			Date first4(2017, 12, 1);
			Date second4(2016, 11, 25);
			Assert::AreEqual(371, first4.diffDays(second4));
			Assert::AreEqual(-371, second4.diffDays(first4));

			Date first5(2017, 11, 26);
			Date second5(2016, 11, 25);
			Assert::AreEqual(366, first5.diffDays(second5));
			Assert::AreEqual(-366, second5.diffDays(first5));
		}
		TEST_METHOD(TestGetWeekDay) {
			Date date(2016, 11, 25);
			Assert::AreEqual(5, date.getWeekDay());
			Date date1(2016, 8, 2);
			Assert::AreEqual(2, date1.getWeekDay());
			Date date2(2017, 5, 9);
			Assert::AreEqual(2, date2.getWeekDay());
		}
	};

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