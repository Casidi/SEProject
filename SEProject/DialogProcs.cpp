﻿#include "DialogProcs.h"
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

	MoveWindow(target, (screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2, 
		windowWidth, windowHeight, TRUE);
}

Date getToday() {
	SYSTEMTIME currentTime;
	GetLocalTime(&currentTime);
	return Date(currentTime.wYear, currentTime.wMonth, currentTime.wDay);
}

void AccountDialog_updateListViewFromData(HWND hwndLVTarget, const vector<Staff>& dataToSet) {
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

void AccountDialog_updateListViewFromServer(HWND hwndLVTarget) {
	vector<Staff> temp;
	temp = dataServer.getAllStaffExceptCurrentUser();
	AccountDialog_updateListViewFromData(hwndLVTarget, temp);
}

void AccountDialog_initListView(HWND hwndLVTarget) {
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

	AccountDialog_updateListViewFromServer(hwndLVTarget);
}

void AccountDialog_handleAddStaff(HWND hwndDialog) {
	char textBuffer[32];
	string staffID, staffPosition;

	Edit_GetText(GetDlgItem(hwndDialog, IDC_EDIT1), textBuffer, 32);
	staffID = string(textBuffer);
	ComboBox_GetText(GetDlgItem(hwndDialog, IDC_COMBO1), textBuffer, 32);
	staffPosition = string(textBuffer);

	dataServer.addStaff(staffID, DataServer::defaultStaffPassword, DataServer::defaultStaffName,
		staffPosition);
	AccountDialog_updateListViewFromServer(GetDlgItem(hwndDialog, IDC_LIST1));
}

void AccountDialog_handleDeleteStaff(HWND hwndDialog) {
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
	AccountDialog_updateListViewFromServer(hwndLV);
}

void AuthorityDialog_handleSetAuthority(HWND hwndDialog) {
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

	bool isSuccessful = true;
	int selectedIndex = ListView_GetNextItem(hwndLV, -1, LVNI_SELECTED);
	if (selectedIndex != -1) {
		while (selectedIndex != -1) {
			lvi.iItem = selectedIndex;
			ListView_GetItem(hwndLV, &lvi);
			isSuccessful &= dataServer.setStaffAuthority(string(lvi.pszText), staffPosition);
			selectedIndex = ListView_GetNextItem(hwndLV, selectedIndex, LVNI_SELECTED);
		}
		if (isSuccessful) 
			MessageBox(NULL, "Success: set authority success.", "Success", MB_OK);
		else 
			MessageBox(NULL, "Error: set authority failed.", "Error", MB_OK);
	}
	AccountDialog_updateListViewFromServer(hwndLV);
}

void SetpwDialog_handleSetName(HWND hwndDialog) {
	char textBuffer[32];
	string staffName;

	Edit_GetText(GetDlgItem(hwndDialog, IDC_EDIT1), textBuffer, 32);
	staffName = string(textBuffer);
	if(dataServer.setCurrentUserName(staffName)) 
		MessageBoxA(NULL, "Success: set username success.", "Success", MB_OK);
	else 
		MessageBoxA(NULL, "Error: fail to set user name.", "Error",MB_OK);
}

void SetpwDialog_handleSetPassword(HWND hwndDialog) {
	char textBuffer[32];
	string staffID, staffPassword;

	Edit_GetText(GetDlgItem(hwndDialog, IDC_EDIT2), textBuffer, 32);
	staffPassword = string(textBuffer);
	if(dataServer.setCurrentUserPassword(staffPassword)) 
		MessageBoxA(NULL, "Success: set password success.", "Success", MB_OK);
	else 
		MessageBoxA(NULL, "Error: fail to set password.", "Error",MB_OK);
}

void BrowseWeekDialog_initComboBox(HWND hwndDialog) {
	HWND hwndCombo = GetDlgItem(hwndDialog, IDC_COMBO1);
	Date today = getToday();
	vector<string> weekDays = today.getAllDatesInThisWeekAsStrings();
	for (int i = 0; i < weekDays.size(); ++i)
		ComboBox_AddString(hwndCombo, weekDays[i].c_str());
}

void BrowseWeekDialog_initListView(HWND hwndDialog) {
	LVCOLUMN lvc;
	HWND hwndLVTarget = GetDlgItem(hwndDialog, IDC_LIST3);
	SendMessage(hwndLVTarget, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	ZeroMemory(&lvc, sizeof(LVCOLUMN));
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = TEXT("ID");
	ListView_InsertColumn(hwndLVTarget, 0, &lvc);

	lvc.cx = 100;
	lvc.pszText = TEXT("Status");
	ListView_InsertColumn(hwndLVTarget, 1, &lvc);

	lvc.pszText = TEXT("Reason");
	ListView_InsertColumn(hwndLVTarget, 2, &lvc);
}

void BrowseWeekDialog_handleDateSelected(HWND hwndDialog) {
	char buffer[16];
	ComboBox_GetLBText(GetDlgItem(hwndDialog, IDC_COMBO1),
		ComboBox_GetCurSel(GetDlgItem(hwndDialog, IDC_COMBO1)), buffer);
	vector<Schedule> scheduleThisDay = dataServer.getDaySchedule(Date(buffer));

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.cchTextMax = 256;

	HWND hwndLVTarget = GetDlgItem(hwndDialog, IDC_LIST3);
	ListView_DeleteAllItems(hwndLVTarget);
	for (int i = 0; i < scheduleThisDay.size(); ++i) {
		lvi.iItem = 0;
		lvi.pszText = (LPSTR)scheduleThisDay[i].staffID.c_str();
		ListView_InsertItem(hwndLVTarget, &lvi);
		ListView_SetItemText(hwndLVTarget, 0, 1, (LPSTR)scheduleThisDay[i].status.c_str());
		ListView_SetItemText(hwndLVTarget, 0, 2, (LPSTR)scheduleThisDay[i].reason.c_str());
	}
}

void BrowseDayDialog_initListView(HWND hwndDialog) {
	LVCOLUMN lvc;
	HWND hwndLVTarget = GetDlgItem(hwndDialog, IDC_LIST1);
	SendMessage(hwndLVTarget, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	ZeroMemory(&lvc, sizeof(LVCOLUMN));
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = TEXT("ID");
	ListView_InsertColumn(hwndLVTarget, 0, &lvc);

	lvc.cx = 100;
	lvc.pszText = TEXT("Status");
	ListView_InsertColumn(hwndLVTarget, 1, &lvc);

	lvc.pszText = TEXT("Reason");
	ListView_InsertColumn(hwndLVTarget, 2, &lvc);

	vector<Schedule> scheduleThisDay = dataServer.getDaySchedule(getToday());

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.cchTextMax = 256;

	ListView_DeleteAllItems(hwndLVTarget);
	for (int i = 0; i < scheduleThisDay.size(); ++i) {
		lvi.iItem = 0;
		lvi.pszText = (LPSTR)scheduleThisDay[i].staffID.c_str();
		ListView_InsertItem(hwndLVTarget, &lvi);
		ListView_SetItemText(hwndLVTarget, 0, 1, (LPSTR)scheduleThisDay[i].status.c_str());
		ListView_SetItemText(hwndLVTarget, 0, 2, (LPSTR)scheduleThisDay[i].reason.c_str());
	}
}

void BrowseDayDialog_initStaticText(HWND hwndDialog) {
	char buffer[32];
	sprintf_s(buffer, "Today is: %s", getToday().toString().c_str());
	SetDlgItemText(hwndDialog, IDC_STATIC1, buffer);
}

void ApplyleaveDialog_handleApply(HWND hwndDialog)
{
	char textBuffer[32];
	string leaveStatus, leaveReason;

	Edit_GetText(GetDlgItem(hwndDialog, IDC_EDIT1), textBuffer, 32);
	leaveReason = string(textBuffer);
	ComboBox_GetText(GetDlgItem(hwndDialog, IDC_COMBO1), textBuffer, 32);
	leaveStatus = string(textBuffer);

	//month calendar
	HWND hwndMC = GetDlgItem(hwndDialog, IDC_MONTHCALENDAR1);
	SYSTEMTIME selday;
	LPSYSTEMTIME lpSelday = &selday;
	MonthCal_GetCurSel(hwndMC, lpSelday);
	cout << "select date: " << selday.wYear << " " << selday.wMonth << " " << selday.wDay << endl;
	Date seldate((int)lpSelday->wYear, (int)lpSelday->wMonth, (int)lpSelday->wDay);
	SYSTEMTIME today;
	LPSYSTEMTIME lpToday = &today;
	MonthCal_GetToday(hwndMC, lpToday);
	cout << "today: " << today.wYear << " " << today.wMonth << " " << today.wDay << endl;
	Date todate((int)lpToday->wYear, (int)lpToday->wMonth, (int)lpToday->wDay);

	if(dataServer.applyLeave(leaveStatus, leaveReason, seldate, todate)) 
		MessageBoxA(NULL, "Success: application success.", "Success", MB_OK);
	else 
		MessageBoxA(NULL, "Error: application failed.", "Error",MB_OK);
}

void ApproveleaveDialog_updateListViewFromData(HWND hwndLVTarget, const vector<Leave>& dataToSet)
{
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT;
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.cchTextMax = 256;

	ListView_DeleteAllItems(hwndLVTarget);
	for (int i = 0; i < dataToSet.size(); ++i) {
		lvi.iItem = 0;
		lvi.pszText = (LPSTR)dataToSet[i].date.c_str();
		ListView_InsertItem(hwndLVTarget, &lvi);
		ListView_SetItemText(hwndLVTarget, 0, 1, (LPSTR)dataToSet[i].staffID.c_str());
		ListView_SetItemText(hwndLVTarget, 0, 2, (LPSTR)dataToSet[i].name.c_str());
		ListView_SetItemText(hwndLVTarget, 0, 3, (LPSTR)dataToSet[i].status.c_str());
		ListView_SetItemText(hwndLVTarget, 0, 4, (LPSTR)dataToSet[i].reason.c_str());
	}
}

void ApproveleaveDialog_updateListViewFromServer(HWND hwndLVTarget)
{
	vector<Leave> temp;
	temp = dataServer.getAllLeave();
	ApproveleaveDialog_updateListViewFromData(hwndLVTarget, temp);
}

void ApproveleaveDialog_initListView(HWND hwndLVTarget)
{
	LVCOLUMN lvc;

	SendMessage(hwndLVTarget, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	ZeroMemory(&lvc, sizeof(LVCOLUMN));
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = TEXT("Date");
	ListView_InsertColumn(hwndLVTarget, 0, &lvc);

	lvc.cx = 100;
	lvc.pszText = TEXT("ID");
	ListView_InsertColumn(hwndLVTarget, 1, &lvc);

	lvc.cx = 100;
	lvc.pszText = TEXT("Name");
	ListView_InsertColumn(hwndLVTarget, 2, &lvc);

	lvc.cx = 100;
	lvc.pszText = TEXT("Status");
	ListView_InsertColumn(hwndLVTarget, 3, &lvc);

	lvc.pszText = TEXT("Reason");
	ListView_InsertColumn(hwndLVTarget, 4, &lvc);

	ApproveleaveDialog_updateListViewFromServer(hwndLVTarget);
}

void ApproveleaveDialog_handleApprove(HWND hwndDialog)
{
	HWND hwndLV = GetDlgItem(hwndDialog, IDC_LIST1);
	LVITEM lvi;
	char textBuffer[32];
	char textID[32];
	char textStatus[32];
	char textReason[32];
	LPTSTR pID = textID;
	LPTSTR pStatus = textStatus;
	LPTSTR pReason = textReason;

	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = textBuffer;
	lvi.cchTextMax = 32;

	bool isSuccessful = true;
	int selectedIndex = ListView_GetNextItem(hwndLV, -1, LVNI_SELECTED);
	if (selectedIndex != -1) {
		while (selectedIndex != -1) {
			lvi.iItem = selectedIndex;
			ListView_GetItem(hwndLV, &lvi);
			ListView_GetItemText(hwndLV, lvi.iItem, 1, pID, 32); //get staffID
			ListView_GetItemText(hwndLV, lvi.iItem, 3, pStatus, 32); //get status
			ListView_GetItemText(hwndLV, lvi.iItem, 4, pReason, 32); //get reason
			isSuccessful &= dataServer.approveLeave(string(lvi.pszText), string(pID), string(pStatus), string(pReason));
			ListView_DeleteItem(hwndLV, selectedIndex);
			--selectedIndex;
			selectedIndex = ListView_GetNextItem(hwndLV, selectedIndex, LVNI_SELECTED);
		}
		if(!isSuccessful) 
			MessageBoxA(NULL, "Error: approval failed.", "Error",MB_OK);
	}
	ApproveleaveDialog_updateListViewFromServer(hwndLV);
}

void SetleaveDialog_updateListViewFromData(HWND hwndLVTarget, const vector<Staff>& dataToSet)
{
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
	}
}

void SetleaveDialog_updateListViewFromServer(HWND hwndLVTarget)
{
	vector<Staff> temp;
	temp = dataServer.getAllStaff();
	SetleaveDialog_updateListViewFromData(hwndLVTarget, temp);
}

void SetleaveDialog_initListView(HWND hwndLVTarget)
{
	LVCOLUMN lvc;

	SendMessage(hwndLVTarget, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	ZeroMemory(&lvc, sizeof(LVCOLUMN));
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = TEXT("ID");
	ListView_InsertColumn(hwndLVTarget, 0, &lvc);

	lvc.pszText = TEXT("Name");
	ListView_InsertColumn(hwndLVTarget, 1, &lvc);

	SetleaveDialog_updateListViewFromServer(hwndLVTarget);
}

void SetleaveDialog_handleSetLeave(HWND hwndDialog)
{
	char reasonBuffer[32];
	string reason;

	Edit_GetText(GetDlgItem(hwndDialog, IDC_EDIT1), reasonBuffer, 32);
	reason = string(reasonBuffer);

	//month calendar
	HWND hwndMC = GetDlgItem(hwndDialog, IDC_MONTHCALENDAR1);
	SYSTEMTIME selday;
	LPSYSTEMTIME lpSelday = &selday;
	MonthCal_GetCurSel(hwndMC, lpSelday);
	cout << "select date: " << selday.wYear << " " << selday.wMonth << " " << selday.wDay << endl;
	Date seldate((int)lpSelday->wYear, (int)lpSelday->wMonth, (int)lpSelday->wDay);

	//list view
	HWND hwndLV = GetDlgItem(hwndDialog, IDC_LIST1);
	LVITEM lvi;
	char textBuffer[32];

	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = textBuffer;
	lvi.cchTextMax = 32;

	bool isSuccessful = true;
	int selectedIndex = ListView_GetNextItem(hwndLV, -1, LVNI_SELECTED);
	if (selectedIndex != -1) {
		while (selectedIndex != -1) {
			lvi.iItem = selectedIndex;
			ListView_GetItem(hwndLV, &lvi);
			isSuccessful &= dataServer.setLeave(string(lvi.pszText), seldate, reason);
			selectedIndex = ListView_GetNextItem(hwndLV, selectedIndex, LVNI_SELECTED);
		}
		if(isSuccessful) 
			MessageBoxA(NULL, "Success: set business with reason.", "Success", MB_OK);
		else 
			MessageBoxA(NULL, "Error: set business failed.", "Error",MB_OK);
	}
	SetleaveDialog_updateListViewFromServer(hwndLV);
}

LRESULT CALLBACK AccountDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		moveToCenter(hwnd);
		AccountDialog_initListView(GetDlgItem(hwnd, IDC_LIST1));
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), "chief");
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), "supervisor");
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), "labor");
		ComboBox_SelectString(GetDlgItem(hwnd, IDC_COMBO1), -1, "labor");
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_ADDSTAFF:
			AccountDialog_handleAddStaff(hwnd);
			return TRUE;
		case IDC_DELETESTAFF:
			AccountDialog_handleDeleteStaff(hwnd);
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
		AccountDialog_initListView(GetDlgItem(hwnd, IDC_LIST1));
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
			AuthorityDialog_handleSetAuthority(hwnd);
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
			SetpwDialog_handleSetName(hwnd);
			return TRUE;

		case IDC_SETPW:
			SetpwDialog_handleSetPassword(hwnd);
			return TRUE;

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
		if (dataServer.getCurrentUser().authority == "supervisor") {
			ShowWindow(GetDlgItem(hwnd, IDC_BUTTON1), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, IDC_BUTTON2), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, IDC_BUTTON7), SW_HIDE);
		}
		else if (dataServer.getCurrentUser().authority == "labor") {
			ShowWindow(GetDlgItem(hwnd, IDC_BUTTON1), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, IDC_BUTTON2), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, IDC_BUTTON4), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, IDC_BUTTON6), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, IDC_BUTTON7), SW_HIDE);
		}
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
			DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_BROWSEWEEK), hwnd, (DLGPROC)BrowseWeekDialogProc);
			break;
		case IDC_BUTTON5:
			DialogBoxA(hInst, MAKEINTRESOURCEA(IDD_BROWSEDAY), hwnd, (DLGPROC)BrowseDayDialogProc);
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
		case IDCANCEL2:
			EndDialog(hwnd, 0);
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			shouldExit = true;
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
		SetFocus(GetDlgItem(hwnd, IDC_USERNAME));
		return FALSE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			GetDlgItemTextA(hwnd, IDC_USERNAME, loginUserName, 20);
			GetDlgItemTextA(hwnd, IDC_PASSWORD, loginPassword, 20);
			if (dataServer.login(loginUserName, loginPassword))
				EndDialog(hwnd, 0);
			else
				MessageBoxA(NULL, "Failed to login", "Error", MB_OK | MB_ICONINFORMATION);

			return TRUE;

		case IDCANCEL:
			shouldExit = true;
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK BrowseWeekDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		moveToCenter(hwnd);
		BrowseWeekDialog_initComboBox(hwnd);
		BrowseWeekDialog_initListView(hwnd);
		return true;

	case WM_COMMAND:
		if ((HWND)lParam == GetDlgItem(hwnd, IDC_COMBO1)) {
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				cout << "combo clicked" << endl;
				BrowseWeekDialog_handleDateSelected(hwnd);
				return TRUE;
			}
		}

		switch (wParam)
		{
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK BrowseDayDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		moveToCenter(hwnd);
		BrowseDayDialog_initListView(hwnd);
		BrowseDayDialog_initStaticText(hwnd);
		return true;

	case WM_COMMAND:
		switch (wParam)
		{
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
		moveToCenter(hwnd);
		ApproveleaveDialog_initListView(GetDlgItem(hwnd, IDC_LIST1));
		return true;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			ApproveleaveDialog_handleApprove(hwnd);
			return TRUE;
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
		moveToCenter(hwnd);
		SetleaveDialog_initListView(GetDlgItem(hwnd, IDC_LIST1));
		return true;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			SetleaveDialog_handleSetLeave(hwnd);
			return TRUE;
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
		moveToCenter(hwnd);
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), "leave");
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), "sick");
		ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO1), "compensatory");
		ComboBox_SelectString(GetDlgItem(hwnd, IDC_COMBO1), -1, "compensatory");
		return true;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			ApplyleaveDialog_handleApply(hwnd);
			return TRUE;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

