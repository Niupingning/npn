/*
 *  @ DataPacket.h
 *  @ Brief:Data analysis class, analyze all kinds of data
 *  @ Created on: 2021-08-18
 *  @ Author: Y3553
 */

#ifndef RADIODATAPACKET_H_
#define RADIODATAPACKET_H_

#include  "FileBase.h"
#include  "AppDatadef.h"
#include  "UIID_Def_Radio.h"

using FY_NAMESPACE::T_APP_MSG; 
using FY_NAMESPACE::ThreadControl; 

class IVIController;
class RadioDataPacket : public ThreadControl
{
public:
	RadioDataPacket(IVIController *pCtrl);
	virtual ~RadioDataPacket();

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

	bool HandleMeterInfoData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	bool HandleSystemData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	bool HandleWirelessSerData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	bool HandleMobileNetData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	bool HandleAppManagerData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);

private:
	IVIController *m_pCtrl;

	pthread_mutex_t m_Lock;

	T_STR_DATA_PACKET m_tDataPacket;

	bool m_bExitFlag;

};

#endif /*RADIODATAPACKET_H_*/
