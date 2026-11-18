/*
 * BTDataPacket.cpp
 *
 *  Created on: 2025-09-26
 *      Author: Y6915
 */
 
#include "BTDataPacket.h"
#include "IVIController.h"
#include "BTData.h"
#include "IVIView.h"
#include "SourceDef.h"

BTDataPacket::BTDataPacket(IVIController *pCtrl)
{
	m_pCtrl = pCtrl;
	pthread_mutex_init (&m_Lock, NULL);

	m_bExitFlag = false;

	//SetToRunning(true);
}

BTDataPacket::~BTDataPacket()
{
	pthread_mutex_destroy( &m_Lock );
}

bool BTDataPacket::Init()
{
	bool nRet = false;
	nRet = ThreadControl::Init();
	nRet = nRet & ThreadIsReady();
	DoIt();
	return nRet;
}

void BTDataPacket::DoIt()
{
	ThreadControl::DoIt();
}

bool BTDataPacket::ThreadIsReady()
{
	return ThreadControl::ThreadIsReady();
}

void BTDataPacket::Close()
{
	ThreadControl::Close();
}

void BTDataPacket::DealWithTask()
{
	static bool bFlag = true;
	if(bFlag)
	{
		bFlag = false;
		int ret = pthread_setname_np(pthread_self(), "BTDataPacket");
		if( ret != EOK )
		{
			bFlag = true;
			LOGDBG(" DataPacket pthread_setname_np failed!");
		}
	}

	while (!m_bExitFlag)
	{
		BTData &mData = BTData::GetInstance();
		mData.WaitForPacket();
		if(mData.GetVecPacket_Size() > 0)
		{
			memset(&m_tDataPacket, 0, sizeof(T_STR_DATA_PACKET));
			mData.GetPopPacket(m_tDataPacket);
			ParseDataPacket(m_tDataPacket);
		}
	}
}

void BTDataPacket::ParseDataPacket(T_STR_DATA_PACKET tPacket)
{
	LOGDBG(" tPacket.nSrvId = %d",tPacket.nSrvId);
	bool bRet = false;
	T_APP_MSG nMsg = {0, 0, 0, 0 };
	switch(tPacket.nSrvId)
	{
		case SRV_ID_METERINFO_BINDER:
		{
			bRet = HandleMeterInfoData(tPacket, nMsg);//获取车辆状态，行车时显示无服务
			break;
		}
		case SRV_ID_WIRELESSSER_BINDER:
		{
			bRet = HandleWirelessSerData(tPacket, nMsg);
			break;
		}
		/*
		case SRV_ID_MOBILENET_BINDER:
		{
			bRet = HandleMobileNetData(tPacket, nMsg);
			break;
		}
		*/
		default:
		{
			break;
		}
	}
	if(bRet && (0 != nMsg.Message))
	{
		LOGDBG("BTApp:SendViewMessage:Message=%d,Param0=%d,Param1=%d,Param2=%d\n",\
									nMsg.Message, nMsg.Param0, nMsg.Param1, nMsg.Param2 );
		IVIView &mView = IVIView::GetInstance();
		mView.SendViewMessage(&nMsg);
	}
}

