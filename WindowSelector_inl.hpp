////////////////////////////////////////////////////////////////////////////
// WindowSelector_inl.hpp -- window selector for Win32
// This file is part of MZC3.  See file "ReadMe.txt" and "License.txt".
////////////////////////////////////////////////////////////////////////////

MZC_INLINE HWND MWindowSelector::GetMainWnd() const
{
    return m_hMainWnd;
}

MZC_INLINE HWND MWindowSelector::GetIconWindow() const
{
    return m_hwndIcon;
}

MZC_INLINE HWND MWindowSelector::GetSelectedWindow() const
{
    return m_hwndSelected;
}

MZC_INLINE HICON MWindowSelector::GetNullIcon() const
{
    return m_hicoNull;
}

MZC_INLINE HCURSOR MWindowSelector::GetAimingCursor() const
{
    return m_hcurAiming;
}

MZC_INLINE bool MWindowSelector::DoesAcceptChildWindow() const
{
    return m_bAcceptChild;
}

MZC_INLINE void
MWindowSelector::AcceptChildWindow(bool accept/* = true*/)
{
    m_bAcceptChild = accept;
}

MZC_INLINE void MWindowSelector::SetIconNull()
{
    ::SendMessage(m_hwndIcon, STM_SETICON,
                  reinterpret_cast<WPARAM>(GetNullIcon()), 0);
}

MZC_INLINE void MWindowSelector::SetIconDraggable()
{
    ::SendMessage(m_hwndIcon, STM_SETIMAGE,
        static_cast<WPARAM>(IMAGE_CURSOR),
        reinterpret_cast<LPARAM>(GetAimingCursor()));
}

MZC_INLINE void MWindowSelector::RestoreMainWnd()
{
    ::MoveWindow(m_hMainWnd, m_rcWindowSaved.left, m_rcWindowSaved.top,
        m_rcWindowSaved.right - m_rcWindowSaved.left,
        m_rcWindowSaved.bottom - m_rcWindowSaved.top, TRUE);
}

MZC_INLINE UINT MWindowSelector::GetNotifMsg() const
{
    return m_uNotifMsg;
}

////////////////////////////////////////////////////////////////////////////
