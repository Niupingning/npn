/*
 * IVIController.h
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */
 
#ifndef IVICONTROLLER_H_
#define IVICONTROLLER_H_

#include "FileBase.h"
#include "IMybinder.h"
#include "PublicController.h"


#include "HomeDataPacket.h"
#include "SettingDataPacket.h"
#include "RadioDataPacket.h"
#include "MediaDataPacket.h"
#include "BTDataPacket.h"

#include "HomeData.h"
#include "SettingData.h"
#include "RadioData.h"
#include "MediaData.h"
#include "BTData.h"



#include "mediaresourcecontrol.h"
#include "mediaplaymoduleimpl.h"

#include "AVMControl.h"

using FY_NAMESPACE::IViewMessage;
using namespace android;
using namespace mybinder;

using FY_NAMESPACE::PublicController;

class IVIController: public PublicController
{
public:
	IVIController(IViewMessage *pIViewMessage);
	virtual ~IVIController();
	virtual bool OnInit();
	virtual bool Start();
	virtual bool Stop();
	virtual bool Close();

	virtual bool Send_CanBox_Data(int nCmd,uint8_t *pbuf,int nLen);

	virtual void OnTimer(uint32_t id);

	int OnCallbackFunc(const std::string nName, int ncmd, int nParam1, int nParam2, int nLen,const uint8_t *pbuf);
	int OnReadData(int ncmd, uint8_t *pbuf, int &nlen);
	int OnWriteData(int ncmd, const uint8_t *pbuf, int nlen);
	int ReOpenServiceCallBack(const char *nsrv,const int Status);
	virtual void OnBinderDeath(const std::string nName) ;
	bool SetMedioVolume(int nValue,int nType = 0);
	bool SetBtVolume(int nValue);
	bool SetMute(int nValue, int mediovol, int btvol);
	bool SetKeyMute(int nValue);
	bool SetBackLight(int nValue);
	bool SetbackLightMode(int Light, bool bAutoBg);
	bool SendBgModeToAA(int mode);
	bool SetBluetooth(int nValue);
	bool SetCarInfo();
	bool SetClearMile(const int nValue);
	bool SetUnitMode(const int nValue);
	bool SetReadLight(const int nValue);
	bool SetWheelType(const int nValue);
	bool SetMaintianMileage(const int nValue);
	bool SetSystemInfo();
	bool SetLanguage(const int nValue);
	bool SetSyncTime(const int nValue);
	bool SetCurTime(T_STR_TIME_INFO ntTime);
	
	bool SendMeterInfoData(int nType, int32_t nBand, int32_t nFreq = 0, int32_t nMute = 0,int32_t nLoc = 0);
	bool SendMeterInfoResourceTypeData(int nCmd,int nType);
	bool SendMeterInfoDataOFRadioRDS(int nType);
	bool SendMeterInfoDataOFStartingUP(int nCmd);
	bool SendMeterInfoDataOFAutoWar(int nCmd,int nType,int nStatus);
	bool SendMeterInfoDataOFPhoneStatus(int nType);
	bool SendCarPlayDataOFBTPhoneStatus(int nType);
	bool SendWirelessPairInfo(int nCmd,int nType,int nIndex,BtAddr MacAddr);
	bool SendWirelessInfoData(int nCmd,int nType,int nIndex = 0);
	bool SendCallInfoData(int nCmd,  unsigned char *phone_number, int len);
	bool SendCarPlayData(int nCmd,int nType);
	bool SendMobileData(int nCmd,int nType);
	bool SendSystemData(int nCmd,int nType);
	//autotest
	bool BackToBTLinkStatusToSys(const int nValue);
	bool BackToBTMACToSys(std::string strMac);
	bool BackWifiNameToSys(std::string strWifiName);
	int  GetBtMacAddr();

	bool SendAndroidautoData(int nCmd,int nType);

	bool InitAudio();
	bool SendBeep(const int nPlay);

	bool unborrowed();
	bool GetAudioTake();
	void SetAudioTake(bool bTake);
	void EnableAudio(bool bSet);


	void SendResourceType(int nType);
	void SendConnectMode(int nsrv);

	bool ReadWirelessData(int nCmd);
	bool SetWifiInfoToMobile(int nlen, uint8_t *pbuf);
	bool SetWifiStatusToMobileNet(const int nValue);
	//bool SetWifiMemoryLinkMode(const int nValue);
	void SendWirelessOFWifiMemoryLinkMode();

