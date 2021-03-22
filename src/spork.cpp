// Copyright (c) 2014-2016 The Dash developers
// Copyright (c) 2016-2017 The PIVX developers
// Copyright (c) 2018-2019 The AXEL Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "spork.h"
#include "base58.h"
#include "key.h"
#include "main.h"
#include "net.h"
#include "protocol.h"
#include "sync.h"
#include "util.h"
#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>
#include <univalue.h>
#include "resckparam.h"
extern CResParam resckparam;

using namespace std;
using namespace boost;

class CSporkMessage;
class CSporkManager;

CSporkManager sporkManager;
std::map<uint256, CSporkMessage> mapSporks;
std::map<int, CSporkMessage> mapSporksActive;
std::set<CBitcoinAddress> setFilterAddress;
bool txFilterState = false;
int txFilterTarget = 0;

/*
    Don't ever reuse these IDs for other sporks
    - This would result in old clients getting confused about which spork is for what
*/
std::vector<CSporkDef> sporkDefs =
{
    NEW_SPORK_ID(SPORK_1_SWIFTTX,                           978307200),     //2001-1-1
    NEW_SPORK_ID(SPORK_2_SWIFTTX_BLOCK_FILTERING,           1424217600),    //2015-2-18
    NEW_SPORK_ID(SPORK_3_MAX_VALUE,                         1000),
    NEW_SPORK_ID(SPORK_4_MASTERNODE_PAYMENT_ENFORCEMENT,    1541505600),    // 11/06/2018 @ 12:00pm (UTC)
    NEW_SPORK_ID(SPORK_5_RECONSIDER_BLOCKS,                 0),
    NEW_SPORK_ID(SPORK_6_MN_WINNER_MINIMUM_AGE,             8000),          // Age in seconds. This should be > MASTERNODE_REMOVAL_SECONDS to avoid
                                                                            // misconfigured new nodes in the list.
                                                                            // Set this to zero to emulate classic behaviour

    NEW_SPORK_ID(SPORK_7_MN_REBROADCAST_ENFORCEMENT,        4102444800),    // off
    NEW_SPORK_ID(SPORK_8_NEW_PROTOCOL_ENFORCEMENT,          1545674400),    // 24/12/2018 @ 18:00 (UTC)
    NEW_SPORK_ID(SPORK_9_TX_FILTERING_ENFORCEMENT,          0),             // off
    NEW_SPORK_ID(SPORK_10_NEW_PROTOCOL_ENFORCEMENT_2,       4102444800),    // off

    NEW_SPORK_ID(SPORK_11_OP_MN_REWARD_2020,                949851),        // active it once block height reach this value 05/11/2021
    NEW_SPORK_ID(SPORK_12_OP_BLOCK_REWARD_2020,             928493),        // active it once block height reach this value 04/26/2021
    NEW_SPORK_ID(SPORK_13_MIN_TX_FEE_2020,                  939893),        // active it once block height reach this value 05/04/2021

    NEW_SPORK_ID(SPORK_14_BLOCK_SIZE_3_M,                   1618812000),    // 04/19/2021 @ 06:00 (UTC)
    NEW_SPORK_ID(SPORK_15_MN_V2,                            1618207200),    // 04/12/2021 @ 06:00 (UTC)
    NEW_SPORK_ID(SPORK_16_RES_CK,                           1621231200),    // 05/17/2021 @ 06:00 (UTC)
};


