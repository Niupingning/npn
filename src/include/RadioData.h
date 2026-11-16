#ifndef RADIODATA_H_
#define RADIODATA_H_

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>

#include "FileBase.h"
#include "AppDatadef.h"
#include "log.h"
#include "UIID_Def_Radio.h"

using FY_NAMESPACE::MutexLock;
using namespace std;

class RadioData 
{
public:
	RadioData();
	virtual ~RadioData();

	static RadioData &GetInstance()
	{
   	 	static RadioData instance;
    	return instance;
	}

	void InsertPacket(const T_STR_DATA_PACKET tPacket);
	void GetPopPacket(T_STR_DATA_PACKET &tPacket);
	int GetVecPacket_Size();
	void WaitForPacket();

	void SaveINTData(const int nkey, const int &Data);
	bool GetINTData(const int nKey, int &Data);

	void SetCurrWnd(int nWndID){m_nCurrWnd = nWndID;}
	int GetCurrWnd(){return m_nCurrWnd;}

	bool SaveCurrFreq(const T_RADIO_CURR_FREQ &tCurrFreq);
	bool GetCurrFreq(T_RADIO_CURR_FREQ &tCurrFreq);

	void SetLastGroup(int nGroupID);
	int GetLastGroup();

	void SetSearchingstatus(int nstatus);
	int GetSearchingstatus();

	bool SaveRDSSwitch(const T_RADIO_RDS_SWITCH &tRDSSwitchStatus);
	bool GetRDSSwitch(T_RADIO_RDS_SWITCH &tRDSSwitchStatus);

	bool SaveRDSSatus(const T_RADIO_RDS_SATUS &tRDSStatus);
	bool GetRDSSatus(T_RADIO_RDS_SATUS &tRDSStatus);

	bool SaveRDSSationName(const T_RADIO_RDS_PTY &tRDSStationName);
	bool GetRDSSationName(T_RADIO_RDS_PTY &tRDSStationName);

	bool SaveRDSSationInfo(char *tRDSStationInfo);
	bool GetRDSSationInfo(char *tRDSStationInfo);

private:
	void Lock(){pthread_mutex_lock( &m_Lock );}
	void UnLock(){pthread_mutex_unlock( &m_Lock );}

private:
	std::vector <T_STR_DATA_PACKET> m_vecDataPacket;

	int m_nCurrWnd;

	std::map<int, int> m_INTDataMap;

	pthread_cond_t m_cond;
	pthread_mutex_t m_Lock;

	T_RADIO_CURR_FREQ         m_RadioInfo;
	int m_GroupID;
	int m_Searchingstatus;
	T_RADIO_RDS_SWITCH m_RDSSwitchStatus;
	T_RADIO_RDS_SATUS m_RDSStatus;
	T_RADIO_RDS_PTY m_RDSStationName;
	char m_RDSStationInfo[65];
};


#endif /* RADIODATA_H_ */