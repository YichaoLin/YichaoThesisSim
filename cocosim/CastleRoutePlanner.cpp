#include "stdafx.h"
#include "CastleRoutePlanner.h"
#include "RoutingTable.h"


CastleRoutePlanner::CastleRoutePlanner()
{
}


CastleRoutePlanner::~CastleRoutePlanner()
{
}

int CastleRoutePlanner::GetPath(int startIndex, const list<int> keyPoints, const PocketMap & pm, list<int> & outputPath, list<int> & keyOrder)
{
	return GetPathAppro(startIndex, keyPoints, pm, outputPath, keyOrder);
	keyOrder.clear();
	outputPath.clear();
	int shortestDistance = INT_MAX;
	list<int> shortestPath;
	for(list<int>::const_iterator ptrI = keyPoints.cbegin(); ptrI != keyPoints.cend(); ++ptrI)
	{
		PocketMap tmpMap = pm;
		list<int> tmpPath;
		int firstDistance = GetPathBetween(startIndex, *ptrI, tmpMap, tmpPath);
		ASSERT(firstDistance != INT_MAX);

		if (true == PretendToWalkThrough(startIndex, tmpPath, tmpMap))
		{
			firstDistance++;
		}

		int subDistance = 0;
		list<int> followingPath;
		list<int> tmpOrder;
		if (keyPoints.size() > 1) 
		{
			list<int> subKeys;
			for (list<int>::const_iterator ptrJ = keyPoints.cbegin(); ptrJ != keyPoints.cend(); ++ptrJ)
			{
				if (ptrJ == ptrI)
					continue;
				subKeys.push_back(*ptrJ);
			}
			subDistance = GetPath(*ptrI, subKeys, tmpMap, followingPath, tmpOrder);
		}
		if (subDistance + firstDistance < shortestDistance) {
			shortestDistance = subDistance + firstDistance;
			shortestPath = tmpPath;
			shortestPath.splice(shortestPath.end(), followingPath);
			keyOrder.clear();
			keyOrder.push_back(*ptrI);
			keyOrder.splice(keyOrder.end(), tmpOrder);
		}
	}
	if (shortestDistance == INT_MAX) {
		return INT_MAX;
	}
	else {
		outputPath = shortestPath;
		return shortestDistance;
	}
}

int CastleRoutePlanner::GetPathAppro(int startIndex, const list<int> keyPoints, const PocketMap & pm, list<int> & outputPath, list<int> & keyOrder)
{
	int nLength = 0;
	outputPath.clear();
	keyOrder.clear();
	m_appMap = pm;
	InitApproVertices(keyPoints);

	PocketMap normalMap = pm;

	vector<bool> keyVisit;
	keyVisit.resize(keyPoints.size());
	for (int i = 0; i < keyVisit.size(); ++i)
	{
		keyVisit[i] = false;
	}

	int minDistance = INT_MAX;
	list<int> partialPath;
	int nShortestId = -1;
	int nId = 0;
	int nShortestNumber = -1;
	for (list<int>::const_iterator ptr = keyPoints.cbegin(); ptr != keyPoints.cend(); ++ptr)
	{
		list<int> tmpPath;
		int firstDistance = GetPathBetween(startIndex, *ptr, normalMap, tmpPath);
		if (firstDistance < minDistance)
		{
			partialPath = tmpPath;
			minDistance = firstDistance;
			nShortestId = nId;
			nShortestNumber = *ptr;
		}
		++nId;
	}
	if (true == PretendToWalkThrough(startIndex, partialPath, normalMap))
	{
		minDistance++;
	}
	outputPath.splice(outputPath.end(), partialPath);
	keyOrder.push_back(nShortestNumber);
	keyVisit[nShortestId] = true;
	nLength += minDistance;
	while (keyOrder.size() < keyPoints.size())
	{
		minDistance = INT_MAX;
		partialPath.clear();
		nShortestId = -1;
		nId = 0;
		int nLastShortestNumber = nShortestNumber;
		nShortestNumber = -1;
		for (list<int>::const_iterator ptr = keyPoints.cbegin(); ptr != keyPoints.cend(); ++ptr)
		{
			if (keyVisit[nId] == true)
			{
				++nId;
				continue;
			}
			list<int> tmpPath;
			int firstDistance = GetPathBetween(nLastShortestNumber, *ptr, normalMap, tmpPath);
			if (firstDistance < minDistance)
			{
				partialPath = tmpPath;
				minDistance = firstDistance;
				nShortestId = nId;
				nShortestNumber = *ptr;
			}
			++nId;
		}
		if (true == PretendToWalkThrough(nLastShortestNumber, partialPath, normalMap))
		{
			minDistance++;
		}
		outputPath.splice(outputPath.end(), partialPath);
		keyOrder.push_back(nShortestNumber);
		keyVisit[nShortestId] = true;
		nLength += minDistance;
	}
	return nLength;
}

int CastleRoutePlanner::GetPathBetween(int from, int to, const PocketMap & pm, list<int> & path)
{
	if (from == to)
	{
		path.clear();
		return 0;
	}
	RoutingTable rt;
	rt.SetFieldMap(&(pm.m_MapScript));
	int nDistance = rt.GetPath(from, to, FM_ACCESS, path);
	return nDistance;
}

bool CastleRoutePlanner::PretendToWalkThrough(int from, list<int> & path, PocketMap & pm)
{
	bool ret = false;
	if (path.size() == 0) 
	{
		return ret;
	}
	list<int>::iterator pPath = path.begin();
	if (pm.m_MapScript.GetConnection(from, *pPath).m_eState < FM_SAFE)
	{
		ret = true;
	}
	pm.SetSafe(from, *pPath);
	list<int>::iterator pPrev = pPath++;
	while (pPath != path.end()) 
	{
		if (pm.m_MapScript.GetConnection(*pPrev, *pPath).m_eState < FM_SAFE)
		{
			ret = true;
		}
		pm.SetSafe(*pPrev, *pPath);
		pPrev = pPath;
		++pPath;
	}
	return ret;
}

void CastleRoutePlanner::SetAppMapSafe()
{
	for (int i = 0; i < m_appMap.m_MapScript.GetNodeCount(); ++i)
	{
		for (int j = 0; j < m_appMap.m_MapScript.GetNodeCount(); ++j)
		{
			if (m_appMap.m_MapScript.GetConnection(i, j).m_eState == FM_ACCESS)
			{
				m_appMap.m_MapScript.SetConnection(i, j, FM_ACCESS);
			}
		}
	}
}

void CastleRoutePlanner::InitApproVertices(const list<int> keyPoints)
{
	int nNodeCount = m_appMap.m_MapScript.GetNodeCount();
	m_appVertices.resize(nNodeCount * nNodeCount);
	list<int> path;
	int i = 0;
	for (list<int>::const_iterator ptri = keyPoints.cbegin(); ptri != keyPoints.cend(); ++ptri)
	{
		int j = 0;
		for (list<int>::const_iterator ptrj = keyPoints.cbegin(); ptrj != keyPoints.cend(); ++ptrj)
		{
			m_appVertices[i * nNodeCount + j] = GetPathBetween(*ptri, *ptrj, m_appMap, path);
			++j;
		}
		++i;
	}
}
