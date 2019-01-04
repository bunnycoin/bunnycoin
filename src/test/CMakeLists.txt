add_executable(test-core
    accounting_tests.cpp
    alert_tests.cpp
    allocator_tests.cpp
    base32_tests.cpp
    base58_tests.cpp
    base64_tests.cpp
    bignum_tests.cpp
    bloom_tests.cpp
    canonical_tests.cpp
    checkblock_tests.cpp
    Checkpoints_tests.cpp
    compress_tests.cpp
    DoS_tests.cpp
    getarg_tests.cpp
    key_tests.cpp
    miner_tests.cpp
    mruset_tests.cpp
    multisig_tests.cpp
    netbase_tests.cpp
    pmt_tests.cpp
    rpc_tests.cpp
    script_P2SH_tests.cpp
    script_tests.cpp
    scrypt_tests.cpp
    serialize_tests.cpp
    sigopcount_tests.cpp
    test_bitcoin.cpp
    transaction_tests.cpp
    uint160_tests.cpp
    uint256_tests.cpp
    util_tests.cpp
    wallet_tests.cpp
    ../wallet.cpp
)

target_link_libraries(test-core PRIVATE
    core
    Boost::unit_test_framework
)

target_compile_definitions(test-core PRIVATE
    $<$<NOT:$<BOOL:${STATIC_BUILD}>>:BOOST_TEST_DYN_LINK>
)

add_test(NAME test-core COMMAND test-core WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
