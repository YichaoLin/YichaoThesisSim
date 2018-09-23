#include "stdafx.h"
#include "AgentCastle.h"
#include "SimEngine.h"
#include "CastleNode.h"
#include "CastleRoutePlanner.h"
#include "CastleAlgorithmCommon.h"
#include "TaskEntry.h"


AgentCastle::AgentCastle()
	: m_eState(AGT_STA_INITIAL)
{
}

void AgentCastle::Init(SimEngine * pEngine)
{
	Agent::Init(pEngine);
	m_handbook.SetOriMap(&(pEngine->GetOriMap()));
	m_oriRoute.SetFieldMap(&m_handbook.m_MapScript);
	m_curRoute.SetFieldMap(&m_handbook.m_MapScript);
	MarkCastles();

	int nMaxShadow = 1;
	map<int, Castle *>::iterator ptr = m_castles.begin();
	while (ptr != m_castles.end()) {
		int shadowNeed = (*ptr).second->GetMinmumShadow();
		if (shadowNeed > nMaxShadow) {
			nMaxShadow = shadowNeed;
		}
		++ptr;
	}

	// Generate shadows
	m_shadows.resize(nMaxShadow);
	for (int i = 0; i < nMaxShadow; ++i) {
		m_shadows[i] = new ShadowCastle();
		m_shadows[i]->Init(pEngine);
		m_shadows[i]->SetLocation(m_currentLocation);
		m_shadows[i]->m_nOwnerId = m_nID;
		m_pEngine->AddAgent(m_shadows[i]);
	}
	
}

AgentCastle::~AgentCastle()
{

	map<int, Castle *>::iterator ptr = m_castles.begin();
	while (ptr != m_castles.end()) {
		delete (ptr->second);
		++ptr;
	}
}

CString AgentCastle::GetName() const
{
	CString output;
	output.Format(_T("Leader_%d"), m_nID);
	return output;
}

void AgentCastle::MarkCastles()
{
	const int FMCount = m_handbook.m_MapScript.GetNodeCount();
	m_oriRoute.GenerateTable(0, FM_HAVE_WAY);
	//OutputDebugString(m_oriRoute.GetRouteTableString());
	// all edges from closer to farther are accessible, but the distance should be 2.
	MarkOriginalRoutes();

	m_NodeMapCastle.resize(FMCount);
	for (int i = 0; i < FMCount; ++i)
	{
		m_NodeMapCastle[i] = NULL;
	}

	bool * visited = new bool[FMCount];
	memset(visited, 0, sizeof(bool) * FMCount);

	// each node can belong to only one castle.
	for (int i = 0; i < FMCount; ++i) {
		Castle * pNewCastle = new Castle();
		pNewCastle->m_nDistanceToZero = m_oriRoute.GetEntry(i).m_nDistance;
		MarkCastlesFrom(i, visited, pNewCastle);
		if (pNewCastle->m_Towers.size() > 0 && (pNewCastle->m_nNumofEntrance > 1 || pNewCastle->m_Exit.size() == 0)) {
			pNewCastle->m_nID = m_castles.size();
			m_castles[pNewCastle->m_nID] = pNewCastle;
			TaskEntry te;
			te.m_nID = pNewCastle->m_nID;
			te.m_strName = pNewCastle->GetString();
			m_pEngine->RegisterEntry(te);
			m_pEngine->WriteLog(1, GetName() + _T(" gets castles:\n") + pNewCastle->GetCastleString() + _T("\n"));
		}
		else {
			delete pNewCastle;
		}
	}

	// disable all paths leaving castles
	map<int, Castle *>::iterator ptr = m_castles.begin();
	while (ptr != m_castles.end()) {
		SetCastleExitWay(ptr->second, FM_HAVE_WAY);

		list<int>::iterator ptrTower = ptr->second->m_Towers.begin();
		while (ptrTower != ptr->second->m_Towers.end()) {
			m_NodeMapCastle[*ptrTower] = ptr->second;
			++ptrTower;
		}
		++ptr;
	}

	delete[] visited;
}

