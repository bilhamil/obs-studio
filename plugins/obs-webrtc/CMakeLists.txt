project(obs-webrtc)

if(WIN32)
	set(obs-webrtc_PLATFORM_DEPS
		ws2_32
		winmm)
endif()

if(MSVC)
	set(obs-webrtc_PLATFORM_DEPS
		${obs-webrtc_PLATFORM_DEPS}
		w32-pthreads)
endif()

if(NOT WIN32)
	set_source_files_properties(${obs-outputs_librtmp_SOURCES} PROPERTIES
		COMPILE_FLAGS "-fvisibility=hidden")
endif()

find_package(WebRTC REQUIRED)

set(obs-webrtc_HEADERS
	obs-webrtc.h)
set(obs-webrtc_SOURCES
	obs-webrtc.cpp
	webrtc-output.cpp)
	
add_library(obs-webrtc MODULE
	${obs-webrtc_SOURCES}
	${obs-webrtc_HEADERS})
	
target_link_libraries(obs-webrtc
	libobs
	${obs-webrtc_PLATFORM_DEPS})

include_directories(${WEBRTC_INCLUDE_DIRS})
	

if(WEBRTC_LIBRARIES_DEBUG AND WEBRTC_LIBRARIES_RELEASE)
	#add all debug libraries
	foreach(library ${WEBRTC_LIBRARIES_DEBUG})
		target_link_libraries(obs-webrtc debug ${library})
	endforeach(library)

	#add all release libraries
	foreach(library ${WEBRTC_LIBRARIES_RELEASE})
		target_link_libraries(obs-webrtc optimized ${library})
	endforeach(library)
else()
	foreach(library ${WEBRTC_LIBRARIES})
		target_link_libraries(obs-webrtc general ${library})
	endforeach(library)
endif()

install_obs_plugin_with_data(obs-webrtc data)
