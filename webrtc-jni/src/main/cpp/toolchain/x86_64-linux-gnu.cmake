set(CMAKE_SYSTEM_NAME		Linux)
set(CMAKE_SYSTEM_PROCESSOR	x86_64)

set(TARGET_CPU				"x64")

set(SYSROOT_ARCH			"amd64")
set(SYSROOT_PATH			"src/build/linux/debian_sid_amd64-sysroot")

set(CMAKE_SYSROOT /home/fairon/work/smart-chemodan/streamer/build-webrtc-java/src/build/linux/debian_sid_amd64-sysroot)
set(tools         /home/fairon/work/smart-chemodan/streamer/build-webrtc-java/src/third_party/llvm-build/Release+Asserts)

set(CMAKE_C_COMPILER ${tools}/bin/clang)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER ${tools}/bin/clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})
add_link_options("-fuse-ld=lld")
