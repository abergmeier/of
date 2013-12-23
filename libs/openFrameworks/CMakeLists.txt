
function( build_source_pairs output )
	foreach( file ${ARGN} )
		list( APPEND files "${file}.cpp" )
		list( APPEND files "${file}.h" )
	endforeach()

	set( ${output} "${${output}};${files}" PARENT_SCOPE )
endfunction()

find_package( Poco REQUIRED )
find_package( KissFFT REQUIRED )

set( includes
	3d
	app
	events
	gl
	graphics
	math
	net
	sound
	types
	utils
	video
	${POCO_INCLUDE_DIRS}
	${KISS_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories(
	${includes}
)

add_subdirectory( 3d )
add_subdirectory( app )
add_subdirectory( events )
add_subdirectory( gl )
add_subdirectory( graphics )
add_subdirectory( math )
add_subdirectory( net )
add_subdirectory( types )
add_subdirectory( utils )
add_subdirectory( sound )
add_subdirectory( video )

add_library( of_core SHARED
	ofMain.h
	$<TARGET_OBJECTS:of_math>
	$<TARGET_OBJECTS:of_net>
	$<TARGET_OBJECTS:tr3>
)

# Have to force linker language, since of_core is only a dummy lib
set_target_properties( of_core PROPERTIES LINKER_LANGUAGE CXX )

target_include_directories( of_core
	PUBLIC
	${includes}
)

target_link_libraries( of_core
	of_3d
	of_app
	of_utils
	of_events
	of_graphics
	of_sound
	of_video
	${POCO_LIBRARIES}
	${KISS_LIBRARIES}
)
	


