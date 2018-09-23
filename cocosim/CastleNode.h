#pragma once
#include "FieldNode.h"
#include "PocketMap.h"
#include <map>
#include "Castle.h"
using namespace std;

class Castle;
class Agent;

class AttackTarget
{
public:
	const Castle * m_pTarget;
	int m_nExpectFinishTime;
	const Agent * m_pLeader;
	CASTEL_STATE m_eState;
};

class CastleNode :
	public FieldNode
{
public:
	CastleNode();
	~CastleNode();
	void SetAsIntersection();
	void SetOriginalMap(FieldMap * pOriMap);
	bool IsSaftMove(int toLocation);

	PocketMap * m_pPocketMap;
	vector<DirectedEdge> m_safeEdges;
	map<int, AttackTarget> m_Targets;
	bool m_bCastleIsCleaning;
};

