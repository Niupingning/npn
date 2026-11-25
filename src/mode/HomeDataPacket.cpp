/*
 * HomeDataPacket.cpp
 *
 *  Created on: 2021-08-18
 *      Author: Y3553
 */

#include "HomeDataPacket.h"
#include "IVIController.h"
#include "HomeData.h"
#include "IVIView.h"
#include "SourceDef.h"

HomeDataPacket::HomeDataPacket(IVIController *pCtrl)
{
	m_pCtrl = pCtrl;
	pthread_mutex_init (&m_Lock, NULL);

	m_bExitFlag = false;

	//SetToRunning(true);
}

HomeDataPacket::~HomeDataPacket()
{
	pthread_mutex_destroy( &m_Lock );
}

bool HomeDataPacket::Init()
{
	bool nRet = false;
	nRet = ThreadControl::Init();
	nRet = nRet & ThreadIsReady();
	DoIt();
	return nRet;
}

void HomeDataPacket::DoIt()
{
	ThreadControl::DoIt();
}

bool HomeDataPacket::ThreadIsReady()
{
	return ThreadControl::ThreadIsReady();
}

void HomeDataPacket::Close()
{
	ThreadControl::Close();
}

void HomeDataPacket::DealWithTask()
{
	static bool bFlag = true;
	if(bFlag)
	{
		bFlag = false;
		int ret = pthread_setname_np(pthread_self(), "HomeDataPacket");
		if( ret != EOK )
		{
			bFlag = true;
			LOGDBG(" DataPacket pthread_setname_np failed!");
		}
	}

	while (!m_bExitFlag)
	{
		HomeData &mData = HomeData::GetInstance();
		mData.WaitForPacket();
		if(mData.GetVecPacket_Size() > 0)
		{
			memset(&m_tDataPacket, 0, sizeof(T_STR_DATA_PACKET));
			mData.GetPopPacket(m_tDataPacket);
			ParseDataPacket(m_tDataPacket);
		}
	}
}

void HomeDataPacket::ParseDataPacket(T_STR_DATA_PACKET tPacket)
{
	LOGDBG(" tPacket.nSrvId = %d",tPacket.nSrvId);
	bool bRet = false;
	T_APP_MSG nMsg = {0, 0, 0, 0 };
	switch(tPacket.nSrvId)
	{
		case SRV_ID_METERINFO_BINDER:
		{
			bRet = HandleMeterInfoData(tPacket, nMsg);
			break;
		}
		case SRV_ID_SYSTEM_BINDER:
		{
			bRet = HandleSystemData(tPacket, nMsg);
			break;
		}
		case SRV_ID_WIRELESSSER_BINDER:
		{
			bRet = HandleWirelessSerData(tPacket, nMsg);
			break;
		}
		case SRV_ID_MOBILENET_BINDER:
		{
			bRet = HandleMobileNetData(tPacket, nMsg);
			break;
		}
		case SRV_ID_CARPLAY_BINDER:
		{
			bRet = HandleCarPlayData(tPacket, nMsg);
			break;
		}
		case SRV_ID_ANDROID_BINDER:
		{
			bRet = HandleAndroidData(tPacket, nMsg);
			break;
		}
		case SRV_ID_REVESER_BINDER:
		{
			bRet = HandleReversesrvData(tPacket, nMsg);
			break;
		}
		case SRV_ID_CANBOX_BINDER:
		{
			bRet = HandleCanBoxData(tPacket, nMsg);
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

bool HomeDataPacket::HandleMeterInfoData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	HomeData &mData = HomeData::GetInstance();
	switch(tPacket.nCmd)
	{
		case METERINFO_APP_TPMS_VALUE://0x4
		{
			if(tPacket.nLen > 0)
			{
				T_STR_TIRE_INFO tTire;
				tTire.nTirePresUnit = tPacket.Buf[0];
				tTire.nFLTirePresValid = tPacket.Buf[1];
				tTire.nFLTirePresValue = MAKEWORD(tPacket.Buf[3], tPacket.Buf[2]);

				tTire.nRLTirePresValid = tPacket.Buf[4];
				tTire.nRLTirePresValue = MAKEWORD(tPacket.Buf[6], tPacket.Buf[5]);

				tTire.nFRTirePresValid = tPacket.Buf[7];
				tTire.nFRTirePresValue = MAKEWORD(tPacket.Buf[9], tPacket.Buf[8]);

				tTire.nRRTirePresValid = tPacket.Buf[10];
				tTire.nRRTirePresValue = MAKEWORD(tPacket.Buf[12], tPacket.Buf[11]);
				LOGDBG("SaveTireData:nTirePresUnit=%d,nFLTirePresValid=%d,nFLTirePresValue=%d,nRLTirePresValid=%d,nRLTirePresValue=%d,nFRTirePresValid=%d,nFRTirePresValue=%d,nRRTirePresValid=%x,nRRTirePresValue=%d",\
					tTire.nTirePresUnit, tTire.nFLTirePresValid, tTire.nFLTirePresValue, tTire.nRLTirePresValid,tTire.nRLTirePresValue, tTire.nFRTirePresValid, tTire.nFRTirePresValue, tTire.nRRTirePresValid, tTire.nRRTirePresValue);
				mData.SaveTireData(tTire);
				msg.Message = METER_VMSG_TIRE_INFO;
				bRet = true;
			}
			break;
		}
		/*tm*/
		case METERINFO_APP_VEHICLE_STATE:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveVehicleState:TOPTempValid=%d,TOPTempUnit=%d,TOPTempValue=%d,ACValid=%d,ACState=%d,nTireRsetStatus=%d\n",\
				tPacket.Buf[7], tPacket.Buf[8], MAKEWORD(tPacket.Buf[10], tPacket.Buf[9]), tPacket.Buf[11], tPacket.Buf[12], tPacket.Buf[13]);
				T_STR_VEHICLE_STATE_INFO nVehicleState;
				nVehicleState.nTopTempValid = tPacket.Buf[7];
				nVehicleState.nTopTempUnit = tPacket.Buf[8];
				nVehicleState.nTopTempValue = MAKEWORD(tPacket.Buf[10], tPacket.Buf[9]);
				nVehicleState.nACValid = tPacket.Buf[11];
				nVehicleState.nACState = tPacket.Buf[12];
				nVehicleState.nTireRsetStatus = tPacket.Buf[13];
				mData.SaveVehicleState(nVehicleState);
				
				msg.Message = APP_VMSG_VEHICLE_STATE_INFO;
				bRet = true;
			}
			break;
		}
		case METERINFO_APP_LAST_CHARGING:
		{
			if(tPacket.nLen > 0)
			{
				T_HOME_LASTCHARGE_INFO nLasrCharge;
				nLasrCharge.nMileValid = tPacket.Buf[0];
				nLasrCharge.nMileUnit = tPacket.Buf[1];
				nLasrCharge.nMileValue = MAKELONG(MAKEWORD(tPacket.Buf[5], tPacket.Buf[4]), MAKEWORD(tPacket.Buf[3], tPacket.Buf[2]));
				nLasrCharge.nTimeValid = tPacket.Buf[6];
				nLasrCharge.nTimeUnit = tPacket.Buf[7];
				nLasrCharge.nTimeValue = MAKELONG(MAKEWORD(tPacket.Buf[11], tPacket.Buf[10]), MAKEWORD(tPacket.Buf[9], tPacket.Buf[8]));
				nLasrCharge.nAveEn = MAKEWORD(tPacket.Buf[13], tPacket.Buf[12]);
				nLasrCharge.nTimeH = tPacket.Buf[14];
				nLasrCharge.nTimeM = tPacket.Buf[15];
				mData.SaveLastCharge(nLasrCharge);
				
				LOGDBG("SaveLastCharging:nMileValid=%d,nMileUnit=%d,nMileValue=%d,nTimeValid=%d,nTimeUnit=%d,nTimeValue=%d,nAveEn=%d,nTimeH=%d,nTimeM=%d\n",\
				nLasrCharge.nMileValid,nLasrCharge.nMileUnit,nLasrCharge.nMileValue,nLasrCharge.nTimeValid,nLasrCharge.\
				nTimeUnit,nLasrCharge.nTimeValue,nLasrCharge.nAveEn,nLasrCharge.nTimeH,nLasrCharge.nTimeM);

				msg.Message = METER_VMSG_LASTCHARGE_INFO;
				bRet = true;
			}
			break;			
		}
		case METERINFO_APP_TIRE_LIGHT:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveTireStatusData:nValue=%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_TIRE_STATUS,tPacket.Buf[0]);				
				msg.Message = METER_VMSG_TIRESTATUS;
				msg.Param0 = tPacket.Buf[0];		
				bRet = true;
			}			
			break;
		}
		case METERINFO_APP_MILEAGE://0x6
		{
			if(tPacket.nLen > 0)
			{
				T_STR_MILEAGE_INFO tMile;
				tMile.nTotaMileValid = tPacket.Buf[0];
				tMile.nTotaMileUnit = tPacket.Buf[1];
				tMile.nTotaMileValue  = MAKELONG(MAKEWORD(tPacket.Buf[5], tPacket.Buf[4]), MAKEWORD(tPacket.Buf[3], tPacket.Buf[2]));
				tMile.nTriMileValid = tPacket.Buf[6];
				tMile.nTriMileUnit = tPacket.Buf[7];
				tMile.nTriMileValue  = MAKELONG(MAKEWORD(tPacket.Buf[11], tPacket.Buf[10]), MAKEWORD(tPacket.Buf[9], tPacket.Buf[8]));
				tMile.nRanMileValue  = MAKEWORD(tPacket.Buf[19], tPacket.Buf[18]);
				tMile.nTriBMileValid = tPacket.Buf[12];
				tMile.nTriBMileUnit = tPacket.Buf[13];
				tMile.nTriBMileValue  = MAKELONG(MAKEWORD(tPacket.Buf[17], tPacket.Buf[16]), MAKEWORD(tPacket.Buf[15], tPacket.Buf[14]));
				LOGDBG("SaveMileageData:nTotaMileValid=%d,nTotaMileValue=%d",tMile.nTotaMileValid, tMile.nTotaMileValue);
				LOGDBG("SaveMileageData:nTriAMileValid=%d,nTriAMileValue=%d",tMile.nTriMileValid, tMile.nTriMileValue);
				LOGDBG("SaveMileageData:nRanMileValue=%d",tMile.nRanMileValue);
				LOGDBG("SaveMileageData:nTriBMileValid=%d,nTriBMileValue=%d",tMile.nTriBMileValid, tMile.nTriBMileValue);
				mData.SaveMileageData(tMile);
				msg.Message = METER_VMSG_MILEAGE;
				bRet = true;
			}
			break;
		}
		case METERINFO_APP_ELECTRIC_QUANTITY:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveCELLData:nValid=%d,nValue=%d,nIndex=%d",tPacket.Buf[0], tPacket.Buf[1], tPacket.Buf[2]);
				T_STR_CELL_INFO tCeel;
				tCeel.nCellValid = tPacket.Buf[0];
				tCeel.nCellValue = tPacket.Buf[1];
				tCeel.nCellIndex = tPacket.Buf[2];

				tCeel.nTimevalid = tPacket.Buf[3];
				tCeel.nTimeUnit = tPacket.Buf[4];
				tCeel.nTimeValue = MAKEWORD(tPacket.Buf[6], tPacket.Buf[5]);
				mData.SaveCellData(tCeel);
				msg.Message = METER_VMSG_CELL;
				msg.Param0 = tCeel.nCellValid;
				msg.Param1 = tCeel.nCellValue;
				msg.Param2 = tCeel.nCellIndex;
				LOGDBG("SaveCELLData:nTimevalid=%d,nTimeUnit=%d,nTimeValue=%d",tCeel.nTimevalid, tCeel.nTimeUnit, tCeel.nTimeValue);
				bRet = true;
			}
			break;
		}
		case METERINFO_APP_TGS://0x5
		{
			if(tPacket.nLen > 0)
			{
				m_pCtrl->SendGearTOAA(tPacket.Buf[0],tPacket.Buf[1]);
			}
			break;
		}
		case METERINFO_APP_INDICATOR_LIGHT:
		{
			if(tPacket.nLen > 0)
			{
				mData.SaveRushStatus(tPacket.Buf[9] & tPacket.Buf[28]);
				LOGDBG("Rush Status = %d",mData.GetRushStatus());

				mData.SaveParkingStatus(tPacket.Buf[14]);//驻车灯
				msg.Message = METER_VMSG_HOMEANIMA;
				bRet = true;
			}
			break;
		}
		case METERINFO_APP_POWER://0x13
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("Handle:tPacket.nCmd=%d,tPacket.nLen=%d,tPacket.Buf[0]=%d,tPacket.Buf[1]=%d,tPacket.Buf[2]=%d,tPacket.Buf[3]=%d\n",\
						tPacket.nCmd,tPacket.nLen,tPacket.Buf[0],tPacket.Buf[1],tPacket.Buf[2],tPacket.Buf[3]);
				T_STR_POWER_INFO nPower;
				nPower.nValid = tPacket.Buf[0];
				nPower.nUnit = tPacket.Buf[1];
				nPower.nValue = MAKEWORD(tPacket.Buf[3], tPacket.Buf[2]);
				mData.SavePowerData(nPower);
				LOGDBG("SavePowerData:nValid=%d,nUnit=%d,nValue=%d\n",nPower.nValid, nPower.nUnit, nPower.nValue );
				msg.Message = METER_VMSG_HOMEANIMA;
				msg.Param0 = nPower.nValid;
				msg.Param1 = nPower.nValue;
				msg.Param2 = nPower.nUnit;
				bRet = true;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	return bRet;
}

