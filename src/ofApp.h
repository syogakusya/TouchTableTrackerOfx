#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

#include "touchTableTracker.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void getWindowSize(int w_, int h_);
	int w, h;

	ofVideoGrabber cam;
	std::unique_ptr<TouchTableThread> touchTableTracker_;
	ofColor targetColor_;

	//Gui部分パラメーター
	ofxPanel gui;
	ofParameter<float> threshold_;
	ofParameter<float> minAreaRadius_;
	ofParameter<float> maxAreaRadius_;
	ofParameter<float> gamma_;
	ofParameter<bool> isCalibMode_;

	bool drawGui = false;
};
