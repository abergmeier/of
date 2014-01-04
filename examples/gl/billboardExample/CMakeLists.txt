
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/bin/data/dot.png@data/dot.png --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/bin/data/shadersGL3/Billboard.frag@data/shadersGL3/Billboard.frag --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/bin/data/shadersGL3/Billboard.vert@data/shadersGL3/Billboard.vert -s TOTAL_MEMORY=134217728" )

add_executable( example_gl_billboard
	src/main.cpp
	src/ofApp.cpp
)

target_link_libraries( example_gl_billboard
	of_core
)

