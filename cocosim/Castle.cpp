#include "stdafx.h"
#include "Castle.h"


Castle::Castle()
	: m_nDistanceToZero(INT_MAX)
	, m_eState(CS_UNREACHABLE)
	, m_nNumofEntrance(0)
	, m_nNumofExit(0)
{
}


Castle::~Castle()
{
}

void Castle::Reset()
{
	m_Towers.clear();
	m_Exit.clear();
}

void Castle::RefreshState(const RoutingTable & table, const FieldMap & fm)
{
	if (m_eState > CS_TARGET)
	{
		return;
	}
	map<int, list<CastleGate *>>::const_iterator ptrEntrance = m_EntranceFromOutside.cbegin();
	while (ptrEntrance != m_EntranceFromOutside.end())
	{
		int nDistance = table.GetEntry(ptrEntrance->first).m_nDistance;
		if (nDistance == INT_MAX)
		{
			return;
		}
		list<CastleGate *>::const_iterator ptrGates = ptrEntrance->second.cbegin();
		while (ptrGates != ptrEntrance->second.cend())
		{
			DirectedEdge de = fm.GetConnection((*ptrGates)->m_outside, (*ptrGates)->m_inside);
			if (de.m_eState < FM_ACCESS)
			{
				return;
			}
			++ptrGates;
		}
		++ptrEntrance;
	}
	m_eState = CS_WAITING_TO_DETECT;
}

void Castle::AddEntrance(int outside, int inside)
{
	m_allEntranceGates.push_back(CastleGate(outside, inside));
	map<int, list<CastleGate *>>::iterator itemOutside = m_EntranceFromOutside.find(outside);
	if (itemOutside != m_EntranceFromOutside.end()) {
		itemOutside->second.push_back(&m_allEntranceGates.back());
	}
	else {
		m_EntranceFromOutside.insert(pair<int, list<CastleGate*>>(outside, list<CastleGate*>()));
		m_EntranceFromOutside[outside].push_back(&m_allEntranceGates.back());
	}

	map<int, list<CastleGate *>>::iterator itemInside = m_EntranceFromInside.find(inside);
	if (itemInside != m_EntranceFromInside.end()) {
		itemInside->second.push_back(&m_allEntranceGates.back());
	}
	else {
		m_EntranceFromInside.insert(pair<int, list<CastleGate*>>(inside, list<CastleGate*>()));
		m_EntranceFromInside[inside].push_back(&m_allEntranceGates.back());
	}
	++m_nNumofEntrance;
}

int Castle::GetMinmumShadow() const
{
	return m_EntranceFromOutside.size() + m_Towers.size() - 1;
}

int Castle::GetEntranceRemainGateNumber(int inside) const
{
	map<int, list<CastleGate *>>::const_iterator itemInside = m_EntranceFromInside.find(inside);
	if (itemInside != m_EntranceFromInside.cend()) {
		list<CastleGate*>::const_iterator ptrGate = itemInside->second.cbegin();
		int numOfGates = itemInside->second.size();
		while (ptrGate != itemInside->second.cend()) {
			if ((*ptrGate)->m_bHold) {
				--numOfGates;
			}
			++ptrGate;
		}
		return numOfGates;
	}
	else {
		ASSERT(false);
	}
	return 0;
}

bool Castle::UpdateState(CASTEL_STATE eState)
{
	if (eState > m_eState)
	{
		m_eState = eState;
		return true;
	}
	return false;
}

CString Castle::GetString() const
{
	CString strOut;
	for (list<int>::const_iterator i = m_Towers.cbegin(); i != m_Towers.cend(); ++i)
	{
		CString strTmp;
		if (i == m_Towers.cbegin())
		{
			strTmp.Format(_T("%d"), *i);
		}
		else
		{
			strTmp.Format(_T(",%d"), *i);
		}
		strOut += strTmp;
	}
	return strOut;
}

CString Castle::GetCastleString() const
{
	CString strOut;
	strOut = _T("Towers: ");
	list<int>::const_iterator ptr = m_Towers.cbegin();
	while (ptr != m_Towers.cend())
	{
		CString strTmp;
		if (ptr == m_Towers.cbegin())
		{
			strTmp.Format(_T("%d"), *ptr);
		}
		else
		{
			strTmp.Format(_T(",%d"), *ptr);
		}
		strOut += strTmp;
		++ptr;
	}
	return strOut;
}
