cmake_minimum_required(VERSION 3.5)

project(tinyxml)

set(SOURCE_FILES
	tinyxmlparser.cpp
	tinyxmlerror.cpp
	tinyxml.cpp
	tinystr.cpp)

add_library(tinyxml ${SOURCE_FILES})