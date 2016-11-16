#include "gtest/gtest.h"
#include "../SEProject/Database.h"
#include "../SEProject/resource.h"
#include "../SEProject/DialogProcs.h"

TEST(testDataFrame, testEmptyFrame) {
	DataFrame frame;
	EXPECT_TRUE(frame.isEmpty());
}

//for testing the GUI, should only be called when designing the dialogs
HINSTANCE hInst;
DataServer dataServer;
void testAllDialogGUI() {
	//DialogBox(NULL, MAKEINTRESOURCE(IDD_LOGIN), NULL, (DLGPROC)LoginDialogProc);
	//DialogBox(NULL, MAKEINTRESOURCE(IDD_ACCOUNT), NULL, (DLGPROC)AccountDialogProc);
	//DialogBox(NULL, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC)MainDialogProc);
}

int main(int argc, char* argv[])
{
	//測試dialog專用，平時請保持註解
	//testAllDialogGUI();

	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	getchar();
	return 0;
}