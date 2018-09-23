
// cocosimDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "cocosim.h"
#include "cocosimDlg.h"
#include "afxdialogex.h"
#include "FieldMap.h"
#include "RunSim.h"
#include <fstream>
#include <string.h>
using namespace std;

/*
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif
*/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CcocosimDlg �Ի���



CcocosimDlg::CcocosimDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_COCOSIM_DIALOG, pParent)
	, m_pRun(NULL)
{
	srand(time(NULL));
	m_nSeed = rand();
	m_pEngine = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcocosimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RANDSEED, m_editRandSeed);
	DDX_Control(pDX, IDC_EDIT_NUMOFNODES, m_editNumOfNodes);
	DDX_Control(pDX, IDC_EDIT_AVE_DEGREE, m_editAveDegree);
	DDX_Control(pDX, IDC_EDIT_NUMOFTEAM, m_editNumOfTeam);
	DDX_Control(pDX, IDC_CHECK_G3D, m_checkGenerate3D);
	DDX_Control(pDX, IDC_BUTTON2, m_btnRun);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_editComment);
	DDX_Control(pDX, IDC_LIST_TASK, m_listTask);
	DDX_Control(pDX, IDC_PROGRESS1, m_prg);
	DDX_Control(pDX, IDC_EDIT_RUN_TIME, m_editRunTime);
	DDX_Control(pDX, IDC_LIST1, m_listResults);
	DDX_Control(pDX, IDC_BTN_GROUPRUN, m_ctrlBtnBatchRun);
	DDX_Control(pDX, IDC_BTN_STOP, m_ctrlBtnBatchStop);
}

BEGIN_MESSAGE_MAP(CcocosimDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &CcocosimDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, &CcocosimDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_GRS, &CcocosimDlg::OnBnClickedButtonGrs)
	ON_MESSAGE(MSG_ID_UPDATE, &CcocosimDlg::OnRunStateUpdated)
	ON_MESSAGE(MSG_ID_FINISH, &CcocosimDlg::OnRunFinished)
	ON_MESSAGE(MSG_ID_COMMENT, &CcocosimDlg::OnUpdateComment)
	ON_MESSAGE(MSG_ID_REINIT_TASK, &CcocosimDlg::ReinitTaskTable)
	ON_BN_CLICKED(IDC_BTN_GROUPRUN, &CcocosimDlg::OnBnClickedBtnGrouprun)
	ON_BN_CLICKED(IDC_BTN_STOP, &CcocosimDlg::OnBnClickedBtnStop)
END_MESSAGE_MAP()


// CcocosimDlg ��Ϣ�������

BOOL CcocosimDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//ShowWindow(SW_MAXIMIZE);
	ShowWindow(SW_NORMAL);

	/*
	Py_Initialize();
	PyObject* moduleName = PyString_FromString("get3dgraph"); //ģ�����������ļ���
	PyObject* pModule = PyImport_Import(moduleName);
	if (!pModule) // ����ģ��ʧ��
	{
		//cout << "[ERROR] Python get module failed." << endl;
		return 0;
	}
	//cout << "[INFO] Python get module succeed." << endl;

	// ���غ���
	PyObject* pv = PyObject_GetAttrString(pModule, "CreateFileTest");
	if (!pv || !PyCallable_Check(pv))  // ��֤�Ƿ���سɹ�
	{
		//cout << "[ERROR] Can't find funftion (test_add)" << endl;
		return 0;
	}
	//cout << "[INFO] Get function (test_add) succeed." << endl;

	// ���ò���
	PyObject* args = PyTuple_New(2);   // 2������
	PyObject* arg1 = PyInt_FromLong(4);    // ����һ��Ϊ4
	PyObject* arg2 = PyInt_FromLong(3);    // ��������Ϊ3
	PyTuple_SetItem(args, 0, arg1);
	PyTuple_SetItem(args, 1, arg2);
	PyObject* pRet = PyObject_CallObject(pv, NULL);
	Py_Finalize();
	*/

	//srand(time(NULL));

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_bkBrush.CreateSolidBrush(RGB(30, 30, 30));
	//m_Engine.Run();

	m_editRandSeed.SetWindowTextW(_T("3054"));
	m_editNumOfNodes.SetWindowTextW(_T("30"));
	m_editNumOfTeam.SetWindowTextW(_T("4"));
	m_editAveDegree.SetWindowTextW(_T("3"));
	m_editRunTime.SetWindowTextW(_T("1"));

	m_listTask.InsertColumn(0, _T("No."), 0, 50);
	m_listTask.InsertColumn(1, _T("Towers in castle"), 0, 150);
	m_listTask.InsertColumn(2, _T("State"), 0, 70);
	m_listTask.InsertColumn(3, _T("Comment"), 0, 220);

	m_listResults.InsertColumn(0, _T("Seed"), 0, 100);
	m_listResults.InsertColumn(1, _T("Steps"), 0, 100);

	int maxNodeCount = 100;
	for (int i = 60; i <= maxNodeCount; i += 20)
	{
		m_NoNs.Add(i);
	}
	int maxTeamCount = 4;
	for (int i = 1; i <= maxTeamCount; i++)
	{
		m_NoTs.Add(i);
	}
	m_bInBatchMode = false;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CcocosimDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CcocosimDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CcocosimDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