bool HomeDataPacket::HandleSystemData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	if(NULL == m_pCtrl)
	{
		LOGDBG("m_pCtrl is null ");
		return bRet;
	}
	HomeData &mData = HomeData::GetInstance();
	IVIView &mView = IVIView::GetInstance();
	switch(tPacket.nCmd)
	{
		case SYSTEMSRV_CMD_MCU_TIME://0x10
		{
			if(tPacket.nLen > 0)
			{
				//LOGDBG("SaveTimeData:nMode=%d,nHours=%d,nMinutes=%d\n",\
							tPacket.Buf[0], tPacket.Buf[5], tPacket.Buf[6]);
				T_STR_TIME_INFO tTime;
				tTime.nClockMode = tPacket.Buf[0];
				tTime.nHours = tPacket.Buf[5];
				tTime.nMinutes = tPacket.Buf[6];
				mData.SaveTimeData(tTime);
				msg.Message = METER_VMSG_DATETIME;
				msg.Param0 = tTime.nClockMode;
				msg.Param1 = tTime.nHours;
				msg.Param2 = tTime.nMinutes;
				bRet = true;
			}
			break;
		}
		case SYSTEMSRV_CMD_BOOTSIGNAL:
		{
			if(tPacket.nLen > 0)
			{
				static int nOldACCStatus = -1;
				LOGDBG("SaveACCStatusData:nValue=%d nOldACCStatus=%d",tPacket.Buf[2],nOldACCStatus);
				if(tPacket.Buf[2] == 0)
				{		
					mData.SaveINTData(INT_ACCSTATUS,0);		
					//msg.Message = METER_VMSG_ACCSTATUS;
					//msg.Param0 = UIID_WNDID_BLACK;
					if(nOldACCStatus != -1 && nOldACCStatus != tPacket.Buf[2])
					{
						//下发BT记忆
						m_pCtrl->BTLinkMemorySelect();
						//下发源记忆
						m_pCtrl->SourceMemorySelect();
						m_pCtrl->SetBTRadioMemory();
					}

					//关背光
					m_pCtrl->ACC_Bklight(0);	

					m_pCtrl->SendCarPlayData(APP_APPSERVICE_OPENCARPLAY_TYPE,0);
					m_pCtrl->SendAndroidautoData(APP_SERVER_AA_ACCSTATUS,tPacket.Buf[2]);
					m_pCtrl->SendAndroidautoData(APP_APPSERVICE_OPENAA_TYPE,0);
					m_pCtrl->SendMobileData(APP_APPSERVICE_OPENYILIAN_TYPE,0);
					m_pCtrl->SendWirelessInfoData(WIRELESS_CMD_SET_ACC_MODE,1);

					mData.SaveBtPairISFrist(true);
					//下发收音机静音
					m_pCtrl->SendMeterInfoData(RADIO_MUTE,0,0,0);

					mData.SaveINTData(INT_MOBIL_ACC,tPacket.Buf[2]);
					m_pCtrl->SendMobileData(APP_MOBILENET_ACC_STATUS,tPacket.Buf[2]);
					m_pCtrl->SendCarPlayData(APP_SERVER_CP_ACCSTATUS,tPacket.Buf[2]);
					m_pCtrl->SendReverseData(APP_SERVER_REVESER_ACCSTATUS,tPacket.Buf[2]);
				}
				else
				{
					m_pCtrl->SendWirelessOFBTMemoryLinkMode();
					msg.Message = SYS_VMSG_ACCSTATUS_TIME;

					//开背光
					m_pCtrl->ACC_Bklight(1);
				}
				mData.SaveINTData(INT_ACC_CURRSTATUS,tPacket.Buf[2]);
				if(nOldACCStatus != tPacket.Buf[2])
				{				
					//bRet = true;
				}
				nOldACCStatus = tPacket.Buf[2];
			}	
			break;
		}
		case SYSTEMSRV_CMD_ANIM_END_IVIHOME:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("Anima end:nValue=%d",tPacket.Buf[0]);
				/*
				T_STR_SCREEN_LOCK_INFO tScreen;
				mData.GetScreenLockInfo(tScreen);*/
				/*if(tPacket.Buf[0] == 1 )//&& tScreen.nScreenLockStatus != 1)
				{
					mData.SaveINTData(INT_ACCSTATUS,1);	
					//msg.Param0 = mData.GetLastWnd();
					msg.Param0  = UIID_WNDID_MAIN;
					msg.Message = METER_VMSG_ACCSTATUS;
					m_pCtrl->SendWirelessInfoData(WIRELESS_CMD_SET_ACC_MODE,0);
					m_pCtrl->SendWirelessOFBTMemoryLinkMode();
					m_pCtrl->SendCarPlayData(APP_SERVER_CP_ANIMEND,1);

					m_pCtrl->SendCarPlayData(APP_SERVER_CP_ACCSTATUS,tPacket.Buf[0]);
					m_pCtrl->SendMobileData(APP_MOBILENET_ACC_STATUS,tPacket.Buf[0]);
					m_pCtrl->SendAndroidautoData(APP_SERVER_AA_ACCSTATUS,tPacket.Buf[0]);
					m_pCtrl->SendReverseData(APP_SERVER_REVESER_ACCSTATUS,tPacket.Buf[0]);
					mData.SaveINTData(INT_MOBIL_ACC,tPacket.Buf[0]);
					int nSourceMemory = 0;
					mData.GetINTData(INT_MCU_SOURCE_MEMORY,nSourceMemory);
					LOGDBG("nSourceMemory = %d",nSourceMemory);
					switch(nSourceMemory)
					{
						case RESOERCE_TYPE_NO:
						case RESOERCE_TYPE_BT:
						{
							//msg.Param0  = UIID_WNDID_MAIN;
							if(nSourceMemory == RESOERCE_TYPE_BT)
							{
								mView.SendResourceType(RESOERCE_TYPE_BT);
							}
							m_pCtrl->SendMeterInfoData(RADIO_MUTE,0,0,0);
							break;
						}
						case RESOERCE_TYPE_RD:
						{
							//msg.Param0  = UIID_WNDID_RADIO;
							int nRadioPlayStatus = 0;
							mData.GetINTData(INT_RADIO_PLAY_STATUS, nRadioPlayStatus);
							LOGDBG("memory Radio Play Status = %d", nRadioPlayStatus);
							m_pCtrl->SendMeterInfoData(RADIO_MUTE,0,0,1 - nRadioPlayStatus);
							break;
						}
						case RESOERCE_TYPE_YL:
						{
							m_pCtrl->SendSystemData(SETAPP_SYSTEM_CONNECT_MODE,1);
							int nCarbitConnectStatus = 0;
							int nMirrorStatus = 0;
							mData.GetINTData(INT_MODILE_YL_CARBIT, nCarbitConnectStatus);
							mData.GetINTData(INT_MODILE_YL_MIRROR, nMirrorStatus);
							LOGDBG("nCarbitConnectStatus is %d",nCarbitConnectStatus);
							LOGDBG("nMirrorStatus is %d",nMirrorStatus);
							if(nCarbitConnectStatus == 4 && nMirrorStatus == 1)
							{	
								//msg.Param0 = UIID_WNDID_BLACK;			
								mView.SendResourceType(RESOERCE_TYPE_YL);	
							}
							else if(nCarbitConnectStatus == 4 && nMirrorStatus == 3)
							{
								//msg.Param0  = UIID_WNDID_APP;
								m_pCtrl->SendMobileData(APP_APPSERVICE_OPENYILIAN_TYPE,1);
							}
							else
							{
								//下发wifi信息生成二维码
								m_pCtrl->ReadWirelessData(WIRELESS_CMD_GET_WIFI_INFO);
								//mView.DrawAppConnectGroup(230,41);
								//msg.Param0  = UIID_WNDID_APP;												
							}
							m_pCtrl->SendMeterInfoData(RADIO_MUTE,0,0,0);
							break;
						}
						case RESOERCE_TYPE_AA:
						case RESOERCE_TYPE_CP:
						{
							m_pCtrl->SendSystemData(SETAPP_SYSTEM_CONNECT_MODE,0);
							//msg.Param0  = UIID_WNDID_APP;
							m_pCtrl->SendMeterInfoData(RADIO_MUTE,0,0,0);
							break;
						}
						default:
						{
							//msg.Param0  = UIID_WNDID_MAIN;
							m_pCtrl->SendMeterInfoData(RADIO_MUTE,0,0,0);
							break;
						}
					}
					bRet = true;
				}*/						
			}
			break;
		}
		case SYSTEMSRV_CMD_CONNECT_MODE:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("ConnectMode: nMode=%d",tPacket.Buf[0]);
				//2023-3-7 屏蔽互联设置
				//mData.SaveINTData(INT_MODILECONNECTMODE,tPacket.Buf[0]);	
				//msg.Message = SYS_VMSG_MODILE_CONNECTMODE;
				bRet = true;
			}
			break;			
		}
		case SYSTEMSRV_CMD_KEY_CTRL://0x24
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveKeyValue:nKeyType=%d,nKeyStatus=%d\n",tPacket.Buf[0], tPacket.Buf[1]);
				T_STR_KEY_INFO m_tKeyInfo;
				m_tKeyInfo.nKeyType = tPacket.Buf[0];
				m_tKeyInfo.nKeyStatus = tPacket.Buf[1];
				mData.SaveKeyValue(m_tKeyInfo);
				int nACCStatus = 1;
				mData.GetINTData(INT_ACCSTATUS,nACCStatus);
				LOGDBG("nACCStatus = %d",nACCStatus);
				if((m_tKeyInfo.nKeyType > 0) && (m_tKeyInfo.nKeyStatus > 0))
				{
					msg.Message = METER_VMSG_KEY_CHANGE;
					msg.Param0 = m_tKeyInfo.nKeyType;
					msg.Param1 = m_tKeyInfo.nKeyStatus;
					if(nACCStatus)
					{
						bRet = true;
					}
				}
			}
			break;
		}

		//4-3 收音机播放状态记忆
		/*
		case SYSTEMSRV_CMD_CARBIT_MODE://0x27
		{
			if(tPacket.nLen > 0)
			{
				int nMemWifiMode = tPacket.Buf[0];
				LOGDBG("memory wifi link mode = %d", nMemWifiMode);
				mData.SaveINTData(INT_WIFI_MEMORY_LINK_STATUS, nMemWifiMode);
				m_pCtrl->SendWirelessOFWifiMemoryLinkMode();
			}
			break;
		}*/
		case SYSTEMSRV_CMD_RADIO_STAUS:
		{
			if(tPacket.nLen > 0)
			{
				int nRadioPlayStatus = tPacket.Buf[0];
				LOGDBG("memory Radio Play Status = %d", nRadioPlayStatus);
				mData.SaveINTData(INT_RADIO_PLAY_STATUS, nRadioPlayStatus);
			}
			break;
		}
		case SYSTEMSRV_CMD_BTMUSIC_STAUS:
		{
			if(tPacket.nLen > 0)
			{
				int nBTMusicPlayStatus = tPacket.Buf[0];
				LOGDBG("memory BTMusic Play Status = %d", nBTMusicPlayStatus);
				mData.SaveINTData(INT_BTMUSIC_PLAY_STATUS, nBTMusicPlayStatus);
			}
			break;
		}
		case SYSTEMSRV_CMD_AUTO_WAR_TEST:
		{
			if(tPacket.nLen > 0)
			{
				//zhuanfa转发MeterInfo
				m_pCtrl->SendMeterInfoDataOFAutoWar(APP_METER_CMD_AUTO_WAR_TEST,tPacket.Buf[0],tPacket.Buf[1]);
			}
			break;
		}
		case SYSTEMSRV_CMD_AUTO_CLEAR_BT_LIST:
		{
			if(tPacket.nLen > 0)
			{
				//zhuanfa转发BT
				m_pCtrl->SendWirelessInfoData(WIRELESS_CMD_SET_AUTO_BT_PAIRLIST_EMPTY,tPacket.Buf[0]);
			}
			break;
		}
		case SYSTEMSRV_CMD_AUTO_SOURCE_CHANGE:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("Auto nValue=%d",tPacket.Buf[0]);
				if(tPacket.Buf[0] == 0)
				{
					msg.Message = METER_VMSG_ACCSTATUS;
					msg.Param0  = UIID_WNDID_RADIO;
					//下发收音机播放
					mView.SendResourceType(RESOERCE_TYPE_RD);
					m_pCtrl->InitAudio();
					m_pCtrl->SendMeterInfoData(RADIO_MUTE,0,0,1);
				}

				if(tPacket.Buf[0] == 1)
				{
					msg.Message = METER_VMSG_ACCSTATUS;
					msg.Param0  = UIID_WNDID_SETTING;
					//下发音乐播放
					mView.SendResourceType(RESOERCE_TYPE_BT);
					m_pCtrl->SendWirelessInfoData(WIRELESS_EVENT_BT_AUDIO_STATE,CMD_PLAY);
				}				
				bRet = true;
			}
			break;
		}
		case SYSTEMSRV_CMD_SOURCE_MEMORY:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("Source Memory: nMode=%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_MCU_SOURCE_MEMORY,tPacket.Buf[0]);
			}
			break;
		}
		case SYSTEMSRV_CMD_BT_MEMORY:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("BTLink Memory: nMode=%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_MCU_BTLINK_MEMORY,tPacket.Buf[0]);				
				m_pCtrl->SendWirelessOFBTMemoryLinkMode();
			}
			break;
		}
		case SYSTEMSRV_CMD_CONFIG_CHANGE:
		{
			m_pCtrl->SaveVendorDisPoseTime();
			msg.Message = SYS_VMSG_CONFIGE;
			bRet = true;
			break;
		}
		case SYSTEMSRV_CMD_LOCKSCREEN_STATUS:
		{
			/*T_STR_SCREEN_LOCK_INFO tScreen;
			if(tPacket.nLen > 0)
			{
				tScreen.nScreenLocknum = tPacket.Buf[0];
				tScreen.nScreenLockStatus = tPacket.Buf[1];
				mData.SaveScreenLockInfo(tScreen);
				LOGDBG("nScreenLocknum=%d, nScreenLockStatus=%d",tScreen.nScreenLocknum, tScreen.nScreenLockStatus);
				if(tScreen.nScreenLockStatus)
				{
					msg.Message = METER_VMSG_ACCSTATUS;
					msg.Param0  = UIID_WNDID_BLACK;
					bRet = true;
				}
			}*/
			break;
		} 
		case SYSTEMSRV_CMD_HOME_DTS_INFO:
		{
			T_DTS_EQ_SETTING tDtsEqTemp;
			memset(&tDtsEqTemp,0,sizeof(T_DTS_EQ_SETTING));

			tDtsEqTemp.nEq_mode = tPacket.Buf[0];
			tDtsEqTemp.nListen_Pos = tPacket.Buf[1];
			tDtsEqTemp.nSurround_Level = tPacket.Buf[2];
			tDtsEqTemp.nDeep_Level = tPacket.Buf[3];
			tDtsEqTemp.nEvelate_Level = tPacket.Buf[4];

			mData.SetDts_EQSetting(tDtsEqTemp);

			printf("&&&---SYSTEMSRV_CMD_HOME_DTS_INFO---nEq_mode:%d\n",tDtsEqTemp.nEq_mode);
			printf("&&&---SYSTEMSRV_CMD_HOME_DTS_INFO---nListen_Pos:%d\n",tDtsEqTemp.nListen_Pos);
			printf("&&&---SYSTEMSRV_CMD_HOME_DTS_INFO---nSurround_Level:%d\n",tDtsEqTemp.nSurround_Level);
			printf("&&&---SYSTEMSRV_CMD_HOME_DTS_INFO---nDeep_Level:%d\n",tDtsEqTemp.nDeep_Level);
			printf("&&&---SYSTEMSRV_CMD_HOME_DTS_INFO---nEvelate_Level:%d\n",tDtsEqTemp.nEvelate_Level);

			msg.Message = SYSTEM_VMSG_DTSEQ_SETTING;
			bRet = true;
			break;
		} 
		default:
		{
			break;
		}
	}

	return bRet;
}