void AgentCastle::OnPhaseWrite(int nTimeStamp)
{
	// read the current node and merge
	CastleNode * pNode = (CastleNode*)m_pEngine->GetNode(m_currentLocation);
	if (!pNode)
	{
		return;
	}

	if (m_currentLocation != m_nLastLocation)
	{
		pNode->m_safeEdges[m_nLastLocation] = DirectedEdge(FM_SAFE, 1);
	}

	if (pNode->m_pPocketMap)
	{
		PocketMap::Merge(m_handbook, *pNode->m_pPocketMap);
	}

	UpdateNodeTargetState();

	if (m_eState == AGT_STA_ATTACK && GetLocation() == *m_ptrTower)
	{
		pNode->m_bCastleIsCleaning = true;
	}
}

void AgentCastle::OnPhaseStrategy(int nTimeStamp)
{
	// read map from node
	CastleNode * pNode = (CastleNode*)m_pEngine->GetNode(m_currentLocation);
	if (pNode && pNode->m_pPocketMap) 
	{
		m_handbook.MergeFrom(*(pNode->m_pPocketMap));
	}
	m_nNextLocation = GetLocation();
	switch (m_eState) {
	case AGT_STA_INITIAL: {
		SwitchToState(AGT_STA_FINDING);
		break;
	}
	case AGT_STA_FINDING: {
		ASSERT(!DifferentLocationWithFirstShadow());
		break;
	}
	case AGT_STA_OCCUPY_GATES: {
		if (DifferentLocationWithFirstShadow()) {
			m_nNextLocation = m_shadows[0]->GetLocation();
			return;
		}
		else {
			if (!m_shadows[0]->IsFollowing()) {
				m_shadows[0]->SetFollowing();
			}
		}
		OnContinueGates();
		break;
	}
	case AGT_STA_ATTACK: {
		OnContinueAttack();
		break;
	}
	case AGT_STA_ASSEMBLE: {
		OnContinueAssemble();
		break;
	}
	case AGT_STA_GATHERING: {
		if (DifferentLocationWithFirstShadow()) {
			m_nNextLocation = m_shadows[0]->GetLocation();
			return;
		}
		else {
			if (!m_shadows[0]->IsFollowing()) {
				m_shadows[0]->SetFollowing();
			}
		}
		OnContinueGathering();
		break;
	}
	case AGT_STA_MOVE_TO_ENTRANCE: {
		if (DifferentLocationWithFirstShadow()) {
			m_nNextLocation = m_shadows[0]->GetLocation();
			return;
		}
		else {
			if (!m_shadows[0]->IsFollowing()) {
				m_shadows[0]->SetFollowing();
			}
		}
		OnContinueToEntrance();
		break;
	}
	case AGT_STA_WAIT_OUTSIDE: {
		ASSERT(!DifferentLocationWithFirstShadow());
		OnContinueWaitOutside();
		break;
	}
	case AGT_STA_WAIT_INSIDE: {
		ASSERT(!DifferentLocationWithFirstShadow());
		OnContinueWaitInside();
		break;
	}
	}
}

void AgentCastle::OnPhaseAction(int nTimeStamp)
{
	if (GetLocation() == m_nNextLocation)
	{
		return;
	}
	Move(m_nNextLocation);
}

void AgentCastle::UpdateNodeTargetState()
{
	CastleNode * pNode = (CastleNode*)m_pEngine->GetNode(m_currentLocation);
	if (!pNode)
	{
		return;
	}
	if (m_eState == AGT_STA_OCCUPY_GATES)
	{
		if (ReadTargetMarks() == true)
		{
			WriteFindDecision();
		}
	}
	map<int, AttackTarget>::iterator ptr = pNode->m_Targets.begin();
	while (ptr != pNode->m_Targets.end())
	{
		if (m_castles[ptr->first]->m_eState == CS_SAFE)
		{
			ptr->second.m_eState = CS_SAFE;
		}
		else
		{
			if (ptr->second.m_pLeader != this)
			{
				switch (ptr->second.m_eState)
				{
				case CS_TARGET:
				{
					if (m_eState == AGT_STA_OCCUPY_GATES)
					{
						if (ReadTargetMarks() == true)
						{
							WriteFindDecision();
						}
					}
					else
					{
						m_castles[ptr->first]->UpdateState(CS_OTHERS_DETECTING);
					}
					break;
				}
				case CS_SAFE:
				{
					if (m_castles[ptr->first]->m_eState != CS_SAFE)
					{
						SetCastleExitWay(m_castles[ptr->first], FM_ACCESS);
						m_castles[ptr->first]->UpdateState(CS_SAFE);
					}
					break;
				}
				}
			}
		}
		++ptr;
	}
}

