#include "MapRegion.h"

MapRegion::MapRegion(void)
{
	center = mPoint(0,0);	
	asPoint = NULL;
	centerCalced = false;
	edge = false;
	ocean = false;
	rainLevel = 0;
	elevation = 0;
}

void MapRegion::closeToEdge()
{
	//TODO: No magic numbers
#define PADD 10
	if(center.x > XSIZ/2 - PADD || center.x < -XSIZ/2 + PADD)
		edge = true;
	if(center.y > YSIZ/2 - PADD || center.y < -YSIZ/2 + PADD)
		edge = true;
}

bool MapRegion::containsPoint(mPoint p)
{
	int sign = 0;
	int nverts = verticies.size();
	for(int i = 0; i < nverts; i++)
	{
		mPoint a = verticies[i];
		mPoint b = verticies[(i+1)%nverts];
		mPoint affineseg = b + (a*-1);
		mPoint affinepnt = p + (a*-1);
		float k = affineseg.cross(affinepnt);
		int ki = (int)(k / abs(k));
		if(sign == 0)
			sign = (int) k;
		else if(k != sign)
			return false;
	}
	return true;
}

mPoint MapRegion::getCentroid()
{
	if(centerCalced)
		return center;
	mPoint centroid = mPoint(0,0);
	for each(mPoint p in verticies)
	{		
		centroid = centroid + p;
	}
	centroid = centroid * (1.0f/verticies.size());
	center = centroid;
	centerCalced = true;
	closeToEdge();
	return centroid;
}

void MapRegion::addVert(mPoint p)
{		
	p.x = clamp(p.x,-XSIZ,XSIZ);
	p.y = clamp(p.y,-YSIZ,YSIZ);
	verticies.push_back(p);
	centerCalced = false;
}

vector<mPoint> MapRegion::sortedPoints()
{
	pSorter sorter(this->getCentroid());
	sort(verticies.begin(), verticies.end(), sorter);
	return verticies;
}

MapRegion::~MapRegion(void)
{
	delete asPoint;
	//Screen shape deleted in overlayOff
}