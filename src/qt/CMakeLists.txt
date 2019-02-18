if(UNIT_TESTS)
    add_subdirectory(test)
endif()

set(USE_QRCODE ON CACHE BOOL "Enable QR code support")
if (USE_QRCODE)
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
endif()

file(GLOB TS_FILES "locale/*.ts")
set_source_files_properties(${TS_FILES} PROPERTIES
    OUTPUT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/locale")
qt5_add_translation(QM_FILES ${TS_FILES})

set(ui_files
    forms/aboutdialog.ui
    forms/addressbookpage.ui
    forms/askpassphrasedialog.ui
    forms/coincontroldialog.ui
    forms/editaddressdialog.ui
    forms/optionsdialog.ui
    forms/overviewpage.ui
    forms/rpcconsole.ui
    forms/sendcoinsdialog.ui
    forms/sendcoinsentry.ui
    forms/signverifymessagedialog.ui
    forms/transactiondescdialog.ui
    forms/votecoinsdialog.ui
    forms/votecoinsentry.ui
)

if (USE_QRCODE)
    list(APPEND ui_files forms/qrcodedialog.ui)
endif()

qt5_wrap_ui(UI_SOURCES ${ui_files})

add_executable(bunnycoin-qt WIN32 MACOSX_BUNDLE
    aboutdialog.cpp
    aboutdialog.h
    addressbookpage.cpp
    addressbookpage.h
    addresstablemodel.cpp
    addresstablemodel.h
    askpassphrasedialog.cpp
    askpassphrasedialog.h
    bitcoin.cpp
    bitcoin.qrc
    bitcoinaddressvalidator.cpp
    bitcoinaddressvalidator.h
    bitcoinamountfield.cpp
    bitcoinamountfield.h
    bitcoingui.cpp
    bitcoingui.h
    bitcoinstrings.cpp
    bitcoinunits.cpp
    bitcoinunits.h
    clientmodel.cpp
    clientmodel.h
    coincontroldialog.cpp
    coincontroldialog.h
    coincontroltreewidget.cpp
    coincontroltreewidget.h
    csvmodelwriter.cpp
    csvmodelwriter.h
    editaddressdialog.cpp
    editaddressdialog.h
    guiconstants.h
    guiutil.cpp
    guiutil.h
    monitoreddatamapper.cpp
    monitoreddatamapper.h
    notificator.cpp
    notificator.h
    optionsdialog.cpp
    optionsdialog.h
    optionsmodel.cpp
    optionsmodel.h
    overviewpage.cpp
    overviewpage.h
    paymentserver.cpp
    paymentserver.h
    $<$<BOOL:${USE_QRCODE}>:
        qrcodedialog.cpp
        qrcodedialog.h
    >
    qvalidatedlineedit.cpp
    qvalidatedlineedit.h
    qvaluecombobox.cpp
    qvaluecombobox.h
    res/bitcoin-qt.rc
    rpcconsole.cpp
    rpcconsole.h
    sendcoinsdialog.cpp
    sendcoinsdialog.h
    sendcoinsentry.cpp
    sendcoinsentry.h
    signverifymessagedialog.cpp
    signverifymessagedialog.h
    splashscreen.cpp
    splashscreen.h
    transactiondesc.cpp
    transactiondesc.h
    transactiondescdialog.cpp
    transactiondescdialog.h
    transactionfilterproxy.cpp
    transactionfilterproxy.h
    transactionrecord.cpp
    transactionrecord.h
    transactiontablemodel.cpp
    transactiontablemodel.h
    transactionview.cpp
    transactionview.h
    votecoinsdialog.cpp
    votecoinsdialog.h
    votecoinsentry.cpp
    votecoinsentry.h
    walletframe.cpp
    walletframe.h
    walletmodel.cpp
    walletmodel.h
    walletstack.cpp
    walletstack.h
    walletview.cpp
    walletview.h
    ${UI_SOURCES}
    $<$<PLATFORM_ID:Darwin>:
        macdockiconhandler.h
        macdockiconhandler.mm
        macnotificationhandler.h
        macnotificationhandler.mm
        ../mac/artwork/Bunnycoin.icns
    >
)

set_target_properties(bunnycoin-qt PROPERTIES
    Qt5_NO_LINK_QTMAIN TRUE
    AUTOMOC TRUE
    AUTORCC TRUE
)

target_link_libraries(bunnycoin-qt PRIVATE
    core-gui
    $<$<BOOL:${USE_QRCODE}>:qrencode>
    Qt5::Network
    Qt5::Widgets
    $<$<PLATFORM_ID:Windows>:Qt5::QWindowsIntegrationPlugin>
)

target_compile_definitions(bunnycoin-qt PRIVATE
    QT_DEPRECATED_WARNINGS
    $<$<PLATFORM_ID:Darwin>:Q_OS_MAC>
    $<$<PLATFORM_ID:Windows>:BUNNYCOIN_STATIC_WINDOWS_PLUGIN>
)

if (USE_QRCODE)
    set_source_files_properties(addressbookpage.cpp PROPERTIES COMPILE_DEFINITIONS USE_QRCODE)
endif()

if (APPLE)
    set_target_properties(bunnycoin-qt PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME Bunnycoin-Qt
        MACOSX_BUNDLE_COPYRIGHT "© Bunnycoin"
        MACOSX_BUNDLE_GUI_IDENTIFIER com.bunnycoin.wallet-qt
        MACOSX_BUNDLE_ICON_FILE Bunnycoin.icns
    )
    set_source_files_properties(../mac/artwork/Bunnycoin.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
endif()

if (WIN32 OR APPLE)
    install(TARGETS bunnycoin-qt DESTINATION .)
elseif (UNIX)
    install(TARGETS bunnycoin-qt DESTINATION bin)
endif()
