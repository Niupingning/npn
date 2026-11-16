/*
 * HomeData.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include "HomeData.h"
#include "SourceDef.h"
#include "SPVendorDataList.h"

HomeData::HomeData()
{
    m_nCurrWnd = 0;
    m_nLastWnd = 1;
    m_PhoneTime = 0;
    m_ResourceType = -1;
    m_VolumeType = -1;
    m_WifiMode = E_WIFI_MODE_P2P;

    m_strUUID = "";
    //SaveVendorUUID();
    m_BtpairIsFfrist = true;
    m_BtPhoneTimeISUpdate = false;
    m_RushStatus = false;
    m_ParkingStatus = 1;
    m_HomeAnimaMode = 0;
    m_nCompleteWnd = 0;
    m_nComboboxAnima =false;
    m_DisPose_EPS = 0;
    m_DisPose_TPMS = 0;
    m_DisPose_EnergyRecover = 1;
    m_TireRsetWarn = false;
    m_TireRsetWarnOFFClose = false;
    m_AAOpenStatus = 0;
    m_strBackPath = 0;
    m_LockPasNum = 0;
    pthread_mutex_init( &m_Lock, NULL );
    pthread_cond_init( &m_cond, NULL );

    //默认CP/AA没有加载资源
    SaveINTData(INT_MODILERESOURCE_CP,1);
    SaveINTData(INT_MODILERESOURCE_AA,1);
    platfomCrtl = SPPlatformControl::GetInstance();
    memset(&m_tCanBox_AirData,0,sizeof(T_CANBOX_AIR_DATA));
    memset(&m_tCanBox_CarSetting,0,sizeof(T_CANBOX_CAR_SETTING));

    memset(&m_tDts_Eq_Setting,0,sizeof(T_DTS_EQ_SETTING));
}

HomeData::~HomeData()
{
    platfomCrtl->ReleaseInstance(platfomCrtl);
    platfomCrtl = NULL;
    pthread_mutex_destroy( &m_Lock );
	pthread_cond_destroy( &m_cond );
}

void HomeData::InsertPacket(const T_STR_DATA_PACKET tPacket)
{
    Lock();
    if((SRV_ID_APPMANAGER_BINDER == tPacket.nSrvId)
         && (APPMGR_CMD_DISP_CTRL == tPacket.nCmd))
    {
        m_vecDataPacket.insert(m_vecDataPacket.begin(),tPacket);
    }
    else
    {
        m_vecDataPacket.push_back(tPacket);
    }
    pthread_cond_signal( &m_cond );
    UnLock();
}

void HomeData::GetPopPacket(T_STR_DATA_PACKET &tPacket)
{
    Lock();
    if(m_vecDataPacket.size()>0)
    {
        std::vector<T_STR_DATA_PACKET>::iterator ItPacket = m_vecDataPacket.begin();
        tPacket = *ItPacket;
        m_vecDataPacket.erase(ItPacket);
    }
    UnLock();
}

int HomeData::GetVecPacket_Size()
{
    int nSize = 0;
    Lock();
    nSize = m_vecDataPacket.size();
    UnLock();
    return nSize;
}

void HomeData::WaitForPacket()
{
    Lock();
    while( m_vecDataPacket.empty())
    {
        pthread_cond_wait( &m_cond, &m_Lock );
    }
    UnLock();
}


void HomeData::SaveTireData(const T_STR_TIRE_INFO &tTire)
{
    Lock();
    m_tTire = tTire;
    UnLock();
}
void HomeData::GetTireData(T_STR_TIRE_INFO &tTire)
{
    Lock();
    tTire = m_tTire;
    UnLock();
}



void HomeData::SaveTimeData(const T_STR_TIME_INFO &tTime)
{
    Lock();
    m_tTime = tTime;
    UnLock();
}

void HomeData::GetTimeData(T_STR_TIME_INFO &tTime)
{
    Lock();
    tTime = m_tTime;
    UnLock();
}

void HomeData::SaveINTData(const int nkey, const int &Data)
{
    Lock();
    auto Itr = m_INTDataMap.find(nkey);
    if(m_INTDataMap.end() != Itr)
    {
        Itr->second = Data;
    }
    else
    {
        m_INTDataMap.insert(std::make_pair(nkey, Data));
    }

    UnLock();
}
bool HomeData::GetINTData(const int nKey, int &Data)
{
    bool bRet = false;
    Lock();
    auto Itr = m_INTDataMap.find(nKey);
    if(m_INTDataMap.end() != Itr)
    {
        Data = Itr->second;
        bRet = true;
    }
    UnLock();
    return bRet;
}

/*tm*/
void HomeData::SaveVehicleState(const T_STR_VEHICLE_STATE_INFO &tVehicleState)
{
    Lock();
    m_VehicleState = tVehicleState;
    UnLock();
}

