#pragma once
#include <vector>
using namespace std;

typedef enum {
	FM_ILLEGAL = -1,
	FM_NO_WAY,
	FM_HAVE_WAY,
	FM_ACCESS,
	FM_SAFE
} FM_STATE;

class DirectedEdge {
public:
	FM_STATE m_eState;
	int m_nLength;
	DirectedEdge() :m_eState(FM_NO_WAY), m_nLength(INT_MAX) {}
	DirectedEdge(FM_STATE eState, int nLen) :m_eState(eState), m_nLength(nLen) {}
};

class FieldMap
{
public:
	FieldMap();
	~FieldMap();

	int GetNodeCount() const { return m_NodeCount; }
	void SetNodeCount(int n);
	inline DirectedEdge GetConnection(int row, int col) const;
	inline void SetConnection(int row, int col, FM_STATE eState);
	void SetConnectionChangeLength(int row, int col, FM_STATE eState, int nLength);
	void GenerateConnectGraph(int percentage);
	void InitCRing(const vector<int> & ds);
	CString GetString() const;
	int GetDegree(int from);

	static bool Merge(FieldMap & a, FieldMap & b);
	bool MergeFrom(const FieldMap & src);

protected:
	void random_card(int* nums, int len);
	void ConnectTwoParts(int al, int ar, int bl, int br, int * nodeIDs);

private:
	int m_NodeCount;
	vector<DirectedEdge> m_Bitmap;
};

