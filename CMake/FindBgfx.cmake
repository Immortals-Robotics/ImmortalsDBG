# - Try to find Bgfx
# Once done, this will define
#
#  Bgfx_FOUND - system has Bgfx
#  Bgfx_INCLUDE_DIRS - the Bgfx include directories
#  Bgfx_LIBRARIES - link these to use Bgfx

include(LibFindMacros)

# Dependencies
libfind_package(Bgfx Bx)
if(APPLE)
  find_library(METAL_LIB NAMES Metal)
  find_library(QUARTZ_CORE_LIB NAMES QuartzCore)
endif()

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Bgfx_PKGCONF bgfx)

# Root dir
find_path(Bgfx_ROOT_DIR
  NAMES include/bgfx/bgfx.h
  PATHS ${DEP_PATH}/bgfx)

# Include dir
find_path(Bgfx_INCLUDE_DIR
  NAMES bgfx/bgfx.h
  HINTS ${Bgfx_ROOT_DIR}/include 
        ${Bgfx_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
if(MSVC)
  set(Bgfx_LIB_HINT_SUFFIX lib/win32)
elseif(APPLE)
  set(Bgfx_LIB_HINT_SUFFIX lib/osx)
endif()

find_library(Bgfx_LIBRARY_DEBUG
  NAMES bgfxDebug libbgfxDebug
  HINTS ${Bgfx_ROOT_DIR}/${Bgfx_LIB_HINT_SUFFIX}
        ${Bgfx_PKGCONF_LIBRARY_DIRS}
)

find_library(Bgfx_LIBRARY_RELEASE
  NAMES bgfxRelease libbgfxRelease
  HINTS ${Bgfx_ROOT_DIR}/${Bgfx_LIB_HINT_SUFFIX}
        ${Bgfx_PKGCONF_LIBRARY_DIRS}
)

  if(Bgfx_LIBRARY_RELEASE AND Bgfx_LIBRARY_DEBUG)
    set(Bgfx_LIBRARY
        debug ${Bgfx_LIBRARY_DEBUG}
        optimized ${Bgfx_LIBRARY_RELEASE})
    if(MSVC)
      set(Bgfx_LIBRARY ${Bgfx_LIBRARY}
              debug psapi.lib
              optimized psapi.lib)
    elseif(APPLE)
      set(Bgfx_LIBRARY ${Bgfx_LIBRARY}
              debug ${METAL_LIB}
              optimized ${METAL_LIB}
              debug ${QUARTZ_CORE_LIB}
              optimized ${QUARTZ_CORE_LIB})
    endif()
  elseif(Bgfx_LIBRARY_RELEASE)
    set(Bgfx_LIBRARY ${Bgfx_LIBRARY_RELEASE})
    if(MSVC)
      set(Bgfx_LIBRARY ${Bgfx_LIBRARY} psapi.lib)
    elseif(APPLE)
      set(Bgfx_LIBRARY ${Bgfx_LIBRARY} ${METAL_LIB} ${QUARTZ_CORE_LIB})
    endif()
  elseif(Bgfx_LIBRARY_DEBUG)
    set(Bgfx_LIBRARY ${Bgfx_LIBRARY_DEBUG})
    if(MSVC)
      set(Bgfx_LIBRARY ${Bgfx_LIBRARY} psapi.lib)
    elseif(APPLE)
      set(Bgfx_LIBRARY ${Bgfx_LIBRARY} ${METAL_LIB} ${QUARTZ_CORE_LIB})
    endif()
  endif()

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(Bgfx_PROCESS_INCLUDES ${Bgfx_INCLUDE_DIR} ${Bx_INCLUDE_DIRS})
set(Bgfx_PROCESS_LIBS ${Bgfx_LIBRARY})

libfind_process(Bgfx)