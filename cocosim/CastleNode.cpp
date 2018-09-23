#include "stdafx.h"
#include "CastleNode.h"


CastleNode::CastleNode()
	: m_pPocketMap(NULL)
	, m_bCastleIsCleaning(false)
{
}


CastleNode::~CastleNode()
{
	if (m_pPocketMap) {
		delete m_pPocketMap;
	}
}

void CastleNode::SetAsIntersection()
{
	m_pPocketMap = new PocketMap();
}

void CastleNode::SetOriginalMap(FieldMap * pOriMap)
{
	if (m_pPocketMap)
	{
		m_pPocketMap->SetOriMap(pOriMap);
	}
	m_safeEdges.resize(pOriMap->GetNodeCount());
	for (int i = 0; i < pOriMap->GetNodeCount(); ++i)
	{
		m_safeEdges[i] = pOriMap->GetConnection(m_NodeId, i);
	}
}

bool CastleNode::IsSaftMove(int toLocation)
{
	return m_safeEdges[toLocation].m_eState == FM_SAFE;
}
