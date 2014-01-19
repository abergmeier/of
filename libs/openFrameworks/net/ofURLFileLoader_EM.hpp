#pragma once

#include "ofURLFileLoader.h"

namespace of {
	namespace net {

		class URLFileLoader : public BaseURLFileLoader {
		public:

			URLFileLoader();
			shared_request get(string url) override;
			shared_request saveTo(string url, string path) override;
		};
	} // namespace net
} // namespace of

using ofURLFileLoader = of::net::URLFileLoader;

