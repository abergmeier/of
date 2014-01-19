#pragma once

#include "ofEvents.h"
#include "ofFileUtils.h"


class ofHttpRequest{
public:
	ofHttpRequest()
	:saveTo(false){};

	ofHttpRequest( ofHttpRequest&& ) noexcept = default;

	ofHttpRequest(string url,string name,bool saveTo=false)
	:url(url)
	,name(name)
	,saveTo(saveTo){}

	virtual ~ofHttpRequest() noexcept;

	string				url;
	string				name;
	bool				saveTo;
};

class ofHttpResponse{
public:
	ofHttpResponse(){}

	ofHttpResponse(ofHttpRequest request,const ofBuffer & data,int status, string error)
	:request( move(request) )
	,data(data)
	,status(status)
	,error(error){}

	ofHttpResponse(ofHttpRequest request,int status,string error)
	:request( move(request) )
	,status(status)
	,error(error){}

	operator ofBuffer&(){
		return data;
	}

	ofHttpRequest	    request;
	ofBuffer		    data;
	int					status;
	string				error;
};

future<ofHttpResponse> ofLoadURL  ( string url ) OF_WARN_UNUSED;
future<ofHttpResponse> ofSaveURLTo( string url, string path ) OF_WARN_UNUSED;

void ofRemoveURLRequest( future<ofHttpResponse>&& );
void ofRemoveAllURLRequests();

void ofStopURLLoader();

ofEvent<ofHttpResponse> & ofURLResponseEvent();

#ifdef TARGET_EMSCRIPTEN
#include "ofURLFileLoader_EM.hpp"
#else
#include "ofURLFileLoader_SYNC.hpp"
#endif