void HomeData::GetVehicleState(T_STR_VEHICLE_STATE_INFO &tVehicleState)
{
    Lock();
    tVehicleState = m_VehicleState;
    UnLock();
}

void HomeData::SaveTireAlarm(const T_STR_TIRE_ALARM &tTireAlarm)
{
    Lock();
    m_TireAlarm = tTireAlarm;
    UnLock();
}

void HomeData::GetTireAlarm(T_STR_TIRE_ALARM &tTireAlarm)
{
    Lock();
    tTireAlarm = m_TireAlarm;
    UnLock();
}

void HomeData::SaveBTMusicInfo(const T_STR_BT_MUSICINFO *tBTMusic)
{
    Lock();
    m_tBTMusic = *tBTMusic;
    UnLock();
}
void HomeData::GetBTMusicInfo(T_STR_BT_MUSICINFO &tBTMusic)
{
    Lock();
    tBTMusic = m_tBTMusic;
    UnLock();
}

void HomeData::SaveBTPhoneInfo(const T_STR_BT_CALLERINFO *tBTCall)
{
    Lock();
    m_tBTCall = *tBTCall;
    UnLock();
}
void HomeData::GetBTPhoneInfo(T_STR_BT_CALLERINFO &tBTCall)
{
    Lock();
    tBTCall = m_tBTCall;
    UnLock();
}

void HomeData::SetCurrWnd(int nWndID)
{
    Lock();
    m_nLastWnd = m_nCurrWnd;
    m_nCurrWnd = nWndID;
    UnLock();
}

int HomeData::GetCurrWnd()
{
    int nWndID = 0;
    Lock();
    nWndID = m_nCurrWnd;
    UnLock();
    return nWndID;
}

void HomeData::SetLastWnd(int nWndID)
{
    Lock();
    m_nLastWnd = nWndID;
    UnLock();
}

int HomeData::GetLastWnd()
{
    int nWndID = 0;
    Lock();
    nWndID = m_nLastWnd;
    UnLock();
    return nWndID;
}

void HomeData::SaveBTPhoneTime(const int tTime)
{
    Lock();
    m_PhoneTime = tTime;
    UnLock();
}

int HomeData::GetBTPhoneTime()
{
    int tTime = 0;
    Lock();
    tTime = m_PhoneTime;
    UnLock();
    return tTime;
}

void HomeData::SaveResourceType(const int tType)
{
    Lock();
    m_ResourceType = tType;
    UnLock();
}
int HomeData::GetResourceType()
{
    int tType = 0;
    Lock();
    tType = m_ResourceType;
    UnLock();
    return tType; 
}

void HomeData::SaveLastCharge(const T_HOME_LASTCHARGE_INFO &tLastCharge)
{
    Lock();
    m_LastCharge = tLastCharge;
    UnLock();
}
void HomeData::GetLastCharge(T_HOME_LASTCHARGE_INFO &tLastCharge)
{
    Lock();
    tLastCharge = m_LastCharge;
    UnLock();
}

void HomeData::SaveWifiInfo(const T_STR_WIFI_INFO *tWifiInfo)
{
    Lock();
    m_tWifiInfo = *tWifiInfo;
    UnLock();
}
void HomeData::GetWifiInfo(T_STR_WIFI_INFO &tWifiInfo)
{
    Lock();
    tWifiInfo = m_tWifiInfo;
    UnLock();
}

void HomeData::SaveKeyValue(const T_STR_KEY_INFO &tKeyInfo)
{
    Lock();
    m_tKeyInfo = tKeyInfo;
    UnLock();
}
void HomeData::GetKeyValue(T_STR_KEY_INFO &tKeyInfo)
{
    Lock();
    tKeyInfo = m_tKeyInfo;
    UnLock();
}

