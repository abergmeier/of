

build_source_pairs( src 
	ofEvents
)

add_library( of_events OBJECT
	ofDelegate.h
	ofEventUtils.h
	${src}
)

