
// cocosimDlg.h : 头文件
//

#pragma once
#include "SimEngine.h"
#include "afxwin.h"
#include "afxcmn.h"

class RunSim;

enum {
	MSG_ID_FINISH = WM_USER + 200,
	MSG_ID_UPDATE,
	MSG_ID_COMMENT,
	MSG_ID_REINIT_TASK,
};

// CcocosimDlg 对话框
class CcocosimDlg : public CDialogEx
{
// 构造
public:
	CcocosimDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COCOSIM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT ReinitTaskTable(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateComment(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRunFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRunStateUpdated(WPARAM wParam, LPARAM lParam);

	void InitTaskList();
	void TestOnce();
	bool TestNext();

	SimEngine * m_pEngine;
	CBrush m_bkBrush;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCheck1();
	CEdit m_editRandSeed;
	CEdit m_editNumOfNodes;
	CEdit m_editAveDegree;
	CEdit m_editNumOfTeam;
	CButton m_checkGenerate3D;
	afx_msg void OnBnClickedButtonGrs();


private:
	int m_nSeed;
	RunSim * m_pRun;
	map<int, int> m_taskToIndex;
	int m_nTestTime;
	int m_nRunTime;

	CArray<int> m_NoNs;
	CArray<int> m_NoTs;
	int m_EdgePercentage;
	bool m_bInBatchMode;
	int m_nNodeIndex;
	int m_nTeamIndex;
public:
	CButton m_btnRun;
	CEdit m_editComment;
	CListCtrl m_listTask;
	CProgressCtrl m_prg;
	CEdit m_editRunTime;
	CListCtrl m_listResults;
	afx_msg void OnBnClickedBtnGrouprun();
	CButton m_ctrlBtnBatchRun;
	CButton m_ctrlBtnBatchStop;
	afx_msg void OnBnClickedBtnStop();
};
