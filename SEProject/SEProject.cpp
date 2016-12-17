#include "resource.h"
#include "DialogProcs.h"
#include "Database.h"

using namespace std;

HINSTANCE hInst = NULL;
DataServer dataServer;
bool shouldExit = false;

#ifdef _DEBUG
int main()
#else
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdline, int iCmdShow)
#endif
{
	hInst = GetModuleHandle(NULL);

	if (dataServer.getIsConnected() == false) {
		MessageBoxA(NULL, "Failed to connect to server", "Error", MB_OK | MB_ICONINFORMATION);
		return 0;
	}

	//DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_APPLY), NULL, (DLGPROC)ApplyleaveDialogProc);
	//return 0;
	//DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_APPROVE), NULL, (DLGPROC)ApproveleaveDialogProc);
	//return 0;
	//DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_SETLEAVE), NULL, (DLGPROC)SetleaveDialogProc);
	//return 0;

	while (!shouldExit) {
		DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_LOGIN), NULL, (DLGPROC)LoginDialogProc);
		if (dataServer.getIsLogined() == false)
			return 0;
		if (shouldExit)
			break;

		DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_MAIN), NULL, (DLGPROC)MainDialogProc);
	}

    return 0;
}

