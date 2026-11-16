/*
 *  @ DataPacket.h
 *  @ Brief:Data analysis class, analyze all kinds of data
 *  @ Created on: 2021-08-18
 *  @ Author: Y3553
 */

#ifndef SETTINGDATAPACKET_H_
#define SETTINGDATAPACKET_H_

#include  "FileBase.h"
#include  "AppDatadef.h"

using FY_NAMESPACE::T_APP_MSG; 
using FY_NAMESPACE::ThreadControl; 

class IVIController;
class SettingDataPacket : public ThreadControl
{
public:
	SettingDataPacket(IVIController *pCtrl);
	virtual ~SettingDataPacket();

	bool Init();
	void Close();

	void ParseDataPacket(T_STR_DATA_PACKET tPacket);

protected:
	/*Data Handle Thread*/
	void DealWithTask();
	void DoIt();
	bool ThreadIsReady();
private:
	void Lock(bool bLook){bLook ? pthread_mutex_lock(&m_Lock) : pthread_mutex_unlock(&m_Lock);};

	bool HandleSysteminfoData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	bool HandleAppManagerData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	bool HandleMeterInfoData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	bool HandleWirelessData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);

private:
	IVIController *m_pCtrl;

	pthread_mutex_t m_Lock;

	T_STR_DATA_PACKET m_tDataPacket;

	bool m_bExitFlag;

};

#endif /*SETTINGDATAPACKET_H_*/
