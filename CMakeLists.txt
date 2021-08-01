# Copyright (c) 2021 Ben Ashbaugh
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_CXX_STANDARD 11)

project(OpenCLExtensionLoader LANGUAGES C CXX)

option(BUILD_SHARED_LIBS "Set to ON to build a shared library." OFF)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

if(NOT DEFINED OpenCL_INCLUDE_DIRS)
    find_package(OpenCL)
endif()

set( OpenCLExtensionLoader_SOURCE_FILES
    src/openclext.cpp
)

source_group(Source FILES ${OpenCLExtensionLoader_SOURCE_FILES})

add_library(OpenCLExt ${OpenCLExtensionLoader_SOURCE_FILES})
set_target_properties(OpenCLExt PROPERTIES FOLDER "OpenCLExtensionLoader")
target_include_directories(OpenCLExt PUBLIC ${OpenCL_INCLUDE_DIRS})
target_compile_definitions(OpenCLExt PRIVATE CL_TARGET_OPENCL_VERSION=300)
target_link_libraries(OpenCLExt PRIVATE ${CMAKE_DL_LIBS})

if(MSVC)
    target_compile_options(OpenCLExt PRIVATE /EHs-c-)
else()
    target_compile_options(OpenCLExt PRIVATE -Wall -fno-exceptions)
endif()

include(GNUInstallDirs)
install(TARGETS OpenCLExt
    EXPORT OpenCLExt
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