void HomeData::SaveOldKeyValue(const T_STR_KEY_INFO &tKeyInfo)
{
    Lock();
    m_tOldKeyInfo = tKeyInfo;
    UnLock();
}
void HomeData::GetOldKeyValue(T_STR_KEY_INFO &tKeyInfo)
{
    Lock();
    tKeyInfo = m_tOldKeyInfo;
    UnLock();
}

void HomeData::SaveVolumeType(const int &tType)
{
    Lock();
    m_VolumeType = tType;
    UnLock();   
}

int  HomeData::GetVolumeType()
{
    int tType = -1;
    Lock();
    tType = m_VolumeType;
    UnLock(); 
    return tType;  
}

void HomeData::SaveWifiMode(const int &tMode)
{
    Lock();
    m_WifiMode = tMode;
    UnLock();   
}

int  HomeData::GetWifiMode()
{
    int tMode = -1;
    Lock();
    tMode = m_WifiMode;
    UnLock(); 
    return tMode;  
}

void HomeData::SaveMileageData(const T_STR_MILEAGE_INFO &tMileage)
{
    Lock();
    m_tMileage = tMileage;
    UnLock();
}
void HomeData::GetMileageData(T_STR_MILEAGE_INFO &tMileage)
{
    Lock();
    tMileage = m_tMileage;
    UnLock();
}

void HomeData::SaveCellData(const T_STR_CELL_INFO &tCell)
{
    Lock();
    m_tCell = tCell;
    UnLock();
}

void HomeData::GetCellData(T_STR_CELL_INFO &tCell)
{
    Lock();
    tCell = m_tCell;
    UnLock();
}

void HomeData::SaveOnClickInfo(const T_STR_ONCLICK_INFO &tInfo)
{
    Lock();
    m_tOnClickInfo = tInfo;
    UnLock();
}
void HomeData::GetOnClickInfo(T_STR_ONCLICK_INFO &tInfo)
{
    Lock();
    tInfo = m_tOnClickInfo;
    UnLock();
}

bool HomeData::SaveVendorUUID()
{
    bool bRet = false;
    char uuid[17] = {0};
    int ret = -1;
    //ret = readVendorDataString(uuid, 16, SP_VENDOR_DATA_CARBIT_UUID_POS);
    if (ret < 0) 
    {
        LOGDBG("read uuid failed!!!\n");
        bRet = false;
    } 
    else 
    {
        m_strUUID = uuid;
        LOGDBG("read uuid:%s,ok\n",uuid);
        bRet = true;
    }
    return bRet;
}
void HomeData::GetVendorUUID(std::string &strUUID)
{
    strUUID = m_strUUID;
}
int HomeData::readVendorDataString(char *data, int len, int puPos)
{
	SPPlatformControl *platfomCrtl = SPPlatformControl::GetInstance();

	if(!platfomCrtl)
	{
		LOGDBG("get SPPlatformControl instance failed!\n");
		return -1;
	}

	int ret = platfomCrtl->Vendor_Data_Read_String(puPos , len, data);

	platfomCrtl->ReleaseInstance(platfomCrtl);

	return ret;
}

void HomeData::SaveBtPairingReq(const T_BtPAIRINGREQ *tBtPairingReq)
{
    Lock();
    m_BtPairingReq = *tBtPairingReq;
    UnLock();
}
void HomeData::GetBtPairingReq(T_BtPAIRINGREQ &tBtPairingReq)
{
    Lock();
    tBtPairingReq = m_BtPairingReq;
    UnLock();
}

void HomeData::SaveBtPairISFrist(bool state)
{
    Lock();
    m_BtpairIsFfrist = state;
    UnLock();
}
bool HomeData::GetBtPairISFrist()
{
    bool state = true;
    Lock();
    state = m_BtpairIsFfrist;
    UnLock();
    return state;
}

void HomeData::SaveBtPhoneTimeISUpdate(bool nshow)
{
    Lock();
    m_BtPhoneTimeISUpdate = nshow;
    UnLock();
}
bool HomeData::GetBtPhoneTimeISUpdate()
{
    bool nshow = true;
    Lock();
    nshow = m_BtPhoneTimeISUpdate;
    UnLock();
    return nshow;
}

