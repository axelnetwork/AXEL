// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2018-2019 The AXEL Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>
#include <limits>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
        (1845, uint256("7adf8c8c9332873abc5cd61d36c51a47555c62b0119dc5089870e3f502f0f7e9"))
        (6200, uint256("6322c4aa3410134ed6d513056b94d6105123deb1051ae293804689f8887f0099"))
        (7623, uint256("7b1978dd0320e66002fda5817f32374d8459a6b235073132f47dc1351de9403b"))
        (14115, uint256("62748f8e96122824366828159e8246c88e810f4260314568abdb0c9ead3f9c1f"))
        (94845, uint256("503eb095657c7c7c79807813f57e3d5295d2013a129bd6bdb76771f98a6d5a0b"))
        (102123, uint256("9aac6fc1824ef4fe540f2dfe21e4762ad495cb7f055a66ca5129ec508edc9532"))
        (142175, uint256("6c036dc68197de682da1d27734f18429f4419cb670b11321b1e676acddd342fb"))
        (191840, uint256("e3ca48e9741267dce0746e9dad6fc1734759be7c10e1dae1ceab83d9816ff721"))
        (205848, uint256("40baf64bd078b66977ea2a3697817585ee23253d1debdabeadeb49d2d0d1e57d"))
        (225895, uint256("03c4ad8c228d5de7db80aac733383560bb2f7481b3ad4b159d28d6b5f64b41e7"))
        (374388, uint256("fd3b057e938788a9c84bd95c6a474ab06cddc0209f7a331998005c67f5e1fa57"))
        (424111, uint256("06c8deb7c17e2440612141b7e844eaed1eb3decad47382aa16e23f1c0ad99928"))
        (531645, uint256("0226c3a2523774be53a1bceef48e5c13b1bdd7081faff97a5e8793b96fbfe735"))
    ;