bool AgentCastle::ReadTargetMarks()
{
	CastleNode * pNode = (CastleNode*)m_pEngine->GetNode(m_currentLocation);
	map<int, AttackTarget>::iterator ptr = pNode->m_Targets.find(m_pTarget->m_nID);
	if (ptr != pNode->m_Targets.cend())
	{
		if (ptr->second.m_pLeader != this)
		{
			if (ptr->second.m_nExpectFinishTime < m_nExpectAttackTime)
			{
				m_pTarget->UpdateState(CS_OTHERS_DETECTING);
				return false;
			}
			else if (ptr->second.m_nExpectFinishTime == m_nExpectAttackTime)
			{
				if (ptr->second.m_pLeader->GetID() < GetID())
				{
					m_pTarget->UpdateState(CS_OTHERS_DETECTING);
					return false;
				}
			}
		}
	}
	return true;
}

void AgentCastle::WriteFindDecision()
{
	AttackTarget at;
	at.m_nExpectFinishTime = m_nExpectAttackTime;
	at.m_pLeader = this;
	at.m_pTarget = m_pTarget;
	at.m_eState = CS_TARGET;

	CastleNode * pNode = (CastleNode*)m_pEngine->GetNode(m_currentLocation);
	map<int, AttackTarget>::iterator ptr = pNode->m_Targets.find(m_pTarget->m_nID);
	if (ptr != pNode->m_Targets.cend())
	{
		if (ptr->second.m_pLeader != this)
		{
			ptr->second = at;
		}
	}
	else
	{
		pNode->m_Targets[m_pTarget->m_nID] = at;
	}
}

void AgentCastle::SwitchToState(AGT_STA eState)
{
	m_pEngine->WriteLog(1, GetName() + _T(" switches to ") + GetStateName(eState) + _T("\n"));

	m_eState = eState;
	switch (m_eState) {
	case AGT_STA_FINDING: {
		OnStateFinding();
		break;
	}
	case AGT_STA_OCCUPY_GATES: {
		OnStateGates();
		break;
	}
	case AGT_STA_ATTACK: {
		OnStateAttack();
		break;
	}
	case AGT_STA_ASSEMBLE: {
		OnStateAssemble();
		break;
	}
	case AGT_STA_GATHERING: {
		OnStateGathering();
		break;
	}
	case AGT_STA_MOVE_TO_ENTRANCE: {
		OnStateToEntrance();
		break;
	}
	case AGT_STA_WAIT_OUTSIDE: {
		OnStateWaitOutside();
		break;
	}
	case AGT_STA_WAIT_INSIDE: {
		OnStateWaitInside();
		break;
	}
	}
}

void AgentCastle::RefreshCastleSet()
{
	m_curRoute.GenerateTable(m_currentLocation, FM_ACCESS);
	//OutputDebugString(m_curRoute.GetRouteTableString() + _T("\n"));

	map<int, Castle*>::iterator ptr = m_castles.begin();
	while (ptr != m_castles.end()) {
		ptr->second->RefreshState(m_curRoute, m_handbook.m_MapScript);
		++ptr;
	}
	m_waitingCastles.clear();
	m_detectingCastles.clear();
	ptr = m_castles.begin();
	while (ptr != m_castles.end()) {
		if (ptr->second->m_eState == CS_WAITING_TO_DETECT) {
			m_waitingCastles.push_back(ptr->second);
		}
		else {
			if (ptr->second->m_eState == CS_OTHERS_DETECTING) {
				m_detectingCastles.push_back(ptr->second);
			}
		}
		++ptr;
	}

	if (m_detectingCastles.size() > 0) {
		int r = rand() % m_detectingCastles.size();
		list<Castle *>::iterator pW = m_detectingCastles.begin();
		while (pW != m_detectingCastles.end() && r > 0) {
			++pW;
		}
	}
}

Castle * AgentCastle::GetAttackTarget()
{
	if (m_waitingCastles.size() == 0) {
		return NULL;
	}
	list<Castle*>::iterator ptr = m_waitingCastles.begin();
	int r = rand() % m_waitingCastles.size();
	while (r > 0) {
		++ptr;
		--r;
	}
	return *ptr;
}

