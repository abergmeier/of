#pragma once

#include "ofEvents.h"
#include "ofFileUtils.h"

class ofHttpResponse;

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
	future<ofHttpResponse> response;
};

namespace of {
	namespace net {
		using std::shared_ptr;
		using std::weak_ptr;
		using shared_request = shared_ptr< ofHttpRequest >;
		using weak_request   = weak_ptr  < ofHttpRequest >;
	}
}

class ofHttpResponse{
public:
	typedef of::net::shared_request shared_request;
	ofHttpResponse(){}

	ofHttpResponse(shared_request request,const ofBuffer & data,int status, string error)
	:request( move(request) )
	,data(data)
	,status(status)
	,error(error){}

	ofHttpResponse(shared_request request,int status,string error)
	:request( move(request) )
	,status(status)
	,error(error){}

	operator ofBuffer&(){
		return data;
	}

	shared_request      request;
	ofBuffer		    data;
	int					status;
	string				error;
};

of::net::shared_request ofLoadURL  ( string url ) OF_WARN_UNUSED;
of::net::shared_request ofSaveURLTo( string url, string path ) OF_WARN_UNUSED;

void ofRemoveAllURLRequests();

void ofStopURLLoader();

ofEvent<ofHttpResponse> & ofURLResponseEvent();

namespace of {
	class BaseURLFileLoader {
	protected:
		BaseURLFileLoader() = default;
		~BaseURLFileLoader() = default;
	public:
		virtual of::net::shared_request get(string url) OF_WARN_UNUSED = 0;
		virtual of::net::shared_request saveTo(string url, string path) OF_WARN_UNUSED = 0;
	};
} // namespace of

#ifdef TARGET_EMSCRIPTEN
#include "ofURLFileLoader_EM.hpp"
#else
#include "ofURLFileLoader_SYNC.hpp"
#endif

