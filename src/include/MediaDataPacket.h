/*
 *  @ DataPacket.h
 *  @ Brief:Data analysis class, analyze all kinds of data
 *  @ Created on: 2021-08-18
 *  @ Author: Y3553
 */

#ifndef MEDIADATAPACKET_H_
#define MEDIADATAPACKET_H_

#include  "FileBase.h"
#include  "AppDatadef.h"
#include  "UIID_Def_Home.h"
#include  "UIID_Def_Setting.h"
using FY_NAMESPACE::T_APP_MSG; 
using FY_NAMESPACE::ThreadControl; 

class IVIController;
class MediaDataPacket : public ThreadControl
{
public:
	MediaDataPacket(IVIController *pCtrl);
	virtual ~MediaDataPacket();

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

private:
	IVIController *m_pCtrl;

	pthread_mutex_t m_Lock;

	T_STR_DATA_PACKET m_tDataPacket;

	bool m_bExitFlag;

};

#endif /*HOMEDATAPACKET_H_*/
