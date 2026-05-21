cmake_minimum_required(VERSION 3.15)

#--------------------------------------------------------------------------------
# version
#--------------------------------------------------------------------------------
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/source/version/sfversion.h.in ${CMAKE_SOURCE_DIR}/source/version/sfversion.h @ONLY)

#--------------------------------------------------------------------------------
# cpp files
#--------------------------------------------------------------------------------
set(SOURCE_CORE ${CMAKE_SOURCE_DIR}/source/core/core.cpp
                ${CMAKE_SOURCE_DIR}/source/core/render/camera/camera.cpp
                ${CMAKE_SOURCE_DIR}/source/core/render/canvas/canvas2d.cpp
                ${CMAKE_SOURCE_DIR}/source/core/render/canvas/canvas3d.cpp
                ${CMAKE_SOURCE_DIR}/source/core/render/render/render.cpp
                ${CMAKE_SOURCE_DIR}/source/core/render/font/font.cpp
                ${CMAKE_SOURCE_DIR}/source/core/string/corestring.cpp
                ${CMAKE_SOURCE_DIR}/source/core/file/file.cpp
                ${CMAKE_SOURCE_DIR}/source/core/format/format.cpp
                ${CMAKE_SOURCE_DIR}/source/core/input/input.cpp
                ${CMAKE_SOURCE_DIR}/source/core/manager/manager.cpp
                ${CMAKE_SOURCE_DIR}/source/core/memory/memory.cpp
                ${CMAKE_SOURCE_DIR}/source/core/opengl/opengl.cpp
                ${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_device.cpp
                ${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_devicestate.cpp
                ${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_shader.cpp
                ${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_staticmesh.cpp
                ${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_texture.cpp
                ${CMAKE_SOURCE_DIR}/source/core/purec/purec.c
                ${CMAKE_SOURCE_DIR}/source/core/purec/puresocket.c
                ${CMAKE_SOURCE_DIR}/source/core/purec/pureusb.c
                ${CMAKE_SOURCE_DIR}/source/core/timer/timer.cpp )

set(SOURCE_SCOPEFUN ${CMAKE_SOURCE_DIR}/source/scopefun/app/app.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/app/managers.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/gui/osc.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopConnection.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopDebug.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopDisplay.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopInfo.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopMeasure.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopOsciloskop.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopStorage.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopThermal.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopTools.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/osc/osccontrol.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/osc/oscfft.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/osc/oscfile.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/osc/oscmng.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/osc/oscrender.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/osc/oscsettings.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/osc/oscsignal.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/window/tool.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/window/wnddisplay.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/window/wndmain.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/window/wndshadow.cpp
                    ${CMAKE_SOURCE_DIR}/source/scopefun/xpm/xpm128.c
                    ${CMAKE_SOURCE_DIR}/source/scopefun/xpm/xpm16.c
                    ${CMAKE_SOURCE_DIR}/source/scopefun/xpm/xpm256.c
                    ${CMAKE_SOURCE_DIR}/source/scopefun/xpm/xpm32.c
                    ${CMAKE_SOURCE_DIR}/source/scopefun/xpm/xpm512.c
                    ${CMAKE_SOURCE_DIR}/source/scopefun/xpm/xpm64.c
                    ${CMAKE_SOURCE_DIR}/source/scopefun/xpm/xpm96.c )

set( HEADER_CORE ${CMAKE_SOURCE_DIR}/source/core/core.h
                 ${CMAKE_SOURCE_DIR}/source/core/render/camera/camera.h
                 ${CMAKE_SOURCE_DIR}/source/core/render/canvas/canvas2d.h
                 ${CMAKE_SOURCE_DIR}/source/core/render/canvas/canvas3d.h
                 ${CMAKE_SOURCE_DIR}/source/core/string/corestring.h
                 ${CMAKE_SOURCE_DIR}/source/core/global/endian.h
                 ${CMAKE_SOURCE_DIR}/source/core/file/file.h
                 ${CMAKE_SOURCE_DIR}/source/core/flag/flag.h
                 ${CMAKE_SOURCE_DIR}/source/core/render/font/font.h
                 ${CMAKE_SOURCE_DIR}/source/core/format/format.h
                 ${CMAKE_SOURCE_DIR}/source/core/input/input.h
                 ${CMAKE_SOURCE_DIR}/source/core/global/macro.h
                 ${CMAKE_SOURCE_DIR}/source/core/manager/manager.h
                 ${CMAKE_SOURCE_DIR}/source/core/memory/memory.h
                 ${CMAKE_SOURCE_DIR}/source/core/opengl/opengl.h
                 ${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_device.h
                 ${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_devicestate.h
                 ${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_shader.h
                 ${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_staticmesh.h
                 ${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_texture.h
                 ${CMAKE_SOURCE_DIR}/source/core/global/platform.h
                 ${CMAKE_SOURCE_DIR}/source/core/purec/purec.h
                 ${CMAKE_SOURCE_DIR}/source/core/purec/puresocket.h
                 ${CMAKE_SOURCE_DIR}/source/core/purec/pureusb.h
                 ${CMAKE_SOURCE_DIR}/source/core/render/render/render.h
                 ${CMAKE_SOURCE_DIR}/source/core/data/ring.h
                 ${CMAKE_SOURCE_DIR}/source/core/data/set.h
                 ${CMAKE_SOURCE_DIR}/source/core/data/map.h
                 ${CMAKE_SOURCE_DIR}/source/core/data/array.h
                 ${CMAKE_SOURCE_DIR}/source/core/timer/timer.h
                 ${CMAKE_SOURCE_DIR}/source/core/global/type.h
                 ${CMAKE_SOURCE_DIR}/source/core/math/vector4.h
                 ${CMAKE_SOURCE_DIR}/source/core/math/matrix4x4.h
                 ${CMAKE_SOURCE_DIR}/source/core/math/interval.h
                 ${CMAKE_SOURCE_DIR}/source/core/math/const.h )

set( HEADER_SCOPEFUN ${CMAKE_SOURCE_DIR}/source/scopefun/ScopeFun.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/guiheader.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/osc.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopConnection.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopDebug.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopDisplay.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopHardwareGenerator.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopInfo.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopMeasure.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopOsciloskop.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopSoftwareGenerator.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopStorage.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopThermal.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/gui/OsciloskopTools.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/osc/oscmng.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/osc/oscrender.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/osc/oscsettings.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/osc/oscsignal.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/osc/osccontrol.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/osc/oscfft.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/osc/oscfile.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/window/tool.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/window/wnddisplay.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/window/wndmain.h
                     ${CMAKE_SOURCE_DIR}/source/scopefun/window/wndshadow.h )

set( FILES_LIB
"${CMAKE_SOURCE_DIR}/lib/cJSON/cJSON.c"
"${CMAKE_SOURCE_DIR}/lib/glew-1.13.0/src/glew.c"
"${CMAKE_SOURCE_DIR}/lib/kiss_fft130/kiss_fft.c"
"${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/examples/ezusb.c"
)

set( FILES_API "${CMAKE_SOURCE_DIR}/source/api/scopefunapi.c" )

set( FILES_TOOL "${CMAKE_SOURCE_DIR}/source/license/license.cpp" )

#--------------------------------------------------------------------------------
# target executable
#--------------------------------------------------------------------------------
if(SCOPEFUN_WINDOWS)
    set( SCOPEFUN_EXE_TYPE WIN32 )
endif()
if(SCOPEFUN_MACOSX)
    set( SCOPEFUN_EXE_TYPE MACOSX_BUNDLE )
endif()
if(SCOPEFUN_LINUX)
    set( SCOPEFUN_EXE_TYPE "" )
endif()
if( SCOPEFUN_VISUALSTUDIO)
    add_executable(ScopeFun ${SCOPEFUN_EXE_TYPE} ${HEADER_CORE} ${HEADER_SCOPEFUN} ${SOURCE_CORE} ${SOURCE_SCOPEFUN} ${FILES_LIB} ${FILES_API} ${FILES_TOOL} )
else()
    add_executable(ScopeFun ${SCOPEFUN_EXE_TYPE} ${SOURCE_CORE} ${SOURCE_SCOPEFUN} ${FILES_LIB} ${FILES_API} ${FILES_TOOL} )
endif()

if(SCOPEFUN_LINUX)
    set( SCOPEFUN_EXE_TYPE "" )
endif()
set_target_properties(ScopeFun PROPERTIES OUTPUT_NAME "${SCOPEFUN_EXE_NAME}")
##set_target_properties(ScopeFun PROPERTIES EXCLUDE_FROM_ALL 1)  ##disable GUI application

#--------------------------------------------------------------------------------
# groups
#--------------------------------------------------------------------------------
source_group("Core"            FILES ${SOURCE_CORE})
source_group("ScopeFun"        FILES ${SOURCE_SCOPEFUN})
source_group("Lib"             FILES ${FILES_LIB})
source_group("Tool"            FILES ${FILES_TOOL})
source_group("Api"             FILES ${FILES_API})
source_group("Header_Core"     FILES ${HEADER_CORE} )
source_group("Header_ScopeFun" FILES ${HEADER_SCOPEFUN} )

#--------------------------------------------------------------------------------
# include
#--------------------------------------------------------------------------------
if( ${SCOPEFUN_BUILD_TYPE} MATCHES "Debug" )
    if( ${SCOPEFUN_VISUALSTUDIO} )
            set(SCOPEFUN_INCLUDE_WX_SETUP   ${CMAKE_BINARY_DIR}/lib/vc_x64_lib/mswud )
    else()
        if(SCOPEFUN_WINDOWS)
            set(SCOPEFUN_INCLUDE_WX_SETUP   ${CMAKE_BINARY_DIR}/lib/wxWidgets-3.2.2.1/lib/gcc_x64_lib/mswud )
        endif()
        if(SCOPEFUN_MACOSX)
            set(SCOPEFUN_INCLUDE_WX_SETUP     ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/lib/wx/include/osx_cocoa-unicode-static-3.2)
        endif()
        if(SCOPEFUN_LINUX)
            set(SCOPEFUN_INCLUDE_WX_SETUP     ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/lib/wx/include/gtk3-unicode-static-3.2)
        endif()
    endif()
endif()
if( ${SCOPEFUN_BUILD_TYPE} MATCHES "Release" )
    if( ${SCOPEFUN_VISUALSTUDIO} )
            set(SCOPEFUN_INCLUDE_WX_SETUP   ${CMAKE_BINARY_DIR}/lib/vc_x64_lib/mswu )
    else()
        if(SCOPEFUN_WINDOWS)
            set(SCOPEFUN_INCLUDE_WX_SETUP   ${CMAKE_BINARY_DIR}/lib/wxWidgets-3.2.2.1/lib/gcc_x64_lib/mswu )
        endif()
        if(SCOPEFUN_MACOSX)
            set(SCOPEFUN_INCLUDE_WX_SETUP   ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/lib/wx/include/osx_cocoa-unicode-static-3.2)
        endif()
        if(SCOPEFUN_LINUX)
            set(SCOPEFUN_INCLUDE_WX_SETUP   ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/lib/wx/include/gtk3-unicode-static-3.2)
        endif()
    endif()
endif()
if(SCOPEFUN_LINUX)
    set(SCOPEFUN_INCLUDE_WX         "${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/include"  CACHE PATH "include folder for wxWidgets library" FORCE)
else()
    set(SCOPEFUN_INCLUDE_WX         "${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/include"  CACHE PATH "include folder for wxWidgets library" FORCE)
endif()
set(SCOPEFUN_INCLUDE_USB            "${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/libusb"     CACHE PATH "include folder for libUsb library" FORCE)
set(SCOPEFUN_INCLUDE_USB_EX         "${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/examples"   CACHE PATH "include folder for ezusb load library" FORCE)
set(SCOPEFUN_INCLUDE_USB_CONF       "${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/Build/${SCOPEFUN_BUILD_TYPE}" CACHE PATH "include folder for libusb config.h" FORCE)
set(SCOPEFUN_INCLUDE_SDL2           "${CMAKE_SOURCE_DIR}/lib/SDL2-2.28.1/include"      CACHE PATH "include fodler for SDL2 library " FORCE)
set(SCOPEFUN_INCLUDE_CJSON          "${CMAKE_SOURCE_DIR}/lib/cJSON"                    CACHE PATH "include folder for cjson" FORCE)
set(SCOPEFUN_INCLUDE_KISSFFT        "${CMAKE_SOURCE_DIR}/lib/kissfft130"               CACHE PATH "include folder for kissfft" FORCE)
set(SCOPEFUN_INCLUDE_GLEW           "${CMAKE_SOURCE_DIR}/lib/glew-1.13.0/include"      CACHE PATH "include folder for glew" FORCE)
set(SCOPEFUN_INCLUDE_PYBIND11       "${CMAKE_SOURCE_DIR}/lib/pybind11-2.11.1/include"  CACHE PATH "include folder for pybind11" FORCE)

target_include_directories(
    ScopeFun
    PUBLIC
    ${CMAKE_SOURCE_DIR}/source
    ${SCOPEFUN_INCLUDE_USB}
    ${SCOPEFUN_INCLUDE_USB_EX}
    ${SCOPEFUN_INCLUDE_USB_CONF}
    ${SCOPEFUN_INCLUDE_SDL2}
    ${SCOPEFUN_INCLUDE_WX}
    ${SCOPEFUN_INCLUDE_WX_SETUP}
    ${SCOPEFUN_INCLUDE_CJSON}
    ${SCOPEFUN_INCLUDE_KISSFFT}
    ${SCOPEFUN_INCLUDE_GLEW}
)

#--------------------------------------------------------------------------------
# target pythonAPI
#--------------------------------------------------------------------------------
set(SCOPEFUN_API_NAME "scopefun")

## pybind11_add_module(<name> [MODULE | SHARED] [EXCLUDE_FROM_ALL] [NO_EXTRAS] [THIN_LTO] [OPT_SIZE] source1 [source2 ...])
pybind11_add_module(pythonApi
    "${CMAKE_SOURCE_DIR}/source/api/pythonApi/pythonApi.cpp"
    "${CMAKE_SOURCE_DIR}/source/api/scopefunapi.c"
    "${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/examples/ezusb.c"
    "${CMAKE_SOURCE_DIR}/source/core/purec/pureusb.c"
    "${CMAKE_SOURCE_DIR}/source/core/purec/purec.c"
    "${CMAKE_SOURCE_DIR}/source/core/purec/puresocket.c"
)

set_target_properties(pythonApi PROPERTIES OUTPUT_NAME "${SCOPEFUN_API_NAME}")

target_include_directories(pythonApi PUBLIC
    "${CMAKE_SOURCE_DIR}/source"
    ${SCOPEFUN_INCLUDE_USB}
    ${SCOPEFUN_INCLUDE_USB_EX}
    ${SCOPEFUN_INCLUDE_USB_CONF}
)

#--------------------------------------------------------------------------------
# warnings
#--------------------------------------------------------------------------------
# target_compile_options(ScopeFun PRIVATE -Wall)
# target_compile_options(ScopeFun PRIVATE -WX)

#--------------------------------------------------------------------------------
# link
#--------------------------------------------------------------------------------
if( SCOPEFUN_VISUALSTUDIO )
    set( SCOPEFUN_LIB_USB ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/Build/${SCOPEFUN_BUILD_TYPE}/libusb-1.0.lib )
else()
    set( SCOPEFUN_LIB_USB ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.25/Build/${SCOPEFUN_BUILD_TYPE}/libusb/.libs/libusb-1.0.a )
endif()

list(APPEND SCOPEFUN_LIB_SDL2 SDL2-static)

if(SCOPEFUN_LINUX)
	list( APPEND SCOPEFUN_LIB_WX core base adv propgrid richtext)
elseif(SCOPEFUN_MACOSX)
	list( APPEND SCOPEFUN_LIB_WX
		${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/lib/libwx_osx_cocoau_core-3.2.a
		${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/lib/libwx_baseu-3.2.a
		${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/lib/libwx_osx_cocoau_adv-3.2.a
		${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/lib/libwx_osx_cocoau_propgrid-3.2.a
		${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/lib/libwx_osx_cocoau_richtext-3.2.a
		${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/lib/libwxpng-3.2.a
	)
else()
    list( APPEND SCOPEFUN_LIB_WX wx::core wx::base wx::adv wx::propgrid wx::richtext wx::stc )
endif()
if(SCOPEFUN_WINDOWS)
    if( SCOPEFUN_VISUALSTUDIO )
        target_link_libraries( ScopeFun
            # libs
            ${SCOPEFUN_LIB_USB}
            ${SCOPEFUN_LIB_SDL2}
            ${SCOPEFUN_LIB_WX}
            # system
            opengl32.lib
            winmm.lib
            ole32.lib
            imm32.lib
            version.lib
            uuid.lib
            oleaut32.lib
            comctl32.lib
            winspool.lib
            ws2_32.lib
            wsock32.lib
            setupapi.lib
            hid.lib
            Rpcrt4.lib
            # res
            ${CMAKE_SOURCE_DIR}/source/scopefun/rc/osc.res
        )
    else()
        target_link_libraries(ScopeFun PRIVATE -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic
        # mingw
        mingw32.a
        # libs
        ${SCOPEFUN_LIB_USB}
        ${SCOPEFUN_LIB_SDL2}
        ${SCOPEFUN_LIB_WX}
        # system
        opengl32.a
        winmm.a
        ole32.a
        imm32.a
        version.a
        uuid.a
        oleaut32.a
        comctl32.a
        winspool.a
        ws2_32.a
        wsock32.a
        setupapi.a
        hid.a
        # res
        ${CMAKE_SOURCE_DIR}/source/scopefun/rc/osc.res
        )
    #statically link standard libraries (libgcc, libstdc++, pthread)
    target_link_libraries(pythonApi PRIVATE -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic
        # mingw
        mingw32.a
        # libs
        pybind11::module
        ${SCOPEFUN_LIB_USB}
        ${SCOPEFUN_LIB_SDL2}
        ${PYTHON_LIBRARIES}
        ws2_32.a
        wsock32.a
    )
    endif()
endif()
if(SCOPEFUN_MACOSX)
    target_link_libraries(ScopeFun
        # system
        iconv.a
        Z.a
        # libs
        ${SCOPEFUN_LIB_USB}
        ${SCOPEFUN_LIB_SDL2}
        ${SCOPEFUN_LIB_WX}
        # frameworks
        "-framework CoreFoundation"
        "-framework AudioUnit"
        "-framework CoreAudioKit"
        "-framework AudioToolbox"
        "-framework ForceFeedback"
        "-framework IOKit"
        "-framework Security"
        "-framework Carbon"
        "-framework Cocoa"
        "-framework CoreAudio"
        "-framework CoreVideo"
        "-framework WebKit"
        "-framework OpenGL"
        "-framework Metal"
    )
    target_link_libraries(pythonApi PRIVATE
        # libs
        pybind11::module
        ${SCOPEFUN_LIB_USB}
        ${SCOPEFUN_LIB_SDL2}
        ${PYTHON_LIBRARIES}
    )
endif()
if(SCOPEFUN_LINUX)
    set(SCOPEFUN_LINK_WX  "${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.2.2.1/sfBuild${SCOPEFUN_BUILD_TYPE}/lib" CACHE FILEPATH "link folder for wxWidgets" FORCE)
    set(SCOPEFUN_LIB_WX
    "${SCOPEFUN_LINK_WX}/libwx_gtk3u_adv-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwx_gtk3u_core-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwx_baseu-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwx_baseu_xml-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwx_baseu_net-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwx_gtk3u_aui-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwx_gtk3u_html-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwx_gtk3u_propgrid-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwx_gtk3u_qa-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwx_gtk3u_ribbon-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwx_gtk3u_richtext-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwx_gtk3u_xrc-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwxscintilla-3.2.a"
    "${SCOPEFUN_LINK_WX}/libwx_gtk3u_stc-3.2.a"
    "gthread-2.0"
    "pthread"
    "X11"
    "Xext"
    "Xxf86vm"
    "SM"
    "gtk-3"
    "gdk-3"
    "pangocairo-1.0"
    "atk-1.0"
    "cairo-gobject"
    "cairo"
    "gdk_pixbuf-2.0"
    "gio-2.0"
    "Xtst"
    "pangoft2-1.0"
    "pango-1.0"
    "gobject-2.0"
    "glib-2.0"
    "fontconfig"
    "freetype"
    "expat"
    "png"
    "z"
    "dl"
    "m")
    target_link_libraries(ScopeFun
        # libs
        ${SCOPEFUN_LIB_USB}
        ${SCOPEFUN_LIB_SDL2}
        ${SCOPEFUN_LIB_WX}
        "GL"
        "udev"
    )
    target_link_libraries(pythonApi PRIVATE
        # libs
        pybind11::module
        ${SCOPEFUN_LIB_USB}
        ${SCOPEFUN_LIB_SDL2}
        ${PYTHON_LIBRARIES}
        "udev"
    )
endif()

#--------------------------------------------------------------------------------
# defines
#--------------------------------------------------------------------------------
if (${SCOPEFUN_BUILD_TYPE} MATCHES "Release")
     add_definitions(-DNDEBUG)
endif()
if (${SCOPEFUN_BUILD_TYPE} MATCHES "Debug")
        add_definitions(-D_DEBUG)
endif()
add_definitions(-D__WX__)
add_definitions(-DwxUSE_UNICODE=1)
add_definitions(-DwxUSE_UNICODE_UTF8=0)
add_definitions(-DwxUSE_UNICODE_WCHAR=1)
add_definitions(-DwxUSE_COMBOCTRL=1)
add_definitions(-DwxUSE_GLOBAL_MEMORY_OPERATORS=0)
add_definitions(-DSDL_MAIN_HANDLED)
add_definitions(-DGLEW_STATIC)
add_definitions(-DGLEW_NO_GLU)
add_definitions(-DUSE_LOCKS=1)
add_definitions(-DUSE_DL_PREFIX)
add_definitions(-D_FILE_OFFSET_BITS=64)
add_definitions(-D_LARGE_FILES)
if(SCOPEFUN_WINDOWS)
    add_definitions(-DWIN64)
    add_definitions(-D__WIN64__)
    add_definitions(-DPLATFORM_WIN)
    add_definitions(-D_WINDOWS)
    if(SCOPEFUN_VISUALSTUDIO)
       add_definitions(-DMS_WIN64)
       add_definitions(-DMS_WIN64)
       add_definitions(-DPLATFORM_MSVC)
    else()
       add_definitions(-D__MINGW64__)
       add_definitions(-DPLATFORM_MINGW)
    endif()
endif()
if(SCOPEFUN_MACOSX)
   add_definitions(-DPLATFORM_MAC)
   add_definitions(-DMAC)
   add_definitions(-D__WXOSX_COCOA__)
   add_definitions(-D__WXOSX__)
   add_definitions(-D__WXMAC__)
   add_definitions(-D_WXMAC_XCODE__=1)
endif()
if(SCOPEFUN_LINUX)
   add_definitions(-DPLATFORM_LINUX)
   add_definitions(-DLINUX)
   add_definitions(-D__LINUX__)
   add_definitions(-D__WXGTK__)
endif(SCOPEFUN_LINUX)


#--------------------------------------------------------------------------------
# install
#--------------------------------------------------------------------------------
if(SCOPEFUN_LINUX)

    set(ALL_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE )
    set( SCOPEFUN_BITS 64)
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/package/scopefun-linux-${SCOPEFUN_BITS}/DEBIAN"   DESTINATION "../"
    FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS})
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/package/scopefun-linux-${SCOPEFUN_BITS}/usr"      DESTINATION "../"
    FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS})
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/package/scopefun-linux-${SCOPEFUN_BITS}/lib"      DESTINATION "../"
    FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS})
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/locale"                                       DESTINATION "./share"
    FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS})
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/data"                                         DESTINATION "./lib/oscilloscope" FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS})
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/script"                                       DESTINATION "./lib/oscilloscope" FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS})
    install(TARGETS ScopeFun
            PERMISSIONS ${ALL_PERMISSIONS}
            RUNTIME  DESTINATION "./bin"
            BUNDLE   DESTINATION "./bin" )
    install(TARGETS pythonApi
            PERMISSIONS ${ALL_PERMISSIONS}
            LIBRARY  DESTINATION "./lib/oscilloscope/script")
else()

    install(TARGETS ScopeFun
            PERMISSIONS ${ALL_PERMISSIONS}
            RUNTIME  DESTINATION "."
            BUNDLE   DESTINATION "." )

endif()
if(SCOPEFUN_MACOSX)

    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/data"                            DESTINATION ./ScopeFun.App/Contents/Resources )
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/locale"                          DESTINATION ./ScopeFun.App/Contents/Resources )
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/script"                          DESTINATION ./ScopeFun.App/Contents/Resources )
    install( FILES     "${CMAKE_SOURCE_DIR}/package/scopefun-mac/AppIcon.icns"   DESTINATION ./ScopeFun.App/Contents/Resources )

    install(TARGETS pythonApi
        PERMISSIONS ${ALL_PERMISSIONS}
        LIBRARY  DESTINATION "./ScopeFun.App/Contents/Resources/script")

    set(MACOSX_BUNDLE_INFO_STRING           "ScopeFun open source oscilloscope" )
    set(MACOSX_BUNDLE_ICON_FILE                 AppIcon )
    set(MACOSX_BUNDLE_GUI_IDENTIFIER        "ScopeFun.Oscilloscope" )
    set(MACOSX_BUNDLE_BUNDLE_NAME           "ScopeFun" )
    set(MACOSX_BUNDLE_LONG_VERSION_STRING   ${SCOPEFUN_VERSION_MAJOR}.${SCOPEFUN_VERSION_MINOR}.${SCOPEFUN_VERSION_MICRO} )
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING  ${SCOPEFUN_VERSION_MAJOR}.${SCOPEFUN_VERSION_MINOR}.${SCOPEFUN_VERSION_MICRO} )
    set(MACOSX_BUNDLE_BUNDLE_VERSION        ${SCOPEFUN_VERSION_MAJOR}.${SCOPEFUN_VERSION_MINOR}.${SCOPEFUN_VERSION_MICRO} )
    set(MACOSX_BUNDLE_COPYRIGHT             "Copyright C 2016 - 2021 David Košenina, Copyright C 2021 - 2024 Dejan Priveršek" )

    set(CPACK_BUNDLE_PLIST ${CMAKE_SOURCE_DIR}/package/scopefun-mac/info.plist )
    set(CPACK_BUNDLE_ICON  ${CMAKE_SOURCE_DIR}/package/scopefun-mac/appicon.icns )
    set(CPACK_BUNDLE_NAME "ScopeFun")

    set(CPACK_GENERATOR "DragNDrop")
endif()
if(SCOPEFUN_WINDOWS)

    set(ALL_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE )

    # install
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/data"   DESTINATION "." FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS} )
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/script" DESTINATION "." FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS} )
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/locale" DESTINATION "." FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS} )

    install(TARGETS pythonApi
            PERMISSIONS ${ALL_PERMISSIONS}
            LIBRARY  DESTINATION "./script")

    # generator
    set(CPACK_GENERATOR "NSIS")

    # version
    set(CPACK_NSIS_PACKAGE_NAME "ScopeFun" )

    # create links
    LIST(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA  "
        CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\ScopeFun.lnk'  '$INSTDIR\\\\ScopeFun${SCOPEFUN_TYPE}.exe'
        CreateShortCut '$DESKTOP\\\\ScopeFun.lnk'  '$INSTDIR\\\\ScopeFun${SCOPEFUN_TYPE}.exe'")

    # delete links
    LIST(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA  "
        Delete '$SMPROGRAMS\\\\$START_MENU\\\\ScopeFun.lnk'
        Delete '$SMPROGRAMS\\\\$START_MENU\\\\ScopeFun.lnk'
        Delete '$DESKTOP\\\\ScopeFun.lnk'
        Delete '$DESKTOP\\\\ScopeFun.lnk'")

    #icons
    set(CPACK_NSIS_MUI_ICON                       "${CMAKE_SOURCE_DIR}/package/scopefun-win\\\\logo.ico")
    set(CPACK_NSIS_MUI_UNIICON                    "${CMAKE_SOURCE_DIR}/package/scopefun-win\\\\logo.ico")
    set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP   "${CMAKE_SOURCE_DIR}/package/scopefun-win\\\\setup.bmp")
    set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP "${CMAKE_SOURCE_DIR}/package/scopefun-win\\\\setup.bmp")
endif(SCOPEFUN_WINDOWS)

#--------------------------------------------------------------------------------
# post-build
#--------------------------------------------------------------------------------
if(SCOPEFUN_MACOSX)
    add_custom_command(
            TARGET ScopeFun POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE_DIR:ScopeFun>/../../../${SCOPEFUN_EXE_NAME}.app
            ${CMAKE_SOURCE_DIR}/bin/ )
else()
    add_custom_command(
            TARGET ScopeFun POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:ScopeFun>
            ${CMAKE_SOURCE_DIR}/bin/${SCOPEFUN_EXE_NAME}.exe )
endif()

#--------------------------------------------------------------------------------
# pack
#--------------------------------------------------------------------------------

# url
set( CPACK_PACKAGE_HOMEPAGE_URL www.scopefun.com)

#icons
if(SCOPEFUN_MACOSX)
    set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/package/header.bmp")
else()
    set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/package\\\\header.bmp")
endif()

# license
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.TXT" )

# version
set(CPACK_PACKAGE_VERSION_MAJOR ${SCOPEFUN_VERSION_MAJOR} )
set(CPACK_PACKAGE_VERSION_MINOR ${SCOPEFUN_VERSION_MINOR} )
set(CPACK_PACKAGE_VERSION_PATCH ${SCOPEFUN_VERSION_MICRO} )

#vendor
set(CPACK_PACKAGE_VENDOR "ScopeFun" )

#vendor
set(CPACK_PACKAGE_CONTACT "info@scopefun.com" )

#description
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/package/desc.txt" )
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Open Source Oscilloscope" )

#checksum
set(CPACK_PACKAGE_CHECKSUM SHA512)

#source
set(CPACK_SOURCE_IGNORE_FILES  ""
"/\\\\.git/;"
"/\\\\sfExe/;"
"/\\\\sfSrc/;"
"/\\\\bin/ScopeFun.exe/;"
"/\\\\bin/sfBuild.exe/;"
"/\\\\lib/libusb-1.0.25/Build/;"
"/\\\\lib/libusb-1.0.25/src/;"
"/\\\\lib/libusb-1.0.25/tmp/;"
"/\\\\lib/SDL2-2.28.1/Build/;"
"/\\\\lib/SDL2-2.28.1/tmp/;"
"/\\\\lib/wxWidgets-3.2.2.1/Build/;"
"/\\\\lib/wxWidgets-3.2.2.1/sfBuildRelease/;"
"/\\\\lib/wxWidgets-3.2.2.1/sfBuildDebug/;"
"/\\\\lib/wxWidgets-3.2.2.1/tmp/;"
"/\\\\lib/pybind11-2.11.1;"
)

include(CPack)
