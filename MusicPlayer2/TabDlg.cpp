﻿#include "stdafx.h"
#include "TabDlg.h"
#include "MusicPlayer2.h"
#include "WinVersionHelper.h"

IMPLEMENT_DYNAMIC(CTabDlg, CBaseDialog)

CTabDlg::CTabDlg(UINT nIDTemplate, CWnd * pParent) : CBaseDialog(nIDTemplate, pParent)
{
}


CTabDlg::~CTabDlg()
{
}

CWnd* CTabDlg::GetParentWindow()
{
    CWnd* pParent = GetParent();
    if (pParent != nullptr)
    {
        return pParent->GetParent();
    }
    return nullptr;
}

void CTabDlg::SetScrollbarInfo(int nPage, int nMax)
{
    //初始化滚动条
    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    scrollinfo.nPage = nPage;    //设置滑块大小
    scrollinfo.nMin = 0;
    scrollinfo.nMax = nMax;     //设置滚动条的最大位置
    if (scrollinfo.nMax < 0)
        scrollinfo.nMax = 0;
    scrollinfo.nPos = scrollinfo.nMin;
    m_last_pos = scrollinfo.nPos;
    SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);

    m_scroll_enable = true;
}

void CTabDlg::ResetScroll()
{
    if (m_scroll_enable)
    {
        SCROLLINFO scrollinfo;
        GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
        int step = scrollinfo.nPos - scrollinfo.nMin;
        scrollinfo.nPos = scrollinfo.nMin;
        m_last_pos = scrollinfo.nPos;
        SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
        ScrollWindow(0, step);
    }
}

void CTabDlg::ScrollWindowSimple(int step)
{
    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    scrollinfo.nPos -= step;
    if (scrollinfo.nPos < scrollinfo.nMin)
    {
        step = scrollinfo.nPos + step - scrollinfo.nMin;        //如果向上滚动一个距离后小于滚动条的最小位置了，则修正step的值，使窗口滚动到最上方
        scrollinfo.nPos = scrollinfo.nMin;
    }
    if (scrollinfo.nPos + static_cast<int>(scrollinfo.nPage) > scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
    {
        step -= (scrollinfo.nMax - (scrollinfo.nPos + scrollinfo.nPage));       //如果向上滚动一个距离后大于滚动条的最大位置了，则修正step的值，使窗口滚动到最下方
        if (step > 0)
            step = 0;
        scrollinfo.nPos = scrollinfo.nMax;
    }
    SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    ScrollWindow(0, step);

}

BEGIN_MESSAGE_MAP(CTabDlg, CBaseDialog)
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


BOOL CTabDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    // 设置窗口背景色
    SetBackgroundColor(CWinVersionHelper::IsWindows11OrLater() ? RGB(249, 249, 249) : RGB(255, 255, 255));
    // SetBackgroundColor(RGB(255, 240, 244));   // 测试用

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


BOOL CTabDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        return TRUE;
    }
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        return TRUE;
    }

    return CBaseDialog::PreTranslateMessage(pMsg);
}


void CTabDlg::OnOK()
{
    // 此处将CDialog原本protected的默认OnOK重写为Public
    // 这是为了对外提供统一调用标签页（此类的派生类）OnOK的方式
    // 而不必区分具体是哪个派生类（dynamic_cast<CTabDlg*>）
    CBaseDialog::OnOK();
}


void CTabDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //参考资料：https://www.cnblogs.com/ranjiewen/p/6013922.html

    if (m_scroll_enable)
    {
        SCROLLINFO scrollinfo;
        GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
        int unit = 1;
        int step = theApp.DPI(16);
        switch (nSBCode)
        {
        case SB_LINEUP:          //Scroll one line up
            ScrollWindowSimple(unit * step);
            break;
        case SB_LINEDOWN:           //Scroll one line down
            ScrollWindowSimple(-unit * step);
            break;
        case SB_PAGEUP:            //Scroll one page up.
            ScrollWindowSimple(unit * step * 5);
            break;
        case SB_PAGEDOWN:        //Scroll one page down        
            ScrollWindowSimple(-unit * step * 5);
            break;
        case SB_ENDSCROLL:      //End scroll     
            break;
        case SB_THUMBPOSITION:  //Scroll to the absolute position. The current position is provided in nPos
            break;
        case SB_THUMBTRACK:                  //Drag scroll box to specified position. The current position is provided in nPos
        {
            int y_amount = (m_last_pos - nPos)*unit;
            ScrollWindow(0, y_amount);
            scrollinfo.nPos = nPos;
            SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
        }
            break;
        }
        m_last_pos = scrollinfo.nPos;
    }
    CBaseDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CTabDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (m_scroll_enable)
    {
        int step = theApp.DPI(64);
        if (zDelta > 0)
        {
            ScrollWindowSimple(step);
        }
        if (zDelta < 0)
        {
            ScrollWindowSimple(-step);
        }
        SCROLLINFO scrollinfo;
        GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
        m_last_pos = scrollinfo.nPos;
    }

    return CBaseDialog::OnMouseWheel(nFlags, zDelta, pt);
}
