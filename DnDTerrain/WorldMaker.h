#pragma once
#include "MapCommon.h"
#include "MapRegion.h"
#include <unordered_map>

class WorldMaker
{
public:
	WorldMaker(unsigned long worldSeed);
	~WorldMaker(void);
	void generateLevelOne();
	//vector of points for voronoi generation
	vector<mPoint> points;
	//regions, indexed by order of appearance in qvoronoi output
	unordered_map<int, MapRegion*> regions;
	//Rivers
	unordered_map<pair<int,int>, mRiver*> rivers;

private:
	Perlin *perlGen;
	mt19937 rng;
	normal_distribution<float> dist;

	unsigned long seed;
	int totalpoints;
	//Sets up a grid of points somewhat evenly (but w/ random var) across screen space
	//results stored in *points
	void setupInitialPointGrid();
	//Sets up the polygonizer, passing delauny triangulization
	void setupRegions(string &output);
	void assignPointsToRegions();
	void doRegionElevations(float floor);
	void floodWorld();
	void runOceans(int iterations);
	void runRain(int iterations);
};

