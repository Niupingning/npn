/*
 * HomeData.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include "MediaData.h"
#include "SourceDef.h"
#include "SPVendorDataList.h"

MediaData::MediaData()
{
    m_nCurrWnd = 0;
    m_nLastWnd = 1;

    memset(m_strMountPath,0,sizeof(m_strMountPath));
    memset(m_strPlayUrl,0,sizeof(m_strPlayUrl));
    
    pthread_mutex_init( &m_Lock, NULL );
    pthread_cond_init( &m_cond, NULL );
}

MediaData::~MediaData()
{
    pthread_mutex_destroy( &m_Lock );
	pthread_cond_destroy( &m_cond );
}

void MediaData::InsertPacket(const T_STR_DATA_PACKET tPacket)
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

void MediaData::GetPopPacket(T_STR_DATA_PACKET &tPacket)
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

int MediaData::GetVecPacket_Size()
{
    int nSize = 0;
    Lock();
    nSize = m_vecDataPacket.size();
    UnLock();
    return nSize;
}

void MediaData::WaitForPacket()
{
    Lock();
    while( m_vecDataPacket.empty())
    {
        pthread_cond_wait( &m_cond, &m_Lock );
    }
    UnLock();
}

void MediaData::SetCurrWnd(int nWndID)
{
    Lock();
    m_nCurrWnd = nWndID;
    UnLock();
}

int MediaData::GetCurrWnd()
{
    int nWndID = 0;
    Lock();
    nWndID = m_nCurrWnd;
    UnLock();
    return nWndID;
}

void MediaData::SetLastWnd(int nWndID)
{
    Lock();
    m_nLastWnd = nWndID;
    UnLock();
}

int MediaData::GetLastWnd()
{
    int nWndID = 0;
    Lock();
    nWndID = m_nLastWnd;
    UnLock();
    return nWndID;
}

void MediaData::SetMountPath(const char *pMount,int nLen)
{
    Lock();
    if((NULL == pMount) || (nLen <= 0))
    {
        //拔盘/无效路径：整块清空，避免残留的旧挂载路径把“已拔出”判成“已挂载”
        memset(m_strMountPath, 0, sizeof(m_strMountPath));
    }
    else
    {
        int nCopy = nLen;
        if(nCopy >= (int)sizeof(m_strMountPath))
        {
            nCopy = (int)sizeof(m_strMountPath) - 1;
        }
        memset(m_strMountPath, 0, sizeof(m_strMountPath));
        memcpy(m_strMountPath, pMount, nCopy);
        m_strMountPath[nCopy] = '\0';
    }
    UnLock();
}

void MediaData::GetMountPath(char *pMount,int nLen)
{
    Lock();
    if(NULL != pMount)
    {
        memcpy(pMount,m_strMountPath,nLen);
    }
    UnLock();
}

void MediaData::SetCurPlayUrl(const char *pPlayUrl)
{
    Lock();
    memset(m_strPlayUrl, 0, sizeof(m_strPlayUrl));
    if((NULL != pPlayUrl) && ('\0' != pPlayUrl[0]))
    {
        strncpy(m_strPlayUrl, pPlayUrl, sizeof(m_strPlayUrl) - 1);
    }
    UnLock();
}

void MediaData::GetCurPlayUrl(char *pPlayUrl)
{
    Lock();
    if(NULL != pPlayUrl)
    {
        memcpy(pPlayUrl,m_strPlayUrl,MAX_FILE_LEN);
    }
    UnLock();
}