Castle * AgentCastle::GetWaitingTarget()
{
	if (m_detectingCastles.size() == 0) {
		return NULL;
	}
	list<Castle*>::iterator ptr = m_detectingCastles.begin();
	int r = rand() % m_detectingCastles.size();
	while (r > 0) {
		++ptr;
		--r;
	}
	return *ptr;
}

void AgentCastle::NextStep(int nTimeStamp, int nPhase)
{
	switch (nPhase) {
	case CASTLE_ALG_PHASE_WRITE: {
		OnPhaseWrite(nTimeStamp);
		break;
	}
	case CASTLE_ALG_PHASE_STRATEGY: {
		OnPhaseStrategy(nTimeStamp);
		break;
	}
	case CASTLE_ALG_PHASE_ACTION: {
		OnPhaseAction(nTimeStamp);
		break;
	}
	}
}

void AgentCastle::MoveOneStepThroughPath()
{

}

void AgentCastle::MarkOriginalRoutes()
{
	const int FMCount = m_handbook.m_MapScript.GetNodeCount();
	for (int i = 0; i < FMCount; ++i) {
		for (int j = 0; j < FMCount; ++j) {
			if (m_handbook.m_MapScript.GetConnection(i, j).m_eState == FM_HAVE_WAY) {
				if (m_oriRoute.GetEntry(i).m_nDistance == m_oriRoute.GetEntry(j).m_nDistance - 1) {
					m_handbook.m_MapScript.SetConnection(i, j, FM_ACCESS);
				}
			}
		}
	}
}

void AgentCastle::MarkCastlesFrom(int nNodeIndex, bool * visited, Castle * pCastle)
{
	if (visited[nNodeIndex] == true) {
		return;
	}
	pCastle->m_Towers.push_back(nNodeIndex);
	visited[nNodeIndex] = true;

	const int FMCount = m_handbook.m_pOriginalMap->GetNodeCount();
	int nDistance = m_oriRoute.GetEntry(nNodeIndex).m_nDistance;
	for (int i = 0; i < FMCount; ++i) {
		if (i == nNodeIndex) {
			continue;
		}
		if (m_handbook.m_pOriginalMap->GetConnection(nNodeIndex, i).m_eState < FM_HAVE_WAY) {
			continue;
		}
		if (m_oriRoute.GetEntry(i).m_nDistance == nDistance) {
			MarkCastlesFrom(i, visited, pCastle);
		}
		else {
			if (m_oriRoute.GetEntry(i).m_nDistance == nDistance - 1) {
				pCastle->AddEntrance(i, nNodeIndex);
			}
			else {
				if (m_oriRoute.GetEntry(i).m_nDistance == nDistance + 1) {
					pCastle->m_Exit.push_back(CastleGate(i, nNodeIndex));
				}
				else {
					ASSERT(FALSE);
				}
			}
		}
	}
}

bool AgentCastle::IsCastleSafe(const Castle * pCastle) const
{
	list<int>::const_iterator ptr = pCastle->m_Towers.cbegin();
	while (ptr != pCastle->m_Towers.cend())
	{
		if (m_handbook.m_NodeState[*ptr] != ND_STA_SAFE)
		{
			return false;
		}
		++ptr;
	}
	return true;
}

bool AgentCastle::DifferentLocationWithFirstShadow()
{
	return DifferentLocationShadow(0);
}

bool AgentCastle::DifferentLocationShadow(int nShadowIndex)
{
	return m_currentLocation != m_shadows[nShadowIndex]->GetLocation();
}

void AgentCastle::Move(int nToLocation)
{
	m_handbook.SetSafe(GetLocation(), nToLocation);
	for (int i = 0; i < m_shadows.size(); ++i) {
		if (m_shadows[i]->m_schedule.m_nStartTime == INT_MAX && m_shadows[i]->GetLocation() == GetLocation()) {
			m_pEngine->MoveAgent(m_shadows[i], nToLocation);
		}
	}

	CastleNode * pNode = (CastleNode *)m_pEngine->GetNode(GetLocation());
	pNode->m_safeEdges[nToLocation] = DirectedEdge(FM_SAFE, 1);

	m_handbook.AddOneSafeNode(nToLocation);
	m_pEngine->MoveAgent(this, nToLocation);
}

