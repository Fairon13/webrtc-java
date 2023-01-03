cmake_minimum_required(VERSION 3.13)

message(STATUS "${CMAKE_TOOLCHAIN_FILE}")

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	message(FATAL_ERROR "TOOLCHAIN is not defined")
endif ()

project(webrtc-java)

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

add_subdirectory(dependencies/webrtc)
add_subdirectory(dependencies/jni-voithos)

file(GLOB SOURCES_ROOT "src/*.cpp")
file(GLOB SOURCES_API "src/api/*.cpp")
file(GLOB SOURCES_MEDIA "src/media/*.cpp")
file(GLOB SOURCES_MEDIA_AUDIO "src/media/audio/*.cpp")
file(GLOB SOURCES_MEDIA_AUDIO_OS "src/media/audio/${SOURCE_TARGET}/*.cpp")
file(GLOB SOURCES_MEDIA_VIDEO "src/media/video/*.cpp")
file(GLOB SOURCES_MEDIA_VIDEO_DESKTOP "src/media/video/desktop/*.cpp")
file(GLOB SOURCES_MEDIA_VIDEO_OS "src/media/video/${SOURCE_TARGET}/*.cpp")
file(GLOB SOURCES_PLATFORM "src/platform/${SOURCE_TARGET}/*.cpp")
file(GLOB SOURCES_RTC "src/rtc/*.cpp")

list(APPEND SOURCES
	${SOURCES_ROOT}
	${SOURCES_API}
	${SOURCES_MEDIA}
	${SOURCES_MEDIA_AUDIO}
	${SOURCES_MEDIA_AUDIO_OS}
	${SOURCES_MEDIA_VIDEO}
	${SOURCES_MEDIA_VIDEO_DESKTOP}
	${SOURCES_MEDIA_VIDEO_OS}
	${SOURCES_PLATFORM}
	${SOURCES_RTC}
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		include
		include/api
		include/media
		include/rtc
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

target_link_libraries(${PROJECT_NAME} jni-voithos webrtc ${PROJECT_LIBRARIES_OS})

if(DEFINED SOURCE_COMPILE_FLAGS)
	set_source_files_properties(${SOURCES} PROPERTIES COMPILE_FLAGS ${SOURCE_COMPILE_FLAGS})
endif()

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT Runtime
)

install (SCRIPT "${CMAKE_SOURCE_DIR}/PostInstall.cmake")