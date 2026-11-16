/*
 * DataPacket.cpp
 *
 *  Created on: 
 *      Author: 
 */

#include "SettingDataPacket.h"
#include "IVIController.h"
#include "SettingData.h"
#include "IVIView.h"
#include "SourceDef.h"
#include "HomeData.h"
SettingDataPacket::SettingDataPacket(IVIController *pCtrl)
{
	m_pCtrl = pCtrl;
	pthread_mutex_init (&m_Lock, NULL);

	m_bExitFlag = false;

	//SetToRunning(true);
}

SettingDataPacket::~SettingDataPacket()
{
	pthread_mutex_destroy( &m_Lock );
}

bool SettingDataPacket::Init()
{
	bool nRet = false;
	nRet = ThreadControl::Init();
	nRet = nRet & ThreadIsReady();
	DoIt();
	return nRet;
}

void SettingDataPacket::DoIt()
{
	ThreadControl::DoIt();
}

bool SettingDataPacket::ThreadIsReady()
{
	return ThreadControl::ThreadIsReady();
}

void SettingDataPacket::Close()
{
	ThreadControl::Close();
}

void SettingDataPacket::DealWithTask()
{
	static bool bFlag = true;
	if(bFlag)
	{
		bFlag = false;
		int ret = pthread_setname_np(pthread_self(), "SettingDataPacket");
		if( ret != EOK )
		{
			bFlag = true;
			LOGDBG("jgwen---  DataPacket pthread_setname_np failed!");
		}
	}

	while (!m_bExitFlag)
	{
		SettingData &mData = SettingData::GetInstance();
		mData.WaitForPacket();
		if(mData.GetVecPacket_Size() > 0)
		{
			memset(&m_tDataPacket, 0, sizeof(T_STR_DATA_PACKET));
			mData.GetPopPacket(m_tDataPacket);
			ParseDataPacket(m_tDataPacket);
		}
	}
}

void SettingDataPacket::ParseDataPacket(T_STR_DATA_PACKET tPacket)
{
	bool bRet = false;
	T_APP_MSG nMsg = {0, 0, 0, 0 };
	LOGDBG("jgwen---- tPacket.nSrvId =%d", tPacket.nSrvId);
	switch(tPacket.nSrvId)
	{
		case SRV_ID_SYSTEM_BINDER:
		{
			bRet = HandleSysteminfoData(tPacket, nMsg);
		}
		break;
		case SRV_ID_APPMANAGER_BINDER:
		{
			bRet = HandleAppManagerData(tPacket, nMsg);	
		}
		break;
		case SRV_ID_METERINFO_BINDER:
		{
			bRet = HandleMeterInfoData(tPacket, nMsg);
		}
		break;
		case SRV_ID_WIRELESSSER_BINDER:
		{
			bRet = HandleWirelessData(tPacket, nMsg);
		}
		break;
		default:
		{
			break;
		}
	}

	if(bRet && (0 != nMsg.Message))
	{
		//LOGDBG("jgwen--- SettingApp:SendViewMessage:Message=%d,Param0=%d,Param1=%d,Param2=%d\n",\
									nMsg.Message, nMsg.Param0, nMsg.Param1, nMsg.Param2 );
		IVIView &mView = IVIView::GetInstance();
		mView.SendViewMessage(&nMsg);
	}
}

