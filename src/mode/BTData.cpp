/*
 * BTData.cpp
 *
 *  Created on: 2025-09-26
 *      Author: Y6915
 */

#include "BTData.h"
#include "SourceDef.h"
#include "SPVendorDataList.h"

BTData::BTData()
{
	m_nCurrWnd = 0;
    m_nLastWnd = 1;
    m_PhoneTime = 0;
}

BTData::~BTData()
{
    pthread_mutex_destroy( &m_Lock );
	pthread_cond_destroy( &m_cond );
}

void BTData::InsertPacket(const T_STR_DATA_PACKET tPacket)
{
    Lock();
    LOGDBG("------XSJXSJXSJBTData----------");
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

void BTData::GetPopPacket(T_STR_DATA_PACKET &tPacket)
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

int BTData::GetVecPacket_Size()
{
    int nSize = 0;
    Lock();
    nSize = m_vecDataPacket.size();
    UnLock();
    return nSize;
}

void BTData::WaitForPacket()
{
    Lock();
    while( m_vecDataPacket.empty())
    {
        pthread_cond_wait( &m_cond, &m_Lock );
    }
    UnLock();
}

void BTData::SetCurrWnd(int nWndID)
{
    Lock();
    m_nCurrWnd = nWndID;
    UnLock();
}

int BTData::GetCurrWnd()
{
    int nWndID = 0;
    Lock();
    nWndID = m_nCurrWnd;
    UnLock();
    return nWndID;
}

void BTData::SetLastWnd(int nWndID)
{
    Lock();
    m_nLastWnd = nWndID;
    UnLock();
}

int BTData::GetLastWnd()
{
    int nWndID = 0;
    Lock();
    nWndID = m_nLastWnd;
    UnLock();
    return nWndID;
}

void BTData::SaveINTData(const int nkey, const int &Data)
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
bool BTData::GetINTData(const int nKey, int &Data)
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

void BTData::SaveBTMusicInfo(const BtMusicInfo_t *tBTMusic)
{
    Lock();
    m_tBTMusic = *tBTMusic;
    UnLock();
}
void BTData::GetBTMusicInfo(BtMusicInfo_t &tBTMusic)
{
    Lock();
    tBTMusic = m_tBTMusic;
    UnLock();
}

void BTData::SaveBTMusicTime(const BtPlaybackTime_t *tBtMusicTime)
{
    Lock();
    m_tBTMusicTime = *tBtMusicTime;
    UnLock();
}
void BTData::GetBTMusicTime(BtPlaybackTime_t &tBtMusicTime)
{
    Lock();
    tBtMusicTime = m_tBTMusicTime;
    UnLock();
}

void BTData::SaveBTMusicStatus(const BTAudioStatus tBTMusicSts)
{
    Lock();
    m_tBTMusicSts = tBTMusicSts;
    UnLock();
}
int BTData::GetBTMusicStatus()
{
    Lock();
    int tBTMusicSts;
    tBTMusicSts = m_tBTMusicSts;
    LOGDBG("&&&---xsjxsj---数据获取--tBTMusicSts:%d",tBTMusicSts);
    UnLock();
    return tBTMusicSts;
}

void BTData::SaveBTPhoneInfo(const T_STR_BT_CALLERINFO *tBTCall)
{
    Lock();
    m_tBTCall = *tBTCall;
    UnLock();
}
void BTData::GetBTPhoneInfo(T_STR_BT_CALLERINFO &tBTCall)
{
    Lock();
    tBTCall = m_tBTCall;
    UnLock();
}

void BTData::SaveBTPhoneSyncInfo(const T_STR_BT_SYNCINFO *tBTPhoInfo)
{
    Lock();
    m_tBTPhoneInfo = *tBTPhoInfo;
    UnLock();
}
void BTData::GetBTPhoneSyncInfo(T_STR_BT_SYNCINFO &tBTPhoInfo)
{
    Lock();
    tBTPhoInfo = m_tBTPhoneInfo;
    UnLock();
}

void BTData::SaveBTPhoneTime(const int tTime)
{
    Lock();
    m_PhoneTime = tTime;
    UnLock();
}
int BTData::GetBTPhoneTime()
{
    int tTime = 0;
    Lock();
    tTime = m_PhoneTime;
    UnLock();
    return tTime;
}

void BTData::SaveBTPhoneCallMode(const int tBTCallMode)
{
    Lock();
    m_CallMode = tBTCallMode;
    UnLock();
}
int BTData::GetBTPhoneCallMode()
{
    int tBTCallMode = 0;
    Lock();
    tBTCallMode = m_CallMode;
    UnLock();
    return tBTCallMode;
}

void BTData::SaveBTMuteStatus(const int tMuteStatus)
{
    Lock();
    m_MuteStatus = tMuteStatus;
    UnLock();
}
int BTData::GetBTMuteStatus()
{
    int tMuteStatus = 0;
    Lock();
    tMuteStatus = m_MuteStatus;
    UnLock();
    return tMuteStatus;
}

void BTData::SaveResourceType(const int tType)
{
    Lock();
    m_ResourceType = tType;
    UnLock();
}
int BTData::GetResourceType()
{
    int tType = 0;
    Lock();
    tType = m_ResourceType;
    UnLock();
    return tType; 
}

void BTData::SaveOnClickInfo(const T_STR_ONCLICK_INFO &tInfo)
{
    Lock();
    m_tOnClickInfo = tInfo;
    UnLock();
}
void BTData::GetOnClickInfo(T_STR_ONCLICK_INFO &tInfo)
{
    Lock();
    tInfo = m_tOnClickInfo;
    UnLock();
}

void BTData::SaveBtPairingReq(const BTPairingRegInfo_t *tBtPairingReq)
{
    Lock();
    m_BtPairingReq = *tBtPairingReq;
    UnLock();
}
void BTData::GetBtPairingReq(BTPairingRegInfo_t &tBtPairingReq)
{
    Lock();
    tBtPairingReq = m_BtPairingReq;
    UnLock();
}


void BTData::SaveBtContacts(const BtPhoneBookSync *tBtContacts)
{
    int contacts = 20;
    int index = (tBtContacts->index - 1) * 20;
    Lock();
    if(tBtContacts->packnum == tBtContacts->index)
    {
        contacts = tBtContacts->curNum % 20;
    }
    m_BtContacts.BtPhoneContacts = tBtContacts->curNum;
    for(int i= 0; i < contacts; i++)
    {
        memcpy( &m_BtContacts.BtPhoneBookInfo[index + i], &tBtContacts->BtPhoneBookInfo[i], sizeof(BtPhoneBookSyncData));
    }
    UnLock();
}
void BTData::GetBtContacts(BtPhoneContactData &tBtContacts)
{
    Lock();
    tBtContacts = m_BtContacts;
    UnLock();
}

void BTData::SaveBtCallHistoryDial(const BtRecordData_t *tBtCallHistory)
{
    Lock();
    m_BtDialHistory = *tBtCallHistory;
    UnLock();
}
void BTData::SaveBtCallHistoryAnswer(const BtRecordData_t *tBtCallHistory)
{
    Lock();
    m_BtAnswerlHistory = *tBtCallHistory;
    UnLock();
}
void BTData::SaveBtCallHistoryMiss(const BtRecordData_t *tBtCallHistory)
{
    Lock();
    m_BtMissCalllHistory = *tBtCallHistory;
    UnLock();
}

void BTData::GetBtCallHistoryDial(BtRecordData_t &tBtCallHistory)
{
    Lock();
    tBtCallHistory = m_BtDialHistory;
    UnLock();
}
void BTData::GetBtCallHistoryAnswer(BtRecordData_t &tBtCallHistory)
{
    Lock();
    tBtCallHistory = m_BtAnswerlHistory;
    UnLock();
}
void BTData::GetBtCallHistoryMiss(BtRecordData_t &tBtCallHistory)
{
    Lock();
    tBtCallHistory = m_BtMissCalllHistory;
    UnLock();
}

void BTData::SaveBtLocalInfo(const BtLocalInfo_t *tBtLocalInfo)
{
    Lock();
    m_tBTLocalInfo = *tBtLocalInfo;
    UnLock();
}
void BTData::GetBtLocalInfo(BtLocalInfo_t &tBtLocalInfo)
{
    Lock();
    tBtLocalInfo = m_tBTLocalInfo;
    UnLock();
}

void BTData::SaveBtDiscoverList(const BtBondedDeviceToAppInfo_t *tBtDiscoverList)
{
    Lock();
    m_tBTDiscoverList = *tBtDiscoverList;
    UnLock();
}
void BTData::GetBtBtDiscoverList(BtBondedDeviceToAppInfo_t &tBtDiscoverList)
{
    Lock();
    tBtDiscoverList = m_tBTDiscoverList;
    UnLock();
}

void BTData::SaveBtPairISFrist(const uint8_t *state)
{
    Lock();
    m_BtpairIsFfrist = *state;
    UnLock();
}
void BTData::GetBtPairISFrist(uint8_t &state)
{
    Lock();
    state = m_BtpairIsFfrist;
    UnLock();
}

void BTData::SaveBtPhoneTimeISUpdate(bool nshow)
{
    Lock();
    m_BtPhoneTimeISUpdate = nshow;
    UnLock();
}
bool BTData::GetBtPhoneTimeISUpdate()
{
    bool nshow = true;
    Lock();
    nshow = m_BtPhoneTimeISUpdate;
    UnLock();
    return nshow;
}

void BTData::SaveBtOpenStatus(const int status)
{
    Lock();
    m_BtOpenStatus = status;
    UnLock();
}
int BTData::GetBtOpenStatus()
{
    int status = 0;
    Lock();
    status = m_BtOpenStatus;
    UnLock();
    return status;
}
void BTData::SaveBtDeviceName(char *bt_name)
{
    Lock();
    strcpy(m_BtDeviceName, bt_name);
    UnLock();
}
void BTData::GetBtDeviceName(char *bt_name)
{
    Lock();
    strcpy(bt_name, m_BtDeviceName);
    UnLock();
}

	void SaveBtDeviceName(char *bt_name);
	void GetBtDeviceName(char &bt_name);

void BTData::SaveBtConnectStatus(const int status)
{
    Lock();
    m_BtConnectStatus = status;
    UnLock();
}
int BTData::GetBtConnectStatus()
{
    int status = 0;
    Lock();
    status = m_BtConnectStatus;
    UnLock();
    return status;
}

void BTData::SaveBTPairedListInfo(const BtPairedDeviceToAppInfo_t *pinfo)
{
    Lock();
    m_BTPairedInfo = *pinfo;
    UnLock();
}
void BTData::GetBTPairedListInfo(BtPairedDeviceToAppInfo_t &pinfo)
{
    Lock();
    pinfo = m_BTPairedInfo;
    UnLock();
}
