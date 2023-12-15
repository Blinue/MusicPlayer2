﻿// DataSettingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "DataSettingsDlg.h"
#include "afxdialogex.h"
#include "UpdateHelper.h"
#include "FilterHelper.h"


// CDataSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CDataSettingsDlg, CTabDlg)

CDataSettingsDlg::CDataSettingsDlg(CWnd* pParent /*=NULL*/)
    : CTabDlg(IDD_DATA_SETTINGS_DIALOG, pParent)
{

}

CDataSettingsDlg::~CDataSettingsDlg()
{
}

bool CDataSettingsDlg::IsAutoRunModified() const
{
    return m_auto_run_modified;
}

void CDataSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SF2_PATH_EDIT, m_sf2_path_edit);
    DDX_Control(pDX, IDC_COMBO1, m_language_combo);
}

void CDataSettingsDlg::GetDataFromUi()
{
    m_data.minimize_to_notify_icon = (((CButton*)GetDlgItem(IDC_MINIMIZE_TO_NOTIFY_RADIO))->GetCheck() != 0);
    m_data.save_lyric_to_song_folder = (((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER))->GetCheck() != 0);
    m_data.save_album_to_song_folder = (((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER3))->GetCheck() != 0);

    //获取语言的设置
    int sel_language = m_language_combo.GetCurSel();
    if (sel_language == 0)
        m_data.language_.clear();
    else
    {
        sel_language -= 1;
        const auto& language_list = theApp.m_str_table.GetLanguageList();
        if (sel_language >= 0 && sel_language < static_cast<int>(language_list.size()))
            m_data.language_ = language_list[sel_language].bcp_47;
    }
    if (m_data.language_ != theApp.m_general_setting_data.language_)
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_OPT_DATA_LANGUAGE_CHANGE_INFO");
        MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
    }

    //获取数据文件保存位置的设置
    m_data.portable_mode = (IsDlgButtonChecked(IDC_SAVE_TO_PROGRAM_DIR_RADIO) != 0);
    if (m_data.portable_mode != theApp.m_general_setting_data.portable_mode)
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_OPT_DATA_CFG_DIR_CHANGED_INFO");
        MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
    }
}

void CDataSettingsDlg::ApplyDataToUi()
{
    EnableControl();
}


BEGIN_MESSAGE_MAP(CDataSettingsDlg, CTabDlg)
    ON_BN_CLICKED(IDC_COVER_AUTO_DOWNLOAD_CHECK, &CDataSettingsDlg::OnBnClickedCoverAutoDownloadCheck)
    ON_BN_CLICKED(IDC_LYRIC_AUTO_DOWNLOAD_CHECK, &CDataSettingsDlg::OnBnClickedLyricAutoDownloadCheck)
    ON_BN_CLICKED(IDC_CHECK_UPDATE_CHECK, &CDataSettingsDlg::OnBnClickedCheckUpdateCheck)
    ON_BN_CLICKED(IDC_MIDI_USE_INNER_LYRIC_CHECK, &CDataSettingsDlg::OnBnClickedMidiUseInnerLyricCheck)
    ON_BN_CLICKED(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK, &CDataSettingsDlg::OnBnClickedDownloadWhenTagFullCheck)
    ON_EN_CHANGE(IDC_SF2_PATH_EDIT, &CDataSettingsDlg::OnEnChangeSf2PathEdit)
    ON_MESSAGE(WM_EDIT_BROWSE_CHANGED, &CDataSettingsDlg::OnEditBrowseChanged)
    ON_BN_CLICKED(IDC_AUTO_RUN_CHECK, &CDataSettingsDlg::OnBnClickedAutoRunCheck)
    ON_BN_CLICKED(IDC_GITHUB_RADIO, &CDataSettingsDlg::OnBnClickedGithubRadio)
    ON_BN_CLICKED(IDC_GITEE_RADIO, &CDataSettingsDlg::OnBnClickedGiteeRadio)
    ON_BN_CLICKED(IDC_OPEN_CONFIG_PATH_BUTTON, &CDataSettingsDlg::OnBnClickedOpenConfigPathButton)
END_MESSAGE_MAP()


// CDataSettingsDlg 消息处理程序


