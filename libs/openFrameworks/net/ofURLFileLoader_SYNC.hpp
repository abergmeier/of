#pragma once

#include "ofURLFileLoader.h"

#include <deque>
#include <queue>

#include "ofThread.h"

#include "Poco/Condition.h"

class ofURLFileLoader : public of::BaseURLFileLoader, public ofThread  {

    public:
        ofURLFileLoader();
		of::net::shared_request get(string url) override;
		of::net::shared_request saveTo(string url, string path) override;

        void stop();

    protected:

		// threading -----------------------------------------------
		void threadedFunction();
        void start();
        void update(ofEventArgs & args);  // notify in update so the notification is thread safe

    private:

		// perform the requests on the thread
        ofHttpResponse handleRequest(ofHttpRequest request);

		deque<ofHttpRequest> requests;
		queue<ofHttpResponse> responses;

		Poco::Condition condition;

};

