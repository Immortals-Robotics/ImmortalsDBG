cmake_minimum_required(VERSION 3.22)

set(USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(ImmortalsDBG)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

find_package(raylib CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(rlimgui CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)

file(GLOB_RECURSE GEN_PROTO_DEF "Source/protos/*.proto")

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

file(GLOB_RECURSE GEN_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Source/*.c" "Source/*.cpp")
file(GLOB_RECURSE GEN_HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Source/*.h" "Source/*.hpp")

set(SOURCE_FILES
    ${GEN_HEADER_FILES}
    ${PROTO_HDRS}
    ${GEN_SOURCE_FILES}
    ${PROTO_SRCS}
    ${SHADER_HDRS})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_definitions(${PROJECT_NAME} PUBLIC DATA_DIR="${CMAKE_SOURCE_DIR}/data")
target_compile_definitions(${PROJECT_NAME} PUBLIC LOG_DIR="${CMAKE_SOURCE_DIR}/log")

target_link_libraries(${PROJECT_NAME} PRIVATE
	raylib
	imgui::imgui
	rlimgui::rlimgui
	protobuf::libprotobuf)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _WIN32_WINNT=_WIN32_WINNT_WIN10)
endif()
