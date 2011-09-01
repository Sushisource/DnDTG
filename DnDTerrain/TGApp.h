#pragma once
#include "WorldMaker.h"

using namespace Polycode;

class TGApp : public EventHandler {
public:
    TGApp(PolycodeView *view);
    ~TGApp();
    
    bool Update();
	void handleEvent(Event *e);
	void randomWorld();
    
private:
    Core *core;
	PhysicsScreen *screen;
	ScreenLabel *title;
	WorldMaker *world;
	vector<ScreenEntity*> polOver; //Regions
	vector<ScreenEntity*> ptOver; //Points
	vector<ScreenEntity*> rvOver; //Rivers
	bool worldInit;
	ScreenEntity* mouseShape;

	void pointOverlay();	
	void polygonOverlay();
	void riverOverlay();
	ScreenEntity* getPolygonAt(int mx, int my);
	void overlayOff(vector<ScreenEntity*> &shapeVec);		
};