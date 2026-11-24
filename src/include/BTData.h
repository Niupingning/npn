/*
 * BTData.h
 *
 *  Created on: 2025-09-25
 *      Author: Y6915
 */
 
#ifndef BTDATA_H_
#define BTDATA_H_

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>

#include "FileBase.h"
#include "AppDatadef.h"
#include "Systemdef.h"
#include "log.h"
#include "SPPlatformControl.h"
using namespace spplfctrl;
using FY_NAMESPACE::MutexLock;
using namespace std;

enum MusicState
{
	Music_Pause = 0,
	Music_Play,
};

typedef struct
{
	BTPassThroughCommand nPlayCmd;
	int nParam;
}T_PLAYMUSIC_CMD;

class BTData
{
public:
	BTData();
	virtual ~BTData();

	static BTData &GetInstance()
	{
   	 	static BTData instance;
    	return instance;
	}

	void InsertPacket(const T_STR_DATA_PACKET tPacket);
	void GetPopPacket(T_STR_DATA_PACKET &tPacket);
	int GetVecPacket_Size();
	void WaitForPacket();
	
	void SaveINTData(const int nkey, const int &Data);
	bool GetINTData(const int nKey, int &Data);

    void SetCurrWnd(int nWndID);
	int GetCurrWnd();

	void SetLastWnd(int nWndID);
	int GetLastWnd();
	
	void SaveBTMusicInfo(const BtMusicInfo_t *tBTMusic);
	void GetBTMusicInfo(BtMusicInfo_t &tBTMusic);

	void SaveBTMusicTime(const BtPlaybackTime_t *tBtMusicTime);
	void GetBTMusicTime(BtPlaybackTime_t &tBtMusicTime);

	void SaveBTMusicStatus(const BTAudioStatus tBTMusicSts);
	int GetBTMusicStatus();

	void SaveBTPhoneInfo(const T_STR_BT_CALLERINFO *tBTCall);
	void GetBTPhoneInfo(T_STR_BT_CALLERINFO &tBTCall);
	void SaveBTPhoneCallState(const int nState);
	int  GetBTPhoneCallState();
	void ClearBTPhoneCallInfo();                    // 清空当前通话人信息
	//通话状态

	void SaveBTPhoneSyncInfo(const T_STR_BT_SYNCINFO *tBTPhoneInfo);
	void GetBTPhoneSyncInfo(T_STR_BT_SYNCINFO &tBTPhoneInfo);
	//蓝牙电话本同步状态信息
	
	void SaveBTPhoneTime(const int tTime);
	int GetBTPhoneTime();

	void SaveBTPhoneCallMode(const int tBTCallMode);
	int GetBTPhoneCallMode();

	void SaveBTMuteStatus(const int tMuteStatus);
	int GetBTMuteStatus();

	void SaveResourceType(const int tType);
	int GetResourceType();
	
	void SaveOnClickInfo(const T_STR_ONCLICK_INFO &tInfo);
	void GetOnClickInfo(T_STR_ONCLICK_INFO &tInfo);
	
	void SaveBtPairingReq(const BTPairingRegInfo_t *tBtPairingReq);
	void GetBtPairingReq(BTPairingRegInfo_t &tBtPairingReq);

	void SaveBtLocalInfo(const BtLocalInfo_t *tBtLocalInfo);
	void GetBtLocalInfo(BtLocalInfo_t &tBtLocalInfo);

	void SaveBtDiscoverList(const BtBondedDeviceToAppInfo_t *tBtDiscoverList);
	void GetBtBtDiscoverList(BtBondedDeviceToAppInfo_t &tBtDiscoverList);
	
	void SaveBtPairISFrist(const uint8_t *state);
	void GetBtPairISFrist(uint8_t &state);

	void SaveBtConnectStatus(const int status);
	int GetBtConnectStatus();

	void SaveBtOpenStatus(const int status);
	int GetBtOpenStatus();

	void SaveBtPhoneTimeISUpdate(bool nshow);
	bool GetBtPhoneTimeISUpdate();

	void SaveBTPairedListInfo(const BtPairedDeviceToAppInfo_t *pinfo);
	void GetBTPairedListInfo(BtPairedDeviceToAppInfo_t &pinfo);

	void SaveBtDeviceName(char *bt_name);
	void GetBtDeviceName(char *bt_name);
	void SaveBtContacts(const BtPhoneBookSync *tBtContacts);
	void GetBtContacts(BtPhoneContactData &tBtContacts);
	void SaveBtCallHistoryDial(const BtRecordData_t *tBtCallHistory);
	void SaveBtCallHistoryAnswer(const BtRecordData_t *tBtCallHistory);
	void SaveBtCallHistoryMiss(const BtRecordData_t *tBtCallHistory);
	void GetBtCallHistoryDial(BtRecordData_t &tBtCallHistory);
	void GetBtCallHistoryAnswer(BtRecordData_t &tBtCallHistory);
	void GetBtCallHistoryMiss(BtRecordData_t &tBtCallHistory);

	
private:
	void Lock(){pthread_mutex_lock( &m_Lock );}
	void UnLock(){pthread_mutex_unlock( &m_Lock );}
	
private:
	BtMusicInfo_t m_tBTMusic;
	T_STR_BT_CALLERINFO m_tBTCall;
	int m_BTCallState;          // 最近一次"已确认"的通话状态
	T_STR_BT_SYNCINFO m_tBTPhoneInfo;
	T_STR_ONCLICK_INFO m_tOnClickInfo;
	BTPairingRegInfo_t m_BtPairingReq;
//	BtPhoneBookSync m_BtContacts;
	BtPhoneContactData m_BtContacts;
	BtRecordData_t m_BtDialHistory;				//拔打
	BtRecordData_t m_BtAnswerlHistory;			//接听
	BtRecordData_t m_BtMissCalllHistory;		//未接
	int m_BtConnectStatus;
	int m_BtOpenStatus;
	BtPairedDeviceToAppInfo_t m_BTPairedInfo;
	BtBondedDeviceToAppInfo_t m_tBTDiscoverList;
	BtPlaybackTime_t m_tBTMusicTime;
	BtLocalInfo_t m_tBTLocalInfo;

	std::map<int, int> m_INTDataMap;

	std::vector <T_STR_DATA_PACKET> m_vecDataPacket;

	bool m_BtPhoneTimeISUpdate;
	uint8_t m_BtpairIsFfrist;
	int m_nCurrWnd;
	int m_nLastWnd;
	int m_PhoneTime;
	int m_ResourceType;
	int m_CallMode;
	int m_MuteStatus;
	BTAudioStatus m_tBTMusicSts;

	char m_BtDeviceName[BT_MAX_LOCAL_NAME_LENGTH];

	
	pthread_cond_t m_cond;
	pthread_mutex_t m_Lock;
	
	
};

#endif