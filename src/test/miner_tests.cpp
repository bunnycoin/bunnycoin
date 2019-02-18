#include <boost/test/unit_test.hpp>

#include "init.h"
#include "main.h"
#include "uint256.h"
#include "util.h"
#include "wallet.h"

extern void SHA256Transform(void* pstate, void* pinput, const void* pinit);

BOOST_AUTO_TEST_SUITE(miner_tests)

static
struct {
    unsigned char extranonce;
    unsigned int nonce;
} blockinfo[] = {
    {4, 0x00077d43}, {2, 0x000500a9}, {1, 0x00289b24}, {1, 0x001ba71c},
    {2, 0x00255048}, {2, 0x00142273}, {1, 0x0001ed5d}, {2, 0x0017cad9},
    {2, 0x000e3e07}, {1, 0x00244e44}, {1, 0x002bac47}, {2, 0x00029f46},
    {2, 0x000baea8}, {1, 0x00051315}, {2, 0x00058911}, {2, 0x0026b46c},
    {1, 0x00001b06}, {2, 0x0002c696}, {1, 0x000248ca}, {1, 0x0005aef1},
    {3, 0x00090cc2}, {2, 0x00125df6}, {2, 0x0000baa6}, {1, 0x00070942},
    {2, 0x00191dc4}, {1, 0x001250a9}, {2, 0x001140c6}, {2, 0x000e0e13},
    {2, 0x00116062}, {2, 0x00018538}, {2, 0x00473625}, {2, 0x00050543},
    {1, 0x001ad13f}, {2, 0x000bee8a}, {2, 0x000d2b37}, {1, 0x001b50a7},
    {2, 0x000189c9}, {1, 0x000573cb}, {2, 0x0006e49d}, {1, 0x00039aef},
    {1, 0x000c3b73}, {3, 0x0000af4e}, {2, 0x002180a0}, {5, 0x0010d47a},
    {1, 0x00006bf4}, {5, 0x002d66d3}, {1, 0x0003f5cc}, {1, 0x0007607b},
    {1, 0x002e9862}, {2, 0x001931a5}, {1, 0x000096ad}, {1, 0x00015648},
    {1, 0x0012bba6}, {1, 0x0006bcb2}, {5, 0x002a2014}, {5, 0x000557dc},
    {1, 0x00036256}, {1, 0x000cc316}, {6, 0x000beb6d}, {2, 0x0000f262},
    {2, 0x0000e892}, {1, 0x00250dcb}, {1, 0x000a1d2c}, {1, 0x000aacee},
    {2, 0x0005fc8e}, {2, 0x000189ba}, {1, 0x000012c5}, {1, 0x00040344},
    {1, 0x0004077a}, {5, 0x00083c15}, {5, 0x00118263}, {1, 0x000bdc4a},
    {1, 0x000786d7}, {2, 0x0005174e}, {2, 0x00001ba0}, {1, 0x000c91eb},
    {2, 0x0002803f}, {1, 0x000a178a}, {2, 0x000439de}, {2, 0x0011ccba},
    {1, 0x000a9b60}, {1, 0x000aab8c}, {1, 0x00525365}, {5, 0x004a0567},
    {1, 0x0028b46f}, {1, 0x000716d0}, {1, 0x00145a38}, {1, 0x0018c000},
    {1, 0x00002cd3}, {1, 0x001d9037}, {1, 0x0006f89b}, {2, 0x00041330},
    {0, 0x00176ce0}, {1, 0x004187ce}, {2, 0x00057c3c}, {2, 0x00101db9},
    {2, 0x001aa9ec}, {1, 0x00107861}, {1, 0x000ebeee}, {1, 0x001e81be},
    {1, 0x0003bdfa}, {1, 0x000d1dc3}, {1, 0x0007c21b}, {5, 0x00114f02},
    {2, 0x000a1fe7}, {1, 0x001a5002}, {1, 0x002addfe}, {1, 0x000800e9},
    {2, 0x00018018}, {2, 0x00050a76}, {1, 0xc00b115e}, {1, 0x80006ba2},
    {1, 0x0001bcb7}, {1, 0x0003a80d}, {1, 0x8003624b}, {1, 0x00074774},
    {1, 0x800711aa}, {1, 0x00031211}, {1, 0x800e007b}, {1, 0x40006e7c},
};

