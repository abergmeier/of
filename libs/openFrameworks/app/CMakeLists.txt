
build_source_pairs( src
	ofAppBaseWindow
	ofAppRunner
)

add_library( of_app SHARED
	BaseEGLWindow.cpp
	BaseEGLWindow.hpp
	EGLPage.cpp
	EGLPage.hpp
	ofIcon.h
	${src}
)

target_link_libraries( of_app
	of_gl
)

