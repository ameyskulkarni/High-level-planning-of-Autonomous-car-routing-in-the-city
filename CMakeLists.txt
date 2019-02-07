cmake_minimum_required(VERSION 2.8)
project(CV)

IF(NOT CMAKE_BUILD_TYPE)
SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(OpenCV REQUIRED)



set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -march=native -std=c++11 -Wno-reorder")


find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})


include_directories(
${PROJECT_SOURCE_DIR}/
#${PROJECT_SOURCE_DIR}/jsoncpp/
${JSONCPP_INCLUDE_DIRS}
)




add_executable(autocar autocar.cpp)
target_link_libraries(autocar
${OpenCV_LIBS} ${JSONCPP_LIBRARIES})

add_executable(carroutingreq carroutingreq.cpp)
target_link_libraries(carroutingreq
${OpenCV_LIBS} ${JSONCPP_LIBRARIES})
