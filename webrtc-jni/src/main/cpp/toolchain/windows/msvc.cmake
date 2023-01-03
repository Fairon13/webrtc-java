set(CMAKE_CXX_FLAGS_RELEASE		"/MT")
set(CMAKE_CXX_FLAGS_DEBUG		"/MTd")

set(SOURCE_TARGET	windows)
set(TARGET_CPU		"x64")

set(PROJECT_LIBRARIES_OS mf.lib mfreadwrite.lib mfplat.lib mfuuid.lib)

set(WEBRTC_COMPILE_DEFINITIONS PUBLIC WEBRTC_WIN NOMINMAX WIN32_LEAN_AND_MEAN)
set(WEBRTC_LIBRARIES_OS D3D11 DXGI user32 gdi32 iphlpapi dmoguids msdmo secur32 strmiids winmm wmcodecdspuuid ws2_32)