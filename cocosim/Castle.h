#pragma once
#include <list>
#include "RoutingTable.h"
#include <map>
using namespace std;

class CastleGate {
public:
	int m_outside;
	int m_inside;
	bool m_bHold;
	CastleGate() :m_outside(-1), m_inside(-1), m_bHold(false) {}
	CastleGate(int o, int i) : m_outside(o), m_inside(i), m_bHold(false) {}
};

class CastleChannel {
public:
	int m_theOtherSide;
	bool m_bHold;
	CastleChannel(int theOther) :m_theOtherSide(theOther), m_bHold(false) {}
};

typedef enum {
	CS_UNREACHABLE = 0,
	CS_WAITING_TO_DETECT,
	CS_TARGET,
	CS_OTHERS_DETECTING,
	CS_SAFE,
}CASTEL_STATE;

class Castle
{
public:
	Castle();
	~Castle();

	void Reset();
	void RefreshState(const RoutingTable & table, const FieldMap & fm);
	void AddEntrance(int outside, int inside);
	int GetMinmumShadow() const;
	int GetEntranceRemainGateNumber(int inside) const;
	bool UpdateState(CASTEL_STATE eState);
	CString GetString() const;
	int m_nDistanceToZero;
	CASTEL_STATE m_eState;
	list<int> m_Towers;
	map<int, list<CastleGate *>> m_EntranceFromOutside;
	map<int, list<CastleGate *>> m_EntranceFromInside;
	list<CastleGate> m_Exit;
	int m_nNumofEntrance;
	int m_nNumofExit;
	int m_nID;

	CString GetCastleString() const;

private:
	list<CastleGate> m_allEntranceGates;
};
