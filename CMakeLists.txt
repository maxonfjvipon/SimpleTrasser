cmake_minimum_required(VERSION 3.13)
project(trasser)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES trasser main.cpp geometry.h Sphere.cpp Sphere.h Material.cpp Material.h Light.cpp Light.h)

add_executable(trasser ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(SFML 2.5.1 REQUIRED system window graphics network audio)
include_directories("${SFML_INCLUDE_DIR}")
target_link_libraries(trasser ${SFML_LIBRARIES})