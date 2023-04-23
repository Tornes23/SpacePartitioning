cmake_minimum_required(VERSION 3.6.2)

##################################
# Build configuration
set(PRJ_TEST_NAME cs350_kdtrees_test)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_CXX_STANDARD 20)
# Source files
set(SRC		
		src/texture.hpp
		src/geometry.cpp
		src/geometry.hpp
		src/scene.cpp
		src/scene.hpp
		src/camera.hpp		
		src/mesh.hpp
		src/math.hpp
		src/material.hpp
		src/kdtree.hpp
		src/kdtree.cpp
		)
include_directories(src)

# Test files
set(SRC_TEST
		src/test/test_raytrace.cpp
		)

# Projects
project(${PRJ_TEST_NAME})

# Raytracer
#add_library(instructor_code src/mesh.cpp src/texture.cpp src/raytracer.cpp src/raytracer.hpp)

##################################
# Libraries
if (WIN32)
	# Directory in which libraries are
	set(DEPENDENCIES_DIR $ENV{USERPROFILE}/documents/digipen/dependencies)
	string(REPLACE "\\" "/" DEPENDENCIES_DIR ${DEPENDENCIES_DIR})
elseif (UNIX)
	# Directory in which libraries are
	set(DEPENDENCIES_DIR $ENV{HOME}/digipen/dependencies)
endif ()

# lodepng
set(SRC_EXTERNAL "${SRC_EXTERNAL}" "${DEPENDENCIES_DIR}/lodepng/lodepng.cpp")
include_directories("${DEPENDENCIES_DIR}/lodepng/")

# glm
include_directories("${DEPENDENCIES_DIR}/glm/")

# gtest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Make GTest match runtime libraries
add_subdirectory("${DEPENDENCIES_DIR}/googletest" "googletest")
enable_testing()

##################################
# Compile arguments

if (MSVC)
	# Visual Studio Configuration
	# Enable warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")

	# Warnings as errors
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX") 
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

	# Disable specific warning
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201") # nameless struct/union
elseif (CMAKE_CXX_GNU)
	# G++ configuration
	# Enable warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
	# Warnings as errors
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
	# Disable specific warning
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-truncation")
endif ()

# Test binaries
add_executable(${PRJ_TEST_NAME} ${SRC} ${SRC_TEST} ${SRC_EXTERNAL})
include_directories(${PRJ_TEST_NAME} PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(${PRJ_TEST_NAME} gtest_main instructor_code)
add_test(NAME ${PRJ_TEST_NAME} COMMAND ${PRJ_TEST_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
