cmake_minimum_required(VERSION 2.6)

PROJECT ( FaceDBsqlProject CXX C )

# build in parallel
SET ( CMAKE_MAKE_PROGRAM "${CMAKE_MAKE_PROGRAM} -j4" )

IF (NOT WIN32)
    ADD_DEFINITIONS(-ggdb)
ENDIF (NOT WIN32)

SET(CMAKE_DEBUG_POSTFIX "d")

#####################################################
#   Find external Libs (OpenCV,IPP,ITK)
#####################################################

SET(CMAKE_VERBOSE_MAKEFILE ON)  # in order to allow eclipse to pick up additional include directories

# for our additional checks we need to set the path to our local check scripts
#SET(CMAKE_MODULE_PATH ${MDLFaces_SOURCE_DIR}/cmake_checks)

#MESSAGE("OpenCV_DIR " '$ENV{OpenCV_DIR}' )
SET( OpenCV_DIR $ENV{OpenCV_DIR})

FIND_PACKAGE(OpenCV REQUIRED)
IF(OpenCV_FOUND)
	Message("opencv root dir: '${OpenCV_ROOT_DIR}'")
	Message("opencv include dirs: '${OpenCV_INCLUDE_DIRS}'")
  INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${OpenCV_LINK_DIRECTORIES} )

# HOTFIX
if(WIN32)
SET(OpenCV_LIBS debug opencv_objdetect232d 
	optimized opencv_objdetect232 
	debug opencv_core232d
	optimized opencv_core232
	debug opencv_imgproc232d
	optimized opencv_imgproc232
	debug opencv_flann232d
	optimized opencv_flann232
	debug opencv_highgui232d
	optimized opencv_highgui232
	debug opencv_features2d232d
	optimized opencv_features2d232
	debug opencv_calib3d232d
	optimized opencv_calib3d232
	debug opencv_gpu232d
	optimized opencv_gpu232)
endif(WIN32)

#MESSAGE("OpenCV_LINK_DIRECTORIES " ${OpenCV_LINK_DIRECTORIES} )
#MESSAGE("OpenCV_LIBS " ${OpenCV_LIBS} )
#MESSAGE("OpenCV_LIBS " ${OpenCV_LIBS} )

# unfortunately we need _cv.h which is in the source directory
#  INCLUDE_DIRECTORIES( ${OpenCV_ROOT_DIR}/../src/cv )
ELSE(OpenCV_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without OpenCV.  Please set OPENCV_DIR.")
ENDIF(OpenCV_FOUND)

# Boost
#FIND_PACKAGE ( FindBoost )
SET ( Boost_USE_STATIC_LIBS   ON)
FIND_PACKAGE(Boost COMPONENTS program_options filesystem system regex REQUIRED)

SET (BUILD_SQLITE OFF CACHE BOOL "Build Sqlite3")

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )
#MESSAGE( ${Boost_INCLUDE_DIRS} )

################################
#
#  Sources 
#

