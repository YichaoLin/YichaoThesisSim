#include "stdafx.h"
#include "RoutingTable.h"
#include <map>
#include <queue>
#include <list>
using namespace std;


RoutingTable::RoutingTable()
{
	m_pFM = NULL;
	m_nStartNodeIndex = -1;
}


RoutingTable::~RoutingTable()
{
	m_nStartNodeIndex = -1;
}

void RoutingTable::SetFieldMap(const FieldMap * fm)
{
	m_pFM = fm;

	const int fmNodeCount = fm->GetNodeCount();
	m_table.resize(fmNodeCount);
	Clean();
}

void RoutingTable::GenerateTable(int startIndex, int accessLevel)
{
	GenerateTableTo(startIndex, -1, accessLevel);
}

int RoutingTable::GetPath(int startIndex, int endIndex, int accessLevel, list<int> & path)
{
	GenerateTableTo(startIndex, endIndex, accessLevel);
	int nDistance = GetEntry(endIndex).m_nDistance;
	if (nDistance == INT_MAX) {
		return INT_MAX;
	}
	deque<int> revertPath;
	int prevIndex = endIndex;
	while (prevIndex != startIndex) {
		revertPath.push_back(prevIndex);
		prevIndex = GetEntry(prevIndex).m_nPrevIndex;
	}
	while (revertPath.size() > 0) {
		path.push_back(revertPath.back());
		revertPath.pop_back();
	}
	return nDistance;
}

CString RoutingTable::GetRouteTableString() const
{
	CString strOut = _T("RT:\n");
	for (int i = 0; i < m_table.size(); ++i)
	{
		CString strTmp;
		strTmp.Format(_T("%d\tdis:%d\tprev:%d\n"), i, m_table[i].m_nDistance, m_table[i].m_nPrevIndex);
		strOut += strTmp;
	}
	return strOut;
}

void RoutingTable::GenerateTableTo(int startIndex, int endIndex, int accessLevel)
{
	if (startIndex == m_nStartNodeIndex) {
		return;
	}
	Clean();
	m_nStartNodeIndex = startIndex;
	m_table[startIndex].m_nDistance = 0;

	const int fmNodeCount = m_pFM->GetNodeCount();
	bool * visited = new bool[fmNodeCount];
	memset(visited, 0, sizeof(bool) * fmNodeCount);

	queue<int> waiting;
	waiting.push(startIndex);
	visited[startIndex] = true;
	while (!waiting.empty()) {
		int visiting = waiting.front();
		waiting.pop();
		for (int i = 0; i < fmNodeCount; ++i) {
			DirectedEdge de = m_pFM->GetConnection(visiting, i);
			if (de.m_eState >= accessLevel) {
				if (visited[i] == false) {
					waiting.push(i);
					visited[i] = true;
					m_table[i].m_nDistance = m_table[visiting].m_nDistance + de.m_nLength;
					m_table[i].m_nPrevIndex = visiting;
					if (i == endIndex) {
						delete[] visited;
						return;
					}
				}
			}
		}
	}
	delete[] visited;
}

void RoutingTable::Clean()
{
	const int fmNodeCount = m_pFM->GetNodeCount();
	for (int i = 0; i < fmNodeCount; ++i) {
		m_table[i].Reset();
	}
	m_nStartNodeIndex = -1;
}