bool SettingDataPacket::HandleSysteminfoData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	if(NULL == m_pCtrl)
	{
		LOGDBG("m_pCtrl is null ");
		return bRet;
	}
	SettingData &mData = SettingData::GetInstance();
	LOGDBG("jgwen--- Systeminfo tPacket.nCmd=0x%02x \n ", tPacket.nCmd);
	switch(tPacket.nCmd)
	{
		case SYSTEMSRV_CMD_MCU_TIME://0x10  //时间
		{
			if(tPacket.nLen > 0)
			{
				T_STR_TIME_INFO tTime;
				tTime.nClockMode = tPacket.Buf[0];
				tTime.nYear = MAKEWORD(tPacket.Buf[2], tPacket.Buf[1]);
				tTime.nMonth = tPacket.Buf[3]; 
				tTime.nDay = tPacket.Buf[4];
				tTime.nHours = tPacket.Buf[5];
				tTime.nMinutes = tPacket.Buf[6];     
				mData.SaveTimeData(tTime);
				LOGDBG("jgwen--- SaveTimeData:ClockMode=%d, Year=%d, Month=%d, Day=%d, Hour=%d, Minute=%d\r\n", \
						tTime.nClockMode, tTime.nYear, tTime.nMonth, tTime.nDay, tTime.nHours, tTime.nMinutes);
				msg.Message = SYS_VMSG_UPDATE_DATETIME;
				bRet = true;
			}
		}
		break;
		case SYSTEMSRV_CMD_SYNC_MCU_DATE:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("jgwen--- SaveINTData:syncstate=%d\n",tPacket.Buf[0]);
				int nTimeSync = tPacket.Buf[0];
				mData.SaveINTData(INT_SYNCTIME, nTimeSync);
				msg.Message = SYS_VMSG_UPDATE_SYNCTIME;
				bRet = true;	
			}
		}
		break;
		case SYSTEMSRV_CMD_MCU_VERSION://0x12  // MCU版本
		{
			string strMCUVer = (char*)tPacket.Buf;
			mData.SaveMCUVersion(strMCUVer);
			LOGDBG("jgwen--- strMCUVer = %s", strMCUVer.c_str());
			msg.Message = SYS_VMSG_UPDATE_VERSION;
			bRet = true;
		}
		break;
		case SYSTEMSRV_CMD_MPU_VERSION://0x14  // MPU版本
		{
			string strMPUVer = (char*)tPacket.Buf;
			mData.SaveMPUVersion(strMPUVer);
			LOGDBG("jgwen--- strMPUVer = %s", strMPUVer.c_str());
			msg.Message = SYS_VMSG_UPDATE_VERSION;	
			bRet = true;
		}	
		break;	
		case SYSTEMSRV_CMD_HARD_VERSION: //0x41  //硬件版本
		{
			string strHARDVer = (char*)tPacket.Buf;
			mData.SaveHARDVersion(strHARDVer);
			LOGDBG("jgwen--- strHARDVer = %s", strHARDVer.c_str());
			msg.Message = SYS_VMSG_UPDATE_VERSION;	
			bRet = true;	
		}
		break;
		case SYSTEMSRV_CMD_SCREEN_HW_VER:  // 屏幕硬件版本号
		{
			string strHwVer="";
			strHwVer.append((char*)&tPacket.Buf[0],tPacket.nLen);
			mData.SaveScreenHwVer(strHwVer);
			LOGDBG("jgwen--- strHwVer = %s", strHwVer.c_str());
			msg.Message = SYS_VMSG_UPDATE_SCREEN_VERSION;	
			bRet = true;	
		}
		break;
		case SYSTEMSRV_CMD_SCREEN_SOFT_VER:  // 屏幕软件版本号
		{
			string strSoftVer="";
			strSoftVer.append((char*)&tPacket.Buf[0],tPacket.nLen);
			mData.SaveScreenSoftVer(strSoftVer);
			LOGDBG("jgwen--- strSoftVer = %s", strSoftVer.c_str());
			msg.Message = SYS_VMSG_UPDATE_SCREEN_VERSION;	
			bRet = true;	
		}
		break;
		case SYSTEMSRV_CMD_SCREEN_TP_VER:  // 屏幕固件版本号
		{
			string strTpVer="";
			strTpVer.append((char*)&tPacket.Buf[0],tPacket.nLen);
			mData.SaveScreenTpVer(strTpVer);
			LOGDBG("jgwen--- strTpVer = %s", strTpVer.c_str());
			msg.Message = SYS_VMSG_UPDATE_SCREEN_VERSION;	
			bRet = true;	
		}
		break;				
		case SYSTEMSRV_CMD_LANGUAGE_SET://0x20  // 语言设置
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("jgwen--- SaveINTData:nLanguage=%d\n",tPacket.Buf[0]);
				int nLanguage = tPacket.Buf[0];
				mData.SaveINTData(INT_LANGUAGE, nLanguage);
				msg.Message = SYS_VMSG_UPDATE_LANGUAGE;
				bRet = true;
			}
		}
		break;
		case SYSTEMSRV_CMD_BACKLIGHT_SET://0x21  背光等级设置
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("jgwen--- SaveINTData:nBacklightLevel=%d\n",tPacket.Buf[0]);
				int nBacklightLevel = tPacket.Buf[0];
				mData.SaveINTData(INT_BACKLIGHT_LEVEL, nBacklightLevel);
				msg.Message = SYS_VMSG_UPDATE_BACKLIGHT_LEVEL;
				bRet = true;
			}
		}
		break;
		case SYSTEMSRV_CMD_VOLUAME_SET:  // 0x42 音量等级设置
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("jgwen--- SaveINTData: AlarmLevel = %d, MediaLevel = %d, RadioLevel = %d, BTCallLevel = %d \n", \
					tPacket.Buf[0],tPacket.Buf[1],tPacket.Buf[2],tPacket.Buf[3]);
				T_VOLUME_INFO tVolume;
				tVolume.nAlarmLevel = tPacket.Buf[0];
				tVolume.nMediaLevel = tPacket.Buf[1];
				tVolume.nRadioLevel = tPacket.Buf[2]; 
				tVolume.nBTCallLevel = tPacket.Buf[3];
				mData.SaveINTData(INT_VOLUME_LEVEL, tVolume.nMediaLevel);
				mData.SaveINTData(INT_BT_VOLUME_LEVEL, tVolume.nBTCallLevel);
				msg.Message = SYS_VMSG_UPDATE_VOLUME_VALUE;
				bRet = true;  
			}
		}
		break;
		case SYSTEMSRV_CMD_BACKGROUND_SET://0x22  背光模式设置
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("jgwen--- SaveINTData:nBackGround=%d\n",tPacket.Buf[0]);
				int nBackGround = tPacket.Buf[0];
				if(nBackGround == 1 || nBackGround == 2)
				{
					mData.SaveINTData(INT_BACKGROUND, nBackGround);
				}
				else
				{
					mData.SaveINTData(INT_BACKGROUND, 1);
				}
				//msg.Message = SYS_VMSG_UPDATE_BACKGROUND;
				//bRet = true;
			}		
		}	
		break;	
		case SYSTEMSRV_CMD_UNIT_SET://0x23  // 单位设置
		{ 
			if(tPacket.nLen > 0)
			{
				int nUnitMode= tPacket.Buf[0];
				LOGDBG("jgwen--- SaveINTData:nUnitMode=%d\n",tPacket.Buf[0]);
				int nPreUnit = -1;
				mData.GetINTData(INT_UNIT_MODE, nPreUnit);
				if(nPreUnit != nUnitMode)
				{
					mData.SaveINTData(INT_UNIT_MODE, nUnitMode);
					msg.Message = SYS_VMSG_UPDATE_UNIT; // 刷新单位
					bRet = true;
				}
			}
		}
		break;
		case SYSTEMSRV_CMD_WHEEL_TYPE: //0x43  // 轮胎规格
		{
			if(tPacket.nLen > 0)
			{
				int nWheelType= tPacket.Buf[0];
				LOGDBG("jgwen--- SaveINTData:nWheelType=%d\n",tPacket.Buf[0]);
				int nPreType = -1;
				mData.GetINTData(INT_WHEEL_TYPE, nPreType);
				if(nPreType != nWheelType)
				{
					mData.SaveINTData(INT_WHEEL_TYPE, nWheelType);
					msg.Message = SYS_VMSG_UPDATE_WHEEL; // 刷新单位
					bRet = true;
				}
			}
		}
		break;
		case SYSTEMSRV_CMD_READ_LIGHT: //0x44  //阅读灯开关
		{
			if(tPacket.nLen > 0)
			{
				int nReadlight= tPacket.Buf[0];
				LOGDBG("jgwen--- SaveINTData:nReadlight=%d\n",tPacket.Buf[0]);
				mData.SaveINTData(INT_READ_LIGHT, nReadlight);
				msg.Message = SYS_VMSG_UPDATE_READLIGHT; // 刷新单位
				mData.GetINTData(INT_POSLIGHT, msg.Param0);
				bRet = true;
				
			}
		}
		break;
		case SYSTEMSRV_CMD_DISCONNECT_BT:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveACCOFFBT:nBTDisconnect=%d\n",tPacket.Buf[0]);
				int nBTDisconnect = tPacket.Buf[0];
				msg.Message = SYS_VMSG_DISCONNECT_BT;
				bRet = true;
			}				
		}
		break;
		case SYSTEMSRV_CMD_TURN_MODE:
		{
			if(tPacket.nLen > 0)
			{
				int nTurnMode= tPacket.Buf[0];
				LOGDBG("SaveINTData:nTurnMode=%d\n",tPacket.Buf[0]);
				mData.SaveINTData(INT_TURN_MODE, nTurnMode);
				msg.Message = SYS_VMSG_UPDATE_TURN_MODE;
				bRet = true;
			}	
		}
		break;
		case SYSTEMSRV_CMD_ENERGY_RECOVER:
		{
			if(tPacket.nLen > 0)
			{
				int nEnergyRecover= tPacket.Buf[0];
				LOGDBG("SaveINTData:nEnergyRecover=%d\n",tPacket.Buf[0]);
				mData.SaveINTData(INT_ENERGY_RECOVER, nEnergyRecover);
				msg.Message = SYS_VMSG_UPDATE_ENERGY_RECOVER;
				bRet = true;
			}	
		}
		break;				
		default:
		{
			break;
		}
	}

	return bRet;
}

