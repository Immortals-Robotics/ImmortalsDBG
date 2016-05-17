cmake_minimum_required(VERSION 3.0)
project(ImmDBG)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake/")

set(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Data")
set(DEP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")

add_definitions(-DDATA_PATH=${DATA_PATH})

if(MSVC)
  set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
  foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  endforeach()
  add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
  add_definitions(-D_HAS_EXCEPTIONS=0)
endif()

find_package(Bgfx REQUIRED)
include_directories(${Bgfx_INCLUDE_DIRS})

file(GLOB_RECURSE GEN_VERTEX_SHADER_SRC "Source/*.vsc")
set(SHADER_HDRS)
foreach(file ${GEN_VERTEX_SHADER_SRC})
    get_filename_component(FILE_ABS ${file} ABSOLUTE)
    get_filename_component(FILE_WE ${file} NAME_WE)
    get_filename_component(FILE_PATH ${file} DIRECTORY)

    set(FILE_VERT_ABS "${FILE_PATH}/${FILE_WE}.vsc")
    set(FILE_FRAG_ABS "${FILE_PATH}/${FILE_WE}.fsc")
    set(FILE_DEF_ABS "${FILE_PATH}/${FILE_WE}.def")

    set(GEN_VERT_SHADER_FILE "${FILE_PATH}/${FILE_WE}.vsb.h")
    set(GEN_FRAG_SHADER_FILE "${FILE_PATH}/${FILE_WE}.fsb.h")

    file(RELATIVE_PATH GEN_VERT_SHADER_FILE_REL ${CMAKE_CURRENT_SOURCE_DIR} ${GEN_VERT_SHADER_FILE})
    file(RELATIVE_PATH GEN_FRAG_SHADER_FILE_REL ${CMAKE_CURRENT_SOURCE_DIR} ${GEN_FRAG_SHADER_FILE})

    list(APPEND SHADER_HDRS ${GEN_VERT_SHADER_FILE_REL})
    list(APPEND SHADER_HDRS ${GEN_FRAG_SHADER_FILE_REL})

    set(ARGS_VERT -f ${FILE_VERT_ABS} -o ${GEN_VERT_SHADER_FILE} -i ${Bgfx_SHADER_INCLUDE_DIR} --bin2c --varyingdef ${FILE_DEF_ABS} --platform ${Bgfx_PLATFORM} --type vertex)
    set(ARGS_FRAG -f ${FILE_FRAG_ABS} -o ${GEN_FRAG_SHADER_FILE} -i ${Bgfx_SHADER_INCLUDE_DIR} --bin2c --varyingdef ${FILE_DEF_ABS} --platform ${Bgfx_PLATFORM} --type fragment)
    if(WIN32)
      set(ARGS_VERT ${ARGS_VERT} --profile vs_4_0)
      set(ARGS_FRAG ${ARGS_FRAG} --profile ps_4_0)
    endif()

    add_custom_command(
      OUTPUT ${GEN_VERT_SHADER_FILE}
      COMMAND  ${Bgfx_SHADERC_EXECUTABLE}
      ARGS ${ARGS_VERT}
      DEPENDS ${FILE_VERT_ABS}
      COMMENT "Running shaderc vertex compiler on ${FILE_VERT_ABS}"
      VERBATIM)

    add_custom_command(
      OUTPUT ${GEN_FRAG_SHADER_FILE}
      COMMAND  ${Bgfx_SHADERC_EXECUTABLE}
      ARGS ${ARGS_FRAG}
      DEPENDS ${FILE_FRAG_ABS}
      COMMENT "Running shaderc fragment compiler on ${FILE_FRAG_ABS}"
      VERBATIM)
endforeach()



if (MSVC)
  set(SDL2_PATH ${DEP_PATH}/SDL2)
endif()
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(ZeroMQ REQUIRED)
include_directories(${ZeroMQ_INCLUDE_DIR})

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

foreach(FILE ${SOURCE_FILES})
  get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

  string(REPLACE "/" "\\\\" GROUP "${PARENT_DIR}")

  source_group(${GROUP} FILES ${FILE})
endforeach()

if(MSVC)
  target_link_libraries(ImmDBG wsock32)
endif()
target_link_libraries(ImmDBG ${PROTOBUF_LIBRARIES} ${Bgfx_LIBRARIES} ${SDL2_LIBRARY} ${ZeroMQ_LIBRARIES})

if (MSVC)
  add_custom_command(
    TARGET ImmDBG POST_BUILD                       # Adds a post-build event to ImmDBG
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
    ${SDL2_DLL}                                    # <--this is in-file
    $<TARGET_FILE_DIR:ImmDBG>)                     # <--this is out-file path
endif()