
#include <map>
#include <cassert>
#include "ofURLFileLoader_EM.hpp"

template <typename... Args> using map = std::map<Args...>;

namespace {
	typedef void (*answer_type)( unsigned int, int );
}

extern "C" {
	void of_url_load( unsigned int id, const char* url, answer_type finish );
}

namespace {
	struct promised_request : public ofHttpRequest {
		typedef ofHttpRequest base_type;
		promised_request( string url, string name, bool load );
		promise<ofHttpResponse> response;
	};

	ofEvent<ofHttpResponse> RESPONSE_EVENT;

	map<unsigned int, weak_ptr<promised_request> > requests;

	shared_ptr<promised_request>
	find_request( unsigned int id ) {
		auto it = requests.find( id );

		auto shared_request = it->second.lock();

		if( !shared_request ) {
			// Cleanup
			requests.erase( it );
		}

		return shared_request;
	}

	void
	request_loaded( unsigned int id, int status ) {

		auto shared_request = find_request( id );

		shared_request->response.set_value( ofHttpResponse{shared_request, status, ""} );
	}

	void
	save_response( unsigned int id, int status ) {

		request_loaded( id, status );
	}

	unsigned int last_id = 0;

	of::net::shared_request
	get_url( string url, answer_type finish, string path="" ) {
		auto shared_request = make_shared<promised_request>( std::move(url),
		                                                     std::move(path),
		                                                     false );
		requests.emplace( last_id, shared_request );

		of_url_load( last_id, shared_request->url.c_str(), finish);

		++last_id;

		return shared_request;
	}
}

using namespace of::net;

shared_request
ofLoadURL( string url ) {

	return get_url( std::move(url), request_loaded );
}

shared_request
ofSaveURL( string url, string path ) {

	return get_url( std::move(url), save_response, std::move(path) );
}

void
ofRemoveAllURLRequests() {
	requests.clear();
}

void
ofStopURLLoader() {
	//TODO: Implement
}

ofEvent<ofHttpResponse>&
ofURLResponseEvent() {
	return RESPONSE_EVENT;
}

promised_request::promised_request( string url, string name, bool load ) :
	base_type( std::move(url), std::move(name), load )
{}

URLFileLoader::URLFileLoader() {
}

shared_request
URLFileLoader::get(string url) {
	return ofLoadURL( move(url) );
}

shared_request
URLFileLoader::saveTo(string url, string path) {
	return ofSaveURL( move(url), move(path) );
}

