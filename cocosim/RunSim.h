#pragma once
#include "SimEngine.h"

class CcocosimDlg;


enum {
	MSG_ID_START_RUN = WM_USER + 100,
};

// RunSim

class RunSim : public CWinThread
{
	DECLARE_DYNCREATE(RunSim)
public:
	void SetEngine(SimEngine * pEngine) { m_pEngine = pEngine; }
	void SetMainWindow(CcocosimDlg * pDlg) { m_pDlg = pDlg; }


	int nRS;
	int nNoN;
	double fAD;
	int nNoT;
	bool generate3d;

protected:
	RunSim();           // 动态创建所使用的受保护的构造函数
	virtual ~RunSim();


public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	int GetRobotCount() const {
		if (!m_pEngine) {
			return -1;
		}
		else {
			return m_pEngine->GetAgentsCount();
		}
	}

protected:
	DECLARE_MESSAGE_MAP()
	void OnRun(WPARAM wParam, LPARAM lParam);

private:
	SimEngine * m_pEngine;
	CcocosimDlg * m_pDlg;
};


