#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ReactTableFollower : public ofxCv::RectFollower {
protected:
		ofColor color;
		ofVec3f cur, smooth;
		float startedDying;
		ofPolyline all;

public:
	ReactTableFollower()
		:startedDying(0) {
	}
	void setup(const cv::Rect& track);
	void update(const cv::Rect& track);
	void kill();
	void draw();
};