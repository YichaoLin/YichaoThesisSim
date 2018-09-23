#pragma once
class SimEngine;

class Agent
{
public:
	Agent();
	virtual void Init(SimEngine * pEngine);
	virtual ~Agent();

	virtual CString GetName() const = 0;

	virtual void NextStep(int nTimeStamp, int nPhase) = 0;
	int GetLocation() const { return m_currentLocation; }
	void SetLocation(int location) { m_currentLocation = location; }
	int GetID() const { return m_nID; }

	static void ResetID() { ms_nAgentID = 0; }
	int m_nLastLocation;

protected:
	int m_currentLocation;
	SimEngine * m_pEngine;
	int m_nID;

private:
	static int ms_nAgentID;
};

