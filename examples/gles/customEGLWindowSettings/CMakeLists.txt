
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/bin/data/Raspi_Colour_R.png@data/Raspi_Colour_R.png" )

add_executable( example_custom_egl
	src/main.cpp
	src/ofApp.cpp
)

target_link_libraries( example_custom_egl
	of_core
)

