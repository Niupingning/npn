/*
 * AppDatadef.h
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#ifndef APPDATADEF_H_
#define APPDATADEF_H_
#include "IViewMessage.h"
#include <string>
#include "AppCMDdef.h"
using FY_NAMESPACE::VMSG_USER;
using std::string;

#define MAX_DATA_LEN   4096
#define TEXT_BUF_LEN   256


//Timer ID
enum ENUM_TIMER_ID
{
	TIMERID_PHONE_TIME = 1001,
	TIMERID_PHONE_WNDCHANGE = 1002,
	TIMERID_VOLUME_CLOSE = 1003,
	TIMERID_VOLUME_FLASH = 1004,
	TIMERID_RADIO_NEXT = 1005,
	TIMERID_RADIO_LAST = 1006,
	TIMERID_GET_UUID   = 1007,
	TIMERID_DATAPACKET_HOME = 1008,
	TIMERID_DATAPACKET_RADIO = 1009,
	TIMERID_DATAPACKET_SETTING = 1010,
	TIMERID_SET_DATA_MCU = 1011,
	TIMERID_ACCON_NOT_ANIEND = 1012,
	TIMERID_GET_DISPOSE = 1013,
	TIMERID_SET_RSET = 1014,
	TIMERID_SET_LOCKSCREEN_NUM = 1015,
	TIMERID_SET_LOCKSCREEN_STATUS = 1090,
	TIMERID_AUDIO_POP_HIDE = 1091,        //隐藏公共音量弹窗
	TIMERID_AVM_SIGN = 1092,
	TIMERID_CPAA_SIGN = 1093,
	TIMERID_AIR_POP_HIDE = 1094,
	TIMERID_DEMO_SET_PIC = 1095,
	TIMERID_DEMO_WIN_SHOW = 1096,
	TIMERID_MAX
};

enum ENUM_TIMER_LONG
{
	TIMER_LONG_250 = 250,
	TIMER_LONG_300 = 300,
	TIMER_LONG_500 = 500,
	TIMER_LONG_1000 = 1000,
	TIMER_LONG_2000 = 2000,
	TIMER_LONG_3000 = 3000,
	TIMER_LONG_3500 = 3500,
	TIMER_LONG_5000 = 5000,
	TIMER_LONG_6000 = 6000,
	TIMER_LONG_8000 = 8000,
	TIMER_LONG_15000 = 15000,
	TIMER_LONG_60000 = 60000
};

//message def
enum  ENUM_VIEW_MSG_TYPE
{
	/*****wen*****/
	APP_VMSG_SET_VOLAME = VMSG_USER + 1,
	APP_VMSG_SET_BRIGHT =  VMSG_USER + 2,
	APP_VMSG_SET_BT =  VMSG_USER + 3,
	APP_VMSG_SET_WIFI =  VMSG_USER + 4,
	APP_VMSG_SET_CAR =  VMSG_USER + 5,
	APP_VMSG_SET_SYSTEM =  VMSG_USER + 6,

	SYS_VMSG_UPDATE_VOLUME_VALUE = VMSG_USER + 8,
	SYS_VMSG_UPDATE_MTUE = VMSG_USER + 9,
	SYS_VMSG_UPDATE_KEY_MUTE = VMSG_USER + 10,

	SYS_VMSG_UPDATE_BACKLIGHT_LEVEL = VMSG_USER + 11,
	SYS_VMSG_UPDATE_BACKGROUND = VMSG_USER + 12,

	SYS_VMSG_UPDATE_BT_STWITCH = VMSG_USER + 16,
	SYS_VMSG_UPDATE_BT_LIST = VMSG_USER + 17,
	SYS_VMSG_UPDATE_BT_NAME = VMSG_USER + 18,

	SYS_VMSG_UPDATE_WIFI_STWITCH = VMSG_USER + 20,
	SYS_VMSG_UPDATE_WIFI_LIST = VMSG_USER + 21,

	SYS_VMSG_UPDATE_UNIT = VMSG_USER + 25,
	SYS_VMSG_UPDATE_READLIGHT = VMSG_USER + 26,
	SYS_VMSG_UPDATE_WHEEL = VMSG_USER + 27,
	METER_VMSG_UPDATE_MAINTAIN = VMSG_USER + 28,

	SYS_VMSG_UPDATE_VICODE = VMSG_USER + 30,
	SYS_VMSG_UPDATE_LANGUAGE = VMSG_USER + 31,
	SYS_VMSG_UPDATE_DATETIME = VMSG_USER + 32,
	SYS_VMSG_UPDATE_VERSION = VMSG_USER + 33,
	SYS_VMSG_UPDATE_APPDOWN = VMSG_USER + 34,

	/*tm*/
	METER_VMSG_DATETIME = VMSG_USER + 35,
	METER_VMSG_TIRE_INFO = VMSG_USER + 36,
	METER_VMSG_WND_CHANGE = VMSG_USER + 37,
	METER_VMSG_BT_CONNECTED_STATE = VMSG_USER + 38,

	APP_VMSG_VEHICLE_STATE_INFO = VMSG_USER + 39,
	METER_VMSG_WIFI_SIGANL = VMSG_USER + 40,
	METER_VMSG_PH_BATTERY = VMSG_USER + 41,
	METER_VMSG_PH_SIGANL = VMSG_USER + 42,
	METER_VMSG_BT_MUSIC_STATE = VMSG_USER + 43,
	METER_VMSG_ACCSTATUS = VMSG_USER + 44,

	//Radio
	METER_VMSG_UPDATE_RADIOINFO = VMSG_USER + 45,

	
	METER_VMSG_BT_MUSICPLAY_STATE = VMSG_USER + 46,
	METER_VMSG_BT_MUSIC_INFO = VMSG_USER + 47,
	METER_VMSG_BT_PHONE_INFO = VMSG_USER + 48,
	CARPLAY_VMSG_CONNECTSTATUS = VMSG_USER + 49,

	METER_VMSG_LASTCHARGE_INFO = VMSG_USER + 50,
	METER_VMSG_UPDATE_RADIORDSINFO = VMSG_USER + 51,
	METER_VMSG_TIRESTATUS = VMSG_USER + 52,
	WIFI_VMSG_BT_PHONE_CALLMUTE = VMSG_USER + 53,
	WIFI_VMSG_BT_PHONE_CALLMODE = VMSG_USER + 54,
	METER_VMSG_UPDATE_RADIORDSSATUS = VMSG_USER + 55,
	METER_VMSG_UPDATE_RADIORDSSATIONNAME = VMSG_USER + 56,
	METER_VMSG_UPDATE_RADIORDSSATIONINFO = VMSG_USER + 57,
	SYS_VMSG_MODILE_CONNECTMODE = VMSG_USER + 58,

	MOBILE_VMSG_MODILERESOURCE_YL = VMSG_USER + 59,
	MOBILE_VMSG_MODILERESOURCE_LOAD_CPAA = VMSG_USER + 60,
	YL_VMSG_CONNECTSTATUS = VMSG_USER + 61,
	YL_VMSG_QRCODE_STATUS = VMSG_USER + 62,
	MOBILE_VMSG_MODILERESOURCE_RELEASE_CPAA = VMSG_USER + 63,

	ANDRIOD_VMSG_CONNECTSTATUS = VMSG_USER + 64,
	METER_VMSG_KEY_CHANGE = VMSG_USER + 65,

	WIFI_VMSG_WIFI_CONNECTSTATUS = VMSG_USER + 66,
	METER_VMSG_MILEAGE = VMSG_USER + 67,
	METER_VMSG_CELL = VMSG_USER + 68,
	SYS_VMSG_UPDATE_SYNCTIME = VMSG_USER + 69,
	SYS_VMSG_UPDATE_SCREEN_VERSION = VMSG_USER + 70,

	SYS_VMSG_DISCONNECT_BT = VMSG_USER + 71,
	CARPLAY_VMSG_CONFIRM = VMSG_USER + 72,

	WIFI_VMSG_BTPAIRINGREQ = VMSG_USER + 73,

	METER_VMSG_HOMEANIMA = VMSG_USER + 74,
	CARPLAY_VMSG_PHONE_STATUS = VMSG_USER + 75,
	SYS_VMSG_ACCSTATUS_TIME = VMSG_USER + 76,
	SYS_VMSG_UPDATE_TURN_MODE = VMSG_USER + 77,
	SYS_VMSG_UPDATE_TIRE_RESET = VMSG_USER + 78,
	SYS_VMSG_CONFIGE = VMSG_USER + 79,
	ANDRIOD_VMSG_PHONE_STATUS = VMSG_USER + 80,
	SYS_VMSG_UPDATE_ENERGY_RECOVER = VMSG_USER + 81,
	REVERSE_VMSG_BACKSTATUS = VMSG_USER + 82,

	SYS_VMSG_USBMOUNT = VMSG_USER + 83,
	MEDIA_VMSG_PLAYSTATUS = VMSG_USER + 84,
	MEDIA_VMSG_PLAYTIME = VMSG_USER + 85,
	METER_VMSG_BT_MUSIC_TIME = VMSG_USER + 86,
	METER_VMSG_BT_OPEN_STATE = VMSG_USER + 87,
	METER_VMSG_BT_PHONE_BOOK_INFO = VMSG_USER + 88,
	METER_VMSG_BT_PHONE_TIME_INFO = VMSG_USER + 89,
	METER_VMSG_BT_LOCAL_INFO = VMSG_USER + 90,
	METER_VMSG_BT_DISCOVER_INFO = VMSG_USER + 91,
	METER_VMSG_BT_PHONE_HISTORY_INFO = VMSG_USER + 92,
	CANBOX_VMSG_AIRDATA_INFO = VMSG_USER + 93,

	CANBOX_VMSG_AIRCANBOXPROTO_INFO = VMSG_USER + 2007,

	CANBOX_VMSG_CARDATA_INFO = VMSG_USER + 94,
	CANBOX_VMSG_CARDATA_SETTING = VMSG_USER + 95,

	SYSTEM_VMSG_DTSEQ_SETTING = VMSG_USER + 96,
	METER_VMSG_MAX
};

