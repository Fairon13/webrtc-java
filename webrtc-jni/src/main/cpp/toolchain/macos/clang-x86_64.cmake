set(CMAKE_CXX_VISIBILITY_PRESET		hidden)
set(CMAKE_SHARED_LINKER_FLAGS		"${CMAKE_SHARED_LINKER_FLAGS} -s")

set(SOURCE_TARGET			macos)
set(SOURCE_COMPILE_FLAGS	"-x objective-c++")

set(PROJECT_LIBRARIES_OS	"-framework Foundation" "-framework AVFoundation" "-framework CoreMedia" "-framework CoreAudio")

set(WEBRTC_COMPILE_DEFINITIONS	PUBLIC WEBRTC_MAC WEBRTC_POSIX)
set(WEBRTC_LIBRARIES_OS			"-framework Foundation" "-framework AVFoundation" "-framework CoreGraphics" "-framework CoreAudio" "-framework AudioToolbox" "-framework IOSurface" "-framework ApplicationServices" "-framework AppKit")