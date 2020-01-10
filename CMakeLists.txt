PROJECT(abelkhan_cpp)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6) 

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

set(BOOST_INCLUDE_PATH ${PROJECT_SOURCE_DIR}//3rdparty//boost CACHE PATH "boost include path")
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_PATH})

set(BOOST_LIBS_PATH ${PROJECT_BINARY_DIR}//3rdparty//boost CACHE PATH "boost lib path")
link_directories(${BOOST_LIBS_PATH})

set(zlib_INCLUDE_PATH ${PROJECT_SOURCE_DIR}//3rdparty//zlib CACHE PATH "zlib include path")
INCLUDE_DIRECTORIES(${zlib_INCLUDE_PATH})

set(zlib_LIBS_PATH ${PROJECT_BINARY_DIR}//3rdparty//zlib CACHE PATH "zlib lib path")
link_directories(${zlib_LIBS_PATH})

set(websocketpp_INCLUDE_PATH ${PROJECT_SOURCE_DIR}//3rdparty//websocketpp CACHE PATH "websocketpp include path")
INCLUDE_DIRECTORIES(${websocketpp_INCLUDE_PATH})

set(enet_INCLUDE_PATH ${PROJECT_SOURCE_DIR}//3rdparty//enet//include CACHE PATH "zlib include path")
INCLUDE_DIRECTORIES(${enet_INCLUDE_PATH})

set(enet_LIBS_PATH ${PROJECT_BINARY_DIR}//3rdparty//zlib CACHE PATH "enet lib path")
link_directories(${enet_LIBS_PATH})

add_subdirectory(./component/client)
add_subdirectory(./component/gate)
add_subdirectory(./component/hub)
add_subdirectory(./component/websocket_gate)