enum ENUM_INT_DATA_KEY
{
	INT_WIFI_SIGANL_STATUS = 0,
	INT_PH_BATTERY_STATUS = 1,
	INT_PH_SIGANL_STATUS = 2,
	INT_BT_LINK_STATUS = 3,
	INT_ACCSTATUS = 4,
	INT_BACKGROUND = 5,
	INT_UISTYLE = 6,
	INT_VOLUME_LEVEL =7,
	INT_BACKLIGHT_LEVEL =8,
	INT_UNIT_MODE = 9,
	INT_WHEEL_TYPE = 10,
	INT_LANGUAGE = 11,
	INT_POSLIGHT = 12,
	INT_RAMILEAGE = 13,

	INT_BT_PLAY_STATUS = 14,
	INT_APPSTATUS = 15,
	INT_CARPLAYCONNECTSTATUS = 16,
	INT_READ_LIGHT = 17,
	INT_TIRE_STATUS = 18,
	INT_BT_CALLMUTE = 19,
	INT_BT_CALLMODE = 20,
	INT_MODILECONNECTMODE = 21,
	INT_MODILERESOURCE_YL = 22,
	INT_MODILERESOURCE_LOAD_CP = 23,
	INT_MODILE_YL_MIRROR = 24,
	INT_MODILE_YL_CARBIT = 25,
	INT_MODILE_YL_QRCode = 26,
	INT_BT_VOLUME_LEVEL =27,
	INT_MODILERESOURCE_RELEASE_CP = 28,
	INT_ANDRIODCONNECTSTATUS = 29,
	INT_MODILERESOURCE_LOAD_AA = 30,
	INT_MODILERESOURCE_RELEASE_AA = 31,

