/*
 * HomeData.h
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */
 
#ifndef HOMEDATA_H_
#define HOMEDATA_H_

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>

#include "FileBase.h"
#include "AppDatadef.h"
#include "log.h"
#include "SPPlatformControl.h"
using namespace spplfctrl;
using FY_NAMESPACE::MutexLock;
using namespace std;

class HomeData 
{
public:
	HomeData();
	virtual ~HomeData();

	static HomeData &GetInstance()
	{
   	 	static HomeData instance;
    	return instance;
	}

	void InsertPacket(const T_STR_DATA_PACKET tPacket);
	void GetPopPacket(T_STR_DATA_PACKET &tPacket);
	int GetVecPacket_Size();
	void WaitForPacket();

	void SaveTireData(const T_STR_TIRE_INFO &tTire);
	void GetTireData(T_STR_TIRE_INFO &tTire);

	void SaveTimeData(const T_STR_TIME_INFO &tTime);
	void GetTimeData(T_STR_TIME_INFO &tTime);

	void SaveINTData(const int nkey, const int &Data);
	bool GetINTData(const int nKey, int &Data);

	void SetCurrWnd(int nWndID);
	int GetCurrWnd();

	void SetLastWnd(int nWndID);
	int GetLastWnd();

	/*tm*/
	void SaveVehicleState(const T_STR_VEHICLE_STATE_INFO &tVehicleState);
	void GetVehicleState(T_STR_VEHICLE_STATE_INFO &tVehicleState);

	void SaveTireAlarm(const T_STR_TIRE_ALARM &tTireAlarm);
	void GetTireAlarm(T_STR_TIRE_ALARM &tTireAlarm);

	void SaveBTMusicInfo(const T_STR_BT_MUSICINFO *tBTMusic);
	void GetBTMusicInfo(T_STR_BT_MUSICINFO &tBTMusic);

	void SaveBTPhoneInfo(const T_STR_BT_CALLERINFO *tBTCall);
	void GetBTPhoneInfo(T_STR_BT_CALLERINFO &tBTCall);

	void SaveBTPhoneTime(const int tTime);
	int GetBTPhoneTime();

	void SaveResourceType(const int tType);
	int GetResourceType();

	void SaveLastCharge(const T_HOME_LASTCHARGE_INFO &tLastCharge);
	void GetLastCharge(T_HOME_LASTCHARGE_INFO &tLastCharge);

	void SaveWifiInfo(const T_STR_WIFI_INFO *tWifiInfo);
	void GetWifiInfo(T_STR_WIFI_INFO &tWifiInfo);

	void SaveKeyValue(const T_STR_KEY_INFO &tKeyInfo);
	void GetKeyValue(T_STR_KEY_INFO &tKeyInfo);

	void SaveOldKeyValue(const T_STR_KEY_INFO &tKeyInfo);
	void GetOldKeyValue(T_STR_KEY_INFO &tKeyInfo);

	void SaveVolumeType(const int &tType);
	int GetVolumeType();

	void SaveWifiMode(const int &tMode);
	int GetWifiMode();

	void SaveMileageData(const T_STR_MILEAGE_INFO &tMileage);
	void GetMileageData(T_STR_MILEAGE_INFO &tMileage);

	void SaveCellData(const T_STR_CELL_INFO &tCell);
	void GetCellData(T_STR_CELL_INFO &tCell);

	void SaveOnClickInfo(const T_STR_ONCLICK_INFO &tInfo);
	void GetOnClickInfo(T_STR_ONCLICK_INFO &tInfo);

	bool SaveVendorUUID();
	void GetVendorUUID(std::string &strUUID);
	static int readVendorDataString(char *data, int len, int puPos);

	void SaveBtPairingReq(const T_BtPAIRINGREQ *tBtPairingReq);
	void GetBtPairingReq(T_BtPAIRINGREQ &tBtPairingReq);

	void SaveBtPairISFrist(bool state);
	bool GetBtPairISFrist();

	void SaveBtPhoneTimeISUpdate(bool nshow);
	bool GetBtPhoneTimeISUpdate();

	void SaveRushStatus(bool Status);
	bool GetRushStatus();
	
	void SaveParkingStatus(int Status);
	int GetParkingStatus();

	void SavePowerData(const T_STR_POWER_INFO &tPower);
	void GetPowerData(T_STR_POWER_INFO &tPower);
	
