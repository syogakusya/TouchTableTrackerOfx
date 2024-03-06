#include "touchTableTracker.h"

void TouchTableTracker::setup(const cv::Rect& track) {
	startedNasent = ofGetElapsedTimef();
	color.setHsb(ofRandom(0, 255), 255, 255);
	cur = ofxCv::toOf(track).getCenter();
	smooth = cur;
	state = NASENT;
	trail.clear();
}

void TouchTableTracker::update(const cv::Rect& track) { 
	if (state == BORN) {
		state = ALIVE;
	}

	if (state == ALIVE) {
		startedDying = 0;

		cur = ofxCv::toOf(track).getCenter();
		smooth.interpolate(cur, .5);
		trail.addVertex(smooth);
		if (trail.size() > 20) {
			trail.removeVertex(0);
		}
	}
	else if (state == NASENT) {
		float curTime = ofGetElapsedTimef();
		if (curTime - startedNasent > nasentTime) {
			color.setHsb(ofRandom(0, 255), 255, 255);
			state = BORN;
		}
	}
}

void TouchTableTracker::kill() {
	float curTime = ofGetElapsedTimef();
	if (state == ALIVE) {
		if (startedDying == 0) {
			startedDying = curTime;
		}
		else if (curTime - startedDying > dyingTime) {
			state = DEAD;
		}
	}
	else {
		state = DEAD;
	}
}

void TouchTableTracker ::draw() {
	if (state != ALIVE) return;

	float curTime = ofGetElapsedTimef();
	float age = curTime - nasentTime;

	ofPushStyle();
	float size = 16;
	ofSetColor(0, 0, 255);
	if (startedDying) {
		ofSetColor(ofColor::red);
		size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
	}
	ofNoFill();
	unsigned int label = this->getLabel();
	ofSeedRandom(label << 24);
	ofSetColor(0, 0, 255);

	ofDrawCircle(cur, size);
	ofDrawBitmapString(ofToString(label), cur);

	ofSetColor(0,255,255);
	switch (state)
	{
	case TouchTableTracker::NASENT:
		ofDrawBitmapString("NASENT", cur.x, cur.y - 10);
		break;
	case TouchTableTracker::BORN:
		ofDrawBitmapString("BORN", cur.x, cur.y - 10);
		break;
	case TouchTableTracker::ALIVE:
		ofDrawBitmapString("ALIVE", cur.x, cur.y - 10);
		break;
	case TouchTableTracker::DEAD:
		ofDrawBitmapString("DEAD", cur.x, cur.y - 10);
		break;
	default:
		break;
	}
	trail.draw();
	
	ofPopStyle();
}

void TouchTableTracker::terminate() {
	dead = true;
	state = DEAD;
	trail.clear();
}

//--public method--------------------------------------------------------------
void TouchTableThread::getCameraImage(ofImage& image) {
	lock();
	ofxCv::toOf(resultImg, image);
	unlock();
}
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

void TouchTableThread::getWindowSize(int w_, int h_) {
	w = w_;
	h = h_;
}

void TouchTableThread::setCamera(ofVideoGrabber* cam) {
	camera = cam;
}


void TouchTableThread::draw() {
	lock();
	ofSetColor(255);//•`‰æF‚ð”’‚ÉÝ’è
	ofSetLineWidth(2);//•`‰æ‚·‚éü‚Ì‘¾‚³‚ðÝ’è
	contourFinder_->draw();//—ÖŠs•`‰æ

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
	//“ñ’l‰»
	lock();
	contourFinder_->setThreshold(threshold);
	contourFinder_->setMinAreaRadius(minAreaRadius);
	contourFinder_->setMaxAreaRadius(maxAreaRadius);
	tracker_->setPersistence(15);
	tracker_->setMaximumDistance(32);
	unlock();
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
				cv::GaussianBlur(gray, gray, cv::Size(11, 11), 0, 0);
				adjustGamma(gray, gamma);

				resultImg = (isCalibMode) ? img : gray.clone();
				//—ÖŠsŒŸ’mŠJŽn
				contourFinder_->findContours(gray);
				tracker_->track(contourFinder_->getBoundingRects());
				sendTUIOData();
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
	pts_src.push_back(ofVec2f(w - 1, 0));
	pts_src.push_back(ofVec2f(w - 1, h - 1));
	pts_src.push_back(ofVec2f(0, h - 1));
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
		ofPushStyle();
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
		ofPopStyle();
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
	dst.push_back(cv::Point2f(w - 1, 0));
	dst.push_back(cv::Point2f(w - 1, h - 1));
	dst.push_back(cv::Point2f(0, h - 1));

	lock();
	perspectiveMat = cv::getPerspectiveTransform(src, dst);
	unlock();
}

void TouchTableThread::setCalib() {
	setPerspective(pts_src);
}

//--TUIO
void TouchTableThread::sendTUIOData() {
	tuioServer_->initFrame(TUIO::TuioTime::getSessionTime());
	for each (auto follower in  tracker_->getFollowers())
	{
		auto label = follower.getLabel();
		auto center = follower.smooth;

		center.x -= (w / 2);
		center.y -= (h / 2);
		center.x /= (w / 2);
		center.y /= (h / 2);
		switch (follower.state)
		{
		case TouchTableTracker::BORN:
			cursors_[label] = tuioServer_->addTuioCursor(center.x, center.y);
			break;

		case TouchTableTracker::ALIVE:
			tuioServer_->updateTuioCursor(cursors_[label], center.x, center.y);
			break;

		case TouchTableTracker::DEAD:
			tuioServer_->removeTuioCursor(cursors_[label]);
			follower.terminate();
			cursors_.erase(label);
			break;

		default:
			break;
		}
	}

	tuioServer_->commitFrame();
}