static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1595314885,
    1258435,
    2000
    // 1549526525, // * UNIX timestamp of last checkpoint block
    // 0,          // * total number of transactions between genesis and last checkpoint
    //             //   (the tx=... number in the SetBestChain debug.log lines)
    // 500        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet = boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData dataTestnet = {&mapCheckpointsTestnet, 1549526525, 0, 250};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest = boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData dataRegtest = {&mapCheckpointsRegtest, 1549526525, 0, 0};

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x23;
        pchMessageStart[1] = 0x32;
        pchMessageStart[2] = 0x25;
        pchMessageStart[3] = 0x52;

        nDefaultPort = 32323;
        bnProofOfWorkLimit = ~uint256(0) >> 20;
        bnStartWork = ~uint256(0) >> 24;

        nMaxReorganizationDepth = 100;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetSpacing = 2 * 60;  // 2 minute
        nTargetSpacingSlowLaunch = 2 * 60; // before block 100
        nPoSTargetSpacing = 60;  // 1 minute
        nMaturity = 40;
        nMasternodeCountDrift = 3;
        nMaxMoneyOut = 1000000000 * COIN;
        nStartMasternodePaymentsBlock = 500;

        /** Height or Time Based Activations **/
        nLastPOWBlock = 500;
        nModifierUpdateBlock = std::numeric_limits<decltype(nModifierUpdateBlock)>::max();

        // https://www.newsbtc.com/2019/02/07/axel-launches-a-global-decentralized-network-harnessing-the-potential-of-masternode-technology/
        const char* pszTimestamp = "AXEL @ 7 Feb 2019: a rev0lu+iVonary new decentral1z@d & distribut@d NETVV0RK";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 0 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04fed4284f0e493cb41b389b9d262066c05edd5f524b64ea2ee6d7b8aa0658f67ff98df15895e0cae5702ab31712da0453f50e931dc1c1bc5d1eba88d09d20b5b3") << OP_CHECKSIG;
        txNew.blob = "Genesis Tx";
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime = 1549526523; // Thu, 7 Feb 2019 08:02:03 GMT
        genesis.nBits = 504365040;
        genesis.nNonce = 0x175B33;  // 545649;

        hashGenesisBlock = genesis.GetHash();
        assert(genesis.hashMerkleRoot == uint256("67d2e8a156a26373a9b96b406d5cef6b03e39071f05b0786fd376785f096ada7"));
        assert(hashGenesisBlock == uint256("000003d2dd01c2fa11ffbaf07a20ce4f966a76ce2a209412a60ecba138d99b5e"));

        vSeeds.push_back(CDNSSeedData("", "45.76.62.42"));       // amn1001.axel.network
        vSeeds.push_back(CDNSSeedData("", "104.207.151.24"));    // amn1002.axel.network
        vSeeds.push_back(CDNSSeedData("", "78.141.206.123"));    // amn1003.axel.network
        vSeeds.push_back(CDNSSeedData("", "95.179.255.98"));     // amn1004.axel.network
        vSeeds.push_back(CDNSSeedData("", "139.180.168.62"));    // amn1005.axel.network
        vSeeds.push_back(CDNSSeedData("", "167.179.76.55"));     // amn1006.axel.network
        vSeeds.push_back(CDNSSeedData("", "66.42.55.200"));      // amn1007.axel.network
        vSeeds.push_back(CDNSSeedData("", "155.138.132.181"));   // amn1008.axel.network
        vSeeds.push_back(CDNSSeedData("", "45.63.38.177"));      // amn1009.axel.network
        vSeeds.push_back(CDNSSeedData("", "95.179.180.214"));    // amn1010.axel.network

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 23); // A
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 75); // X
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 83);     // a
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x02)(0x2D)(0x25)(0x33).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x02)(0x21)(0x31)(0x2B).convert_to_container<std::vector<unsigned char> >();
        // BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md 9984
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x04)(0x61).convert_to_container<std::vector<unsigned char> >(); // 1121

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));
        // vFixedSeeds.clear();
        // vSeeds.clear();

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = true;
        fTestnetToBeDeprecatedFieldRPC = false;
        fHeadersFirstSyncingActive = false;

        nPoolMaxTransactions = 3;

        vAlertPubKey = ParseHex("04732afb18c3c41a739fb00b381291d8da4e4d74606c88eb85496899ed7638911792f0a7fd0bdfa51e7587f2c006a4a16aaf475d11e0b8106dfd3f23f9e9bedf62");
        vGMPubKey = ParseHex("04fed4284f0e493cb41b389b9d262066c05edd5f524b64ea2ee6d7b8aa0658f67ff98df15895e0cae5702ab31712da0453f50e931dc1c1bc5d1eba88d09d20b5b3");
        strSporkKey = "048563419991ec5e3566a0b6fd067bd65912a491363986c4ea447662c6fe449698aca198fe0c3fbbec6da98971e77e96ac9463a85dbe4f2a30275cdeba0ac99855";
        strObfuscationPoolDummyAddress = "ANYmoiCwjMaEtrBu5RefFqpDrK64hMKsSp";
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0xd9;
        pchMessageStart[1] = 0x79;
        pchMessageStart[2] = 0x68;
        pchMessageStart[3] = 0xbd;

        bnProofOfWorkLimit = ~uint256(0) >> 1;
        bnStartWork = bnProofOfWorkLimit;

        nDefaultPort = 42323;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetSpacing = 1 * 60;  // 1 minute
        nLastPOWBlock = 500;
        nMaturity = 15;
        nMasternodeCountDrift = 4;
        nModifierUpdateBlock = std::numeric_limits<decltype(nModifierUpdateBlock)>::max();
        nMaxMoneyOut = 1000000000 * COIN;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1549526523; // Thu, 7 Feb 2019 08:02:03 GMT
        genesis.nNonce = 0;

        hashGenesisBlock = genesis.GetHash();

        // assert(hashGenesisBlock == uint256("019a701040d795514ea77eda681e74f8de73afdb1b39d541fc0c697585b878dc"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 83);   // Testnet AXEL addresses start with 'a'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 137);  // Testnet AXEL script addresses start with 'x'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 23);       // Testnet private keys start with 'A'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x3a)(0x80)(0x61)(0xa0).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x3a)(0x80)(0x58)(0x37).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

        //convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;

        vAlertPubKey = ParseHex("04fed4284f0e493cb41b389b9d262066c05edd5f524b64ea2ee6d7b8aa0658f67ff98df15895e0cae5702ab31712da0453f50e931dc1c1bc5d1eba88d09d20b5b3");
        vGMPubKey = ParseHex("0414b78fd29848ca55bacabe49c6bf53c8cb5224cdd84590f21616457c564b01d2c26c69fea8a55b5e336cb40981ba3167b04ddd149a21f59ab07cf30a4b7285b1");
        strSporkKey = "04d549d4d839d8c404e18f3b4c5722c471bde4df76c77a48d52ddfa07fe6d07a753d5ddba68fc6addcfda2779b3ded5d18be69fefba8c58610f1d7eb2a2ad6a3a2";
        strObfuscationPoolDummyAddress = "ANYmoiCwjMaEtrBu5RefFqpDrK64hMKsSp";

    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        pchMessageStart[0] = 0xf8;
        pchMessageStart[1] = 0xcf;
        pchMessageStart[2] = 0x7e;
        pchMessageStart[3] = 0xaf;

        bnStartWork = ~uint256(0) >> 20;

        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetSpacing = 1 * 60;
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        genesis.nTime = 1549526523; // Thu, 7 Feb 2019 08:02:03 GMT
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 1;

        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 52322;

        //assert(hashGenesisBlock == uint256("300552a9db8b2921c3c07e5bbf8694df5099db579742e243daeaf5008b1e74de"));

        vFixedSeeds.clear(); //! Testnet mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Testnet mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 51478;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fMineBlocksOnDemand = true;


    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams* pCurrentParams = 0;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