	INT_KEY_RETURN_APP = 32,
	INT_KEY_RETURN_RADIO = 33,
	INT_KEY_RETURN_SETTING = 34,
	INT_WIFI_STATUS = 35,
	INT_SYNCTIME = 36,
	INT_WIFI_MEMORY_LINK_STATUS = 37,
	INT_MODILERESOURCE_AA = 38,
	INT_MODILERESOURCE_CP = 39,
	INT_CARPLAY_CONFIRM = 40,
	INT_MCU_SOURCE_MEMORY = 41,
	INT_MOBIL_ACC = 42,
	INT_CP_PHONE = 43,
	INT_MCU_BTLINK_MEMORY = 44,
	INT_RADIO_PLAY_STATUS = 45,
	INT_ACC_CURRSTATUS = 46,
	INT_BTMUSIC_PLAY_STATUS = 47,
	INT_CP_CONNECT_REDAY_STATUS = 48,
	INT_CARPLAYSHOWSTATUS = 49,
	INT_TURN_MODE = 50,
	INT_TIRE_RESET = 51,
	INT_AASHOWSTATUS = 52,
	INT_AA_PHONE = 53,
	INT_ENERGY_RECOVER = 54,
	INT_REVERSE = 55,
	INT_AVM_SIGN = 56,
	INT_CP_SPLITSTATUS = 57,   //CP分屏：0全屏 1分屏
	INT_AA_SPLITSTATUS = 58,   //AA分屏：0全屏 1分屏
	INT_SPLIT_PAGE = 59,       //分屏卡片当前页 0/1/2
	INT_DATA_NUM
};

typedef struct STR_DATA_PACKET
{
	int nSrvId;
	int nCmd;
	int nLen;
	int nParam1;
	int nParam2;
	uint8_t Buf[MAX_DATA_LEN];
	STR_DATA_PACKET():nSrvId(0),nCmd(0),nLen(0),nParam1(0),nParam2(0)
	{
		memset(Buf, 0, MAX_DATA_LEN);
	}

	STR_DATA_PACKET(int SrvId, int Cmd, int Param1, int Param2, int Len, uint8_t *pbuf)
	{
		this->nSrvId = SrvId;
		this->nCmd = Cmd;
		this->nParam1 = Param1;
		this->nParam2 = Param2;
		this->nLen = Len;
		memcpy(this->Buf, pbuf , sizeof(uint8_t) * Len);
	}

	~STR_DATA_PACKET(){}

}T_STR_DATA_PACKET;

typedef struct STR_TIRE_INFO
{
	int nTirePresUnit;
	int nFLTirePresValid;
	int nFLTirePresValue;
	int nRLTirePresValid;
	int nRLTirePresValue;
	int nFRTirePresValid;
	int nFRTirePresValue;
	int nRRTirePresValid;
	int nRRTirePresValue;
	STR_TIRE_INFO():nTirePresUnit(0),nFLTirePresValid(0),nFLTirePresValue(0),nRLTirePresValid(0),\
			nRLTirePresValue(0),nFRTirePresValid(0),nFRTirePresValue(0),nRRTirePresValid(0),\
			nRRTirePresValue(0)
	{

	}

}T_STR_TIRE_INFO;

typedef struct STR_TIRE_ALARM
{
	int nFLTirePresAlarm;
	int nRLTirePresAlarm;
	int nFRTirePresAlarm;
	int nRRTirePresAlarm;
	STR_TIRE_ALARM():nFLTirePresAlarm(0),nRLTirePresAlarm(0),nFRTirePresAlarm(0),nRRTirePresAlarm(0)
	{
		
	}
}T_STR_TIRE_ALARM;

enum ENUM_APP_STATUS
{
	E_APP_SHOW = 1,
	E_APP_HIDE = 2
};

enum ENUM_THEME
{
	E_THEME1 = 0,
	E_THEME2 = 1
};

/*tm*/
typedef struct STR_VEHICLE_STATE_INFO
{
	int nTopTempValid;
	int nTopTempUnit;
	int nTopTempValue;
	int nACValid;
	int nACState;
	int nTireRsetStatus;
	STR_VEHICLE_STATE_INFO():nTopTempValid(0),nTopTempUnit(0),nTopTempValue(0),nACValid(0),nACState(0),nTireRsetStatus(0)
	{

	}
}T_STR_VEHICLE_STATE_INFO;

