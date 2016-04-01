#
# GeoWave support
#

# GeoWave support - optional, default=OFF
option(BUILD_PLUGIN_GEOWAVE "Choose if GeoWave support should be built" FALSE)
if(BUILD_PLUGIN_GEOWAVE)
    find_package(GeoWave)
    if(GEOWAVE_FOUND)    
        add_definitions(-DGEOWAVE_RUNTIME_JAR=${GEOWAVE_RUNTIME_JAR})
        set(CMAKE_REQUIRED_LIBRARIES "${GEOWAVE_LIBRARY}")
        include_directories(${GEOWAVE_INCLUDE_DIR})
        mark_as_advanced(CLEAR GEOWAVE_RUNTIME_JAR)
        mark_as_advanced(CLEAR GEOWAVE_INCLUDE_DIR)
        mark_as_advanced(CLEAR GEOWAVE_LIBRARY)
        set(PDAL_HAVE_GEOWAVE 1)
    else()
        set(GEOWAVE_RUNTIME_JAR "")
        set(GEOWAVE_INCLUDE_DIR "")
        set(GEOWAVE_LIBRARY "")
        set(BUILD_PLUGIN_GEOWAVE FALSE)
    endif()
endif()
