/*
 * SettingData.h
 *
 *  Created on: 2021-08-03
 *      Author: Y
 */
 
#ifndef SETTINGDATA_H_
#define SETTINGDATA_H_

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>

#include "FileBase.h"
#include "AppDatadef.h"
#include "log.h"

using FY_NAMESPACE::MutexLock;
using namespace std;

class SettingData 
{
public:
	SettingData();
	virtual ~SettingData();

	static SettingData &GetInstance()
	{
   	 	static SettingData instance;
    	return instance;
	}

	void InsertPacket(const T_STR_DATA_PACKET tPacket);
	void GetPopPacket(T_STR_DATA_PACKET &tPacket);
	int GetVecPacket_Size();
	void WaitForPacket();

	void SaveINTData(const int nkey, const int &Data);
	bool GetINTData(const int nKey, int &Data);

	void SetKeyVolState(int State);
	int GetKeyVolState();

	void SetMuteState(int State);
	int GetMuteState();

	void SaveBtState(int State);
	int GetBtState();

	void SaveBtLocalInfo(const BtLocalInfo_t *tBtInfo);
	void GetBtLocalInfo(BtLocalInfo_t &tBtInfo);

	void GetBTName(std::string &strBTName);
	void SaveBTName(std::string strBTName);

	void SavePairedlist(const BT_PAIRED_LIST_T *Pairedlist);
	void GetPairedlist(BT_PAIRED_LIST_T &Pairedlist);

	void SaveMPUVersion(std::string strMPUVerion);
	void GetMPUVersion(std::string &strMPUVerion);
	void SaveMCUVersion(std::string strMCUVerion);
	void GetMCUVersion(std::string &strMCUVerion);
	void SaveHARDVersion(std::string strHARDVer);
	void GetHARDVersion(std::string &strHARDVer);

	void SaveScreenHwVer(std::string strHwVer);
	void GetScreenHwVer(std::string &strHwVer);
	void SaveScreenSoftVer(std::string strSoftVer);
	void GetScreenSoftVer(std::string &strSoftVer);
	void SaveScreenTpVer(std::string strTpVer);
	void GetScreenTpVer(std::string &strTpVer);

	void SaveVinData(std::string strVin);
	void GetVinData(std::string &strVin);

	void SaveVinData(const char* strVin);
	void GetVinData(char* strVin);	

	void SaveTimeData(const T_STR_TIME_INFO &tTime);
	void GetTimeData(T_STR_TIME_INFO &tTime);

	void SetCurrWnd(int nWndID){m_nCurrWnd = nWndID;}
	int GetCurrWnd(){return m_nCurrWnd;}

	void SetTireRsetOnclick(bool nState);
	bool GetTireRsetOnclick();

	void SetDtsOnclick(bool nState);
	bool GetDtsOnclick();
	void SetRoadsideParkOnclick(bool nState);
	bool GetRoadsideParkOnclick();
	void SetStorageParkOnclick(bool nState);
	bool GetStorageParkOnclick();
	void SetRaderSoundOnclick(bool nState);
	bool GetRaderSoundOnclick();
	void SetDemoMode(bool nState);
	bool GetDemoMode();
private:
	void Lock(){pthread_mutex_lock( &m_Lock );}
	void UnLock(){pthread_mutex_unlock( &m_Lock );}

private:


	std::vector <T_STR_DATA_PACKET> m_vecDataPacket;

	int m_nCurrWnd;
	int m_nDataMute;
	int m_nDataKeyVol;
	int m_nBtState;

	std::map<int, int> m_INTDataMap;
	std::string m_strMPUVer;
	std::string m_strMCUVer;
	std::string m_strHARDVer;
	std::string m_strBTName;
	
	std::string m_strHwVer;
	std::string m_strSoftVer;
	std::string m_strTpVer;

	char m_strVin[17];

	T_STR_TIME_INFO m_tTime; 
	T_VOLUME_INFO m_Volume;

	BtLocalInfo_t m_tBtInfo;
	BT_PAIRED_LIST_T m_tPairedlist;

	pthread_cond_t m_cond;
	pthread_mutex_t m_Lock;
	bool m_TireRsetOnclick;
	bool m_DtsOnclick;
	bool m_RoadsideParkOnclick;
	bool m_StorageParkOnclick;
	bool m_RaderSoundOnclick;

	bool m_bDemoMode;       //true:on
};


#endif /* SETTINGDATA_H_ */