typedef struct STR_BT_MUSICINFO
{
	unsigned int dCurrentTrack;
	unsigned int dTotalTracks;
	unsigned int dPlayingTime;
	char abMusicTitle[256];
	char abArtist[256];
	char abAlbum[256];
	char abGenre[256];
	STR_BT_MUSICINFO()
	{
		this->dCurrentTrack = 0;
		this->dTotalTracks = 0;
		this->dPlayingTime = 0;
		memset(&abMusicTitle, 0 , sizeof(abMusicTitle));
		memset(&abArtist, 0 , sizeof(abMusicTitle));
		memset(&abAlbum, 0 , sizeof(abAlbum));
		memset(&abGenre, 0 , sizeof(abGenre));
	}

	STR_BT_MUSICINFO &operator=(const STR_BT_MUSICINFO &tMusicInfo)
	{
		if(this != &tMusicInfo)
		{
			this->dCurrentTrack = tMusicInfo.dCurrentTrack;
			this->dTotalTracks = tMusicInfo.dTotalTracks;
			this->dPlayingTime = tMusicInfo.dPlayingTime;
			memcpy(abMusicTitle, tMusicInfo.abMusicTitle, sizeof(abMusicTitle));
			memcpy(abArtist, tMusicInfo.abArtist, sizeof(abArtist));
			memcpy(abAlbum, tMusicInfo.abAlbum, sizeof(abAlbum));
			memcpy(abGenre, tMusicInfo.abGenre, sizeof(abGenre));
		}
		return *this;
	}

}T_STR_BT_MUSICINFO;

typedef struct _BtLocalInfo
{
	char BTName[30];
	char macAddr[128];
	int nBTState;
	
	_BtLocalInfo()
	{
		memset(BTName, 0, 30);
		memset(macAddr, 0, 128);
		nBTState = 0;
	}

	void __RESET()
	{
		memset(BTName, 0, 30);
		memset(macAddr, 0, 128);
		nBTState = 0;
	}

}BtLocalInfo_t;

typedef struct _BT_PAIRED_LIST
{
	int num;
	int index[5];
	int connState[5];
	char BTDeviceName[5][30];
	char addr[5][128];
	_BT_PAIRED_LIST()
	{
		num = 0;
		memset(index, 0, 5);
		memset(connState, 0, 5);
		memset(BTDeviceName, 0, sizeof(BTDeviceName));
		memset(addr, 0, sizeof(addr));
	}
}BT_PAIRED_LIST_T;

/*enum BTAudioStatus
{
 AUDIO_STATUS_STOP = 0,
 AUDIO_STATUS_PLAY = 1,
 AUDIO_STATUS_PAUSE = 2,
 AUDIO_STATUS_FWD_SEEK = 3,
 AUDIO_STATUS_REV_SEEK = 4,
 AUDIO_STATUS_UNKNOWN = 0xFF
};*/

/*enum BTMusicPlayStatus
{
	CMD_PLAY = 0x44,
	CMD_PAUSE = 0x46,
	CMD_NEXT = 0x4B,
	CMD_PREV = 0x4C,
};*/

/*
************wen************/
const int32_t MIN_BAR_VALUE = 0;
const int32_t BAR_DELTA = 1;

const int32_t SETTING_DATE_YEAR_MIN = 2000;
const int32_t SETTING_DATE_YEAR_MAX = 2099;

enum MENU_LEVEL
{
	MENU_DEFAULT = 0,
	MENU_SYSTEM_1 = 1,
	MENU_SYSTEM_2 = 2,
	MENU_SYSTEM_3 = 3,
};

enum ENUM_UNIT_MODE
{
	E_UNNIT_MODE_METRIC = 0,
	E_UNNIT_MODE_IMPERIAL = 1,
};

enum ENUM_LANGAGE
{
	E_LANGAGE_CHINESE = 0,
	E_LANGAGE_ENGLISH = 1,
};

enum ENUM_WHEEL_TYPE
{
	E_WHEEL_TYPE_R15 = 0,
	E_WHEEL_TYPE_R14 = 1,
};

enum ENUM_BT_STATE
{
	E_BT_STATE_ON = 0,
	E_BT_STATE_OFF = 1,
};

enum ENUM_READLIGHT_STATE
{
	E_READLIGHT_STATE_OFF = 0,
	E_READLIGHT_STATE_ON = 1,
};

enum ENUM_WIFI_STATE
{
	E_WIFI_STATE_OFF = 0,
	E_WIFI_STATE_ON = 1,
};

enum ENUM_BT_STATUS
{
	E_BT_STATUS_PAIRINGCANCEL = 0, //00/*!<Pairing Cancel*/
	E_BT_STATUS_PAIRINGOFF = 1, //01/*!<Pairing Off*/
	E_BT_STATUS_PAIRINGSUCCESS = 2, //02/*!<Pairing Success*/
};

typedef struct UI_Node
{	
	int WndID;
	int NodeId;
	int GroupId;
	UI_Node():WndID(0),NodeId(0),GroupId(0)
	{}
	UI_Node(int Wnd, int Node, int Group):
		WndID(Wnd),NodeId(Node),GroupId(Group)
	{}

}T_UI_NODE;

typedef struct STR_TIME_INFO
{
	int nClockMode;
	int nYear;
	int nMonth;
	int nDay;
	int nHours;
	int nMinutes;

	STR_TIME_INFO():nClockMode(0),nYear(0),nMonth(0),\
		nDay(0),nHours(0),nMinutes(0)
	{

	}
	STR_TIME_INFO & operator = (const STR_TIME_INFO& state)
	{
		nClockMode = state.nClockMode;
		nHours = state.nHours;
		nMinutes = state.nMinutes;
		nYear = state.nYear;
		nMonth = state.nMonth;
		nDay = state.nDay;

		return *this;
	}

}T_STR_TIME_INFO;

typedef struct STR_VOLUME_INFO
{
	int nAlarmLevel;
	int nMediaLevel;
	int nRadioLevel;
	int nBTCallLevel;
	STR_VOLUME_INFO():nAlarmLevel(0),nMediaLevel(0), \
		nRadioLevel(0),nBTCallLevel(0)
	{

	}

}T_VOLUME_INFO;

