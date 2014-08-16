////////////////////////////////////////////////////////////////////////////
// WindowSelector.cpp -- window selector for Win32
// This file is part of MZC3.  See file "ReadMe.txt" and "License.txt".
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef MZC_NO_INLINING
    #undef MZC_INLINE
    #define MZC_INLINE  /*empty*/
    #include "WindowSelector_inl.hpp"
#endif

////////////////////////////////////////////////////////////////////////////

void MWindowSelector::AdjustIconWindow()
{
    // adjust m_hwndIcon to 32x32
    RECT rc;
    ::SetRect(&rc, 0, 0, 32, 32);
    ::AdjustWindowRectEx(&rc, ::GetWindowLong(m_hwndIcon, GWL_STYLE),
        FALSE, ::GetWindowLong(m_hwndIcon, GWL_EXSTYLE));
    ::SetWindowPos(m_hwndIcon, NULL, 0, 0,
        rc.right - rc.left, rc.bottom - rc.top,
        SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

/*virtual*/ MWindowSelector::~MWindowSelector()
{
    DestroyIcon(m_hicoNull);
    DestroyCursor(m_hcurAiming);
    DeleteObject(m_hPen);
}

void MWindowSelector::SetSelectedWindow(HWND hwndSelected)
{
    if (m_hwndSelected != hwndSelected)
    {
        m_hwndSelected = hwndSelected;

        ::SendMessage(m_hMainWnd, m_uNotifMsg, 0,
                      reinterpret_cast<LPARAM>(hwndSelected));
    }
}

void MWindowSelector::StartDragging()
{
    ::SetCapture(m_hwndIcon);
    SetIconNull();
    m_hwndOver = m_hwndOverOld = NULL;
    m_dwDelayCount = 0;
    ::SetCursor(GetAimingCursor());
}

#define mzcDELAY_COUNT_TO_HIDE      8
#define mzcDELAY_COUNT_TO_REDRAW    12

void MWindowSelector::DoDragging()
{
    if (::GetCapture() != m_hwndIcon)
        return;

    if (m_dwDelayCount < mzcDELAY_COUNT_TO_HIDE)    // delay hiding window
    {
        m_dwDelayCount++;
        if (m_dwDelayCount == mzcDELAY_COUNT_TO_HIDE)
        {
            ::GetWindowRect(m_hMainWnd, &m_rcWindowSaved);
            // move window to invisible area
            ::MoveWindow(m_hMainWnd, -32768, -32768, 0, 0, TRUE);
        }
        ::SetCursor(GetAimingCursor());
        return;
    }
    if (m_dwDelayCount < mzcDELAY_COUNT_TO_REDRAW)   // delay for redraw
    {
        m_dwDelayCount++;
        ::SetCursor(GetAimingCursor());
        return;
    }

    POINT pt;
    RECT rc;
    ::GetCursorPos(&pt);

    m_hwndOverOld = m_hwndOver;
    if (m_bAcceptChild)
    {
        m_hwndOver = ::WindowFromPoint(pt);
    }
    else
    {
        HWND hwnd, hwndParent;
        hwnd = ::WindowFromPoint(pt);
        for (;;)
        {
            hwndParent = ::GetParent(hwnd);
            if (hwndParent == NULL)
                break;
            hwnd = hwndParent;
        }
        m_hwndOver = hwnd;
    }

    if (m_hwndOverOld != m_hwndOver)
    {
        HWND hwndDesktop = ::GetDesktopWindow();
        HDC hdc = ::GetWindowDC(hwndDesktop);
        ::SetROP2(hdc, R2_XORPEN);
        SelectObject(hdc, m_hPen);
        SelectObject(hdc, ::GetStockObject(NULL_BRUSH));
        // erase previous box
        if (m_hwndOverOld)
        {
            ::GetWindowRect(m_hwndOverOld, &rc);
            ::InflateRect(&rc, -2, -2);
            ::Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
        }
        // draw box
        if (m_hwndOver)
        {
            ::GetWindowRect(m_hwndOver, &rc);
            ::InflateRect(&rc, -2, -2);
            ::Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
        }
        ::ReleaseDC(hwndDesktop, hdc);
    }
    ::SetCursor(GetAimingCursor());
}

void MWindowSelector::CancelDragging()
{
    if (m_hwndOverOld)
    {
        // erase previous box
        HWND hwndDesktop = ::GetDesktopWindow();
        HDC hdc = ::GetWindowDC(hwndDesktop);
        SelectObject(hdc, m_hPen);
        SelectObject(hdc, ::GetStockObject(NULL_BRUSH));
        {
            RECT rc;
            ::GetWindowRect(m_hwndOverOld, &rc);
            ::InflateRect(&rc, -2, -2);
            ::SetROP2(hdc, R2_XORPEN);
            ::Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
        }
        ::ReleaseDC(hwndDesktop, hdc);
        m_hwndOverOld = NULL;
    }

    RestoreMainWnd();
    SetIconDraggable();
}

void MWindowSelector::FinishDragging()
{
    ::ReleaseCapture();   // dragging ends
    if (m_hwndOver)
    {
        SetSelectedWindow(m_hwndOver);
        m_hwndOver = NULL;
    }
    RestoreMainWnd();
    SetIconDraggable();
}

/*static*/ LRESULT CALLBACK
MWindowSelector::IconWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MWindowSelector *self =
        reinterpret_cast<MWindowSelector *>(
            GetWindowLongPtr(hWnd, GWLP_USERDATA));

    assert(self);
    switch(uMsg)
    {
    case WM_LBUTTONDOWN:
        self->StartDragging();
        break;

    case WM_MOUSEMOVE:
        self->DoDragging();
        break;

    case WM_LBUTTONUP:
        self->FinishDragging();
        break;

    case WM_CAPTURECHANGED:
        self->CancelDragging();
        break;

    default:
        return ::CallWindowProc(self->m_fnOldWndProc,
                                hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////
