cmake_minimum_required(VERSION 3.15)

# libusb
if( SCOPEFUN_VISUALSTUDIO )
    include_external_msproject(libUsb ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/msvc/libusb_static_2017.vcxproj )
    set_target_properties(libUsb PROPERTIES FOLDER libUsb )
else()
    include(ExternalProject)
    if( SCOPEFUN_MACOSX )
        externalproject_add(libUsb
            PREFIX            ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/
            SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/
            BINARY_DIR        ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/Build/${SCOPEFUN_BUILD_TYPE}/
            CONFIGURE_COMMAND sh ../../configure --disable-shared  --enable-shared=no CXXFLAGS="-fPIC" "CFLAGS=${CMAKE_C_FLAGS}"
            BUILD_COMMAND     make
            UPDATE_COMMAND    ""
            INSTALL_COMMAND   ""
            )
    else()
        externalproject_add(libUsb
            PREFIX            ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/
            SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/
            BINARY_DIR        ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/Build/${SCOPEFUN_BUILD_TYPE}/
            CONFIGURE_COMMAND sh ../../configure --disable-shared  --enable-shared=no --enable-static CXXFLAGS=-fPIC CFLAGS=-fPIC
            BUILD_COMMAND     make
            UPDATE_COMMAND    ""
            INSTALL_COMMAND   ""
            )
    endif()
endif()

# SDL2
set( SDL_STATIC     ON CACHE BOOL "" FORCE )
set( SDL_STATIC_PIC ON CACHE BOOL "" FORCE )
set( SDL_SHARED    OFF CACHE BOOL "" FORCE )
set( SDL_HAPTIC    OFF CACHE BOOL "" FORCE )
set( SDL_JOYSTICK  OFF CACHE BOOL "" FORCE )
add_subdirectory ( SDL2-2.28.1 EXCLUDE_FROM_ALL )
set_target_properties (SDL2main     PROPERTIES FOLDER SDL2 )
set_target_properties (SDL2-static  PROPERTIES FOLDER SDL2 )
set_target_properties (uninstall    PROPERTIES FOLDER SDL2 )

# wxWidgets
if(SCOPEFUN_LINUX)
    externalproject_add(wxWidgets322
            PREFIX            ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/
            SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/
            BINARY_DIR        ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/
            CONFIGURE_COMMAND sh ../configure --disable-shared --without-libtiff
            BUILD_COMMAND     make
            UPDATE_COMMAND    ""
            INSTALL_COMMAND   ""
            )
elseif(SCOPEFUN_MACOSX)
    externalproject_add(wxWidgets322
            PREFIX            ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/
            SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/
            BINARY_DIR        ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/
            CONFIGURE_COMMAND sh ../configure --enable-debug --disable-shared --without-libtiff --with-osx_cocoa --with-macosx-version-min=10.15 --enable-macosx_arch=x86_64,arm64 CFLAGS=-fPIC
            BUILD_COMMAND     make
            UPDATE_COMMAND    ""
            INSTALL_COMMAND   ""
            )
else()
    set(wxBUILD_USE_STATIC_RUNTIME ON)
    set(wxBUILD_SHARED  OFF)
    set(wxBUILD_PRECOMP OFF)
    set(wxUSE_LIBTIFF OFF CACHE STRING "" FORCE) 
    add_subdirectory ( wxWidgets-3.2.2.1 EXCLUDE_FROM_ALL )
endif()

# pybind11
set(PYBIND11_FINDPYTHON ON)
add_subdirectory(pybind11-master)

#set(PYBIND11_FINDPYTHON ON)
#set(pybind11_DIR ${CMAKE_SOURCE_DIR}/lib/pybind11-2.11.1/)
#find_package(pybind11 REQUIRED)