bool BTDataPacket::HandleMeterInfoData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	BTData &mData = BTData::GetInstance();
	switch(tPacket.nCmd)
	{
		case METERINFO_APP_TGS://0x5
		{
			if(tPacket.nLen > 0)
			{
				m_pCtrl->SendGearTOAA(tPacket.Buf[0],tPacket.Buf[1]);
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

bool BTDataPacket::HandleWirelessSerData(T_STR_DATA_PACKET tPacket, T_APP_MSG& msg)
{
	bool bRet = false;
	if(NULL == m_pCtrl)
	{
		LOGDBG("m_pCtrl is null ");
		return bRet;
	}
	BTData &mData = BTData::GetInstance();
	LOGDBG("-------XSJXSJXSJ------ ");
	switch(tPacket.nCmd)
	{
		case WIRELESS_EVENT_BT_CONNECTED_STATE: //0X01 蓝牙连接状态
		{
			if(tPacket.nLen > 0)
			{
				int nBTLink = tPacket.Buf[0];
				mData.SaveBtConnectStatus(nBTLink);
				LOGDBG("nBTLink = %d", nBTLink);
				msg.Message = METER_VMSG_BT_CONNECTED_STATE;
				/*
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
		case WIRELESS_EVENT_BT_OPEN_STATE: //0X02 蓝牙开启状态
		{
			if(tPacket.nLen > 0)
			{
				int nBTOpen = tPacket.Buf[0];
				mData.SaveBtOpenStatus(nBTOpen);
				LOGDBG("nBTOpen = %d", nBTOpen);
				msg.Message = METER_VMSG_BT_OPEN_STATE;
				bRet = true;
			}
			break;
		}
		case WIRELESS_EVENT_BTNAME:
		{
			if(tPacket.nLen > 0)
			{				
				LOGDBG("nBTName = %s", tPacket.Buf);

				mData.SaveBtDeviceName((char *)tPacket.Buf);
				msg.Message = SYS_VMSG_UPDATE_BT_NAME;
				bRet = true;
			}
			break;
		}
		case WIRELESS_EVENT_BT_INFO: //0X27 蓝牙历史配对列表信息
		{
			if(tPacket.nLen > 0)
			{
				BtLocalInfo_t *mBTLocalInfo = (BtLocalInfo_t*)tPacket.Buf;
				mData.SaveBtLocalInfo(mBTLocalInfo);
				//LOGDBG("mBTLocalInfo->BTName = %s", mBTLocalInfo->BTName);
				msg.Message = METER_VMSG_BT_LOCAL_INFO;
				bRet = true;
			}
			break;
		}
		case WIRELESS_EVENT_BT_DISCOVER_LIST: //蓝牙发现列表
		{
			if(tPacket.nLen > 0)
			{
				BtBondedDeviceToAppInfo_t *mBTDiscoverList = (BtBondedDeviceToAppInfo_t*)tPacket.Buf;
				mData.SaveBtDiscoverList(mBTDiscoverList);
				for(int i=0;i<5;i++)
				{
					LOGDBG("---xsjxsjxsjBTDiscoverDevice---BTDeviceName[%d] = %s---BTCharAddr[%d] = %02x:%02x:%02x:%02x:%02x:%02x---",
					i, mBTDiscoverList->BTDeviceName[i], i,
					mBTDiscoverList->BTCharAddr[i].address[0], mBTDiscoverList->BTCharAddr[i].address[1],
					mBTDiscoverList->BTCharAddr[i].address[2], mBTDiscoverList->BTCharAddr[i].address[3],
					mBTDiscoverList->BTCharAddr[i].address[4], mBTDiscoverList->BTCharAddr[i].address[5]);
				}
				msg.Message = METER_VMSG_BT_DISCOVER_INFO;
				bRet = true;
			}
			break;
		}
		case WIRELESS_EVENT_BTBOOK_SYNC_FINISHED: //0X03 蓝牙电话本更新状态
		{
			if(tPacket.nLen > 0)
			{
				STR_BT_SYNCINFO *tBTPhone = (STR_BT_SYNCINFO*)tPacket.Buf;
				LOGDBG("nSyncStatus =%s", tBTPhone->nSyncStatus);
				LOGDBG("nRecordNum =%s", tBTPhone->nRecordNum);
				mData.SaveBTPhoneSyncInfo(tBTPhone);
				bRet = true;
			}
			break;
		}
		case WIRELESS_EVENT_BTCALL_STATE_CHANGE: //0X04 通话状态
		{
			if(tPacket.nLen > 0)
			{
				STR_BT_CALLERINFO *tBTCallinfo = (STR_BT_CALLERINFO*)tPacket.Buf;
				LOGDBG("addr = %s", tBTCallinfo->addr);
				LOGDBG("PhoneNum = %s", tBTCallinfo->PhoneNum);
				LOGDBG("Name = %s", tBTCallinfo->Name);
				LOGDBG("FName = %s", tBTCallinfo->FName);
				LOGDBG("nState = %d", tBTCallinfo->nState);
				msg.Message = METER_VMSG_BT_PHONE_INFO;
				msg.Param0 = tBTCallinfo->nState;

				mData.SaveBTPhoneInfo(tBTCallinfo);
				bRet = true;
			}
			break;
		}
		case WIRELESS_EVENT_BT_PAIRED_DEVICE_INFO:
		{
			BtPairedDeviceToAppInfo_t *pInfo = (BtPairedDeviceToAppInfo_t*)tPacket.Buf;
			if(tPacket.nLen > 0)
			{
				//获取配对列表信息
				mData.SaveBTPairedListInfo(pInfo);
				for(int i=0;i<5;i++)
				{
					LOGDBG("---xsjxsjxsjtBTPairedList---BTDeviceName[%d] = %s---BTCharAddr[%d] = %02x:%02x:%02x:%02x:%02x:%02x---",
					i, pInfo->BTDeviceName[i], i,
					pInfo->BTCharAddr[i][0], pInfo->BTCharAddr[i][1],
					pInfo->BTCharAddr[i][2], pInfo->BTCharAddr[i][3],
					pInfo->BTCharAddr[i][4], pInfo->BTCharAddr[i][5]);
				}
				msg.Message = SYS_VMSG_UPDATE_BT_LIST;
				bRet = true;
			}
			break;	
		}
		case WIRELESS_EVENT_BT_AUDIO_STATE: //蓝牙音乐播放状态
		{
			if(tPacket.nLen > 0)
			{
				BTAudioStatus nBTMusicStatus = (BTAudioStatus)tPacket.Buf[0];
				mData.SaveBTMusicStatus(nBTMusicStatus);
				LOGDBG("-----xsj-----nBTMusicStatus=%d", nBTMusicStatus);
				msg.Message = METER_VMSG_BT_MUSICPLAY_STATE;
				bRet = true;
			}
			break;
		}
		case WIRELESS_EVENT_BT_MUSIC_INFO: //0X29
		{
			if(tPacket.nLen > 0)
			{
				BtMusicInfo_t *tBTMusic = (BtMusicInfo_t*)tPacket.Buf;
				if(NULL != tBTMusic)
				{
					mData.SaveBTMusicInfo(tBTMusic);
					LOGDBG("abAlbum=%s",tBTMusic->abAlbum);
					msg.Message = METER_VMSG_BT_MUSIC_INFO;
					bRet = true;
				}
			}
			break;
		}
		case WIRELESS_EVENT_BT_MUSIC_TIME:
		{
			if(tPacket.nLen > 0)
			{
				BtPlaybackTime_t *tBtMusicTime = (BtPlaybackTime_t*)tPacket.Buf;
				if(NULL != tBtMusicTime)
				{
					mData.SaveBTMusicTime(tBtMusicTime);
					LOGDBG("-----XSJXSJ-BTMUSICTIME-----dCurrentTime = %d",tBtMusicTime->dCurrentTime);
					msg.Message = METER_VMSG_BT_MUSIC_TIME;
					msg.Param0 = tBtMusicTime->dCurrentTime;
					msg.Param1 = tBtMusicTime->dTotalTime;
					bRet = true;
				}
			}
			break;
		}
		case WIRELESS_EVENT_BT_BATTERY_CHANGE: //0x05 电池状态
		{
			if(tPacket.nLen > 0)
			{
				int nPHBattery = tPacket.Buf[0];
				LOGDBG("nPHBattery =%d", nPHBattery);
				msg.Param0 = nPHBattery;
				mData.SaveINTData(INT_PH_BATTERY_STATUS, nPHBattery);
				msg.Message = METER_VMSG_PH_BATTERY;
				bRet = true;
			}
			break;
		}
		case WIRELESS_EVENT_BT_SIGANL_CHANGE: //0x06 手机信号状态
		{
			if(tPacket.nLen > 0)
			{
				int nPHSiganl = tPacket.Buf[0];
				LOGDBG("nPHSiganl =%d", nPHSiganl);
				msg.Param0 = nPHSiganl;

				mData.SaveINTData(INT_PH_SIGANL_STATUS, nPHSiganl);
				msg.Message = METER_VMSG_PH_SIGANL;
				bRet = true;
			}
			break;
		}
		case 0x21: // 同步联系人
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
				LOGDBG("zzzzzzzzzz nBTCallMute =%d", nBTCallMute);
				msg.Param0 = nBTCallMute;
				mData.SaveBTMuteStatus(nBTCallMute);				
			}else{
				msg.Param0 = 0;
				mData.SaveBTMuteStatus(0);	
			}
			msg.Message = WIFI_VMSG_BT_PHONE_CALLMUTE;
			
			bRet = true;

			break;
		}
		case WIRELESS_EVENT_BT_PHONE_CALL_MODE:
		{
			if(tPacket.nLen > 0)
			{
				int nBTCallMode = tPacket.Buf[0];
				LOGDBG("zzzzzzzzz nBTCallMode =%d", nBTCallMode);
				msg.Param0 = nBTCallMode;
				mData.SaveBTPhoneCallMode(nBTCallMode);				
			}else{
				msg.Param0 = 0;
				mData.SaveBTPhoneCallMode(0);
			}
			msg.Message = WIFI_VMSG_BT_PHONE_CALLMODE;
			bRet = true;

			break;
		}
		case WIRELESS_EVENT_BT_PAIRING_REQ_INFO:
		{
			BTPairingRegInfo_t *tBtPairingReq = (BTPairingRegInfo_t*)tPacket.Buf;
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
		case WIRELESS_EVENT_BTPHONEBOOK_SYNC:
		{
			msg.Message = METER_VMSG_BT_PHONE_BOOK_INFO;
			if(tPacket.nLen > 0)
			{
//				BtPhoneBookData *phonebook_date = (BtPhoneBookData*)tPacket.Buf;		
				BtPhoneBookSync *phone_book = (BtPhoneBookSync*)tPacket.Buf;

				LOGDBG("xxxxxxxxxxxxxxxxxxxxx contact_package_len = %d",tPacket.nLen);
				LOGDBG("xxxxxxxxxxxxxxxxxxxxx packnum=%d, index=%d, contact_number= %d",phone_book->packnum, phone_book->index, phone_book->curNum);

				int contacts = 20;
				if(phone_book->packnum == phone_book->index && phone_book->curNum % 20 > 0)
				{
					contacts = phone_book->curNum % 20;
				}
				for (int i = 0; i < contacts; i++)
				{
					LOGDBG("name: %s",phone_book->BtPhoneBookInfo[i].N_name);
					LOGDBG("number: %s",phone_book->BtPhoneBookInfo[i].number);
				}
				if(phone_book->curNum <= 1000)
				{
					mData.SaveBtContacts(phone_book);
				}
				
				if( phone_book->packnum == phone_book->index)
				{
					LOGDBG("xxxxxxxxxxxxxxxxxxxxx sendto view");
					bRet = true;
				}
				
			}
			break;
		}
		case WIRELESS_EVENT_BT_PHONE_TIME:
		{
			msg.Message = METER_VMSG_BT_PHONE_TIME_INFO;
			if(tPacket.nLen >= 3)
			{
				LOGDBG("call time: hour=%d, min=%d, sec=%d",tPacket.Buf[0], tPacket.Buf[1], tPacket.Buf[2]);
				msg.Param0 = tPacket.Buf[0];
				msg.Param1 = tPacket.Buf[1];
				msg.Param2 = tPacket.Buf[2];
				bRet = true;
			}
			break;
		}
		case WIRELESS_EVENT_BT_HISTORY_RECORD_INCOMING:
		case WIRELESS_EVENT_BT_HISTORY_RECORD_OUTGOING:
		case WIRELESS_EVENT_BT_HISTORY_RECORD_MISS:
		{
			msg.Message = METER_VMSG_BT_PHONE_HISTORY_INFO;
			if(tPacket.nLen > 0)
			{	
				BtRecordData_t *call_history = (BtRecordData_t*)tPacket.Buf;

				LOGDBG("xxxxxxxxxxxxxxxxxxxxx call_history_package_len = %d",tPacket.nLen);
				LOGDBG("xxxxxxxxxxxxxxxxxxxxx call_history_number= %d, state=%d",call_history->count, call_history->state);
				for (int i = 0; i < 10; i++)
				{
					LOGDBG("name: %s",call_history->RecordInfo[i].N_name);
					LOGDBG("number: %s",call_history->RecordInfo[i].number);
				}
				if(call_history->state ==1)
				{
					mData.SaveBtCallHistoryAnswer(call_history);
				}else if(call_history->state == 2)
				{
					mData.SaveBtCallHistoryDial(call_history);
				}else if(call_history->state == 3)
				{
					mData.SaveBtCallHistoryMiss(call_history);
				}
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