bool HomeDataPacket::HandleWirelessSerData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	HomeData &mData = HomeData::GetInstance();
	switch(tPacket.nCmd)
	{
		case WIRELESS_EVENT_BT_CONNECTED_STATE://0X01
		{
			if(tPacket.nLen > 0)
			{
				int nBTLink = tPacket.Buf[0];
				LOGDBG("nBTLink = %d", nBTLink);
				/*/
				mData.SaveINTData(INT_BT_LINK_STATUS, nBTLink);
				msg.Message = METER_VMSG_BT_CONNECTED_STATE;

				//转发BT连接状态
				m_pCtrl->BackToBTLinkStatusToSys(nBTLink);
				if(nBTLink == 2)
				{
					m_pCtrl->SendWirelessOFBTMemoryBTMusicPlayStatus();
				}*/
				bRet = true;
			}
			break;
		}
		case WIRELESS_EVENT_BT_AUDIO_STATE:
		{
			if(tPacket.nLen > 0)
			{
				int nBTPlay = tPacket.Buf[0];
				LOGDBG("nBTPlay = %d", nBTPlay);
				mData.SaveINTData(INT_BT_PLAY_STATUS, nBTPlay);
				if(nBTPlay == AUDIO_STATUS_PLAY)
				{
					m_pCtrl->SendMeterInfoData(RADIO_MUTE,0,0,0);
				}
				msg.Message = METER_VMSG_BT_MUSICPLAY_STATE;
				bRet = true;
			}
			break;
		}
		
		case WIRELESS_EVENT_BT_MUSIC_INFO://0X29
		{
			if(tPacket.nLen > 0)
			{
				T_STR_BT_MUSICINFO *tBTMusic = (T_STR_BT_MUSICINFO*)tPacket.Buf;
				if(NULL != tBTMusic)
				{
					mData.SaveBTMusicInfo(tBTMusic);
					LOGDBG("abMusicTitle=%s",tBTMusic->abMusicTitle);
					msg.Message = METER_VMSG_BT_MUSIC_INFO;
					bRet = true;
				}
			}
			break;
		}
		case WIRELESS_EVENT_BT_BATTERY_CHANGE:
		{
			if(tPacket.nLen > 0)
			{
				int nPHBattery = tPacket.Buf[0];
				LOGDBG("nPHBattery =%d", nPHBattery);

				mData.SaveINTData(INT_PH_BATTERY_STATUS, nPHBattery);
				msg.Message = METER_VMSG_PH_BATTERY;
				bRet = true;
			}
			break;
		}
		case WIRELESS_EVENT_BT_SIGANL_CHANGE:
		{
			if(tPacket.nLen > 0)
			{
				int nPHSiganl = tPacket.Buf[0];
				LOGDBG("nPHSiganl =%d", nPHSiganl);

				mData.SaveINTData(INT_PH_SIGANL_STATUS, nPHSiganl);
				msg.Message = METER_VMSG_PH_SIGANL;
				bRet = true;
			}
			break;
		}
		//case WIRELESS_EVENT_WIFI_STRENGTH_STATE:
		case 0x8:
		{
			if(tPacket.nLen > 0)
			{
				int nWIFISiganl = tPacket.Buf[0];
				LOGDBG("nWIFISiganl =%d", nWIFISiganl);

				mData.SaveINTData(INT_WIFI_SIGANL_STATUS, nWIFISiganl);
				//msg.Message = METER_VMSG_WIFI_SIGANL;
				bRet = true;
			}
			break;
		}
		case WIRELESS_EVENT_BTCALL_STATE_CHANGE:
		{			
			if(tPacket.nLen > 0)
			{
				T_STR_BT_CALLERINFO *tBTCall = (T_STR_BT_CALLERINFO*)tPacket.Buf;

				if(NULL != tBTCall)
				{
					LOGDBG("PhoneNum =%s", tBTCall->PhoneNum);
					LOGDBG("Name =%s", tBTCall->Name);
					LOGDBG("nState =%d", tBTCall->nState);
					mData.SaveBTPhoneInfo(tBTCall);

					if(tBTCall->nState == 0)
					{
						//m_pCtrl->SendMeterInfoDataOFPhoneStatus(0);
						m_pCtrl->SendCarPlayDataOFBTPhoneStatus(0);
						mData.SaveINTData(INT_AA_PHONE, 0);
					}
					else
					{
						//m_pCtrl->SendMeterInfoDataOFPhoneStatus(1);
						m_pCtrl->SendCarPlayDataOFBTPhoneStatus(1);
						mData.SaveINTData(INT_AA_PHONE, 1);
					}
					msg.Message = METER_VMSG_BT_PHONE_INFO;
					bRet = true;
				}
			}
			break;
		}
		case WIRELESS_EVENT_BT_PHONE_MIC_MUTE:
		{
			if(tPacket.nLen > 0)
			{
				int nBTCallMute = tPacket.Buf[0];
				LOGDBG("nBTCallMute =%d", nBTCallMute);

				mData.SaveINTData(INT_BT_CALLMUTE, nBTCallMute);

				msg.Message = WIFI_VMSG_BT_PHONE_CALLMUTE;
				bRet = true;
			}			
			break;
		}
		case WIRELESS_EVENT_BT_PHONE_CALL_MODE:
		{
			if(tPacket.nLen > 0)
			{
				int nBTCallMode = tPacket.Buf[0];
				LOGDBG("nBTCallMode =%d", nBTCallMode);

				mData.SaveINTData(INT_BT_CALLMODE, nBTCallMode);
				msg.Message = WIFI_VMSG_BT_PHONE_CALLMODE;
				bRet = true;
			}			
			break;
		}
		case WIRELESS_EVENT_WIFI_INFO_CHANGE://0x23
		{
			T_STR_WIFI_INFO *tWifiInfo = (T_STR_WIFI_INFO*)tPacket.Buf;
			if(NULL != tWifiInfo)
			{
				mData.SaveWifiInfo(tWifiInfo);
				LOGDBG("Name=%s, ifName=%s, password=%s, hwMode=%s, security=%s, macAddr=%s, channel=%d, mode=%d, freq=%d",\
						tWifiInfo->Name, tWifiInfo->ifName, tWifiInfo->password, tWifiInfo->hwMode, tWifiInfo->security,\
						tWifiInfo->macAddr, tWifiInfo->channel, tWifiInfo->mode, tWifiInfo->freq);
				msg.Message = YL_VMSG_QRCODE_STATUS;
				m_pCtrl->SetWifiInfoToMobile(tPacket.nLen, tPacket.Buf);
				//转发Wifiname SYS
				m_pCtrl->BackWifiNameToSys(tWifiInfo->Name);
				bRet = true;
			}

			break;
		}
		case WIRELESS_EVENT_WIFI_STATE_CHANGE:
		{
			if(tPacket.nLen > 0)
			{
				int nWifiStatus = tPacket.Buf[0];
				LOGDBG("nWifiStatus =%d", nWifiStatus);

				mData.SaveINTData(INT_WIFI_STATUS, nWifiStatus);
				msg.Message = WIFI_VMSG_WIFI_CONNECTSTATUS;
				bRet = true;
				if(NULL != m_pCtrl)
				{
					m_pCtrl->SetWifiStatusToMobileNet(nWifiStatus);
				}
			}
			break;	
		}
		case WIRELESS_EVENT_BT_PAIRING_REQ_INFO:
		{
			T_BtPAIRINGREQ *tBtPairingReq = (T_BtPAIRINGREQ*)tPacket.Buf;
			if(NULL != tBtPairingReq)
			{
				mData.SaveBtPairingReq(tBtPairingReq);
				LOGDBG("Name=%s, state=%d",tBtPairingReq->name, tBtPairingReq->state);
				msg.Message = WIFI_VMSG_BTPAIRINGREQ;
				bRet = true;
			}

			break;
		}		
		case WIRELESS_EVENT_AUTO_BT_PAIRLIST_EMPTY:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("BT_PAIRLIST = %d",tPacket.Buf[0]);
				m_pCtrl->SendSystemData(APP_EVENT_AUTO_BT_PAIRLIST_EMPTY,tPacket.Buf[0]);
			}
			break;	
		}
		case WIRELESS_EVENT_BT_AUDIO_PLAY_STATE:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("MemoryBTMusicPlayStatus = %d",tPacket.Buf[0]);
				m_pCtrl->SetBTMusicMemoryPlayStatus(tPacket.Buf[0]);
			}
			break;	
		}
		default:
		{
			break;
		} 
	}

	return bRet;
}

