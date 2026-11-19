/*
 * IVIController.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include <fcntl.h>
#include <unistd.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include<algorithm>

#include "IVIController.h"
#include "AppDatadef.h"
#include "Systemdef.h"
#include "audiocontrol.h"

#include "backlight_gpio_ctrl.h"
#include "fyaudiocontrol.h"
#include "libgpioctrl.h"

IVIController::IVIController(IViewMessage *pIViewMessage)
:PublicController(pIViewMessage)
,m_HomeDataPacket(this)
,m_SettingDataPacket(this)
,m_RadioDataPacket(this)
,m_MediaDataPacket(this)
,m_BTDataPacket(this)
,m_MediaModuleImpl(this)
,m_resAudioCbk(new ResourceTransferProgramCbk(&m_MediaModuleImpl, ResAudio))
{
	pthread_mutex_init(&m_Lock, NULL);
	m_CMDVec.clear();
	InitCMDVec();
	m_ResCtrol.associateResource(m_resAudioCbk,ResAudio);

	m_bAudioTake = false;
	m_bSetBGMode = false;

	m_nBklight_En = 1;
}

IVIController::~IVIController()
{
	pthread_mutex_destroy(&m_Lock);
}

bool IVIController::OnInit()
{
	bool bRet = false;

	bRet = m_HomeDataPacket.Init();
	bRet = m_SettingDataPacket.Init();
	bRet = m_RadioDataPacket.Init();
	bRet = m_MediaDataPacket.Init();
	bRet = m_BTDataPacket.Init();
	FyAudioControl::FyAudioControlInit();

	bRet = OpenService(SYSTEM_SRV);
	if (!bRet)
	{
		PushReOpenService(SYSTEM_SRV);
	}
	else
	{
		m_SystemSrv = GetServerBinder(SYSTEM_SRV);
		if (nullptr != m_SystemSrv.get())
		{
			//ReadSytemSerData(SYSTEMSRV_CMD_BT_MEMORY);
			ReadSytemSerData(SYSTEMSRV_CMD_BOOTSIGNAL);
			ReadSytemSerData(SYSTEMSRV_CMD_ANIM_END_IVIHOME);
			ReadSytemSerData(SYSTEMSRV_CMD_CONNECT_MODE);
			ReadSytemSerData(SYSTEMSRV_CMD_LANGUAGE_SET);
			ReadSytemSerData(SYSTEMSRV_CMD_SOURCE_MEMORY);
			ReadSytemSerData(SYSTEMSRV_CMD_RADIO_STAUS);
			ReadSytemSerData(SYSTEMSRV_CMD_BTMUSIC_STAUS);
			ReadSytemSerData(SYSTEMSRV_CMD_LOCKSCREEN_STATUS);			
		}
		else
		{
			LOGDBG("GetServerBinder(\"com.system_srv\"); == nullptr !!");
		}
	}

	//open media
	bRet = OpenService(MEDIA_SRV);
	if(!bRet)
	{
		PushReOpenService(MEDIA_SRV);
	}
	else
	{
		m_MediaSrv = GetServerBinder(MEDIA_SRV);

		if(nullptr != m_MediaSrv.get())
		{

		}
		else
		{
			LOGDBG("GetServerBinder(\"com.media_srv\"); == nullptr !!");
		}
	}

	//open meterinfo
	bRet = OpenService(METERINFO_SRV);
	if (!bRet)
	{
		PushReOpenService(METERINFO_SRV);
	}
	else
	{
		
		m_MeterInfoSrv = GetServerBinder(METERINFO_SRV);
		if (nullptr != m_MeterInfoSrv.get())
		{
			ReadMeterInfoData(METERINFO_APP_REPORT_MESSAGE);
			SendMeterInfoDataOFStartingUP(APP_METERINFO_RADIO_COMMAND);
			//SendMeterInfoData(RADIO_MUTE,0,0,0);
		}
		else
		{
			LOGDBG("GetServerBinder(\"METERINFO_SRV\"); == nullptr !!");
		}

	}
	// /*
	bRet = OpenService(CARPLAY_SRV);
	if (!bRet)
	{
		LOGDBG("OpenService(\"com.carplay_srv\"); faild !!");
		PushReOpenService(CARPLAY_SRV);
	}
	else
	{
		LOGDBG("OpenService(\"com.carplay_srv\"); Success !!");
		m_carplaysrv = GetServerBinder(CARPLAY_SRV);
		if (nullptr != m_carplaysrv.get())
		{
			LOGDBG("GetServerBinder(\"com.carplay_srv\"); Success !!");
			HomeData &mData = HomeData::GetInstance();
			//T_STR_SCREEN_LOCK_INFO tScreen;
			//mData.GetScreenLockInfo(tScreen);
			if(0)
			{
				SendCarPlayData(APP_SERVER_MOBILE_RESOURCE_RELEASE,1);
			}
			else
			{
				//下发连接模式
				SendConnectMode(1);

				int nACCStatus = -1;
				mData.GetINTData(INT_MOBIL_ACC,nACCStatus);
				LOGDBG("ReOpenServiceCallBack nACCStatus = %d !!", nACCStatus);
				SendCarPlayData(APP_SERVER_CP_ACCSTATUS,nACCStatus);
			}
		}
		else
		{
			LOGDBG("GetServerBinder(\"com.carplay_srv\"); == nullptr !!");
		}
	}
	// */
	bRet = OpenService(ANDROIDAUTO_SRV);
	if (!bRet)
	{
		LOGDBG("OpenService(\"com.androidauto_srv\"); faild !!");
		PushReOpenService(ANDROIDAUTO_SRV);
	}
	else
	{
		LOGDBG("OpenService(\"com.androidauto_srv\"); Success !!");
		m_Androidautosrv = GetServerBinder(ANDROIDAUTO_SRV);
		if (nullptr != m_Androidautosrv.get())
		{
			LOGDBG("GetServerBinder(\"com.androidauto_srv\"); Success !!");

			HomeData &mData = HomeData::GetInstance();
			T_STR_SCREEN_LOCK_INFO tScreen;
			mData.GetScreenLockInfo(tScreen);
			if(tScreen.nScreenLockStatus == 1)
			{
				SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_RELEASE,1);
			}
			else
			{
				//1.加载基础资源
				SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_LOAD,1);

				//2.获取并同步ACC状态（车辆核心状态）
				int nACCStatus = -1;
				mData.GetINTData(INT_MOBIL_ACC,nACCStatus);
				LOGDBG("ypb::IVIController::OnInit nACCStatus = %d !!", nACCStatus);
				SendAndroidautoData(APP_SERVER_AA_ACCSTATUS,nACCStatus);

				//3.获取并同步蓝牙名称
				SettingData &mSettingData = SettingData::GetInstance();
				BtLocalInfo_t tBtInfo;
				mSettingData.GetBtLocalInfo(tBtInfo);
				SendAAOFBTName(tBtInfo.BTName);

				//4.AA启动类型
				SendAndroidautoData(APP_APPSERVICE_OPENAA_TYPE,1);
				/* if(mData.GetAAOpenStatus() == 0)
				{
					SendAndroidautoData(APP_APPSERVICE_OPENAA_TYPE,1);
				}
				else
				{
					//下发连接模式
					SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_LOAD,1);
					int nACCStatus = -1;
					mData.GetINTData(INT_MOBIL_ACC,nACCStatus);
					LOGDBG("ReOpenServiceCallBack nACCStatus = %d !!", nACCStatus);
					SendAndroidautoData(APP_SERVER_AA_ACCSTATUS,nACCStatus);

					SettingData &mSettingData = SettingData::GetInstance();
					BtLocalInfo_t tBtInfo;
					mSettingData.GetBtLocalInfo(tBtInfo);
					SendAAOFBTName(tBtInfo.BTName);
				} */
			}

		}
		else
		{
			LOGDBG("GetServerBinder(\"com.androidauto_srv\"); == nullptr !!");
		}
	}
	
	bRet = OpenService(WIRELESS_SRV);
	if (!bRet)
	{
		PushReOpenService(WIRELESS_SRV);
	}
	else
	{
		m_WirelessSrv = GetServerBinder(WIRELESS_SRV);
		if (nullptr != m_WirelessSrv.get())
		{
			LOGDBG("GetServerBinder(\"com.wireless_srv\"); Success !!");
			HomeData &mData = HomeData::GetInstance();
			T_STR_SCREEN_LOCK_INFO tScreen;
			mData.GetScreenLockInfo(tScreen);
			if(tScreen.nScreenLockStatus == 1)
			{
				SendWirelessInfoData(WIRELESS_CMD_SET_CARPLAY_ACTIVE_STATE,1);
				SendWirelessInfoData(WIRELESS_CMD_SET_ACC_MODE,1);
			}
			else
			{
				ReadWirelessData(WIRELESS_CMD_GET_BTNAME);
				SendWirelessOFWifiMemoryLinkMode();
				int nACCStatus = -1;
				mData.GetINTData(INT_MOBIL_ACC,nACCStatus);
				LOGDBG("ReOpenServiceCallBack nACCStatus = %d !!", nACCStatus);
				SendWirelessInfoData(WIRELESS_CMD_SET_ACC_MODE,1 - nACCStatus);
				SendWirelessOFBTMemoryLinkMode();
			}
		}
	}

	bRet = OpenService(CANBOX_SRV);
	if (!bRet)
	{
		PushReOpenService(CANBOX_SRV);
	}
	else
	{
		m_CanBoxSrv = GetServerBinder(CANBOX_SRV);
		if (nullptr != m_CanBoxSrv.get())
		{
			//下发车型选择
			//SendCanBox_CarSelect(0x24);
		}
	}
	// PushReOpenService(CARPLAY_SRV);
	/*bool bRet = OpenService(FY_REVESER_SRV);
	if (!bRet)
	{
		LOGDBG("OpenService(\"com.reverse_srv\"); faild !!");
		PushReOpenService(FY_REVESER_SRV);
	}
	else
	{
		ReadReverseData(REVERSE_CMD_CAMERA);
		LOGDBG("OpenService(\"com.reverse_srv\"); Success !!");
		m_Reversesrv = GetServerBinder(FY_REVESER_SRV);
		if (nullptr != m_Reversesrv.get())
		{
			LOGDBG("GetServerBinder(\"com.reverse_srv\"); Success !!");
			HomeData &mData = HomeData::GetInstance();
			int nACCStatus = -1;
			mData.GetINTData(INT_MOBIL_ACC,nACCStatus);
			LOGDBG("ReOpenServiceCallBack nACCStatus = %d !!", nACCStatus);
			SendReverseData(APP_SERVER_REVESER_ACCSTATUS,nACCStatus);
		}
		else
		{
			LOGDBG("GetServerBinder(\"com.reverse_srv\"); == nullptr !!");
		}
	}

	bRet = m_HomeDataPacket.Init();
	if(!bRet)
	{
		LOGDBG("HomeDataPacket ThreadIsReady fail");
		SetTimer(TIMERID_DATAPACKET_HOME, TIMER_LONG_2000);
	}
	else
	{
		LOGDBG("HomeDataPacket ThreadIsReady success");
	}

	bRet = m_SettingDataPacket.Init();
	if(!bRet)
	{
		LOGDBG("SettingDataPacket ThreadIsReady fail");
		SetTimer(TIMERID_DATAPACKET_SETTING, TIMER_LONG_2000);
	}
	else
	{
		LOGDBG("SettingDataPacket ThreadIsReady success");
	}

	bRet = m_RadioDataPacket.Init();
	if(!bRet)
	{
		LOGDBG("RadioDataPacket ThreadIsReady fail");
		SetTimer(TIMERID_DATAPACKET_RADIO, TIMER_LONG_2000);
	}
	else
	{
		LOGDBG("RadioDataPacket ThreadIsReady success");
	}
	PublicController::Init();
	SendResourceType(RESOERCE_TYPE_NO);

	SetTimer(TIMERID_SET_DATA_MCU, TIMER_LONG_3000);
	//SetTimer(TIMERID_GET_UUID, 1000);

	SaveVendorDisPoseTime();*/

	//auxin mode
	//m_nAuxinObj.changeToParkinMode();

	//open canbox
	can_gpio_set();
	//open bklight
	backlight_gpio_open();

	ProcessState::self()->startThreadPool();
	return true;
}

bool IVIController::Start()
{
	return true;
}

bool IVIController::Stop()
{
	return true;
}

