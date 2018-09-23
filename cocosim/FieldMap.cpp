#include "stdafx.h"
#include "FieldMap.h"
#include "CastleAlgorithmCommon.h"


FieldMap::FieldMap()
{
	m_NodeCount = 0;
}

FieldMap::~FieldMap()
{
}

void FieldMap::SetNodeCount(int n)
{
	m_NodeCount = n;
	m_Bitmap.resize(n * n);
}

DirectedEdge FieldMap::GetConnection(int row, int col) const
{
	DirectedEdge ret;
	ret.m_eState = FM_ILLEGAL;
	if (row < 0 || m_NodeCount <= row) {
		return ret;
	}
	if (col < 0 || m_NodeCount <= col) {
		return ret;
	}
	return m_Bitmap[row * m_NodeCount + col];
}

void FieldMap::SetConnectionChangeLength(int row, int col, FM_STATE eState, int nLength)
{
	SetConnection(row, col, eState);
	m_Bitmap[row * m_NodeCount + col].m_nLength = nLength;
}

void FieldMap::SetConnection(int row, int col, FM_STATE eState)
{
	if (row < 0 || m_NodeCount <= row) {
		ASSERT(FALSE);
		return;
	}
	if (col < 0 || m_NodeCount <= col) {
		ASSERT(FALSE);
		return;
	}
	m_Bitmap[row * m_NodeCount + col].m_eState = eState;
}

void FieldMap::GenerateConnectGraph(int percentage)
{
	int edgeCount = percentage * m_NodeCount * (m_NodeCount - 1) / 200;
	if (edgeCount < m_NodeCount - 1) {
		edgeCount = m_NodeCount - 1;
	}
	int * nodeIDs = new int[m_NodeCount];
	for (int i = 0; i < m_NodeCount; ++i) {
		nodeIDs[i] = i;
	}
	random_card(nodeIDs, m_NodeCount);

	int ml = (m_NodeCount - 1) / 2, mr = ml + 1;
	ConnectTwoParts(0, ml, mr, m_NodeCount - 1, nodeIDs);
	delete[] nodeIDs;
	nodeIDs = NULL;

	int fullConnectionCount = m_NodeCount * (m_NodeCount - 1) / 2;
	int * connectionIDs = new int[fullConnectionCount];
	for (int i = 0; i < fullConnectionCount; ++i) {
		connectionIDs[i] = i;
	}
	random_card(connectionIDs, fullConnectionCount);
	FM_STATE * connections = new FM_STATE[fullConnectionCount];
	int connIndex = 0;
	for (int i = 0; i < m_NodeCount - 1; ++i) {
		for (int j = i + 1; j < m_NodeCount; ++j) {
			connections[connIndex++] = GetConnection(i, j).m_eState;
		}
	}
	edgeCount -= (m_NodeCount - 1);
	for (int i = 0; i < fullConnectionCount && edgeCount > 0; ++i) {
		if (connections[connectionIDs[i]] == FM_NO_WAY) {
			connections[connectionIDs[i]] = FM_HAVE_WAY;
			--edgeCount;
		}
	}
	delete[] connectionIDs;
	connectionIDs = NULL;
	connIndex = 0;
	for (int i = 0; i < m_NodeCount - 1; ++i) {
		for (int j = i + 1; j < m_NodeCount; ++j) {
			SetConnectionChangeLength(i, j, connections[connIndex++], 1);
		}
	}
	connIndex = 0;
	for (int j = 0; j < m_NodeCount - 1; ++j) {
		for (int i = j + 1; i < m_NodeCount; ++i) {
			SetConnectionChangeLength(i, j, connections[connIndex++], 1);
		}
	}
	delete[] connections;
	connections = NULL;
	GetString();
}

void FieldMap::InitCRing(const vector<int> & ds)
{
	if (m_NodeCount <= ds[ds.size() - 1])
	{
		return;
	}

	for (int i = 0; i < m_NodeCount; ++i)
	{
		for (int j = 0; j < ds.size(); ++j)
		{
			int next = (i + ds[j]) % m_NodeCount;
			SetConnectionChangeLength(i, next, FM_HAVE_WAY, 1);
			SetConnectionChangeLength(next, i, FM_HAVE_WAY, 1);
		}
	}
}

CString FieldMap::GetString() const
{
	CString strLine;
	for (int i = 0; i < m_NodeCount; ++i) {
		strLine.Empty();
		for (int j = 0; j < m_NodeCount; ++j) {
			CString strTmp;
			strTmp.Format(_T("\t%d"), GetConnection(i, j));
			strLine += strTmp;
		}
		strLine += _T("\n");
		OutputDebugString(strLine);
	}
	OutputDebugString(_T("Output edges\n"));
	for (int i = 0; i < m_NodeCount; ++i) {
		for (int j = i + 1; j < m_NodeCount; ++j) {
			if (GetConnection(i, j).m_eState != FM_NO_WAY) {
				CString strTmp;
				strTmp.Format(_T("G.add_edge(%d,%d)\n"), i, j);
				OutputDebugString(strTmp);
			}
		}
	}
	return _T("");
}

int FieldMap::GetDegree(int from)
{
	int ret = 0;
	for (int i = 0; i < m_NodeCount; ++i)
	{
		if (GetConnection(from, i).m_eState > FM_NO_WAY)
		{
			++ret;
		}
	}
	return ret;
}

bool FieldMap::Merge(FieldMap & a, FieldMap & b)
{
	bool ret = a.MergeFrom(b);
	b = a;
	return ret;
}

bool FieldMap::MergeFrom(const FieldMap & src)
{
	bool ret = false;
	ASSERT(m_NodeCount == src.m_NodeCount);
	for (int i = 0; i < m_NodeCount; ++i)
	{
		for (int j = 0; j < m_NodeCount; ++j)
		{
			DirectedEdge de = src.GetConnection(i, j);
			if (de.m_eState > GetConnection(i, j).m_eState)
			{
				SetConnectionChangeLength(i, j, de.m_eState, de.m_nLength);
				ret = true;
			}
		}
	}
	return ret;
}

void FieldMap::random_card(int* nums, int len)
{
	CastleAlgorithmCommon::random_card(nums, len);
}

void FieldMap::ConnectTwoParts(int al, int ar, int bl, int br, int * nodeIDs)
{
	if (al != ar) {
		int ml = (ar + al) / 2;
		int mr = ml + 1;
		ConnectTwoParts(al, ml, mr, ar, nodeIDs);
	}
	if (bl != br) {
		int ml = (br + bl) / 2;
		int mr = ml + 1;
		ConnectTwoParts(bl, ml, mr, br, nodeIDs);
	}
	int indexA = rand() % (ar - al + 1) + al;
	int indexB = rand() % (br - bl + 1) + bl;
	SetConnectionChangeLength(nodeIDs[indexA], nodeIDs[indexB], FM_HAVE_WAY, 1);
	SetConnectionChangeLength(nodeIDs[indexB], nodeIDs[indexA], FM_HAVE_WAY, 1);
}
