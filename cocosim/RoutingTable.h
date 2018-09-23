#pragma once
#include "FieldMap.h"
#include <climits>
#include <list>
using namespace std;

class RoutingEntry {
public:
	int m_nDistance;
	int m_nPrevIndex;
	RoutingEntry() {
		Reset();
	}
	void Reset() {
		m_nDistance = INT_MAX;
		int m_nPrevIndex = -1;
	}
};

class RoutingTable
{
public:
	RoutingTable();
	~RoutingTable();

	void SetFieldMap(const FieldMap * fm);
	void GenerateTable(int startIndex, int accessLevel);
	int GetPath(int startIndex, int endIndex, int accessLevel, list<int> & path);
	const RoutingEntry & GetEntry(int i) const { return m_table[i]; }
	CString GetRouteTableString() const;

protected:
	void Clean();
	void GenerateTableTo(int startIndex, int endIndex, int accessLevel);


private:
	const FieldMap * m_pFM;
	vector<RoutingEntry> m_table;
	int m_nStartNodeIndex;

};

