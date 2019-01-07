function(add_gui_test target_name)
    add_executable(${target_name} ${ARGN})
    target_link_libraries(${target_name} PRIVATE core-gui Qt5::Test Qt5::Widgets)
    set_target_properties(${target_name} PROPERTIES AUTOMOC TRUE AUTORCC TRUE)
    add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

add_gui_test(gui_uri_tests
    uritests.cpp
    ../bitcoinaddressvalidator.cpp
    ../bitcoinunits.cpp
    ../guiutil.cpp
)
