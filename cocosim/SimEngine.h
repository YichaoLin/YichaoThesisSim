#pragma once
#include "FieldMap.h"
#include <vector>
#include "FieldNode.h"
#include "Agent.h"
#include <list>
#include "TaskList.h"
using namespace std;

class CcocosimDlg;

class SimEngine
{
public:
	SimEngine();
	~SimEngine();

	void Init(int nNumofNodes, double aveDegree, int nAgentCount, bool Generate3D);
	void Run();
	void MoveOneStep();
	const FieldMap & GetOriMap() const { return m_OriginalMap; }
	void AddAgent(Agent * pNewAgent);
	FieldNode * GetNode(int i) const { return m_Nodes[i]; }
	void MoveAgent(Agent * pAgt, int toLocation);
	int GetTime() const { return m_nTime; }
	void SetEnd() { m_bStop = true; }
	void SetRandSeed(int nSeed) { m_nRandSeed = nSeed; }

	void RegisterEntry(const TaskEntry & newEntry);
	void SetFinished(int nID, const CString & strComment);

	void WriteLog(int nLevel, const CString & strLog);

	int GetStepCount() const;
	int GetAgentsCount() const {
		return m_Agents.size();
	}

	TaskList m_taskList;
	bool m_bNeedResetlist;
	CcocosimDlg * m_pDlg;
protected:
	void InitMap(int nNumofNodes, int connPercentage);
	void InitCRing(int nNumofNodes, const vector<int> & ds);
	void InitAgents(int nCount);

	FieldMap m_OriginalMap;
	vector<FieldNode *> m_Nodes;
	list<Agent *> m_Agents;
	bool m_bStop;
	int m_nTime;
	
private:
	int m_nBVPosition;
	int m_nRandSeed;
};

