#pragma once
#include "resource.h"
#include "Database.h"

using namespace std;

extern HINSTANCE hInst;
extern DataServer dataServer;

void moveToCenter(HWND target);
LRESULT CALLBACK AccountDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AuthorityDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LoginDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);