#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class TouchTableTracker : public ofxCv::RectFollower {
protected:
		ofColor color;
		ofVec3f cur, smooth;
		float startedDying;
		ofPolyline all;

public:
	TouchTableTracker()
		:startedDying(0) {
	}
	void setup(const cv::Rect& track);
	void update(const cv::Rect& track);
	void kill();
	void draw();

};

class TouchTableThread : public ofThread 
{
public: 
	TouchTableThread() {
		contourFinder_ = std::make_unique<ofxCv::ContourFinder>();
		tracker_ = std::make_unique <ofxCv::RectTrackerFollower<TouchTableTracker>>();
		reset_Circle();
	};

	~TouchTableThread(){}

	void setCamera(ofVideoGrabber* cam);
	void adjustGamma(cv::Mat& img, float gamma = 1.0);
	//~TouchTableThread();
	void draw();
	void setParam(
		float minAR, 
		float maxAR, 
		float th, 
		float gm);
	ofColor getTargetColor(int x, int y);
	

private:
	ofVideoGrabber* camera = nullptr;//ÉJÉÅÉââfëúéÊìæ
	std::unique_ptr<ofxCv::ContourFinder> contourFinder_;
	std::unique_ptr<ofxCv::RectTrackerFollower<TouchTableTracker>> tracker_;
	cv::Mat img, gray;

	// contourFinder param
	float minAreaRadius;
	float maxAreaRadius;
	float gamma;
	float threshold;

	ofColor targetColor;


	void threadedFunction();

//--Circle Drawing for Perspective--------------------------------------------
public:
	std::vector<ofVec2f>pts_src;

	void reset_Circle();
	void setCalibMode(bool calibMode);
	bool getCalibMode();
	void moveClosestPoint(int x, int y);
	void pickClosestPoint(int x, int y);
	void setCalib();

private:
	bool isCalibMode;
	int pickedCircle;
	ofVec2f pickOffset;
	cv::Mat perspectiveMat;
	cv::Mat resultImg;

	void setPerspective(std::vector<ofVec2f> circles);
	void drawSrcCircle();
};