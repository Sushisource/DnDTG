#pragma once
#include "MapCommon.h"

class MapRegion
{
public:
	MapRegion(void);
	~MapRegion(void);
	mPoint getCentroid();
	vector<mPoint> sortedPoints();
	void addVert(mPoint p);
	void setElevation(float e);
	bool containsPoint(mPoint p);
	void addNeighbor(int i);
	void addRiver(int i);
	Vector2* getLocation();

	float elevation;
	float rainLevel;
	vector<int> neighbors; //List of keys to other neigbor regions
	vector<int> rivers; //List of connected rivers by their index
	mPoint location; //Represents the polygon's "true" location
	mPoint* asPoint; //Pointer to the point associated w/ region
	ScreenShape* myShape; //On-screen representation
	bool edge; //Is this an edge region?
	bool ocean; //Is it an ocean?
private:
	vector<mPoint> verticies; //Surrounging voronoi verticies
	bool centerCalced;	
	mPoint center; //Centroid (not set until getCentroid is called)	

	void closeToEdge();
};