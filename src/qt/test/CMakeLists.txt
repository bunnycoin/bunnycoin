add_executable(test-qt
    test_main.cpp
    uritests.cpp
    uritests.h
    ../bitcoinaddressvalidator.cpp
    ../bitcoinunits.cpp
    ../guiutil.cpp
)

target_link_libraries(test-qt PRIVATE
    core-gui
    Qt5::Test
    Qt5::Widgets
)

add_test(NAME test-qt COMMAND test-qt)
