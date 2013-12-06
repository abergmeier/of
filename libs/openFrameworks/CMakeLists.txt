
function( build_source_pairs output )
	foreach( file ${ARGN} )
		list( APPEND files "${file}.cpp" )
		list( APPEND files "${file}.h" )
	endforeach()

	set( ${output} "${${output}};${files}" PARENT_SCOPE )
endfunction()

find_package( Poco REQUIRED )

set( includes
	3d
	app
	events
	gl
	graphics
	math
	types
	utils
	video
	${POCO_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories(
	${includes}
)

#add_subdirectory( 3d )
#add_subdirectory( app )
add_subdirectory( events )
#add_subdirectory( gl )
#add_subdirectory( graphics )
#add_subdirectory( math )
#add_subdirectory( types )
add_subdirectory( utils )
#add_subdirectory( video )

add_library( of_core SHARED
	ofMain.h
	$<TARGET_OBJECTS:of_events>
)

target_include_directories( of_core
	PUBLIC
	${includes}
)

target_link_libraries( of_core
	of_utils
	${POCO_LIBRARIES}
)
	