LRESULT CcocosimDlg::ReinitTaskTable(WPARAM wParam, LPARAM lParam)
{
	m_prg.SetRange(0, m_pEngine->m_taskList.m_entries.size());
	m_prg.SetStep(1);
	m_prg.SetPos(0);
	InitTaskList();
	return 0;
}

LRESULT CcocosimDlg::OnUpdateComment(WPARAM wParam, LPARAM lParam)
{
	CString * pComment = (CString *)wParam;
	if (!pComment)
	{
		return 0;
	}
	m_editComment.SetWindowTextW(*pComment);
	delete pComment;
}

LRESULT CcocosimDlg::OnRunFinished(WPARAM wParam, LPARAM lParam)
{
	CString strCount;
	strCount.Format(_T("Finished: step count %d"), m_pEngine->GetStepCount());
	m_editComment.SetWindowTextW(strCount);
	char filename[200] = { 0 };
	sprintf_s(filename, "testresult_N%03d_T%d_D%0.2f.csv", m_pRun->nNoN, m_pRun->nNoT, m_pRun->fAD);
	ofstream fout(filename, ios::app);
	fout << m_pRun->nRS << "," << m_pRun->nNoN << "," << m_pRun->fAD << "," << m_pRun->nNoT << "," << m_pEngine->GetStepCount() << ","<<m_pEngine->GetAgentsCount() << endl;
	fout.close();

	CString strItem;
	strItem.Format(_T("%d"), m_pRun->nRS);
	m_listResults.SetItemText(m_nRunTime - 1, 0, strItem);
	strItem.Format(_T("%d"), m_pEngine->GetStepCount());
	m_listResults.SetItemText(m_nRunTime - 1, 1, strItem);

	if (m_pRun)
	{
		DWORD ExitCode;
		GetExitCodeThread(m_pRun->m_hThread, &ExitCode);
		TerminateThread(m_pRun->m_hThread, ExitCode);
	}

	OnBnClickedButtonGrs();

	TestOnce();
	return 0;
}

LRESULT CcocosimDlg::OnRunStateUpdated(WPARAM wParam, LPARAM lParam)
{
	m_prg.SetPos(m_pEngine->m_taskList.m_nFinishedCount);
	int nIndex = m_taskToIndex[wParam];
	m_listTask.SetItemText(nIndex, 2, _T("OK"));
	m_listTask.SetItemText(nIndex, 3, m_pEngine->m_taskList.m_entries[nIndex].m_strFinishComment);
	return 0;
}

