#include "main.h"
#include "bignum.h"

#include <boost/test/unit_test.hpp>

#include <vector>

namespace {
const CBigNum PROOF_OF_WORK_LIMIT(~uint256(0) >> 20);
}

BOOST_AUTO_TEST_SUITE(consensus_tests)

BOOST_AUTO_TEST_CASE(nextworkrequired_null)
{
    BOOST_CHECK_EQUAL(GetNextWorkRequired(nullptr, nullptr), PROOF_OF_WORK_LIMIT.GetCompact());
}

BOOST_AUTO_TEST_CASE(nextworkrequired_first_block)
{
    CBlockHeader header;
    CBlockIndex index;
    index.nHeight = 0;
    index.nTime = 0;
    index.nBits = 1337;

    BOOST_CHECK_EQUAL(GetNextWorkRequired(&index, &header), 1337);
}

BOOST_AUTO_TEST_CASE(nextworkrequired_retarget_v1)
{
    CBlockHeader header;

    std::vector<CBlockIndex> blockIndexes(1000);
    for (std::size_t i = 0; i < blockIndexes.size(); ++i) {
        blockIndexes[i].nHeight = i;
        if (i > 0) {
            blockIndexes[i-1].pnext = &blockIndexes[i];
            blockIndexes[i].pprev = &blockIndexes[i-1];
        }
    }

    const CBigNum bn(1000);

    for (std::size_t i = 0; i < 240; ++i) {
        blockIndexes[i].nTime = i;
        blockIndexes[i].nBits = bn.GetCompact();
    }

    BOOST_CHECK_EQUAL(GetNextWorkRequired(&blockIndexes[239], &header), (bn/16).GetCompact());

    for (std::size_t i = 240; i < 480; ++i) {
        blockIndexes[i].nTime = i*10;
        blockIndexes[i].nBits = bn.GetCompact();
    }

    BOOST_CHECK_EQUAL(GetNextWorkRequired(&blockIndexes[479], &header), (bn*4551/(4*60*60)).GetCompact());

    for (std::size_t i = 480; i < 720; ++i) {
        blockIndexes[i].nTime = i*1000;
        blockIndexes[i].nBits = bn.GetCompact();
    }

    BOOST_CHECK_EQUAL(GetNextWorkRequired(&blockIndexes[719], &header), (bn*4).GetCompact());
}

BOOST_AUTO_TEST_CASE(nextworkrequired_retarget_v2)
{
    CBlockHeader header;

    std::vector<CBlockIndex> blockIndexes(1000);
    for (std::size_t i = 0; i < blockIndexes.size(); ++i) {
        blockIndexes[i].nHeight = 5039+i;
        if (i > 0) {
            blockIndexes[i-1].pnext = &blockIndexes[i];
            blockIndexes[i].pprev = &blockIndexes[i-1];
        }
    }

    const CBigNum bn(1000);

    for (std::size_t i = 1; i <= 240; ++i) {
        blockIndexes[i].nTime = i;
        blockIndexes[i].nBits = bn.GetCompact();
    }

    BOOST_CHECK_EQUAL(GetNextWorkRequired(&blockIndexes[240], &header), (bn/8).GetCompact());

    for (std::size_t i = 241; i <= 480; ++i) {
        blockIndexes[i].nTime = i*10;
        blockIndexes[i].nBits = bn.GetCompact();
    }

    BOOST_CHECK_EQUAL(GetNextWorkRequired(&blockIndexes[480], &header), (bn*4551/(4*60*60)).GetCompact());

    for (std::size_t i = 481; i <= 720; ++i) {
        blockIndexes[i].nTime = i*1000;
        blockIndexes[i].nBits = bn.GetCompact();
    }

    BOOST_CHECK_EQUAL(GetNextWorkRequired(&blockIndexes[720], &header), (bn*4).GetCompact());
}

BOOST_AUTO_TEST_CASE(nextworkrequired_retarget_v3)
{
    CBlockHeader header;

    std::vector<CBlockIndex> blockIndexes(1000);
    for (std::size_t i = 0; i < blockIndexes.size(); ++i) {
        blockIndexes[i].nHeight = 10079+i;
        if (i > 0) {
            blockIndexes[i-1].pnext = &blockIndexes[i];
            blockIndexes[i].pprev = &blockIndexes[i-1];
        }
    }

    const CBigNum bn(1000);

    for (std::size_t i = 1; i <= 240; ++i) {
        blockIndexes[i].nTime = i;
        blockIndexes[i].nBits = bn.GetCompact();
    }

    BOOST_CHECK_EQUAL(GetNextWorkRequired(&blockIndexes[240], &header), (bn/4).GetCompact());

    for (std::size_t i = 241; i <= 480; ++i) {
        blockIndexes[i].nTime = i*10;
        blockIndexes[i].nBits = bn.GetCompact();
    }

    BOOST_CHECK_EQUAL(GetNextWorkRequired(&blockIndexes[480], &header), (bn*4551/(4*60*60)).GetCompact());

    for (std::size_t i = 481; i <= 720; ++i) {
        blockIndexes[i].nTime = i*1000;
        blockIndexes[i].nBits = bn.GetCompact();
    }

    BOOST_CHECK_EQUAL(GetNextWorkRequired(&blockIndexes[720], &header), (bn*4).GetCompact());
}

BOOST_AUTO_TEST_SUITE_END()
