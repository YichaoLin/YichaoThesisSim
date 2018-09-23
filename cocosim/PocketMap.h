#pragma once
#include "FieldMap.h"
#include <vector>
using namespace std;

typedef enum {
	ND_STA_UNKNOWN = 0,
	ND_STA_SAFE,
	ND_STA_BV
}ND_STA;

class PocketMap
{
public:
	PocketMap();
	~PocketMap();
	void SetOriMap(const FieldMap * pOri);
	void SetSafe(int from, int to);

	static bool Merge(PocketMap & a, PocketMap & b);
	bool MergeFrom(const PocketMap & src);
	void AddOneSafeNode(int nNodeId);

	FieldMap m_MapScript;
	vector<ND_STA> m_NodeState;
	const FieldMap * m_pOriginalMap;
};

