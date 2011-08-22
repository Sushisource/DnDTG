#include "TGApp.h"

TGApp::TGApp(PolycodeView *view) {
	core = new Win32Core(view,XSIZ,YSIZ,false,0,60);	  
	CoreServices::getInstance()->getResourceManager()->addArchive("default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYUP);
	core->getInput()->addEventListener(this, InputEvent::EVENT_MOUSEMOVE);
	screen = new PhysicsScreen();
	title = new ScreenLabel("DnD Terrain Generator",16);
	title->setPosition(-XSIZ/2,-YSIZ/2);
	title->depthTest = false;
	screen->setScreenOffset(XSIZ/2,YSIZ/2);
	worldInit = false;
	randomWorld();
	screen->addChild(title);
}

//Replaces the world with a new random one
void TGApp::randomWorld()
{				
	if(worldInit)
	{
		delete world;
		overlayOff(polOver);
	}
	world = new WorldMaker(boost::lexical_cast<unsigned long, time_t>(time(0)));	
	world->generateLevelOne();	
	worldInit = true;	
	polygonOverlay();	
	/*MapRegion *r = world->regions[100];
    for_each(r->neighbors.begin(), r->neighbors.end(), [&] (int rk)
	{
		world->regions[rk]->myShape->setColor(1,0,0,1);
	});*/
	//pointOverlay();	
}

//Handle events
void TGApp::handleEvent(Event *e) {
	if(e->getDispatcher() == core->getInput()) {
		InputEvent *inputEvent = (InputEvent*)e;		
		switch(e->getEventCode()) {
			case InputEvent::EVENT_KEYUP:
				switch (inputEvent->key) {					
					case KEY_SPACE:
						randomWorld();
					break;
				}
			break;
			case InputEvent::EVENT_MOUSEDOWN:
				float xx = inputEvent->mousePosition.x - XSIZ/2;
				float yy = inputEvent->mousePosition.y - YSIZ/2;
				ScreenEntity *e = screen->getEntityAtPosition(xx,yy);
				if(e == NULL) break;
				e->setColor(1,0,0,1);
				printf("%f",e->bBox.length());				
			break;
		}		
	}
}

void TGApp::pointOverlay()
{
	vector<mPoint> pts = world->points;	
	for each(mPoint p in pts)
	{
		ScreenShape *s = new ScreenShape(ScreenShape::SHAPE_CIRCLE,5,5,3);
		s->setPosition(p.x,p.y);		
		ptOver.push_back(s);
		screen->addChild(s);
	}
}

void TGApp::polygonOverlay()
{
	//We need to build the polygons from points
	for_each(world->regions.begin(), world->regions.end(), [&] (pair<int,MapRegion*> pr)
	{		
		MapRegion* p = pr.second;
		if(!p->edge)
		{
			vector<mPoint> pizzoints = p->sortedPoints();
			mPoint c = p->getCentroid();
			ScreenShape *s = new ScreenShape(ScreenShape::SHAPE_CUSTOM);
			s->setPosition(c.x ,c.y);
			for each(mPoint v in pizzoints)
			{
				s->addShapePoint(v.x - c.x,v.y - c.y);
			}								
			if(p->ocean)
				s->setColor(p->elevation/2,p->elevation/2,p->elevation,1);
			else
				s->setColor(p->elevation,p->elevation,p->elevation,1);			
			s->strokeEnabled = true;
			s->setStrokeColor(1,1,0,1);
			s->setStrokeWidth(1);
			polOver.push_back(s);
			screen->addChild(s);
			p->myShape = s;
			//screen->addCollisionChild(s, PhysicsScreenEntity::ENTITY_MESH);		
		}
	});
}

//Give it a vec of shapes, turns them off.
void TGApp::overlayOff(vector<ScreenShape *> &shapeVec)
{
	for each(ScreenShape *s in shapeVec)
	{
		screen->removeChild(s);
		delete s;
	}
	shapeVec.erase(shapeVec.begin(), shapeVec.end());
}

TGApp::~TGApp() {
	delete world;
	delete title;
	delete screen;	
}

bool TGApp::Update() {
	return core->Update();
}