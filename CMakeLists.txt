cmake_minimum_required(VERSION 3.10)

project(bunnycoin
    VERSION 1.0.2
)

set(STATIC_BUILD OFF CACHE BOOL "Link dependencies staticaly")
set(USE_SSE2 OFF CACHE BOOL "Use SSE2")
set(USE_BUILD_INFO OFF CACHE BOOL "Pass git commit information to the binary")
set(UNIT_TESTS ON CACHE BOOL "Build unit tests")
set(OVERRIDE_OPENSSL_CRYPTO_LIBRARY "" CACHE FILEPATH "Explicit path to crypto library")
set(OVERRIDE_OPENSSL_SSL_LIBRARY "" CACHE FILEPATH "Explicit path to openssl library")
set(OVERRIDE_OPENSSL_INCLUDE_DIR "" CACHE PATH "Explicit path to openssl include dir")

if(STATIC_BUILD)
    set(STATIC_OR_SHARED "STATIC")
else()
    set(STATIC_OR_SHARED "SHARED")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
set(CMAKE_CXX_STANDARD 11)

set(Boost_USE_STATIC_LIBS ${STATIC_BUILD})
set(Boost_USE_DEBUG_LIBS OFF)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)

enable_testing()

if (APPLE)
    find_library(MACOSX_FRAMEWORK_APP_KIT NAMES AppKit)
    find_library(MACOSX_FRAMEWORK_APPLICATION_SERVICES NAMES ApplicationServices)
    find_library(MACOSX_FRAMEWORK_CORE_SERVICES NAMES CoreServices)
    find_library(MACOSX_FRAMEWORK_FOUNDATION NAMES Foundation)

    if(NOT MACOSX_FRAMEWORK_APP_KIT)
        message(FATAL_ERROR "cannot find AppKit Framework")
    elseif(NOT MACOSX_FRAMEWORK_APPLICATION_SERVICES)
        message(FATAL_ERROR "cannot find ApplicationServices Framework")
    elseif(NOT MACOSX_FRAMEWORK_CORE_SERVICES)
        message(FATAL_ERROR "cannot find CoreServices Framework")
    elseif(NOT MACOSX_FRAMEWORK_FOUNDATION)
        message(FATAL_ERROR "cannot find Foundation Framework")
    endif()
endif()

if(WIN32)
    set(Boost_THREADAPI "win32")
endif()

find_package(Boost REQUIRED COMPONENTS chrono filesystem program_options system thread unit_test_framework)
find_package(Qt5 REQUIRED COMPONENTS Gui LinguistTools Network Test Widgets)
find_package(Threads REQUIRED)

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
set_property(TARGET miniupnpc APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    $<$<PLATFORM_ID:Windows>:iphlpapi>
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

if(STATIC_BUILD)
    find_package(ZLIB REQUIRED)

    find_library(QTHARFBUZZ_LIB NAMES qtharfbuzz)
    add_library(qtharfbuzz STATIC IMPORTED)
    set_property(TARGET qtharfbuzz PROPERTY IMPORTED_LOCATION ${QTHARFBUZZ_LIB})

    find_library(QTLIBPNG_LIB NAMES qtlibpng)
    add_library(qtlibpng STATIC IMPORTED)
    set_property(TARGET qtlibpng PROPERTY IMPORTED_LOCATION ${QTLIBPNG_LIB})

    find_library(QTPCRE2_LIB NAMES qtpcre2)
    add_library(qtpcre2 STATIC IMPORTED)
    set_property(TARGET qtpcre2 PROPERTY IMPORTED_LOCATION ${QTPCRE2_LIB})

    set_property(TARGET Qt5::Core APPEND PROPERTY INTERFACE_LINK_LIBRARIES
        qtharfbuzz
        qtlibpng
        qtpcre2
        ZLIB::ZLIB
        $<$<PLATFORM_ID:Windows>:version>
        $<$<PLATFORM_ID:Windows>:winmm>
        $<$<PLATFORM_ID:Windows>:ws2_32>
    )

    set_property(TARGET Qt5::Widgets APPEND PROPERTY INTERFACE_LINK_LIBRARIES
        $<$<PLATFORM_ID:Windows>:gdi32>
        $<$<PLATFORM_ID:Windows>:user32>
        $<$<PLATFORM_ID:Windows>:uxtheme>
    )
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

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(USE_SANITIZER_ADDR OFF CACHE BOOL "Enable address sanitizer (cannot be combined with thread sanitizer)")
    if (USE_SANITIZER_ADDR)
        add_compile_options(-fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract)
        link_libraries(asan)
    endif()

    set(USE_SANITIZER_THREAD OFF CACHE BOOL "Enable thread sanitizer (cannot be combined with address sanitizer or leak sanitizer)")
    if (USE_SANITIZER_THREAD)
        add_compile_options(-fsanitize=thread)
        link_libraries(tsan)
    endif()

    set(USE_SANITIZER_LEAK OFF CACHE BOOL "Enable leak sanitizer (cannot be combined with thread sanitizer)")
    if (USE_SANITIZER_LEAK)
        add_compile_options(-fsanitize=leak)
        link_libraries(lsan)
    endif()

    set(USE_SANITIZER_UB OFF CACHE BOOL "Enable undefined behavior sanitizer")
    if (USE_SANITIZER_UB)
        add_compile_options(
            -fsanitize=undefined
            -fsanitize=shift
            -fsanitize=shift-exponent
            -fsanitize=shift-base
            -fsanitize=integer-divide-by-zero
            -fsanitize=unreachable
            -fsanitize=vla-bound
            -fsanitize=null
            -fsanitize=return
            -fsanitize=signed-integer-overflow
            -fsanitize=bounds
            -fsanitize=bounds-strict
            -fsanitize=alignment
            -fsanitize=object-size
            -fsanitize=float-divide-by-zero
            -fsanitize=float-cast-overflow
            -fsanitize=nonnull-attribute
            -fsanitize=returns-nonnull-attribute
            -fsanitize=bool
            -fsanitize=enum
            -fsanitize=vptr
            -fsanitize=pointer-overflow
            -fsanitize=builtin
        )
        link_libraries(ubsan)
    endif()
endif()

add_subdirectory(src)

if (WIN32 OR APPLE)
    install(FILES release/bunnycoin.conf DESTINATION .)
    include(InstallRequiredSystemLibraries)
elseif (UNIX AND NOT APPLE)
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