bool IVIController::Close()
{
	m_HomeDataPacket.Close();
	m_SettingDataPacket.Close();
	m_RadioDataPacket.Close();
	return true;
}

void IVIController::can_gpio_set()
{
	int fd;
    fd = gpio_dev_open();
    gpio_init(fd,5,0);
}

bool IVIController::SendCanBox_CarSelect(int nType)
{
	bool bRet = false;
	m_CanBoxSrv = GetServerBinder(CANBOX_SRV);
	if(NULL == m_CanBoxSrv.get())
	{
		LOGERR("m_CanBoxSrv.get() is null!");
		return bRet;
	}
	int nlen = 2;
	int nCmd = CANBOX_CAR_SELECT;
	uint8_t Buf[2]= {0};
	Buf[0] = nType;
	Buf[1] = 0x01;
	int nret = m_CanBoxSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("SendCanBox_CarSelect success! nType=%d", nType);
	}
	else
	{
		LOGDBG("SendCanBox_CarSelect failed");
	}

	return bRet;
}

bool IVIController::Send_CanBox_Data(int nCmd,uint8_t *pbuf,int nLen)
{
	bool bRet = false;
	m_CanBoxSrv = GetServerBinder(CANBOX_SRV);
	if(NULL == m_CanBoxSrv.get())
	{
		LOGERR("m_CanBoxSrv.get() is null!");
		return bRet;
	}

	int nret = m_CanBoxSrv->WriteData(nCmd, pbuf, nLen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("&&&---Send_CanBox_Data success! nCmd=%d", nCmd);
	}
	else
	{
		LOGDBG("&&&---Send_CanBox_Data failed");
	}

	return bRet;
}

bool IVIController::SendCanBox_Air_Control(int nCtrlID,int nParam)
{
	bool bRet = false;
	m_CanBoxSrv = GetServerBinder(CANBOX_SRV);
	if(NULL == m_CanBoxSrv.get())
	{
		LOGERR("m_CanBoxSrv.get() is null!");
		return bRet;
	}
	int nlen = 2;
	int nCmd = CANBOX_AIR_CONTROL;
	uint8_t Buf[2]= {0};
	Buf[0] = nCtrlID;
	Buf[1] = nParam;
	int nret = m_CanBoxSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("SendCanBox_CarSelect success! nCtrlID=%d", nCtrlID);
	}
	else
	{
		LOGDBG("SendCanBox_CarSelect failed");
	}

	return bRet;
}

void IVIController::Bklight_Enable()
{
	if(1 == m_nBklight_En)
	{
		m_nBklight_En = 0;
	}
	else
	{
		m_nBklight_En = 1;
	}
	set_backlight_en(m_nBklight_En);
}

void IVIController::ACC_Bklight(int nACCStatus)
{
	LOGDBG("&&&----nACCStatus:%d",nACCStatus);
	if(1 == nACCStatus)
	{
		m_nBklight_En = 1;
		set_backlight_en(1);
	}
	else if(0 == nACCStatus)
	{
		m_nBklight_En = 0;
		set_backlight_en(0);
	}
}

void IVIController::AVM_StreamOn()
{
	//m_nAuxinObj.streamOn();
}

void IVIController::AVM_StreamOff()
{
	//m_nAuxinObj.streamOff();
}

void IVIController::AVM_InitDispWin()
{
	m_nAuxinObj.InitDispWin();
}

bool IVIController::Dts_SetEQMode(int nEqMode)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = SYSTEMSRV_CMD_DTS_MODE;
	uint8_t Buf[1]= {0};
	Buf[0] = nEqMode;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("&&&---nEqMode=%d", nEqMode);
	}
	else
	{
		LOGDBG("&&&---Dts_SetEQMode--failed");
	}

	return bRet;
}
bool IVIController::Dts_QueEQInfo()
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = SYSTEMSRV_CMD_DTS_QUE;
	uint8_t Buf[1]= {0};
	Buf[0] = 0x0A;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
	}
	else
	{
		LOGDBG("&&&---Dts_QueEQInfo--failed");
	}

	return bRet;
}
bool IVIController::Dts_SetEQListen_Pos(int nListenPos)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = SYSTEMSRV_CMD_DTS_LISTEN;
	uint8_t Buf[1]= {0};
	Buf[0] = nListenPos;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("&&&---nListenPos=%d", nListenPos);
	}
	else
	{
		LOGDBG("&&&---Dts_SetEQListen_Pos--failed");
	}

	return bRet;
}
bool IVIController::Dts_SetEQSurround_Level(int nLevel)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = SYSTEMSRV_CMD_DTS_SURR_LEVEL;
	uint8_t Buf[1]= {0};
	Buf[0] = nLevel;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("&&&---nLevel=%d", nLevel);
	}
	else
	{
		LOGDBG("&&&---Dts_SetEQSurround_Level--failed");
	}

	return bRet;
}
bool IVIController::Dts_SetEQBass_Level(int nLevel)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = SYSTEMSRV_CMD_DTS_DEEP_LEVEL;
	uint8_t Buf[1]= {0};
	Buf[0] = nLevel;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("&&&---nLevel=%d", nLevel);
	}
	else
	{
		LOGDBG("&&&---Dts_SetEQBass_Level--failed");
	}

	return bRet;
}

bool IVIController::Dts_SetEvelates_Level(int nLevel)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = SYSTEMSRV_CMD_DTS_EVEL_LEVEL;
	uint8_t Buf[1]= {0};
	Buf[0] = nLevel;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("&&&---nLevel=%d", nLevel);
	}
	else
	{
		LOGDBG("&&&---Dts_SetEvelates_Level--failed");
	}

	return bRet;
}
void IVIController::OnTimer(uint32_t id)
{
	static bool bFlag = true;
	if(bFlag)
	{
		bFlag = false;
		int ret = pthread_setname_np(pthread_self(), "ControlTimer");
		if( 0 != ret )
		{
			bFlag = true;
			LOGDBG("Controller Timer thread name fail!");
		}
	}
	static int nHomeDataPacketInitNum = 0;
	static int nSettingDataPacketInitNum = 0;
	static int nRadioDataPacketInitNum = 0;
	switch(id)
	{
		case TIMERID_GET_UUID:
		{
			LOGDBG("&&&&----TIMERID_GET_UUID---\n");
			HomeData &mHomeData = HomeData::GetInstance();
			if(mHomeData.SaveVendorUUID())
			{
				KillTimer(TIMERID_GET_UUID);
			}
		}
		break;
		case TIMERID_DATAPACKET_HOME:
		{
			bool bRet = m_HomeDataPacket.Init();
			if(bRet)
			{
				LOGDBG("HomeDataPacket ThreadIsReady success");
				KillTimer(id);
				nHomeDataPacketInitNum = 0;
			}
			else
			{
				if(nHomeDataPacketInitNum == 1)
				{
					nHomeDataPacketInitNum = 0;
					system("pkill -9 IVIHomeApp");
				}
				nHomeDataPacketInitNum ++;
				LOGDBG("HomeDataPacket ThreadIsReady fail");
			}
		}
		break;
		case TIMERID_DATAPACKET_SETTING:
		{
			bool bRet = m_SettingDataPacket.Init();
			if(bRet)
			{
				LOGDBG("SettingDataPacket ThreadIsReady success");
				KillTimer(id);
				nSettingDataPacketInitNum = 0;
			}
			else
			{
				if(nSettingDataPacketInitNum == 1)
				{
					nSettingDataPacketInitNum = 0;
					system("pkill -9 IVIHomeApp");
				}
				nSettingDataPacketInitNum ++;
				LOGDBG("SettingDataPacket ThreadIsReady fail");
			}
		}
		break;
		case TIMERID_DATAPACKET_RADIO:
		{
			bool bRet = m_RadioDataPacket.Init();
			if(bRet)
			{
				LOGDBG("RadioDataPacket ThreadIsReady success");
				KillTimer(id);
				nRadioDataPacketInitNum = 0;
			}
			else
			{
				if(nRadioDataPacketInitNum == 1)
				{
					nRadioDataPacketInitNum = 0;
					system("pkill -9 IVIHomeApp");
				}
				nRadioDataPacketInitNum ++;
				LOGDBG("RadioDataPacket ThreadIsReady fail");
			}
		}
		break;
		case TIMERID_SET_DATA_MCU:
		{
			KillTimer(TIMERID_SET_DATA_MCU);
			m_bSetBGMode = true;
			LOGDBG("TIMERID_SET_DATA_MCU: m_bSetBGMode = true");
		}
		break;
		case TIMERID_GET_DISPOSE:
		{
			LOGDBG("&&&&----TIMERID_GET_DISPOSE---\n");
			static int nNum = 0;
			HomeData &mHomeData = HomeData::GetInstance();
			if((mHomeData.SaveVendorDisPose() && mHomeData.GetVendorDataAA() ) || nNum == 50)
			{
				nNum = 0;
				KillTimer(TIMERID_GET_DISPOSE);
				T_STR_SCREEN_LOCK_INFO tScreen;
				mHomeData.GetScreenLockInfo(tScreen);
				if(tScreen.nScreenLockStatus == 1)
				{
					SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_RELEASE,1);
				}
				else
				{
					if(mHomeData.GetAAOpenStatus() == 0)
					{
						SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_RELEASE,1);
					}
					else
					{
						//下发连接模式
						SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_LOAD,1);
					}
				}
			}
			else
			{
				nNum++;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

bool IVIController::ReadSytemSerData(int nCmd)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	uint8_t Buf[MAX_DATA_LEN];
	int nlen = 0;;
	int nRet = m_SystemSrv->ReadData(nCmd, Buf, nlen);
	if(nlen > 0)
	{
		HomeData &mHomeData = HomeData::GetInstance();
		SettingData &mData = SettingData::GetInstance();
		switch(nCmd)
		{
			case SYSTEMSRV_CMD_BOOTSIGNAL:
			{
				LOGDBG("SaveACCStatusData:nValue=%d",Buf[2]);
				SendAndroidautoData(APP_SERVER_AA_ACCSTATUS,Buf[2]);
				if(Buf[2] != 1)
				{
					mHomeData.SaveINTData(INT_ACCSTATUS,0);
					SendCarPlayData(APP_APPSERVICE_OPENCARPLAY_TYPE,0);
					SendAndroidautoData(APP_APPSERVICE_OPENAA_TYPE,0);
					SendMobileData(APP_APPSERVICE_OPENYILIAN_TYPE,0);
					SendWirelessInfoData(WIRELESS_CMD_SET_ACC_MODE,1);	
					//下发收音机静音
					SendMeterInfoData(RADIO_MUTE,0,0,0);					
				}
				else
				{
					SendWirelessOFBTMemoryLinkMode();
				}
				mHomeData.SaveINTData(INT_MOBIL_ACC,Buf[2]);
				SendCarPlayData(APP_SERVER_CP_ACCSTATUS,Buf[2]);
				SendMobileData(APP_MOBILENET_ACC_STATUS,Buf[2]);
				SendReverseData(APP_SERVER_REVESER_ACCSTATUS,Buf[2]);	
				break;
			}
			case SYSTEMSRV_CMD_ANIM_END_IVIHOME:
			{
				LOGDBG("SaveACCStatusData:nValue=%d",Buf[0]);
				if(Buf[0] == 1)
				{
					mData.SaveINTData(INT_ACCSTATUS,UIID_WNDID_MAIN);
				}
				break;
			}
			case SYSTEMSRV_CMD_CONNECT_MODE:
			{
				LOGDBG("ConnectMode: nMode=%d",Buf[0]);
				//2023-3-7 屏蔽互联设置
				//mHomeData.SaveINTData(INT_MODILECONNECTMODE,Buf[0]);
				break;
			}
			case SYSTEMSRV_CMD_LANGUAGE_SET:
			{
				LOGDBG("jgwen--- SaveINTData:nLanguage=%d\n",Buf[0]);
				int nLanguage = Buf[0];
				mData.SaveINTData(INT_LANGUAGE, nLanguage);
				break;
			}
			case SYSTEMSRV_CMD_SOURCE_MEMORY:
			{
				LOGDBG("Source Memory: nMode=%d",Buf[0]);
				mHomeData.SaveINTData(INT_MCU_SOURCE_MEMORY,Buf[0]);
				break;
			}
			case SYSTEMSRV_CMD_BT_MEMORY:
			{
				LOGDBG("BTLink Memory: nMode=%d",Buf[0]);
				mHomeData.SaveINTData(INT_MCU_BTLINK_MEMORY,Buf[0]);
				break;
			}
			case SYSTEMSRV_CMD_RADIO_STAUS:
			{
				int nRadioPlayStatus = Buf[0];
				LOGDBG("memory Radio Play Status = %d", nRadioPlayStatus);
				mHomeData.SaveINTData(INT_RADIO_PLAY_STATUS, nRadioPlayStatus);
				break;
			}
			case SYSTEMSRV_CMD_BTMUSIC_STAUS:
			{
				int nBTMusicPlayStatus = Buf[0];
				LOGDBG("memory BTMusic Play Status = %d", nBTMusicPlayStatus);
				mData.SaveINTData(INT_BTMUSIC_PLAY_STATUS, nBTMusicPlayStatus);
				break;
			}
			case SYSTEMSRV_CMD_LOCKSCREEN_STATUS:
			{
				T_STR_SCREEN_LOCK_INFO tScreen;
				tScreen.nScreenLocknum = Buf[0];
				tScreen.nScreenLockStatus = Buf[1];
				mHomeData.SaveScreenLockInfo(tScreen);
				LOGDBG("nScreenLocknum=%d, nScreenLockStatus=%d",tScreen.nScreenLocknum, tScreen.nScreenLockStatus);
				break;
			}
			default:
			{
				break;
			}
		}
		bRet = true;
	}

	return bRet;
}


