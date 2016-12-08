#include "resource.h"
#include "DialogProcs.h"
#include "Database.h"

using namespace std;

HINSTANCE hInst = NULL;
DataServer dataServer;

#ifdef _DEBUG
int main()
#else
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdline, int iCmdShow)
#endif
{
	hInst = GetModuleHandle(NULL);

	//dataServer.resetDatabase();
	//return 0;
	//DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_BROWSEDAY), NULL, (DLGPROC)BrowseDayDialogProc);
	//return 0;
	
	if (dataServer.getIsConnected() == false) {
		MessageBoxA(NULL, "Failed to connect to server", "Error", MB_OK | MB_ICONINFORMATION);
		return 0;
	}

	DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_LOGIN), NULL, (DLGPROC)LoginDialogProc);
	if (dataServer.getIsLogined() == false)
		return 0;

	DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_MAIN), NULL, (DLGPROC)MainDialogProc);

    return 0;
}

