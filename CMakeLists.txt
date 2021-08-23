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

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)

project(OpenCLExtensionLoader LANGUAGES C CXX)

option (BUILD_SHARED_LIBS "Build shared libs" ON)
option (OPENCL_EXTENSION_LOADER_FORCE_STATIC_LIB "Unconditionally build a static library for the OpenCL Extension Loader" ON)
option (OPENCL_EXTENSION_LOADER_INCLUDE_GL      "Include OpenGL Extension APIs" ON)
option (OPENCL_EXTENSION_LOADER_INCLUDE_EGL     "Include EGL Extension APIs" ON)
option (OPENCL_EXTENSION_LOADER_INCLUDE_DX9     "Include DirectX 9 Extension APIs" OFF)
option (OPENCL_EXTENSION_LOADER_INCLUDE_D3D10   "Include Direct3D 10 Extension APIs" OFF)
option (OPENCL_EXTENSION_LOADER_INCLUDE_D3D11   "Include Direct3D 11 Extension APIs" OFF)
option (OPENCL_EXTENSION_LOADER_INCLUDE_VA_API  "Include OpenGL Extension APIs" OFF)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    find_package(Python3 COMPONENTS Interpreter)
    set(OPENCL_EXTENSION_LOADER_XML_PATH CACHE FILEPATH "Path to cl.xml for OpenCL Extension Loader genereration")
    set(OPENCL_EXTENSION_LOADER_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)
    add_custom_target(extension_loader_generate
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OPENCL_EXTENSION_LOADER_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/scripts
            ${Python3_EXECUTABLE} gen_openclext.py
            -registry ${OPENCL_EXTENSION_LOADER_XML_PATH}
            -o ${OPENCL_EXTENSION_LOADER_OUTPUT_DIRECTORY}
    )
    add_custom_target(extension_loader_copy
        COMMAND ${CMAKE_COMMAND} -E copy
            ${OPENCL_EXTENSION_LOADER_OUTPUT_DIRECTORY}/openclext.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src
        COMMAND ${CMAKE_COMMAND} -E copy
            ${OPENCL_EXTENSION_LOADER_OUTPUT_DIRECTORY}/call_all.c
            ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )
    set_target_properties(extension_loader_generate PROPERTIES FOLDER "OpenCLExtensionLoader/Generation")
    set_target_properties(extension_loader_copy PROPERTIES FOLDER "OpenCLExtensionLoader/Generation")
endif()

if(NOT DEFINED OpenCL_INCLUDE_DIRS)
    find_package(OpenCL)
endif()

set( OpenCLExtensionLoader_SOURCE_FILES
    src/openclext.cpp
)

source_group(Source FILES ${OpenCLExtensionLoader_SOURCE_FILES})

if (OPENCL_EXTENSION_LOADER_FORCE_STATIC_LIB)
    add_library(OpenCLExt STATIC ${OpenCLExtensionLoader_SOURCE_FILES})
else()
    add_library(OpenCLExt ${OpenCLExtensionLoader_SOURCE_FILES})
endif()
set_target_properties(OpenCLExt PROPERTIES FOLDER "OpenCLExtensionLoader")
set_target_properties(OpenCLExt PROPERTIES VERSION "0.9" SOVERSION "0")
target_include_directories(OpenCLExt SYSTEM PUBLIC ${OpenCL_INCLUDE_DIRS})
target_compile_definitions(OpenCLExt PRIVATE CL_TARGET_OPENCL_VERSION=300)
if (OPENCL_EXTENSION_LOADER_INCLUDE_GL)
    target_compile_definitions(OpenCLExt PRIVATE CLEXT_INCLUDE_GL)
endif()
if (OPENCL_EXTENSION_LOADER_INCLUDE_EGL)
    target_compile_definitions(OpenCLExt PRIVATE CLEXT_INCLUDE_EGL)
endif()
if (OPENCL_EXTENSION_LOADER_INCLUDE_DX9)
    target_compile_definitions(OpenCLExt PRIVATE CLEXT_INCLUDE_DX9)
endif()
if (OPENCL_EXTENSION_LOADER_INCLUDE_D3D10)
    target_compile_definitions(OpenCLExt PRIVATE CLEXT_INCLUDE_D3D10)
endif()
if (OPENCL_EXTENSION_LOADER_INCLUDE_D3D11)
    target_compile_definitions(OpenCLExt PRIVATE CLEXT_INCLUDE_D3D11)
endif()
if (OPENCL_EXTENSION_LOADER_INCLUDE_VA_API)
    target_compile_definitions(OpenCLExt PRIVATE CLEXT_INCLUDE_VA_API)
endif()
target_link_libraries(OpenCLExt PRIVATE ${OpenCL_LIBRARIES})

if(MSVC)
    #/EHs enable C++ EH (no SEH exceptions)
    #/EHc extern "C" defaults to nothrow
    target_compile_options(OpenCLExt PRIVATE /EHs- /EHc-)
else()
    target_compile_options(OpenCLExt PRIVATE -Wall -fno-exceptions)
endif()

include(GNUInstallDirs)
install(TARGETS OpenCLExt
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Install Path" FORCE)
endif()

