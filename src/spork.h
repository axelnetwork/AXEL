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
    SPORK_15_MN_V2                          = 10015,
    SPORK_16_RES_CK                         = 10016,
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
bool sporkCheckSignature(std::string &strMessage, std::vector<unsigned char> &vchSig);

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

/** detail message for a CNotifyMessage */
class CNotifyInternalMsg
{
public:
    int index;
    string value;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        READWRITE(index);
        READWRITE(LIMITED_STRING(value, 10));
    }

    CNotifyInternalMsg(int idx,string val){index = idx; value = val;};
    CNotifyInternalMsg(){};
};

#define NEW_NOTIFY_ID(id, value) CNotifyDef(id, value, #id)
typedef enum _ENotifyID{
    NOTIFY_1_RESCKPARAM                    = 20001,
    NOTIFY_INVALID                           = -1
}NotifyId;

// Default values
struct CNotifyDef
{
    CNotifyDef(): notifyId(NOTIFY_INVALID) {}
    CNotifyDef(NotifyId id, std::vector<CNotifyInternalMsg> val, std::string n): notifyId(id), vdefaultMsg(val), name(n) {}
    NotifyId notifyId;
    std::vector<CNotifyInternalMsg> vdefaultMsg;
    std::string name;
};

class CNotifyMessage;
extern std::vector<CNotifyDef> notifyDefs;
extern std::map<uint256, CNotifyMessage> mapNotifys;
extern std::map<int, CNotifyMessage> mapNotifysActive;

std::vector<CNotifyInternalMsg> GetNotifyValue(int nNotifyID);
bool IsNotifyActive(int nNotifyID);
bool ExecuteNotify(CNotifyMessage& notifyMsg);

//
// Notify Class
// Keeps track of all of the network notify settings
//

class CNotifyMessage
{
public:
    std::vector<unsigned char> vchSig;
    int nNotifyID;
    std::vector<CNotifyInternalMsg> vMsg;
    int64_t nTimeSigned;

    uint256 GetHash()
    {
        uint256 n = HashKeccak256(BEGIN(nNotifyID), END(nTimeSigned));
        return n;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        READWRITE(nNotifyID);
        READWRITE(vMsg);
        READWRITE(nTimeSigned);
        READWRITE(vchSig);
    }
};

/** Access to the Notify database (notify.dat)
 */
void DumpNotify();
class CNotifyDB
{
private:
    boost::filesystem::path pathNotify;
    std::string strMagicMessage;

public:
    enum ReadResult {
        Ok,
        FileError,
        HashReadError,
        IncorrectHash,
        IncorrectMagicMessage,
        IncorrectMagicNumber,
        IncorrectFormat
    };

    CNotifyDB();
    bool Write(const std::map<int, CNotifyMessage>& notifymapToSave);
    ReadResult Read(std::map<int, CNotifyMessage>& notifymapToLoad);
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
    bool CheckSignature(std::string &strMessage, std::vector<unsigned char> &vchSig);
    bool Sign(std::string &strMessage, std::vector<unsigned char> &vchSig);
    bool Sign(CSporkMessage& spork);
    void Relay(CSporkMessage& msg);

    void LoadNotifysFromDB();
    std::string GetNotifyNameByID(int id);
    int GetNotifyIDByName(std::string strName);
    bool UpdateNotify(int nSporkID, std::vector<CNotifyInternalMsg> Value);
    bool CheckSignature(CNotifyMessage& notity);
    bool Sign(CNotifyMessage& notity);
    void Relay(CNotifyMessage& msg);
};

#endif
