#include "stdafx.h"
#include "PocketMap.h"


PocketMap::PocketMap()
{
	m_pOriginalMap = NULL;
}


PocketMap::~PocketMap()
{
}

void PocketMap::SetOriMap(const FieldMap * pOri)
{
	m_pOriginalMap = pOri;
	m_MapScript = *pOri;
	int nodeCount = pOri->GetNodeCount();
	m_NodeState.resize(nodeCount);
	for (int i = 0; i < nodeCount; ++i) {
		m_NodeState[i] = ND_STA_UNKNOWN;
	}
}

void PocketMap::SetSafe(int from, int to)
{
	m_NodeState[from] = ND_STA_SAFE;
	m_NodeState[to] = ND_STA_SAFE;
	m_MapScript.SetConnectionChangeLength(from, to, FM_ACCESS, 1);
	m_MapScript.SetConnectionChangeLength(to, from, FM_ACCESS, 1);
}

bool PocketMap::Merge(PocketMap & a, PocketMap & b)
{
	bool ret = a.MergeFrom(b);
	b = a;
	return ret;
}

bool PocketMap::MergeFrom(const PocketMap & src)
{
	bool ret = false;
	// MergeNodes
	ASSERT(m_NodeState.size() == src.m_NodeState.size());
	int nNodeCount = m_NodeState.size();
	for (int i = 0; i < nNodeCount; ++i)
	{
		if (m_NodeState[i] < src.m_NodeState[i])
		{
			if (m_NodeState[i] == ND_STA_UNKNOWN)
			{
				switch (src.m_NodeState[i])
				{
				case ND_STA_SAFE:
				{
					AddOneSafeNode(i);
					break;
				}
				default:
				{
					ASSERT(0);
					break;
				}
				}
			}
			m_NodeState[i] = src.m_NodeState[i];
			ret = true;
		}
	}
	// Merge Map
	m_MapScript.MergeFrom(src.m_MapScript);
	return ret;
}

void PocketMap::AddOneSafeNode(int nNodeId)
{
	int nNodeCount = m_NodeState.size();
	m_NodeState[nNodeId] = ND_STA_SAFE;
	for (int i = 0; i < nNodeCount; ++i)
	{
		if (i == nNodeId || m_NodeState[i] != ND_STA_SAFE)
		{
			continue;
		}
		if (m_MapScript.GetConnection(i, nNodeId).m_eState > FM_NO_WAY)
		{
			m_MapScript.SetConnection(i, nNodeId, FM_SAFE);
		}
		if (m_MapScript.GetConnection(nNodeId, i).m_eState > FM_NO_WAY)
		{
			m_MapScript.SetConnection(nNodeId, i, FM_SAFE);
		}
	}
}