void AgentCastle::MoveInGating(int nToLocation)
{
	if (m_handbook.m_NodeState[nToLocation] == ND_STA_SAFE)
	{
		m_nNextLocation = *m_ptrPath++;
	}
	else
	{
		ASSERT(m_shadows[0]->IsFollowing());
		m_shadows[0]->m_schedule.Clean();
		m_shadows[0]->m_schedule.m_nStartTime = m_pEngine->GetTime();
		while (m_ptrPath != m_path.end()) {
			m_shadows[0]->m_schedule.m_path.push_back(*m_ptrPath);
			if (*m_ptrPath == *m_ptrEntrance)
			{
				++m_ptrPath;
				break;
			}
			++m_ptrPath;
		}
		m_shadows[0]->m_recentStep = m_shadows[0]->m_schedule.m_path.begin();
	}
}

void AgentCastle::SetPathsInsideCastleSafe(Castle * pCastle)
{
	list<int>::const_iterator ptrTower = pCastle->m_Towers.cbegin();

	list<int> relativeNodes = pCastle->m_Towers;
	map<int, list<CastleGate *>>::const_iterator ptrEntrance = pCastle->m_EntranceFromOutside.cbegin();
	while (ptrEntrance != pCastle->m_EntranceFromOutside.cend())
	{
		relativeNodes.push_back(ptrEntrance->first);
		++ptrEntrance;
	}

	list<int>::const_iterator ptrRela = relativeNodes.cbegin();
	while (ptrRela != relativeNodes.cbegin())
	{
		m_handbook.AddOneSafeNode(*ptrRela);
		++ptrRela;
	}
}

void AgentCastle::GenerateCastleMap(Castle * pCastle, PocketMap & pm) const
{
	list<int>::const_iterator ptrTower = pCastle->m_Towers.cbegin();

	list<int> relativeNodes = pCastle->m_Towers;
	map<int, list<CastleGate *>>::const_iterator ptrEntrance = pCastle->m_EntranceFromOutside.cbegin();
	while (ptrEntrance != pCastle->m_EntranceFromOutside.cend())
	{
		relativeNodes.push_back(ptrEntrance->first);
		++ptrEntrance;
	}

	pm.m_MapScript.SetNodeCount(m_handbook.m_MapScript.GetNodeCount());
	pm.m_NodeState.resize(m_handbook.m_MapScript.GetNodeCount());
	list<int>::const_iterator ptrRela = relativeNodes.cbegin();
	while (ptrRela != relativeNodes.cend())
	{
		pm.m_NodeState[*ptrRela] = ND_STA_SAFE;
		list<int>::const_iterator ptrNxt = relativeNodes.cbegin();
		while (ptrNxt != relativeNodes.cend())
		{
			if (m_handbook.m_MapScript.GetConnection(*ptrRela, *ptrNxt).m_eState > FM_NO_WAY)
			{
				pm.m_MapScript.SetConnectionChangeLength(*ptrRela, *ptrNxt, FM_SAFE, 1);
			}
			++ptrNxt;
		}
		++ptrRela;
	}
}

int AgentCastle::GetTheLastFollowingShadowIndex() const
{
	for (int i = m_shadows.size() - 1; i >= 0; --i)
	{
		if (m_shadows[i]->IsFollowing())
		{
			return i;
		}
	}
	return -1;
}

void AgentCastle::SetCastleExitWay(const Castle * pcastle, FM_STATE eState)
{
	list<CastleGate>::const_iterator ptrGate = pcastle->m_Exit.cbegin();
	while (ptrGate != pcastle->m_Exit.cend()) {
		m_handbook.m_MapScript.SetConnection(ptrGate->m_inside, ptrGate->m_outside, eState);
		++ptrGate;
	}
}

CString AgentCastle::GetStateName(AGT_STA eState)
{
	switch (eState)
	{
	case AGT_STA_INITIAL:
		return _T("AGT_STA_INITIAL");
	case AGT_STA_FINDING:
		return _T("AGT_STA_FINDING");
	case AGT_STA_OCCUPY_GATES:
		return _T("AGT_STA_OCCUPY_GATES");
	case AGT_STA_ATTACK:
		return _T("AGT_STA_ATTACK");
	case AGT_STA_ASSEMBLE:
		return _T("AGT_STA_ASSEMBLE");
	case AGT_STA_GATHERING:
		return _T("AGT_STA_GATHERING");
	case AGT_STA_MOVE_TO_ENTRANCE:
		return _T("AGT_STA_MOVE_TO_ENTRANCE");
	case AGT_STA_WAIT_OUTSIDE:
		return _T("AGT_STA_WAIT_OUTSIDE");
	case AGT_STA_WAIT_INSIDE:
		return _T("AGT_STA_WAIT_INSIDE");
	case AGT_STA_END:
		return _T("AGT_STA_END");
	}
}

