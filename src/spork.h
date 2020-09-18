// Copyright (c) 2014-2016 The Dash developers
// Copyright (c) 2016-2017 The PIVX developers
// Copyright (c) 2017-2018 The Bulwark developers
// Copyright (c) 2018-2019 The AXEL Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SPORK_H
#define SPORK_H

#include "base58.h"
#include "key.h"
#include "main.h"
#include "net.h"
#include "sync.h"
#include "util.h"

#include "obfuscation.h"
#include "protocol.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

#define NEW_SPORK_ID(id, value) CSporkDef(id, value, #id)
typedef enum _EsporkID{

    SPORK_1_SWIFTTX                         = 10001,
    SPORK_2_SWIFTTX_BLOCK_FILTERING         = 10002,
    SPORK_3_MAX_VALUE                       = 10003,
    SPORK_4_MASTERNODE_PAYMENT_ENFORCEMENT  = 10004,
    SPORK_5_RECONSIDER_BLOCKS               = 10005,
    SPORK_6_MN_WINNER_MINIMUM_AGE           = 10006,
    SPORK_7_MN_REBROADCAST_ENFORCEMENT      = 10007,
    SPORK_8_NEW_PROTOCOL_ENFORCEMENT        = 10008,
    SPORK_9_TX_FILTERING_ENFORCEMENT        = 10009,
    SPORK_10_NEW_PROTOCOL_ENFORCEMENT_2     = 10010,

    SPORK_11_OP_MN_REWARD_2020              = 10011,
    SPORK_12_OP_BLOCK_REWARD_2020           = 10012,
    SPORK_13_MIN_TX_FEE_2020                = 10013,

    SPORK_14_BLOCK_SIZE_3_M                 = 10014,

    SPORK_INVALID                           = -1
}SporkId;

// Default values
struct CSporkDef
{
    CSporkDef(): sporkId(SPORK_INVALID), defaultValue(0) {}
    CSporkDef(SporkId id, int64_t val, std::string n): sporkId(id), defaultValue(val), name(n) {}
    SporkId sporkId;
    int64_t defaultValue;
    std::string name;
};

class CSporkMessage;
class CSporkManager;

extern CSporkManager sporkManager;
extern std::vector<CSporkDef> sporkDefs;
extern std::map<uint256, CSporkMessage> mapSporks;
extern std::map<int, CSporkMessage> mapSporksActive;
extern std::set<CBitcoinAddress> setFilterAddress;
extern bool txFilterState;
extern int txFilterTarget;

void ProcessSpork(CNode* pfrom, std::string& strCommand, CDataStream& vRecv);
int64_t GetSporkValue(int nSporkID);
bool IsSporkActive(int nSporkID);
void ExecuteSpork(int nSporkID, int64_t nValue);
void ReprocessBlocks(int nBlocks);
void InitTxFilter();

//
// Spork Class
// Keeps track of all of the network spork settings
//

class CSporkMessage
{
public:
    std::vector<unsigned char> vchSig;
    int nSporkID;
    int64_t nValue;
    int64_t nTimeSigned;

    uint256 GetHash()
    {
        uint256 n = HashKeccak256(BEGIN(nSporkID), END(nTimeSigned));
        return n;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        READWRITE(nSporkID);
        READWRITE(nValue);
        READWRITE(nTimeSigned);
        READWRITE(vchSig);
    }
};


class CSporkManager
{
private:
    std::vector<unsigned char> vchSig;
    std::string strMasterPrivKey;

public:
    CSporkManager()
    {
    }

    std::string GetSporkNameByID(int id);
    int GetSporkIDByName(std::string strName);
    bool UpdateSpork(int nSporkID, int64_t nValue);
    bool SetPrivKey(std::string strPrivKey);
    bool CheckSignature(CSporkMessage& spork);
    bool Sign(CSporkMessage& spork);
    void Relay(CSporkMessage& msg);
};

#endif
