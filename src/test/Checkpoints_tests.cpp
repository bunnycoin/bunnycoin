//
// Unit tests for block-chain checkpoints
//
#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

#include "../checkpoints.h"
#include "../util.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(Checkpoints_tests)

BOOST_AUTO_TEST_CASE(sanity)
{
    uint256 p0 = uint256("0x8cb5b97deab4f04ada1b242c8b95a14de9eb07874e39beacb23aa4d867491a97");
    uint256 wrongHash = uint256("0x557bb7c17ed9e6d4a6f9361cfddf7c1fc0bdc394af7019167442b41f507252b4");
    BOOST_CHECK(Checkpoints::CheckBlock(0, p0));

    // Wrong hashes at checkpoints should fail:
    BOOST_CHECK(!Checkpoints::CheckBlock(0, wrongHash));

    // ... but any hash not at a checkpoint should succeed:
    BOOST_CHECK(Checkpoints::CheckBlock(0+1, wrongHash));
    BOOST_CHECK(Checkpoints::CheckBlock(42400, p0));

    BOOST_CHECK_EQUAL(Checkpoints::GetTotalBlocksEstimate(), 0);
}    

BOOST_AUTO_TEST_SUITE_END()
