#pragma once
#include <vector>
#include "FieldMap.h"
#include "RoutingTable.h"
#include <list>
using namespace std;

class Coor3D;

class Force
{
public:
	double m_y;
	double m_z;
	Force() :m_y(0), m_z(0) {}
	Force(const Force & src) :m_y(src.m_y), m_z(src.m_z) {}
	Force operator+(const Force & src)
	{
		Force ret(*this);
		ret.m_y += src.m_y;
		ret.m_z += src.m_z;
		return ret;
	}
};

class Direction
{
public:
	double m_y;
	double m_z;
};

class Barycenter
{
public:
	Coor3D * m_pSelf;
	list<Coor3D *> m_withLine;
	list<Coor3D *> m_centers;

	void Update(const list<Coor3D *> allSameLevel);
	Force GetRepulsion(const list<Coor3D *> allSameLevel);
	Force GetAttractive();
	Force GetCenterForce();
	Force GetRepulsionTo(const Coor3D * pAnother);
	Force GetAttractiveTo(const Coor3D * pAnother);
	Force GetCenterTo(const Coor3D * pAnother);
	static double GetDistance(const Coor3D * pa, const Coor3D *pb);
	static double GetRepulsionStrength(const Coor3D * pa, const Coor3D *pb);

	static double m_radius;
	static double m_minimumScale;
};

class Coor3D
{
public:
	int m_x;
	int m_y;
	int m_z;
	Coor3D()
	{
		m_x = 0;
		m_y = 0;
		m_z = 0;
	}
	Coor3D & operator=(const Coor3D & other)
	{
		this->m_x = other.m_x;
		this->m_y = other.m_y;
		this->m_z = other.m_z;
		return *this;
	}
	Coor3D operator+(const Coor3D & other)
	{
		Coor3D ret = *this;
		ret.m_x += other.m_x;
		ret.m_y += other.m_y;
		ret.m_z += other.m_z;
		return ret;
	}
};

class Edge3D
{
public:
	Coor3D m_a;
	Coor3D m_b;
};

class Generate3DCoor
{
public:
	Generate3DCoor();
	~Generate3DCoor();

	void GetCoor(int nStart, int nRange, int Scale, const FieldMap & src);
	void Output();

protected:
	void GenerateLevel(int nLevel, int nRange, int Scale, const FieldMap & src);
	Coor3D GetRandomCoor(int left, int bottom, int front, int w, int h, int deep);
	void GenerateEdges(const FieldMap & src);


	vector<Coor3D> m_coor;
	vector<Barycenter> m_bc;
	list<Edge3D> m_edges;
private:
	RoutingTable m_route;
	int m_nLevelDistance;

	int m_yMin;
	int m_yMax;
};

