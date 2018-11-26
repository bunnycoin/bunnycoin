cmake_minimum_required(VERSION 3.10)
project(bunnycoin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_DEBUG_LIBS OFF)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_THREADAPI "win32")

find_package(Boost COMPONENTS chrono filesystem program_options system thread REQUIRED)
find_package(Qt5 COMPONENTS Gui LinguistTools Network Widgets REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

find_path(MINIUPNPC_INC NAMES miniupnpc/miniupnpc.h)
find_library(MINIUPNPC_LIB NAMES miniupnpc)
add_library(miniupnpc STATIC IMPORTED)
set_property(TARGET miniupnpc PROPERTY IMPORTED_LOCATION ${MINIUPNPC_LIB})

find_path(BERKELEYDB_INC NAMES db_cxx.h)
find_library(BERKELEYDB_LIB NAMES db_cxx libdb48 db48)
add_library(berkeleydb STATIC IMPORTED)
set_property(TARGET berkeleydb PROPERTY IMPORTED_LOCATION ${BERKELEYDB_LIB})

find_library(QRENCODE_LIB NAMES qrencode)
add_library(qrencode STATIC IMPORTED)
set_property(TARGET qrencode PROPERTY IMPORTED_LOCATION ${QRENCODE_LIB})

find_library(QTHARFBUZZ_LIB NAMES qtharfbuzz)
add_library(qtharfbuzz STATIC IMPORTED)
set_property(TARGET qtharfbuzz PROPERTY IMPORTED_LOCATION ${QTHARFBUZZ_LIB})

find_library(QTLIBPNG_LIB NAMES qtlibpng)
add_library(qtlibpng STATIC IMPORTED)
set_property(TARGET qtlibpng PROPERTY IMPORTED_LOCATION ${QTLIBPNG_LIB})

find_library(QTPCRE2_LIB NAMES qtpcre2)
add_library(qtpcre2 STATIC IMPORTED)
set_property(TARGET qtpcre2 PROPERTY IMPORTED_LOCATION ${QTPCRE2_LIB})

find_library(QT5EVENTDISPATCHERSUPPORT_LIB NAMES Qt5EventDispatcherSupport)
add_library(Qt5EventDispatcherSupport STATIC IMPORTED)
set_property(TARGET Qt5EventDispatcherSupport PROPERTY IMPORTED_LOCATION ${QT5EVENTDISPATCHERSUPPORT_LIB})

find_library(QT5FONTDATABASESUPPORT_LIB NAMES Qt5FontDatabaseSupport)
add_library(Qt5FontDatabaseSupport STATIC IMPORTED)
set_property(TARGET Qt5FontDatabaseSupport PROPERTY IMPORTED_LOCATION ${QT5FONTDATABASESUPPORT_LIB})

find_library(QT5THEMESUPPORT_LIB NAMES Qt5ThemeSupport)
add_library(Qt5ThemeSupport STATIC IMPORTED)
set_property(TARGET Qt5ThemeSupport PROPERTY IMPORTED_LOCATION ${QT5THEMESUPPORT_LIB})

set_property(TARGET Qt5::QWindowsIntegrationPlugin APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    Qt5EventDispatcherSupport
    Qt5FontDatabaseSupport
    Qt5ThemeSupport
    $<$<PLATFORM_ID:Windows>:dwmapi>
    $<$<PLATFORM_ID:Windows>:imm32>
)

set_property(TARGET OpenSSL::Crypto APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    $<$<PLATFORM_ID:Windows>:ws2_32>
)

set_property(TARGET Qt5::Core APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    qtharfbuzz
    qtlibpng
    qtpcre2
    ZLIB::ZLIB
    $<$<PLATFORM_ID:Windows>:version>
    $<$<PLATFORM_ID:Windows>:winmm>
)

add_subdirectory(src)
