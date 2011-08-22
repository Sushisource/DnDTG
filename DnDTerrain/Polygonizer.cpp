#include "Polygonizer.h"

vector<mPolygon> Polygonizer::calcGons(vector<mEdge> &edges, vector<mEdge> &vedges)
{	
	unordered_map<mPoint, mPolygon*,mHasher> polygonVerts;	
	vector<mEdge>::iterator vit = vedges.begin();
	//Here we create our map of verticies -> sets of edges
	for each(mEdge e in edges)
	{		
		mPoint a = mPoint(e.x1,e.y1);
		mPoint b = mPoint(e.x2,e.y2);		
		if(polygonVerts.count(a) == 0)						
			polygonVerts[a] = new mPolygon(e,a.x,a.y);	
		if(polygonVerts.count(b) == 0)						
			polygonVerts[b] = new mPolygon(e,b.x,b.y);
		polygonVerts[a]->outs.insert(e);
		polygonVerts[a]->neighb.insert(b);			
		polygonVerts[b]->outs.insert(e);
		polygonVerts[b]->neighb.insert(a);		
		mEdge ve = *vit++;
		polygonVerts[a]->verticies.insert((b+a*-1)*.3);
		polygonVerts[b]->verticies.insert((a+b*-1)*.3);
	}

	//Make copies of the polygons to return
	vector<mPolygon> ret;
	for each(pair<mPoint, mPolygon *> p in polygonVerts)
	{
		ret.push_back(*p.second);
		//Free up that pointer
		delete p.second;
	}
	return ret;
}