BOOL CDataSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    //SetBackgroundColor(RGB(255, 255, 255));

    m_language_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_DATA_LANGUAGE_FOLLOWING_SYSTEM").c_str());
    const auto& language_list = theApp.m_str_table.GetLanguageList();
    int language_sel{};
    for (size_t i{}; i < language_list.size(); ++i)
    {
        m_language_combo.AddString(language_list[i].display_name.c_str());
        if (language_list[i].bcp_47 == m_data.language_)
            language_sel = i + 1;
    }
    ASSERT(language_sel != 0 || m_data.language_.empty());  // 仅当设置为“跟随系统(空)”时索引才可能为0
    m_language_combo.SetCurSel(language_sel);

    m_auto_run = theApp.GetAutoRun();
    CheckDlgButton(IDC_AUTO_RUN_CHECK, m_auto_run);
    if (m_data.update_source == static_cast<int>(CUpdateHelper::UpdateSource::GitHubSource))
        CheckDlgButton(IDC_GITHUB_RADIO, TRUE);
    else
        CheckDlgButton(IDC_GITEE_RADIO, TRUE);

    CheckDlgButton(IDC_SAVE_TO_APPDATA_RADIO, !m_data.portable_mode);
    CheckDlgButton(IDC_SAVE_TO_PROGRAM_DIR_RADIO, m_data.portable_mode);
    EnableDlgCtrl(IDC_SAVE_TO_PROGRAM_DIR_RADIO, theApp.m_module_dir_writable);

    ((CButton*)GetDlgItem(IDC_COVER_AUTO_DOWNLOAD_CHECK))->SetCheck(m_data.auto_download_album_cover);
    ((CButton*)GetDlgItem(IDC_LYRIC_AUTO_DOWNLOAD_CHECK))->SetCheck(m_data.auto_download_lyric);
    ((CButton*)GetDlgItem(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK))->SetCheck(m_data.auto_download_only_tag_full);
    ((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->SetCheck(m_data.check_update_when_start);
    m_sf2_path_edit.SetWindowText(m_data.sf2_path.c_str());
    wstring sf2_filter = FilterHelper::GetSF2FileFilter();
    m_sf2_path_edit.EnableFileBrowseButton(L"SF2", sf2_filter.c_str());
    ((CButton*)GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK))->SetCheck(m_data.midi_use_inner_lyric);
    if (m_data.minimize_to_notify_icon)
        ((CButton*)GetDlgItem(IDC_MINIMIZE_TO_NOTIFY_RADIO))->SetCheck(TRUE);
    else
        ((CButton*)GetDlgItem(IDC_EXIT_PROGRAM_RADIO))->SetCheck(TRUE);

    if (m_data.save_lyric_to_song_folder)
        ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER))->SetCheck(TRUE);
    else
        ((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER))->SetCheck(TRUE);

    //判断歌词文件夹是否存在
    bool lyric_path_exist = CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path);
    if (!lyric_path_exist)		//如果歌词文件不存在，则禁用“保存到歌词文件夹”单选按钮，并强制选中“保存到歌曲所在目录”
    {
        ((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER))->EnableWindow(FALSE);
        ((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER))->SetCheck(FALSE);
        ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER))->SetCheck(TRUE);
        m_data.save_lyric_to_song_folder = true;
    }
    
    // 设置封面存储位置选项
    if (m_data.save_lyric_to_song_folder)
        ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER3))->SetCheck(TRUE);
    else
        ((CButton*)GetDlgItem(IDC_SAVE_TO_ALBUM_FOLDER3))->SetCheck(TRUE);

    // 判断封面文件夹是否存在
    bool album_path_exist = CCommon::FolderExist(theApp.m_app_setting_data.album_cover_path);
    if (!album_path_exist)		// 如果封面文件夹不存在，则禁用“保存到歌词文件夹”单选按钮，并强制选中“保存到歌曲所在目录”
    {
        ((CButton*)GetDlgItem(IDC_SAVE_TO_ALBUM_FOLDER3))->EnableWindow(FALSE);
        ((CButton*)GetDlgItem(IDC_SAVE_TO_ALBUM_FOLDER3))->SetCheck(FALSE);
        ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER3))->SetCheck(TRUE);
        m_data.save_album_to_song_folder = true;
    }

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(GetDlgItem(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK), theApp.m_str_table.LoadText(L"TIP_OPT_DATA_AUTO_DL_ONLY_WHEN_TAG_FULL").c_str());
    //m_toolTip.AddTool(GetDlgItem(IDC_SF2_PATH_EDIT), _T("需要额外的音色库才能播放 MIDI 音乐。"));
    m_toolTip.AddTool(GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK), theApp.m_str_table.LoadText(L"TIP_OPT_DATA_MIDI_USE_INNER_LYRIC_FIRSR").c_str());
    m_toolTip.AddTool(GetDlgItem(IDC_SAVE_TO_APPDATA_RADIO), theApp.m_appdata_dir.c_str());
    m_toolTip.AddTool(GetDlgItem(IDC_SAVE_TO_PROGRAM_DIR_RADIO), theApp.m_module_dir.c_str());

    m_toolTip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    EnableControl();

    //设置控件不响应鼠标滚轮消息
    m_language_combo.SetMouseWheelEnable(false);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CDataSettingsDlg::EnableControl()
{
    bool enable = CPlayer::GetInstance().IsBassCore();
    m_sf2_path_edit.EnableWindow(enable && theApp.m_format_convert_dialog_exit);		//正在进行格式转换时不允许更改音色库
    CWnd* pWnd = GetDlgItem(IDC_BROWSE_BUTTON);
    if (pWnd != nullptr)
        pWnd->EnableWindow(enable && theApp.m_format_convert_dialog_exit);
    pWnd = GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK);
    if (pWnd != nullptr)
        pWnd->EnableWindow(enable);

    ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER))->EnableWindow(m_data.auto_download_lyric);
    ((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER))->EnableWindow(m_data.auto_download_lyric && CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path));

    ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER3))->EnableWindow(m_data.auto_download_album_cover);
    ((CButton*)GetDlgItem(IDC_SAVE_TO_ALBUM_FOLDER3))->EnableWindow(m_data.auto_download_album_cover && CCommon::FolderExist(theApp.m_app_setting_data.album_cover_path));
}