// NOTE: These tests rely on CreateNewBlock doing its own self-validation!
BOOST_AUTO_TEST_CASE(CreateNewBlock_validity)
{
    CReserveKey reservekey(pwalletMain);
    CBlockTemplate *pblocktemplate;
    CTransaction tx;
    CScript script;
    uint256 hash;

    // Simple block creation, nothing special yet:
    BOOST_CHECK(pblocktemplate = CreateNewBlockWithKey(reservekey));

    // We can't make transactions until we have inputs
    // Therefore, load 100 blocks :)
    std::vector<CTransaction*>txFirst;
    for (unsigned int i = 0; i < sizeof(blockinfo)/sizeof(*blockinfo); ++i)
    {
        CBlock *pblock = &pblocktemplate->block; // pointer for convenience
        pblock->nVersion = 1;
        pblock->nTime = pindexBest->GetMedianTimePast()+1;
        pblock->vtx[0].vin[0].scriptSig = CScript();
        pblock->vtx[0].vin[0].scriptSig.push_back(blockinfo[i].extranonce);
        pblock->vtx[0].vin[0].scriptSig.push_back(pindexBest->nHeight);
        pblock->vtx[0].vout[0].nValue = GetBlockValue(pindexBest->nHeight+1, 0, pindexBest->GetBlockHash());
        pblock->vtx[0].vout[0].scriptPubKey = CScript();
        if (isGrantAwardBlock(pindexBest->nHeight+1)) {
            pblock->vtx[0].vout.resize(2);
            CBitcoinAddress address("BUNNchaiaxxaS3DEcb894TomLT3VxxCcmK");
            pblock->vtx[0].vout[1].scriptPubKey.SetDestination( address.Get() );
            pblock->vtx[0].vout[1].nValue = 308823250000000;
        }
        if (txFirst.size() < 2)
            txFirst.push_back(new CTransaction(pblock->vtx[0]));
        pblock->hashMerkleRoot = pblock->BuildMerkleTree();
        pblock->nNonce = blockinfo[i].nonce;
        CValidationState state;
        BOOST_CHECK(ProcessBlock(state, NULL, pblock));
        BOOST_CHECK(state.IsValid());
        pblock->hashPrevBlock = pblock->GetHash();
    }
    delete pblocktemplate;

    // Just to make sure we can still make simple blocks
    BOOST_CHECK(pblocktemplate = CreateNewBlockWithKey(reservekey));

    // block sigops > limit: 1000 CHECKMULTISIG + 1
    tx.vin.resize(1);
    // NOTE: OP_NOP is used to force 20 SigOps for the CHECKMULTISIG
    tx.vin[0].scriptSig = CScript() << OP_0 << OP_0 << OP_0 << OP_NOP << OP_CHECKMULTISIG << OP_1;
    tx.vin[0].prevout.hash = txFirst[0]->GetHash();
    tx.vin[0].prevout.n = 0;
    tx.vout.resize(1);
    tx.vout[0].nValue = 5000000000LL;
    for (unsigned int i = 0; i < 1001; ++i)
    {
        tx.vout[0].nValue -= 1000000;
        hash = tx.GetHash();
        mempool.addUnchecked(hash, tx);
        tx.vin[0].prevout.hash = hash;
    }
    BOOST_CHECK(pblocktemplate = CreateNewBlockWithKey(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // block size > limit
    tx.vin[0].scriptSig = CScript();
    // 18 * (520char + DROP) + OP_1 = 9433 bytes
    std::vector<unsigned char> vchData(520);
    for (unsigned int i = 0; i < 18; ++i)
        tx.vin[0].scriptSig << vchData << OP_DROP;
    tx.vin[0].scriptSig << OP_1;
    tx.vin[0].prevout.hash = txFirst[0]->GetHash();
    tx.vout[0].nValue = 5000000000LL;
    for (unsigned int i = 0; i < 128; ++i)
    {
        tx.vout[0].nValue -= 10000000;
        hash = tx.GetHash();
        mempool.addUnchecked(hash, tx);
        tx.vin[0].prevout.hash = hash;
    }
    BOOST_CHECK(pblocktemplate = CreateNewBlockWithKey(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // orphan in mempool
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    BOOST_CHECK(pblocktemplate = CreateNewBlockWithKey(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // child with higher priority than parent
    tx.vin[0].scriptSig = CScript() << OP_1;
    tx.vin[0].prevout.hash = txFirst[1]->GetHash();
    tx.vout[0].nValue = 4900000000LL;
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    tx.vin[0].prevout.hash = hash;
    tx.vin.resize(2);
    tx.vin[1].scriptSig = CScript() << OP_1;
    tx.vin[1].prevout.hash = txFirst[0]->GetHash();
    tx.vin[1].prevout.n = 0;
    tx.vout[0].nValue = 5900000000LL;
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    BOOST_CHECK(pblocktemplate = CreateNewBlockWithKey(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // coinbase in mempool
    tx.vin.resize(1);
    tx.vin[0].prevout.SetNull();
    tx.vin[0].scriptSig = CScript() << OP_0 << OP_1;
    tx.vout[0].nValue = 0;
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    BOOST_CHECK(pblocktemplate = CreateNewBlockWithKey(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // invalid (pre-p2sh) txn in mempool
    tx.vin[0].prevout.hash = txFirst[0]->GetHash();
    tx.vin[0].prevout.n = 0;
    tx.vin[0].scriptSig = CScript() << OP_1;
    tx.vout[0].nValue = 4900000000LL;
    script = CScript() << OP_0;
    tx.vout[0].scriptPubKey.SetDestination(script.GetID());
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    tx.vin[0].prevout.hash = hash;
    tx.vin[0].scriptSig = CScript() << (std::vector<unsigned char>)script;
    tx.vout[0].nValue -= 1000000;
    hash = tx.GetHash();
    mempool.addUnchecked(hash,tx);
    BOOST_CHECK(pblocktemplate = CreateNewBlockWithKey(reservekey));
    delete pblocktemplate;
    mempool.clear();

    // double spend txn pair in mempool
    tx.vin[0].prevout.hash = txFirst[0]->GetHash();
    tx.vin[0].scriptSig = CScript() << OP_1;
    tx.vout[0].nValue = 4900000000LL;
    tx.vout[0].scriptPubKey = CScript() << OP_1;
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    tx.vout[0].scriptPubKey = CScript() << OP_2;
    hash = tx.GetHash();
    mempool.addUnchecked(hash, tx);
    BOOST_CHECK(pblocktemplate = CreateNewBlockWithKey(reservekey));
    delete pblocktemplate;
    mempool.clear();
}

BOOST_AUTO_TEST_CASE(sha256transform_equality)
{
    unsigned int pSHA256InitState[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};


    // unsigned char pstate[32];
    unsigned char pinput[64];

    int i;

    for (i = 0; i < 32; i++) {
        pinput[i] = i;
        pinput[i+32] = 0;
    }

    uint256 hash;

    SHA256Transform(&hash, pinput, pSHA256InitState);

    BOOST_TEST_MESSAGE(hash.GetHex());

    uint256 hash_reference("0x2df5e1c65ef9f8cde240d23cae2ec036d31a15ec64bc68f64be242b1da6631f3");

    BOOST_CHECK(hash == hash_reference);
}

BOOST_AUTO_TEST_SUITE_END()