bool IVIController::ReadWirelessData(int nCmd)
{
	bool bRet = false;
	m_WirelessSrv = GetServerBinder(WIRELESS_SRV);
	if(NULL == m_WirelessSrv.get())
	{
		LOGERR("m_WirelessSrv.get() is null!");
		return bRet;
	}

	LOGDBG("jgwen------ ReadWirelessData =0x%2x", nCmd);

	uint8_t Buf[MAX_DATA_LEN];
	int nlen = 0;
	int nRet = m_WirelessSrv->ReadData(nCmd, Buf, nlen);
	if(nlen > 0)
	{
		SettingData &mSettingData = SettingData::GetInstance();
		HomeData &mHomeData = HomeData::GetInstance();
		BTData &mBTPhoneData = BTData::GetInstance();
		switch(nCmd)
		{
			case WIRELESS_CMD_GET_BTNAME://0x5
			{
				mSettingData.SaveBTName((char*)Buf);
				LOGDBG("jgwen------ BT Name =%s", (char*)Buf);
				break;
			}
			case WIRELESS_CMD_GET_WIFI_INFO://0x23
			{
				T_STR_WIFI_INFO *tWifiInfo = (T_STR_WIFI_INFO*)Buf;
				if(NULL != tWifiInfo)
				{
					mHomeData.SaveWifiInfo(tWifiInfo);
					LOGDBG("Name=%s, ifName=%s, password=%s, hwMode=%s, security=%s, macAddr=%s, channel=%d, mode=%d, freq=%d",\
							tWifiInfo->Name, tWifiInfo->ifName, tWifiInfo->password, tWifiInfo->hwMode, tWifiInfo->security,\
							tWifiInfo->macAddr, tWifiInfo->channel, tWifiInfo->mode, tWifiInfo->freq);
					SetWifiInfoToMobile(nlen, Buf);
				}
				break;
			}
			case WIRELESS_CMD_GET_BTMUSIC_INFO://获取蓝牙音乐信息
			{
				BtMusicInfo_t *tBTMusic = (BtMusicInfo_t*)Buf;
				if(NULL != tBTMusic)
				{
					//mBTPhoneData.SaveBTMusicInfo(tBTMusic);
					LOGDBG("dCurrentTrack=%d, dTotalTracks=%d ,abAlbum=%s ,dPlayingTime=%d ,abMusicTitle=%s ,abArtist=%d ,abGenre=%s",\
							tBTMusic->dCurrentTrack, tBTMusic->dTotalTracks, tBTMusic->abAlbum, tBTMusic->dPlayingTime,\
							tBTMusic->abTitle, tBTMusic->abArtist, tBTMusic->abGenre);
				}
				break;
			}
			case WIRELESS_CMD_GET_BT_STATE://蓝牙开关状态
			{
				uint8_t *nBTStatus = (uint8_t*)Buf;
				if(NULL != nBTStatus)
				{
					mBTPhoneData.SaveBtPairISFrist(nBTStatus);
					LOGDBG("-----xsj-----nBTStatus=%d", nBTStatus);
				}
				break;
			}
			case WIRELESS_CMD_GET_BTCALL_INFO://获取通话信息
			{
				T_STR_BT_CALLERINFO *BTCallInfo = (T_STR_BT_CALLERINFO*)Buf;
				if(NULL != BTCallInfo)
				{
					mBTPhoneData.SaveBTPhoneInfo(BTCallInfo);
					LOGDBG("-----xsj-----addr=%s ,PhoneNum=%s ,Name=%s ,FName=%s ,nState=%d",
							BTCallInfo->addr, BTCallInfo->PhoneNum ,BTCallInfo->Name ,
							BTCallInfo->FName ,BTCallInfo->nState);
				}
				break;
			}
			case WIRELESS_CMD_GET_DEVICE_STATE://获取设备连接状态
			{
				int nBTConnectSts = Buf[0];
				mBTPhoneData.SaveBtConnectStatus(nBTConnectSts);
				LOGDBG("-----xsj-----nBTConnectSts=%d", nBTConnectSts);
			}
			/*
			case WIRELESS_CMD_GET_AUDIO_STATE://获取蓝牙音乐播放状态
			{
				BTPassThroughCommand *nBTMusicStatus = (BTPassThroughCommand*)Buf;
				if(NULL != nBTMusicStatus)
				{
					mBTPhoneData.SaveBTMusicStatus(nBTMusicStatus);
					LOGDBG("-----xsj-----nBTMusicStatus=%d", nBTMusicStatus);

				}
				break;
			}
			*/
			case WIRELESS_CMD_GET_BT_MAC://获取设备列表信息
			{
				break;
			}			
			default:
			{
				break;
			}
		}
		bRet = true;
	}

	return bRet;
}

bool IVIController::SetWifiInfoToMobile(int nlen, uint8_t *pbuf)
{
	bool bRet = false;

	HomeData &mHomeData = HomeData::GetInstance();
	int nMode = 0;
	//2023-3-7 屏蔽互联设置
	//mHomeData.GetINTData(INT_MODILECONNECTMODE, nMode);
	if(!nMode)
	{
		return bRet;
	}

	m_mobilenetsrv = GetServerBinder(MOBILENET_SRV);
	if(NULL == m_mobilenetsrv.get())
	{
		LOGERR("m_mobilenetsrv.get() is null!");
		return bRet;
	}
	int nCmd = APP_MOBILENET_WIFI_INFO;

	int nret = m_mobilenetsrv->WriteData(nCmd, pbuf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("SetWifiInfoToMobile success! ");
	}
	else
	{
		LOGDBG("SetWifiInfoToMobile failed");
	}

	return bRet;
}

bool IVIController::SetWifiStatusToMobileNet(const int nValue)
{
	bool bRet = false;
	m_mobilenetsrv = GetServerBinder(MOBILENET_SRV);
	if(NULL == m_mobilenetsrv.get())
	{
		LOGERR("m_mobilenetsrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = APP_MOBILENET_WIFI_STATUS;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_mobilenetsrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("SetWifiStatusToMobileNet success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("SetWifiStatusToMobileNet failed");
	}

	return bRet;
}

int IVIController::OnCallbackFunc(const std::string nName, int ncmd, int nParam1, int nParam2, int nLen,const uint8_t *pbuf)
{
	LOGDBG("OnCallbackFunc %s--%d--%d",nName.c_str(),ncmd,nLen);
	if((NULL == pbuf) || (nLen <= 0))
	{
		LOGERR("(NULL == pbuf) || (nLen <= 0)");
		return RETURN_ERROR;
	}

	uint8_t buf[nLen] = {0};
	memcpy(buf, pbuf, nLen);

	DealWithCMD(nName, ncmd, nParam1, nParam2, nLen,buf);
	return 0;
}

int IVIController::OnReadData(int ncmd, uint8_t *pbuf, int &nlen)
{
	return 0;
}

int IVIController::OnWriteData(int ncmd, const uint8_t *pbuf, int nlen)
{
	return 0;
}

int IVIController::ReOpenServiceCallBack(const char *nsrv,const int Status)
{
	if(Status == 0)
	{
		return 0;
	}

	if(strcmp(nsrv,METERINFO_SRV) == 0)
	{
		m_MeterInfoSrv = GetServerBinder(METERINFO_SRV);
		if (nullptr != m_MeterInfoSrv.get())
		{
			ReadMeterInfoData(METERINFO_APP_REPORT_MESSAGE);
			SendMeterInfoDataOFStartingUP(APP_METERINFO_RADIO_COMMAND);
			//SendMeterInfoData(RADIO_MUTE,0,0,0);
		}
		else
		{
			LOGDBG("GetServerBinder(\"m_MeterInfoSrv\"); == nullptr !!");
		}
	}

	if(strcmp(nsrv,SYSTEM_SRV) == 0)
	{
		m_SystemSrv = GetServerBinder(SYSTEM_SRV);
		if (nullptr != m_SystemSrv.get())
		{
			ReadSytemSerData(SYSTEMSRV_CMD_BT_MEMORY);
			ReadSytemSerData(SYSTEMSRV_CMD_BOOTSIGNAL);
			ReadSytemSerData(SYSTEMSRV_CMD_ANIM_END_IVIHOME);
			ReadSytemSerData(SYSTEMSRV_CMD_CONNECT_MODE);
			ReadSytemSerData(SYSTEMSRV_CMD_LANGUAGE_SET);
			ReadSytemSerData(SYSTEMSRV_CMD_SOURCE_MEMORY);
			ReadSytemSerData(SYSTEMSRV_CMD_RADIO_STAUS);
			ReadSytemSerData(SYSTEMSRV_CMD_BTMUSIC_STAUS);
			ReadSytemSerData(SYSTEMSRV_CMD_LOCKSCREEN_STATUS);	
		}
		else
		{
			LOGDBG("GetServerBinder(\"m_SystemSrv\"); == nullptr !!");
		}
	}

	if(strcmp(nsrv, WIRELESS_SRV) == 0)
	{
		m_WirelessSrv = GetServerBinder(WIRELESS_SRV);
		if (nullptr != m_WirelessSrv.get())
		{
			LOGDBG("GetServerBinder(\"m_WirelessSrv\"); success !!");
			HomeData &mData = HomeData::GetInstance();
			T_STR_SCREEN_LOCK_INFO tScreen;
			mData.GetScreenLockInfo(tScreen);
			if(tScreen.nScreenLockStatus == 1)
			{
				SendWirelessInfoData(WIRELESS_CMD_SET_CARPLAY_ACTIVE_STATE,1);
				SendWirelessInfoData(WIRELESS_CMD_SET_ACC_MODE,1);
			}
			else
			{
				ReadWirelessData(WIRELESS_CMD_GET_BTNAME);
				SendWirelessOFWifiMemoryLinkMode();
				int nACCStatus = -1;
				mData.GetINTData(INT_MOBIL_ACC,nACCStatus);
				LOGDBG("ReOpenServiceCallBack nACCStatus = %d !!", nACCStatus);
				SendWirelessInfoData(WIRELESS_CMD_SET_ACC_MODE,1 - nACCStatus);

				int nType = mData.GetResourceType();
				LOGDBG("ReOpenServiceCallBack nType = %d",nType);
				SendWirelessInfoData(APP_SERVER_RESOURCE_TPYE,nType);

				SendWirelessOFBTMemoryLinkMode();

				int nAAConnectSatus = 0;
				mData.GetINTData(INT_ANDRIODCONNECTSTATUS, nAAConnectSatus);
				SendWirelessInfoData(WIRELESS_CMD_SET_AA_ACTIVE_STATE,nAAConnectSatus);
			
				int nCPConnectSatus = 0;
				mData.GetINTData(INT_CARPLAYCONNECTSTATUS,nCPConnectSatus);
				SendWirelessInfoData(WIRELESS_CMD_SET_CARPLAY_ACTIVE_STATE,nCPConnectSatus);
			}		
		}
		else
		{
			LOGDBG("GetServerBinder(\"m_WirelessSrv\"); == nullptr !!");
		}
	}

	if(strcmp(nsrv, CARPLAY_SRV) == 0)
	{
		m_carplaysrv = GetServerBinder(CARPLAY_SRV);
		if (nullptr != m_carplaysrv.get())
		{
			LOGDBG("GetServerBinder(\"m_carplaysrv\"); success !!");
			HomeData &mData = HomeData::GetInstance();
			T_STR_SCREEN_LOCK_INFO tScreen;
			mData.GetScreenLockInfo(tScreen);
			if(tScreen.nScreenLockStatus == 1)
			{
				SendCarPlayData(APP_SERVER_MOBILE_RESOURCE_RELEASE,1);
			}
			else
			{
				//下发连接模式
				SendConnectMode(1);
				int nACCStatus = -1;
				mData.GetINTData(INT_MOBIL_ACC,nACCStatus);
				LOGDBG("ReOpenServiceCallBack nACCStatus = %d !!", nACCStatus);
				SendCarPlayData(APP_SERVER_CP_ACCSTATUS,nACCStatus);

				int nAAConnectSatus = 0;
				mData.GetINTData(INT_ANDRIODCONNECTSTATUS, nAAConnectSatus);
				SendCPOFAAConnectStatus(nAAConnectSatus);
			}

		}
		else
		{
			LOGDBG("GetServerBinder(\"m_carplaysrv\"); == nullptr !!");
		}
	}
//2023-3-7 屏蔽互联设置
/*
	if(strcmp(nsrv, MOBILENET_SRV) == 0)
	{
		m_mobilenetsrv = GetServerBinder(MOBILENET_SRV);
		if (nullptr != m_mobilenetsrv.get())
		{
			LOGDBG("GetServerBinder(\"com.mobilenet_srv\"); Success !!");
			//下发连接模式
			SendConnectMode(2);
			HomeData &mData = HomeData::GetInstance();
			int nACCStatus = -1;
			mData.GetINTData(INT_MOBIL_ACC,nACCStatus);
			LOGDBG("ReOpenServiceCallBack nACCStatus = %d !!", nACCStatus);
			SendMobileData(APP_MOBILENET_ACC_STATUS,nACCStatus);
		}
		else
		{
			LOGDBG("GetServerBinder(\"com.mobilenet_srv\"); == nullptr !!");
		}
	}
*/
//2023-7-5 AA屏蔽

	if(strcmp(nsrv, ANDROIDAUTO_SRV) == 0)
	{
		m_Androidautosrv = GetServerBinder(ANDROIDAUTO_SRV);
		if (nullptr != m_Androidautosrv.get())
		{
			LOGDBG("GetServerBinder(\"com.androidauto_srv\"); Success !!");

			HomeData &mData = HomeData::GetInstance();
			T_STR_SCREEN_LOCK_INFO tScreen;
			mData.GetScreenLockInfo(tScreen);
			if(tScreen.nScreenLockStatus == 1)
			{
				SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_RELEASE,1);
			}
			else
			{	
				//1.加载基础资源
				SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_LOAD,1);

				//2.获取并同步ACC状态（车辆核心状态）
				int nACCStatus = -1;
				mData.GetINTData(INT_MOBIL_ACC,nACCStatus);
				LOGDBG("ReOpenServiceCallBack nACCStatus = %d !!", nACCStatus);
				SendAndroidautoData(APP_SERVER_AA_ACCSTATUS,nACCStatus);

				//3.获取并同步蓝牙名称
				SettingData &mSettingData = SettingData::GetInstance();
				BtLocalInfo_t tBtInfo;
				mSettingData.GetBtLocalInfo(tBtInfo);
				SendAAOFBTName(tBtInfo.BTName);

				//4.AA启动类型
				SendAndroidautoData(APP_APPSERVICE_OPENAA_TYPE,1);

				/* if(mData.GetAAOpenStatus() == 0)
				{
					SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_RELEASE,1);
				}
				else
				{
					//下发连接模式
					SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_LOAD,1);
					int nACCStatus = -1;
					mData.GetINTData(INT_MOBIL_ACC,nACCStatus);
					LOGDBG("ReOpenServiceCallBack nACCStatus = %d !!", nACCStatus);
					SendAndroidautoData(APP_SERVER_AA_ACCSTATUS,nACCStatus);

					SettingData &mSettingData = SettingData::GetInstance();
					BtLocalInfo_t tBtInfo;
					mSettingData.GetBtLocalInfo(tBtInfo);
					SendAAOFBTName(tBtInfo.BTName);
				} */
			}
		}
		else
		{
			LOGDBG("GetServerBinder(\"com.androidauto_srv\"); == nullptr !!");
		}
	}

	if(strcmp(nsrv, FY_REVESER_SRV) == 0)
	{
		m_Reversesrv = GetServerBinder(FY_REVESER_SRV);
		if (nullptr != m_Reversesrv.get())
		{
			ReadReverseData(REVERSE_CMD_CAMERA);
			LOGDBG("GetServerBinder(\"com.reverse_srv\"); Success !!");
			HomeData &mData = HomeData::GetInstance();
			int nACCStatus = -1;
			mData.GetINTData(INT_MOBIL_ACC,nACCStatus);
			LOGDBG("ReOpenServiceCallBack nACCStatus = %d !!", nACCStatus);
			SendReverseData(APP_SERVER_REVESER_ACCSTATUS,nACCStatus);
		}
		else
		{
			LOGDBG("GetServerBinder(\"com.reverse_srv\"); == nullptr !!");
		}
	}
	return 0;
}

