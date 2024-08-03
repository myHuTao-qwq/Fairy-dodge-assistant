cmake_minimum_required(VERSION 3.4.1...3.5)
set(CMAKE_CXX_STANDARD 17)

project(zzz VERSION 1.0.0)

option(RELEASE "change directories" OFF)

if(NOT RELEASE)
    set(OpenCV_DIR "../opencv/build")
else()
    set(OpenCV_STATIC ON) # this option doesn't work with a pre-build opencv (not mobile)
    set(OpenCV_DIR "../opencv-mobile/x64")
endif()

find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(
    FAIRY_SRCS
    main.cpp
    screenshot.cpp
)

add_executable(zzz ${FAIRY_SRCS})

SET_TARGET_PROPERTIES(zzz PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")

target_link_libraries(
    zzz
    ${OpenCV_LIBS}
)