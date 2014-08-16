////////////////////////////////////////////////////////////////////////////
// WindowSelector.hpp -- window selector for Win32
// This file is part of MZC3.  See file "ReadMe.txt" and "License.txt".
////////////////////////////////////////////////////////////////////////////

#ifndef __MZC3_WINDOWSELECTOR__
#define __MZC3_WINDOWSELECTOR__

#ifndef MZC3_IDI_NULL           // null icon
    #define MZC3_IDI_NULL 1923
#endif

#ifndef MZC3_IDC_AIMING         // aiming cursor
    #define MZC3_IDC_AIMING 1263
#endif

#ifndef MZC3_WM_WINDOWSELECT    // message on window selection
    #define MZC3_WM_WINDOWSELECT (WM_USER + 1000)
#endif

////////////////////////////////////////////////////////////////////////////

struct MWindowSelector EXTENDS_MOBJECT
{
    HWND    m_hMainWnd;         // main window
    HWND    m_hwndIcon;         // icon window to drag
    WNDPROC m_fnOldWndProc;     // old window procedure
    HICON   m_hicoNull;         // null icon
    HCURSOR m_hcurAiming;       // aiming cursor
    HPEN    m_hPen;             // pen of line width 3
    HWND    m_hwndSelected;     // selected window
    bool    m_bAcceptChild;     // accept child?
    UINT    m_uNotifMsg;        // sended to main window

    MWindowSelector(
        HWND hMainWnd, HWND hwndIcon, HWND hwndSelected = NULL,
        UINT uNotifMsg = MZC3_WM_WINDOWSELECT,
        HINSTANCE hInstance = ::GetModuleHandle(NULL)) :

        m_hMainWnd(hMainWnd), m_hwndIcon(hwndIcon),
        m_fnOldWndProc(
            reinterpret_cast<WNDPROC>(
                SetWindowLongPtr(hwndIcon, GWLP_WNDPROC,
                    reinterpret_cast<LONG_PTR>(
                        MWindowSelector::IconWndProc)))),
        m_hicoNull(::LoadIcon(hInstance,
                              MAKEINTRESOURCE(MZC3_IDI_NULL))),
        m_hcurAiming(::LoadCursor(hInstance,
                                  MAKEINTRESOURCE(MZC3_IDC_AIMING))),
        m_hPen(::CreatePen(PS_SOLID, 3, RGB(255, 255, 255))),
        m_hwndSelected(NULL), m_bAcceptChild(false),
        m_uNotifMsg(uNotifMsg)
    {
        assert(m_hMainWnd);
        assert(::IsWindow(m_hMainWnd));
        assert(m_hwndIcon);
        assert(::IsWindow(m_hwndIcon));
        assert(m_fnOldWndProc);
        assert(m_hicoNull);
        assert(m_hcurAiming);
        assert(m_hPen);
        if (hwndSelected)
            SetSelectedWindow(hwndSelected);

        // set user data
        SetWindowLongPtr(hwndIcon, GWLP_USERDATA,
                         reinterpret_cast<LONG_PTR>(this));

        // adjust hwndIcon size
        AdjustIconWindow();

        // set image
        SetIconDraggable();

        #ifndef NDEBUG
            TCHAR szClass[64];
            ::GetClassName(hwndIcon, szClass, 64);
            assert(::lstrcmpi(szClass, TEXT("STATIC")) == 0);
            const DWORD style = (SS_ICON | SS_NOTIFY);
            assert((::GetWindowLong(hwndIcon, GWL_STYLE) & style) == style);
        #endif
    }

    virtual ~MWindowSelector();

    bool    DoesAcceptChildWindow() const;
    void    AcceptChildWindow(bool accept = true);

    HWND    GetMainWnd() const;
    HWND    GetIconWindow() const;
    HWND    GetSelectedWindow() const;
    HICON   GetNullIcon() const;
    HCURSOR GetAimingCursor() const;

    void SetSelectedWindow(HWND hwndSelected);
    void StartDragging();
    void DoDragging();
    void CancelDragging();
    void FinishDragging();

    void SetIconNull();
    void SetIconDraggable();
    UINT GetNotifMsg() const;

protected:
    DWORD   m_dwDelayCount;
    RECT    m_rcWindowSaved;
    HWND    m_hwndOver, m_hwndOverOld;

    void    AdjustIconWindow();
    void    RestoreMainWnd();

    static LRESULT CALLBACK IconWndProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    // NOTE: MWindowSelector is not copyable.
    MWindowSelector();
    MWindowSelector(const MWindowSelector&);
    MWindowSelector& operator=(const MWindowSelector&);
};

////////////////////////////////////////////////////////////////////////////

#ifndef MZC_NO_INLINING
    #undef MZC_INLINE
    #define MZC_INLINE inline
    #include "WindowSelector_inl.hpp"
#endif

#endif  // ndef __MZC3_WINDOWSELECTOR__