bool HomeDataPacket::HandleMobileNetData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	HomeData &mData = HomeData::GetInstance();
	switch(tPacket.nCmd)
	{
		case MOBILENET_APP_EVENT_TIME://0x14
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveMobilenetTimeData:nMode=%d,nHours=%d,nMinutes=%d,seconds=%d",\
								tPacket.Buf[0], tPacket.Buf[1], tPacket.Buf[2], tPacket.Buf[3]);
				if(NULL != m_pCtrl)
				{
					// m_pCtrl->SetTimeInfo(tPacket.Buf[1], tPacket.Buf[2], tPacket.Buf[3]);
				}
				bRet = true;
			}
			break;
		}
		case MOBILENET_APP_RESOURCERELEASE:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveMobilenetResourceRelease:%d",tPacket.Buf[0]);
				if(NULL != m_pCtrl && tPacket.Buf[0] == 1)
				{
					int nCP = 0;
					mData.GetINTData(INT_MODILERESOURCE_CP, nCP);
					int nAA = 0;
					mData.GetINTData(INT_MODILERESOURCE_AA, nAA);
					LOGDBG("nCP = %d",nCP);
					LOGDBG("nAAStatus = %d",nAA);

					if(nCP)
					{
						m_pCtrl->SendCarPlayData(APP_SERVER_MOBILE_RESOURCE_LOAD,1);
					}
					if(nAA)
					{
						m_pCtrl->SendAndroidautoData(APP_SERVER_MOBILE_RESOURCE_LOAD,1);
					}

					mData.SaveINTData(INT_MODILERESOURCE_YL, 0);					
					bRet = true;
				}
			}
			break;			
		}
		case MOBILENET_APP_RESOURCELOAD:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveMobilenetResourceLoad:%d",tPacket.Buf[0]);
				if(tPacket.Buf[0] == 1)
				{
					//加载1 释放0
					mData.SaveINTData(INT_MODILERESOURCE_YL, 1);
				}
				msg.Message = MOBILE_VMSG_MODILERESOURCE_YL;
				bRet = true;				
			}
			break;			
		}
		case MOBILENET_APP_EVENT_MIRROR:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveMobilenetMirror:%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_MODILE_YL_MIRROR, tPacket.Buf[0]);
				msg.Message = YL_VMSG_CONNECTSTATUS;
				bRet = true;				
			}
			break;
		}
		case MOBILENET_APP_EVENT_CARBIT:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveMobilenetCarbit:%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_MODILE_YL_CARBIT, tPacket.Buf[0]);
				msg.Message = YL_VMSG_CONNECTSTATUS;
				bRet = true;				
			}
			break;
		}
		case MOBILENET_APP_QRCODE_INFO:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("QRCode:%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_MODILE_YL_QRCode,tPacket.Buf[0]);	
				msg.Message = YL_VMSG_QRCODE_STATUS;
				bRet = true;
			}
			break;			
		}
		default:
		{
			break;
		}
	}

	return bRet;
}

