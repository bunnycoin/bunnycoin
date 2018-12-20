cmake_minimum_required(VERSION 3.10)

project(bunnycoin
    VERSION 1.0.1
)

set(STATIC_BUILD OFF CACHE BOOL "Link dependencies staticaly")
set(OVERRIDE_OPENSSL_CRYPTO_LIBRARY "" CACHE FILEPATH "Explicit path to crypto library")
set(OVERRIDE_OPENSSL_SSL_LIBRARY "" CACHE FILEPATH "Explicit path to openssl library")
set(OVERRIDE_OPENSSL_INCLUDE_DIR "" CACHE PATH "Explicit path to openssl include dir")

if(STATIC_BUILD)
    set(STATIC_OR_SHARED "STATIC")
else()
    set(STATIC_OR_SHARED "SHARED")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Boost_USE_STATIC_LIBS ${STATIC_BUILD})
set(Boost_USE_DEBUG_LIBS OFF)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)

if(WIN32)
    set(Boost_THREADAPI "win32")
endif()

find_package(Boost COMPONENTS chrono filesystem program_options system thread REQUIRED)
find_package(Qt5 COMPONENTS Gui LinguistTools Network Widgets REQUIRED)
find_package(ZLIB REQUIRED)

if(OVERRIDE_OPENSSL_CRYPTO_LIBRARY AND OVERRIDE_OPENSSL_SSL_LIBRARY AND OVERRIDE_OPENSSL_INCLUDE_DIR)
    add_library(OpenSSL::Crypto ${STATIC_OR_SHARED} IMPORTED)
    set_target_properties(OpenSSL::Crypto PROPERTIES
        IMPORTED_LOCATION ${OVERRIDE_OPENSSL_CRYPTO_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES "${OVERRIDE_OPENSSL_INCLUDE_DIR}"
    )

    add_library(OpenSSL::SSL ${STATIC_OR_SHARED} IMPORTED)
    set_target_properties(OpenSSL::SSL PROPERTIES
        IMPORTED_LOCATION ${OVERRIDE_OPENSSL_SSL_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES "${OVERRIDE_OPENSSL_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES OpenSSL::Crypto
    )
else()
    find_package(OpenSSL REQUIRED)

    if (Boost_USE_STATIC_LIBS)
        set_property(TARGET OpenSSL::Crypto APPEND PROPERTY INTERFACE_LINK_LIBRARIES
            $<$<PLATFORM_ID:Windows>:ws2_32>
        )
    endif()
endif()

find_path(MINIUPNPC_INC NAMES miniupnpc/miniupnpc.h)
find_library(MINIUPNPC_LIB NAMES miniupnpc)
add_library(miniupnpc ${STATIC_OR_SHARED} IMPORTED)
set_target_properties(miniupnpc PROPERTIES
    IMPORTED_LOCATION ${MINIUPNPC_LIB}
    INTERFACE_INCLUDE_DIRECTORIES "${MINIUPNPC_INC}"
)

if(NOT MINIUPNPC_INC)
    message(FATAL_ERROR "cannot find miniupnpc include directory")
elseif(NOT MINIUPNPC_LIB)
    message(FATAL_ERROR "cannot find miniupnpc library")
else()
    message(STATUS "found miniupnpc library: ${MINIUPNPC_INC} ${MINIUPNPC_LIB}")
endif()

find_path(BERKELEYDB_INC NAMES db_cxx.h)
find_library(BERKELEYDB_LIB NAMES db_cxx libdb48 db48)
add_library(berkeleydb ${STATIC_OR_SHARED} IMPORTED)
set_target_properties(berkeleydb PROPERTIES
    IMPORTED_LOCATION ${BERKELEYDB_LIB}
    INTERFACE_INCLUDE_DIRECTORIES "${BERKELEYDB_INC}"
)

if(NOT BERKELEYDB_INC)
    message(FATAL_ERROR "cannot find berkeleydb include directory")
elseif(NOT BERKELEYDB_LIB)
    message(FATAL_ERROR "cannot find berkeleydb library")
else()
    message(STATUS "found berkeleydb library: ${BERKELEYDB_INC} ${BERKELEYDB_LIB}")
endif()

find_path(QRENCODE_INC NAMES qrencode.h)
find_library(QRENCODE_LIB NAMES qrencode)
add_library(qrencode ${STATIC_OR_SHARED} IMPORTED)
set_target_properties(qrencode PROPERTIES
    IMPORTED_LOCATION ${QRENCODE_LIB}
    INTERFACE_INCLUDE_DIRECTORIES "${QRENCODE_INC}"
)

if(NOT QRENCODE_INC)
    message(FATAL_ERROR "cannot find qrencode include directory")
elseif(NOT QRENCODE_LIB)
    message(FATAL_ERROR "cannot find qrencode library")
else()
    message(STATUS "found qrencode library: ${QRENCODE_INC} ${QRENCODE_LIB}")
endif()

if(STATIC_BUILD)
    find_library(QTHARFBUZZ_LIB NAMES qtharfbuzz)
    add_library(qtharfbuzz STATIC IMPORTED)
    set_property(TARGET qtharfbuzz PROPERTY IMPORTED_LOCATION ${QTHARFBUZZ_LIB})

    find_library(QTLIBPNG_LIB NAMES qtlibpng)
    add_library(qtlibpng STATIC IMPORTED)
    set_property(TARGET qtlibpng PROPERTY IMPORTED_LOCATION ${QTLIBPNG_LIB})

    find_library(QTPCRE2_LIB NAMES qtpcre2)
    add_library(qtpcre2 STATIC IMPORTED)
    set_property(TARGET qtpcre2 PROPERTY IMPORTED_LOCATION ${QTPCRE2_LIB})
endif()


if(WIN32)
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
endif()

set_property(TARGET Qt5::Core APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    $<$<BOOL:${STATIC_BUILD}>:qtharfbuzz>
    $<$<BOOL:${STATIC_BUILD}>:qtlibpng>
    $<$<BOOL:${STATIC_BUILD}>:qtpcre2>
    ZLIB::ZLIB
    $<$<PLATFORM_ID:Windows>:version>
    $<$<PLATFORM_ID:Windows>:winmm>
)

add_subdirectory(src)

if (WIN32)
    install(FILES release/bunnycoin.conf DESTINATION .)
    include(InstallRequiredSystemLibraries)
elseif (UNIX)
    install(FILES release/bunnycoin.conf DESTINATION share/bunnycoin)

    install(
        FILES contrib/debian/bunnycoin-qt.desktop.desktop
        RENAME bunnycoin-qt.desktop
        DESTINATION share/applications)

    install(
        FILES
            share/pixmaps/bunnycoin64.png
            share/pixmaps/bunnycoin128.png
            share/pixmaps/bunnycoin256.png
        DESTINATION share/pixmaps)
endif()
