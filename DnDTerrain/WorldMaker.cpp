#include "WorldMaker.h"

/***
Author: Spencer Judge

This class presents an interface for generating an entire world on an infinite plane, with three levels of depth resolution.

Level 1: Overworld. At this level we present the entire map within some bounds. We use polygons to calculate the nature
of cerain areas, and when a user clicks one of these polygons, they zoom to level 2
Level 2: Birds-eye. At this level we can see local geography, forests, etc
Level 3: Battle map
***/

//This method is just to help read from
//qhull's output.
std::string exec(const char* cmd) 
{
    FILE* pipe = _popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
                result += buffer;
    }
    _pclose(pipe);	
    return result;
}

WorldMaker::WorldMaker(unsigned long worldSeed)
{
	seed = worldSeed;
	rng.seed(seed);
	perlGen = new Perlin(8,30,1,seed);
	regions = unordered_map<int, MapRegion*>();
	setupInitialPointGrid();
}

//Just a slightly randomized uniform grid
void WorldMaker::setupInitialPointGrid()
{
	for(int x = -XSIZ/2; x < XSIZ/2; x+=TILESIZ)
	{
		for(int y = -YSIZ/2; y < YSIZ/2; y+=TILESIZ)
		{
			mPoint randPert = mPoint(dist(rng),dist(rng))*PERTMOD;
			mPoint p = mPoint(x,y) + randPert;			
			points.push_back(p);
		}
	}
}
//Main method for generating overworld
void WorldMaker::generateLevelOne()
{	
	//First we generate the data file holding the points
	int numpts = points.size();		
	ofstream input("data", ios::out, ios::binary);	
	input << "2\n";
	input << numpts << "\n";
	for each(mPoint p in points)
	{						
		input << p.x << " " << p.y << "\n";
	}
	input.close();
	stringstream cmd;
	cmd << "qvoronoi.exe s o Fv < data";	
	//Run qvoronoi
	string output = exec(cmd.str().data());	
	//Done generating voronoi diagram
	setupRegions(output);	
	doRegionElevations(1);
	floodWorld();
}

//Assigns elevations to the regions based on perlin noise
void WorldMaker::doRegionElevations(float floor)
{	
	for_each(regions.begin(), regions.end(), [&] (pair<int,MapRegion*> pr)
	{		
		if(pr.second->edge == true)
			pr.second->elevation = 0;
		//TODO: Make floor behave properly
		float n = (floor + perlGen->Get(pr.second->location.x, pr.second->location.y))/2;
		pr.second->elevation = n;
    });
}

//Floods the world, first oceans from the borders then lakes
//from imaginary rain
void WorldMaker::floodWorld()
{
	//First we've got to designate where initial ocean tiles are
	//Just the edges in this simple case
	for_each(regions.begin(), regions.end(), [&] (pair<int,MapRegion*> pr)
	{
		MapRegion* r = pr.second;
		//If it's an edge we must assign it's neighbors ocean status
		if(r->edge)
		{
			//For each neighbor
			for_each(r->neighbors.begin(), r->neighbors.end(), [&](int rk)
			{
				//If it isn't also an edge, it's an ocean tile now.
				if(regions.count(rk) > 0 && !regions[rk]->edge)
					regions[rk]->ocean = true;
			});
			r->ocean = false;
		}
	});	

	runOceans(15); //TODO: Parameterize
}

void WorldMaker::runOceans(int iterations)
{
	//For each region
	for_each(regions.begin(), regions.end(), [&] (pair<int,MapRegion*> pr)
	{
		MapRegion* r = pr.second;
		if(r->ocean)
		{
			//If it's an ocean, propagagate ocean status to any lower tile
			for_each(r->neighbors.begin(), r->neighbors.end(), [&](int rk)
			{
				if(regions.count(rk) > 0 && regions[rk]->elevation < r->elevation)
					regions[rk]->ocean = true;
			});
		}
	});
	if(iterations > 0)
		runOceans(iterations - 1);
}

//Parses qvoronoi output
void WorldMaker::setupRegions(string &output)
{
	vector<string> lines;
	vector<string> sizinfo;
	boost::split(lines, output, boost::is_any_of("\n"));	
	lines.pop_back(); //There's an extra line.
	//Split up the second line of the file that has size info
	boost::split(sizinfo, lines[1], boost::is_any_of(" "));
	int numverts = atoi(sizinfo[0].data());
	int numregions = atoi(sizinfo[1].data());
	int offset = 2; //This is where the infinity point is
	int lc = offset+numverts; //Line counter

	//For each region
	for(; lc < numregions+offset+numverts; lc++)
	{
		vector<string> verts;
		boost::split(verts, lines[lc], boost::is_any_of(" "));
		int numrverts = atoi(verts[0].data());
		if(numrverts < 3) continue;
		MapRegion *region = new MapRegion();
		//For each vertex in the region
		for(int vi = 1; vi <= numrverts; vi++)
		{
			int pindex = atoi(verts[vi].data()) + offset;
			if(pindex == offset) continue;
			vector<string> point;			
			boost::split(point, lines[pindex], boost::is_any_of(" "));
			mPoint pt = mPoint(boost::lexical_cast<double>(point[0]),
				boost::lexical_cast<double>(point[1]));
			region->addVert(pt);
		}
		mPoint c = region->getCentroid();
		//We don't like edge regions.
		if(region->edge)				
			region->edge = true;				
		else
			region->edge = false;
		//Set region location to something tolerable for perlin
		c = c * (1.0f/PERLSCALE);
		region->location = c;

		regions[lc-offset-numverts] = region;
	}
	//Skip number of ridges
	lc++;
	//At this point in the parsing we've moved on to the ridges
	//Which help us figure out which regions are neighbors
	for(; lc < lines.size(); lc++) //For each ridge
	{
		vector<string> bits;
		boost::split(bits, lines[lc], boost::is_any_of(" "));
		//Get the two connected sites
		int s1index = atoi(bits[1].data());
		int s2index = atoi(bits[2].data());
		//Add each other to neighbor lists
		if(regions.count(s1index) > 0)			
			regions[s1index]->neighbors.push_back(s2index);
		if(regions.count(s2index) > 0)
			regions[s2index]->neighbors.push_back(s1index);
	}
}

//Destructor
WorldMaker::~WorldMaker(void)
{
	for_each(regions.begin(), regions.end(), [&] (pair<int,MapRegion*> pr)
	{
		delete pr.second;
	});	
	regions.clear();
	delete perlGen;
}
