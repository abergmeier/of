
build_source_pairs( src
	#ofVideoGrabber
	ofVideoPlayer
)

add_library( of_video SHARED
	${src}
)

target_link_libraries( of_video
	of_gl
)

