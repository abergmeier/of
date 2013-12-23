
build_source_pairs( src
	ofMath
	ofMatrix3x3
	ofMatrix4x4
	ofQuaternion
	ofVec2f
	ofVec4f
)

add_library( of_math OBJECT
	ofVectorMath.h
	ofVec3f.h
	${src}
)


