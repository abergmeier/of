
build_source_pairs( src
	ofFileUtils
	ofLog
	ofMatrixStack
	ofSystemUtils
	ofUtils
	ofXml
)

add_library( of_utils SHARED
	ofConstants.h
	ofNoise.h
	${src}
)

target_link_libraries( of_utils
	of_3d
)

