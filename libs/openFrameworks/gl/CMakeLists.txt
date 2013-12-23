
build_source_pairs( src
	#ofFbo
	ofGLProgrammableRenderer
	ofGLUtils
	#ofLight
	#ofMaterial
	ofShader
	ofTexture
	ofVbo
	ofVboMesh
)

add_library( of_gl SHARED
	${src}
)

target_link_libraries( of_gl
	of_types
)

