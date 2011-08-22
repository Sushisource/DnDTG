#pragma once
//#ifdef _DEBUG
//#include <vld.h>
//#endif
#include "PolycodeView.h"
#include "Polycode.h"
#include "Polycode2DPhysics.h"
#include <vector>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <random>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#define XSIZ 650
#define YSIZ 650
#define TILESIZ 15
#define PERTMOD 2.2f
#define PERLSCALE 3000.0f
using namespace std;

//Clamp helper
inline float clamp(float x, float a, float b)
{
	return x < a ? a : (x > b ? b : x);
}

//Point structure
struct mPoint
{
	float x;
	float y;
	mPoint::mPoint()
	{ x = y = 0; }
	mPoint::mPoint(float xx, float yy)
	{ x = xx; y = yy; }
	float cross(mPoint &other)
	{
		return x*other.y-y*other.x;
	}
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
//Point sorter
struct pSorter
{
	mPoint cent;
	pSorter::pSorter(mPoint centroid)
	{ cent = centroid; }
	bool operator() (const mPoint &a, const mPoint &b)
	{
		float anga = atan2(a.y-cent.y, a.x-cent.x);
		float angb = atan2(b.y-cent.y, b.x-cent.x);
		return anga < angb;
	}
};