// axel: on startup load notify values from previous session if they exist in the notifyDB
void CSporkManager::LoadNotifysFromDB()
{
    CNotifyDB notifydb;
    std::map<int, CNotifyMessage> mapNotifyToLoadTemp;
    CNotifyDB::ReadResult notifyreadResult = notifydb.Read(mapNotifyToLoadTemp);
    if (notifyreadResult == CNotifyDB::FileError)
        LogPrintf("LoadNotifysFromDB() - Missing notify file - notify.dat, will try to recreate\n");
    else if (notifyreadResult != CNotifyDB::Ok) {
        LogPrintf("LoadNotifysFromDB() - Error reading notify.dat: ");
        if (notifyreadResult == CNotifyDB::IncorrectFormat)
            LogPrintf("LoadNotifysFromDB() - magic is ok but data has invalid format, will try to recreate\n");
        else
            LogPrintf("LoadNotifysFromDB() - file format is unknown or invalid, please fix it manually\n");
    }
    else {
        mapNotifysActive = mapNotifyToLoadTemp;
        if (!mapNotifysActive.empty() && mapNotifysActive.count(NOTIFY_1_RESCKPARAM)) {
            LogPrintf("mapNotifysActive has notify NOTIFY_1_RESCKPARAM\n");
            resckparam.ProcessResCkParam(mapNotifysActive[NOTIFY_1_RESCKPARAM]);
        }
    }
}

void ProcessSpork(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    if (fLiteMode) return; //disable all obfuscation/masternode related functionality

    if (strCommand == "spork") {
        LogPrintf("ProcessSpork::spork\n");
        CDataStream vMsg(vRecv);
        CSporkMessage spork;
        vRecv >> spork;

        if (chainActive.Tip() == NULL) return;

        uint256 hash = spork.GetHash();
        if (mapSporksActive.count(spork.nSporkID)) {
            if (mapSporksActive[spork.nSporkID].nTimeSigned >= spork.nTimeSigned) {
                if (fDebug) LogPrintf("spork - seen %s block %d \n", hash.ToString(), chainActive.Tip()->nHeight);
                return;
            } else {
                if (fDebug) LogPrintf("spork - got updated spork %s block %d \n", hash.ToString(), chainActive.Tip()->nHeight);
            }
        }

        LogPrintf("spork - new %s ID %d Time %d bestHeight %d\n", hash.ToString(), spork.nSporkID, spork.nValue, chainActive.Tip()->nHeight);

        if (!sporkManager.CheckSignature(spork)) {
            LogPrintf("spork - invalid signature\n");
            Misbehaving(pfrom->GetId(), 100);
            return;
        }

        mapSporks[hash] = spork;
        mapSporksActive[spork.nSporkID] = spork;
        sporkManager.Relay(spork);

        //does a task if needed
        ExecuteSpork(spork.nSporkID, spork.nValue);
    }
    if (strCommand == "notify") {
        LogPrintf("ProcessSpork::notify\n");
        CDataStream vMsg(vRecv);
        CNotifyMessage notify;
        vRecv >> notify;

        if (chainActive.Tip() == NULL) return;

        uint256 hash = notify.GetHash();
        if (mapNotifysActive.count(notify.nNotifyID)) {
            if (mapNotifysActive[notify.nNotifyID].nTimeSigned >= notify.nTimeSigned) {
                if (fDebug) LogPrintf("notify - seen %s block %d \n", hash.ToString(), chainActive.Tip()->nHeight);
                return;
            } else {
                if (fDebug) LogPrintf("notify - got updated notify %s block %d \n", hash.ToString(), chainActive.Tip()->nHeight);
            }
        }

        LogPrintf("notify - new %s ID %d.bestHeight %d\n", hash.ToString(), notify.nNotifyID, chainActive.Tip()->nHeight);

        if (!sporkManager.CheckSignature(notify)) {
            LogPrintf("notify - invalid signature\n");
            Misbehaving(pfrom->GetId(), 100);
            return;
        }
        //does a task if needed
        if(!ExecuteNotify(notify)) {
            LogPrintf("notify - ExecuteNotify fail\n");
            return;
        }

        mapNotifys[hash] = notify;
        mapNotifysActive[notify.nNotifyID] = notify;
        sporkManager.Relay(notify);
    }
    if (strCommand == "getsporks") {
        std::map<int, CSporkMessage>::iterator it = mapSporksActive.begin();

        while (it != mapSporksActive.end()) {
            pfrom->PushMessage("spork", it->second);
            it++;
        }

        std::map<int, CNotifyMessage>::iterator it1 = mapNotifysActive.begin();

        while (it1 != mapNotifysActive.end()) {
            pfrom->PushMessage("notify", it1->second);
            it1++;
        }
    }
}