bool HomeDataPacket::HandleCarPlayData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	HomeData &mData = HomeData::GetInstance();
	IVIView &mView = IVIView::GetInstance();
	switch(tPacket.nCmd)
	{
		case CARPLAY_CMD_SHOW://0x14
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveCarPlayData:%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_CARPLAYSHOWSTATUS, tPacket.Buf[0]);
				int nAPPStatus = -1;
				mData.GetINTData(INT_APPSTATUS, nAPPStatus);
				int nResourceType = mData.GetResourceType();

				if(tPacket.Buf[0] == 0)   //CP后台显示
				{	
					/*if(nAPPStatus == APP_STATUS_CP)
					{
						msg.Param0 = UIID_WNDID_APP;
						mData.SaveINTData(INT_APPSTATUS, APP_STATUS_NO);
						msg.Message = METER_VMSG_ACCSTATUS;	
					}*/

					msg.Param0 = UIID_WNDID_MAIN;
					msg.Message = METER_VMSG_ACCSTATUS;
		
				}
				else if(tPacket.Buf[0] == 1)  //CP前台显示
				{
					msg.Param0 = UIID_WNDID_BLACK;
					mData.SaveINTData(INT_APPSTATUS, APP_STATUS_CP);
					msg.Message = METER_VMSG_ACCSTATUS;
					mView.SendResourceType(RESOERCE_TYPE_CP);
					//下发收音机静音
					m_pCtrl->SendMeterInfoData(RADIO_MUTE,0,0,0);	
					MarkCPLinkValid("CP_SHOW");
				}
				
				int nACCStutas = 1;
				mData.GetINTData(INT_ACCSTATUS,nACCStutas);
				LOGDBG("nACCStutas:%d",nACCStutas);
				if(nACCStutas)
				{
					bRet = true;
				}

				bRet = true;
			}
			break;
		}
		case APP_SERVER_CP_SPLITSCREEN:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("CarPlay SplitScreen:%d", tPacket.Buf[0]);
				mData.SaveINTData(INT_CP_SPLITSTATUS, tPacket.Buf[0]);
				mView.OnSplitScreenChange(tPacket.Buf[0], APP_STATUS_CP);
				bRet = true;
			}
			break;
		}
		case CARPLAY_CMD_CONNECT:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveCarPlayConnectStatus:%d",tPacket.Buf[0]);

				int nPrevCPValid = 0;
				mData.GetINTData(INT_CP_LINK_VALID, nPrevCPValid);
				if (1 == tPacket.Buf[0])
				{
					if (2 == nPrevCPValid)      // 正常时序：先 SHOW/PAAS，后 CONNECT
					{
						mData.SaveINTData(INT_CP_LINK_VALID, 1);
						LOGDBG("CPLinkValid:1 by pending");
					}
				}
				else
				{
					mData.SaveINTData(INT_CP_LINK_VALID, 0);
					LOGDBG("CPLinkValid:0 reset, raw=%d", tPacket.Buf[0]);
				}

				mData.SaveINTData(INT_CARPLAYCONNECTSTATUS, tPacket.Buf[0]);
				if(tPacket.Buf[0] != 1)
				{
					mData.SaveINTData(INT_CP_PHONE,0);
					m_pCtrl->SendWirelessInfoData(WIRELESS_CMD_SET_CARPLAY_ACTIVE_STATE,0);
				}
				m_pCtrl->SendAAOFCPConnectStatus(tPacket.Buf[0]);	
				msg.Message = CARPLAY_VMSG_CONNECTSTATUS;
				bRet = true;
			}			
			break;
		}
				
		case CARPLAY_CMD_PAAS_START:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveCarPlayResourceLoad:%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_MODILERESOURCE_LOAD_CP, tPacket.Buf[0]);
				if(tPacket.Buf[0] == 1)
				{
					//加载0 释放1
					mData.SaveINTData(INT_MODILERESOURCE_CP,0);
					MarkCPLinkValid("CP_PAAS_START");
				}
				msg.Message = MOBILE_VMSG_MODILERESOURCE_LOAD_CPAA;	
				bRet = true;
			}			
			break;			
		}
		case  CARPLAY_CMD_PAAS_SHUTDOWN:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveCarPlayResourceRelease:%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_MODILERESOURCE_RELEASE_CP, tPacket.Buf[0]);
				if(tPacket.Buf[0] == 1)
				{
					mData.SaveINTData(INT_MODILERESOURCE_CP,1);
				}
				msg.Message = MOBILE_VMSG_MODILERESOURCE_RELEASE_CPAA;
				bRet = true;
			}
			break;
		}
		//CP弹窗
		case CARPLAY_CMD_SHOW_WIRELESS_CONFIRM:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveCarPlayConfirm:%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_CARPLAY_CONFIRM, tPacket.Buf[0]);
				msg.Message = CARPLAY_VMSG_CONFIRM;
				bRet = true;
			}			
			break;
		}
		case CARPLAY_CMD_MODE:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("CarplayMode:scree:%d",tPacket.Buf[0]);
				LOGDBG("CarplayMode:audio:%d",tPacket.Buf[1]);
				LOGDBG("CarplayMode:Phone:%d",tPacket.Buf[2]);
				LOGDBG("CarplayMode:speech:%d",tPacket.Buf[3]);
				LOGDBG("CarplayMode:speechMode:%d",tPacket.Buf[4]);
				LOGDBG("CarplayMode:navi:%d",tPacket.Buf[5]);
				mData.SaveINTData(INT_CP_PHONE, tPacket.Buf[2]);
				MarkCPLinkValid("CP_MODE");
				if(tPacket.Buf[1] == 1)
				{
					mView.SendResourceType(RESOERCE_TYPE_CP);
				}
				msg.Message = CARPLAY_VMSG_PHONE_STATUS;
				bRet = true;
			}
			break;
		}
		case CARPLAY_CMD_CONNECT_READY:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("CarplayMode:connect_ReAdy:%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_CP_CONNECT_REDAY_STATUS, tPacket.Buf[0]);
				if (1 == tPacket.Buf[0])
				{
					MarkCPLinkValid("CP_READY");
				}
				bRet = true;
			}
			msg.Message = METER_VMSG_LASTCHARGE_INFO;
			break;
		}
		case CARPLAY_CMD_DISABLE_BLUETOOTH:
		{
			if(tPacket.nLen > 0)
			{
				m_pCtrl->SendWirelessInfoData(WIRELESS_CMD_SET_CARPLAY_ACTIVE_STATE,tPacket.Buf[0]);
				LOGDBG("CARPLAY_CMD_DISABLE_BLUETOOTH %d",tPacket.Buf[0]);
			}
			break;
		}
		case CARPLAY_CMD_CTRCONNECT_CUR_BT:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("CARPLAY_CMD_RECONNECT_CUR_BT %d",tPacket.Buf[0]);
				BT_PAIRED_LIST_T nPairedlist;
				SettingData &mSettingData = SettingData::GetInstance();
				mSettingData.GetPairedlist(nPairedlist);
				LOGDBG("nPairedlist.connState[0] %d",nPairedlist.connState[0]);
				if(1 == tPacket.Buf[0])
				{
					if(nPairedlist.connState[0] != 2)
					{
						m_pCtrl->SendWirelessInfoData(WIRELESS_CMD_SET_INITIATIVE_PAIRING,0,0);
					}
				}

				if(0 == tPacket.Buf[0])
				{
					if(nPairedlist.connState[0] == 2)
					{
						m_pCtrl->SendWirelessInfoData(WIRELESS_CMD_SET_INITIATIVE_PAIRING,1,0);
					}
				}			
			}
			break;
		}
		default:
		{
			break;
		}
	}

	return bRet;
}

