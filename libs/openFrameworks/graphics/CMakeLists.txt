
find_package( FreeImage REQUIRED )
find_package( FreeType REQUIRED )

build_source_pairs( src
	of3dGraphics
	ofBitmapFont
	ofGraphics
	ofImage
	ofPath
	ofPixels
	ofPolyline
	ofRendererCollection
	ofTessellator
	ofTrueTypeFont
)

add_library( of_graphics SHARED
	${src}
)

target_include_directories( of_graphics
	PUBLIC
		../../libc++
		${FREEIMAGE_INCLUDE_DIRS}
		${FREETYPE_INCLUDE_DIRS}
)

target_link_libraries( of_graphics
	of_3d
	${FREEIMAGE_LIBRARIES}
	${FREETYPE_LIBRARIES}
)