	void SendGearTOAA(int nValid,int nValue);
	void SourceMemorySelect();
	bool SetSourceMemory(const int nValue);

	void BTLinkMemorySelect();
	bool SetBTRadioMemory();
	void SendWirelessOFBTMemoryLinkMode();
	bool SetBTMusicMemoryPlayStatus(const int nValue);
	void SendWirelessOFBTMemoryBTMusicPlayStatus();

	bool SendCarPlayDataOFComboboxStatus(int nType);
	void SendCarPlayDataOFReconnectOnclick();
	bool SendCarPlayDataOFCallAction(int cmd ,int nType);

	void SaveVendorDisPoseTime();

	bool SetTurnMode(const int nValue);
	bool SetTireReset(const int nValue);

	bool SendReverseData(int cmd ,int nType);

	bool SendAAOFComboboxStatus(int nType);
	bool SendAAOFCallAction(int cmd ,int nType);
	bool SendAAOFBTName(std::string info);

	bool SendAAOFCPConnectStatus(int nStatus);
	bool SendCPOFAAConnectStatus(int nStatus);

	bool SetScreenLockSelect(const int nValue);
	bool SetEnergyRecover(const int nValue);

	//media
	bool SetMediaPlay_Url(T_PLAYMEDIA_URL &tPlayUrl);
	bool SetMediaPlay_Cmd(T_PLAYMEDIA_CMD &tPlayCmd);
	bool Query_UsbStatus(int &nMounted);   //主动向 system service 查一次 U盘挂载状态

	bool SetSystem_AVMSetting(int nSwitch,int nPointX,int nPointY);

	void AVM_StreamOn();
	void AVM_StreamOff();
	void AVM_InitDispWin();

	void Bklight_Enable();

	bool Mpu_McuRest();

	int getActiveTracksName(vector<string> &tracksName);

	void ACC_Bklight(int nACCStatus);
	bool SendCanBox_CarSelect(int nType);
	bool SendCanBox_Air_Control(int nCtrlID,int nParam);

	bool Dts_SetEQMode(int nEqMode);
	bool Dts_QueEQInfo();
	bool Dts_SetEQListen_Pos(int nListenPos);
	bool Dts_SetEQSurround_Level(int nLevel);
	bool Dts_SetEQBass_Level(int nLevel);
	bool Dts_SetEvelates_Level(int nLevel);
private:
	void Lock(bool bLock){bLock ? pthread_mutex_lock(&m_Lock) : pthread_mutex_unlock(&m_Lock);};

	void InitCMDVec();
	int DealWithCMD(const std::string nName,int ncmd,int nParam1,int nParam2,int nLen,uint8_t *pbuf);
	int ReturnSrvNameID(const std::string nName);

	bool ReadSytemSerData(int nCmd);
	bool ReadMeterInfoData(int nCmd);
	bool ReadReverseData(int nCmd);
	void can_gpio_set();

private:
	pthread_mutex_t m_Lock;
	sp<IMybinder> m_MeterInfoSrv;   //meterinfoservice
	sp<IMybinder> m_SystemSrv;
	sp<IMybinder> m_WirelessSrv;
	sp<IMybinder> m_carplaysrv;
	sp<IMybinder> m_mobilenetsrv;
	sp<IMybinder> m_Androidautosrv;
	sp<IMybinder> m_Reversesrv;
	sp<IMybinder> m_MediaSrv;
	sp<IMybinder> m_CanBoxSrv;

	HomeDataPacket m_HomeDataPacket;
	SettingDataPacket m_SettingDataPacket;
	RadioDataPacket m_RadioDataPacket;
	MediaDataPacket m_MediaDataPacket;
	BTDataPacket m_BTDataPacket;
	
	map<int,int> m_MsgMap;
	std::vector<T_STR_DATA_CMD> m_CMDVec;

	
	MediaResourceControl    m_ResCtrol;
	MediaplayModuleImpl     m_MediaModuleImpl;
	shared_ptr<ResourceTransferProgram> m_resAudioCbk;

	bool m_bAudioTake;
	bool m_bSetBGMode;

	AVMControl m_nAuxinObj;

	int m_nBklight_En;
};

#endif /*IVICONTROLLER_H_*/
