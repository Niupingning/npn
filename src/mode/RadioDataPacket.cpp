/*
 * HomeDataPacket.cpp
 *
 *  Created on: 2021-08-18
 *      Author: Y3553
 */

#include "RadioDataPacket.h"
#include "IVIController.h"
#include "RadioData.h"
#include "IVIView.h"
#include "SourceDef.h"

RadioDataPacket::RadioDataPacket(IVIController *pCtrl)
{
	m_pCtrl = pCtrl;
	pthread_mutex_init (&m_Lock, NULL);

	m_bExitFlag = false;

	//SetToRunning(true);
}

RadioDataPacket::~RadioDataPacket()
{
	pthread_mutex_destroy( &m_Lock );
}

bool RadioDataPacket::Init()
{
	bool nRet = false;
	nRet = ThreadControl::Init();
	nRet = nRet & ThreadIsReady();
	DoIt();
	return nRet;
}

void RadioDataPacket::DoIt()
{
	ThreadControl::DoIt();
}

bool RadioDataPacket::ThreadIsReady()
{
	return ThreadControl::ThreadIsReady();
}

void RadioDataPacket::Close()
{
	ThreadControl::Close();
}

void RadioDataPacket::DealWithTask()
{
	static bool bFlag = true;
	if(bFlag)
	{
		bFlag = false;
		int ret = pthread_setname_np(pthread_self(), "RadioDataPacket");
		if( ret != EOK )
		{
			bFlag = true;
			LOGDBG(" DataPacket pthread_setname_np failed!");
		}
	}

	while (!m_bExitFlag)
	{
		RadioData &mData = RadioData::GetInstance();
		mData.WaitForPacket();
		if(mData.GetVecPacket_Size() > 0)
		{
			memset(&m_tDataPacket, 0, sizeof(T_STR_DATA_PACKET));
			mData.GetPopPacket(m_tDataPacket);
			ParseDataPacket(m_tDataPacket);
		}
	}
}

void RadioDataPacket::ParseDataPacket(T_STR_DATA_PACKET tPacket)
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
		default:
		{
			break;
		}
	}
	if(bRet && (0 != nMsg.Message))
	{
		LOGDBG("RadioApp:SendViewMessage:Message=%d,Param0=%d,Param1=%d,Param2=%d\n",\
									nMsg.Message, nMsg.Param0, nMsg.Param1, nMsg.Param2 );
		IVIView &mView = IVIView::GetInstance();
		mView.SendViewMessage(&nMsg);
	}
}

