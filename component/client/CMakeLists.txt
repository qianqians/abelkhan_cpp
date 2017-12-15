PROJECT(client)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6) 

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(caller_header__PATH ..//..//protcol//caller CACHE PATH "caller include path")
INCLUDE_DIRECTORIES(${caller_header__PATH})

file(GLOB caller_header_files ../../protcol/caller/ ../../protcol/caller/*.h)

set(module_header__PATH ..//..//protcol//module CACHE PATH "module include path")
INCLUDE_DIRECTORIES(${module_header__PATH})

file(GLOB module_header_files .../../protcol/module/ ../../protcol/module/*.h)

set(juggle_header__PATH ..//..//tools//juggle//c++ CACHE PATH "juggle include path")
INCLUDE_DIRECTORIES(${juggle_header__PATH})

AUX_SOURCE_DIRECTORY(../../tools/juggle/c++/ juggle_DIR_SRC)
file(GLOB juggle_header_files ../../tools/juggle/c++/ ./tools/juggle/c++/*.h)

set(jsonparse_header__PATH ..//..//tools//jsonparsepp CACHE PATH "jsonparse include path")
INCLUDE_DIRECTORIES(${jsonparse_header__PATH})

file(GLOB jsonparse_header_files ../../tools/jsonparse/ ../../tools/jsonparsepp/*.h)

set(service_header__PATH ..//..//service CACHE PATH "service include path")
INCLUDE_DIRECTORIES(${service_header__PATH})

file(GLOB service_header_files ../../service/ ../../service/*.h)

set(module_header__PATH1 ..//..//module CACHE PATH "module include path")
INCLUDE_DIRECTORIES(${module_header__PATH1})

file(GLOB module_header_files1 ../../module/ ../../module/*.h)

AUX_SOURCE_DIRECTORY(. DIR_SRC)
file(GLOB header_client_files ./ ./*.h)

SET(header_files
    ${header_client_files}
    ${module_header_files}
    ${service_header_files}
    ${juggle_header_files}
    ${caller_header_files}
    ${module_header_files1}
    ${jsonparse_header_files}
)

SET(SRC_LIST 
    ${DIR_SRC}
    ${juggle_DIR_SRC}
)

ADD_LIBRARY(client ${SRC_LIST} ${header_files})