void CcocosimDlg::InitTaskList()
{
	m_taskToIndex.clear();
	if (!m_pEngine)
	{
		return;
	}
	int nIndex = 0;
	map<int, TaskEntry>::const_iterator ptr = m_pEngine->m_taskList.m_entries.cbegin();
	while (ptr != m_pEngine->m_taskList.m_entries.cend())
	{
		m_listTask.InsertItem(nIndex, NULL);
		CString strNo;
		strNo.Format(_T("%d"), nIndex);
		m_listTask.SetItemText(nIndex, 0, strNo);
		CString strName;
		strName = ptr->second.m_strName;
		m_listTask.SetItemText(nIndex, 1, strName);
		m_listTask.SetItemText(nIndex, 2, _T("to do"));

		m_taskToIndex[ptr->first] = nIndex;
		++nIndex;
		++ptr;
	}
}

void CcocosimDlg::TestOnce()
{
	if (m_nRunTime >= m_nTestTime)
	{
		m_btnRun.EnableWindow(TRUE);
		m_editRunTime.EnableWindow(TRUE);
		if (m_bInBatchMode)
		{
			if (TestNext())
			{
				OnBnClickedButton2();
			}
			else
			{
				OnBnClickedBtnStop();
				m_ctrlBtnBatchRun.EnableWindow(TRUE);
			}
		}
		else
		{
			m_ctrlBtnBatchRun.EnableWindow(TRUE);
		}
		return;
	}
	++m_nRunTime;

	m_editComment.SetWindowTextW(_T("New test"));
	m_listTask.SetRedraw(FALSE);
	m_listTask.DeleteAllItems();
	m_listTask.SetRedraw(TRUE);

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_pRun = (RunSim*)AfxBeginThread(RUNTIME_CLASS(RunSim));
	m_pRun->SetMainWindow(this);

	if (m_pEngine != NULL)
	{
		delete m_pEngine;
	}
	m_pEngine = new SimEngine();
	m_pEngine->m_pDlg = this;

	m_pRun->SetEngine(m_pEngine);

	CString strRandSeed;
	m_editRandSeed.GetWindowTextW(strRandSeed);
	CString strNumOfNodes;
	m_editNumOfNodes.GetWindowTextW(strNumOfNodes);
	CString strAveDegree;
	m_editAveDegree.GetWindowTextW(strAveDegree);
	CString strNumOfTeams;
	m_editNumOfTeam.GetWindowTextW(strNumOfTeams);

	m_pRun->nRS = _ttoi(strRandSeed);
	m_pRun->nNoN = _ttoi(strNumOfNodes);
	m_pRun->fAD = _ttof(strAveDegree);
	m_pRun->nNoT = _ttoi(strNumOfTeams);
	m_pRun->generate3d = false;

	UINT nCheck = m_checkGenerate3D.GetState();
	if (nCheck == BST_CHECKED)
	{
		if (m_pRun->nNoN <= 100 && m_nTestTime == 1)
		{
			m_pRun->generate3d = true;
		}
		else
		{
			m_checkGenerate3D.SetCheck(BST_UNCHECKED);
		}
	}

	m_pRun->PostThreadMessage(MSG_ID_START_RUN, 0, 0);
}

bool CcocosimDlg::TestNext()
{
	if (m_EdgePercentage == 100) {
		if (m_nTeamIndex == m_NoTs.GetSize() - 1)
		{
			if (m_nNodeIndex == m_NoNs.GetSize() - 1) {
				return false;
			}
			else
			{
				m_nNodeIndex++;
				CString strNumOfNode;
				strNumOfNode.Format(_T("%d"), m_NoNs[m_nNodeIndex]);
				m_editNumOfNodes.SetWindowTextW(strNumOfNode);
			}
			m_nTeamIndex = 0;
		}
		else
		{
			++m_nTeamIndex;
		}
		CString strNumOfTeam;
		strNumOfTeam.Format(_T("%d"), m_NoTs[m_nTeamIndex]);
		m_editNumOfTeam.SetWindowTextW(strNumOfTeam);
		m_EdgePercentage = 10;
	}
	else {
		m_EdgePercentage += 10;
	}
	double fDegree = (m_NoNs[m_nNodeIndex] - 1)*m_EdgePercentage / 100.0;
	if (fDegree > 29) {
		int k = 0;
	}
	CString strDegree;
	strDegree.Format(_T("%.2f"), fDegree);
	m_editAveDegree.SetWindowTextW(strDegree);
	return true;
}

