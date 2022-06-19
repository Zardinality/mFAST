#########################################################################
#  The following lines shows how to write CMakeLists.txt for mFAST applications
#  when the applications are not in the mFAST source tree.
##########################################################################

# cmake_minimum_required(VERSION 2.8)
# find_package(mFAST REQUIRED COMPONENTS coder)
#
# include_directories(${MFAST_INCLUDE_DIR})
# link_directories(${MFAST_LIBRARY_DIRS})

if(SKBUILD)
  # Scikit-Build does not add your site-packages to the search path
  # automatically, so we need to add it _or_ the pybind11 specific directory
  # here.
  execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c
            "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE _tmp_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT)
  list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")
endif()

# Now we can find pybind11
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(_core MODULE _core.cpp)
target_link_libraries (_core ${MFAST_LIBRARIES})

# add_library(pybind11-demo pybind11-demo.cpp)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