typedef struct STR_KEY_INFO
{
	int nKeyType;
	int nKeyStatus;
	STR_KEY_INFO():nKeyType(0), nKeyStatus(0)
	{

	}

}T_STR_KEY_INFO;

typedef struct STR_BT_SYNCINFO
{
	int nSyncStatus;
	unsigned int nRecordNum;
	STR_BT_SYNCINFO(): nSyncStatus(0), nRecordNum(0)
	{

	}

	STR_BT_SYNCINFO(int nSyncStatus, unsigned int nRecordNum): nSyncStatus(nSyncStatus), nRecordNum(nRecordNum)
	{

	}

}T_STR_BT_SYNCINFO;

typedef struct STR_BT_CALLERINFO
{
	char addr[6];
	char PhoneNum[128];
	char Name[128];
	char FName[128];
	int nState;
	STR_BT_CALLERINFO()
	{
		memset(&addr, 0 , sizeof(addr));
		memset(&PhoneNum, 0 , sizeof(PhoneNum));
		memset(&Name, 0 , sizeof(Name));
		memset(&FName, 0 , sizeof(FName));
		this->nState = 0;
	}

	STR_BT_CALLERINFO &operator=(const STR_BT_CALLERINFO &tCallerInfo)
	{
		if(this != &tCallerInfo)
		{
			memcpy(addr, tCallerInfo.addr, sizeof(addr));
			memcpy(PhoneNum, tCallerInfo.PhoneNum, sizeof(PhoneNum));
			memcpy(Name, tCallerInfo.Name, sizeof(Name));
			memcpy(FName, tCallerInfo.FName, sizeof(FName));
			this->nState =  tCallerInfo.nState;
		}
		return *this;
	}

}T_STR_BT_CALLERINFO;



typedef struct STR_WIFI_INFO
{
	char Name[64];
	char ifName[16];
	char password[64];
	char hwMode[16];
	char security[16];
	char macAddr[32];
	uint8_t channel;
	uint8_t mode;
	uint8_t freq;

	STR_WIFI_INFO()
	{
		memset(Name, 0 , sizeof(Name));
		memset(ifName, 0 , sizeof(ifName));
		memset(password, 0 , sizeof(password));
		memset(hwMode, 0 , sizeof(hwMode));
		memset(security, 0 , sizeof(security));
		memset(macAddr, 0 , sizeof(macAddr));
		this->channel = 0;
		this->mode = 0;
		this->freq = 0;
	}

	STR_WIFI_INFO &operator=(const STR_WIFI_INFO &tWifiInfo)
	{
		if(this != &tWifiInfo)
		{
			memcpy(Name, tWifiInfo.Name, sizeof(Name));
			memcpy(ifName, tWifiInfo.ifName, sizeof(ifName));
			memcpy(password, tWifiInfo.password, sizeof(password));
			memcpy(hwMode, tWifiInfo.hwMode, sizeof(hwMode));
			memcpy(security, tWifiInfo.security, sizeof(security));
			memcpy(macAddr, tWifiInfo.macAddr, sizeof(macAddr));
			this->channel = tWifiInfo.channel;
			this->mode =  tWifiInfo.mode;
			this->freq =  tWifiInfo.freq;
		}
		return *this;
	}

}T_STR_WIFI_INFO;

typedef struct _BT_DEVICE {
	char szDeviceName[256];
	char szBTAddress[64];
	
	_BT_DEVICE(const char* Name, const char* Addr)
	{
		memcpy(szDeviceName, Name, strlen(Name));
		memcpy(szBTAddress,	Addr, strlen(Addr));
	}

	_BT_DEVICE()
	{
		memset(szDeviceName, 0, 256);
		memset(szBTAddress, 0, 64);
	}
	
	bool operator == (const _BT_DEVICE & data) const
    {
		return (0 == strcmp(data.szBTAddress, szBTAddress));
    }

	bool operator != (const _BT_DEVICE & data) const
    {
        if (*this == data)
        {
            return false;
        }
		else
		{
        	return true;
		}
    }

	_BT_DEVICE & operator = (const _BT_DEVICE & info)
    {
		strcpy(szDeviceName, info.szDeviceName);
		strcpy(szBTAddress,	info.szBTAddress);
		
        return *this;
    }
} BT_DEVICE_T;

typedef struct SET_VIEW_NODE
{
	int mCurrCtrlID;
	int mNextCtrlID;
	int nCtrlID;

	SET_VIEW_NODE()
	{
		mCurrCtrlID = 0;
		mNextCtrlID = 0;
		nCtrlID = 0;
	}

	SET_VIEW_NODE &operator=(const SET_VIEW_NODE &tNode)
	{
		if(this != &tNode)
		{
			this->mCurrCtrlID = tNode.mCurrCtrlID;
			this->mNextCtrlID = tNode.mNextCtrlID;
			this->nCtrlID = tNode.nCtrlID;
		}
		return *this;
	}

}T_SET_NODE;

/*************wen end*************/


