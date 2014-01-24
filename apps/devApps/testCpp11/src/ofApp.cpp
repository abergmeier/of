#include "ofApp.h"
#include <thread>

//--------------------------------------------------------------
void ofApp::setup(){

	// test constexpr
	// test initializer list
	constexpr int TestId {12};

	// test default list initialisation
	vector<int> testVec {1,2,3,4,5,6,7,8};

	assert(testVec.size() == 8);
	
	// testing begin() end() on iterator_type
	// testing for_each
	// tesing lambdas
	
	string message {"Testing, "};

	assert(message == "Testing, ");
	
	for_each(begin(testVec), end(testVec), [&message](int i){
		message += to_string(i);
		ofLog() << message;
	});

	assert(message == "Testing, 12345678");
	
	// create a vector of threads.
	vector<std::thread> threads;
	
	// threads and lambdas

	// this should give us 8 threads.

	ofLogNotice() << testVec.size() << " threads starting, prepare for some seriously garbled text:\n";
	
	for(auto i : testVec){
		threads.push_back(std::thread([](){
			ofLogNotice() << "I am Thread " << std::this_thread::get_id();
		}));
	}

	// join all threads. SWEEEET!
	for(auto &t : threads){
		t.join();
	}
	
	// now back for some single-threaded action
	ofLogNotice() << "Threads joined, garbled text finished.";
	
	ofExit();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
