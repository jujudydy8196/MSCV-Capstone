CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(Cointerest)
ADD_COMPILE_OPTIONS(-std=c++11)
FIND_PACKAGE(OpenCV REQUIRED)
#FIND_PACKAGE(Boost REQUIRED)

SET(VLFEAT_INCLUDE_DIRS "../src")

#FIND_PACKAGE(Boost COMPONENTS random system filesystem thread date_time iostreams serialization chrono REQUIRED)
# Might be needed if using homebrew
# LINK_DIRECTORIES(/usr/local/Cellar/tbb/4.4-20160128/lib)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS} ${VLFEAT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})


ADD_DEFINITIONS("-DVL_DISABLE_AVX")
FILE(GLOB SOURCES ../src/vlfeat/vl/*.c) # PATH TO vlfeat so we can do ugly glob
MESSAGE(STATUS "    sources path: ${SOURCES}")
ADD_LIBRARY(vlfeats ${SOURCES} )

ADD_EXECUTABLE(coint cointerest.cpp)
TARGET_LINK_LIBRARIES(coint ${OpenCV_LIBS} vlfeats ${OpenCV_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -lpthread -lm)