typedef struct RADIO_CURR_FREQ
{
	int nSignalStrength;    //信号强度
	int nCurrFreq;          //当前频率
	int nRadioFlagLOCAL;    //
	int nRadioStatus;       //搜索状态
	int nStereoMono;       	//立体声/单声道
	int nValid;            	//当前台是否为有效台 ,电台是否能出声，可以用来作为电台列表是判断依据
	int nCurrBand;         	//当前波段 AM/FM
	int nRadioArea;         //收音区域 RDS
	int nCurrMute;			//收音机是否静音
	int nWorkStatus;		//收音机工作状态
	RADIO_CURR_FREQ()
	{
		nSignalStrength = 0;    
		nCurrFreq = 0;          
		nRadioFlagLOCAL = 0;    
		nRadioStatus = 0;       
		nStereoMono = 0;       
		nValid = 0;            
		nCurrBand = 0;         
		nRadioArea = 0;         
		nCurrMute = 1;
		nWorkStatus = 0;
	}

}T_RADIO_CURR_FREQ;

typedef enum
{
    FM                = 0x00,
    AM                = 0x01,
    OIRT              = 0x02,
	MW                = 0x03,
	LW                = 0x04,
	BAND_COUNT        = 0x05,
} T_BAND;

typedef enum
{
	FM_FREQ_MIN = 8750,
	FM_FREQ_MAX = 10800,
	AM_FREQ_MIN = 531,
	AM_FREQ_MAX = 1629
} T_FREQ;

typedef enum
{
	FM_POINTER_LEN = 205,
	AM_POINTER_LEN = 119
} T_POINTER_LEN;

typedef enum
{
    RADIO_BAND_FREQ = 0,
    RADIO_TUNING_UP = 1,
	RADIO_TUNING_DOWN = 2,
	RADIO_SEEK_UP = 3,
	RADIO_SEEK_DOWN = 4,
	RADIO_AS = 5,
	RADIO_PREVIEW_SCAN = 6,
	RADIO_PRESET_SCAN = 7,
	RADIO_PRESET_SAVE = 8,
	RADIO_PRESET_LOAD = 9,
	RADIO_TUNE_FREQ = 10,
	RADIO_LOC = 11,
	RADIO_PTY_SEEK = 12,
	RADIO_MUTE = 13,
	RADIO_BREAK = 14,
	RADIO_AREA = 15
} T_RADIO_KEY_COMMAND;

typedef enum
{
	RADIO_SEARSTATUS_NO = 0,
	RADIO_SEARSTATUS_AS = 1,
	RADIO_SEARSTATUS_PS = 2,

	RADIO_SEARSTATUS_START = 3,
	RADIO_SEARSTATUS_SEACHING = 4,
	RADIO_SEARSTATUS_ENE = 5,
	RADIO_SEARSTATUS_START_PS = 6,
	RADIO_SEARSTATUS_SEACHING_PS = 7,
	RADIO_SEARSTATUS_ENE_PS = 8
} T_RADIO_SEARSTATUS;

typedef enum
{
	T_RADIO_COLLECTLIST_AMLEN = 14,
	T_RADIO_COLLECTLIST_FMLEN = 18
} T_RADIO_COLLECTLIST;

typedef enum
{
	APP_STATUS_NO = 0,
	APP_STATUS_CP = 1,
	APP_STATUS_AA = 2,
	APP_STATUS_YL = 3
} T_APP_STATUS;


typedef enum
{
	RESOERCE_TYPE_NO = 0,
	RESOERCE_TYPE_CP = 1,
	RESOERCE_TYPE_AA = 2,
	RESOERCE_TYPE_YL = 3,
	RESOERCE_TYPE_RD = 4,
	RESOERCE_TYPE_BT = 5,
} T_RESOERCE_TYPE;


typedef struct HOME_LASTCHARGE_INFO
{
	int nMileValid;    		//上次充电后行驶里程的有效性
	int nMileUnit;          //上次充电后行驶里程单位
	int nMileValue;    		//上次充电后的行驶里程
	int nTimeValid;       	//上次充电后经过的时间值有效性
	int nTimeUnit;       	//时间单位
	int nTimeValue;         //上次充电后经过的时间值
	int nAveEn;         	//上次充电后平均能耗
	int nTimeH;
	int nTimeM;
	HOME_LASTCHARGE_INFO()
	{
		nMileValid = 0;    
		nMileUnit = 0;          
		nMileValue = 0;    
		nTimeValid = 0;       
		nTimeUnit = 0;       
		nTimeValue = 0;            
		nAveEn = 0;
		nTimeH = 0;
		nTimeM = 0;        
	}

}T_HOME_LASTCHARGE_INFO;

typedef struct RADIO_RDS_SWITCH
{
	uint8_t REG_Switch; 
	uint8_t EON_Switch; 
	uint8_t PTY_Switch; 
	uint8_t PS_Switch; 
	uint8_t AF_switch; 
	uint8_t TA_Switch; 
	uint8_t RDS_Switch; 
	uint8_t CT_Switch; 
	uint8_t RT_Switch; 
	uint8_t TA_Volume;
	RADIO_RDS_SWITCH()
	{
		REG_Switch = 0;
		EON_Switch = 0;
		PTY_Switch = 0;
		PS_Switch = 0;
		AF_switch = 1;
		TA_Switch = 1;
		RDS_Switch = 0;
		CT_Switch = 0;
		RT_Switch = 0;
		TA_Volume = 0;
	}   
}T_RADIO_RDS_SWITCH;

typedef enum
{
	REG_SWITCH = 0,
	EON_SWITCH = 1,
	PTY_SWITCH = 2,
	PS_SWITCH = 3,
	AF_SWITCH = 4,
	TA_SWITCH = 5,
	RDS_SWITCH = 6,
	CT_SWITCH = 7,
	RT_SWITCH = 8,
	TA_VOLUME = 9
} T_RADIO_SWITCH_COMMAND;