void IVIController::OnBinderDeath(const std::string nName)
{
	LOGDBG("nName = %s",nName);
}

void IVIController::InitCMDVec()
{
	T_STR_DATA_CMD tCMD;
	for(auto &i : SC_CMD_ConfigList)
	{
		tCMD.nSrvID = i.nSrvID;
		tCMD.nModelId = i.nModelId;
		tCMD.nCmd = i.nCmd;
		m_CMDVec.push_back(tCMD);
	}	
}

int IVIController::DealWithCMD(const std::string nName,int ncmd,int nParam1,int nParam2,int nLen,uint8_t *pbuf)
{
	T_STR_DATA_CMD tCMD;	
	HomeData &mHomeData = HomeData::GetInstance();
	SettingData &mSettingData = SettingData::GetInstance();
	RadioData &mRadioData = RadioData::GetInstance();
	MediaData &mMediaData = MediaData::GetInstance();
	BTData &mBTPhoneData = BTData::GetInstance();



	for(auto it = m_CMDVec.begin(); it != m_CMDVec.end(); it++)
	{ 
		tCMD = *it;
		//LOGDBG("tCMD.nCmd = %d",tCMD.nCmd);
		if((ncmd == tCMD.nCmd) && \
		(ReturnSrvNameID(nName) == tCMD.nSrvID))
		{
			//插包
			T_STR_DATA_PACKET tPacket(tCMD.nSrvID, ncmd, nParam1, nParam2, nLen, pbuf);
			switch (tCMD.nModelId)
			{
				case MODEL_HOME:
				{
					Lock(true);
					mHomeData.InsertPacket(tPacket);
					Lock(false);
					break;
				}
				case MODEL_SETTING:
				{
					Lock(true);
					mSettingData.InsertPacket(tPacket);
					Lock(false);
					break;
				}
				case MODEL_RADIO:
				{
					Lock(true);
					mRadioData.InsertPacket(tPacket);
					Lock(false);
					break;
				}
				case MODEL_APP:
				{
					break;
				}
				case MODEL_PHONE:
				{
					Lock(true);
					mBTPhoneData.InsertPacket(tPacket);
					Lock(false);
					break;
				}
				case MODEL_MEDIA:
				{
					Lock(true);
					mMediaData.InsertPacket(tPacket);
					Lock(false);					
					break;
				}
				default:
					break;
			}
		}
	}
	return 0;
}

int IVIController::ReturnSrvNameID(const std::string nName)
{
	int nSrvID = -1;
	if(strcmp(nName.c_str(), SYSTEM_SRV) == 0)
	{
		nSrvID = SRV_ID_SYSTEM_BINDER;
	}
	else if(strcmp(nName.c_str(), APPMANAGER_SRV) == 0)
	{
		nSrvID = SRV_ID_APPMANAGER_BINDER;
	}
	else if(strcmp(nName.c_str(), METERINFO_SRV) == 0)
	{
		nSrvID = SRV_ID_METERINFO_BINDER;
	}
	else if(strcmp(nName.c_str(), WIRELESS_SRV) == 0)
	{
		nSrvID = SRV_ID_WIRELESSSER_BINDER;
	}
	else if(strcmp(nName.c_str(), MOBILENET_SRV) == 0)
	{
		nSrvID = SRV_ID_MOBILENET_BINDER;
	}
	else if(strcmp(nName.c_str(), CARPLAY_SRV) == 0)
	{
		nSrvID = SRV_ID_CARPLAY_BINDER;
	}
	else if(strcmp(nName.c_str(), ANDROIDAUTO_SRV) == 0)
	{
		nSrvID = SRV_ID_ANDROID_BINDER;
	}
	else if(strcmp(nName.c_str(), FY_REVESER_SRV) == 0)
	{
		nSrvID = SRV_ID_REVESER_BINDER;
	}
	else if(strcmp(nName.c_str(), MEDIA_SRV) == 0)
	{
		nSrvID = SRV_ID_MEDIA_BINDER;
	}
	else if(strcmp(nName.c_str(), CANBOX_SRV) == 0)
	{
		nSrvID = SRV_ID_CANBOX_BINDER;
	}
	//LOGDBG("nSrvID = %d",nSrvID);
	return nSrvID;
}

bool IVIController::SetMedioVolume(int nValue,int nType)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 2;
	int nCmd = SETAPP_SYSTEM_SET_VOLUME;
	uint8_t Buf[2]= {0};
	Buf[0] = nValue;
	Buf[1] = nType;   //0:media 1:radio
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- set medio Volume success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("jgwen--- set medio Volume failed");
	}

	return bRet;
}
bool IVIController::SetBtVolume(int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);	
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = SETAPP_SYSTEM_SET_BT_VOLUME;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- set Bt Volume success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("jgwen--- set Bt Volume failed");
	}

	return bRet;
}
bool IVIController::SetMute(int nValue, int mediovol, int btvol)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 3;
	int nCmd = SETAPP_SYSTEM_SET_MUTE;
	uint8_t Buf[3]= {0};
	Buf[0] = nValue;
	Buf[1] = mediovol;
	Buf[2] = btvol;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- set Mute success! nValue=%d, mediovol=%d, btvol=%d", nValue, mediovol, btvol);
	}
	else
	{
		LOGDBG("jgwen--- set Mute failed");
	}

	return bRet;
}
bool IVIController::SetKeyMute(int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = SETAPP_SYSTEM_SET_KEY_MUTE;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- set Key Mute success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("jgwen--- set Key Mute failed");
	}

	return bRet;
}

bool IVIController::SetBackLight(int nValue)
{
	LOGDBG("jgwen--- SetBackLight");
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = SETAPP_SYSTEM_SET_BACKLIGHT;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- set back light success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("jgwen--- set back light failed");
	}

	return bRet;	
}

bool IVIController::SetbackLightMode(int Light, bool bAutoBg)
{
	LOGDBG("m_bSetBGMode = %d \n", m_bSetBGMode);
	if(!m_bSetBGMode)
	{
		LOGDBG("Set BG Mode time < 6s");
		return false;
	}
	
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	SettingData &mData = SettingData::GetInstance();
	LOGDBG("jgwen--- SetbackLightMode:: Light=%d, bAutoBg=%d \n", Light, bAutoBg);
	int nlen = 2;
	int nCmd = APP_SYSTEM_SET_BACKGROUND;
	uint8_t Buf[2]= {0};
	if(Light == 1 && bAutoBg == true)  //黑夜模式
	{
		Buf[0] = 2;
		SendBgModeToAA(2);
	}
	else // 白天模式
	{
		Buf[0] = 1;
		SendBgModeToAA(1);
	}

	int nMode = 0;
	mData.GetINTData(INT_BACKGROUND, nMode);
	LOGDBG("jgwen--- GetINTData(INT_BACKGROUND::nMode=%d, Buf[0]=%d \n", nMode, Buf[0]);
	if(nMode != Buf[0])
	{
		if(bAutoBg)
		{
			Buf[1] = 1;
		}
		else
		{
			Buf[1] = 0;
		}
		int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
		if(0 == nret)
		{
			bRet = true;
			LOGDBG("jgwen--- Set backLight Mode success =%d! \n", Buf[0]);
		}
		else
		{
			LOGDBG("jgwen--- Set backLight Mode failed");
		}
	}
	else
	{
		LOGDBG("jgwen--- The backLight Mode same as before \n");
	}
	
	return bRet;	
}

