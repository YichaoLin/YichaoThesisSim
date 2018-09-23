#pragma once
#include <set>
using namespace std;
class Agent;
class FieldNode
{
public:
	FieldNode();
	virtual ~FieldNode();
	virtual void SetAsIntersection() {}
	set<Agent *> m_Agents;
	int m_NodeId;
};

