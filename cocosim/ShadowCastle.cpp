#include "stdafx.h"
#include "ShadowCastle.h"
#include "CastleAlgorithmCommon.h"
#include "SimEngine.h"
#include "CastleNode.h"


ShadowCastle::ShadowCastle()
	: m_nOwnerId(0)
{
	m_recentStep = m_schedule.m_path.cend();
	m_gatherProcess = m_gather.m_guards.cend();
}


ShadowCastle::~ShadowCastle()
{
}

void ShadowCastle::SetSchedule(int nTime, const list<int> & path)
{
	m_schedule.m_nStartTime = nTime;
	m_schedule.m_path = path;
	m_recentStep = m_schedule.m_path.cbegin();
}

void ShadowCastle::NextStep(int nTimeStamp, int nPhase)
{
	switch (nPhase) {
	case CASTLE_ALG_PHASE_WRITE: {
		// do nothing
		if (!IsFollowing() && m_currentLocation != m_nLastLocation)
		{
			CastleNode * pNode = (CastleNode*)m_pEngine->GetNode(m_currentLocation);
			if (!pNode)
			{
				return;
			}
			pNode->m_safeEdges[m_nLastLocation] = DirectedEdge(FM_SAFE, 1);
			pNode = (CastleNode*)m_pEngine->GetNode(m_nLastLocation);
			if (!pNode)
			{
				return;
			}
			pNode->m_safeEdges[m_currentLocation] = DirectedEdge(FM_SAFE, 1);
		}
		break;
	}
	case CASTLE_ALG_PHASE_STRATEGY: {
		break;
	}
	case CASTLE_ALG_PHASE_ACTION: {
		if (IsFollowing() || IsStay())
		{
			return;
		}
		else
		{
			if (nTimeStamp >= m_schedule.m_nStartTime && m_recentStep != m_schedule.m_path.end()) {
				m_pEngine->MoveAgent(this, *m_recentStep);
				++m_recentStep;
			}
			if (m_gatherProcess != m_gather.m_guards.cend())
			{
				int guardLocation = (m_gatherProcess->second)->GetLocation();

				if (m_gatherProcess->first <= nTimeStamp)
				{
					CastleNode * pNode = (CastleNode *)m_pEngine->GetNode(guardLocation);
					pNode->m_safeEdges[GetLocation()] = DirectedEdge(FM_SAFE, 1);
					m_pEngine->MoveAgent(m_gatherProcess->second, GetLocation());
					++m_gatherProcess;
				}
				else if (m_gatherProcess->first == nTimeStamp + 1)
				{
					CastleNode * pNode = (CastleNode *)m_pEngine->GetNode(GetLocation());
					pNode->m_safeEdges[guardLocation] = DirectedEdge(FM_SAFE, 1);
				}
			}
		}
		break;
	}
	}
}

void ShadowCastle::SetFollowing()
{
	CleanSchedule();
	m_schedule.m_nStartTime = INT_MAX;
}

void ShadowCastle::CleanSchedule()
{
	m_schedule.Clean();
	m_recentStep = m_schedule.m_path.end();
}

void ShadowCastle::SetStay()
{
	CleanSchedule();
	m_schedule.m_nStartTime = -1;
}

void ShadowCastle::SetOneStepSchedule(int location, int startTime)
{
	CleanSchedule();
	m_schedule.m_path.push_back(location);
	m_schedule.m_nStartTime = startTime;
	m_recentStep = m_schedule.m_path.begin();
}

void ShadowCastle::CleanAssemble()
{
	m_gather.m_guards.clear();
	m_gather.m_nEndTIme = 0;
	m_gatherProcess = m_gather.m_guards.end();
}

void ShadowCastle::SetAssembleEndTime()
{
	m_gather.m_nEndTIme = m_pEngine->GetTime() + m_gather.m_guards.size() * 2;
	m_gatherProcess = m_gather.m_guards.begin();
}

bool ShadowCastle::IsAssembleFinished()
{
	return m_gather.m_nEndTIme <= m_pEngine->GetTime();
}

CString ShadowCastle::GetName() const
{
	CString output;
	output.Format(_T("SD_%d_%d"), m_nOwnerId, m_nID - m_nOwnerId);
	return output;
}