// grab the spork, otherwise say it's off
bool IsSporkActive(int nSporkID)
{
    int64_t r = GetSporkValue(nSporkID);
    if (r == -1) return false;

    return r < GetTime();
}

// grab the value of the spork on the network, or the default
int64_t GetSporkValue(int nSporkID)
{
    int64_t r = -1;

    if (mapSporksActive.count(nSporkID)) {
        r = mapSporksActive[nSporkID].nValue;
    } else {
        for (const CSporkDef& sporkDef : sporkDefs) {
            if (sporkDef.sporkId == nSporkID){
                return sporkDef.defaultValue;
            }
        }

        if (r == -1) LogPrintf("GetSpork::Unknown Spork %d\n", nSporkID);
    }

    return r;
}

void ExecuteSpork(int nSporkID, int64_t nValue)
{
    //correct fork via spork technology
    if (nSporkID == SPORK_5_RECONSIDER_BLOCKS && nValue > 0) {
        LogPrintf("Spork::ExecuteSpork -- Reconsider Last %d Blocks\n", nValue);
        ReprocessBlocks(nValue);
    } else if (nSporkID == SPORK_9_TX_FILTERING_ENFORCEMENT) {
        LogPrintf("Spork::ExecuteSpork -- Initialize TX filter list\n");
        InitTxFilter();
    }
}

void InitTxFilter()
{
    if(Params().NetworkID() == CBaseChainParams::REGTEST)
        return;
    LOCK(cs_main);
    setFilterAddress.clear();
    CBitcoinAddress Address;
    CTxDestination Dest;

    CBlock referenceBlock;
    uint64_t sporkBlockValue = (GetSporkValue(SPORK_9_TX_FILTERING_ENFORCEMENT) >> 32) & 0xffffffff; // 32-bit block number

    txFilterTarget = sporkBlockValue; // set filter targed on spork recived
    if (txFilterTarget == 0) {
        // no target block, return
        txFilterState = true;
        return;
    }

    CBlockIndex *referenceIndex = chainActive[sporkBlockValue];
    if (referenceIndex != NULL) {
        assert(ReadBlockFromDisk(referenceBlock, referenceIndex));
        int sporkMask = GetSporkValue(SPORK_9_TX_FILTERING_ENFORCEMENT) & 0xffffffff; // 32-bit tx mask
        int nAddressCount = 0;

        // Find the addresses that we want filtered
        for (unsigned int i = 0; i < referenceBlock.vtx.size(); i++) {
            // The mask can support up to 32 transaction indexes (as it is 32-bit)
            if (((sporkMask >> i) & 0x1) != 0) {
                for (unsigned int j = 0; j < referenceBlock.vtx[i].vout.size(); j++) {
                    if (referenceBlock.vtx[i].vout[j].nValue > 0) {
                        ExtractDestination(referenceBlock.vtx[i].vout[j].scriptPubKey, Dest);
                        Address.Set(Dest);
                        auto it  = setFilterAddress.insert(Address);

                        if (/*fDebug &&*/ it.second)
                            LogPrintf("InitTxFilter(): Add Tx filter address %d in reference block %ld, %s\n",
                                          ++nAddressCount, sporkBlockValue, Address.ToString());
                    }
                }
            }
        }
        // filter initialization completed
        txFilterState = true;
    }
}

void ReprocessBlocks(int nBlocks)
{
    std::map<uint256, int64_t>::iterator it = mapRejectedBlocks.begin();
    while (it != mapRejectedBlocks.end()) {
        //use a window twice as large as is usual for the nBlocks we want to reset
        if ((*it).second > GetTime() - (nBlocks * 60 * 5)) {
            BlockMap::iterator mi = mapBlockIndex.find((*it).first);
            if (mi != mapBlockIndex.end() && (*mi).second) {
                LOCK(cs_main);

                CBlockIndex* pindex = (*mi).second;
                LogPrintf("ReprocessBlocks - %s\n", (*it).first.ToString());

                CValidationState state;
                ReconsiderBlock(state, pindex);
            }
        }
        ++it;
    }

    {
        LOCK(cs_main);
        DisconnectBlocksAndReprocess(nBlocks);
    }

    CValidationState state;

    ActivateBestChain(state);
}

