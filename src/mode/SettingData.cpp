/*
 * SettingData.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include "SettingData.h"
#include "SourceDef.h"

SettingData::SettingData()
{
    m_nCurrWnd = 0;
    m_nDataMute = 0;
	m_nDataKeyVol = 1;  // ������Ĭ�Ͽ���
    m_nBtState = 1;

    m_strMPUVer = "";
    m_strMCUVer = "";
    m_strHARDVer = "";

    m_strHwVer = "";
	m_strSoftVer = "";
	m_strTpVer = "";
    
    //m_strVin = "";
    memset(m_strVin, 0, 17);
    m_INTDataMap.clear();
    m_TireRsetOnclick = false;
    m_DtsOnclick = false;
    m_RoadsideParkOnclick = false;
	m_StorageParkOnclick = false;
	m_RaderSoundOnclick = false;
    memset(&m_tBtInfo, 0, sizeof(BtLocalInfo_t));
    memset(&m_tPairedlist, 0, sizeof(BT_PAIRED_LIST_T));
    pthread_mutex_init( &m_Lock, NULL );
    pthread_cond_init( &m_cond, NULL );

    m_bDemoMode = false;

}

SettingData::~SettingData()
{
    pthread_mutex_destroy( &m_Lock );
	pthread_cond_destroy( &m_cond );
}

void SettingData::InsertPacket(const T_STR_DATA_PACKET tPacket)
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

void SettingData::GetPopPacket(T_STR_DATA_PACKET &tPacket)
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

int SettingData::GetVecPacket_Size()
{
    int nSize = 0;
    Lock();
    nSize = m_vecDataPacket.size();
    UnLock();
    return nSize;
}

void SettingData::WaitForPacket()
{
    Lock();
    while( m_vecDataPacket.empty())
    {
        pthread_cond_wait( &m_cond, &m_Lock );
    }
    UnLock();
}

void SettingData::SaveINTData(const int nkey, const int &Data)
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
bool SettingData::GetINTData(const int nKey, int &Data)
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


void SettingData::SaveBtLocalInfo(const BtLocalInfo_t *tBtInfo)
{
    Lock();
    m_tBtInfo = *tBtInfo;
    UnLock();
}
void SettingData::GetBtLocalInfo(BtLocalInfo_t &tBtInfo)
{
    Lock();
    tBtInfo = m_tBtInfo;
    UnLock();
}

void SettingData::SavePairedlist(const BT_PAIRED_LIST_T *Pairedlist)
{
    Lock();
    m_tPairedlist = *Pairedlist;
    UnLock();
}
void SettingData::GetPairedlist(BT_PAIRED_LIST_T &Pairedlist)
{
    Lock();
    Pairedlist = m_tPairedlist;
    UnLock();
}


void SettingData::SetKeyVolState(int State)
{
    Lock();
    m_nDataKeyVol = State;
    UnLock();
}

int SettingData::GetKeyVolState()
{
    return m_nDataKeyVol;
}

void SettingData::SetMuteState(int State)
{
    Lock();
    m_nDataMute = State;
    UnLock();
}

int SettingData::GetMuteState()
{
    return m_nDataMute;
}


void SettingData::SaveBtState(int State)
{
    Lock();
    m_nBtState = State;
    UnLock();
}

int SettingData::GetBtState()
{
    return m_nBtState;
}

void SettingData::SaveBTName(std::string strBTName)
{
    Lock();
    m_strBTName = strBTName;
    memcpy(m_tBtInfo.BTName, strBTName.c_str(), strBTName.size());
    UnLock();
}
void SettingData::GetBTName(std::string &strBTName)
{
    Lock();
    strBTName = m_strBTName;
    UnLock();
}


void SettingData::SaveTimeData(const T_STR_TIME_INFO &tTime)
{
    Lock();
    m_tTime = tTime;
    UnLock();
}

void SettingData::GetTimeData(T_STR_TIME_INFO &tTime)
{
    Lock();
    tTime = m_tTime;
    UnLock();
}

void SettingData::SaveMPUVersion(std::string strMPUVerion)
{
    Lock();
    m_strMPUVer = strMPUVerion;
    UnLock();
}
void SettingData::GetMPUVersion(std::string &strMPUVerion)
{
    Lock();
    strMPUVerion = m_strMPUVer;
    UnLock();
}
void SettingData::SaveMCUVersion(std::string strMCUVerion)
{
    Lock();
    m_strMCUVer = strMCUVerion;
    UnLock();
}
void SettingData::GetMCUVersion(std::string &strMCUVerion)
{
    Lock();
    strMCUVerion = m_strMCUVer;
    UnLock();
}
void SettingData::SaveHARDVersion(std::string strHARDVer)
{
    Lock();
    m_strHARDVer = strHARDVer;
    UnLock();
}
void SettingData::GetHARDVersion(std::string &strHARDVer)
{
    Lock();
    strHARDVer = m_strHARDVer;
    UnLock();
}


void SettingData::SaveScreenHwVer(std::string strHwVer)
{
    Lock();
    m_strHwVer = strHwVer;
    UnLock();
}
void SettingData::GetScreenHwVer(std::string &strHwVer)
{
    Lock();
    strHwVer = m_strHwVer;
    UnLock();
}

void SettingData::SaveScreenSoftVer(std::string strSoftVer)
{
    Lock();
    m_strSoftVer = strSoftVer;
    UnLock();
}
void SettingData::GetScreenSoftVer(std::string &strSoftVer)
{
    Lock();
    strSoftVer = m_strSoftVer;
    UnLock();
}

void SettingData::SaveScreenTpVer(std::string strTpVer)
{
    Lock();
    m_strTpVer = strTpVer;
    UnLock();
}
void SettingData::GetScreenTpVer(std::string &strTpVer)
{
    Lock();
    strTpVer = m_strTpVer;
    UnLock();
}

void SettingData::SaveVinData(const char* strVin)
{
    Lock();
    //m_strVin = strVin;
    memcpy(m_strVin, strVin, sizeof(m_strVin));

    UnLock();
}
void SettingData::GetVinData(char* strVin)
{
    Lock();
    //strVin = m_strVin;
    memcpy(strVin, m_strVin, sizeof(m_strVin));
    UnLock();
}

void SettingData::SetTireRsetOnclick(bool nState)
{
    Lock();
    m_TireRsetOnclick = nState;
    UnLock();
}

bool SettingData::GetTireRsetOnclick()
{
    int nState = 0;
    Lock();
    nState = m_TireRsetOnclick;
    UnLock();
    return nState;
}

void SettingData::SetDtsOnclick(bool nState)
{
    Lock();
    m_DtsOnclick = nState;
    UnLock();
}

bool SettingData::GetDtsOnclick()
{
    bool nState = false;
    Lock();
    nState = m_DtsOnclick;
    UnLock();
    return nState;
}

void SettingData::SetRoadsideParkOnclick(bool nState)
{
    Lock();
    m_RoadsideParkOnclick = nState;
    UnLock();
}

bool SettingData::GetRoadsideParkOnclick()
{
    bool nState = false;
    Lock();
    nState = m_RoadsideParkOnclick;
    UnLock();
    return nState;
}

void SettingData::SetStorageParkOnclick(bool nState)
{
    Lock();
    m_StorageParkOnclick = nState;
    UnLock();
}

bool SettingData::GetStorageParkOnclick()
{
    bool nState = false;
    Lock();
    nState = m_StorageParkOnclick;
    UnLock();
    return nState;
}

void SettingData::SetRaderSoundOnclick(bool nState)
{
    Lock();
    m_RaderSoundOnclick = nState;
    UnLock();
}

bool SettingData::GetRaderSoundOnclick()
{
    bool nState = false;
    Lock();
    nState = m_RaderSoundOnclick;
    UnLock();
    return nState;
}

void SettingData::SetDemoMode(bool nState)
{
    Lock();
    m_bDemoMode = nState;
    UnLock();   
}

bool SettingData::GetDemoMode()
{
    bool nState = false;
    Lock();
    nState = m_bDemoMode;
    UnLock();
    return nState;
}
