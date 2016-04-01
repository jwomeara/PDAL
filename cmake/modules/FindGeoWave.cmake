###############################################################################
#
# CMake module to search for GeoWave
#
# On success, the macro sets the following variables:
# GEOWAVE_FOUND       = if the runtime jar is found
# GEOWAVE_RUNTIME     = full path to the GeoWave jar
# GEOWAVE_INCLUDE_DIR = where to find the library headers
# GEOWAVE_LIBRARY     = full path to the library
#
###############################################################################
MESSAGE(STATUS "Searching for GeoWave")

IF(GEOWAVE_RUNTIME_JAR AND GEOWAVE_INCLUDE_DIR AND GEOWAVE_LIBRARY)
  # Already in cache, be silent
  SET(GEOWAVE_FIND_QUIETLY TRUE)
ENDIF()

FIND_FILE(GEOWAVE_RUNTIME_JAR
  geowave-runtime.jar
  PATHS
  /usr/bin
  /usr/local
  /usr/local/bin)

FIND_PATH(GEOWAVE_INCLUDE_DIR
  Jace.h
  PATHS
  /usr/include
  /usr/local
  /usr/local/include)

FIND_LIBRARY(GEOWAVE_LIBRARY
  NAMES geowave
  PATHS
  /usr/lib
  /usr/local
  /usr/local/lib)

# Handle the QUIETLY and REQUIRED arguments and set GEOWAVE_FOUND to TRUE
# if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GEOWAVE DEFAULT_MSG GEOWAVE_RUNTIME_JAR GEOWAVE_INCLUDE_DIR GEOWAVE_LIBRARY)
