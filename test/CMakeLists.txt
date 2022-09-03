cmake_minimum_required(VERSION 3.21)
project(iceengine_test)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
   googletest
   GIT_REPOSITORY https://github.com/google/googletest.git
   GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(${PROJECT_NAME}
   System/KdTreeTests.cpp
)

target_link_libraries(${PROJECT_NAME}
   GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

find_library(iceengine iceengine)
find_library(crossguid crossguid)
find_library(uuid uuid)

if (NOT iceengine)
	message(FATAL_ERROR "iceengine not found")
endif()

if (NOT crossguid)
	message(FATAL_ERROR "Crossguid not found")
endif()

if (NOT uuid)
	message(FATAL_ERROR "uuid not found")
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -g)
target_link_directories(${PROJECT_NAME} PRIVATE /usr/local/lib PRIVATE /usr/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../../iceengine/headers PUBLIC ../../iceengine/external/inc PUBLIC ../,,/iceengine/external/inc/SDL2 PUBLIC .)
target_link_libraries(${PROJECT_NAME} iceengine crossguid uuid)