bool RadioDataPacket::HandleMeterInfoData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	RadioData &mData = RadioData::GetInstance();
	switch(tPacket.nCmd)
	{
		case METERINFO_APP_REPORT_MESSAGE://0x17 = 23
		{
			if(tPacket.nLen > 0)
			{
	            LOGDBG("!!! - 0000 - Handle:tPacket.nCmd=%d,tPacket.nLen=%d,tPacket.Buf[10]=%d\n",\
									tPacket.nCmd,tPacket.nLen,tPacket.Buf[10]);
									
	            LOGDBG("!!! - 0001 - Handle:tPacket - tPacket.Buf[0] = %d, tPacket.Buf[1] = %d, tPacket.Buf[2] = %d, tPacket.Buf[3] = %d, tPacket.Buf[4] = %d\n",
									                  tPacket.Buf[0], tPacket.Buf[1], tPacket.Buf[2], tPacket.Buf[3], tPacket.Buf[4]);
	            LOGDBG("!!! - 0002 - Handle:tPacket - tPacket.Buf[5] = %d, tPacket.Buf[6] = %d, tPacket.Buf[7] = %d, tPacket.Buf[8] = %d, tPacket.Buf[9] = %d\n",
									                  tPacket.Buf[5], tPacket.Buf[6], tPacket.Buf[7], tPacket.Buf[8], tPacket.Buf[9]);				
                T_RADIO_CURR_FREQ tCurrFreq;
				tCurrFreq.nSignalStrength    = tPacket.Buf[0];
				tCurrFreq.nCurrFreq          = MAKEWORD(tPacket.Buf[2], tPacket.Buf[1]);
				tCurrFreq.nRadioFlagLOCAL    = tPacket.Buf[3];
				tCurrFreq.nRadioStatus       = tPacket.Buf[4];
				tCurrFreq.nStereoMono        = tPacket.Buf[5];
				tCurrFreq.nValid             = tPacket.Buf[6];
				tCurrFreq.nCurrBand          = tPacket.Buf[7];
				tCurrFreq.nRadioArea         = tPacket.Buf[8];
				tCurrFreq.nCurrMute          = tPacket.Buf[9];
				tCurrFreq.nWorkStatus        = tPacket.Buf[10];
			    LOGDBG("!!! - 0003 - 01 - nCurrFreq = %d, nCurrBand = %d, nCurrMute = %d, nWorkStatus = %d\n", tCurrFreq.nCurrFreq, tCurrFreq.nCurrBand, tCurrFreq.nCurrMute,tCurrFreq.nWorkStatus);
				mData.SaveCurrFreq(tCurrFreq);
				msg.Message = METER_VMSG_UPDATE_RADIOINFO;
				msg.Param0 = tCurrFreq.nCurrFreq;
				msg.Param1 = tCurrFreq.nCurrBand;
				msg.Param2 = tCurrFreq.nCurrMute;
				bRet = true;
			}
			break;
		}
		case METERINFO_APP_RDS_PARAM_REPLY:
		{
			if(tPacket.nLen > 0)
			{
	            LOGDBG("!!! - 0000 - Handle:tPacket.nCmd=%d,tPacket.nLen=%d,tPacket.Buf[0]=%d\n",\
									tPacket.nCmd,tPacket.nLen,tPacket.Buf[0]);
									
	            LOGDBG("!!! - 0001 - Handle:tPacket - tPacket.Buf[0] = %d, tPacket.Buf[1] = %d, tPacket.Buf[2] = %d, tPacket.Buf[3] = %d, tPacket.Buf[4] = %d\n",
									                  tPacket.Buf[0], tPacket.Buf[1], tPacket.Buf[2], tPacket.Buf[3], tPacket.Buf[4]);
	            LOGDBG("!!! - 0002 - Handle:tPacket - tPacket.Buf[5] = %d, tPacket.Buf[6] = %d, tPacket.Buf[7] = %d, tPacket.Buf[8] = %d, tPacket.Buf[9] = %d\n",
									                  tPacket.Buf[5], tPacket.Buf[6], tPacket.Buf[7], tPacket.Buf[8], tPacket.Buf[9]);				
                T_RADIO_RDS_SWITCH tRDSSwitchStatus;
				tRDSSwitchStatus.REG_Switch    = tPacket.Buf[0];
				tRDSSwitchStatus.EON_Switch    = tPacket.Buf[1];
				tRDSSwitchStatus.PTY_Switch    = tPacket.Buf[2];
				tRDSSwitchStatus.PS_Switch     = tPacket.Buf[3];
				tRDSSwitchStatus.AF_switch     = tPacket.Buf[4];
				tRDSSwitchStatus.TA_Switch     = tPacket.Buf[5];
				tRDSSwitchStatus.RDS_Switch    = tPacket.Buf[6];
				tRDSSwitchStatus.CT_Switch     = tPacket.Buf[7];
				tRDSSwitchStatus.RT_Switch     = tPacket.Buf[8];
				tRDSSwitchStatus.TA_Volume     = tPacket.Buf[9];
		
				mData.SaveRDSSwitch(tRDSSwitchStatus);
				msg.Message = METER_VMSG_UPDATE_RADIORDSINFO;
				bRet = true;
			}
			break;
		}
		case METERINFO_APP_RDS_STATUS:
		{
			if(tPacket.nLen > 0)
			{
	            LOGDBG("nRDSStatus - tPacket.Buf[0] = %d, tPacket.Buf[1] = %d, tPacket.Buf[2] = %d, tPacket.Buf[3] = %d, tPacket.Buf[4] = %d\n",\
									                  tPacket.Buf[0], tPacket.Buf[1], tPacket.Buf[2], tPacket.Buf[3], tPacket.Buf[4]);
	            LOGDBG("nRDSStatus - tPacket.Buf[5] = %d, tPacket.Buf[6] = %d, tPacket.Buf[7] = %d\n",\
								tPacket.Buf[5], tPacket.Buf[6], tPacket.Buf[7]);
				T_RADIO_RDS_SATUS nRDSStatus;
				nRDSStatus.Active_State = tPacket.Buf[0];
				nRDSStatus.TA_Info = tPacket.Buf[1];
				nRDSStatus.EON_Info = tPacket.Buf[2];
				nRDSStatus.EWS_Info = tPacket.Buf[3];
				nRDSStatus.TP_Flag = tPacket.Buf[4];
				nRDSStatus.TA_Flag = tPacket.Buf[5];
				nRDSStatus.RDS_Flag = tPacket.Buf[6];
				nRDSStatus.AF_Flash = tPacket.Buf[7];
				mData.SaveRDSSatus(nRDSStatus);
				msg.Message = METER_VMSG_UPDATE_RADIORDSSATUS;
				bRet = true;
			}
			break;
		}
		case METERINFO_APP_PTY_PS_PARAM:
		{
			if(tPacket.nLen > 0)
			{
				T_RADIO_RDS_PTY nPTY;
				LOGDBG("PTY_Name = %s",tPacket.Buf + 1);
				nPTY.PTY_Type = tPacket.Buf[0];
				memcpy(nPTY.PTY_Name, tPacket.Buf + 1 , sizeof(uint8_t) * (tPacket.nLen -1 ));
				LOGDBG("PTY_Type = %d",nPTY.PTY_Type);
				LOGDBG("PTY_Name = %s",nPTY.PTY_Name);
				mData.SaveRDSSationName(nPTY);
				msg.Message = METER_VMSG_UPDATE_RADIORDSSATIONNAME;
				bRet = true;
			}

			break;
		}
		case METERINFO_APP_RT_INFO:
		{
			if(tPacket.nLen > 0)
			{
				LOGDBG("RTInfo = %s",tPacket.Buf);
				char RTInfo[65] = {0};
				memcpy(RTInfo, tPacket.Buf , sizeof(uint8_t) * tPacket.nLen);
				LOGDBG("RTInfo = %s",RTInfo);

				mData.SaveRDSSationInfo(RTInfo);
				msg.Message = METER_VMSG_UPDATE_RADIORDSSATIONINFO;

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

bool RadioDataPacket::HandleSystemData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	if(NULL == m_pCtrl)
	{
		LOGDBG("m_pCtrl is null ");
		return bRet;
	}
	RadioData &mData = RadioData::GetInstance();
	switch(tPacket.nCmd)
	{
		default:
		{
			break;
		}
	}

	return bRet;
}

bool RadioDataPacket::HandleWirelessSerData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	RadioData &mData = RadioData::GetInstance();
	switch(tPacket.nCmd)
	{
		default:
		{
			break;
		} 
	}

	return bRet;
}

bool RadioDataPacket::HandleMobileNetData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	RadioData &mData = RadioData::GetInstance();
	switch(tPacket.nCmd)
	{
		default:
		{
			break;
		}
	}

	return bRet;
}
