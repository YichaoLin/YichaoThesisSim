#pragma once
#include <vector>
#include "PocketMap.h"
#include <list>
using namespace std;

class CastleRoutePlanner
{
public:
	CastleRoutePlanner();
	~CastleRoutePlanner();

	int GetPath(int startIndex, const list<int> keyPoints, const PocketMap & pm, list<int> & outputPath, list<int> & keyOrder);
	int GetPathAppro(int startIndex, const list<int> keyPoints, const PocketMap & pm, list<int> & outputPath, list<int> & keyOrder);

protected:
	int GetPathBetween(int from, int to, const PocketMap & pm, list<int> & path);
	bool PretendToWalkThrough(int from, list<int> & path, PocketMap & pm);

	void SetAppMapSafe();
	void InitApproVertices(const list<int> keyPoints);

public:
	vector<int> m_path;
	vector<int> m_appVertices;
	PocketMap m_appMap;
};

