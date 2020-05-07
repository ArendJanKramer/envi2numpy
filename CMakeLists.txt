cmake_minimum_required (VERSION 2.6)
set (CMAKE_CXX_STANDARD 11)

project (envi2numpy)

set (EXE_NAME "convert")

set (SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/src/pythonwrapper.cpp" src/envi_parser.h)
list (APPEND SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/src/cnpy.cpp")
list (APPEND SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/src/envi_parser.cpp")
list (APPEND SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/src/numpy_helpers.cpp")
list (APPEND SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/src/numpy_helpers.h")
list (APPEND SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/src/INIReader.cpp")
list (APPEND SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/src/ini.c")

#Use -DOVERRIDE_PYTHON=OFF
set(OVERRIDE_PYTHON ON) #specify OFF to use find_package to use the system Python instead of anaconda.

if (OVERRIDE_PYTHON)
    if (NOT DEFINED PYTHON_ENV)
        ##PYTHON 3.6 Linux (conda)
        set(PYTHON_ENV "/home/arendjan/miniconda3/envs/arendjan")
        set(PYTHON_LIB1 "python3.6m")
        set(PYTHON_LIB2 "python3.6")
    endif()
    link_directories("${PYTHON_ENV}/lib") #Location of PYTHON_LIBRARY
    set(PYTHON_LIBRARY ${PYTHON_LIB1})
    set(PYTHON_INCLUDE_DIRS "${PYTHON_ENV}/include/${PYTHON_LIB1}")
    set(NUMPY_INCLUDE_DIRS  "${PYTHON_ENV}/lib/${PYTHON_LIB2}/site-packages/numpy/core/include")
    message("Overriding system Python location with ${PYTHON_ENV} with lib: ${PYTHON_LIBRARY} and include: ${PYTHON_INCLUDE_DIRS} ${NUMPY_INCLUDE_DIRS}")
endif()

add_executable(${EXE_NAME} ${SRC_FILES})

if (NOT OVERRIDE_PYTHON)
    find_package(PythonLibs REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIRS})

    message("PYTHON_INCLUDE_DIRS=" ${PYTHON_INCLUDE_DIRS})
    message("PYTHON_LIBRARIES=" ${PYTHON_LIBRARIES})
    target_include_directories(${EXE_NAME} PUBLIC ${PYTHON_INCLUDE_DIRS})
    target_include_directories(${EXE_NAME} PUBLIC "/usr/local/lib/python3.6/dist-packages/numpy/core/include/")
    target_link_libraries(${EXE_NAME} ${PYTHON_LIBRARIES})
else()
    target_include_directories(${EXE_NAME} PUBLIC "${PYTHON_INCLUDE_DIRS}")
    target_include_directories(${EXE_NAME} PUBLIC "${NUMPY_INCLUDE_DIRS}")
    target_link_libraries(${EXE_NAME} ${PYTHON_LIBRARY})
endif()


