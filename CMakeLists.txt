cmake_minimum_required(VERSION 2.8)
project(inpainting_dectection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(SOURCE_FILES
        src/utils.h
        src/connected_components.h
        src/detector.h src/detector.cpp
        src/patch_distance.h
        src/misc.h src/misc.cpp
        )

set(TEST_FILES
        test/test_utils.h
        test/test_connected_components.cpp
        test/test_experimentations.cpp
        test/test_misc.cpp
        )


include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/test
        ${CMAKE_CURRENT_SOURCE_DIR}/patch_match/include
        ${CMAKE_CURRENT_SOURCE_DIR}/patch_match/core/include
)


add_executable(run_tests test/main.cpp ${SOURCE_FILES} ${TEST_FILES})


# EXTERNAL LIBRAIRIES =====================

# OpenCV
FIND_PACKAGE(OpenCV 3.1 REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
set(TARGET_LIBS ${TARGET_LIBS} ${OpenCV_LIBS})

find_package(Boost 1.36.0 COMPONENTS filesystem system)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set(TARGET_LIBS ${TARGET_LIBS} ${Boost_LIBRARIES})
endif ()

TARGET_LINK_LIBRARIES(run_tests ${TARGET_LIBS})