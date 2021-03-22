#ifndef AXEL_RESCKPARAM_H
#define AXEL_RESCKPARAM_H

#include "spork.h"
#include "sync.h"
#include "serialize.h"
#include "univalue/include/univalue.h"

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

typedef enum _ERESParamValueType{
    RES_PARAM_TYPE_E_INT = 1,
    RES_PARAM_TYPE_E_FLOAT,
}ERESParamValueType;

class CRESParamDef
{
public:
    CRESParamDef(): paramId(-1), value(0) {}
    CRESParamDef(int id,  std::string n, double val,ERESParamValueType type): paramId(id), value(val), name(n), valueType(type) {}
    int paramId;
    ERESParamValueType valueType;
    double value;
    std::string name;
};

class CResParam
{
private:
    std::map<int, CRESParamDef> mapParam;
    void add(int id, std::string n, int val);
    void add(int id, std::string n, int64_t val);
    void add(int id, std::string n, double val, ERESParamValueType type=RES_PARAM_TYPE_E_FLOAT);
    void setParam(string name,UniValue val);

public:
    CResParam();
    double getParam(string name);
    bool ProcessResCkParam(CNotifyMessage& notifyMsg);
    std::vector<CNotifyInternalMsg> getAllParams();
    std::vector<CNotifyInternalMsg> ParseStringToVNotifyInternalMsg(string strValue);
    string ParseVNotifyInternalMsgToString(std::vector<CNotifyInternalMsg> vValue);
};

#endif
