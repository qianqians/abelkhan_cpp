PROJECT(client)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6) 

set(BOOST_INCLUDE_PATH ${PROJECT_SOURCE_DIR}//3rdparty//boost CACHE PATH "boost include path")
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_PATH})

set(BOOST_LIBS_PATH ${PROJECT_BINARY_DIR}//3rdparty//boost//stage//lib CACHE PATH "boost lib path")
link_directories(${BOOST_LIBS_PATH})

add_subdirectory(./component/client)
add_subdirectory(./component/gate)