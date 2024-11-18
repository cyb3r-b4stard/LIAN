cmake_minimum_required(VERSION 3.5)

project(LianSearch)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCE_FILES 
	main.cpp
	src/liansearch.cpp
	src/openlist.cpp
	src/config.cpp
	src/map.cpp
	src/mission.cpp
	src/xmllogger.cpp)

set(HEADER_FILES
	include/searchresult.h
	include/gl_const.h
	include/liansearch.h
	include/node.h
	include/openlist.h
	include/config.h
	include/map.h
	include/logger.h
	include/mission.h
	include/search.h
	include/xmllogger.h)

add_subdirectory(tinyxml)

add_executable(LianSearch ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(LianSearch PUBLIC ${PROJECT_SOURCE_DIR}/tinyxml)
link_directories(LianSearch PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(LianSearch PUBLIC tinyxml)