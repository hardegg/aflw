PROJECT(PoseAnnotationGuiSQL CXX C)

cmake_minimum_required(VERSION 2.6.0)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

FILE(TO_CMAKE_PATH "$ENV{CMAKE_MODULE_PATH}" OT_CMAKE_MODULE_PATH)
FILE(TO_CMAKE_PATH "$ENV{ICG_CMAKE_MODULE_PATH}" OT_ICG_CMAKE_MODULE_PATH)
SET(CMAKE_MODULE_PATH ${OT_CMAKE_MODULE_PATH} ${OT_ICG_CMAKE_MODULE_PATH})

SET(CMAKE_VERBOSE_MAKEFILE ON) 

if(WIN32)
  # for msvc 8 deprecated warning removal
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
  ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE_GLOBALS)
endif(WIN32)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

IF(WIN32)
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:libboost_program_options-vc80-mt-1_34.lib,libboost_program_options-vc80-mt-gd-1_34.lib")
  MESSAGE(${CMAKE_SHARED_LINKER_FLAGS})
  #/NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libcmtd.lib /NODEFAULTLIB:libcpmt.lib /NODEFAULTLIB:libcpmtd.lib
ENDIF(WIN32)

###############################################################################
# Qt:
###############################################################################

set(QT_USE_QTOPENGL TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

###############################################################################
# Coin:
###############################################################################

FIND_PACKAGE(Coin REQUIRED)
INCLUDE_DIRECTORIES(${COIN_INCLUDE_DIR})
LINK_DIRECTORIES(${COIN_LINK_DIRECTORIES})
ADD_DEFINITIONS(${COIN_DEFINITIONS})

###############################################################################
# Quarter:
###############################################################################

FIND_PACKAGE(Quarter REQUIRED)
INCLUDE_DIRECTORIES(${QUARTER_INCLUDE_DIR})
LINK_DIRECTORIES(${QUARTER_LINK_DIRECTORIES})
ADD_DEFINITIONS(${QUARTER_DEFINITIONS})

###############################################################################
# GLEW
###############################################################################

FIND_PACKAGE(GLEW REQUIRED)
INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR})

###############################################################################
# OpenCV
###############################################################################

SET( OpenCV_DIR $ENV{OpenCV_DIR})
FIND_PACKAGE(OpenCV REQUIRED)
IF(OpenCV_FOUND)
#	Message("opencv root dir: '${OpenCV_ROOT_DIR}'")
#	Message("opencv include dirs: '${OpenCV_INCLUDE_DIRS}'")
  INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${OpenCV_LINK_DIRECTORIES} )
#MESSAGE("OpenCV_LINK_DIRECTORIES " ${OpenCV_LINK_DIRECTORIES} )
#MESSAGE("OpenCV_LIBS " ${OpenCV_LIBS} )
ELSE(OpenCV_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without OpenCV.  Please set OPENCV_DIR.")
ENDIF(OpenCV_FOUND)

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

###############################################################################
# facedbsql
###############################################################################

FIND_PACKAGE(FaceDBSql QUIET REQUIRED)
INCLUDE_DIRECTORIES(${FaceDBSql_INCLUDE_DIRS})
LINK_DIRECTORIES(${FaceDBSql_LINK_DIRECTORIES})

#MESSAGE("FaceDBSql_LIBRARIES" ${FaceDBSql_LIBRARIES})

###############################################################################
# Boost
###############################################################################

IF (WIN32)
	IF(CMAKE_CL_64)
		SET(BOOST_INCLUDEDIR $ENV{BOOST_ROOT_x64}/include/boost-1_45)
		SET(BOOST_LIBRARYDIR $ENV{BOOST_ROOT_x64}/lib)
	ELSE(CMAKE_CL_64)
		SET(BOOST_INCLUDEDIR $ENV{BOOST_ROOT_x86}/include/boost-1_45)
		SET(BOOST_LIBRARYDIR $ENV{BOOST_ROOT_x86}/lib)
	ENDIF(CMAKE_CL_64)
ENDIF(WIN32)

SET ( Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE(Boost COMPONENTS program_options filesystem system regex REQUIRED)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${Boost_LIBRARY_DIRS} )

#MESSAGE( "boost incl dirs: " ${Boost_INCLUDE_DIRS})
#MESSAGE( "boost lib dirs: " ${Boost_LIBRARY_DIRS})

###############################################################################
# MAIN TARGET
###############################################################################

SET( MOC_INPUT_FILES
     main_window.h
     FeaturePointRadioButton.h
	 qpixeltool.h
	 ImageViewer.h
)

QT4_WRAP_CPP( MOC_OUTPUT_FILES ${MOC_INPUT_FILES} )

SET( GUI_SOURCES 
  main.cpp
  main_window.h
  main_window.cpp
  pose_estimation_quarter_widget.h
  pose_estimation_quarter_widget.cpp
  FeaturePointRadioButton.h
  FeaturePointRadioButton.cpp
  utils.h
  qpixeltool.h
  qpixeltool.cpp
  ImageViewer.h
  ImageViewer.cpp
)

ADD_EXECUTABLE( PoseAnnotationGuiSQL 
		${GUI_SOURCES} 
		${MOC_OUTPUT_FILES} 
		PoseEstimationAnnotationGui.rc
)

TARGET_LINK_LIBRARIES( PoseAnnotationGuiSQL 
                       ${OpenCV_LIBS} 
                       ${GLEW_LIBRARIES} 
                       ${COIN_LIBRARIES}
                       ${QUARTER_LIBRARIES} 
                       ${QT_LIBRARIES} 
                       ${ITK_LIBRARIES} 
					   ${FaceDBSql_LIBRARIES}
					   ${Boost_LIBRARIES}
                       optimized sqlite3
					   debug sqlite3
)