bool SettingDataPacket::HandleMeterInfoData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	SettingData &mData = SettingData::GetInstance();
	switch(tPacket.nCmd)
	{
		case METERINFO_APP_POSITION_LIGHT_INFO://0x22  // 前照灯
		{
			LOGDBG("jgwen--- SaveINTData:Position=%d\n",tPacket.Buf[0]);
			int nPosLight = tPacket.Buf[0];
			mData.SaveINTData(INT_POSLIGHT, nPosLight);
			msg.Message = SYS_VMSG_UPDATE_BACKGROUND;
			msg.Param0 = nPosLight;
			bRet = true;
		}
		break;
		case METERINFO_APP_REMAINING_MILEAGE_INFO://0x23  //剩余保养里程信息
		{
			int nRaMileage = -1;
			nRaMileage = MAKELONG(MAKEWORD(tPacket.Buf[5], tPacket.Buf[4]), MAKEWORD(tPacket.Buf[3], tPacket.Buf[2]));
			LOGDBG("jgwen--- SaveINTData:nRaMileage=%d\n",nRaMileage);
			mData.SaveINTData(INT_RAMILEAGE, nRaMileage);
			msg.Message = METER_VMSG_UPDATE_MAINTAIN;
			msg.Param0 = nRaMileage;
			bRet = true;	
		}
		break;
		case METERINFO_APP_VIN_PARAM:
		{
			char strVin[24] = {0};
			for(int i=0; i<tPacket.nLen; i++ )
			{
				LOGDBG("jgwen--- strVin=%c, %d, %x\n",tPacket.Buf[i], tPacket.Buf[i], tPacket.Buf[i]);
				strVin[i] = tPacket.Buf[i];
			}
			mData.SaveVinData(strVin);
			msg.Message = SYS_VMSG_UPDATE_VICODE;
			bRet = true;
		}
		break;
		case METERINFO_APP_VEHICLE_STATE:
		{
			if(tPacket.nLen > 0)
			{
				int nTireReset= tPacket.Buf[13];
				LOGDBG("SaveINTData:nTireReset=%d\n",tPacket.Buf[13]);
				mData.SaveINTData(INT_TIRE_RESET, nTireReset);
				msg.Message = SYS_VMSG_UPDATE_TIRE_RESET;
				msg.Param0 = 0;
				bRet = true;
			}	
		}
		break;
		default:
			break;
	}

	return bRet;
}

