#include "stdafx.h"
#include "Generate3DCoor.h"
#include <fstream>


Generate3DCoor::Generate3DCoor()
{
	m_yMin = INT_MAX;
	m_yMax = INT_MIN;
}


Generate3DCoor::~Generate3DCoor()
{
}

void Generate3DCoor::GetCoor(int nStart, int nRange, int Scale, const FieldMap & src)
{
	m_coor.resize(src.GetNodeCount());
	m_bc.resize(src.GetNodeCount());
	for (int i = 0; i < src.GetNodeCount(); ++i)
	{
		m_bc[i].m_pSelf = &m_coor[i];
	}
	m_route.SetFieldMap(&src);
	m_route.GenerateTable(nStart, FM_HAVE_WAY);
	int maxLevel = 0;
	for (int i = 0; i < m_coor.size(); ++i)
	{
		if (m_route.GetEntry(i).m_nDistance > maxLevel)
		{
			maxLevel = m_route.GetEntry(i).m_nDistance;
		}
	}
	m_nLevelDistance = nRange / (maxLevel + 1);

	for (int i = 0; i <= maxLevel; ++i)
	{
		GenerateLevel(i, nRange, Scale, src);
	}

	int newRange = (m_yMax - m_yMin) * 2;
	int xRange = newRange / (3 * maxLevel + 1);
	for (int i = 0; i < m_coor.size(); ++i)
	{
		int nLevel = m_route.GetEntry(i).m_nDistance;
		m_coor[i].m_x = rand() % (xRange + 1) + nLevel * (newRange / (maxLevel + 1));
	}

	GenerateEdges(src);
}

void Generate3DCoor::GenerateLevel(int nLevel, int nRange, int Scale, const FieldMap & src)
{
	list<Coor3D *> allSameLevel;
	list<Barycenter*> bcInLevel;
	int smallrange = nRange / 4;
	for (int i = 0; i < m_coor.size(); ++i)
	{
		if (m_route.GetEntry(i).m_nDistance == nLevel)
		{
			allSameLevel.push_back(&m_coor[i]);
			bcInLevel.push_back(&m_bc[i]);
			Coor3D ave;
			int nNum = 0;
			for (int j = 0; j < m_coor.size(); ++j)
			{
				if (src.GetConnection(i, j).m_eState >= FM_HAVE_WAY)
				{
					if (m_route.GetEntry(j).m_nDistance == nLevel - 1)
					{
						m_bc[i].m_centers.push_back(&m_coor[j]);
						ave = ave + m_coor[j];
						++nNum;
					}
				}
			}
			if (nNum > 0)
			{
				ave.m_x /= nNum;
				ave.m_y /= nNum;
				ave.m_z /= nNum;
				m_coor[i] = GetRandomCoor(nLevel*m_nLevelDistance, ave.m_y - smallrange / 2, ave.m_z - smallrange / 2, m_nLevelDistance / 3, smallrange, smallrange);
			}
			else
			{
				m_coor[i] = GetRandomCoor(nLevel*m_nLevelDistance, nRange / 2 - smallrange / 2, nRange / 2 - smallrange / 2, m_nLevelDistance / 3, smallrange, smallrange);
			}
		}
	}

	for (int i = 0; i < 500; i++)
	{
		for (list<Barycenter*>::iterator ptr = bcInLevel.begin(); ptr != bcInLevel.end(); ++ptr)
		{
			(*ptr)->Update(allSameLevel);
			if ((*ptr)->m_pSelf->m_y > m_yMax)
			{
				m_yMax = (*ptr)->m_pSelf->m_y;
			}
			if ((*ptr)->m_pSelf->m_y < m_yMin)
			{
				m_yMin = (*ptr)->m_pSelf->m_y;
			}
		}
	}

}

Coor3D Generate3DCoor::GetRandomCoor(int left, int bottom, int front, int w, int h, int deep)
{
	Coor3D ret;
	ret.m_x = w == 0 ? left : rand() % w + left;
	ret.m_y = h == 0 ? bottom : rand() % h + bottom;
	ret.m_z = deep == 0 ? front : rand() % deep + front;
	return ret;
}

void Generate3DCoor::GenerateEdges(const FieldMap & src)
{
	for (int i = 0; i < m_coor.size(); ++i)
	{
		for (int j = i + 1; j < m_coor.size(); ++j)
		{
			if (src.GetConnection(i, j).m_eState >= FM_HAVE_WAY)
			{
				Edge3D e;
				e.m_a = m_coor[i];
				e.m_b = m_coor[j];
				m_edges.push_back(e);
			}
		}
	}
}

