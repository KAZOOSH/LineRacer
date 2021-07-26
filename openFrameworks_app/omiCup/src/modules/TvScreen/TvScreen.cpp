#include "TvScreen.h"
#include "GPF.h"

namespace ofxModule {
    
	TvScreen::TvScreen(string moduleName):ModuleDrawable("TvScreen",moduleName){
		


		// loading the settings saved in settings.json/"TvScreen"
		color = GPF::colorFromJson(settings["color"]);

		ofBackground(0);
    }
  
    
    
    //------------------------------------------------------------------
	void TvScreen::update() {
		// here could run something ;)
    }

	void TvScreen::draw()
	{
		// draw the received text
		ofSetColor(color);
		ofDrawBitmapString(text, 50, 50);
	}


    
    //------------------------------------------------------------------
	void TvScreen::proceedModuleEvent(ModuleEvent& e) {
		
		// receive the communicator message of address "response"
		if (e.address== "response") {

			// read the answer
			text = e.message["answer"].get<string>();
		}
    }
    
}
