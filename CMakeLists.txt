cmake_minimum_required(VERSION 2.8)
project(OTBioLabCppClient)


if (WIN32)
  set(CMAKE_CXX_FLAGS "/EHsc")# -std=c++11 -fpermissive")
endif(WIN32)
if(UNIX)
  set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_CXX_FLAGS "-O3 -Wall")
  set(CMAKE_EXE_LINKER_FLAGS "-O3")
 set(CMAKE_SHARED_LINKER_FLAGS "-O3")
ENDIF(UNIX)

SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "C:/opt/boost_1_53_0")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/opt/boost_1_53_0/stage/lib")

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS thread date_time program_options filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory(src)
