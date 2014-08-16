////////////////////////////////////////////////////////////////////////////
// SelectWnd.cpp -- sample of window selector for Win32
// This file is part of MZC3.  See file "ReadMe.txt" and "License.txt".
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

HINSTANCE g_hInstance;

////////////////////////////////////////////////////////////////////////////

INT_PTR CALLBACK
DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hwndSelected;
    static MWindowSelector *s_pSelector = NULL;

    switch(uMsg)
    {
    case WM_INITDIALOG:
        {
            HWND hMainWnd = hDlg;
            HWND hIconWnd = ::GetDlgItem(hDlg, stc1);
            assert(hMainWnd);
            assert(hIconWnd);
            s_pSelector = new MWindowSelector(hMainWnd, hIconWnd);
            assert(s_pSelector);
            s_pSelector->AcceptChildWindow(true);
            EnableWindow(GetDlgItem(hMainWnd, IDOK), FALSE);
        }
        return TRUE;

    case MZC3_WM_WINDOWSELECT:
        hwndSelected = reinterpret_cast<HWND>(lParam);
        if (hwndSelected)
        {
            ::EnableWindow(::GetDlgItem(hDlg, IDOK), TRUE);
            {
                // show window info
                TCHAR szClsName[20], szText[32], szInfo[20 + 32 + 64];
                ::GetClassName(hwndSelected, szClsName, 20);
                ::GetWindowText(hwndSelected, szText, 32);
                #ifdef _WIN64
                    DWORDLONG dwl;
                    DWORD dwHi, dwLo;
                    dwl = reinterpret_cast<DWORDLONG>(hwndSelected);
                    dwHi = static_cast<DWORD>(dwl >> 32);
                    dwLo = static_cast<DWORD>(dwl);
                    ::wsprintf(szInfo, TEXT("[%s]\r\n%s\r\nhWnd: %08X%08X"),
                        szClsName, szText, dwHi, dwLo);
                #else
                    ::wsprintf(szInfo, TEXT("[%s]\r\n%s\r\nhWnd: %08X"),
                        szClsName, szText,
                        reinterpret_cast<DWORD>(hwndSelected));
                #endif
                ::SetDlgItemText(hDlg, stc2, szInfo);
            }
            ::SetDlgItemText(hDlg, stc3, NULL);
            ::SetFocus(::GetDlgItem(hDlg, IDOK));
        }
        else
        {
            ::EnableWindow(::GetDlgItem(hDlg, IDOK), FALSE);
            ::SetDlgItemText(hDlg, stc2, NULL);
        }
        break;

    case WM_SYSCOMMAND:
        if (wParam == SC_CLOSE)
        {
            delete s_pSelector;
            s_pSelector = NULL;
            ::EndDialog(hDlg, IDCANCEL);
            break;
        }
        break;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            delete s_pSelector;
            s_pSelector = NULL;
            ::EndDialog(hDlg, (INT) LOWORD(wParam));
            break;

        case psh1:
            delete s_pSelector;
            s_pSelector = NULL;
            ::EndDialog(hDlg, IDCANCEL);
            break;
        }
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////

INT WINAPI WinMain(
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       pszCmdLine,
    INT         nCmdShow)
{
    g_hInstance = hInstance;
    DialogBox(hInstance, MAKEINTRESOURCE(1), NULL, DialogProc);
    return 0;
}

////////////////////////////////////////////////////////////////////////////
