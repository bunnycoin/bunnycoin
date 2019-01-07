add_library(leveldb
    db/builder.cc
    db/c.cc
    db/db_impl.cc
    db/db_iter.cc
    db/dbformat.cc
    db/filename.cc
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
    $<$<PLATFORM_ID:Windows>:port/port_win.cc>
    $<$<PLATFORM_ID:Windows>:util/env_win.cc>
    $<$<OR:$<PLATFORM_ID:Darwin>,$<PLATFORM_ID:Linux>>:port/port_posix.cc>
    $<$<OR:$<PLATFORM_ID:Darwin>,$<PLATFORM_ID:Linux>>:util/env_posix.cc>
)

target_include_directories(leveldb
    SYSTEM PUBLIC include
    SYSTEM INTERFACE helpers
)

set (private_definitions
    $<$<OR:$<PLATFORM_ID:Darwin>,$<PLATFORM_ID:Linux>>:LEVELDB_PLATFORM_POSIX>
    $<$<PLATFORM_ID:Darwin>:OS_MACOSX>
    $<$<PLATFORM_ID:Linux>:OS_LINUX>
    $<$<PLATFORM_ID:Windows>:OS_WINDOWS>
    $<$<PLATFORM_ID:Windows>:_REENTRANT>
    $<$<PLATFORM_ID:Windows>:LEVELDB_PLATFORM_WINDOWS>
    $<$<PLATFORM_ID:Windows>:WINVER=0x0500>
)

target_compile_definitions(leveldb PRIVATE ${private_definitions})

target_compile_options(leveldb PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-fno-builtin-memcmp>
)

target_link_libraries(leveldb INTERFACE
    $<$<PLATFORM_ID:Windows>:shlwapi>
    Threads::Threads
)

if (UNIT_TESTS)
    add_library(leveldb_testutil STATIC
        util/testharness.cc
        util/testharness.h
        util/testutil.cc
        util/testutil.h
    )
    target_link_libraries(leveldb_testutil PRIVATE leveldb)
    target_compile_definitions(leveldb_testutil PRIVATE ${private_definitions})

    function (add_leveldb_test target_name source_file)
        add_executable(${target_name} ${source_file})
        target_link_libraries(${target_name} PRIVATE leveldb leveldb_testutil)
        target_compile_definitions(${target_name} PRIVATE ${private_definitions})
        add_test(NAME ${target_name} COMMAND ${target_name})
    endfunction()

    add_leveldb_test(leveldb_autocompact_test db/autocompact_test.cc)
    add_leveldb_test(leveldb_corruption_test db/corruption_test.cc)
    add_leveldb_test(leveldb_db_test db/db_test.cc)
    add_leveldb_test(leveldb_dbformat_test db/dbformat_test.cc)
    add_leveldb_test(leveldb_filename_test db/filename_test.cc)
    add_leveldb_test(leveldb_log_test db/log_test.cc)
    add_leveldb_test(leveldb_skiplist_test db/skiplist_test.cc)
    add_leveldb_test(leveldb_version_edit_test db/version_edit_test.cc)
    add_leveldb_test(leveldb_version_set_test db/version_set_test.cc)
    add_leveldb_test(leveldb_write_batch_test db/write_batch_test.cc)
    add_leveldb_test(leveldb_memenv_test helpers/memenv/memenv_test.cc)
    add_leveldb_test(leveldb_issue178_test issues/issue178_test.cc)
    add_leveldb_test(leveldb_issue200_test issues/issue200_test.cc)
    add_leveldb_test(leveldb_filter_block_test table/filter_block_test.cc)
    add_leveldb_test(leveldb_table_test table/table_test.cc)
    add_leveldb_test(leveldb_arena_test util/arena_test.cc)
    add_leveldb_test(leveldb_bloom_test util/bloom_test.cc)
    add_leveldb_test(leveldb_cache_test util/cache_test.cc)
    add_leveldb_test(leveldb_coding_test util/coding_test.cc)
    add_leveldb_test(leveldb_crc32c_test util/crc32c_test.cc)
    add_leveldb_test(leveldb_env_test util/env_test.cc)
endif (UNIT_TESTS)