bool AgentCastle::AllShadowsFollowing() const
{
	for (int i = 0; i < m_shadows.size(); ++i)
	{
		if (!m_shadows[i]->IsFollowing())
		{
			return false;
		}
	}
	return true;
}

void AgentCastle::SetTargetSafe()
{
	if (m_pTarget->m_eState == CS_SAFE)
	{
		return;
	}
	SetCastleExitWay(m_pTarget, FM_ACCESS);
	m_pTarget->UpdateState(CS_SAFE);
}

void AgentCastle::OnContinueAttack()
{
	if (m_pEngine->GetTime() <= m_nExpectAttackTime)
	{
		return;
	}
	if (GetLocation() == *m_ptrTower)
	{
// 		CastleNode * pNode = (CastleNode*)m_pEngine->GetNode(GetLocation());
// 		pNode->m_bCastleIsCleaning = true;
		ShadowCastle * pSuicide = m_deployedTowerAndSuicide[*m_ptrTower];
		pSuicide->CleanAssemble();
		map<int, list<CastleGate *>>::const_iterator pFind = m_pTarget->m_EntranceFromInside.find(*m_ptrTower);
		ASSERT(pFind != m_pTarget->m_EntranceFromInside.end());
		list<CastleGate *>::const_iterator pGate = pFind->second.cbegin();
		while (pGate != pFind->second.cend())
		{
			map<int, ShadowCastle *>::iterator ptrGuard = m_deployedEntranceAndGuard.find((*pGate)->m_outside);
			if (ptrGuard != m_deployedEntranceAndGuard.end())
			{
				int gatherTime = pSuicide->m_gather.m_guards.size() * 2 + 1 + m_pEngine->GetTime();
				pSuicide->m_gather.m_guards.push_back(pair<int, ShadowCastle *>(gatherTime, ptrGuard->second));
				m_deployedEntranceAndGuard.erase(ptrGuard);
			}
			++pGate;
		}
		pSuicide->SetAssembleEndTime();
		m_AssembleOrder.push_back(pSuicide);
		++m_ptrTower;
	}
	if (m_ptrTower == m_towers.end())
	{
		m_AssembleOrder.sort(cmpSuicideShadow);
		SetTargetSafe();
		SetCastleExitWay(m_pTarget, FM_ACCESS);
		SwitchToState(AGT_STA_ASSEMBLE);
	}
	else
	{
		m_nNextLocation = *m_ptrPath;
		++m_ptrPath;
	}
}

void AgentCastle::OnContinueAssemble()
{
	CastleNode * pNode = (CastleNode*)m_pEngine->GetNode(m_currentLocation);
	pNode->m_bCastleIsCleaning = false;

	if (GetLocation() == *m_ptrRevTower)
	{
		ShadowCastle * pSuicideDeployHere = m_deployedTowerAndSuicide[GetLocation()];
		if (pSuicideDeployHere->IsFollowing())
		{
			m_nNextLocation = *m_ptrRevPath;
			++m_ptrRevPath;
		}
		else
		{
			if (pSuicideDeployHere->IsAssembleFinished())
			{
				for (int i = m_shadows.size() - 1; i >= 0; --i)
				{
					if (!DifferentLocationShadow(i) && !m_shadows[i]->IsFollowing())
					{
						m_shadows[i]->SetFollowing();
					}
				}
				++m_ptrRevTower;
				if (m_ptrRevTower == m_towers.crend())
				{
					SwitchToState(AGT_STA_FINDING);
				}
				else
				{
					m_nNextLocation = *m_ptrRevPath;
					++m_ptrRevPath;
				}
			}
		}
	}
	else
	{
		m_nNextLocation = *m_ptrRevPath;
		++m_ptrRevPath;
	}
}

void AgentCastle::OnContinueGathering()
{
	for (int i = 0; i < m_shadows.size(); ++i)
	{
		if (!m_shadows[i]->IsFollowing() && !DifferentLocationShadow(i))
		{
			m_shadows[i]->SetFollowing();
		}
	}
	if (m_ptrRevPath == m_path.crend() || AllShadowsFollowing())
	{
		SwitchToState(AGT_STA_FINDING);
	}
	else
	{
		m_nNextLocation = *m_ptrRevPath++;
	}
}