bool sporkCheckSignature(std::string &strMessage, std::vector<unsigned char> &vchSig)
{
    return sporkManager.CheckSignature(strMessage, vchSig);
}

bool CSporkManager::CheckSignature(CSporkMessage& spork)
{
    //note: need to investigate why this is failing
    std::string strMessage = boost::lexical_cast<std::string>(spork.nSporkID) + boost::lexical_cast<std::string>(spork.nValue) + boost::lexical_cast<std::string>(spork.nTimeSigned);
    CPubKey pubkey(ParseHex(Params().SporkKey()));

    std::string errorMessage = "";
    if (!obfuScationSigner.VerifyMessage(pubkey, spork.vchSig, strMessage, errorMessage)) {
        return false;
    }

    return true;
}

bool CSporkManager::CheckSignature(std::string &strMessage, std::vector<unsigned char> &vchSig)
{
    CPubKey pubkey(ParseHex(Params().SporkKey()));

    std::string errorMessage = "";
    if (!obfuScationSigner.VerifyMessage(pubkey, vchSig, strMessage, errorMessage)) {
        return false;
    }

    return true;
}

bool CSporkManager::Sign(std::string &strMessage, std::vector<unsigned char> &vchSig)
{
    CKey key2;
    CPubKey pubkey2;
    std::string errorMessage = "";

    if (!obfuScationSigner.SetKey(strMasterPrivKey, errorMessage, key2, pubkey2)) {
        LogPrintf("CMasternodePayments::Sign - ERROR: Invalid masternodeprivkey: '%s'\n", errorMessage);
        return false;
    }

    if (!obfuScationSigner.SignMessage(strMessage, errorMessage, vchSig, key2)) {
        LogPrintf("CMasternodePayments::Sign - Sign message failed");
        return false;
    }

    if (!obfuScationSigner.VerifyMessage(pubkey2, vchSig, strMessage, errorMessage)) {
        LogPrintf("CMasternodePayments::Sign - Verify message failed");
        return false;
    }

    return true;
}


bool CSporkManager::Sign(CSporkMessage& spork)
{
    std::string strMessage = boost::lexical_cast<std::string>(spork.nSporkID) + boost::lexical_cast<std::string>(spork.nValue) + boost::lexical_cast<std::string>(spork.nTimeSigned);

    CKey key2;
    CPubKey pubkey2;
    std::string errorMessage = "";

    if (!obfuScationSigner.SetKey(strMasterPrivKey, errorMessage, key2, pubkey2)) {
        LogPrintf("CMasternodePayments::Sign - ERROR: Invalid masternodeprivkey: '%s'\n", errorMessage);
        return false;
    }

    if (!obfuScationSigner.SignMessage(strMessage, errorMessage, spork.vchSig, key2)) {
        LogPrintf("CMasternodePayments::Sign - Sign message failed");
        return false;
    }

    if (!obfuScationSigner.VerifyMessage(pubkey2, spork.vchSig, strMessage, errorMessage)) {
        LogPrintf("CMasternodePayments::Sign - Verify message failed");
        return false;
    }

    return true;
}

bool CSporkManager::UpdateSpork(int nSporkID, int64_t nValue)
{
    CSporkMessage msg;
    msg.nSporkID = nSporkID;
    msg.nValue = nValue;
    msg.nTimeSigned = GetTime();

    if (Sign(msg)) {
        Relay(msg);
        mapSporks[msg.GetHash()] = msg;
        mapSporksActive[nSporkID] = msg;
        return true;
    }

    return false;
}

void CSporkManager::Relay(CSporkMessage& msg)
{
    CInv inv(MSG_SPORK, msg.GetHash());
    RelayInv(inv);
}

