#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofBackground(0);

	cam.setup(640, 480);

	touchTableTracker_ = std::make_unique<TouchTableThread>();
	touchTableTracker_->startThread(true);
	

	gui.setup();
	gui.add(minAreaRadius_.set("MinAreaRadius", 10.0, 0, 50.0));
	gui.add(maxAreaRadius_.set("MaxAreaRadius", 100, 0, 500));
	gui.add(threshold_.set("Threshold", 128, 0, 255));
	gui.add(gamma_.set("Gamma", 0.5, 0, 50));
}

//--------------------------------------------------------------
void ofApp::update(){
	cam.update();
	touchTableTracker_->getCamera(&cam);
	touchTableTracker_->setParam(minAreaRadius_, maxAreaRadius_, threshold_, gamma_);
}

//--------------------------------------------------------------
void ofApp::draw() {
	//cam.draw(0, 0);
	touchTableTracker_->draw();

	// draw FPS
	ofSetColor(0, 0, 255);
	auto msg = "fps: " + ofToString(ofGetFrameRate(), 0);
	ofDrawBitmapString(msg, 500, 20);

	// draw GUI
	gui.draw();

	ofTranslate(220, 10);
	ofFill();
	ofSetColor(0);
	ofDrawRectangle(-3, -3, 32 + 6, 32 + 6);
	ofSetColor(targetColor_);
	ofDrawRectangle(0, 0, 32, 32);
}

//--------------------------------------------------------------
void ofApp::exit() {
	touchTableTracker_->stopThread();
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
	targetColor_ = touchTableTracker_->getTargetColor(x, y);
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
