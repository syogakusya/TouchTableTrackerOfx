#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

#include "ReactTableFollower.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

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

	ofVideoGrabber cam;//カメラ映像取得
	cv::Mat img;//ガンマ補正用Mat
	ofxCv::ContourFinder contourFinder;
	ofColor targetColor;
	ofxCv::RectTrackerFollower<ReactTableFollower> tracker;


	//Gui部分パラメーター
	ofxPanel gui;
	ofParameter<float> threshold;
	ofParameter<bool> trackHs;
	ofParameter<bool> holes;
	ofParameter<float> minAreaRadius;
	ofParameter<float> maxAreaRadius;
	ofParameter<float> gamma;
};
