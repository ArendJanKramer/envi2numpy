cmake_minimum_required (VERSION 2.6)
set (CMAKE_CXX_STANDARD 11)

project (envi2numpy)

set (EXE_NAME "convert")

set (SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/pythonwrapper.cpp" envi_parser.h)
list (APPEND SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/cnpy.cpp")
list (APPEND SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/envi_parser.cpp")
list (APPEND SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/INIReader.cpp")
list (APPEND SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/ini.c")

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
#target_link_libraries(${EXE_NAME} ${PYTHON_LIBRARIES})

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
#include_directories("${PROJECT_BINARY_DIR}")

# add the executable

add_executable(${EXE_NAME} ${SRC_FILES})