bool IVIController::SendBgModeToAA(int mode)
{
	bool bRet = false;
	m_Androidautosrv = GetServerBinder(ANDROIDAUTO_SRV);
	if(NULL == m_Androidautosrv.get())
	{
		LOGERR("m_Androidautosrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	int nCmd = APP_AA_CMD_BGLIGHT_MODE;  // 2:黑夜  1:白天
	uint8_t Buf[1]= {0};
	Buf[0] = mode;
	int nret = m_Androidautosrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- Send Bg Mode to AA success! mode=%d", mode);
	}
	else
	{
		LOGDBG("jgwen--- Send Bg Mode to AA failed");
	}

	return bRet;	
}

bool IVIController::SetBluetooth(int nValue)
{
	bool bRet = false;
	m_WirelessSrv = GetServerBinder(WIRELESS_SRV);
	if(NULL == m_WirelessSrv.get())
	{
		LOGERR("m_WirelessSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	int nCmd = WIRELESS_CMD_SET_BT_STATE;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_WirelessSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- Set BT success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("jgwen--- Set BT failed");
	}

	return bRet;
}


bool IVIController::SetCarInfo()
{
	return true;
}
bool IVIController::SetClearMile(const int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	int nCmd = SETAPP_SYSTEM_SET_TRIP_MILEAGE;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- clear mileage success! nValue=%d",nValue);
	}
	else
	{
		LOGDBG("jgwen--- clear mileage failed");
	}

	return bRet;
}
bool IVIController::SetUnitMode(const int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	int nCmd = SETAPP_SYSTEM_SET_UNIT;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- set unit success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("jgwen--- set unit failed");
	}

	return bRet;
}

bool IVIController::SetReadLight(const int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	int nCmd = SETAPP_SYSTEM_READ_LIGHT;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- set read light! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("jgwen--- set read light failed");
	}

	return bRet;	
}

bool IVIController::SetWheelType(const int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	int nCmd = SETAPP_SYSTEM_SET_WHEEL;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- set unit success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("jgwen--- set unit failed");
	}

	return bRet;	
}
bool IVIController::SetMaintianMileage(const int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	int nlen = 3;
	int nCmd = SETAPP_SYSTEM_RESET_MAINTIAN;
	uint8_t Buf[3]= {0};
	Buf[0] = 1;
	Buf[1] = (nValue >> 8); // 高八位
	Buf[2] = nValue; // 低八位
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);

	LOGDBG("jgwen---Buf[1]=%d, Buf[2] = %d \n", Buf[1], Buf[2]);
	LOGDBG("jgwen--- MainTian nValue=%d \n", (Buf[1]<<8) | Buf[2]);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen---  set MainTian success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("jgwen---  set MainTian failed");
	}

	return bRet;
}

bool IVIController::SetSystemInfo()
{
	return true;	
}

bool IVIController::SetLanguage(const int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	int nCmd = SETAPP_SYSTEM_SET_LANGUAGE;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen---  Set Language success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("jgwen---  Set Language failed");
	}

	return bRet;
}

bool IVIController::SetSyncTime(const int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	int nCmd = APP_SYSTEM_MCU_SYNC_DATE;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen---  Set Sync Time success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("jgwen---  Set Sync Time failed");
	}

	return bRet;
}

bool IVIController::SetCurTime(T_STR_TIME_INFO ntTime)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	int nlen = 7;
	int nCmd = APP_SYSTEM_SET_TIME;
	uint8_t Buf[nlen]= {0};
	Buf[0] = ntTime.nClockMode;
	Buf[1] = (ntTime.nYear >> 8) & 0xff;
	Buf[2] = ntTime.nYear & 0xff;
	Buf[3] = ntTime.nMonth;
	Buf[4] = ntTime.nDay;
	Buf[5] = ntTime.nHours;
	Buf[6] = ntTime.nMinutes;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- set time success! nHours=%d, nMinutes=%d, nSeconds=%d",ntTime.nHours, ntTime.nMinutes);
	}
	else
	{
		LOGDBG("jgwen--- set time failed");
	}

	return bRet;
}

bool IVIController::ReadMeterInfoData(int nCmd)
{
	bool bRet = false;
	m_MeterInfoSrv = GetServerBinder(METERINFO_SRV);
	if(NULL == m_MeterInfoSrv.get())
	{
		LOGERR("m_UpdateSrv.get() is null!");
		return bRet;
	}
	
	int nlen = 11;
	uint8_t Buf[nlen] = {0};
	if(nlen > 0)
	{
		Buf[10] = 1;
		bRet = true;
	}
	int nret = m_MeterInfoSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG(" send success! Buf=%d",Buf[10]);
	}
	else
	{
		LOGDBG("failed");
	}
	return bRet;
}

bool IVIController::SendMeterInfoData(int nType, int32_t nBand, int32_t nFreq, int32_t nMute,int32_t nLoc)
{
	bool bRet = false;
	
	m_MeterInfoSrv = GetServerBinder(METERINFO_SRV);
	if(NULL == m_MeterInfoSrv.get())
	{
		LOGERR("m_MeterInfoSrv.get() is null!");
		return bRet;
	}

	int nlen = 4;
	int nCmd = APP_METERINFO_RADIO_KEY_COMMAND;
	uint8_t Buf[4]= {0};
	switch(nType)
	{
		case RADIO_BAND_FREQ: //切换波段
		{
			int nband = -1;
			if(nBand == FM)
			{
				nband = AM;
			}
			else if(nBand == AM)
			{
				nband = FM;
			}
			Buf[0] = RADIO_BAND_FREQ;
			Buf[1] = nband;
			Buf[2] = 0;
			Buf[3] = 0;
		}
		break;
		case RADIO_TUNING_UP: //Step +
		{
			Buf[0] = RADIO_TUNING_UP;
			Buf[1] = 0;
			Buf[2] = 0;
			Buf[3] = 0;
		}
		break;
		case RADIO_TUNING_DOWN: //Step -
		{
			Buf[0] = RADIO_TUNING_DOWN;
			Buf[1] = 0;
			Buf[2] = 0;
			Buf[3] = 0;
		}
		break;
		case RADIO_SEEK_UP: //AS +
		{
			Buf[0] = RADIO_SEEK_UP;
			Buf[1] = nBand;
			Buf[2] = 0;
			Buf[3] = 0;
		}
		break;
		case RADIO_SEEK_DOWN: //AS -
		{
			Buf[0] = RADIO_SEEK_DOWN;
			Buf[1] = nBand;
			Buf[2] = 0;
			Buf[3] = 0;
		}
		break;
		case RADIO_TUNE_FREQ:
		{
			Buf[0] = RADIO_TUNE_FREQ;
			Buf[1] = nBand;
			Buf[2] = (nFreq & 0xff00) >> 8;
			Buf[3] = (nFreq & 0xff);
		}
		break;
		case RADIO_AS:
		{
			Buf[0] = RADIO_AS;
			Buf[1] = 1;
			Buf[2] = 0;
			Buf[3] = 0;
		}
		break;		
		case RADIO_BREAK:
		{
			Buf[0] = RADIO_BREAK;
			Buf[1] = 0;
			Buf[2] = 0;
			Buf[3] = 0;
		}
		break;
		case RADIO_MUTE:   // 0不静音 1静音
		{
			Buf[0] = RADIO_MUTE;
			if(nMute == 0)
			{
				Buf[1] = 1;
			}
			else if(nMute == 1)
			{
				Buf[1] = 0;
			}			
			Buf[2] = 0;
			Buf[3] = 0;
			LOGDBG("11111111111111111111111111111111111111111111 = %d",Buf[1]);
		}
		break;
		case RADIO_AREA:
		{
			Buf[0] = RADIO_AREA;
			Buf[1] = nBand;
			Buf[2] = 0;
			Buf[3] = 0;
			LOGDBG("RADIO_AREA! nBand = %d, nFreq = %d", nBand, nFreq);						
		}
		break;
		case RADIO_PTY_SEEK:
		{
			Buf[0] = RADIO_PTY_SEEK;
			Buf[1] = nMute;
			Buf[2] = 0;
			Buf[3] = 0;
			LOGDBG("RADIO_PTY_SEEK! nMute = %d", nMute);				
		}
		break;
		case RADIO_LOC:    //远近程
		{
			Buf[0] = RADIO_LOC;
			Buf[1] = nLoc;
			Buf[2] = 0;
			Buf[3] = 0;
		}
		break;
		default:
			break;
	}
	
	int nret = m_MeterInfoSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("GoTo success! nBand = %d, nFreq = %d", nBand, nFreq);
		if(nType == RADIO_AS)
		{
			RadioData &mRadioData = RadioData::GetInstance();
			mRadioData.SetSearchingstatus(RADIO_SEARSTATUS_START);
		}
		if(nType == RADIO_PTY_SEEK)
		{
			RadioData &mRadioData = RadioData::GetInstance();
			mRadioData.SetSearchingstatus(RADIO_SEARSTATUS_START_PS);
		}
	}
	else
	{
		LOGDBG("GoTo failed!");
	}
    
	return bRet;
}


bool IVIController::SendMeterInfoDataOFRadioRDS(int nType)
{
	bool bRet = false;
	
	m_MeterInfoSrv = GetServerBinder(METERINFO_SRV);
	if(NULL == m_MeterInfoSrv.get())
	{
		LOGERR("m_MeterInfoSrv.get() is null!");
		return bRet;
	}

	T_RADIO_RDS_SWITCH tRDSSwitchStatus; 
	RadioData &mData = RadioData::GetInstance();
	mData.GetRDSSwitch(tRDSSwitchStatus);

	int nlen = 10;
	int nCmd = APP_METERINFO_RDS_PARAM_SETTING;
	uint8_t Buf[nlen]= {tRDSSwitchStatus.REG_Switch,\
						tRDSSwitchStatus.EON_Switch,\
						tRDSSwitchStatus.PTY_Switch,\
						tRDSSwitchStatus.PS_Switch,\
						tRDSSwitchStatus.AF_switch,\
						tRDSSwitchStatus.TA_Switch,\
						tRDSSwitchStatus.RDS_Switch,\
						tRDSSwitchStatus.CT_Switch,\
						tRDSSwitchStatus.RT_Switch,\
						tRDSSwitchStatus.TA_Volume,\
						};
	int nSwitch = 0;

	static int OldAFStatus = 1;
	static int OldTAStatus = 1;
	static int OldCTStatus = 0;

	switch(nType)
	{
		case RDS_SWITCH: //RDS
		{
			if(tRDSSwitchStatus.RDS_Switch == 0)
			{
				nSwitch = 1;
				//根据记忆的来
				Buf[4] = OldAFStatus;
				Buf[5] = OldTAStatus;
				Buf[7] = OldCTStatus;
			}
			else
			{
				if(tRDSSwitchStatus.AF_switch == 1)
				{
					Buf[4] = 0;
				}

				if(tRDSSwitchStatus.TA_Switch == 1)
				{
					Buf[5] = 0;
				}

				if(tRDSSwitchStatus.CT_Switch == 1)
				{
					Buf[7] = 0;
				}
				nSwitch = 0;
			}

			Buf[6] = nSwitch;
		}
		break;
		case AF_SWITCH: //AF
		{
			if(tRDSSwitchStatus.AF_switch == 0)
			{
				nSwitch = 1;
			}
			Buf[4] = nSwitch;
			OldAFStatus = nSwitch;
		}
		break;
		case TA_SWITCH: //TA
		{
			if(tRDSSwitchStatus.TA_Switch == 0)
			{
				nSwitch = 1;
			}
			Buf[5] = nSwitch;
			OldTAStatus = nSwitch;
		}
		break;
		case CT_SWITCH: //CT
		{
			if(tRDSSwitchStatus.CT_Switch == 0)
			{
				nSwitch = 1;
			}
			Buf[7] = nSwitch;
			OldCTStatus = nSwitch;
		}
		break;
		default:
			break;
	}
	
	int nret = m_MeterInfoSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("GoTo success! nType = %d,nSwitch =%d", nType,nSwitch);
	}
	else
	{
		LOGDBG("GoTo failed!");
	}
    
	return bRet;
}

