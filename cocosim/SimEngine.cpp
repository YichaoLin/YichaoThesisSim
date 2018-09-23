#include "stdafx.h"
#include "SimEngine.h"
#include "CastleNode.h"

#define TEST_CASTLE_ALG
#ifdef TEST_CASTLE_ALG
#include "AgentCastle.h"
typedef AgentCastle AGENTTYPE;
typedef CastleNode NODETYPE;
#define NUMBEROFPHASE 4
#endif
#include "Generate3DCoor.h"
#include "cocosimDlg.h"

SimEngine::SimEngine()
	: m_bStop(false)
	, m_nTime(0)
	, m_nBVPosition(-1)
	, m_nRandSeed(0)
	, m_bNeedResetlist(false)
	, m_pDlg(NULL)
{
}


SimEngine::~SimEngine()
{
	for (int i = 0; i < m_Nodes.size(); ++i)
	{
		delete m_Nodes[i];
	}
	for (list<Agent *>::iterator ptr = m_Agents.begin(); ptr != m_Agents.end(); ++ptr)
	{
		delete *ptr;
		*ptr = NULL;
	}
}

void SimEngine::Init(int nNumofNodes, double aveDegree, int nAgentCount, bool Generate3D)
{
	Agent::ResetID();
	srand(m_nRandSeed);
	if (nNumofNodes <= 1 || aveDegree > nNumofNodes - 1)
	{
		return;
	}
	int connPercentage = 100 * aveDegree / (nNumofNodes - 1);
	if (nAgentCount == 0 || connPercentage > 100) {
		return;
	}
	InitMap(nNumofNodes, connPercentage);
	if (Generate3D)
	{
		WriteLog(0, _T("Generating 3D coordinators"));
		Generate3DCoor gc;
		gc.GetCoor(0, 700, 10, m_OriginalMap);
		gc.Output();
	}
	InitAgents(nAgentCount);
	m_nBVPosition = rand() % nNumofNodes;

	++m_nTime;
}

void SimEngine::Run()
{
	while (!m_bStop) {
		if (m_nTime == 31)
		{
			int mmm = 0;
		}
		MoveOneStep();
		if (m_nTime == 2)
		{
			if (m_pDlg)
			{
				m_pDlg->PostMessageW(MSG_ID_REINIT_TASK);
			}
		}
	}
}

void SimEngine::MoveOneStep()
{
	CString strOut;
	strOut.Format(_T("\nTime: %d\n"), m_nTime);
	WriteLog(9, strOut);
	for (int i = 0; i < NUMBEROFPHASE; ++i) {
		list<Agent *>::iterator pAgt = m_Agents.begin();
		while (pAgt != m_Agents.end()) {
			(*pAgt)->NextStep(m_nTime, i);
			++pAgt;
		}
#if 0
		pAgt = m_Agents.begin();
		while (pAgt != m_Agents.end()) {
			if (m_nBVPosition == (*pAgt)->GetLocation()) {
				m_bStop = true;
				return;
	}
			++pAgt;
}
#endif
		}
	if (m_nTime >= 61)
	{
		int m = 0;
	}
	++m_nTime;
}

void SimEngine::AddAgent(Agent * pNewAgent)
{
	m_Agents.push_back(pNewAgent);
}

void SimEngine::MoveAgent(Agent * pAgt, int toLocation)
{
	pAgt->m_nLastLocation = pAgt->GetLocation();
	CString strOut;
	if (pAgt->GetLocation() == toLocation)
	{
		strOut.Format(_T("T%d %s stay at %d\n"), m_nTime, pAgt->GetName(), pAgt->GetLocation(), toLocation);
	}
	else
	{
		strOut.Format(_T("T%d %s from %d move to %d\n"), m_nTime, pAgt->GetName(), pAgt->GetLocation(), toLocation);
		m_Nodes[pAgt->GetLocation()]->m_Agents.erase(pAgt);
		m_Nodes[toLocation]->m_Agents.insert(pAgt);
		pAgt->SetLocation(toLocation);
	}
	WriteLog(9, strOut);
}

void SimEngine::RegisterEntry(const TaskEntry & newEntry)
{
	m_taskList.RegisterEntry(newEntry);
}

void SimEngine::SetFinished(int nID, const CString & strComment)
{
	m_taskList.SetFinished(nID, strComment);
	if (m_pDlg)
	{
		m_pDlg->PostMessage(MSG_ID_UPDATE, nID);
	}
	if (m_taskList.m_nFinishedCount == m_taskList.m_entries.size())
	{
		m_bStop = true;
	}
}

void SimEngine::WriteLog(int nLevel, const CString & strLog)
{
	switch (nLevel)
	{
	case 0:
	{
		if (m_pDlg)
		{
			CString * pNewLog = new CString(strLog);
			m_pDlg->PostMessage(MSG_ID_COMMENT, (WPARAM)pNewLog, 0);
			break;
		}
	}
	case 1:
	{
		OutputDebugString(_T("!!!!        ") + strLog);
		break;
	}
	default:
	{
		if (m_OriginalMap.GetNodeCount() <= 30)
		{
			OutputDebugString(strLog);
		}
	}
	}
}

int SimEngine::GetStepCount() const
{
	return m_nTime;
}

void SimEngine::InitMap(int nNumofNodes, int connPercentage)
{
	m_OriginalMap.SetNodeCount(nNumofNodes);
#if 1
	m_OriginalMap.GenerateConnectGraph(connPercentage);
#else
	vector<int> ds;
	ds.resize(3);
	ds[0] = 1;
	ds[1] = 2;
	ds[2] = 7;
	m_OriginalMap.InitCRing(ds);
#endif
	m_Nodes.resize(nNumofNodes);

	m_Nodes[0] = new NODETYPE();
	m_Nodes[0]->m_NodeId = 0;
	m_Nodes[0]->SetAsIntersection();
	((NODETYPE*)m_Nodes[0])->SetOriginalMap(&m_OriginalMap);
	((NODETYPE*)m_Nodes[0])->m_pPocketMap->AddOneSafeNode(0);

	for (int i = 1; i < nNumofNodes; ++i)
	{
		m_Nodes[i] = new NODETYPE();
		m_Nodes[i]->m_NodeId = i;
#ifdef TEST_CASTLE_ALG
		if (m_OriginalMap.GetDegree(i) > 2)
		{
			m_Nodes[i]->SetAsIntersection();
		}
		((NODETYPE*)m_Nodes[i])->SetOriginalMap(&m_OriginalMap);
#endif
	}

}

void SimEngine::InitCRing(int nNumofNodes, const vector<int> & ds)
{

}

void SimEngine::InitAgents(int nCount)
{
	for (int i = 0; i < nCount; ++i) {
		AGENTTYPE * pNewAgent = new AGENTTYPE();
#ifdef TEST_CASTLE_ALG
		m_Agents.push_back(pNewAgent);
		pNewAgent->Init(this);
#endif
	}
}
