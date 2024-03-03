#include "touchTableTracker.h"

const float dyingTime = 1;

void TouchTableTracker::setup(const cv::Rect& track) {
	color.setHsb(ofRandom(0, 255), 255, 255);
	cur = ofxCv::toOf(track).getCenter();
	smooth = cur;
}

void TouchTableTracker::update(const cv::Rect& track) {
	cur = ofxCv::toOf(track).getCenter();
	smooth.interpolate(cur, .5);
	all.addVertex(smooth);
}

void TouchTableTracker::kill() {
	float curTime = ofGetElapsedTimef();
	if (startedDying == 0) {
		startedDying = curTime;
	}
	else if (curTime - startedDying > dyingTime) {
		dead = true;
	}
}

void TouchTableTracker ::draw() {
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

//--public method--------------------------------------------------------------
void TouchTableThread ::adjustGamma(cv::Mat& img, float gamma) {
	/*cv::Mat lookUpTable(1, 256, CV_8U);
	unsigned char* p = lookUpTable.ptr();
	for (int i = 0; i < 256; i++) {
		p[i] = cv::saturate_cast<unsigned char>(pow(i / 255.0, gamma) * 255.0);
	}*/
	uchar LUT[256];
	for (int i = 0; i < 256; i++) {
		LUT[i] = (int)(pow((double)i / 255.0, gamma) * 255.0);
	}
	cv::Mat lookUpTable = cv::Mat(1, 256, CV_8UC1, LUT);
	cv::LUT(img, lookUpTable, img);
}

void TouchTableThread::setCamera(ofVideoGrabber* cam) {
	camera = cam;
}


void TouchTableThread::draw() {
	lock();
	ofxCv::drawMat(resultImg, 0, 0);
	
	ofSetColor(255);//描画色を白に設定
	ofSetLineWidth(2);//描画する線の太さを設定
	contourFinder_->draw();//輪郭描画

	//ofNoFill();//描画をアウトラインのみに設定
	//int n = contourFinder.size();//検知した数を確保
	//for (int i = 0; i < n; i++) {

	//}

	vector<TouchTableTracker>& followers = tracker_->getFollowers();
	for (int i = 0; i < followers.size(); i++) {
		followers[i].draw();
	}

	drawSrcCircle();
	unlock();
}

void TouchTableThread::setParam(
	float minAR, 
	float maxAR, 
	float th, 
	float gm) 
{
	minAreaRadius = minAR;
	maxAreaRadius = maxAR;
	threshold = th;
	gamma = gm;
	//二値化
	lock();
	contourFinder_->setThreshold(threshold);
	contourFinder_->setMinAreaRadius(minAreaRadius);
	contourFinder_->setMaxAreaRadius(maxAreaRadius);
	tracker_->setPersistence(15);
	tracker_->setMaximumDistance(32);
	unlock();
}

ofColor TouchTableThread::getTargetColor(int x, int y) {
	targetColor = camera->getPixels().getColor(x, y);
	return targetColor;
}

//--private method-------------------------------------------------
void TouchTableThread::threadedFunction() {
	contourFinder_->setAutoThreshold(true);
	while (isThreadRunning()) {
		if (camera != nullptr) {
			if (camera->isFrameNew()) {
				lock();
				img = ofxCv::toCv(*camera);
				cv::cvtColor(img.clone(), gray, cv::COLOR_RGB2GRAY);
				cv::warpPerspective(gray, gray, perspectiveMat, gray.size(), cv::INTER_NEAREST);
				//cv::resize(gray,gray,cv::Size(640/2, 480/2), 0 ,0 ,0);
				adjustGamma(gray, gamma);

				resultImg = (isCalibMode) ? img : gray.clone();
				//輪郭検知開始
				contourFinder_->findContours(gray);
				tracker_->track(contourFinder_->getBoundingRects());
				unlock();
			}
			Sleep(2);
		}
	}
}

//--Circle Drawing for Perspective

void TouchTableThread::reset_Circle() {
	pts_src.clear();
	pts_src.push_back(ofVec2f(0, 0));
	pts_src.push_back(ofVec2f(640 - 1, 0));
	pts_src.push_back(ofVec2f(640 - 1, 480 - 1));
	pts_src.push_back(ofVec2f(0, 480 - 1));
	setPerspective(pts_src);
}

void TouchTableThread::setCalibMode(bool calibMode) {
	if (isCalibMode != calibMode) {
		lock();
		isCalibMode = calibMode;
		pickedCircle = -1;
		unlock();
	}
}

bool TouchTableThread::getCalibMode() {
	return isCalibMode;
}

void TouchTableThread::drawSrcCircle() {
	if (isCalibMode) {
		ofNoFill();
		ofSetColor(255, 0, 0);
		ofDrawCircle(pts_src[0], 10);
		ofDrawBitmapString("0", pts_src[0]);
		ofDrawLine(pts_src[0], pts_src[1]);

		ofSetColor(0, 255, 0);
		ofDrawCircle(pts_src[1], 10);
		ofDrawBitmapString("1", pts_src[1]);
		ofDrawLine(pts_src[1], pts_src[2]);

		ofSetColor(0, 0, 255);
		ofDrawCircle(pts_src[2], 10);
		ofDrawBitmapString("2", pts_src[2]);
		ofDrawLine(pts_src[2], pts_src[3]);

		ofSetColor(255, 255, 0);
		ofDrawCircle(pts_src[3], 10);
		ofDrawBitmapString("3", pts_src[3]);
		ofDrawLine(pts_src[3], pts_src[0]);
		ofFill();
	}
}

void TouchTableThread::moveClosestPoint(int x, int y) {
	if (pickedCircle > -1) {
		pts_src[pickedCircle] = ofVec2f(x, y) + pickOffset;
	}
}

void TouchTableThread::pickClosestPoint(int x, int y) {
	float cls = INFINITY;
	for (size_t i = 0; i < 4; i++) {
		ofVec2f v = pts_src[i] - ofVec2f(x, y);
		float d = v.length();
		if (cls > d) {
			pickedCircle = i;
			cls = d;
			pickOffset = v;
		}
	}
}

void TouchTableThread::setPerspective(std::vector<ofVec2f> circles) {
	if (circles.size() != 4) return;

	pts_src.clear();
	std::vector<cv::Point2f> src;
	for (size_t i = 0; i < 4; i++) {
		pts_src.push_back(circles[i]);
		src.push_back(cv::Point2f(circles[i].x, circles[i].y));
	}

	std::vector<cv::Point2f> dst;
	dst.push_back(cv::Point2f(0, 0));
	dst.push_back(cv::Point2f(640 - 1, 0));
	dst.push_back(cv::Point2f(640 - 1, 480 - 1));
	dst.push_back(cv::Point2f(0, 480 - 1));

	lock();
	perspectiveMat = cv::getPerspectiveTransform(src, dst);
	unlock();
}

void TouchTableThread::setCalib() {
	setPerspective(pts_src);
}
