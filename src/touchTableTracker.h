#pragma once
#include "ofMain.h"
#include "ofxCv.h"
#include "TUIO/TuioServer.h"

class TouchTableTracker : public ofxCv::RectFollower {
protected:
		ofColor color;
		ofVec3f cur, smooth;
		float startedDying, startedNasent, dyingTime, nasenttime;
		ofPolyline trail;

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
		tuioServer_ = std::make_unique<TUIO::TuioServer>();
		tuioServer_->setSourceName("TouchTableTracker");
		tuioServer_->enableObjectProfile(false);
		tuioServer_->enableBlobProfile(false);
		contourFinder_ = std::make_unique<ofxCv::ContourFinder>();
		tracker_ = std::make_unique <ofxCv::RectTrackerFollower<TouchTableTracker>>();
	};

	~TouchTableThread(){}

	void getWindowSize(int w_, int h_);
	void setCamera(ofVideoGrabber* cam);
	void adjustGamma(cv::Mat& img, float gamma = 1.0);
	void draw();
	void setParam(
		float minAR, 
		float maxAR, 
		float th, 
		float gm);
	void getCameraImage(ofImage& image);
	

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

	int w, h;



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

//--TUIO-----------------------------------------------------------------------
public:
	void sendTUIOData();

private:
	std::unique_ptr<TUIO::TuioServer> tuioServer_;
	std::map< int, TUIO::TuioCursor* > cursors_;
};