bool HomeDataPacket::HandleAndroidData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	HomeData &mData = HomeData::GetInstance();
	IVIView &mView = IVIView::GetInstance();
	/*if(mData.GetAAOpenStatus() == 0)
	{
		LOGDBG("NO AA");
		return bRet;
	}*/
	switch(tPacket.nCmd)
	{
		case ANDROIDAUTO_CMD_SHOW://0x14
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveAndriodData:%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_AASHOWSTATUS, tPacket.Buf[0]);
				int nAPPStatus = -1;
				mData.GetINTData(INT_APPSTATUS, nAPPStatus);
				int nResourceType = mData.GetResourceType();
				if(tPacket.Buf[0] == 0)
				{	
					/*if(nAPPStatus == APP_STATUS_AA)
					{
						msg.Param0 = UIID_WNDID_APP;
						mData.SaveINTData(INT_APPSTATUS, APP_STATUS_NO);
						msg.Message = METER_VMSG_ACCSTATUS;	
					}*/
					msg.Param0 = UIID_WNDID_MAIN;
					msg.Message = METER_VMSG_ACCSTATUS;						
				}
				else if(tPacket.Buf[0] == 1)
				{
					msg.Param0 = UIID_WNDID_BLACK;
					mData.SaveINTData(INT_APPSTATUS, APP_STATUS_AA);
					msg.Message = METER_VMSG_ACCSTATUS;
					mView.SendResourceType(RESOERCE_TYPE_AA);
					//下发收音机静音
					m_pCtrl->SendMeterInfoData(RADIO_MUTE,0,0,0);	
					MarkAALinkValid("AA_SHOW");
				}

				int nACCStutas = 1;
				mData.GetINTData(INT_ACCSTATUS,nACCStutas);
				LOGDBG("nACCStutas:%d",nACCStutas);
				if(nACCStutas)
				{
					bRet = true;
				}
				bRet = true;
			}
			break;
		}
		case APP_SERVER_AA_SPLITSCREEN:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("AndroidAuto SplitScreen:%d", tPacket.Buf[0]);
				mData.SaveINTData(INT_AA_SPLITSTATUS, tPacket.Buf[0]);
				mView.OnSplitScreenChange(tPacket.Buf[0], APP_STATUS_AA);
				bRet = true;
			}
			break;
		}
		case ANDROIDAUTO_CMD_CONNECT:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveAndriodStatus:%d",tPacket.Buf[0]);

				int nPrevAAValid = 0;
				mData.GetINTData(INT_AA_LINK_VALID, nPrevAAValid);
				if (1 == tPacket.Buf[0])
				{
					if (2 == nPrevAAValid)      // 正常时序：先 AA_SHOW/PAAS，后 CONNECT
					{
						mData.SaveINTData(INT_AA_LINK_VALID, 1);
						LOGDBG("AALinkValid:1 by pending");
					}
				}
				else
				{
					mData.SaveINTData(INT_AA_LINK_VALID, 0);
					LOGDBG("AALinkValid:0 reset, raw=%d", tPacket.Buf[0]);
				}

				mData.SaveINTData(INT_ANDRIODCONNECTSTATUS, tPacket.Buf[0]);
				msg.Message = ANDRIOD_VMSG_CONNECTSTATUS;
				m_pCtrl->SendWirelessInfoData(WIRELESS_CMD_SET_AA_ACTIVE_STATE,tPacket.Buf[0]);
				m_pCtrl->SendCPOFAAConnectStatus(tPacket.Buf[0]);	
				bRet = true;
			}			
			break;
		}				
		case ANDROIDAUTO_CMD_PAAS_START:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveAndriodResourceLoad:%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_MODILERESOURCE_LOAD_AA, tPacket.Buf[0]);
				if(tPacket.Buf[0] == 1)
				{
					//加载0 释放1
					mData.SaveINTData(INT_MODILERESOURCE_AA,0);
					MarkAALinkValid("AA_PAAS_START");
				}
				msg.Message = MOBILE_VMSG_MODILERESOURCE_LOAD_CPAA;	
				bRet = true;
			}			
			break;			
		}
		case ANDROIDAUTO_CMD_PAAS_SHUTDOWN:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveAndriodResourceRelease:%d",tPacket.Buf[0]);
				mData.SaveINTData(INT_MODILERESOURCE_RELEASE_AA, tPacket.Buf[0]);
				if(tPacket.Buf[0] == 1)
				{
					mData.SaveINTData(INT_MODILERESOURCE_AA,1);
				}
				msg.Message = MOBILE_VMSG_MODILERESOURCE_RELEASE_CPAA;
				bRet = true;
			}
			break;
		}
		case ANDROIDAUTO_CMD_BT_ADDRESS:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveAndriodBTAddress:%d %x",*tPacket.Buf,*tPacket.Buf);
				LOGDBG("SaveAndriodBTAddress:%d %x",tPacket.Buf[0],tPacket.Buf[0]);
				LOGDBG("SaveAndriodBTAddress:%d %x",tPacket.Buf[1],tPacket.Buf[1]);
				LOGDBG("SaveAndriodBTAddress:%d %x",tPacket.Buf[2],tPacket.Buf[2]);
				LOGDBG("SaveAndriodBTAddress:%d %x",tPacket.Buf[3],tPacket.Buf[3]);
				LOGDBG("SaveAndriodBTAddress:%d %x",tPacket.Buf[4],tPacket.Buf[4]);
				LOGDBG("SaveAndriodBTAddress:%d %x",tPacket.Buf[5],tPacket.Buf[5]);
			}			
			break;
		}
		case ANDROIDAUTO_CMD_BT_PHONECALL_STATUS:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveAndriodPhone:%d",tPacket.Buf[0]);
				T_STR_BT_CALLERINFO tPhone;
				mData.GetBTPhoneInfo(tPhone);
				if(tPacket.Buf[0] == 1 || tPhone.nState != 0)
				{
					mData.SaveINTData(INT_AA_PHONE, 1);
					LOGDBG("SaveAndriodPhone:%d",1);
				}
				else
				{
					mData.SaveINTData(INT_AA_PHONE, 0);
					LOGDBG("SaveAndriodPhone:%d",0);
				}
				msg.Message = ANDRIOD_VMSG_PHONE_STATUS;
				bRet = true;
			}
			break;
		}
		case ANDROIDAUTO_CMD_AUDIO_SOURCE_STATUS:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("SaveAndriodAudio:%d",tPacket.Buf[0]);
				if(tPacket.Buf[0] == 1)
				{
					mView.SendResourceType(RESOERCE_TYPE_AA);
					MarkAALinkValid("AA_AUDIO_SOURCE");
				}
			}
			break;	
		}
		default:
		{
			break;
		}
	}

	return bRet;
}


