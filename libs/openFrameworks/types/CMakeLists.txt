
build_source_pairs( src
	ofBaseTypes
	ofColor
	ofParameter
	ofParameterGroup
	ofRectangle
)

add_library( of_types SHARED
	ofPoint.h
	ofTypes.h
	${src}
)

target_link_libraries( of_types
	of_3d
)