FILE ( GLOB DBCONN_HEADERS   dbconn/*.h )
FILE ( GLOB FACEDATA_HEADERS facedata/*.h )
FILE ( GLOB QUERYS_HEADERS   querys/*.h )
FILE ( GLOB UTIL_HEADERS   util/*.h )

FILE ( GLOB DBCONN_SOURCES   dbconn/*.cpp )
FILE ( GLOB FACEDATA_SOURCES facedata/*.cpp )
FILE ( GLOB QUERYS_SOURCES   querys/*.cpp )
FILE ( GLOB UTIL_SOURCES   util/*.cpp )

IF (BUILD_SQLITE)
   INCLUDE_DIRECTORIES(sqlite3)
ENDIF (BUILD_SQLITE)

################################
#
#  FaceDBsqlTest Executable 
#

ADD_EXECUTABLE ( FaceDBsqlTest
	tests/main.cpp 
	${DBCONN_HEADERS} 
	${FACEDATA_HEADERS} 
	${QUERYS_HEADERS} 
	${DBCONN_SOURCES} 
	${FACEDATA_SOURCES} 
	${QUERYS_SOURCES} 
)

TARGET_LINK_LIBRARIES ( FaceDBsqlTest
	sqlite3
	${OpenCV_LIBS}
)

#################################
# 
#  AnnotatedFeaturePatch Test
#

ADD_EXECUTABLE ( AnnotatedFeaturePatches
	tests/AnnotatedFeaturePatches.cpp
	${DBCONN_HEADERS} 
	${FACEDATA_HEADERS} 
	${QUERYS_HEADERS} 
	${DBCONN_SOURCES} 
	${FACEDATA_SOURCES} 
	${QUERYS_SOURCES} 
)

TARGET_LINK_LIBRARIES ( AnnotatedFeaturePatches
	sqlite3
	${OpenCV_LIBS}
)

#################################
# 
#  FlickrImport
#

SET ( FLICKR_IMPORT_HEADERS   import/FeaturesFileReader.h  import/PoseFileReader.h import/MetaDataFileReader.h)
SET ( FLICKR_IMPORT_SOURCES   import/flickr_import.cpp import/FeaturesFileReader.cpp  import/PoseFileReader.cpp import/MetaDataFileReader.cpp )

ADD_EXECUTABLE ( FlickrImport
	${FLICKR_IMPORT_HEADERS} 
	${FLICKR_IMPORT_SOURCES} 
	${DBCONN_HEADERS} 
	${DBCONN_SOURCES} 
	${FACEDATA_HEADERS} 
	${FACEDATA_SOURCES} 
	${QUERYS_HEADERS} 
	${QUERYS_SOURCES} 
	${UTIL_HEADERS} 
	${UTIL_SOURCES} 
) 

TARGET_LINK_LIBRARIES ( FlickrImport
	sqlite3
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)

#################################
# 
#  BioIdImport
#

SET ( BIOID_IMPORT_HEADERS   import/BioIdFileReader.h import/ImagesFromFolder.h import/DeleteSqlEntries.h  import/MetaDataFileReader.h)
SET ( BIOID_IMPORT_SOURCES   import/bioid_import.cpp import/BioIdFileReader.cpp import/ImagesFromFolder.cpp import/DeleteSqlEntries.cpp import/MetaDataFileReader.cpp )

ADD_EXECUTABLE ( BioIdImport
	${BIOID_IMPORT_HEADERS} 
	${BIOID_IMPORT_SOURCES} 
	${DBCONN_HEADERS} 
	${DBCONN_SOURCES} 
	${FACEDATA_HEADERS} 
	${FACEDATA_SOURCES} 
	${QUERYS_HEADERS} 
	${QUERYS_SOURCES} 
	${UTIL_HEADERS} 
	${UTIL_SOURCES} 
) 

TARGET_LINK_LIBRARIES ( BioIdImport
	sqlite3
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)

#################################
# 
#  PUT Import
#

SET ( PUT_IMPORT_HEADERS   import/PutFileReader.h import/ImagesFromFolder.h import/DeleteSqlEntries.h  import/MetaDataFileReader.h )
SET ( PUT_IMPORT_SOURCES   import/put_import.cpp import/PutFileReader.cpp import/ImagesFromFolder.cpp import/DeleteSqlEntries.cpp import/MetaDataFileReader.cpp )

ADD_EXECUTABLE ( PutImport
	${PUT_IMPORT_HEADERS} 
	${PUT_IMPORT_SOURCES} 
	${DBCONN_HEADERS} 
	${DBCONN_SOURCES} 
	${FACEDATA_HEADERS} 
	${FACEDATA_SOURCES} 
	${QUERYS_HEADERS} 
	${QUERYS_SOURCES} 
	${UTIL_HEADERS} 
	${UTIL_SOURCES} 
) 

TARGET_LINK_LIBRARIES ( PutImport
	sqlite3
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)


#################################
# 
#  FACE TRACER Import
#

SET ( FACETRACER_IMPORT_HEADERS   import/FaceTracerFileReader.h import/ImagesFromFolder.h import/DeleteSqlEntries.h  import/MetaDataFileReader.h )
SET ( FACETRACER_IMPORT_SOURCES   import/facetracer_import.cpp import/FaceTracerFileReader.cpp import/ImagesFromFolder.cpp import/DeleteSqlEntries.cpp  import/MetaDataFileReader.cpp )

ADD_EXECUTABLE ( FaceTracerImport
	${FACETRACER_IMPORT_HEADERS} 
	${FACETRACER_IMPORT_SOURCES} 
	${DBCONN_HEADERS} 
	${DBCONN_SOURCES} 
	${FACEDATA_HEADERS} 
	${FACEDATA_SOURCES} 
	${QUERYS_HEADERS} 
	${QUERYS_SOURCES} 
	${UTIL_HEADERS} 
	${UTIL_SOURCES} 
) 

TARGET_LINK_LIBRARIES ( FaceTracerImport
	sqlite3
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)

#################################
# 
#  LPFW Importer
#

SET ( LPFW_IMPORT_HEADERS   import/LFPWFilesReader.h import/ImagesFromFolder.h import/DeleteSqlEntries.h  import/MetaDataFileReader.h )
SET ( LPFW_IMPORT_SOURCES   import/LFPW_import.cpp import/LFPWFilesReader.cpp import/ImagesFromFolder.cpp import/DeleteSqlEntries.cpp import/MetaDataFileReader.cpp )

ADD_EXECUTABLE ( LPFWImport
	${LPFW_IMPORT_HEADERS} 
	${LPFW_IMPORT_SOURCES} 
	${DBCONN_HEADERS} 
	${DBCONN_SOURCES} 
	${FACEDATA_HEADERS} 
	${FACEDATA_SOURCES} 
	${QUERYS_HEADERS} 
	${QUERYS_SOURCES} 
	${UTIL_HEADERS} 
	${UTIL_SOURCES} 
) 

TARGET_LINK_LIBRARIES ( LPFWImport
	sqlite3
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)

#################################
# 
#  CMU Profile Import
#

SET ( CMU_IMPORT_HEADERS   import/CMUGTFilesReader.h )
SET ( CMU_IMPORT_SOURCES   import/CMUProfile_import.cpp import/CMUGTFilesReader.cpp )

ADD_EXECUTABLE ( CMUProfileImport
	${CMU_IMPORT_HEADERS} 
	${CMU_IMPORT_SOURCES} 
	${DBCONN_HEADERS} 
	${DBCONN_SOURCES} 
	${FACEDATA_HEADERS} 
	${FACEDATA_SOURCES} 
	${QUERYS_HEADERS} 
	${QUERYS_SOURCES} 
	${UTIL_HEADERS} 
	${UTIL_SOURCES} 
) 

TARGET_LINK_LIBRARIES ( CMUProfileImport
	sqlite3
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)

#################################
# 
#  FaceDBTool
#

FILE ( GLOB FACEDBTOOL_HEADERS   facedbtool/*.h )
FILE ( GLOB FACEDBTOOL_HEADERS   facedbtool/*.cpp )

#MESSAGE(${FACEDBTOOL_HEADERS})

ADD_EXECUTABLE ( facedbtool
	${FACEDBTOOL_HEADERS} 
	${FACEDBTOOL_SOURCES} 
	${DBCONN_HEADERS} 
	${DBCONN_SOURCES} 
	${FACEDATA_HEADERS} 
	${FACEDATA_SOURCES} 
	${QUERYS_HEADERS} 
	${QUERYS_SOURCES} 
	${UTIL_HEADERS} 
	${UTIL_SOURCES} 
) 

TARGET_LINK_LIBRARIES ( facedbtool
	sqlite3
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)

#################################
# 
#  small test with two sql files
#
ADD_EXECUTABLE ( merge_test
    tests/merge_test.cpp
    ${DBCONN_HEADERS} 
	${DBCONN_SOURCES} 
)

TARGET_LINK_LIBRARIES ( merge_test
	sqlite3
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)

#################################
# 
#  facedbsqllib
#

FILE ( GLOB FACEDBTOOL_HEADERS   facedbtool/*.h )
FILE ( GLOB FACEDBTOOL_SOURCES   facedbtool/*.cpp )

ADD_LIBRARY( facedbsqllib STATIC
	${FACEDBTOOL_HEADERS} 
	${FACEDBTOOL_SOURCES} 
	${DBCONN_HEADERS} 
	${FACEDATA_HEADERS} 
	${QUERYS_HEADERS} 
	${DBCONN_SOURCES} 
	${FACEDATA_SOURCES} 
	${QUERYS_SOURCES} 
	${UTIL_HEADERS} 
	${UTIL_SOURCES} 
) 

TARGET_LINK_LIBRARIES ( facedbsqllib
	sqlite3
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)


install(TARGETS facedbsqllib 
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
install(DIRECTORY . DESTINATION include/facedbsql
		FILES_MATCHING PATTERN "*.h"
		PATTERN ".svn" EXCLUDE 
)

#################################
# SQLITE3
#################################


IF (BUILD_SQLITE)
	ADD_LIBRARY(sqlite3 STATIC sqlite3/sqlite3.c sqlite3/sqlite3.h)

	install(TARGETS sqlite3 
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	)
	install(FILES sqlite3/sqlite3.h DESTINATION include/facedbsql)
ENDIF (BUILD_SQLITE)
