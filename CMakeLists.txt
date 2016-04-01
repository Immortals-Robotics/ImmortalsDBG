cmake_minimum_required(VERSION 3.0)
project(ImmDBG)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake/")

set(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Data")
set(DEP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Deps")

add_definitions(-DDATA_PATH=${DATA_PATH})

find_package(Bgfx REQUIRED)
include_directories(${Bgfx_INCLUDE_DIRS})

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
#find_package(ZeroMQ REQUIRED)
#include_directories(${ZeroMQ_INCLUDE_DIR})

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

file(GLOB_RECURSE GEN_PROTO_DEF "Source/*.proto")

set(PROTO_SRCS)
set(PROTO_HDRS)

foreach(file ${GEN_PROTO_DEF})
    get_filename_component(FILE_ABS ${file} ABSOLUTE)
    get_filename_component(FILE_WE ${file} NAME_WE)
    get_filename_component(FILE_PATH ${file} DIRECTORY)

    set(GEN_FILE_HEADER "${FILE_PATH}/${FILE_WE}.pb.h")
    set(GEN_FILE_SOURCE "${FILE_PATH}/${FILE_WE}.pb.cc")

    file(RELATIVE_PATH GEN_FILE_HEADER_REL ${CMAKE_CURRENT_SOURCE_DIR} ${GEN_FILE_HEADER})
    file(RELATIVE_PATH GEN_FILE_SOURCE_REL ${CMAKE_CURRENT_SOURCE_DIR} ${GEN_FILE_SOURCE})

    list(APPEND PROTO_SRCS ${GEN_FILE_SOURCE_REL})
    list(APPEND PROTO_HDRS ${GEN_FILE_HEADER_REL})

    add_custom_command(
      OUTPUT ${GEN_FILE_SOURCE}
             ${GEN_FILE_HEADER}
      COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --proto_path=${FILE_PATH} --cpp_out=${FILE_PATH} ${FILE_ABS}
      DEPENDS ${FILE_ABS}
      COMMENT "Running C++ protocol buffer compiler on ${file}" 
      VERBATIM )
endforeach()

file(GLOB_RECURSE GEN_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Source/*.c" "Source/*.cpp" "/Source/*.cc" "/Source/*.cxx")
file(GLOB_RECURSE GEN_HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Source/*.h" "Source/*.hpp")

set(SOURCE_FILES
    ${GEN_HEADER_FILES}
    ${PROTO_HDRS}
    ${GEN_SOURCE_FILES}
    ${PROTO_SRCS})

add_executable(ImmDBG ${SOURCE_FILES})

foreach(FILE ${SOURCE_FILES})
  get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

  string(REPLACE "/" "\\\\" GROUP "${PARENT_DIR}")

  source_group(${GROUP} FILES ${FILE})
endforeach()

if(WIN32)
  target_link_libraries(ImmDBG wsock32)
endif()
target_link_libraries(ImmDBG ${PROTOBUF_LIBRARIES} ${Bgfx_LIBRARIES} ${SDL_LIBRARY})