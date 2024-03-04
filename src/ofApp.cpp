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
	gui.add(minAreaRadius_.set("MinAreaRadius", 10.0, 0, 30.0));
	gui.add(maxAreaRadius_.set("MaxAreaRadius", 100, 0, 500));
	gui.add(threshold_.set("Threshold", 128, 0, 255));
	gui.add(gamma_.set("Gamma", 0.5, 0.1, 40));
	gui.add(isCalibMode_.set("Calibration", false));

	cameraImg.allocate(w, h, OF_IMAGE_COLOR);
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
	touchTableTracker_->getCameraImage(cameraImg);
	cameraImg.update();
	cameraImg.draw(0, 0);

	touchTableTracker_->draw();


	// draw FPS
	ofPushStyle();
	ofSetColor(0, 0, 255);
	auto msg = "fps: " + ofToString(ofGetFrameRate(), 0);
	ofDrawBitmapString(msg, 500, 20);
	ofPopStyle();

	// draw GUI
	if (drawGui) {
		gui.draw();
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