bool CSporkManager::SetPrivKey(std::string strPrivKey)
{
    CSporkMessage msg;

    // Test signing successful, proceed
    strMasterPrivKey = strPrivKey;

    Sign(msg);

    if (CheckSignature(msg)) {
        LogPrintf("CSporkManager::SetPrivKey - Successfully initialized as spork signer\n");
        return true;
    } else {
        return false;
    }

    CNotifyMessage msgnotify;

    Sign(msgnotify);

    if (CheckSignature(msgnotify)) {
        LogPrintf("CSporkManager::SetPrivKey - Successfully initialized as notify signer\n");
        return true;
    } else {
        return false;
    }
}

int CSporkManager::GetSporkIDByName(std::string strName)
{
    for (const CSporkDef& sporkDef : sporkDefs) {
        if (sporkDef.name == strName){
            return sporkDef.sporkId;
        }
    }

    return -1;
}

std::string CSporkManager::GetSporkNameByID(int id)
{
    for (const CSporkDef& sporkDef : sporkDefs) {
        if (sporkDef.sporkId == id){
            return sporkDef.name;
        }
    }

    return "Unknown";
}

std::map<uint256, CNotifyMessage> mapNotifys;
std::map<int, CNotifyMessage> mapNotifysActive;
std::vector<CNotifyInternalMsg> vresckparamDefaultMsg;

std::vector<CNotifyDef> notifyDefs =
{
    NEW_NOTIFY_ID(NOTIFY_1_RESCKPARAM,vresckparamDefaultMsg)
};

// grab the notify, otherwise say it's off
bool IsNotifyActive(int nNotifyID)
{
    std::vector<CNotifyInternalMsg> r = GetNotifyValue(nNotifyID);
    if (r.empty()) return false;

    return true;
}

// grab the value of the notify on the network, or the default
std::vector<CNotifyInternalMsg> GetNotifyValue(int nNotifyID)
{
    std::vector<CNotifyInternalMsg> r;

    if (mapNotifysActive.count(nNotifyID)) {
        LogPrintf("GetNotifyValue 02 %d\n", nNotifyID);
        r = mapNotifysActive[nNotifyID].vMsg;
        if (r.empty()) LogPrintf("GetNotify::Unknown Notify 1 %d\n", nNotifyID);
    }
    if (r.empty())
    {
        LogPrintf("GetNotifyValue 12 %d\n", nNotifyID);
        for (const CNotifyDef& notifyDef : notifyDefs) {
            if (notifyDef.notifyId == nNotifyID){
                return resckparam.getAllParams();
            }
        }

        if (r.empty()) LogPrintf("GetNotify::Unknown Notify 2 %d\n", nNotifyID);
    }

    return r;
}

bool ExecuteNotify(CNotifyMessage& notifyMsg)
{
    if (notifyMsg.nNotifyID == NOTIFY_1_RESCKPARAM && !notifyMsg.vMsg.empty()) {
        LogPrintf("Notify::ExecuteNotify -- ProcessResCkParam\n");
        if(!resckparam.ProcessResCkParam(notifyMsg))
            return false;
    }
    return true;
}

bool CSporkManager::CheckSignature(CNotifyMessage& notify)
{
    LogPrintf("CSporkManager::CheckSignature Notify 1\n");

    std::string strMessage = boost::lexical_cast<std::string>(notify.nNotifyID)+ resckparam.ParseVNotifyInternalMsgToString(notify.vMsg) + boost::lexical_cast<std::string>(notify.nTimeSigned);
    CPubKey pubkey(ParseHex(Params().SporkKey()));

    std::string errorMessage = "";
    if (!obfuScationSigner.VerifyMessage(pubkey, notify.vchSig, strMessage, errorMessage)) {
        return false;
    }

    return true;
}

