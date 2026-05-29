cmake_minimum_required(VERSION 3.15)
# project
project(ScopeFun)

# policy
cmake_policy(SET CMP0091 NEW)

# folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# pic
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# timestamp
string(TIMESTAMP SCOPEFUN_VERSION_TIMESTAMP "%H:%M %d-%m-%Y")

# version
set (SCOPEFUN_VERSION_MAJOR  "2" CACHE STRING "major version" )
set (SCOPEFUN_VERSION_MINOR  "4" CACHE STRING "minor version" )
set (SCOPEFUN_VERSION_MICRO  "0" CACHE STRING "micro version" )

#统一输出目录：将可执行文件放到 build/bin，库/静态库放到 build/lib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 为多配置生成器（Visual Studio 等）设置 per-config 输出目录
if (CMAKE_GENERATOR MATCHES "Visual Studio" OR CMAKE_CONFIGURATION_TYPES)
	foreach(config IN LISTS CMAKE_CONFIGURATION_TYPES)
		string(TOUPPER "${config}" config_upper)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/bin)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/lib)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/lib)
	endforeach()
endif()

# debug & release
set ( CMAKE_CONFIGURATION_TYPES Debug Release )

# visual Studio
if (CMAKE_GENERATOR MATCHES "Visual Studio")
	set(SCOPEFUN_VISUALSTUDIO TRUE CACHE BOOL "visual studio build")
else()
	set(SCOPEFUN_VISUALSTUDIO FALSE CACHE BOOL "not visual studio")
endif()

# type
set( SCOPEFUN_BUILD_TYPE "" CACHE STRING "set build type" )
if ( ${SCOPEFUN_BUILD_TYPE} MATCHES "Release")
	if(SCOPEFUN_VISUALSTUDIO)
		set (SCOPEFUN_VERSION_TYPE "not for public release" CACHE STRING "version information string" FORCE)
	else()
		set (SCOPEFUN_VERSION_TYPE "release" CACHE STRING "version information string" FORCE)
	endif()
	set (SCOPEFUN_BUILD_TYPE "Release" )
	set (SCOPEFUN_EXE_NAME   "scopefun" )
	set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type: Release" FORCE)
endif()
if ( ${SCOPEFUN_BUILD_TYPE} MATCHES "Debug")
   	set (SCOPEFUN_VERSION_TYPE "debug" CACHE STRING "version information string" FORCE)
	set (SCOPEFUN_BUILD_TYPE   "Debug")
	set (SCOPEFUN_EXE_NAME     "sfDebug" )
	set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type: Debug" FORCE)
endif()
if( ${SCOPEFUN_BUILD_TYPE} MATCHES "Debug" )
	message(STATUS "Debug build.")
elseif( ${SCOPEFUN_BUILD_TYPE} MATCHES "Release" )
	message(STATUS "Release build.")
else()
	message(FATAL_ERROR "Specify Debug or Release SCOPEFUN_BUILD_TYPE !")
endif()

# visual studio runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# platform
if (WIN32)
    set(SCOPEFUN_WINDOWS TRUE  CACHE BOOL "os type")
    set(SCOPEFUN_LINUX   FALSE CACHE BOOL "os type")
    set(SCOPEFUN_MACOSX  FALSE CACHE BOOL "os type")
endif()
if (UNIX AND NOT APPLE)
    set(SCOPEFUN_WINDOWS FALSE CACHE BOOL "os type")
    set(SCOPEFUN_LINUX   TRUE  CACHE BOOL "os type")
    set(SCOPEFUN_MACOSX  FALSE CACHE BOOL "os type")
endif()
if (APPLE)
    set(SCOPEFUN_WINDOWS FALSE CACHE BOOL "os type")
    set(SCOPEFUN_LINUX   FALSE CACHE BOOL "os type")
    set(SCOPEFUN_MACOSX  TRUE  CACHE BOOL "os type")
endif()

# bundle
if(MACOSX)
	set(CMAKE_MACOSX_BUNDLE "true" )
endif()

# subfolders
add_subdirectory(lib)
add_subdirectory(source)
