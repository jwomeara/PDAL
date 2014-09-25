###############################################################################
#
# CMake module to search for GeoWave
#
# On success, the macro sets the following variables:
# GeoWave_FOUND    = if the runtime jar is found
# GeoWave_RUNTIME  = full path to the GeoWave jar
#
###############################################################################
MESSAGE(STATUS "Searching for GeoWave")

IF(GeoWave_RUNTIME_JAR)
  # Already in cache, be silent
  SET(GeoWave_FIND_QUIETLY TRUE)
ENDIF()

FIND_FILE(GeoWave_RUNTIME_JAR
  geowave-gt-0.8.0-SNAPSHOT-accumulo-singlejar.jar
  PATHS
  /usr/bin
  /usr/local
  /usr/local/bin)

# Handle the QUIETLY and REQUIRED arguments and set GeoWave_FOUND to TRUE
# if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GeoWave DEFAULT_MSG GeoWave_RUNTIME_JAR)