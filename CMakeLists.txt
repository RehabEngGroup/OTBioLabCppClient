cmake_minimum_required(VERSION 2.8)
project(OTBioLabCppClient)

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS thread date_time program_options filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


add_subdirectory(src)
