
find_package( Tess2 REQUIRED )

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

target_include_directories( of_utils
	PUBLIC
	${TESS_INCLUDE_DIRS}
)

target_link_libraries( of_utils
	${TESS_LIBRARIES}
)

