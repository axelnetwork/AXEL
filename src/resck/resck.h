#ifndef AXEL_RESCK_H
#define AXEL_RESCK_H

#include "uint256.h"
#include "resckparam.h"

// ***** macros ***** //
#define SCORE_100 (100)
#define SCORE_80 (80)
#define SCORE_60 (60)
#define SCORE_40 (40)
#define SCORE_20 (20)
#define SCORE_10 (10)

using namespace std;

class CResParam;
extern CResParam resckparam;

class CResCker
{
private:
    enum ResType {
        CPU_NUM,
        CPU_FREQ,
        MEM_SIZE,
        HD_SIZE,
        NET_BANDWIDTH
    };

    string getOsType();
    long getCpuNumber();
    long getCpuFreq();
    long getMemorySize();
    long getHardDiskSize();
    long getNetBandwidth();
    string getCPUId();
    string getMac();

    float getResScore(ResType type, long value);
    bool isTierResTotalScorePassed(unsigned level);
    bool isTierResPassed(unsigned level);
    string getResId();

public:
    bool CkRes(string strPubKeyMasternode, uint256 hash, uint32_t n, unsigned level, std::vector<unsigned char> sigAuth, int64_t sigTime, string& errorMessage, string& strHostID, vector<unsigned char>& vchSigRes);
    bool VerifyCkRes(string strPubKeyMasternode, uint256 hash, uint32_t n, unsigned level, std::vector<unsigned char> sigAuth, int64_t sigTime, string strHostID, vector<unsigned char> vchSigRes, std::string& errorMessage);
    string ResTest();
};

#endif
