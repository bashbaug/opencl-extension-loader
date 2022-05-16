# OpenCL Extension Loader
GitHub Actions: [![GitHub Actions Build Status](https://github.com/bashbaug/opencl-extension-loader/actions/workflows/cmake.yml/badge.svg?branch=main)](https://github.com/bashbaug/opencl-extension-loader/actions/workflows/cmake.yml)

This repository contains an extension loader library for OpenCL extension APIs.

The extension loader library provides function definitions for all known OpenCL extension APIs and code to query the extension APIs, allowing them to be called just like core OpenCL APIs.

## How to Build the OpenCL Extension Loader

The OpenCL Extension Loader uses CMake to generate build files.
It is regularly used and tested on Windows and Linux.

The OpenCL Extension Loader requires the following external dependencies: OpenCL headers and an OpenCL library to link with.
These dependencies can be provided via several different methods:

1. Via CMake's `find_package(OpenCL)`.
For many operating systems, if OpenCL packages or vendor-specific SDKs are installed in standard locations, they will be found automatically.
Updated CMake packages may also be provided by the Khronos [OpenCL-SDK](https://github.com/KhronosGroup/OpenCL-SDK).

1. Specified manually via the CMake variables `OpenCL_INCLUDE_DIRS` and `OpenCL_LIBRARIES`.
For these variables, please note that `OpenCL_INCLUDE_DIRS` should be the full path to the OpenCL headers (the directory containing the `CL` directory), and that `OpenCL_LIBRARIES` should be the full filepath to the OpenCL library (e.g. `OpenCL.dll` or `libOpenCL.so`).

The OpenCL Extension Loader may also be used as a CMake sub-project, in which case it will inherit the CMake variables `OpenCL_INCLUDE_DIRS` and `OpenCL_LIBRARIES` if set, and will `find_package(OpenCL)` otherwise.

## How to Use the OpenCL Extension Loader

After building the OpenCL Extension Loader, simply link with it as you would link to the standard OpenCL libraries.
This will satisfy build dependencies for OpenCL extension APIs, allowing them to be called just like core OpenCL APIs.

For example usage, please see [SimpleOpenCLSamples](https://github.com/bashbaug/SimpleOpenCLSamples).

## How to Generate Support for New Extensions

The OpenCL Extension Loader is generated from API definitions in the [OpenCL XML File](https://github.com/KhronosGroup/OpenCL-Docs/blob/main/xml/cl.xml) and several [Python Mako Templates](https://www.makotemplates.org/).
To add support for many new extensions, simply run the `gen_openclext.py` script in the `scripts` directory.
To simplify generation, there is also a CMake target `extension_loader_generate`, which will invoke the script.

After generation, examine the generated files to ensure everything looks correct, then copy the generated files to the OpenCL Extension Loader source directories.
This can be done manually, or via the CMake target `extension_loader_copy`.

In some cases, the scripts themselves may need to be modified.
This typically happens when an extension adds a new type.
When an extension adds a new type:

* The OpenCL Extension Loader template file `openclext.cpp.mako` may need to add a function to get the OpenCL Extension Loader dispatch table from an object of the new type.
* The OpenCL Extension Loader template file may also need to be modified if new extension functions are atypical and the OpenCL Extension Loader dispatch table should not be queried from the first function parameter.
* The OpenCL Extension Loader test generation template file may need to be updated to add default values for the new type.

The OpenCL Extension Loader is most commonly used with the upstream OpenCL XML File, but it can also be used with a fork of the upstream OpenCL XML File, say to generate functions for a private or as-yet unreleased extension.

## CMake Variables

The following CMake variables are supported.  To specify one of these variables
via the command line generator, use the CMake syntax `-D<option name>=<value>`.
See your CMake documentation for more details.

| Variable | Type | Description |
|:---------|:-----|:------------|
|OPENCL\_EXTENSION\_LOADER\_FORCE\_STATIC\_LIB     | BOOL | Unconditionally Build a Static OpenCL Extension Loader.  A static library can simplify building and distribution.  Default: `TRUE`
|OPENCL\_EXTENSION\_LOADER\_SINGLE\_PLATFORM\_ONLY | BOOL | Only Support Extensions from a Single OpenCL Platform.  This may improve performance by simplifying dispatch table lookups.  Default: `FALSE`
|OPENCL\_EXTENSION\_LOADER\_INSTALL                | BOOL | Generate a CMake Installation Target for the OpenCL Extension Loader.  Default: `FALSE`
|OPENCL\_EXTENSION\_LOADER\_INCLUDE\_GL            | BOOL | Include OpenGL Interop Extension APIs.  Requires OpenGL Headers.  Default: `TRUE`
|OPENCL\_EXTENSION\_LOADER\_INCLUDE\_EGL           | BOOL | Include EGL Interop Extension APIs.  Requires EGL Headers.  Default: `TRUE`
|OPENCL\_EXTENSION\_LOADER\_INCLUDE\_DX9           | BOOL | Include DirectX 9 Interop Extension APIs.  Requires DirectX 9 Headers.  Default: `FALSE`
|OPENCL\_EXTENSION\_LOADER\_INCLUDE\_D3D10         | BOOL | Include Direct3D 10 Interop Extension APIs.  Requires DirectX 10 Headers.  Default: `FALSE`
|OPENCL\_EXTENSION\_LOADER\_INCLUDE\_D3D11         | BOOL | Include Direct3D 11 Interop Extension APIs.  Required DirectX 11 Headers.  Default: `FALSE`
|OPENCL\_EXTENSION\_LOADER\_INCLUDE\_VA\_API       | BOOL | Include VA_API Interop Extension APIs.  Requires support for VA_API Headers.  Default: `FALSE`
|OPENCL\_EXTENSION\_LOADER\_XML\_PATH              | FILEPATH | Full path to an XML file (e.g. `cl.xml`) for OpenCL Extension Loader generation.  Default: None.

---

OpenCL and the OpenCL logo are trademarks of Apple Inc. used by permission by Khronos.

\* Other names and brands may be claimed as the property of others.
