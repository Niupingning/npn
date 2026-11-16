/*
 * MediaDataPacket.cpp
 *
 *  Created on: 2021-08-18
 *      Author: Y3553
 */

#include "MediaDataPacket.h"
#include "IVIController.h"
#include "MediaData.h"
#include "IVIView.h"
#include "SourceDef.h"

MediaDataPacket::MediaDataPacket(IVIController *pCtrl)
{
	m_pCtrl = pCtrl;
	pthread_mutex_init (&m_Lock, NULL);

	m_bExitFlag = false;

	//SetToRunning(true);
}

MediaDataPacket::~MediaDataPacket()
{
	pthread_mutex_destroy( &m_Lock );
}

bool MediaDataPacket::Init()
{
	bool nRet = false;
	nRet = ThreadControl::Init();
	nRet = nRet & ThreadIsReady();
	DoIt();
	return nRet;
}

void MediaDataPacket::DoIt()
{
	ThreadControl::DoIt();
}

bool MediaDataPacket::ThreadIsReady()
{
	return ThreadControl::ThreadIsReady();
}

void MediaDataPacket::Close()
{
	ThreadControl::Close();
}

void MediaDataPacket::DealWithTask()
{
	static bool bFlag = true;
	if(bFlag)
	{
		bFlag = false;
		int ret = pthread_setname_np(pthread_self(), "MediaDataPacket");
		if( ret != EOK )
		{
			bFlag = true;
			LOGDBG(" DataPacket pthread_setname_np failed!");
		}
	}

	while (!m_bExitFlag)
	{
		MediaData &mData = MediaData::GetInstance();
		mData.WaitForPacket();
		if(mData.GetVecPacket_Size() > 0)
		{
			memset(&m_tDataPacket, 0, sizeof(T_STR_DATA_PACKET));
			mData.GetPopPacket(m_tDataPacket);
			ParseDataPacket(m_tDataPacket);
		}
	}
}

void MediaDataPacket::ParseDataPacket(T_STR_DATA_PACKET tPacket)
{
	LOGDBG(" tPacket.nSrvId = %d",tPacket.nSrvId);
	bool bRet = false;
	T_APP_MSG nMsg = {0, 0, 0, 0 };
	MediaData &mData = MediaData::GetInstance();

	switch(tPacket.nSrvId)
	{
		case SRV_ID_SYSTEM_BINDER:
		{
			if(MPU_SYSTEM_USB_STATUS == tPacket.nCmd)
			{
				if(0 == tPacket.nParam1)
				{
					//移除U盘
					nMsg.Message = SYS_VMSG_USBMOUNT;
					nMsg.Param0 = 0;
				}
				else if(1 == tPacket.nParam1)
				{
					//插入U盘
					char tmpMount[128] = {0};
					memcpy(tmpMount,tPacket.Buf,tPacket.nLen);

					mData.SetMountPath(tmpMount,tPacket.nLen);

					nMsg.Message = SYS_VMSG_USBMOUNT;
					nMsg.Param0 = 1;
				}
				
				bRet = true;
			}
			break;
		}
		case SRV_ID_MEDIA_BINDER:
		{
			if(MEDIA_CMD_PLAY_STATUS == tPacket.nCmd)
			{
				char strPlayUrl[MAX_FILE_LEN] = {0};
				memcpy(strPlayUrl,tPacket.Buf,tPacket.nLen);

				mData.SetCurPlayUrl(strPlayUrl);

				nMsg.Message = MEDIA_VMSG_PLAYSTATUS;
				nMsg.Param0 = tPacket.nParam1;
				nMsg.Param1 = tPacket.nParam2;

				bRet = true;
			}
			else if(MEDIA_CMD_PLAY_TIME == tPacket.nCmd)
			{
				nMsg.Message = MEDIA_VMSG_PLAYTIME;
				nMsg.Param0 = tPacket.nParam1;
				nMsg.Param1 = tPacket.nParam2;
				nMsg.Param2 = tPacket.Buf[0];

				bRet = true;				
			}
			break;
		}
		default:
		{
			break;
		}
	}
	if(bRet && (0 != nMsg.Message))
	{
		LOGDBG("HomeApp:SendViewMessage:Message=%d,Param0=%d,Param1=%d,Param2=%d\n",\
									nMsg.Message, nMsg.Param0, nMsg.Param1, nMsg.Param2 );
		IVIView &mView = IVIView::GetInstance();
		mView.SendViewMessage(&nMsg);
	}
}

