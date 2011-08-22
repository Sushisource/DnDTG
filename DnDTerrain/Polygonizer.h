#pragma once
#include "Polycode.h"
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
using namespace std;
//Point structure
struct mPoint
{
	float x;
	float y;
	mPoint::mPoint()
	{ x = y = 0; }
	mPoint::mPoint(float xx, float yy)
	{ x = xx; y = yy; }
	mPoint operator + (const mPoint &other) const
	{
		return mPoint(x + other.x, y + other.y);
	}
	mPoint operator * (float scalar)
	{
		return mPoint(x*scalar, y*scalar);
	}
	bool operator==(const mPoint &r) const
	{
		if(x == r.x && y == r.y)
			return true;
		return false;
	}
};
//Edge structure
struct mEdge
{
	float x1, x2, y1, y2;
	mEdge::mEdge()
	{ x1 = 0; y1 = 0; x2 = 0; y2 = 0;}
	mEdge::mEdge(float xa, float ya, float xb, float yb)
	{ x1 = xa; y1 = ya; x2 = xb; y2 = yb;}
	bool operator==(const mEdge &r) const
	{ 
		if(x1 == r.x1 && x2 == r.x2 && y1 == r.y1 && y2 == r.y2)
			return true; 
		return false;
	}
	bool intersects(const mEdge &o) const
	{
		float a = y2 - y1;
		float b = x1 - x2;
		float c = a*x1 + b*y1;
		float oa = o.y2 - o.y1;
		float ob = o.x1 - o.x2;
		float oc = oa*x1 + ob*y1;
		float det = a*ob - oa*b;
		if(det == 0)
			return false;
		float ix = ob*c - b*oc/det;
		float iy = a*oc - oa*c/det;
		return min(x1,x2) <= ix && ix <= max(x1,x2) &&
			min(y1,y2) <= iy && iy <= max(y1,y2);
	}
	bool isperpto(const mEdge &o) const
	{
		Vector3 me = Vector2(x2,y2) - Vector2(x1,y1);		
		Vector3 them = Vector2(o.x2,o.y2) - Vector2(o.x1,o.y1);		
		return me.dot(them) == 0.0f;
	}
};
//Hasher
struct mHasher
{
	size_t operator() (mPoint const &p) const {						
		return hash_value(p.x) ^ hash_value(p.y) + 0x9e3779b9;
	}
	size_t operator() (mEdge const &p) const {
		return hash_value(p.x1) ^ hash_value(p.y2) + 0x9e3779b9;
	}
};
//Polygons
struct mPolygon
{	
	unordered_set<mPoint,mHasher> neighb; //Neightboring polygons - accesed through map
	unordered_set<mPoint,mHasher> verticies; //Surrounging voronoi verticies
	mPolygon::mPolygon()
	{}
};
class Polygonizer
{
public:
	static vector<mPolygon> calcGons(vector<mEdge> &dedges, vector<mEdge> &vedges);
};

