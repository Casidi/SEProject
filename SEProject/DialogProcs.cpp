#include "DialogProcs.h"
#include <windowsx.h>
#include <iostream>

using namespace std;

void moveToCenter(HWND target) {
	RECT windowRect;
	int screenWidth, screenHeight;
	int windowWidth, windowHeight;

	GetWindowRect(target, &windowRect);
	windowWidth = windowRect.right - windowRect.left;
	windowHeight = windowRect.bottom - windowRect.top;

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	MoveWindow(target, (screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2, windowWidth, windowHeight, TRUE);
}

LRESULT CALLBACK AccountDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		moveToCenter(hwnd);
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), L"Add");
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), L"Delete");
		return true;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK AuthorityDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		moveToCenter(hwnd);
		return true;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK MainDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		moveToCenter(hwnd);
		return true;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_BUTTON1:
			cout << "add/delete account button clicked" << endl;
			DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_ACCOUNT), hwnd, (DLGPROC)AccountDialogProc);
			break;
		case IDC_BUTTON2:
			cout << "modify authority button clicked" << endl;
			DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_AUTHORITY), hwnd, (DLGPROC)AuthorityDialogProc);
			break;
		case IDC_BUTTON4:
			cout << "browse week" << endl;
			break;
		case IDC_BUTTON5:
			cout << "browse day" << endl;
			break;
		case IDC_BUTTON6:
			cout << "approve leaves" << endl;
			break;
		case IDC_BUTTON7:
			cout << "set to leave" << endl;
			break;
		case IDC_BUTTON8:
			cout << "apply for leaves" << endl;
			break;
		case IDC_BUTTON9:
			cout << "set password/name" << endl;
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK LoginDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char loginUserName[20];
	char loginPassword[20];

	switch (message) {
	case WM_INITDIALOG:
		moveToCenter(hwnd);
		return true;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			GetDlgItemTextA(hwnd, IDC_USERNAME, loginUserName, 20);
			GetDlgItemTextA(hwnd, IDC_PASSWORD, loginPassword, 20);
			if (dataServer.login(loginUserName, loginPassword))
				EndDialog(hwnd, 0);

			return TRUE;

		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