bool HomeDataPacket::HandleReversesrvData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	HomeData &mData = HomeData::GetInstance();
	IVIView &mView = IVIView::GetInstance();
	switch(tPacket.nCmd)
	{
		case REVERSE_CMD_CAMERA:
		{
			static int Old_Revers = -1;
			if(tPacket.nLen > 0 && tPacket.Buf[0] != Old_Revers)
			{
				LOGDBG("SaveReverseData:%d",tPacket.Buf[0]);
				int nLasWnd = mData.GetLastWnd();
				LOGDBG("SaveReverseData:nLasWnd %d",nLasWnd);

				int nACCStutas = 1;
				mData.GetINTData(INT_ACCSTATUS,nACCStutas);
				LOGDBG("nACCStutas:%d",nACCStutas);
				if(tPacket.Buf[0] == 0)
				{	
					if(nLasWnd == UIID_WNDID_BLACK && nACCStutas == 1)
					{
						msg.Param0 = UIID_WNDID_APP;
					}
					else if(nACCStutas == 1)
					{
						msg.Param0 = nLasWnd;
						if(nLasWnd == 12)
						{
							T_STR_BT_CALLERINFO tPhone;
							mData.GetBTPhoneInfo(tPhone);
							LOGDBG("tPhone.nState:%d",tPhone.nState);
							if(tPhone.nState == 0)
							{
								msg.Param0 = UIID_WNDID_APP;
							}
						}
					}
					msg.Message = METER_VMSG_ACCSTATUS;
					mData.SaveINTData(INT_REVERSE, tPacket.Buf[0]);		
					bRet = true;	
				}
				else if(tPacket.Buf[0] == 1 && nACCStutas == 1)
				{
					msg.Param0 = UIID_WNDID_BLACK;
					msg.Message = METER_VMSG_ACCSTATUS;
					mData.SaveINTData(INT_REVERSE, tPacket.Buf[0]);
					bRet = true;
				}
				else
				{
					mData.SaveINTData(INT_REVERSE, 0);
					msg.Message = REVERSE_VMSG_BACKSTATUS;
					bRet = true;
				}
				Old_Revers = tPacket.Buf[0];				
			}
			break;
		}
		case REVERSE_CMD_SET_BACKGROUND:
		{
			if(tPacket.nLen > 0)
			{
				int BackPath = 0;
				memcpy(&BackPath, tPacket.Buf, tPacket.nLen);
				LOGDBG("SaveReverseDataPath:%d",BackPath);
				mData.SaveBackPath(BackPath);
				msg.Message = REVERSE_VMSG_BACKSTATUS;
				bRet = true;				
			}
			break;
		}
		default:
		{
			break;
		}
	}

	return bRet;
}
/**
 * 功能：分发CanBox数据
*/
bool HomeDataPacket::HandleCanBoxData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	HomeData &mData = HomeData::GetInstance();
	IVIView &mView = IVIView::GetInstance();

	for(int i = 0;i<tPacket.nLen;i++)
	{
		LOGDBG("&&&&---cmd:0x%x--buf[%d]:0x%x ,nParam1[0x%x]",tPacket.nCmd,i,tPacket.Buf[i], tPacket.nParam1);
	}
	
	// 调用解析MCU数据（此时动态库未卸载，pDb 有效）
    mView.pCanBoxProto->Rev_ParseData(tPacket.nParam1,tPacket.Buf,tPacket.nLen);

	// switch(tPacket.nCmd)
	// {
	// 	case CANBOX_CAR_INFOMATION:
	// 	{
	// 		T_CANBOX_CAR_DATA tCarInfoData;
	// 		tCarInfoData.car_park = (tPacket.Buf[0] & 0x08) ? 1 : 0;
	// 		tCarInfoData.car_rev = (tPacket.Buf[0] & 0x04) ? 1 : 0;
	// 		tCarInfoData.car_key = tPacket.Buf[2];
	// 		tCarInfoData.wheel_angle = MAKEWORD(tPacket.Buf[7], tPacket.Buf[6]);

	// 		mData.SetCanBox_CarInfo(tCarInfoData);
	// 		msg.Message = CANBOX_VMSG_CARDATA_INFO;
	// 		bRet = true;
	// 	}
	// 	break;
	// 	case CANBOX_AIR_INFOMATION:
	// 	{
	// 		T_CANBOX_AIR_DATA tAirData;
	// 		tAirData.air_mem = (tPacket.Buf[0] & 0x80) ? 1 : 0;
	// 		tAirData.air_switch = (tPacket.Buf[0] & 0x40) ? 1 : 0;
	// 		tAirData.air_ac_max = (tPacket.Buf[0] & 0x20) ? 1 : 0;
	// 		tAirData.air_rear_switch = (tPacket.Buf[0] & 0x10) ? 1 : 0;
	// 		tAirData.air_auto = (tPacket.Buf[0] & 0x08) ? 1 : 0;
	// 		tAirData.air_sync = (tPacket.Buf[0] & 0x04) ? 1 : 0;
	// 		tAirData.air_Frt_air = (tPacket.Buf[0] & 0x02) ? 1 : 0;
	// 		tAirData.air_unit = tPacket.Buf[0] & 0x01;

	// 		tAirData.air_rearkey = (tPacket.Buf[1] & 0x80) ? 1 : 0;
	// 		tAirData.air_ac = (tPacket.Buf[1] & 0x40) ? 1 : 0;
	// 		tAirData.air_airdetect = (tPacket.Buf[1] & 0x20) ? 1 : 0;
	// 		tAirData.air_inout_set = (tPacket.Buf[1] & 0x10) ? 1 : 0;
	// 		tAirData.air_inout_auto = (tPacket.Buf[1] & 0x08) ? 1 : 0;
	// 		tAirData.air_dual = (tPacket.Buf[1] & 0x04) ? 1 : 0;
	// 		tAirData.air_eco = (tPacket.Buf[1] & 0x02) ? 1 : 0;
	// 		tAirData.air_airok = (tPacket.Buf[1] & 0x01) ? 1 : 0;

	// 		tAirData.rear_air_auto = (tPacket.Buf[2] & 0x80) ? 1 : 0;
	// 		tAirData.auto_remove = (tPacket.Buf[2] & 0x40) ? 1 : 0;
	// 		tAirData.rear_remove = (tPacket.Buf[2] & 0x20) ? 1 : 0;
	// 		tAirData.front_remove = (tPacket.Buf[2] & 0x10) ? 1 : 0;
	// 		tAirData.front_right_hot = (tPacket.Buf[2] & 0x0c) >> 2;
	// 		tAirData.front_left_hot = tPacket.Buf[2] & 0x03;

	// 		tAirData.front_right_air = (tPacket.Buf[3] & 0xC0) >> 6;
	// 		tAirData.front_left_air = (tPacket.Buf[3] & 0x30) >> 4;
	// 		tAirData.right_win_hot = (tPacket.Buf[3] & 0x08) ? 1 : 0;
	// 		tAirData.air_maxheat = (tPacket.Buf[3] & 0x04) ? 1 : 0;
	// 		tAirData.auto_detect = (tPacket.Buf[3] & 0x02) ? 1 : 0;

	// 		tAirData.air_mode = tPacket.Buf[4];
	// 		tAirData.air_speed = tPacket.Buf[5];
	// 		tAirData.air_left_temp = tPacket.Buf[6];
	// 		tAirData.air_right_temp = tPacket.Buf[7];
	// 		tAirData.rear_air_mode = tPacket.Buf[8];
	// 		tAirData.rear_airspeed = tPacket.Buf[9];
	// 		tAirData.rear_set_temp = tPacket.Buf[10];
	// 		tAirData.exter_temp = tPacket.Buf[11];

	// 		mData.SetCanBox_AirData(tAirData);
	// 		msg.Message = CANBOX_VMSG_AIRDATA_INFO;
	// 		bRet = true;
	// 	}
	// 	break;
	// 	case CANBOX_AIR_SETTING:
	// 	{
	// 		T_CANBOX_CAR_SETTING tCarSetting;
	// 		tCarSetting.turn_right = (tPacket.Buf[1] & 0x80) ? 1 : 0;
	// 		tCarSetting.turn_left = (tPacket.Buf[1] & 0x40) ? 1 : 0;

	// 		mData.SetCanBox_CarSetting(tCarSetting);
	// 		msg.Message = CANBOX_VMSG_CARDATA_SETTING;
	// 	}
	// 	break;
	// 	default:
	// 	{
	// 		break;
	// 	}
	// }

	return bRet;
}

