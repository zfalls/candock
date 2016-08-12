# Find openmm
#
# Find the openmm includes and library
# 
# if you nee to add a custom library search path, do it via via CMAKE_PREFIX_PATH 
# 
# This module defines
#  OPENMM_INCLUDE_DIR, where to find header, etc.
#  OPENMM_LIBRARY, the libraries needed to use openmm.
#  OPENMM_FOUND, If false, do not try to use openmm.

# only look in default directories
find_path(
	OPENMM_INCLUDE_DIR 
	NAMES OpenMM.h
    PATHS foreign/openmm/include
	DOC "openmm include dir"
)

find_library(
	OPENMM_LIBRARY
	NAMES libOpenMM_static.a libOpenMM.dylib
    PATHS foreign/openmm/lib
	DOC "openmm library"
)

set(OPENMM_INCLUDE_DIR ${OPENMM_INCLUDE_DIR} CACHE STRING INTERNAL)
set(OPENMM_LIBRARY ${OPENMM_LIBRARY} CACHE STRING INTERNAL)

MESSAGE( STATUS ":::OPENMM_INCLUDE_DIR:         " "${OPENMM_INCLUDE_DIR}" )
MESSAGE( STATUS ":::OPENMM_LIBRARY:         " "${OPENMM_LIBRARY}" )


IF(OPENMM_LIBRARY)
IF(OPENMM_INCLUDE_DIR OR OPENMM_CXX_FLAGS)

SET(OPENMM_FOUND 1)

ENDIF(OPENMM_INCLUDE_DIR OR OPENMM_CXX_FLAGS)
ENDIF(OPENMM_LIBRARY)


# ==========================================
IF(NOT OPENMM_FOUND)
# make FIND_PACKAGE friendly
IF(NOT OPENMM_FIND_QUIETLY)
IF(OPENMM_FIND_REQUIRED)
 MESSAGE(FATAL_ERROR "OPENMM required, please specify it's location.")
ELSE(OPENMM_FIND_REQUIRED)
 MESSAGE(STATUS       "ERROR: OPENMM was not found.")
ENDIF(OPENMM_FIND_REQUIRED)
ENDIF(NOT OPENMM_FIND_QUIETLY)
ENDIF(NOT OPENMM_FOUND)