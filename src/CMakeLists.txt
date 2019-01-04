add_subdirectory(json)
add_subdirectory(leveldb)
add_subdirectory(qt)
add_subdirectory(test)

add_library(core STATIC
    addrman.cpp
    addrman.h
    alert.cpp
    alert.h
    allocators.h
    base58.h
    bignum.h
    bitcoinrpc.cpp
    bitcoinrpc.h
    bloom.cpp
    bloom.h
    checkpoints.cpp
    checkpoints.h
    checkqueue.h
    clientversion.h
    coincontrol.h
    compat.h
    crypter.cpp
    crypter.h
    db.cpp
    db.h
    hash.cpp
    hash.h
    key.cpp
    key.h
    keystore.cpp
    keystore.h
    leveldb.cpp
    leveldb.h
    limitedmap.h
    main.cpp
    main.h
    mruset.h
    net.cpp
    net.h
    netbase.cpp
    netbase.h
    noui.cpp
    protocol.cpp
    protocol.h
    rpcblockchain.cpp
    rpcdump.cpp
    rpcmining.cpp
    rpcnet.cpp
    rpcrawtransaction.cpp
    rpcwallet.cpp
    script.cpp
    script.h
    scrypt.cpp
    scrypt.h
    serialize.h
    sync.cpp
    sync.h
    threadsafety.h
    txdb.cpp
    txdb.h
    ui_interface.h
    uint256.h
    util.cpp
    util.h
    version.cpp
    version.h
    walletdb.cpp
    walletdb.h
    $<$<BOOL:${USE_SSE2}>:scrypt-sse2.cpp>
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(core
    PRIVATE
        ${MACOSX_FRAMEWORK_APP_KIT}
        ${MACOSX_FRAMEWORK_APPLICATION_SERVICES}
        ${MACOSX_FRAMEWORK_CORE_SERVICES}
        ${MACOSX_FRAMEWORK_FOUNDATION}
        $<$<PLATFORM_ID:Windows>:ws2_32>
        $<$<PLATFORM_ID:Windows>:iphlpapi>
        $<$<PLATFORM_ID:Windows>:mswsock>

    PUBLIC
        berkeleydb
        Boost::chrono
        Boost::filesystem
        Boost::program_options
        Boost::system
        Boost::thread
        json
        leveldb
        miniupnpc
        OpenSSL::SSL
)

target_compile_definitions(core
    PUBLIC
        USE_UPNP
        $<$<PLATFORM_ID:Darwin>:MAC_OSX>
        $<$<PLATFORM_ID:Linux>:LINUX>
        $<$<PLATFORM_ID:Windows>:WIN32>
        $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
        $<$<PLATFORM_ID:Windows>:NOMINMAX>
        $<$<BOOL:${STATIC_BUILD}>:MINIUPNP_STATICLIB>
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
        $<$<BOOL:${USE_SSE2}>:USE_SSE2>

    PRIVATE
        $<$<PLATFORM_ID:Linux>:_FILE_OFFSET_BITS=64> # _FILE_OFFSET_BITS=64 lets 32-bit fopen transparently support large files.
)

if (USE_SSE2)
    set_source_files_properties(scrypt-sse2.cpp PROPERTIES
        COMPILE_OPTIONS "-msse2;-mstackrealign"
    )
endif()

add_library(core-gui STATIC
    init.cpp
    init.h
    wallet.cpp
    wallet.h
)
target_compile_definitions(core-gui PRIVATE QT_GUI)
target_link_libraries(core-gui PUBLIC core)

add_executable(bunnycoind MACOSX_BUNDLE
    init.cpp
    init.h
    wallet.cpp
    wallet.h
)
target_link_libraries(bunnycoind PRIVATE core)

if (WIN32 OR APPLE)
    install(TARGETS bunnycoind DESTINATION .)
elseif (UNIX)
    install(TARGETS bunnycoind DESTINATION bin)
endif()