bool CSporkManager::Sign(CNotifyMessage& notify)
{
    LogPrintf("CSporkManager::Sign Notify 1\n");
    std::string strMessage = boost::lexical_cast<std::string>(notify.nNotifyID) + resckparam.ParseVNotifyInternalMsgToString(notify.vMsg) + boost::lexical_cast<std::string>(notify.nTimeSigned);

    CKey key2;
    CPubKey pubkey2;
    std::string errorMessage = "";

    if (!obfuScationSigner.SetKey(strMasterPrivKey, errorMessage, key2, pubkey2)) {
        LogPrintf("CSporkManager::Sign Notify- ERROR: Invalid masternodeprivkey: '%s'\n", errorMessage);
        return false;
    }

    if (!obfuScationSigner.SignMessage(strMessage, errorMessage, notify.vchSig, key2)) {
        LogPrintf("CSporkManager::Sign Notify - Sign message failed\n");
        return false;
    }

    if (!obfuScationSigner.VerifyMessage(pubkey2, notify.vchSig, strMessage, errorMessage)) {
        LogPrintf("CSporkManager::Sign Notify - Verify message failed\n");
        return false;
    }

    return true;
}

bool CSporkManager::UpdateNotify(int nNotifyID, std::vector<CNotifyInternalMsg> Value)
{
    CNotifyMessage msg;
    msg.nNotifyID = nNotifyID;
    //msg.vMsg = Value;
    std::vector<CNotifyInternalMsg> oldvMsg = GetNotifyValue(nNotifyID);
    for(int i=0; i < Value.size(); i++) {
        CNotifyInternalMsg tmpmsg = Value[i];
        LogPrintf("in UpdateNotify index=%d Value=%s\n",tmpmsg.index,tmpmsg.value);
        bool found = false;
        for (std::vector<CNotifyInternalMsg>::iterator it = oldvMsg.begin(); it != oldvMsg.end();) {
            if ((*it).index == tmpmsg.index) {
                LogPrintf("in UpdateNotify found old msg index=%d Value=%s\n",(*it).index,(*it).value);
                if ((*it).value != tmpmsg.value) {
                    it = oldvMsg.erase(it);
                    oldvMsg.push_back(tmpmsg);
                }
                found = true;
                break;
            } else {
                ++it;
            }
        }
        if(!found)
            oldvMsg.push_back(tmpmsg);
    }

    msg.vMsg = oldvMsg;
    msg.nTimeSigned = GetTime();

    LogPrintf("CSporkManager::UpdateNotify Notify 1\n");
    if (Sign(msg)) {
        Relay(msg);
        mapNotifys[msg.GetHash()] = msg;
        mapNotifysActive[nNotifyID] = msg;
        return true;
    }

    return false;
}

void CSporkManager::Relay(CNotifyMessage& msg)
{
    CInv inv(MSG_NOTIFY, msg.GetHash());
    RelayInv(inv);
}

int CSporkManager::GetNotifyIDByName(std::string strName)
{
    for (const CNotifyDef& notifyDef : notifyDefs) {
        if (notifyDef.name == strName){
            return notifyDef.notifyId;
        }
    }
    return -1;
}

std::string CSporkManager::GetNotifyNameByID(int id)
{
    for (const CNotifyDef& notifyDef : notifyDefs) {
        if (notifyDef.notifyId == id){
            return notifyDef.name;
        }
    }
    return "Unknown";
}

//
// CNotifyDB
//

CNotifyDB::CNotifyDB()
{
    pathNotify = GetDataDir() / "notify.dat";
    strMagicMessage = "Notify";
}

bool CNotifyDB::Write(const std::map<int, CNotifyMessage>& notifymapToSave)
{
    int64_t nStart = GetTimeMillis();

    // serialize, checksum data up to that point, then append checksum
    CDataStream ssNotify(SER_DISK, CLIENT_VERSION);
    ssNotify << strMagicMessage; //file specific magic message
    ssNotify << FLATDATA(Params().MessageStart()); // network specific magic number
    ssNotify << notifymapToSave;
    uint256 hash = Hash(ssNotify.begin(), ssNotify.end());
    ssNotify << hash;

    // open output file, and associate with CAutoFile
    FILE* file = fopen(pathNotify.string().c_str(), "wb");
    CAutoFile fileout(file, SER_DISK, CLIENT_VERSION);
    if (fileout.IsNull())
        return error("%s : Failed to open file %s", __func__, pathNotify.string());

    // Write and commit header, data
    try {
        fileout << ssNotify;
    } catch (std::exception& e) {
        return error("%s : Serialize or I/O error - %s", __func__, e.what());
    }
    // FileCommit(fileout);
    fileout.fclose();

    LogPrintf("Written info to notfiy.dat  %dms\n", GetTimeMillis() - nStart);

    return true;
}