void Generate3DCoor::Output()
{
	ofstream outNode("node.txt");
	for (int i = 0; i < m_coor.size(); ++i)
	{
		outNode << i << " " << m_coor[i].m_x << " " << m_coor[i].m_y << " " << m_coor[i].m_z << endl;
	}
	outNode.close();
	ofstream outEdge("edge.txt");
	for (list<Edge3D>::iterator ptr = m_edges.begin(); ptr != m_edges.end(); ++ptr)
	{
		outEdge << ptr->m_a.m_x << " " << ptr->m_a.m_y << " " << ptr->m_a.m_z << " ";
		outEdge << ptr->m_b.m_x << " " << ptr->m_b.m_y << " " << ptr->m_b.m_z << endl;
	}
	outEdge.close();
}

void Barycenter::Update(const list<Coor3D *> allSameLevel)
{
	Force FRep = GetRepulsion(allSameLevel);
	Force FAtt = GetAttractive();
	Force FCen = GetCenterForce();
	Force FSum = FRep + FAtt + FCen;
	if (abs(FSum.m_y) >= 10)
	{
		m_pSelf->m_y += FSum.m_y / 10;
	}
	else if (abs(FSum.m_y) >= 1)
	{
		if (FSum.m_y > 0)
			m_pSelf->m_y++;
		else
			m_pSelf->m_y--;
	}

	if (abs(FSum.m_z) >= 10)
	{
		m_pSelf->m_z += FSum.m_z / 10;
	}
	else if (abs(FSum.m_z) >= 1)
	{
		if (FSum.m_z > 0)
			m_pSelf->m_z++;
		else
			m_pSelf->m_z--;
	}
}

Force Barycenter::GetRepulsion(const list<Coor3D *> allSameLevel)
{
	Force ret;
	list<Coor3D *>::const_iterator ptr = allSameLevel.cbegin();
	while (ptr != allSameLevel.cend())
	{
		ret = ret + GetRepulsionTo(*ptr);
		++ptr;
	}
	return ret;
}

Force Barycenter::GetAttractive()
{
	Force ret;
	list<Coor3D *>::const_iterator ptr = m_withLine.cbegin();
	while (ptr != m_withLine.cend())
	{
		ret = ret + GetAttractiveTo(*ptr);
		++ptr;
	}
	return ret;
}

Force Barycenter::GetCenterForce()
{
	Force ret;
	list<Coor3D *>::const_iterator ptr = m_centers.cbegin();
	while (ptr != m_centers.cend())
	{
		ret = ret + GetCenterTo(*ptr);
		++ptr;
	}
	return ret;
}

Force Barycenter::GetRepulsionTo(const Coor3D * pAnother)
{
	double strength = GetRepulsionStrength(m_pSelf, pAnother);
	Force ret;
	ret.m_y = m_pSelf->m_y - pAnother->m_y;
	ret.m_z = m_pSelf->m_z - pAnother->m_z;
	double distance = GetDistance(m_pSelf, pAnother);
	ret.m_y /= distance;
	ret.m_z /= distance;
	ret.m_y *= strength;
	ret.m_z *= strength;
	return ret;
}

Force Barycenter::GetAttractiveTo(const Coor3D * pAnother)
{
	Force ret;
	ret.m_y = pAnother->m_y - m_pSelf->m_y;
	ret.m_z = pAnother->m_z - m_pSelf->m_z;
	return ret;
}

Force Barycenter::GetCenterTo(const Coor3D * pAnother)
{
	return GetAttractiveTo(pAnother);
}

double Barycenter::GetDistance(const Coor3D * pa, const Coor3D *pb)
{
	double dy = (pa->m_y - pb->m_y) * (pa->m_y - pb->m_y);
	double dz = (pa->m_z - pb->m_z) * (pa->m_z - pb->m_z);
	double d = sqrt(dy + dz);
	if (d < m_minimumScale)
		return m_minimumScale;
	else
		return d;
}

double Barycenter::GetRepulsionStrength(const Coor3D * pa, const Coor3D *pb)
{
	double distance = GetDistance(pa, pb);
	return pow(m_radius, 3) / distance;
}

double Barycenter::m_radius = 20;

double Barycenter::m_minimumScale = 0.001;