HBRUSH CcocosimDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
#if 1
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO: Change any attributes of the DC here
	if (nCtlColor == CTLCOLOR_STATIC)//�����ǰ�ؼ����ھ�̬�ı�
	{
		pDC->SetTextColor(RGB(255, 0, 0)); //������ɫ
										   //pDC->SetBkMode(TRANSPARENT); //�������屳��Ϊ͸��
		pDC->SetBkColor(RGB(0, 0, 255));  //���屳��ɫ
	}
	else if (nCtlColor == CTLCOLOR_BTN) //�����ǰ�ؼ����ڰ�ť
	{
		pDC->SetTextColor(RGB(255, 0, 0)); //������ɫ
										   //pDC->SetBkMode(TRANSPARENT); //�������屳��Ϊ͸��
		pDC->SetBkColor(RGB(0, 0, 255));  //���屳��ɫ
	}
	else if (nCtlColor == CTLCOLOR_DLG)
	{
		return m_bkBrush;
	}
#else
	// TODO: Return a different brush if the default is not desired
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor); // TODO: Change any attributes of the DC here 
	if(nCtlColor==CTLCOLOR_DLG) //�����CTLCOLOR_EDIT edit����ɫ�ܸı��� 
	{
		m_bkBrush.CreateSolidBrush(RGB(255,0,0)); //����һ�ѻ�ɫ�ı���ˢ�� 
		return m_bkBrush; //���ظղŴ����ı���ˢ�� 
	}
#endif
	return hbr;
}


void CcocosimDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_pEngine->MoveOneStep();
	//CDialogEx::OnOK();
}


void CcocosimDlg::OnBnClickedButton2()
{
	m_listResults.SetRedraw(FALSE);
	m_listResults.DeleteAllItems();
	m_listResults.SetRedraw(TRUE);

	CString strRunTimes;
	m_editRunTime.GetWindowTextW(strRunTimes);
	m_nTestTime = _ttoi(strRunTimes);
	m_nRunTime = 0;
	if (m_nTestTime < 0 || m_nTestTime > 200)
	{
		return;
	}

	for (int i = 0; i < m_nTestTime; ++i)
	{
		m_listResults.InsertItem(i, NULL);
		m_listResults.SetItemText(i, 0, _T("--"));
		m_listResults.SetItemText(i, 1, _T("--"));
	}

	m_btnRun.EnableWindow(FALSE);
	m_editRunTime.EnableWindow(FALSE);

	TestOnce();
}


void CcocosimDlg::OnBnClickedCheck1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CcocosimDlg::OnBnClickedButtonGrs()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strSeed;
	strSeed.Format(_T("%d"), ++m_nSeed);
	m_editRandSeed.SetWindowTextW(strSeed);
}


void CcocosimDlg::OnBnClickedBtnGrouprun()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ctrlBtnBatchRun.EnableWindow(FALSE);
	m_EdgePercentage = 0;
	m_bInBatchMode = true;
	m_nNodeIndex = 0;
	m_nTeamIndex = 0;
	CString strNumOfNode;
	strNumOfNode.Format(_T("%d"), m_NoNs[m_nNodeIndex]);
	m_editNumOfNodes.SetWindowTextW(strNumOfNode);
	CString strNumOfTeam;
	strNumOfTeam.Format(_T("%d"), m_NoTs[m_nTeamIndex]);
	m_editNumOfTeam.SetWindowTextW(strNumOfTeam);
	if (TestNext())
	{
		OnBnClickedButton2();
		m_ctrlBtnBatchStop.EnableWindow(TRUE);
	}
	else
	{
		OnBnClickedBtnStop();
		m_ctrlBtnBatchRun.EnableWindow(TRUE);
	}
}


void CcocosimDlg::OnBnClickedBtnStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bInBatchMode = false;
	m_ctrlBtnBatchStop.EnableWindow(FALSE);
}
