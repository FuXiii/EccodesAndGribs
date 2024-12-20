/*
 * (C) Copyright 2011- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ECCODES_ecbuild_config_h
#define ECCODES_ecbuild_config_h

/* ecbuild info */

#ifndef ECBUILD_VERSION_STR
#define ECBUILD_VERSION_STR "3.9.0"
#endif
#ifndef ECBUILD_VERSION
#define ECBUILD_VERSION "3.9.0"
#endif
#ifndef ECBUILD_MACROS_DIR
#define ECBUILD_MACROS_DIR  "E:/TestDelete/eccodes-2.39.0-Source/cmake"
#endif

/* config info */

#define ECCODES_OS_NAME          "Windows-10.0.19045"
#define ECCODES_OS_BITS          64
#define ECCODES_OS_BITS_STR      "64"
#define ECCODES_OS_STR           "windows.64"
#define ECCODES_OS_VERSION       "10.0.19045"
#define ECCODES_SYS_PROCESSOR    "AMD64"

#define ECCODES_BUILD_TIMESTAMP  ""
#define ECCODES_BUILD_TYPE       "Release"

#define ECCODES_C_COMPILER_ID      "MSVC"
#define ECCODES_C_COMPILER_VERSION "19.29.30143.0"

#define ECCODES_CXX_COMPILER_ID      "MSVC"
#define ECCODES_CXX_COMPILER_VERSION "19.29.30143.0"

#define ECCODES_C_COMPILER       "F:/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe"
#define ECCODES_C_FLAGS          "/DWIN32 /D_WINDOWS /W3 /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_DEPRECATE /wd4267 /MD /O2 /Ob2 /DNDEBUG"

#define ECCODES_CXX_COMPILER     "F:/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe"
#define ECCODES_CXX_FLAGS        "/DWIN32 /D_WINDOWS /W3 /GR /EHsc /MD /O2 /Ob2 /DNDEBUG"

/* Needed for finding per package config files */

#define ECCODES_INSTALL_DIR       "E:/TestDelete/eccodes-2.39.0-Source/build/install"
#define ECCODES_INSTALL_BIN_DIR   "E:/TestDelete/eccodes-2.39.0-Source/build/install/bin"
#define ECCODES_INSTALL_LIB_DIR   "E:/TestDelete/eccodes-2.39.0-Source/build/install/lib"
#define ECCODES_INSTALL_DATA_DIR  "E:/TestDelete/eccodes-2.39.0-Source/build/install/share/eccodes"

#define ECCODES_DEVELOPER_SRC_DIR "E:/TestDelete/eccodes-2.39.0-Source"
#define ECCODES_DEVELOPER_BIN_DIR "E:/TestDelete/eccodes-2.39.0-Source/build"

/* Fortran support */

#if 0

#define ECCODES_Fortran_COMPILER_ID      ""
#define ECCODES_Fortran_COMPILER_VERSION ""

#define ECCODES_Fortran_COMPILER ""
#define ECCODES_Fortran_FLAGS    ""

#endif

#endif /* ECCODES_ecbuild_config_h */
