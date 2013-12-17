
#include <map>
#include <cassert>
#include "ofURLFileLoader_EM.hpp"

template <typename... Args> using map = std::map<Args...>;

namespace {
	typedef void (*answer_type)( unsigned int, int );
}

extern "C" {
	void _of_url_load( unsigned int id, answer_type finish );
}

namespace {
	struct promised_request : public ofHttpRequest {
		typedef ofHttpRequest base_type;
		promised_request( string url, string name, bool load );
		promise<ofHttpResponse> response;
	};

	ofEvent<ofHttpResponse> RESPONSE_EVENT;

	map<unsigned int, promised_request> requests;

	decltype(requests)::mapped_type&
	get_request( unsigned int id ) {
		auto it = requests.find( id );

		assert( it != requests.end() );
		return it->second;
	}

	void
	request_loaded( unsigned int id, int status ) {

		auto& request = get_request( id );
		auto response = ofHttpResponse{ request, status, "" };

		request.response.set_value( response );
	}

	void
	save_response( unsigned int id, int status ) {

		auto& request = get_request( id );
		auto response = ofHttpResponse{ request, status, "" };

		request.response.set_value( response );
	}

	unsigned int last_id = 0;

	future<ofHttpResponse>
	get_url( string url, answer_type finish, string path="" ) {
		auto insert = requests.emplace( last_id, promised_request{ std::move(url), std::move(path), false } );

		_of_url_load( last_id, finish);

		++last_id;

		return insert.first->second.response.get_future();
	}
}

future<ofHttpResponse>
ofLoadURL( string url ) {

	return get_url( std::move(url), request_loaded );
}

future<ofHttpResponse>
ofSaveURL( string url, string path ) {

	return get_url( std::move(url), save_response, std::move(path) );
}

void
ofRemoveURLRequest( future<ofHttpResponse>&& ) {
	//TODO: Implement
}

void
ofRemoveAllURLRequests() {
	//TODO: Implement
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


