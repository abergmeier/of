
find_package( Sndfile REQUIRED )

include_directories(
	${SNDFILE_INCLUDE_DIRS}
)

add_library( of_sound SHARED
	ofBaseSoundPlayer.h
	ofOpenALSoundPlayer.cpp
)

target_link_libraries( of_sound
	${SNDFILE_LIBRARIES}
)