void AgentCastle::OnContinueToEntrance()
{
	if (m_ptrPath != m_path.end())
	{
		CastleNode * pNode = (CastleNode *)m_pEngine->GetNode(GetLocation());
		if (pNode->m_safeEdges[*m_ptrPath].m_eState == FM_SAFE)
		{
			m_nNextLocation = *m_ptrPath++;
		}
	}
	else
	{
		SwitchToState(AGT_STA_WAIT_OUTSIDE);
	}
}

void AgentCastle::OnContinueWaitOutside()
{
	if (m_pTarget->m_EntranceFromInside.find(GetLocation()) != m_pTarget->m_EntranceFromInside.end())
	{
		SwitchToState(AGT_STA_WAIT_INSIDE);
	}
	else
	{
		CastleNode * pNode = (CastleNode*)m_pEngine->GetNode(m_currentLocation);
		map<int, list<CastleGate *>>::const_iterator ptr = m_pTarget->m_EntranceFromOutside.find(GetLocation());
		if (pNode->IsSaftMove((*(ptr->second.begin()))->m_inside))
		{
			m_nNextLocation = (*(ptr->second.begin()))->m_inside;
		}
	}
}

void AgentCastle::OnContinueWaitInside()
{
	CastleNode * pNode = (CastleNode*)m_pEngine->GetNode(m_currentLocation);
	if (pNode->m_bCastleIsCleaning == false)
	{
		SetTargetSafe();
		SwitchToState(AGT_STA_FINDING);
	}
}

void AgentCastle::OnStateFinding()
{
	ResetPaths();
	UpdateNodeTargetState();
	RefreshCastleSet();
	if (m_waitingCastles.size() > 0) {
		m_pTarget = GetAttackTarget();
		SwitchToState(AGT_STA_OCCUPY_GATES);
		WriteFindDecision();
		m_pEngine->WriteLog(1, GetName() + _T(" Attack castle ") + m_pTarget->GetString() + _T("\n"));
	}
	else if (m_detectingCastles.size() > 0) {
		m_pTarget = GetWaitingTarget();
		SwitchToState(AGT_STA_MOVE_TO_ENTRANCE);
		m_pEngine->WriteLog(1, GetName() + _T(" Go entrance of castle ") + m_pTarget->GetString() + _T("\n"));
	}
	else {
		m_pTarget = NULL;
		SwitchToState(AGT_STA_END);
		m_pEngine->SetEnd();
		m_pEngine->WriteLog(1, GetName() + _T(" OVER\n"));
	}
}

void AgentCastle::OnStateGates()
{
	CastleRoutePlanner planner;
	list<int> entrances;
	for (map<int, list<CastleGate *>>::const_iterator ptrEntrance = m_pTarget->m_EntranceFromOutside.cbegin(); ptrEntrance != m_pTarget->m_EntranceFromOutside.cend(); ++ptrEntrance)
	{
		entrances.push_back(ptrEntrance->first);
	}
	m_nExpectAttackTime = planner.GetPath(m_currentLocation, entrances, m_handbook, m_path, m_entrances) + m_pEngine->GetTime();
	m_ptrPath = m_path.cbegin();
	m_path.push_front(GetLocation());
	m_ptrEntrance = m_entrances.cbegin();
	m_deployedEntranceAndGuard.clear();
	m_deployedTowerAndSuicide.clear();

	OnContinueGates();
}

void AgentCastle::OnStateAttack()
{
	m_pEngine->WriteLog(1, GetName() + _T(" starts Attack\n"));
	CastleRoutePlanner planner;
	PocketMap castleMap;
	SetPathsInsideCastleSafe(m_pTarget);
	GenerateCastleMap(m_pTarget, castleMap);
	m_path.clear();
	m_towers.clear();
	planner.GetPath(GetLocation(), m_pTarget->m_Towers, castleMap, m_path, m_towers);
	m_ptrPath = m_path.begin();
	m_ptrTower = m_towers.begin();
	OnContinueAttack();
}

