#pragma once


// CCoverBatchDownloadDlg 对话框

class CCoverBatchDownloadDlg : public CDialog
{
	DECLARE_DYNAMIC(CCoverBatchDownloadDlg)

public:
	CCoverBatchDownloadDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCoverBatchDownloadDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COVER_BATCH_DOWNLOAD_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