/* ===== 互联连接有效性：只有收到会话级证据才认定“真连接” ===== */
void HomeDataPacket::MarkCPLinkValid(const char* pWhy)
{
	HomeData &mData = HomeData::GetInstance();
	int nCP = 0;
	mData.GetINTData(INT_CARPLAYCONNECTSTATUS, nCP);
	if (1 == nCP)
	{
		mData.SaveINTData(INT_CP_LINK_VALID, 1);
		LOGDBG("CPLinkValid:1 by %s", pWhy);
	}
	else
	{
		mData.SaveINTData(INT_CP_LINK_VALID, 2);   // 证据先到，等 CONNECT=1 来确认
		LOGDBG("CPLinkValid:2 pending by %s (cp=%d)", pWhy, nCP);
	}
}

void HomeDataPacket::MarkAALinkValid(const char* pWhy)
{
	HomeData &mData = HomeData::GetInstance();
	int nAA = 0;
	mData.GetINTData(INT_ANDRIODCONNECTSTATUS, nAA);
	if (1 == nAA)
	{
		mData.SaveINTData(INT_AA_LINK_VALID, 1);
		LOGDBG("AALinkValid:1 by %s", pWhy);
	}
	else
	{
		mData.SaveINTData(INT_AA_LINK_VALID, 2);   // 证据先到，等 CONNECT=1 来确认
		LOGDBG("AALinkValid:2 pending by %s (aa=%d)", pWhy, nAA);
	}
}