void AgentCastle::OnStateAssemble()
{
	CString strComment;
	strComment.Format(_T("Finished By %s"), GetName());
	m_pEngine->SetFinished(m_pTarget->m_nID, strComment);
	SetTargetSafe();
	UpdateNodeTargetState();
	m_pEngine->WriteLog(1, GetName() + _T(" starts Assemble\n"));
	m_ptrRevPath = m_path.crbegin();
	m_ptrRevTower = m_towers.crbegin();
	OnContinueAssemble();
}

void AgentCastle::OnStateGathering()
{
	if (m_ptrPath == m_path.cend())
	{
		m_ptrRevPath = m_path.crbegin();
	}
	else
	{
		list<int>::const_reverse_iterator reptr(m_ptrPath);
		m_ptrRevPath = reptr;
	}
	++m_ptrRevPath;
	OnContinueGathering();
}

void AgentCastle::OnStateToEntrance()
{
	RoutingTable rt;
	rt.SetFieldMap(&m_handbook.m_MapScript);
	rt.GenerateTable(GetLocation(), FM_ACCESS);
	map<int, list<CastleGate *>>::const_iterator ptr = m_pTarget->m_EntranceFromOutside.cbegin();
	int closestEntrance = -1;
	int closestDistance = INT_MAX;
	while (ptr != m_pTarget->m_EntranceFromOutside.cend())
	{
		if (rt.GetEntry(ptr->first).m_nDistance < closestDistance)
		{
			closestDistance = rt.GetEntry(ptr->first).m_nDistance;
			closestEntrance = ptr->first;
		}
		++ptr;
	}
	m_path.clear();
	rt.GetPath(GetLocation(), closestEntrance, FM_ACCESS, m_path);
	m_ptrPath = m_path.cbegin();
	OnContinueToEntrance();
}

void AgentCastle::OnStateWaitOutside()
{
	OnContinueWaitOutside();
}

void AgentCastle::OnStateWaitInside()
{
	OnContinueWaitInside();
}

void AgentCastle::ResetPaths()
{
	m_path.clear();
	m_entrances.clear();
	m_towers.clear();
	m_ptrPath = m_path.cbegin();
	m_ptrRevPath = m_path.crbegin();
	m_ptrEntrance = m_entrances.cbegin();
	m_ptrTower = m_towers.cbegin();
	m_ptrRevTower = m_towers.crbegin();
}

void AgentCastle::OnContinueGates()
{
	if (ReadTargetMarks() == false)
	{
		SwitchToState(AGT_STA_GATHERING);
		return;
	}
	if (*m_ptrEntrance == GetLocation())
	{
		// deploy suicide
		map<int, list<CastleGate *>>::iterator pFind = m_pTarget->m_EntranceFromOutside.find(GetLocation());
		ASSERT(pFind != m_pTarget->m_EntranceFromOutside.end());
		list<CastleGate *>::iterator pGates = pFind->second.begin();
		while (pGates != pFind->second.end())
		{
			int nTowerIndex = (*pGates)->m_inside;
			int nRemain = m_pTarget->GetEntranceRemainGateNumber(nTowerIndex);
			if (nRemain == 1)
			{
				int lastShadowIndex = GetTheLastFollowingShadowIndex();
				ASSERT(lastShadowIndex != -1);
				m_shadows[lastShadowIndex]->SetOneStepSchedule(nTowerIndex, m_nExpectAttackTime);
				m_deployedTowerAndSuicide[nTowerIndex] = m_shadows[lastShadowIndex];
			}
			(*pGates)->m_bHold = true;
			++pGates;
		}

		//deploy guard
		list<int>::const_iterator ptrentrance = m_ptrEntrance;
		++ptrentrance;
		if (ptrentrance != m_entrances.cend())
		{
			int lastShadowIndex = GetTheLastFollowingShadowIndex();
			m_shadows[lastShadowIndex]->SetStay();
			m_deployedEntranceAndGuard[*m_ptrEntrance] = m_shadows[lastShadowIndex];
		}

		++m_ptrEntrance;
	}
	if (m_ptrEntrance == m_entrances.end())
	{
		SwitchToState(AGT_STA_ATTACK);
	}
	else
	{
		MoveInGating(*m_ptrPath);
	}
}

bool cmpSuicideShadow(const ShadowCastle * a, const ShadowCastle * b)
{
	return a->m_gather.m_nEndTIme > b->m_gather.m_nEndTIme;
}
