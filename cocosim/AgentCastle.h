#pragma once
#include "Agent.h"
#include "FieldMap.h"
#include <list>
#include "RoutingTable.h"
#include "Castle.h"
#include <vector>
#include "ShadowCastle.h"
#include "PocketMap.h"
#include <queue>
using namespace std;

typedef enum {
	AGT_STA_INITIAL,
	AGT_STA_FINDING,
	AGT_STA_OCCUPY_GATES,
	AGT_STA_ATTACK,
	AGT_STA_ASSEMBLE,
	AGT_STA_GATHERING,
	AGT_STA_MOVE_TO_ENTRANCE,
	AGT_STA_WAIT_OUTSIDE,
	AGT_STA_WAIT_INSIDE,
	AGT_STA_END
}AGT_STA;

typedef enum {
	AGT_MOVE_STA_TOGETHER,
	AGT_MOVE_STA_FOLLOWING
}AGT_MOVE_STA;

bool cmpSuicideShadow(const ShadowCastle * a, const ShadowCastle * b);

class AgentCastle : public Agent
{
public:
	AgentCastle();
	virtual void Init(SimEngine * pEngine);
	~AgentCastle();

	virtual CString GetName() const;

	void MarkCastles();
	void NextStep(int nTimeStamp, int nPhase);
	void MoveOneStepThroughPath();
	void SwitchToState(AGT_STA eState);

protected:
	void OnPhaseWrite(int nTimeStamp);
	void OnPhaseStrategy(int nTimeStamp);
	void OnPhaseAction(int nTimeStamp);

	void UpdateNodeTargetState();
	bool ReadTargetMarks();
	void WriteFindDecision();

	void RefreshCastleSet();
	Castle * GetAttackTarget();
	Castle * GetWaitingTarget();
	void MarkOriginalRoutes();
	void MarkCastlesFrom(int nNodeIndex, bool * visited, Castle * pCastle);
	bool IsCastleSafe(const Castle * pCastle) const;
	bool DifferentLocationWithFirstShadow();
	bool DifferentLocationShadow(int nShadowIndex);
	void Move(int nToLocation);
	void MoveInGating(int nToLocation);
	void SetPathsInsideCastleSafe(Castle * pCastle);
	void GenerateCastleMap(Castle * pCastle, PocketMap & pm) const;
	int GetTheLastFollowingShadowIndex() const;
	void SetCastleExitWay(const Castle * pcastle, FM_STATE eState);
	static CString GetStateName(AGT_STA eState);
	bool AllShadowsFollowing() const;
	void SetTargetSafe();

	// Already in state
	void OnContinueGates();
	void OnContinueAttack();
	void OnContinueAssemble();
	void OnContinueGathering();
	void OnContinueToEntrance();
	void OnContinueWaitOutside();
	void OnContinueWaitInside();

	// Machine
	void OnStateFinding();
	void OnStateGates();
	void OnStateAttack();
	void OnStateAssemble();
	void OnStateGathering();
	void OnStateToEntrance();
	void OnStateWaitOutside();
	void OnStateWaitInside();

	void ResetPaths();

protected:
	FieldMap m_map;
	RoutingTable m_oriRoute;
	RoutingTable m_curRoute;
	map<int, Castle *> m_castles;
	vector<Castle*> m_NodeMapCastle;

	AGT_STA m_eState;

private:
	Castle * m_pTarget;
	int m_nExpectAttackTime;
	list<int> m_path;
	list<int>::const_iterator m_ptrPath;
	list<int>::const_reverse_iterator m_ptrRevPath;
	list<int> m_entrances;
	list<int>::const_iterator m_ptrEntrance;
	list<int> m_towers;
	list<int>::const_iterator m_ptrTower;
	list<int>::const_reverse_iterator m_ptrRevTower;
	// Castles which can be viewed as targets.
	list<Castle *> m_waitingCastles;
	// Castles which can be viewed as targets, but others are working on them.
	list<Castle *> m_detectingCastles;
	vector<ShadowCastle*> m_shadows;

	map<int, ShadowCastle*> m_deployedTowerAndSuicide;
	map<int, ShadowCastle*> m_deployedEntranceAndGuard;
	list<ShadowCastle*> m_AssembleOrder;

	PocketMap m_handbook;
	int m_nNextLocation;
	int m_nLastLocation;
};