void HomeData::SaveRushStatus(bool Status)
{
    Lock();
    m_RushStatus = Status;
    UnLock();
}
bool HomeData::GetRushStatus()
{
    bool Status = false;
    Lock();
    Status = m_RushStatus;
    UnLock();
    return Status;
}

void HomeData::SaveParkingStatus(int Status)
{
    Lock();
    m_ParkingStatus = Status;
    UnLock();
}
int HomeData::GetParkingStatus()
{
    int Status = 1;
    Lock();
    Status = m_ParkingStatus;
    UnLock();
    return Status;
}

void HomeData::SavePowerData(const T_STR_POWER_INFO &tPower)
{
    Lock();
    m_tPower = tPower;
    UnLock();
}
void HomeData::GetPowerData(T_STR_POWER_INFO &tPower)
{
    Lock();
    tPower = m_tPower;
    UnLock();
}

void HomeData::SaveAnimaMode(int nmode)
{
    Lock();
    m_HomeAnimaMode = nmode;
    UnLock();
}

void HomeData::GetAnimaMode(int &nmode)
{
    Lock();
    nmode = m_HomeAnimaMode;
    UnLock();
}

void HomeData::SetCompleteWnd(int nWndID)
{
    Lock();
    m_nCompleteWnd = nWndID;
    UnLock();
}

int HomeData::GetCompleteWnd()
{
    int nWndID = 0;
    Lock();
    nWndID = m_nCompleteWnd;
    UnLock();
    return nWndID;
}

void HomeData::SetComboboxAnima(bool nPlay)
{
    Lock();
    m_nComboboxAnima = nPlay;
    UnLock();
}

bool HomeData::GetComboboxAnima()
{
    int nPlay = 0;
    Lock();
    nPlay = m_nComboboxAnima;
    UnLock();
    return nPlay;
}

bool HomeData::SaveVendorDisPose()
{
    bool bRet = false;
    int nlen = 12;
    char DisPose[nlen] = {0};
    int ret = -1;
    //ret = readVendorDataString_1(DisPose, nlen, SP_VENDOR_DATA_CONFIG_INFO_POS);
    if (ret < 0) 
    {
        LOGDBG("read DisPose failed!!!\n");
        bRet = false;
    } 
    else 
    {
        m_DisPose_EPS = (DisPose[5] - '0') & (1 - (DisPose[6] - '0'));
        m_DisPose_EnergyRecover = DisPose[7] - '0';
        m_DisPose_TPMS = DisPose[10] - '0';
        LOGDBG("read DisPose[5]:%d,ok\n",DisPose[5]);
        LOGDBG("read DisPose[6]:%d,ok\n",DisPose[6]);
        LOGDBG("read DisPose[7]:%d,ok\n",DisPose[7]);
        LOGDBG("read m_DisPose_EPS:%d,ok\n",m_DisPose_EPS);
        LOGDBG("read DisPose[10]:%d,ok\n",DisPose[10]);
        bRet = true;
    }
    return bRet;
}
//0不配 1配
int HomeData::GetVendorEPS()
{
    int DisPose_EPS = 0;
    Lock();
    DisPose_EPS = m_DisPose_EPS;
    UnLock();
    return DisPose_EPS;
}
//0不配 非0配
int HomeData::GetVendorTPMS()
{
    int DisPose_TPMS = 0;
    Lock();
    DisPose_TPMS = m_DisPose_TPMS;
    UnLock();
    return DisPose_TPMS;
}

//0可以调，1不可调
int HomeData::GetVendorER()
{
    int DisPose_EnergyRecover = 0;
    Lock();
    DisPose_EnergyRecover = m_DisPose_EnergyRecover;
    UnLock();
    return DisPose_EnergyRecover;
}

int HomeData::readVendorDataString_1(char *data, int len, int puPos)
{
	if(!platfomCrtl)
	{
		LOGDBG("get SPPlatformControl instance failed!\n");
		return -1;
	}

	int ret = platfomCrtl->Vendor_Data_Read_String(puPos , len, data);
	return ret;
}

