
if(WIN32)
    set(PLATFORM_SOURCES
        port/port_win.cc
        util/env_win.cc
    )
elseif(UNIX)
    set(PLATFORM_SOURCES
        port/port_posix.cc
        util/env_posix.cc
    )
endif()

add_library(leveldb
    db/builder.cc
    db/c.cc
    db/db_impl.cc
    db/db_iter.cc
    db/dbformat.cc
    db/filename.cc
    db/leveldb_main.cc
    db/log_reader.cc
    db/log_writer.cc
    db/memtable.cc
    db/repair.cc
    db/table_cache.cc
    db/version_edit.cc
    db/version_set.cc
    db/write_batch.cc
    helpers/memenv/memenv.cc
    table/block.cc
    table/block_builder.cc
    table/filter_block.cc
    table/format.cc
    table/iterator.cc
    table/merger.cc
    table/table.cc
    table/table_builder.cc
    table/two_level_iterator.cc
    util/arena.cc
    util/bloom.cc
    util/cache.cc
    util/coding.cc
    util/comparator.cc
    util/crc32c.cc
    util/env.cc
    util/filter_policy.cc
    util/hash.cc
    util/histogram.cc
    util/logging.cc
    util/options.cc
    util/status.cc
    ${PLATFORM_SOURCES}
)

target_include_directories(leveldb
    SYSTEM PUBLIC include
    SYSTEM INTERFACE helpers
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(COMPILER_OPTIONS PRIVATE -fno-builtin-memcmp)
endif()

if(WIN32)
    set(COMPILER_DEFINITIONS PRIVATE
        _REENTRANT
        OS_WINDOWS
        LEVELDB_PLATFORM_WINDOWS
        WINVER=0x0500
    )

    set(LINK_LIBRARIES INTERFACE shlwapi)
elseif(UNIX)
    set(COMPILER_DEFINITIONS PRIVATE
        OS_LINUX
        LEVELDB_PLATFORM_POSIX
    )
    set(COMPILER_OPTIONS ${COMPILER_OPTIONS} -fPIC)
endif()


target_compile_definitions(leveldb ${COMPILER_DEFINITIONS})

if (COMPILER_OPTIONS)
    target_compile_options(leveldb ${COMPILER_OPTIONS})
endif()

if(LINK_LIBRARIES)
    target_link_libraries(leveldb ${LINK_LIBRARIES})
endif()
