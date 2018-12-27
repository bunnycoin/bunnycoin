add_subdirectory(leveldb)

file(GLOB TS_FILES "qt/locale/*.ts")
set_source_files_properties(${TS_FILES} PROPERTIES
    OUTPUT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/qt/locale")
qt5_add_translation(QM_FILES ${TS_FILES})

qt5_wrap_ui(UI_SOURCES
    qt/forms/aboutdialog.ui
    qt/forms/addressbookpage.ui
    qt/forms/askpassphrasedialog.ui
    qt/forms/coincontroldialog.ui
    qt/forms/editaddressdialog.ui
    qt/forms/optionsdialog.ui
    qt/forms/overviewpage.ui
    qt/forms/qrcodedialog.ui
    qt/forms/rpcconsole.ui
    qt/forms/sendcoinsdialog.ui
    qt/forms/sendcoinsentry.ui
    qt/forms/signverifymessagedialog.ui
    qt/forms/transactiondescdialog.ui
    qt/forms/votecoinsdialog.ui
    qt/forms/votecoinsentry.ui
)

add_executable(bunnycoin-qt WIN32 MACOSX_BUNDLE
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
    init.cpp
    init.h
    json/json_spirit.h
    json/json_spirit_error_position.h
    json/json_spirit_reader.h
    json/json_spirit_reader_template.h
    json/json_spirit_stream_reader.h
    json/json_spirit_utils.h
    json/json_spirit_value.cpp
    json/json_spirit_value.h
    json/json_spirit_writer.h
    json/json_spirit_writer_template.h
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
    qt/aboutdialog.cpp
    qt/aboutdialog.h
    qt/addressbookpage.cpp
    qt/addressbookpage.h
    qt/addresstablemodel.cpp
    qt/addresstablemodel.h
    qt/askpassphrasedialog.cpp
    qt/askpassphrasedialog.h
    qt/bitcoin.cpp
    qt/bitcoin.qrc
    qt/bitcoinaddressvalidator.cpp
    qt/bitcoinaddressvalidator.h
    qt/bitcoinamountfield.cpp
    qt/bitcoinamountfield.h
    qt/bitcoingui.cpp
    qt/bitcoingui.h
    qt/bitcoinstrings.cpp
    qt/bitcoinunits.cpp
    qt/bitcoinunits.h
    qt/clientmodel.cpp
    qt/clientmodel.h
    qt/coincontroldialog.cpp
    qt/coincontroldialog.h
    qt/coincontroltreewidget.cpp
    qt/coincontroltreewidget.h
    qt/csvmodelwriter.cpp
    qt/csvmodelwriter.h
    qt/editaddressdialog.cpp
    qt/editaddressdialog.h
    qt/guiconstants.h
    qt/guiutil.cpp
    qt/guiutil.h
    $<$<PLATFORM_ID:Darwin>:
        qt/macdockiconhandler.h
        qt/macdockiconhandler.mm
        qt/macnotificationhandler.h
        qt/macnotificationhandler.mm
    >
    qt/monitoreddatamapper.cpp
    qt/monitoreddatamapper.h
    qt/notificator.cpp
    qt/notificator.h
    qt/optionsdialog.cpp
    qt/optionsdialog.h
    qt/optionsmodel.cpp
    qt/optionsmodel.h
    qt/overviewpage.cpp
    qt/overviewpage.h
    qt/paymentserver.cpp
    qt/paymentserver.h
    qt/qrcodedialog.cpp
    qt/qrcodedialog.h
    qt/qvalidatedlineedit.cpp
    qt/qvalidatedlineedit.h
    qt/qvaluecombobox.cpp
    qt/qvaluecombobox.h
    qt/res/bitcoin-qt.rc
    qt/rpcconsole.cpp
    qt/rpcconsole.h
    qt/sendcoinsdialog.cpp
    qt/sendcoinsdialog.h
    qt/sendcoinsentry.cpp
    qt/sendcoinsentry.h
    qt/signverifymessagedialog.cpp
    qt/signverifymessagedialog.h
    qt/splashscreen.cpp
    qt/splashscreen.h
    qt/transactiondesc.cpp
    qt/transactiondesc.h
    qt/transactiondescdialog.cpp
    qt/transactiondescdialog.h
    qt/transactionfilterproxy.cpp
    qt/transactionfilterproxy.h
    qt/transactionrecord.cpp
    qt/transactionrecord.h
    qt/transactiontablemodel.cpp
    qt/transactiontablemodel.h
    qt/transactionview.cpp
    qt/transactionview.h
    qt/votecoinsdialog.cpp
    qt/votecoinsdialog.h
    qt/votecoinsentry.cpp
    qt/votecoinsentry.h
    qt/walletframe.cpp
    qt/walletframe.h
    qt/walletmodel.cpp
    qt/walletmodel.h
    qt/walletstack.cpp
    qt/walletstack.h
    qt/walletview.cpp
    qt/walletview.h
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
    wallet.cpp
    wallet.h
    walletdb.cpp
    walletdb.h
    ${UI_SOURCES}
)

set_property(TARGET bunnycoin-qt PROPERTY Qt5_NO_LINK_QTMAIN TRUE)

target_include_directories(bunnycoin-qt PRIVATE
    qt
)

target_link_libraries(bunnycoin-qt PRIVATE
    berkeleydb
    Boost::chrono
    Boost::filesystem
    Boost::program_options
    Boost::system
    Boost::thread
    leveldb
    miniupnpc
    OpenSSL::SSL
    qrencode
    Qt5::Network
    Qt5::Widgets
    ${MACOSX_FRAMEWORK_APP_KIT}
    ${MACOSX_FRAMEWORK_APPLICATION_SERVICES}
    ${MACOSX_FRAMEWORK_CORE_SERVICES}
    ${MACOSX_FRAMEWORK_FOUNDATION}
    $<$<PLATFORM_ID:Windows>:gdi32>
    $<$<PLATFORM_ID:Windows>:user32>
    $<$<PLATFORM_ID:Windows>:uxtheme>
    $<$<PLATFORM_ID:Windows>:ws2_32>
    $<$<PLATFORM_ID:Windows>:iphlpapi>
    $<$<PLATFORM_ID:Windows>:mswsock>
    $<$<PLATFORM_ID:Windows>:Qt5::QWindowsIntegrationPlugin>
)

target_compile_definitions(bunnycoin-qt PRIVATE
    QT_GUI
    QT_DEPRECATED_WARNINGS
    USE_QRCODE
    USE_UPNP
    $<$<PLATFORM_ID:Darwin>:MAC_OSX>
    $<$<PLATFORM_ID:Darwin>:Q_OS_MAC>
    $<$<PLATFORM_ID:Linux>:LINUX>
    $<$<PLATFORM_ID:Linux>:_FILE_OFFSET_BITS=64> # _FILE_OFFSET_BITS=64 lets 32-bit fopen transparently support large files.
    $<$<PLATFORM_ID:Windows>:WIN32>
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
    $<$<PLATFORM_ID:Windows>:BUNNYCOIN_STATIC_WINDOWS_PLUGIN>
    $<$<BOOL:${STATIC_BUILD}>:MINIUPNP_STATICLIB>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
)

target_compile_options(bunnycoin-qt PRIVATE
    $<$<OR:$<PLATFORM_ID:Darwin>,$<PLATFORM_ID:Linux>>:-fPIC>
)

if (APPLE)
    set_target_properties(bunnycoin-qt PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist"
    )
endif()

if (WIN32 OR APPLE)
    install(TARGETS bunnycoin-qt DESTINATION .)
elseif (UNIX)
    install(TARGETS bunnycoin-qt DESTINATION bin)
endif()
