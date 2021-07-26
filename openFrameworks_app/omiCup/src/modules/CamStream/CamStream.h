#pragma once

#include "ofMain.h"
#include "ModuleDrawable.h"

namespace ofxModule {

struct Cam{
	string id;
	ofVideoGrabber grabber;
	shared_ptr<ofTexture> tex;
	bool isActive = false;
};

class CamStream : public ModuleDrawable{
	
  public:
	
	  CamStream(string moduleName = "CamStream");
	
    void update();
	void draw();

    
	
protected:
    void proceedModuleEvent(ModuleEvent& e);
    
    private:
		map<string,Cam> cams;
};
}
