add_library(json STATIC
    json_spirit.h
    json_spirit_error_position.h
    json_spirit_reader.h
    json_spirit_reader_template.h
    json_spirit_stream_reader.h
    json_spirit_utils.h
    json_spirit_value.cpp
    json_spirit_value.h
    json_spirit_writer.h
    json_spirit_writer_template.h
)

target_link_libraries(json PUBLIC Boost::system)
