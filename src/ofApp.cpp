#include "ofApp.h"

void ofApp::getWindowSize(int w_, int h_) {
	w = w_;
	h = h_;
}

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofBackground(0);

	cam.setup(w, h);

	touchTableTracker_ = std::make_unique<TouchTableThread>();
	touchTableTracker_->getWindowSize(w, h);
	touchTableTracker_->reset_Circle();
	touchTableTracker_->startThread(true);
	

	gui.setup();
	//gui.setPosition(480, 20);
	gui.add(minAreaRadius_.set("MinAreaRadius", 30.0, 0, 100.0));
	gui.add(maxAreaRadius_.set("MaxAreaRadius", 400, 0, 1000));
	gui.add(threshold_.set("Threshold", 128, 0, 255));
	gui.add(gamma_.set("Gamma", 0.5, 0, 40));
	gui.add(isCalibMode_.set("Calibration", false));
}

//--------------------------------------------------------------
void ofApp::update(){
	cam.update();
	touchTableTracker_->setCalibMode(isCalibMode_);
	touchTableTracker_->setCamera(&cam);
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
	if (drawGui) {
		gui.draw();
		ofTranslate(220, 10);
		ofFill();
		ofSetColor(0);
		ofDrawRectangle(-3, -3, 32 + 6, 32 + 6);
		ofSetColor(targetColor_);
		ofDrawRectangle(0, 0, 32, 32);
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	touchTableTracker_->stopThread();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'g') {
		drawGui = !drawGui;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (touchTableTracker_->getCalibMode()) {
		touchTableTracker_->moveClosestPoint(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (touchTableTracker_->getCalibMode()) {
		touchTableTracker_->pickClosestPoint(x, y);
	}
	else if(drawGui){
		targetColor_ = touchTableTracker_->getTargetColor(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if (touchTableTracker_->getCalibMode()) {
		touchTableTracker_->setCalib();
	}
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
