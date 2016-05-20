# CMake helper macros
# Ali Salehi, Spring 2016

macro(enable_cpp14_support)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
      message(STATUS "Adding -std=c++14")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  endif()
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
      message(STATUS "Adding -std=c14")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c14")
  endif()
endmacro()

macro(enable_parallel_build)
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
      set(${CompilerFlag} "${${CompilerFlag}} /MP")
    endforeach()
    message(STATUS "Added parallel build arguments to CMAKE_*_FLAGS")
  elseif(${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
    message(STATUS ${CMAKE_BUILD_TOOL})
    include(ProcessorCount)
    ProcessorCount(CORES_COUNT)
    if(CORES_COUNT EQUAL 0)
      set(CORES_COUNT 4)
      message(STATUS "Failed to determine cpu cores, using 4")
    else()
      message(STATUS "Found ${CORES_COUNT} cpu cores")
    endif()
      set(CMAKE_MAKE_PROGRAM "${CMAKE_MAKE_PROGRAM} -j${CORES_COUNT}")
      message(STATUS "Added parallel build arguments to CMAKE_MAKE_PROGRAM")
  endif()
endmacro()

macro(set_files_group SOURCE_FILES)
  foreach(FILE ${${SOURCE_FILES}})
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    string(REPLACE "/" "\\\\" GROUP "${PARENT_DIR}")

    source_group(${GROUP} FILES ${FILE})
  endforeach()
endmacro()

macro(set_project_paths)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")
  set(DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Data")
  set(DEP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")
  add_definitions(-DDATA_PATH=${DATA_PATH})

  if (MSVC)
    set(SDL2_PATH "${DEP_PATH}/SDL2")
    set(PROTOBUF_SRC_ROOT_FOLDER "${DEP_PATH}/protobuf")
    set(ZeroMQ_ROOT_DIR "${DEP_PATH}/zeromq")
  endif()
endmacro()

macro(add_protobuf_generate_command PROTO_DEF PROTO_SRCS PROTO_HDRS)
  set(${PROTO_SRCS})
  set(${PROTO_HDRS})
  foreach(file ${${PROTO_DEF}})
      get_filename_component(FILE_ABS ${file} ABSOLUTE)
      get_filename_component(FILE_WE ${file} NAME_WE)
      get_filename_component(FILE_PATH ${file} DIRECTORY)

      set(GEN_FILE_HEADER "${FILE_PATH}/${FILE_WE}.pb.h")
      set(GEN_FILE_SOURCE "${FILE_PATH}/${FILE_WE}.pb.cc")

      file(RELATIVE_PATH GEN_FILE_HEADER_REL ${CMAKE_CURRENT_SOURCE_DIR} ${GEN_FILE_HEADER})
      file(RELATIVE_PATH GEN_FILE_SOURCE_REL ${CMAKE_CURRENT_SOURCE_DIR} ${GEN_FILE_SOURCE})

      list(APPEND ${PROTO_SRCS} ${GEN_FILE_SOURCE_REL})
      list(APPEND ${PROTO_HDRS} ${GEN_FILE_HEADER_REL})

      add_custom_command(
        OUTPUT ${GEN_FILE_SOURCE}
               ${GEN_FILE_HEADER}
        COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
        ARGS --proto_path=${FILE_PATH} --cpp_out=${FILE_PATH} ${FILE_ABS}
        DEPENDS ${FILE_ABS}
        COMMENT "Running C++ protocol buffer compiler on ${file}"
        VERBATIM )
  endforeach()
endmacro()

macro(add_shader_generate_command VERTEX_SHADER_SRC SHADER_HDRS)
  set(${SHADER_HDRS})
  foreach(file ${${VERTEX_SHADER_SRC}})
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

      list(APPEND ${SHADER_HDRS} ${GEN_VERT_SHADER_FILE_REL})
      list(APPEND ${SHADER_HDRS} ${GEN_FRAG_SHADER_FILE_REL})

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
endmacro()