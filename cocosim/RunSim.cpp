// RunSim.cpp : 实现文件
//

#include "stdafx.h"
#include "cocosim.h"
#include "RunSim.h"
#include "cocosimDlg.h"


// RunSim

IMPLEMENT_DYNCREATE(RunSim, CWinThread)

RunSim::RunSim()
{
}

RunSim::~RunSim()
{
}

BOOL RunSim::InitInstance()
{
	// TODO:    在此执行任意逐线程初始化
	return TRUE;
}

int RunSim::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

void RunSim::OnRun(WPARAM wParam, LPARAM lParam)
{
	m_pEngine->SetRandSeed(nRS);
	m_pEngine->Init(nNoN, fAD, nNoT, generate3d);
	m_pEngine->WriteLog(0, _T("Running .."));
	m_pEngine->Run();
	m_pDlg->PostMessage(MSG_ID_FINISH, 0, 0);
}

BEGIN_MESSAGE_MAP(RunSim, CWinThread)
	ON_THREAD_MESSAGE(MSG_ID_START_RUN, OnRun)
END_MESSAGE_MAP()


// RunSim 消息处理程序