bool SettingDataPacket::HandleWirelessData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	SettingData &mData = SettingData::GetInstance();
	switch(tPacket.nCmd)
	{
		case WIRELESS_EVENT_BT_OPEN_STATE: //0x02
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("jgwen--- SaveBtState:nBtState=%d\n",tPacket.Buf[0]);
				int nBtState = tPacket.Buf[0];
				mData.SaveBtState(nBtState);
				msg.Message = SYS_VMSG_UPDATE_BT_STWITCH;
				bRet = true;
			}
		}
		break;
		case WIRELESS_EVENT_BT_INFO:
		{
			BtLocalInfo_t *tBtInfo = (BtLocalInfo_t*)tPacket.Buf;
			if(NULL != tBtInfo)
			{
				LOGDBG("jgwen--- tBtInfo.BTName=%s\n",tBtInfo->BTName);
				LOGDBG("jgwen--- tBtInfo.macAddr=%s\n",tBtInfo->macAddr);
				mData.SaveBtLocalInfo(tBtInfo);

				if(NULL != m_pCtrl)
				{
					//转发BTMac SYS
					m_pCtrl->BackToBTMACToSys(tBtInfo->macAddr);
					m_pCtrl->GetBtMacAddr();  //��ȡBT MAC��ַ
					m_pCtrl->SendAAOFBTName(tBtInfo->BTName);
				}
				msg.Message = SYS_VMSG_UPDATE_BT_NAME;
				bRet = true;
			}
			break;
		}
		break;
		case WIRELESS_EVENT_BT_PAIRED_DEVICE_INFO:
		{
			BT_PAIRED_LIST_T *nPairedlist = (BT_PAIRED_LIST_T*)tPacket.Buf;
			bool nShow = false;
			for(int i=0; i<5; i++)
			{
				LOGDBG("jgwen[%d]--- num=%d, index=%d, connState=%d, BTDeviceName=%s,add=%s\n", \
				i, nPairedlist->num, nPairedlist->index[i],nPairedlist->connState[i],nPairedlist->BTDeviceName[i],nPairedlist->addr[i]);
				LOGDBG("\n");
				if(nPairedlist->connState[i] == 2)
				{
					nShow = true;
				}			
			}		
			if(nShow)
			{
				HomeData &mData = HomeData::GetInstance();
				mData.SaveINTData(INT_BT_LINK_STATUS, 2);

				//转发BT连接状态
				m_pCtrl->BackToBTLinkStatusToSys(2);
				m_pCtrl->SendWirelessOFBTMemoryBTMusicPlayStatus();
			}
			else
			{
				HomeData &mData = HomeData::GetInstance();
				mData.SaveINTData(INT_BT_LINK_STATUS, 0);
			}
			mData.SavePairedlist(nPairedlist);
			msg.Message = SYS_VMSG_UPDATE_BT_LIST;
			bRet = true;
		}
		break;
		default:
		{
			break;
		}
	}

	return bRet;
}

bool SettingDataPacket::HandleAppManagerData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	SettingData &mData = SettingData::GetInstance();
	switch(tPacket.nCmd)
	{
		case 1://0x1
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return bRet;
}

