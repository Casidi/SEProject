#include "DialogProcs.h"
#include <windowsx.h>
#include <CommCtrl.h>
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

void updateListViewFromData(HWND hwndLVTarget, const vector<Staff>& dataToSet) {
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.cchTextMax = 256;

	ListView_DeleteAllItems(hwndLVTarget);
	for (int i = 0; i < dataToSet.size(); ++i) {
		lvi.iItem = 0;
		lvi.pszText = (LPSTR)dataToSet[i].id.c_str();
		ListView_InsertItem(hwndLVTarget, &lvi);
		ListView_SetItemText(hwndLVTarget, 0, 1, (LPSTR)dataToSet[i].name.c_str());
		ListView_SetItemText(hwndLVTarget, 0, 2, (LPSTR)dataToSet[i].authority.c_str());
	}
}

void updateListViewFromServer(HWND hwndLVTarget) {
	vector<Staff> temp;
	temp = dataServer.getAllStaffExceptCurrentUser();
	updateListViewFromData(hwndLVTarget, temp);
}

void initListView(HWND hwndLVTarget) {
	LVCOLUMN lvc;

	SendMessage(hwndLVTarget, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	ZeroMemory(&lvc, sizeof(LVCOLUMN));
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = TEXT("ID");
	ListView_InsertColumn(hwndLVTarget, 0, &lvc);

	lvc.cx = 100;
	lvc.pszText = TEXT("Name");
	ListView_InsertColumn(hwndLVTarget, 1, &lvc);

	lvc.pszText = TEXT("Position");
	ListView_InsertColumn(hwndLVTarget, 2, &lvc);

	updateListViewFromServer(hwndLVTarget);
}

void handleAddStaff(HWND hwndDialog) {
	char textBuffer[32];
	string staffID, staffPosition;

	Edit_GetText(GetDlgItem(hwndDialog, IDC_EDIT1), textBuffer, 32);
	staffID = string(textBuffer);
	ComboBox_GetText(GetDlgItem(hwndDialog, IDC_COMBO1), textBuffer, 32);
	staffPosition = string(textBuffer);

	dataServer.addStaff(staffID, DataServer::defaultStaffPassword, DataServer::defaultStaffName, staffPosition);
	updateListViewFromServer(GetDlgItem(hwndDialog, IDC_LIST1));
}

void handleDeleteStaff(HWND hwndDialog) {
	HWND hwndLV = GetDlgItem(hwndDialog, IDC_LIST1);
	LVITEM lvi;
	char textBuffer[32];

	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = textBuffer;
	lvi.cchTextMax = 32;

	int selectedIndex = ListView_GetNextItem(hwndLV, -1, LVNI_SELECTED);
	while (selectedIndex != -1) {
		lvi.iItem = selectedIndex;
		ListView_GetItem(hwndLV, &lvi);
		dataServer.deleteStaff(string(lvi.pszText));
		ListView_DeleteItem(hwndLV, selectedIndex);
		--selectedIndex;
		selectedIndex = ListView_GetNextItem(hwndLV, selectedIndex, LVNI_SELECTED);
	}
	updateListViewFromServer(hwndLV);
}

//以下修改過
void handleSetAuthority(HWND hwndDialog) {
	HWND hwndLV = GetDlgItem(hwndDialog, IDC_LIST1);
	LVITEM lvi;
	char textBuffer[32];
	string staffPosition;

	ComboBox_GetText(GetDlgItem(hwndDialog, IDC_COMBO1), textBuffer, 32);
	staffPosition = string(textBuffer);

	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = textBuffer;
	lvi.cchTextMax = 32;

	int selectedIndex = ListView_GetNextItem(hwndLV, -1, LVNI_SELECTED);
	while (selectedIndex != -1) {
		lvi.iItem = selectedIndex;
		ListView_GetItem(hwndLV, &lvi);
		dataServer.setStaffAuthority(string(lvi.pszText), staffPosition); //ID??
		--selectedIndex;
		selectedIndex = ListView_GetNextItem(hwndLV, selectedIndex, LVNI_SELECTED);
	}
	updateListViewFromServer(hwndLV);
}

void handleSetName(HWND hwndDialog) {
	char textBuffer[32];
	string staffName;
	
	Edit_GetText(GetDlgItem(hwndDialog, IDC_EDIT1), textBuffer, 32);
	staffName = string(textBuffer);
	dataServer.setStaffName(dataServer.getcurrentUserID(), staffName);
}

void handleSetPassword(HWND hwndDialog) {
	char textBuffer[32];
	string staffID, staffPassword;

	Edit_GetText(GetDlgItem(hwndDialog, IDC_EDIT2), textBuffer, 32);
	staffPassword = string(textBuffer);
	dataServer.setStaffPassword(dataServer.getcurrentUserID(), staffPassword);
}

LRESULT CALLBACK AccountDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		moveToCenter(hwnd);
		initListView(GetDlgItem(hwnd, IDC_LIST1));	
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), "chief");
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), "supervisor");
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), "labor");
		ComboBox_SelectString(GetDlgItem(hwnd, IDC_COMBO1), -1, "labor");
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_ADDSTAFF:
			handleAddStaff(hwnd);
			return TRUE;
		case IDC_DELETESTAFF:
			handleDeleteStaff(hwnd);
			return TRUE;

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
		initListView(GetDlgItem(hwnd, IDC_LIST1));
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), "chief");
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), "supervisor");
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), "labor");
		ComboBox_SelectString(GetDlgItem(hwnd, IDC_COMBO1), -1, "labor");
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_SETAUTHORITY:
			cout << "set authority..." << endl;
			handleSetAuthority(hwnd);
			return TRUE;

		case IDOK:
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

LRESULT CALLBACK SetpwDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		moveToCenter(hwnd);
		return true;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_SETNAME:
			cout << "set name..." << endl;
			handleSetName(hwnd);
			return TRUE;
		case IDC_SETPW:
			cout << "set password..." << endl;
			handleSetPassword(hwnd);
			return TRUE;

		case IDOK:
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
//到這為止

//TODO: filter buttons according to the authority of current user
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
			DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_ACCOUNT), hwnd, (DLGPROC)AccountDialogProc);
			break;
		case IDC_BUTTON2:
			DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_AUTHORITY), hwnd, (DLGPROC)AuthorityDialogProc);
			break;
		case IDC_BUTTON4:
			DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_BROWSEWEEK), hwnd, (DLGPROC)BrowseweekDialogProc);
			break;
		case IDC_BUTTON5:
			DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_BROWSEDAY), hwnd, (DLGPROC)BrowsedayDialogProc);
			break;
		case IDC_BUTTON6:
			DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_APPROVE), hwnd, (DLGPROC)ApproveleaveDialogProc);
			break;
		case IDC_BUTTON7:
			DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_SETLEAVE), hwnd, (DLGPROC)SetleaveDialogProc);
			break;
		case IDC_BUTTON8:
			DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_APPLY), hwnd, (DLGPROC)ApplyleaveDialogProc);
			break;
		case IDC_BUTTON9:
			DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_SETPW), hwnd, (DLGPROC)SetpwDialogProc);
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

LRESULT CALLBACK BrowseweekDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
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

LRESULT CALLBACK BrowsedayDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
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

LRESULT CALLBACK ApproveleaveDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
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

LRESULT CALLBACK SetleaveDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
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

LRESULT CALLBACK ApplyleaveDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
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

LRESULT CALLBACK SetpwDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
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