	void SaveAnimaMode(int nmode);
	void GetAnimaMode(int &nmode);
	
	void SetCompleteWnd(int nWndID);
	int GetCompleteWnd();

	void SetComboboxAnima(bool nPlay);
	bool GetComboboxAnima();

	bool SaveVendorDisPose();
	int GetVendorEPS();
	int GetVendorTPMS();
	int GetVendorER();
	int readVendorDataString_1(char *data, int len, int puPos);

	void SetTireRsetWarn(bool nShow);
	bool GetTireRsetWarn();

	void SetTireRsetWarnOFFClose(bool nShow);
	bool GetTireRsetWarnOFFClose();

	void SaveScreenLockInfo(const T_STR_SCREEN_LOCK_INFO tScreen);
	void GetScreenLockInfo(T_STR_SCREEN_LOCK_INFO &tScreen);

	bool GetVendorDataAA();
	bool GetAAOpenStatus();

	void SaveBackPath(int strBackPath);
	void GetBackPath(int &strBackPath);

	void SaveLockPasNum(int nLockPasNum);
	int GetLockPasNum();

	void SetCanBox_AirData(T_CANBOX_AIR_DATA &tAirData);
	void GetCanBox_AirData(T_CANBOX_AIR_DATA &tAirData);

	void SetCanBox_CarInfo(T_CANBOX_CAR_DATA &tCarData);
	void GetCanBox_CarInfo(T_CANBOX_CAR_DATA &tCarData);

	void SetCanBox_CarSetting(T_CANBOX_CAR_SETTING &tCarSetting);
	void GetCanBox_CarSetting(T_CANBOX_CAR_SETTING &tCarSetting);
	void SetDts_EQSetting(T_DTS_EQ_SETTING &tEqSetting);
	void GetDts_EQSetting(T_DTS_EQ_SETTING &tEqSetting);
private:
	void Lock(){pthread_mutex_lock( &m_Lock );}
	void UnLock(){pthread_mutex_unlock( &m_Lock );}

private:
	T_STR_TIRE_INFO m_tTire;
	T_STR_TIME_INFO m_tTime;

	std::map<int, int> m_INTDataMap;

	std::vector <T_STR_DATA_PACKET> m_vecDataPacket;

	int m_nCurrWnd;
	int m_nLastWnd;
	pthread_cond_t m_cond;
	pthread_mutex_t m_Lock;

	/*tm*/
	T_STR_VEHICLE_STATE_INFO m_VehicleState;
	T_STR_TIRE_ALARM m_TireAlarm;
	T_STR_BT_MUSICINFO m_tBTMusic;
	T_STR_BT_CALLERINFO m_tBTCall;

	int m_PhoneTime;
	int m_ResourceType;

	T_HOME_LASTCHARGE_INFO m_LastCharge;
	T_STR_WIFI_INFO m_tWifiInfo;
	T_STR_KEY_INFO m_tKeyInfo;
	T_STR_KEY_INFO m_tOldKeyInfo;
	int m_VolumeType;
	int m_WifiMode;

	T_STR_MILEAGE_INFO m_tMileage;
	T_STR_CELL_INFO m_tCell;

	T_STR_ONCLICK_INFO m_tOnClickInfo;

	std::string m_strUUID;
	T_BtPAIRINGREQ m_BtPairingReq;
	bool m_BtpairIsFfrist;
	bool m_BtPhoneTimeISUpdate;

	bool m_RushStatus;
	int m_ParkingStatus;
	T_STR_POWER_INFO m_tPower;
	int m_HomeAnimaMode;
	int m_nCompleteWnd;

	bool m_nComboboxAnima;
	int m_DisPose_EPS;
	int m_DisPose_TPMS;
	int m_DisPose_EnergyRecover;
	SPPlatformControl *platfomCrtl;
	bool m_TireRsetWarn;
	bool m_TireRsetWarnOFFClose;

	T_STR_SCREEN_LOCK_INFO	m_tScreenLock;
	int m_AAOpenStatus;
	int m_strBackPath;
	int m_LockPasNum;
	T_CANBOX_AIR_DATA m_tCanBox_AirData;
	T_CANBOX_CAR_DATA m_tCanBox_CarInfo;

	T_CANBOX_CAR_SETTING m_tCanBox_CarSetting;

	T_DTS_EQ_SETTING m_tDts_Eq_Setting;
};


#endif /* HOMEDATA_H_ */
