#include "gtest/gtest.h"
#include "../SEProject/Database.h"

TEST(TestDataFrame, testEmptyFrame) {
	DataFrame frame;
	EXPECT_TRUE(frame.isEmpty());
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	getchar();
}