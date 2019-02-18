#ifndef TEST_BITCOIN_H
#define TEST_BITCOIN_H

struct MiningResult
{
    bool success;
    unsigned int nonce;
};

class CBlock;

MiningResult mine(CBlock block, int blockIndex);

#endif // TEST_BITCOIN_H
