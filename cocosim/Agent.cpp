#include "stdafx.h"
#include "Agent.h"


Agent::Agent()
	: m_pEngine(NULL)
	, m_currentLocation(0)
	, m_nID(ms_nAgentID++)
	, m_nLastLocation(0)
{
}

void Agent::Init(SimEngine * pEngine)
{
	m_pEngine = pEngine;
}

Agent::~Agent()
{
}

int Agent::ms_nAgentID = 0;