void HomeData::SetTireRsetWarn(bool nShow)
{
    Lock();
    m_TireRsetWarn = nShow;
    UnLock();
}

bool HomeData::GetTireRsetWarn()
{
    int nShow = 0;
    Lock();
    nShow = m_TireRsetWarn;
    UnLock();
    return nShow;
}

void HomeData::SetTireRsetWarnOFFClose(bool nShow)
{
    Lock();
    m_TireRsetWarnOFFClose = nShow;
    UnLock();
}

bool HomeData::GetTireRsetWarnOFFClose()
{
    int nShow = 0;
    Lock();
    nShow = m_TireRsetWarnOFFClose;
    UnLock();
    return nShow;
}

void HomeData::SaveScreenLockInfo(const T_STR_SCREEN_LOCK_INFO tScreen)
{
    Lock();
    m_tScreenLock = tScreen;
    UnLock();
}
void HomeData::GetScreenLockInfo(T_STR_SCREEN_LOCK_INFO &tScreen)
{
    Lock();
    tScreen = m_tScreenLock;
    UnLock();
}

bool HomeData::GetVendorDataAA()
{
	if(!platfomCrtl)
	{
		LOGDBG("get SPPlatformControl instance failed!\n");
		return -1;
	}
    char data[1] = {0};
	int ret = -1;
	//int ret = platfomCrtl->Vendor_Data_Read_String(SP_VENDOR_DATA_AA_CONFIG_STATUS_POS , 1, data);
    if(ret != -1)
    {
        if(data[0] == '1')
        {
            m_AAOpenStatus = 1;
        }
        else
        {
            m_AAOpenStatus = 0;
        }
        LOGDBG("m_AAOpenStatus = %d",m_AAOpenStatus);
        return true;
    }
    return false;
}

bool HomeData::GetAAOpenStatus()
{
    int nShow = 0;
    Lock();
    nShow = m_AAOpenStatus;
    UnLock();
    return nShow;
}

void HomeData::SaveBackPath(int strBackPath)
{
    Lock();
    m_strBackPath = strBackPath;
    UnLock();
}
void HomeData::GetBackPath(int &strBackPath)
{
    Lock();
    strBackPath = m_strBackPath;
    UnLock();
}

void HomeData::SaveLockPasNum(int nLockPasNum)
{
    Lock();
    if(nLockPasNum == 0)
    {
        m_LockPasNum = nLockPasNum;
    }
    else
    {
        m_LockPasNum++;
    }
    UnLock();
}
int HomeData::GetLockPasNum()
{
    int nLockPasNum = 0;
    Lock();
    nLockPasNum = m_LockPasNum;
    UnLock();
    return nLockPasNum;
}
void HomeData::SetCanBox_AirData(T_CANBOX_AIR_DATA &tAirData)
{
    Lock();
    m_tCanBox_AirData = tAirData;
    UnLock();
}

void HomeData::GetCanBox_AirData(T_CANBOX_AIR_DATA &tAirData)
{
    Lock();
    tAirData = m_tCanBox_AirData;
    UnLock();
}

void HomeData::SetCanBox_CarInfo(T_CANBOX_CAR_DATA &tCarData)
{
    Lock();
    m_tCanBox_CarInfo = tCarData;
    UnLock();
}

void HomeData::GetCanBox_CarInfo(T_CANBOX_CAR_DATA &tCarData)
{
    Lock();
    tCarData = m_tCanBox_CarInfo;
    UnLock();
}

void HomeData::SetCanBox_CarSetting(T_CANBOX_CAR_SETTING &tCarSetting)
{
    Lock();
    m_tCanBox_CarSetting = tCarSetting;
    UnLock();
}

void HomeData::GetCanBox_CarSetting(T_CANBOX_CAR_SETTING &tCarSetting)
{
    Lock();
    tCarSetting = m_tCanBox_CarSetting;
    UnLock();
}
void HomeData::SetDts_EQSetting(T_DTS_EQ_SETTING &tEqSetting)
{
    Lock();
    m_tDts_Eq_Setting = tEqSetting;
    UnLock();
}

void HomeData::GetDts_EQSetting(T_DTS_EQ_SETTING &tEqSetting)
{
    Lock();
    tEqSetting = m_tDts_Eq_Setting;
    UnLock();
}
