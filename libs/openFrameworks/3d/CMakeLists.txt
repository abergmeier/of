
find_package( Tess2 REQUIRED )

build_source_pairs( src
	of3dPrimitives
	of3dUtils
	ofCamera
	ofEasyCam
	ofMesh
	ofNode
)

add_library( of_3d SHARED
	${src}
)

target_include_directories( of_3d
	PUBLIC
		${TESS_INCLUDE_DIRS}
)

target_link_libraries( of_3d
	${TESS_LIBRARIES}
)


