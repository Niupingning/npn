/*
 * RadioData.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include "RadioData.h"
#include "SourceDef.h"

RadioData::RadioData()
{
    m_nCurrWnd = 0;
    m_INTDataMap.clear();
    m_GroupID = UIID_CONTENT_RADIO_GROUP;
    m_Searchingstatus = RADIO_SEARSTATUS_NO;
    pthread_mutex_init( &m_Lock, NULL );
    pthread_cond_init( &m_cond, NULL );
    memset(&m_RDSStationInfo, 0 , sizeof(m_RDSStationInfo));   
}

RadioData::~RadioData()
{
    pthread_mutex_destroy( &m_Lock );
	pthread_cond_destroy( &m_cond );
}

void RadioData::InsertPacket(const T_STR_DATA_PACKET tPacket)
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

void RadioData::GetPopPacket(T_STR_DATA_PACKET &tPacket)
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

int RadioData::GetVecPacket_Size()
{
    int nSize = 0;
    Lock();
    nSize = m_vecDataPacket.size();
    UnLock();
    return nSize;
}

void RadioData::WaitForPacket()
{
    Lock();
    while( m_vecDataPacket.empty())
    {
        pthread_cond_wait( &m_cond, &m_Lock );
    }
    UnLock();
}

void RadioData::SaveINTData(const int nkey, const int &Data)
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
bool RadioData::GetINTData(const int nKey, int &Data)
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


bool RadioData::SaveCurrFreq(const T_RADIO_CURR_FREQ &tCurrFreq)
{
	Lock();
	m_RadioInfo = tCurrFreq;
	UnLock();
    return true;
}


bool RadioData::GetCurrFreq(T_RADIO_CURR_FREQ &tCurrFreq)
{
    Lock();
	tCurrFreq = m_RadioInfo;
    UnLock();
    return true;
}
void RadioData::SetLastGroup(int nGroupID)
{
    Lock();
	m_GroupID = nGroupID;
	UnLock();
}
int RadioData::GetLastGroup()
{
    int nGroupID = 0;
    Lock();
	nGroupID = m_GroupID;
	UnLock();
    return nGroupID;
}

void RadioData::SetSearchingstatus(int nstatus)
{
    Lock();
	m_Searchingstatus = nstatus;
	UnLock();
}
int RadioData::GetSearchingstatus()
{
    int nstatus = 0;
    Lock();
    nstatus = m_Searchingstatus;
    UnLock();
    return nstatus;
}

bool RadioData::SaveRDSSwitch(const T_RADIO_RDS_SWITCH &tRDSSwitchStatus)
{
	Lock();
	m_RDSSwitchStatus = tRDSSwitchStatus;
	UnLock();
    return true;
}


bool RadioData::GetRDSSwitch(T_RADIO_RDS_SWITCH &tRDSSwitchStatus)
{
    Lock();
	tRDSSwitchStatus = m_RDSSwitchStatus;
    UnLock();
    return true;
}

bool RadioData::SaveRDSSatus(const T_RADIO_RDS_SATUS &tRDSStatus)
{
  	Lock();
	m_RDSStatus = tRDSStatus;
	UnLock();
    return true;  
}
bool RadioData::GetRDSSatus(T_RADIO_RDS_SATUS &tRDSStatus)
{
    Lock();
	tRDSStatus = m_RDSStatus;
    UnLock();
    return true;
}

bool RadioData::SaveRDSSationName(const T_RADIO_RDS_PTY &tRDSStationName)
{
   	Lock();
	m_RDSStationName = tRDSStationName;
	UnLock();
    return true;     
}
bool RadioData::GetRDSSationName(T_RADIO_RDS_PTY &tRDSStationName)
{
    Lock();
	tRDSStationName = m_RDSStationName;
    UnLock();
    return true;   
}

bool RadioData::SaveRDSSationInfo(char *tRDSStationInfo)
{
   	Lock();
	memcpy(m_RDSStationInfo, tRDSStationInfo, sizeof(m_RDSStationInfo));
	UnLock();
    return true;     
}

bool RadioData::GetRDSSationInfo(char *tRDSStationInfo)
{
    Lock();
	memcpy(tRDSStationInfo, m_RDSStationInfo, sizeof(m_RDSStationInfo));
    UnLock();
    return true;   
}