typedef struct RADIO_RDS_PTY
{
	int PTY_Type; 
	char PTY_Name[9]; 
	RADIO_RDS_PTY()
	{
		PTY_Type = 0;
		memset(PTY_Name, 0 , sizeof(PTY_Name));
	}   
}T_RADIO_RDS_PTY;

typedef struct RADIO_RDS_SATUS
{
	int Active_State; 
	int TA_Info;
	int EON_Info;
	int EWS_Info;
	int TP_Flag;
	int TA_Flag;
	int RDS_Flag;
	int AF_Flash;
	RADIO_RDS_SATUS()
	{
		Active_State = 0;
		TA_Info = 0;
		EON_Info = 0;
		EWS_Info = 0;
		TP_Flag = 0;
		TA_Flag = 0;
		RDS_Flag = 0;
		AF_Flash = 0;
	}   
}T_RADIO_RDS_SATUS;
/*
buf[0]:RDS Active State
  0: Normal
  1: PI Seek
  2: PTY Seek
RDS Station Notify:       
buf[1]: TA Info ( 1: Traffic announcement arrived )         
buf[2]:EON Info ( 1: EON Traffic announcement arrived )          
buf[3]: EWS Info ( 1: EWS arrived )          
buf[4]:TP Flag
 ( 1: This is a TP station, and has traffic announcement ability)
buf[5]:TA Flag
( 1: This is a TA station, and has EON Traffic announcement ability)
buf[6]:RDS Flag ( 1: Current station is a RDS station )          
buf[7]:AF Flash ( 1: AF Checking)    
*/
enum ENUM_RDS_AREA
{

	E_EUROPE_AREA = 0,//欧洲
	E_NORTH_AMERICA_ARE,//北美
	E_SOURTH_AMERICA_AREA,//南美
	E_ASIA_AREA,//亚洲
	E_AUSTRALIAN_AREA,//澳大利亚
	E_MIDDLE_EAST_AREA,//中东
	E_BRAZIL_AREA//巴西
};
enum ENUM_WIFI_LINK_MODE
{
	E_WIFI_MODE_UNKNOW = 0x0,
	E_WIFI_MODE_AP = 0x1,
	E_WIFI_MODE_P2P = 0x2,
	E_WIFI_MODE_STA = 0x3
};

enum ENUM_KEY_ID
{
	k_PHONE_ANSWER = 0x10,
	k_PHONE_HANGUP = 0x11,
	K_VOLUME_UP = 0x12,
	K_VOLUME_DOWN = 0x13,
	K_VOLUME_MUTE = 0x14,
	K_TPYE_BACK = 0x03,
	K_TPYE_OK = 0x02,
	K_MUSIC_UP = 0x06,
	K_MUSIC_DOWN = 0x07,
	K_MUSIC_PLAY = 0x02,
	K_VOICE_WAKEUP = 0x9,
	K_MAX
};

enum ENUM_KEY_STATUS
{
	k_NO_VALUE = 0x0,
	K_MODE_UP = 0x1,
	K_MODE_DOWN = 0x2,
	K_MODE_LONG = 0x3,
};

enum ENUM_KEY_CPACTION
{
	CPACTION_NO = 0x0,
	CPACTION_ANSWER = 0x1,
	CPACTION_HANGUP= 0x2,
	CPACTION_PLAYPAUSE = 0x3,
	CPACTION_NEXT = 0x4,
	CPACTION_PREV = 0x5,
	CPACTION_RETURN = 0x6,
};

typedef struct STR_MILEAGE_INFO
{
	int nTotaMileValid;
	int nTotaMileValue;
	int nTotaMileUnit;
	int nTriMileValid;
	int nTriMileValue;
	int nTriMileUnit;
	int nRanMileValue;
	int nTriBMileValid;
	int nTriBMileValue;
	int nTriBMileUnit;
	STR_MILEAGE_INFO():nTotaMileValid(0),nTotaMileValue(0),nTotaMileUnit(0),nTriMileValid(0),\
			nTriMileValue(0),nTriMileUnit(0),nRanMileValue(0),nTriBMileValid(0),\
			nTriBMileValue(0),nTriBMileUnit(0)
	{

	}
}T_STR_MILEAGE_INFO;

typedef struct STR_CELL_INFO
{
	int nCellValid;
	int nCellValue;
	int nCellIndex;
	int nTimevalid;
	int nTimeUnit;
	int nTimeValue;
	STR_CELL_INFO():nCellValid(0),nCellValue(0),nCellIndex(0),nTimevalid(0),nTimeUnit(0),nTimeValue(0)
	{

	}

}T_STR_CELL_INFO;

typedef struct STR_ONCLICK_INFO
{
	long nWndId;
	long nCtrlId; 
	int nBtState;
	STR_ONCLICK_INFO():nWndId(0),nCtrlId(0),nBtState(0)
	{

	}
}T_STR_ONCLICK_INFO;

typedef struct BtPAIRINGREQ
{
	char name[128];
	int state; 
	BtPAIRINGREQ()
	{
		memset(name, 0 , sizeof(name));
		state = 0;
	}
}T_BtPAIRINGREQ;

typedef struct STR_SPEED_INFO
{
	int nValue;
	int nValid;
	int nUnit;
	STR_SPEED_INFO():nValue(0),nValid(0),nUnit(0)
	{

	}

}T_STR_SPEED_INFO, T_STR_POWER_INFO;

enum ENUM_SYS_TURN_MODE
{
	TURN_MODE_COMFORT = 0x0,
	TURN_MODE_SPORT = 0x1
};

enum ENUM_SYS_TIRE_RESET
{
	TIRE_RESET_NO = 0x0,
	TIRE_RESET_ING = 0x1,
	TIRE_RESET_FAILE = 0x2,
	TIRE_RESET_FINISH = 0x3 
};