CNotifyDB::ReadResult CNotifyDB::Read(std::map<int, CNotifyMessage>& notifymapToLoad)
{
    int64_t nStart = GetTimeMillis();
    // open input file, and associate with CAutoFile
    FILE* file = fopen(pathNotify.string().c_str(), "rb");
    CAutoFile filein(file, SER_DISK, CLIENT_VERSION);
    if (filein.IsNull()) {
        error("%s : Failed to open file %s", __func__, pathNotify.string());
        return FileError;
    }

    // use file size to size memory buffer
    int fileSize = boost::filesystem::file_size(pathNotify);
    int dataSize = fileSize - sizeof(uint256);
    // Don't try to resize to a negative number if file is small
    if (dataSize < 0)
        dataSize = 0;
    vector<unsigned char> vchData;
    vchData.resize(dataSize);
    uint256 hashIn;

    // read data and checksum from file
    try {
        filein.read((char*)&vchData[0], dataSize);
        filein >> hashIn;
    } catch (std::exception& e) {
        error("%s : Deserialize or I/O error - %s", __func__, e.what());
        return HashReadError;
    }
    filein.fclose();

    CDataStream ssNotify(vchData, SER_DISK, CLIENT_VERSION);

    // verify stored checksum matches input data
    uint256 hashTmp = Hash(ssNotify.begin(), ssNotify.end());
    if (hashIn != hashTmp) {
        error("%s : Checksum mismatch, data corrupted", __func__);
        return IncorrectHash;
    }

    unsigned char pchMsgTmp[4];
    std::string strMagicMessageTmp;
    try {
        // de-serialize file header (notify file specific magic message) and ..
        ssNotify >> strMagicMessageTmp;

        // ... verify the message matches predefined one
        if (strMagicMessage != strMagicMessageTmp) {
            error("%s : Invalid notify magic message", __func__);
            return IncorrectMagicMessage;
        }

        // de-serialize file header (network specific magic number) and ..
        ssNotify >> FLATDATA(pchMsgTmp);

        // ... verify the network matches ours
        if (memcmp(pchMsgTmp, Params().MessageStart(), sizeof(pchMsgTmp))) {
            error("%s : Invalid network magic number", __func__);
            return IncorrectMagicNumber;
        }
        // de-serialize data into CResParam object
        ssNotify >> notifymapToLoad;
    } catch (std::exception& e) {
        error("%s : Deserialize or I/O error - %s", __func__, e.what());
        return IncorrectFormat;
    }

    LogPrintf("Loaded info from notify.dat  %dms\n", GetTimeMillis() - nStart);

    return Ok;
}

void DumpNotify()
{
    int64_t nStart = GetTimeMillis();

    CNotifyDB notifydb;
    std::map<int, CNotifyMessage> tempmapNotify;

    LogPrintf("Verifying notify.dat format...\n");
    CNotifyDB::ReadResult readResult = notifydb.Read(tempmapNotify);
    // there was an error and it was not an error on file opening => do not proceed
    if (readResult == CNotifyDB::FileError)
        LogPrintf("Missing notify file - notify.dat, will try to recreate\n");
    else if (readResult != CNotifyDB::Ok) {
        LogPrintf("Error reading notify.dat: ");
        if (readResult == CNotifyDB::IncorrectFormat)
            LogPrintf("magic is ok but data has invalid format, will try to recreate\n");
        else {
            LogPrintf("file format is unknown or invalid, please fix it manually\n");
            return;
        }
    }

    if(!mapNotifysActive.empty()) {
        LogPrintf("Writting info to notify.dat...\n");
        notifydb.Write(mapNotifysActive);
        LogPrintf("notify dump finished  %dms\n", GetTimeMillis() - nStart);
    }
}
