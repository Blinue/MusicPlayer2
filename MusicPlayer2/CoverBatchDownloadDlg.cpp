// CoverBatchDownloadDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CoverBatchDownloadDlg.h"
#include "afxdialogex.h"


// CCoverBatchDownloadDlg 对话框

IMPLEMENT_DYNAMIC(CCoverBatchDownloadDlg, CDialog)

CCoverBatchDownloadDlg::CCoverBatchDownloadDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_COVER_BATCH_DOWNLOAD_DIALOG, pParent)
{

}

CCoverBatchDownloadDlg::~CCoverBatchDownloadDlg()
{
}

void CCoverBatchDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCoverBatchDownloadDlg, CDialog)
END_MESSAGE_MAP()


// CCoverBatchDownloadDlg 消息处理程序
