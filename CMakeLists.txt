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


set(PROTOS_DIR protos)
add_subdirectory(${PROTOS_DIR})

set(PROTO_SRCS)
set(PROTO_HDRS)

set(PROTO_DEF_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${PROTOS_DIR})
set(PROTO_GEN_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PROTOS_DIR})

foreach(file ${PROTO_FILES})
	get_filename_component(FILE_ABS ${file} ABSOLUTE)  # Full path to file
	get_filename_component(FILE_WE  ${file} NAME_WE)   # File name with neither the directory nor the longest extension
	get_filename_component(FILE_DIR ${file} DIRECTORY) # Directory without file name

	file(RELATIVE_PATH FILE_REL     ${PROTO_DEF_BASE_DIR} ${FILE_ABS})
	file(RELATIVE_PATH FILE_DIR_REL ${PROTO_DEF_BASE_DIR} ${FILE_DIR})

	set(GEN_DIR "${PROTO_GEN_BASE_DIR}/${FILE_DIR_REL}")
	set(GEN_FILE_HEADER "${GEN_DIR}/${FILE_WE}.pb.h")
	set(GEN_FILE_SOURCE "${GEN_DIR}/${FILE_WE}.pb.cc")

	list(APPEND PROTO_SRCS ${GEN_FILE_SOURCE})
	list(APPEND PROTO_HDRS ${GEN_FILE_HEADER})

	add_custom_command(
	OUTPUT ${GEN_FILE_SOURCE}
			${GEN_FILE_HEADER}
	COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
	ARGS --proto_path=${PROTO_DEF_BASE_DIR} --cpp_out=${PROTO_GEN_BASE_DIR} ${FILE_REL} 
	DEPENDS ${FILE_ABS}
	COMMENT "Running C++ protocol buffer compiler on ${FILE_REL}"
	VERBATIM )
endforeach()

file(GLOB_RECURSE GEN_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "source/*.c" "source/*.cpp")
file(GLOB_RECURSE GEN_HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "source/*.h" "source/*.hpp")

set(SOURCE_FILES
    ${GEN_HEADER_FILES}
    ${PROTO_HDRS}
    ${GEN_SOURCE_FILES}
    ${PROTO_SRCS}
    ${SHADER_HDRS})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "${PROTO_GEN_BASE_DIR}")

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

target_precompile_headers(${PROJECT_NAME} PUBLIC ${PROTO_HDRS} "source/pch.h")
