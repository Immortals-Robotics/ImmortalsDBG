# - Try to find Bgfx
# Once done, this will define
#
#  Bgfx_FOUND - system has Bgfx
#  Bgfx_INCLUDE_DIRS - the Bgfx include directories
#  Bgfx_LIBRARIES - link these to use Bgfx

include(LibFindMacros)

# Dependencies
libfind_package(Bgfx Bx)

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
if(MSVC)
  # Finally the library itself
  find_library(Bgfx_LIBRARY_DEBUG
    NAMES bgfxDebug
    HINTS ${Bgfx_ROOT_DIR}/lib 
          ${Bgfx_PKGCONF_LIBRARY_DIRS}
  )

  find_library(Bgfx_LIBRARY_RELEASE
    NAMES bgfxRelease
    HINTS ${Bgfx_ROOT_DIR}/lib 
          ${Bgfx_PKGCONF_LIBRARY_DIRS}
  )

  if(Bgfx_LIBRARY_RELEASE AND Bgfx_LIBRARY_DEBUG)
    set(Bgfx_LIBRARY
        debug ${Bgfx_LIBRARY_DEBUG}
        optimized ${Bgfx_LIBRARY_RELEASE}
        debug psapi.lib
        optimized psapi.lib
        )
  elseif(Bgfx_LIBRARY_RELEASE)
    set(Bgfx_LIBRARY ${Bgfx_LIBRARY_RELEASE} psapi.lib)
  elseif(Bgfx_LIBRARY_DEBUG)
    set(Bgfx_LIBRARY ${Bgfx_LIBRARY_DEBUG} psapi.lib)
  endif()
else()
  find_library(Bgfx_LIBRARY
    NAMES bgfx libbgfx
    HINTS ${Bgfx_ROOT_DIR}/lib 
          ${Bgfx_PKGCONF_LIBRARY_DIRS}
  )
endif()

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(Bgfx_PROCESS_INCLUDES ${Bgfx_INCLUDE_DIR} ${Bx_INCLUDE_DIRS})
set(Bgfx_PROCESS_LIBS ${Bgfx_LIBRARY})

libfind_process(Bgfx)