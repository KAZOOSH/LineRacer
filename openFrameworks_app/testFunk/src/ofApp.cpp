#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToConsole();
	cam1.listDevices();
	cam1.setDeviceID(1);
	cam1.setup(640, 480);
	
	cam2.setDeviceID(2);
	cam2.setup(640, 480);

}

//--------------------------------------------------------------
void ofApp::update(){
	cam1.update();
	cam2.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	cam1.draw(0, 0);
	cam2.draw(640, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