bool IVIController::InitAudio()
{
	HomeData &mData = HomeData::GetInstance();
	int nCurWnd = mData.GetCurrWnd();
	if(nCurWnd == UIID_WNDID_BLACK)
	{
		LOGDBG("&&&----InitAudio----UIID_WNDID_BLACK--\n");
		return true;
	}

	bool ret =  false;
	AudioControl *audioCtrl = AudioControl::getInstance();
	audioCtrl->setAuxinChannel(AudioControl::AudioInChannel2);

	shared_ptr<ResourceHolder> holder = m_ResCtrol.getHolder(ResAudio);

	if(holder == NULL) 
	{
		LOGDBG("&&&---holder fail!\n");
        return false;
    }

	ResourceState state = holder->state();

	/*if(state.isCurrent()) 
	{
		LOGDBG("audio resource,but borrowed.wait for unborrow\n");
		return false;
	}*/
	if(GetAudioTake())
	{
		LOGDBG("&&&----InitAudio----take--true--\n");
		audioCtrl->setOutputSource(AUDIO_TAG_MAIN_AUDIO, AUDIO_SP_SOURCE_AUXIN);
		return true;
	}
	ret = holder->take();

	if(ret)
	{
		LOGDBG("&&&----take--success\n");
		SetAudioTake(true);
		audioCtrl->setOutputSource(AUDIO_TAG_MAIN_AUDIO, AUDIO_SP_SOURCE_AUXIN);
	}
	else
	{
		LOGDBG("&&&----take--fail\n");
	}

	//设置Radio声音等级
	SettingData &SettingData = SettingData::GetInstance();
	int nMedioVolume = 0;
	SettingData.GetINTData(INT_VOLUME_LEVEL, nMedioVolume);

	SetMedioVolume(nMedioVolume,1);
	return ret;
}

bool IVIController::unborrowed()
{
	bool ret =  false;

	shared_ptr<ResourceHolder> holder = m_ResCtrol.getHolder(ResAudio);

	if(holder == NULL) 
	{
		LOGDBG("&&&---holder fail!\n");
        return false;
    }

	ResourceState state = holder->state();

	if(state.isCurrent()) 
	{
		LOGDBG("&&&----current----take\n");
		HomeData &mHomeData = HomeData::GetInstance();
		int nCPPhone = 0;
		mHomeData.GetINTData(INT_CP_PHONE,nCPPhone);

		int nAAPhone = 0;
		mHomeData.GetINTData(INT_AA_PHONE,nAAPhone);
		if(nCPPhone == 0 && nAAPhone == 0)
		{
			EnableAudio(true);
		}
	}

	return ret;
}

int IVIController::getActiveTracksName(vector<string> &tracksName)
{
    int ret = -1;
    audio::SPAudioSetting::Source *setting = NULL;
    setting = audio::SPAudioSetting::createSource();

	const char *trackTag = AUDIO_TAG_MAIN_AUDIO;

	if(NULL != setting)
	{
		ret = setting->getActiveTracksName(trackTag, tracksName);

		if(ret != 0)
		{
			LOGDBG("&&&---getActiveTracksName failed ret:%d",ret);
		}

		audio::SPAudioSetting::destroy(setting);
	}

    return ret;
}

bool IVIController::GetAudioTake()
{
	return m_bAudioTake;
}

void IVIController::SetAudioTake(bool bTake)
{
	m_bAudioTake = bTake;
}

void IVIController::EnableAudio(bool bSet)
{
	AudioControl *audioCtrl = AudioControl::getInstance();
	if(bSet)
	{
		audioCtrl->setOutputSource(AUDIO_TAG_MAIN_AUDIO, AUDIO_SP_SOURCE_AUXIN);
	}
	else
	{
		//audioCtrl->setOutputSource(AUDIO_TAG_MAIN_AUDIO, AUDIO_SOURCE_NONE);
		LOGDBG("NO setOutputSource NOT");
	}
}


