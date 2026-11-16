/*
 * BTDataPacket.h
 *
 *  Created on: 2025-09-25
 *      Author: Y6915
 */
 
#ifndef BTDATAPACKET_H_
#define BTDATAPACKET_H_
 
#include  "FileBase.h"
#include  "AppDatadef.h"
#include  "UIID_Def_Home.h"
#include  "UIID_Def_Setting.h"
//#include  "Systemdef.h"
using FY_NAMESPACE::T_APP_MSG; 
using FY_NAMESPACE::ThreadControl; 

class IVIController;
class BTDataPacket : public ThreadControl
{
public:
	BTDataPacket(IVIController *pCtrl);
	virtual ~BTDataPacket();

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
	//bool HandleSystemData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	bool HandleWirelessSerData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	//bool HandleMobileNetData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	//bool HandleAppManagerData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	//bool HandleCarPlayData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	//bool HandleAndroidData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);
	//bool HandleReversesrvData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg);


private:
	IVIController *m_pCtrl;

	pthread_mutex_t m_Lock;

	T_STR_DATA_PACKET m_tDataPacket;

	bool m_bExitFlag;

};
 
#endif