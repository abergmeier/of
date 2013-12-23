

build_source_pairs( src 
	ofEvents
)

add_library( of_events
	SHARED
	ofDelegate.h
	ofEventUtils.h
	${src}
)

target_link_libraries( of_events
	of_utils
)