bool IVIController::SendBeep(const int nPlay)
{
	bool bRet = false;
	//
	SettingData &mSettingData = SettingData::GetInstance();
	if(!mSettingData.GetKeyVolState())
	{
		LOGDBG("Keyvol Mute");
		return bRet;
	}
	m_MeterInfoSrv = GetServerBinder(METERINFO_SRV);
	if(NULL == m_MeterInfoSrv.get())
	{
		LOGERR("m_MeterInfoSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	int nCmd = APP_METERINFO_KEY_TONE;
	uint8_t Buf[1]= {0};
	Buf[0] = nPlay;
	int nret = m_MeterInfoSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Play Key Beep success! nPlay=%d", nPlay);
	}
	else
	{
		LOGDBG("Play Key Beep failed");
	}
	return bRet;
}

bool IVIController::SendCallInfoData(int nCmd, unsigned char *phone_number, int len)
{
	bool bRet = false;
	int nret = false;		
	
	m_WirelessSrv = GetServerBinder(WIRELESS_SRV);
	if(NULL == m_WirelessSrv.get())
	{
		LOGERR("m_WirelessSrv.get() is null!");
		return bRet;
	}
	switch(nCmd)
	{
		case WIRELESS_CMD_SET_DIAL_REQ:
		{			
			nret = m_WirelessSrv->WriteData(nCmd, phone_number, len);
			LOGDBG("%s, dail phone_number:%s, len=%d, nret=%d", __func__, phone_number, len, nret);
			break;
		}
	}
	
	return 0;
}

bool IVIController::SendWirelessPairInfo(int nCmd,int nType,int nIndex,BtAddr MacAddr)
{
	bool bRet = false;
	m_WirelessSrv = GetServerBinder(WIRELESS_SRV);
	if(NULL == m_WirelessSrv.get())
	{
		LOGERR("m_WirelessSrv.get() is null!");
		return bRet;
	}
	int nret = 0;

	switch(nCmd)
	{
		case WIRELESS_CMD_SET_INITIATIVE_PAIRING:
		{
			int nlen = 8;
			uint8_t Buf[nlen]= {0};
			Buf[0] = nIndex;
			Buf[1] = nType;
			//memcpy(Buf + 2, MacAddr, nlen - 2);
			for(int i = 0;i <= 5; i++)
			{
				Buf[i + 2] = (MacAddr.address[i]);
			}
			nret = m_WirelessSrv->WriteData(nCmd, Buf, nlen);
			LOGDBG("Send success! MacAddr=%02x:%02x:%02x:%02x:%02x:%02x",Buf[2],Buf[3],Buf[4],Buf[5],Buf[6],Buf[7]);
		}
		break;
		default:
		break;
	}
	return true;
}

bool IVIController::SendWirelessInfoData(int nCmd,int nType,int nIndex)
{
	bool bRet = false;
	m_WirelessSrv = GetServerBinder(WIRELESS_SRV);
	if(NULL == m_WirelessSrv.get())
	{
		LOGERR("m_WirelessSrv.get() is null!");
		return bRet;
	}
	int nret = 0;

	switch(nCmd)
	{
		case WIRELESS_CMD_SET_AUDIO_STATE:
		{
			int nlen = 1;
			uint8_t Buf[nlen]= {0};
			Buf[0] = nType;
			nret = m_WirelessSrv->WriteData(nCmd, Buf, nlen);
			LOGDBG("Send success! nPlayStatus=%d", Buf[0]);
		}
		break;
		case WIRELESS_CMD_CALL_ACTION://电话接听挂断
		{
			int nlen = 1;
			uint8_t Buf[nlen]= {0};
			Buf[0] = nType;
			nret = m_WirelessSrv->WriteData(nCmd, Buf, nlen);
			LOGDBG("Send success! nPhoneCallStatus=%d", Buf[0]);
		}
		break;
		case WIRELESS_CMD_SET_BTPHONE_MIC_MUTE: //设置麦克风静音
		{
			int nlen = 1;
			uint8_t Buf[nlen]= {0};
			Buf[0] = nType;
			nret = m_WirelessSrv->WriteData(nCmd, Buf, nlen);
			LOGDBG("Send success! nMicroPhoneStatus=%d", Buf[0]);
		}
		break;
		case WIRELESS_CMD_SET_BTPHONE_CALL_MODE: //设置免提
		{
			int nlen = 1;
			uint8_t Buf[nlen]= {0};
			Buf[0] = nType;
			nret = m_WirelessSrv->WriteData(nCmd, Buf, nlen);
			LOGDBG("Send success! nPhoneCallMode=%d", Buf[0]);
		}
		break;
		case WIRELESS_CMD_SET_BT_PAIRING_REQ://配对请求
		{
			int nlen = 1;
			uint8_t Buf[nlen]= {0};
			Buf[0] = nType;
			nret = m_WirelessSrv->WriteData(nCmd, Buf, nlen);
			LOGDBG("Send success! nPlay=%d", Buf[0]);
		}
		break;
		case WIRELESS_CMD_SET_BT_DISCOVER_REQ://搜索蓝牙设备请求
		{
			int nlen = 1;
			uint8_t Buf[nlen]= {0};
			Buf[0] = nType;
			nret = m_WirelessSrv->WriteData(nCmd, Buf, nlen);
			LOGDBG("Send success! nDiscoverReq=%d", Buf[0]);
		}
		break;
		case WIRELESS_CMD_SYNC_CONTACT:
		{
			int nlen = 1;
			uint8_t Buf[nlen]= {0};
			Buf[0] = nType;
			nret = m_WirelessSrv->WriteData(nCmd, Buf, nlen);
			LOGDBG("Send success! nContactReq=%d", Buf[0]);
		}
		break;
		default:
		{
			int nlen = 1;
			uint8_t Buf[1]= {0};
			Buf[0] = nType;
			nret = m_WirelessSrv->WriteData(nCmd, Buf, nlen);
			LOGDBG("Send success! nPlay=%d", Buf[0]);
		}
		break;
	}

	if(0 == nret)
	{
		bRet = true;
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

bool IVIController::SendCarPlayData(int nCmd,int nType)
{
	bool bRet = false;
	m_carplaysrv = GetServerBinder(CARPLAY_SRV);
	if(NULL == m_carplaysrv.get())
	{
		LOGERR("m_carplaysrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = nType;
	int nret = m_carplaysrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

bool IVIController::SendMeterInfoResourceTypeData(int nCmd,int nType)
{
	bool bRet = false;
	m_MeterInfoSrv = GetServerBinder(METERINFO_SRV);
	if(NULL == m_MeterInfoSrv.get())
	{
		LOGERR("m_MeterInfoSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = nType;
	int nret = m_MeterInfoSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}
bool IVIController::SendMeterInfoDataOFStartingUP(int nCmd)
{
	bool bRet = false;
	m_MeterInfoSrv = GetServerBinder(METERINFO_SRV);
	if(NULL == m_MeterInfoSrv.get())
	{
		LOGERR("m_MeterInfoSrv.get() is null!");
		return bRet;
	}

	int nlen = 11;
	uint8_t Buf[11]= {0};
	Buf[0] = 10;
	Buf[1] = 10;
	Buf[2] = (8750 & 0xFF00) >> 8;
	Buf[3] = 8750 & 0x00FF;
	Buf[4] = (10800 & 0xFF00) >> 8;
	Buf[5] = 10800 & 0x00FF;
	Buf[6] = 0;    //FM
	Buf[7] = (8750 & 0xFF00) >> 8;
	Buf[8] = 8750 & 0x00FF;
	Buf[9] = 2;
	Buf[10] = 1;
	int nret = m_MeterInfoSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}
void IVIController::SendResourceType(int nType)
{
	HomeData &mHomeData = HomeData::GetInstance();
	int nOldType = mHomeData.GetResourceType();
	LOGDBG("nOldType = %d, nType = %d",nOldType,nType);
	int nCarbitConnectStatus = 0;
	mHomeData.GetINTData(INT_MODILE_YL_CARBIT, nCarbitConnectStatus);
	LOGDBG("nCarbitConnectStatus is %d",nCarbitConnectStatus);
	if(nCarbitConnectStatus == 4 && nType == RESOERCE_TYPE_BT)
	{
		LOGDBG("YL NO BT");
		nType = RESOERCE_TYPE_YL;
	}
	
	int nStatus = 0;
    mHomeData.GetINTData(INT_ANDRIODCONNECTSTATUS, nStatus);
	LOGDBG("nStatus = %d",nStatus);
	if(nStatus == 1 && nType == RESOERCE_TYPE_BT)
	{
		LOGDBG("AA NO BT");
		nType = RESOERCE_TYPE_AA;
	}

	LOGDBG("nOldType = %d, nType = %d",nOldType,nType);
	if(nType == nOldType)
	{
		return;
	}
	if(nType != RESOERCE_TYPE_RD)
	{
		//下发收音机静音
		SendMeterInfoData(RADIO_MUTE,0,0,0);
	}
	SendWirelessInfoData(APP_SERVER_RESOURCE_TPYE,nType);
	SendCarPlayData(APP_SERVER_RESOURCE_TPYE,nType);
	SendAndroidautoData(APP_SERVER_RESOURCE_TPYE,nType);
	SendMeterInfoResourceTypeData(APP_SERVER_RESOURCE_TPYE,nType);
	mHomeData.SaveResourceType(nType);
}

bool IVIController::SendMobileData(int nCmd,int nType)
{
	bool bRet = false;
	m_mobilenetsrv = GetServerBinder(MOBILENET_SRV);
	if(NULL == m_mobilenetsrv.get())
	{
		LOGERR("m_mobilenetsrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = nType;
	int nret = m_mobilenetsrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

bool IVIController::SendSystemData(int nCmd,int nType)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	if(nCmd == SETAPP_SYSTEM_CONNECT_MODE)
	{
		LOGERR("SETAPP_SYSTEM_CONNECT_MODE!");
		return bRet;
	}
	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = nType;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

bool IVIController::SetSystem_AVMSetting(int nSwitch,int nPointX,int nPointY)
{
	//by zxw
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	int nlen = 5;
	int nCmd = SYSTEMSRV_CMD_MPU_AVM_SETTING;
	uint8_t Buf[5]= {0};
	Buf[0] = nSwitch;
	Buf[1] = (nPointX >> 8) & 0xFF;
	Buf[2] = nPointX & 0xFF;
	Buf[3] = (nPointY >> 8) & 0xFF;;
	Buf[4] = nPointY & 0xFF;

	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("&&&---nSwitch=%d, nPointX=%d, nPointY=%d",nSwitch,nPointX,nPointY);
	}
	else
	{
		LOGDBG("&&&---SetSystem_AVMSetting failed");
	}

	return bRet;
}

void IVIController::SendConnectMode(int nsrv)
{
	HomeData &mData = HomeData::GetInstance();
	int nMode = 0;
	//2023-3-7 屏蔽互联设置
	//mData.GetINTData(INT_MODILECONNECTMODE, nMode);
	LOGDBG("nMode = %d",nMode);
	if(nMode)
	{
		//YL
		/*
		if(nsrv == 1)
		{
			SendCarPlayData(APP_SERVER_MOBILE_RESOURCE_RELEASE,1);
		}
		if(nsrv == 3)
		{
			SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_RELEASE,1);
		}*/
		if(nsrv == 2)
		{
			SendMobileData(APP_SERVER_MOBILE_RESOURCE_LOAD,2);
		}		
	}
	else if(nMode == 0)
	{
		//CP/AA
		if(nsrv == 1)
		{
			SendCarPlayData(APP_SERVER_MOBILE_RESOURCE_LOAD,1);
		}
		if(nsrv == 3)
		{
			SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_LOAD,1);
		}
		/*
		if(nsrv == 2)
		{
			SendMobileData(APP_SERVER_MOBILE_RESOURCE_RELEASE,2);
		}*/
	}
}

bool IVIController::SendAndroidautoData(int nCmd,int nType)
{
	bool bRet = false;
	m_Androidautosrv = GetServerBinder(ANDROIDAUTO_SRV);
	if (nullptr == m_Androidautosrv.get())
	{
		LOGERR("m_Androidautosrv.get() is null!");
		return bRet;		
	}
	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = nType;
	int nret = m_Androidautosrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d %X", Buf[0],nCmd);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

//4-3 收音机播放状态记忆
/*
bool IVIController::SetWifiMemoryLinkMode(const int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	HomeData &mHomeData = HomeData::GetInstance();
	int nWifiStatus = 0;
	mHomeData.GetINTData(INT_WIFI_MEMORY_LINK_STATUS, nWifiStatus);
	LOGDBG("nWifiStatus = %d",nWifiStatus);
	if(nValue == nWifiStatus)
	{
		LOGDBG("MemoryLinkMode == nWifiStatus");
		return bRet;
	}
	int nlen = 1;
	int nCmd = APP_SYSTEM_SET_PHONE_STATUS;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("set wifi memory link mode=%d",Buf[0]);
	}
	else
	{
		LOGDBG("set wifi memory link mode failed");
	}

	return bRet;
}
*/

void IVIController::SendWirelessOFWifiMemoryLinkMode()
{
	HomeData &mHomeData = HomeData::GetInstance();
	int nWifiStatus = 0;
	mHomeData.GetINTData(INT_WIFI_MEMORY_LINK_STATUS, nWifiStatus);
	LOGDBG("nWifiStatus = %d",nWifiStatus);
	SendWirelessInfoData(WIRELESS_CMD_SET_WIFI_MODE,nWifiStatus);
}

bool IVIController::SendMeterInfoDataOFAutoWar(int nCmd,int nType,int nStatus)
{
	bool bRet = false;
	m_MeterInfoSrv = GetServerBinder(METERINFO_SRV);
	if(NULL == m_MeterInfoSrv.get())
	{
		LOGERR("m_MeterInfoSrv.get() is null!");
		return bRet;
	}

	int nlen = 2;
	uint8_t Buf[2]= {0};
	Buf[0] = nType;
	Buf[1] = nStatus;
	int nret = m_MeterInfoSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d %d", Buf[0],Buf[1]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

bool IVIController::BackToBTMACToSys(std::string strMac)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	if((!strMac.empty()) && (strMac.length() < 32))
	{
		int nlen = 32;
		int nCmd = APP_SYSTEM_BT_MAC;
		uint8_t Buf[32]={0};
		memcpy(Buf, strMac.c_str(), strMac.length());
		int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
		if(1 == nret)
		{
			bRet = true;
			LOGDBG("BackToBTMACToSys MAC=%s",Buf);
			
		}
		else
		{
			LOGDBG("BackToBTMACToSys failed");
		}
	}

	return bRet;
}

bool IVIController::BackToBTLinkStatusToSys(const int nValue)
{
	LOGDBG("m_bSetBGMode = %d \n", m_bSetBGMode);
	if(!m_bSetBGMode)
	{
		LOGDBG("Set BG Mode time < 6s");
		return false;
	}
	
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = APP_SYSTEM_BT_STATUS;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;

	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(1 == nret)
	{
		bRet = true;
		LOGDBG("BackToBTLinkStatusToSys value=%d",Buf[0]);
	}
	else
	{
		LOGDBG("BackToBTLinkStatusToSys failed");
	}

	return bRet;
}

bool IVIController::BackWifiNameToSys(std::string strWifiName)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	if(!strWifiName.empty())
	{
		int nlen = strWifiName.length();
		int nCmd = APP_SYSTEM_WIFI_NAME;
		uint8_t Buf[10]={0};
		memcpy(Buf, strWifiName.c_str(), nlen);
		int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
		if(1 == nret)
		{
			bRet = true;
			LOGDBG("BackWifiNameToSys WIFIName=%s",strWifiName.c_str());
		}
		else
		{
			LOGDBG("BackWifiNameToSys failed");
		}
	}
	return bRet;
}


int IVIController::GetBtMacAddr()
{
	int bRet = RETURN_OK;
	m_WirelessSrv = GetServerBinder(WIRELESS_SRV);
	if(NULL == m_WirelessSrv.get())
	{
		LOGERR("m_WirelessSrv.get() is null!");
		return bRet;
	}

	uint8_t Buf[MAX_DATA_LEN] = {0};
	int nlen = 0;
	int nRet = m_WirelessSrv->ReadData(WIRELESS_CMD_GET_BT_MAC, Buf, nlen);

	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nret = m_SystemSrv->WriteData(APP_SYSTEM_BT_MAC, Buf, nlen);

	return nret;
}

void IVIController::SendGearTOAA(int nValid,int nValue)
{
	m_Androidautosrv = GetServerBinder(ANDROIDAUTO_SRV);
	if (nullptr == m_Androidautosrv.get())
	{
		LOGERR("m_Androidautosrv.get() is null!");
		return ;		
	}
	int nCmd = APP_AA_CMD_GEAR;
	int nlen = 2;
	uint8_t Buf[2]= {0};
	Buf[0] = nValid;
	Buf[1] = nValue;
	int nret = m_Androidautosrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		LOGDBG("Send success! %d--%d", Buf[0],Buf[1]);
	}
	else
	{
		LOGDBG("Send failed %d--%d", Buf[0],Buf[1]);
	}
	return ;
}

void IVIController::SourceMemorySelect()
{
	HomeData &mData = HomeData::GetInstance();
	int nResourceType = mData.GetResourceType();
	LOGDBG("nResourceType = %d",nResourceType);
	SetSourceMemory(nResourceType);
}
bool IVIController::SetSourceMemory(const int nValue)
{
	bool bRet = false;
	LOGDBG("m_bSetBGMode = %d \n", m_bSetBGMode);
	if(!m_bSetBGMode)
	{
		LOGDBG("Set BG Mode time < 6s");
		return false;
	}
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	int nCmd = APP_SYSTEM_SET_SOURCE_MEMORY;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("set Source Memory success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("set Source Memory failed");
	}

	return bRet;	
}

bool IVIController::SendMeterInfoDataOFPhoneStatus(int nType)
{
	bool bRet = false;
	m_MeterInfoSrv = GetServerBinder(METERINFO_SRV);
	if(NULL == m_MeterInfoSrv.get())
	{
		LOGERR("m_MeterInfoSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = nType;
	int nret = m_MeterInfoSrv->WriteData(APP_METERINFO_PHONE_STATUS, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

void IVIController::BTLinkMemorySelect()
{
	LOGDBG("m_bSetBGMode = %d \n", m_bSetBGMode);
	if(!m_bSetBGMode)
	{
		LOGDBG("Set BG Mode time < 6s");
		return;
	}
	HomeData &mData = HomeData::GetInstance();
	int nBTLink = 255;
	mData.GetINTData(INT_BT_LINK_STATUS, nBTLink);
	LOGDBG("nBTLink = %d",nBTLink);

	int nCarPlayConnect = 0;
	mData.GetINTData(INT_CARPLAYCONNECTSTATUS, nCarPlayConnect);
	LOGDBG("nCarPlayConnect = %d",nCarPlayConnect);

	//AA 1连接
	int nAAConnectStatus = 0;
	mData.GetINTData(INT_ANDRIODCONNECTSTATUS, nAAConnectStatus);
	LOGDBG("nAAConnectStatus = %d",nAAConnectStatus);

	if(nBTLink == 2 || nCarPlayConnect == 1 || nAAConnectStatus == 1)
	{
		mData.SaveINTData(INT_MCU_BTLINK_MEMORY, 1);		
	}
	else if(nBTLink != 255)
	{
		mData.SaveINTData(INT_MCU_BTLINK_MEMORY, 0);
	}
	else
	{
		mData.SaveINTData(INT_MCU_BTLINK_MEMORY, 0);
		//255
	}
}

bool IVIController::SetBTRadioMemory()
{
	bool bRet = false;
	LOGDBG("m_bSetBGMode = %d \n", m_bSetBGMode);
	if(!m_bSetBGMode)
	{
		LOGDBG("Set BG Mode time < 6s");
		return false;
	}

	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	HomeData &mHomeData = HomeData::GetInstance();
	int nBTLink = 255;
	mHomeData.GetINTData(INT_MCU_BTLINK_MEMORY, nBTLink);
	LOGDBG("nBTLink = %d",nBTLink);

	int nBTMusicPlayStatus = 255;
	mHomeData.GetINTData(INT_BTMUSIC_PLAY_STATUS, nBTMusicPlayStatus);
	LOGDBG("nBTMusicPlayStatus = %d",nBTMusicPlayStatus);

	RadioData &mRadioData = RadioData::GetInstance();
	T_RADIO_CURR_FREQ tCurrFreq; 
	mRadioData.GetCurrFreq(tCurrFreq);
	LOGDBG("tCurrFreq.nCurrMute = %d",tCurrFreq.nCurrMute);

	int nlen = 3;
	int nCmd = APP_SERVER_ACCOFF_SENDMCU_MEMORY;
	uint8_t Buf[3]= {0};
	Buf[0] = nBTLink;
	Buf[1] = nBTMusicPlayStatus;
	Buf[2] = tCurrFreq.nCurrMute;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("set BTLink Memory success! Buf[0]=%d,Buf[1]=%d,Buf[2]=%d", Buf[0],Buf[1],Buf[2]);
	}
	else
	{
		LOGDBG("set BTLink Memory failed");
	}

	return bRet;	
}

void IVIController::SendWirelessOFBTMemoryLinkMode()
{
	HomeData &mHomeData = HomeData::GetInstance();
	int nBTStatus = 255;
	mHomeData.GetINTData(INT_MCU_BTLINK_MEMORY, nBTStatus);
	LOGDBG("nBTStatus = %d",nBTStatus);
	if(nBTStatus != 255)
	{
		SendWirelessInfoData(WIRELESS_CMD_SET_SLEEP_WAKE_UP_BT_RECONNECT,nBTStatus);
	}
}

bool IVIController::SendCarPlayDataOFBTPhoneStatus(int nType)
{
	bool bRet = false;
	m_carplaysrv = GetServerBinder(CARPLAY_SRV);
	if(NULL == m_carplaysrv.get())
	{
		LOGERR("m_carplaysrv.get() is null!");
		return bRet;
	}
	HomeData &mHomeData = HomeData::GetInstance();
	//AA 1连接
	int nAAConnectStatus = 0;
	mHomeData.GetINTData(INT_ANDRIODCONNECTSTATUS, nAAConnectStatus);
	LOGDBG("nAAConnectStatus = %d",nAAConnectStatus);

	if(nAAConnectStatus)
	{
		LOGDBG("AA Connect");
		return bRet;
	}

	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = nType;
	int nret = m_carplaysrv->WriteData(APP_SERVER_CP_BTPHONESTATUS, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

bool IVIController::SetBTMusicMemoryPlayStatus(const int nValue)
{
	HomeData &mHomeData = HomeData::GetInstance();
	mHomeData.SaveINTData(INT_BTMUSIC_PLAY_STATUS, nValue);
	LOGDBG("BTMusic Play Status = %d", nValue);
	SetBTRadioMemory();
	return true;
}

void IVIController::SendWirelessOFBTMemoryBTMusicPlayStatus()
{
	HomeData &mHomeData = HomeData::GetInstance();
	int nBTMusicPlayStatus = 255;
	mHomeData.GetINTData(INT_BTMUSIC_PLAY_STATUS, nBTMusicPlayStatus);
	LOGDBG("nBTMusicPlayStatus = %d",nBTMusicPlayStatus);
	if(nBTMusicPlayStatus != 255)
	{
		int nType = mHomeData.GetResourceType();
		SendResourceType(nType);
		
		SendWirelessInfoData(WIRELESS_CMD_SET_SLEEP_WAKE_UP_BTMUSIC_REPLAY,nBTMusicPlayStatus);

	}
}

bool IVIController::SendCarPlayDataOFComboboxStatus(int nType)
{
	bool bRet = false;
	m_carplaysrv = GetServerBinder(CARPLAY_SRV);
	if(NULL == m_carplaysrv.get())
	{
		LOGERR("m_carplaysrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = nType;
	int nret = m_carplaysrv->WriteData(APP_SERVER_COMBOBOX_STATUS, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

void IVIController::SendCarPlayDataOFReconnectOnclick()
{
	bool bRet = false;
	m_carplaysrv = GetServerBinder(CARPLAY_SRV);
	if(NULL == m_carplaysrv.get())
	{
		LOGERR("m_carplaysrv.get() is null!");
		return;
	}

	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = 1;
	int nret = m_carplaysrv->WriteData(APP_SERVER_CP_RECONNECT_ONCLICK, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return;
}

bool IVIController::SendCarPlayDataOFCallAction(int cmd ,int nType)
{
	bool bRet = false;
	m_carplaysrv = GetServerBinder(CARPLAY_SRV);
	if(NULL == m_carplaysrv.get())
	{
		LOGERR("m_carplaysrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = nType;
	int nret = m_carplaysrv->WriteData(cmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

void IVIController::SaveVendorDisPoseTime()
{
	HomeData &mHomeData = HomeData::GetInstance();
	if(mHomeData.SaveVendorDisPose() && mHomeData.GetVendorDataAA())
	{
		LOGDBG("SaveVendorDisPose");
		T_STR_SCREEN_LOCK_INFO tScreen;
		mHomeData.GetScreenLockInfo(tScreen);
		if(tScreen.nScreenLockStatus == 1)
		{
			SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_RELEASE,1);
		}
		else
		{
			if(mHomeData.GetAAOpenStatus() == 0)
			{
				SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_RELEASE,1);
			}
			else
			{
				//下发连接模式
				SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_LOAD,1);
			}
		}
	}
	else
	{
		SetTimer(TIMERID_GET_DISPOSE, 100);
	}
}

//转向模式
bool IVIController::SetTurnMode(const int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	int nCmd = APP_SERVER_TURN_MODE;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("set Turn success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("set Turn failed");
	}

	return bRet;
}

//胎压复位
bool IVIController::SetTireReset(const int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = APP_SERVER_TIRE_RESET;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("set TireReset success! nValue=%d", nValue);		
	}
	else
	{
		LOGDBG("set TireReset failed");
	}

	return bRet;
}

//系统复位
bool IVIController::Mpu_McuRest()
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	int nCmd = MPU_SEND_TO_MCU_RESET_CMD;
	uint8_t Buf[1]= {0};
	Buf[0] = 1;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("set Mpu_McuRest success");		
	}
	else
	{
		LOGDBG("set Mpu_McuRest failed");
	}

	return bRet;
}

bool IVIController::SendReverseData(int cmd ,int nType)
{
	bool bRet = false;
	m_Reversesrv = GetServerBinder(FY_REVESER_SRV);
	if(NULL == m_Reversesrv.get())
	{
		LOGERR("m_Reversesrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = nType;
	int nret = m_Reversesrv->WriteData(cmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}


bool IVIController::SendAAOFComboboxStatus(int nType)
{
	bool bRet = false;
	m_Androidautosrv = GetServerBinder(ANDROIDAUTO_SRV);
	if(NULL == m_Androidautosrv.get())
	{
		LOGERR("m_Androidautosrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = nType;
	int nret = m_Androidautosrv->WriteData(APP_SERVER_COMBOBOX_STATUS, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

bool IVIController::SendAAOFCallAction(int cmd ,int nType)
{
	bool bRet = false;
	m_Androidautosrv = GetServerBinder(ANDROIDAUTO_SRV);
	if(NULL == m_Androidautosrv.get())
	{
		LOGERR("m_Androidautosrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	uint8_t Buf[1]= {0};
	Buf[0] = nType;
	int nret = m_Androidautosrv->WriteData(cmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf[0]);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

bool IVIController::SendAAOFBTName(std::string info)
{
	bool bRet = false;
	m_Androidautosrv = GetServerBinder(ANDROIDAUTO_SRV);
	if(NULL == m_Androidautosrv.get())
	{
		LOGERR("m_Androidautosrv.get() is null!");
		return bRet;
	}
	if(info.empty())
	{
		LOGERR("BTName.get() is null!");
		return bRet;
	}
	int nlen = 30;
	uint8_t Buf[nlen]= {0};
	memcpy(Buf, info.c_str(), nlen);
	int nret = m_Androidautosrv->WriteData(APP_SERVER_AA_BT_NAME, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %s", Buf);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

bool IVIController::SendAAOFCPConnectStatus(int nStatus)
{
	HomeData &mData = HomeData::GetInstance();
	if(mData.GetAAOpenStatus() == 0)
	{
		return false;
	}	
	bool bRet = false;
	m_Androidautosrv = GetServerBinder(ANDROIDAUTO_SRV);
	if(NULL == m_Androidautosrv.get())
	{
		LOGERR("m_Androidautosrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	uint8_t Buf[nlen]= {0};
	Buf[0] = nStatus;
	int nret = m_Androidautosrv->WriteData(APP_SERVER_CPCONNECTSTATUS, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

bool IVIController::SendCPOFAAConnectStatus(int nStatus)
{
	bool bRet = false;
	m_carplaysrv = GetServerBinder(CARPLAY_SRV);
	if(NULL == m_carplaysrv.get())
	{
		LOGERR("m_carplaysrv.get() is null!");
		return bRet;
	}
	int nlen = 1;
	uint8_t Buf[nlen]= {0};
	Buf[0] = nStatus;
	int nret = m_carplaysrv->WriteData(APP_SERVER_AACONNECTSTATUS, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("Send success! %d", Buf);
	}
	else
	{
		LOGDBG("Send failed");
	}
	return bRet;
}

bool IVIController::SetScreenLockSelect(const int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	T_STR_SCREEN_LOCK_INFO tScreen;
	HomeData &mHomeData = HomeData::GetInstance();
	mHomeData.GetScreenLockInfo(tScreen);
	LOGDBG("nScreenLocknum = %d",tScreen.nScreenLocknum);
	if(tScreen.nScreenLocknum != 0 && nValue == SCREEN_LOCK_LOCK)
	{
		return bRet;
	}

	int nlen = 1;
	int nCmd = APP_SERVER_LOCKACTION;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("jgwen--- set unit success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("jgwen--- set unit failed");
	}

	return bRet;
}

//能量回收强度
bool IVIController::SetEnergyRecover(const int nValue)
{
	bool bRet = false;
	m_SystemSrv = GetServerBinder(SYSTEM_SRV);
	if(NULL == m_SystemSrv.get())
	{
		LOGERR("m_SystemSrv.get() is null!");
		return bRet;
	}

	int nlen = 1;
	int nCmd = APP_SERVER_ENERGY_RECOVER;
	uint8_t Buf[1]= {0};
	Buf[0] = nValue;
	int nret = m_SystemSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
		LOGDBG("set EnergyRecover success! nValue=%d", nValue);
	}
	else
	{
		LOGDBG("set EnergyRecover failed");
	}

	return bRet;
}

bool IVIController::ReadReverseData(int nCmd)
{
	bool bRet = false;
	m_Reversesrv = GetServerBinder(FY_REVESER_SRV);
	if(NULL == m_Reversesrv.get())
	{
		LOGERR("m_Reversesrv.get() is null!");
		return bRet;
	}

	LOGDBG("jgwen------ ReadWirelessData =0x%2x", nCmd);

	uint8_t Buf[MAX_DATA_LEN];
	int nlen = 0;
	int nRet = m_Reversesrv->ReadData(nCmd, Buf, nlen);
	if(nlen > 0)
	{
		HomeData &mHomeData = HomeData::GetInstance();
		switch(nCmd)
		{
			case REVERSE_CMD_CAMERA:
			{
				LOGDBG("SaveReverseData:%d",Buf[0]);
				mHomeData.SaveINTData(INT_REVERSE,Buf[0]);	
				break;
			}
			default:
			{
				break;
			}
		}
		bRet = true;
	}

	return bRet;
}

bool IVIController::SetMediaPlay_Url(T_PLAYMEDIA_URL &tPlayUrl)
{
	bool bRet = false;
	m_MediaSrv = GetServerBinder(MEDIA_SRV);
	if(NULL == m_MediaSrv.get())
	{
		LOGERR("m_MediaSrv.get() is null!");
		return bRet;
	}
	int nlen = sizeof(T_PLAYMEDIA_URL);
	int nCmd = HOME_CMD_MEDIA_URL;
	uint8_t Buf[256]= {0};

	memcpy(Buf,&tPlayUrl,nlen);

	int nret = m_MediaSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
	}
	else
	{
		LOGDBG("&&&---media url fail");
	}

	return bRet;
}

bool IVIController::SetMediaPlay_Cmd(T_PLAYMEDIA_CMD &tPlayCmd)
{
	bool bRet = false;
	m_MediaSrv = GetServerBinder(MEDIA_SRV);
	if(NULL == m_MediaSrv.get())
	{
		LOGERR("m_MediaSrv.get() is null!");
		return bRet;
	}
	int nlen = sizeof(T_PLAYMEDIA_CMD);
	int nCmd = HOME_CMD_MEDIA_CTRL;
	uint8_t Buf[256]= {0};

	memcpy(Buf,&tPlayCmd,nlen);

	int nret = m_MediaSrv->WriteData(nCmd, Buf, nlen);
	if(0 == nret)
	{
		bRet = true;
	}
	else
	{
		LOGDBG("&&&---media url fail");
	}

	return bRet;
}