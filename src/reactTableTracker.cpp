#include "reactTableTracker.h"

const float dyingTime = 1;

void ReactTableFollower::setup(const cv::Rect& track) {
	color.setHsb(ofRandom(0, 255), 255, 255);
	cur = ofxCv::toOf(track).getCenter();
	smooth = cur;
}

void ReactTableFollower::update(const cv::Rect& track) {
	cur = ofxCv::toOf(track).getCenter();
	smooth.interpolate(cur, .5);
	all.addVertex(smooth);
}

void ReactTableFollower::kill() {
	float curTime = ofGetElapsedTimef();
	if (startedDying == 0) {
		startedDying = curTime;
	}
	else if (curTime - startedDying > dyingTime) {
		dead = true;
	}
}

void ReactTableFollower ::draw() {
	ofPushStyle();
	float size = 16;
	ofSetColor(255);
	if (startedDying) {
		ofSetColor(ofColor::red);
		size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
	}
	ofNoFill();
	ofDrawCircle(cur, size);
	ofSetColor(color);
	all.draw();
	ofSetColor(255);
	ofDrawBitmapString(ofToString(label), cur);
	ofPopStyle();
}