cmake_minimum_required(VERSION 3.0)
project(ImmDBG)
include("${CMAKE_SOURCE_DIR}/CMake/Helpers.cmake")

set_project_paths()
enable_parallel_build()
enable_cpp14_support()

find_package(Bgfx REQUIRED)
include_directories(${Bgfx_INCLUDE_DIRS})

file(GLOB_RECURSE GEN_VERTEX_SHADER_SRC "Source/*.vsc")
add_shader_generate_command(GEN_VERTEX_SHADER_SRC SHADER_HDRS)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(ZeroMQ REQUIRED)
include_directories(${ZeroMQ_INCLUDE_DIR})

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

file(GLOB_RECURSE GEN_PROTO_DEF "*.proto")

add_protobuf_generate_command(GEN_PROTO_DEF PROTO_SRCS PROTO_HDRS)

file(GLOB_RECURSE GEN_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Source/*.c" "Source/*.cpp" "/Source/*.cc" "/Source/*.cxx")
file(GLOB_RECURSE GEN_HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Source/*.h" "Source/*.hpp")

file(GLOB_RECURSE GEN_DEP_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "3rdparty/*.c" "3rdparty/*.cpp" "/3rdparty/*.cc" "/3rdparty/*.cxx")
file(GLOB_RECURSE GEN_DEP_HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "3rdparty/*.h" "3rdparty/*.hpp")

set(SOURCE_FILES
    ${GEN_HEADER_FILES}
    ${PROTO_HDRS}
    ${GEN_DEP_HEADER_FILES}
    ${GEN_SOURCE_FILES}
    ${PROTO_SRCS}
    ${GEN_DEP_SOURCE_FILES}
    ${SHADER_HDRS})

add_executable(ImmDBG ${SOURCE_FILES})

set_files_group(SOURCE_FILES)

if(MSVC)
  target_link_libraries(ImmDBG wsock32 ws2_32)
endif()
target_link_libraries(ImmDBG ${PROTOBUF_LIBRARIES} ${Bgfx_LIBRARIES} ${SDL2_LIBRARY} ${ZeroMQ_LIBRARIES})

if (MSVC)
  add_custom_command(
    TARGET ImmDBG POST_BUILD                       # Adds a post-build event to ImmDBG
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
    ${SDL2_DLL}                                    # <--this is in-file
    $<TARGET_FILE_DIR:ImmDBG>)                     # <--this is out-file path
endif()