typedef struct STR_SCREEN_LOCK_INFO
{
	int nScreenLocknum;//
	int nScreenLockStatus;//0 解锁 1 锁屏
	STR_SCREEN_LOCK_INFO():nScreenLocknum(1),nScreenLockStatus(0)
	{

	}

}T_STR_SCREEN_LOCK_INFO;

enum ENUM_SCREEN_LOCK
{
	SCREEN_LOCK_NO = 0x0,
	SCREEN_LOCK_UNLOCK = 0x1,
	SCREEN_LOCK_LOCK = 0x2,
};

enum ENUM_SYS_ENERGY_RECOVER
{
	ENERGY_RECOVER_STANDARD = 0x0,
	ENERGY_RECOVER_HIGH = 0x1
};

enum
{
	TIMEDATE_DD_MM_YY = 0,
	TIMEDATE_MM_DD_YY = 1,
};
typedef struct CANBOX_AIR_DATA
{
	bool air_mem;         //data0 bit7 显示空调菜单
	bool air_switch;      //data0 bit6 空调开关
	bool air_ac_max;      //data0 bit5 AC MAX
	bool air_rear_switch; //data0 bit4 后座空调开关
	bool air_auto;        //data0 bit3 AUTO
	bool air_sync;        //data0 bit2 SYNC
	bool air_Frt_air;     //data0 bit1 前排集中送风
	int air_unit;         //data0 bit0 温度单位 0：摄氏度；1：华氏度；
	bool air_rearkey;     //data1 bit7 Rear锁
	bool air_ac;          //data1 bit6 AC
	bool air_airdetect;    //data1 bit5 空气质量
	int air_inout_set;     //data1 bit4 内外循环 1：内循环 0:外循环
	bool air_inout_auto;   //data1 bit3 自动内外循环
	bool air_dual;         //data1 bit2 DUAL
	bool air_eco;          //data1 bit1 ECO
	bool air_airok;        //data1 bit0 空气净化
	bool rear_air_auto;    //data2 bit7 后排空调auto
	bool auto_remove;      //data2 bit6 自动除雾
	bool rear_remove;      //data2 bit5 后窗除雾
	bool front_remove;     //data2 bit4 前窗除雾
	int front_right_hot;   //data2 Bit 3~2 前右座椅加热
	int front_left_hot;    //data2 Bit 1~0 前左座椅加热
	int front_right_air;   //data3 Bit 7~6 前右座椅吹风
	int front_left_air;    //data3 Bit 5~4 前左座椅吹风
	bool right_win_hot;    //data3 bit3 前窗加热
	bool air_maxheat;      //data3 bit2 MAXHEAT
	bool auto_detect;      //data3 bit1 自动监测净化
	int air_mode; //data4 吹风模式  0x00:无 0x01:自动 0x03:吹脚 0x5:吹身吹脚 0x6:吹身 0xC:吹前窗吹脚 0xD:吹前窗吹身 0xE:吹前窗吹身吹脚    
	int air_speed; //data5 前排风速信息 0：关； 1~7：风速等级
	int air_left_temp;  //data6 前排左设定温度 Temp = Value * 0.5; 0xFE = Low_Temp; 0xFF =High_Temp
	int air_right_temp; //data7 前排右设定温度 Temp = Value * 0.5; 0xFE = Low_Temp; 0xFF =High_Temp
	int rear_air_mode;  //data8 后座空调吹风模式 0x00:无 0x01:吹脚 0x02:吹身 0x03:吹身吹脚
	int rear_airspeed;  //data9 后座风速信息 0:关 1~8：风速等级
	int rear_set_temp;  //data10 后座设定温度 Temp = Value * 0.5; 0xFE = Low_Temp; 0xFF =High_Temp
	int exter_temp;  //data11 外部温度 温度值=Data11*0.5-40
}T_CANBOX_AIR_DATA;

typedef struct CANBOX_CAR_DATA
{
	int car_park;     //data0 bit3 Park(泊车)
	int car_rev;      //data0 bit2 倒车
	int car_key;      //data2 0x0:无 0x1:音量加 0x2:音量减 0x8:上一曲 0x9:下一曲
	int wheel_angle;  //data6~data7 右转则为正数 左转则为负数 左转右转最大角度均为540值即从-540~+540（0xFDE4~0x021C）
}T_CANBOX_CAR_DATA;

typedef struct CANBOX_CAR_SETTING
{
	int turn_left;
	int turn_right;
}T_CANBOX_CAR_SETTING;
typedef struct DTS_EQ_SETTING
{
	int nEq_mode;
	int nListen_Pos;
	int nSurround_Level;
	int nDeep_Level;
	int nEvelate_Level;
}T_DTS_EQ_SETTING;
enum
{
	DTS_EQ_CLOSE = 0X00,
	DTS_EQ_NATURAL = 0x0D,
	DTS_EQ_CLEAR = 0x0E,
	DTS_EQ_BASS = 0x0F,
	DTS_EQ_RELAXING = 0x10,
};


// 单个协议配置结构体（对应INI里的一个[ID]段）
typedef struct {
    uint16_t id;            // 协议ID（如0x0001）
    char vendor[32];        // 供应商
    char car_series[64];    // 车系
    char car_type[64];      // 车型
    char so_name[128];      // so库路径
} T_CANBOX_PROTO_CONFIG;


// 临时解析上下文（用于INI回调函数）
typedef struct {
    uint16_t current_id;    // 当前解析的段ID
} ParseContext;

#endif /* APPDATADEF_H_ */
