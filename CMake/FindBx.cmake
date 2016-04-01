# - Try to find Bx
# Once done, this will define
#
#  Bx_FOUND - system has Bx
#  Bx_INCLUDE_DIRS - the Bx include directories
#  Bx_LIBRARIES - link these to use Bx

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Bx_PKGCONF bx)

# Root dir
find_path(Bx_ROOT_DIR
	NAMES include/bx/bx.h
	PATHS ${DEP_PATH}/bx)

# Include dir
find_path(Bx_INCLUDE_DIR
  NAMES bx/bx.h
  HINTS ${Bx_ROOT_DIR}/include 
		${Bx_PKGCONF_INCLUDE_DIRS})

set(Bx_LIBRARY)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(Bx_PROCESS_INCLUDES ${Bx_INCLUDE_DIR})
set(Bx_PROCESS_LIBS ${Bx_LIBRARY})
libfind_process(Bx)