void CDataSettingsDlg::OnBnClickedCoverAutoDownloadCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.auto_download_album_cover = (((CButton*)GetDlgItem(IDC_COVER_AUTO_DOWNLOAD_CHECK))->GetCheck() != 0);
    EnableControl();
}


void CDataSettingsDlg::OnBnClickedLyricAutoDownloadCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.auto_download_lyric = (((CButton*)GetDlgItem(IDC_LYRIC_AUTO_DOWNLOAD_CHECK))->GetCheck() != 0);
    EnableControl();
}


void CDataSettingsDlg::OnBnClickedCheckUpdateCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.check_update_when_start = (((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->GetCheck() != 0);
}


BOOL CDataSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_toolTip.RelayEvent(pMsg);

    return CTabDlg::PreTranslateMessage(pMsg);
}


void CDataSettingsDlg::OnBnClickedMidiUseInnerLyricCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.midi_use_inner_lyric = (((CButton*)GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK))->GetCheck() != 0);
}


void CDataSettingsDlg::OnBnClickedDownloadWhenTagFullCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.auto_download_only_tag_full = (((CButton*)GetDlgItem(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK))->GetCheck() != 0);
}


void CDataSettingsDlg::OnEnChangeSf2PathEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。
    if (m_sf2_path_edit.GetModify())
    {
        CString str;
        m_sf2_path_edit.GetWindowText(str);
        m_data.sf2_path = str;
    }

    // TODO:  在此添加控件通知处理程序代码
}


void CDataSettingsDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类


    CTabDlg::OnOK();
}


afx_msg LRESULT CDataSettingsDlg::OnEditBrowseChanged(WPARAM wParam, LPARAM lParam)
{
    CString str;
    m_sf2_path_edit.GetWindowText(str);
    m_data.sf2_path = str;
    return 0;
}


void CDataSettingsDlg::OnBnClickedAutoRunCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_auto_run = (IsDlgButtonChecked(IDC_AUTO_RUN_CHECK) != 0);
    m_auto_run_modified = true;
}


void CDataSettingsDlg::OnBnClickedGithubRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.update_source = static_cast<int>(CUpdateHelper::UpdateSource::GitHubSource);
}


void CDataSettingsDlg::OnBnClickedGiteeRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.update_source = static_cast<int>(CUpdateHelper::UpdateSource::GiteeSource);
}


void CDataSettingsDlg::OnBnClickedOpenConfigPathButton()
{
    // TODO: 在此添加控件通知处理程序代码
    ShellExecute(NULL, _T("explore"), theApp.m_config_dir.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
