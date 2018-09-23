#pragma once
#include "Agent.h"
#include <list>
using namespace std;

class ShadowSchedule
{
public:
	// INT_MAX stay with leader
	int m_nStartTime;
	list<int> m_path;
	void Clean() { m_path.clear(); m_nStartTime = INT_MAX; }
	ShadowSchedule() : m_nStartTime(INT_MAX) {}
};

class ShadowCastle;

class GatherSchedule
{
public:
	int m_nEndTIme;
	list<pair<int, ShadowCastle *>> m_guards;
	GatherSchedule() :m_nEndTIme(-1) {}
};

class ShadowCastle :
	public Agent
{
public:
	ShadowCastle();
	~ShadowCastle();

	void SetSchedule(int nTime, const list<int> & path);
	virtual void NextStep(int nTimeStamp, int nPhase);
	bool IsFollowing() const { return m_schedule.m_nStartTime == INT_MAX; }
	bool IsStay() const { return m_schedule.m_nStartTime == -1; }
	void SetFollowing();
	void CleanSchedule();
	void SetStay();
	void SetOneStepSchedule(int location, int startTime);
	void CleanAssemble();
	void SetAssembleEndTime();
	bool IsAssembleFinished();
	CString GetName() const;

	int m_nOwnerId;
	ShadowSchedule m_schedule;
	list<int>::const_iterator m_recentStep;
	GatherSchedule m_gather;
	list<pair<int, ShadowCastle *>>::const_iterator m_gatherProcess;
};

