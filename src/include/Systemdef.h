/*
 * Systemdef.h
 *
 *  Created on: 2021
 *      Author: 
 */

#ifndef SYSTEMDEF_H_
#define SYSTEMDEF_H_

#include <string>
#include <assert.h>
#include "SPBlueTooth.h"
#include "SPWifiManager.h"
#include "SourceDef.h"
using namespace spbt;
using std::string;
using namespace network;

#include "IViewMessage.h"

#define WIRELESS_SERVER_TEST_BT 0  //调试开关
#define WIRELESS_SERVER_TEST_APWIFI 0  //调试开关
#define WIRELESS_SERVER_TEST_P2PWIFI 0  //调试开关
#define WIRELESS_SERVER_TEST_STAWIFI 0  //调试开关
#define WIRELESS_SERVER_WIFI_SYS_STARTED_AP              0    //开机是否启动了AP        

//#define MAX_DATA_LEN                300

#define BT_ADDR_LENGTH	(6)
#define BT_MAX_REMOTE_NAME_LENGTH (30)
#define BT_MAX_LOCAL_NAME_LENGTH (30)
#define MAX_PHONENUMBERLIST_LENGTH (128)

#define BT_MAX_CALLNUMBER_LENGTH (36) /*!<max contact number length*/
#define BT_MAX_CALLNAME_LENGTH   (128)//(256) /*!<max contact name length*/
//#define BT_MAX_CALLNAME_LENGTH (256) /*!<max contact name length*/
//#define BT_MAX_CALLHISTORY_TIME_LEN (21) /*!<max timestamp length*/
#define BT_MAX_PHONEBOOKNUM (1000) /*!<max timestamp length*/
//#define BT_MAX_HISTORYNUM (20) /*!<max timestamp length*/
#define WIFI_MAX_NAME_LEN (64)
#define WIFI_MAX_PASSWORD_LEN (64)
#define WIFI_MAX_IF_NAME (16)
#define WIFI_DEF_LEN (16)
#define WIFI_DEF_MAC_LEN (32)


#define WIFI_DEF_IFNAME "wlan0"
#define WIFI_DEF_PASS "88888888"
#define WIFI_DEF_NAME "qjxxxx"
#define WIFI_DEF_OTA_NAME "qjota"

// for Bluetooth Audio
#define BT_MAX_AVRCP13_INFO_CHAR_SIZE                   (255+1)/*!<the last char is always 0.this define must <= 256(MAX_AVRCP13_MEDIA_INFO_CHAR_SIZE+1)*/
#define BT_MAX_AVRCP14_INFO_CHAR_SIZE                   (63+1) /*!<the last char is always 0.this define must <= 64(MAX_AVRCP14_MEDIA_INFO_CHAR_SIZE+1)*/
#define BT_AVRCP_FEATURE_BIT_MASK_LENGTH               (16)
#define BT_PAIRLIST_MAX_MUN                            (20)
#define BT_PAIREDLIST_MAX_MUN 							(5)


enum TIMER_ID
{
	TIMER_ID_BASE,
	TIMER_ID_BOOK_SYNC,
	TIMER_ID_BT_SWITCH_STATE,//蓝牙开关状态定时回调
	TIMER_ID_BT_NAME,  //蓝牙名称定时回调
	TIMER_ID_MAC,     //mac地址回调
	TIMER_ID_BT_RECONNECTION_A2DP,
	TIMER_ID_BT_RECONNECTION_HFPHF,
	TIMER_ID_BT_ACC_OFF_TURN_OFF_BT,
	TIMER_ID_BT_ACC_OFF_SET_DENY_CONNECTION,
	TIMER_ID_BT_CARPLAY_ACTIVE_SET_DENY_CONNECTION,
	TIMER_ID_BT_PAIRING_REQ,
	TIMER_ID_BT_ACC_ON_PLAY_BTMUSIC,
	//TIMER_ID_BT_ACC_ON_PAUSE_BTMUSIC,
	TIMER_ID_SET_BTMUSIC_REPLAY_FUNCTION,
};

enum AD_BTState
{
	AD_BT_ON, /*!<bluetooth is on*/
	AD_BT_OFF /*!<bluetooth is off*/
} ;

enum AD_BTSetState
{
	AD_BT_SET_OFF, 
	AD_BT_SET_ON 
} ;


enum AD_STAState
{
	AD_STA_DISCONNECT, /*!<STA is off*/
	AD_STA_CONNECT /*!<STA is on*/
} ;


enum AD_IVI_BTCONNECT_CMD
{
	IVI_BTCONNECT_CMD_CONNECT, 
	IVI_BTCONNECT_CMD_DISCONNET, 
	IVI_BTCONNECT_CMD_FORGET,
	IVI_BTCONNECT_CMD_CHANCEL
} ;

enum ACC_MODE
{
	ACC_OFF, //00
	ACC_ON, //01
};

enum AD_CallAction
{
	AD_BT_ACCEPT_CALL,
	AD_BT_REJECT_CALL,
};
	
enum AA_ActiveState
{
	AA_DEACTIVE,  //00
	AA_ACTIVE,    //01
};

enum Carplay_ActiveState
{
	CARPLAY_DEACTIVE,  //00
	CARPLAY_ACTIVE,    //01
};

enum BTCall_State
{
	BTCALL_STATE_IDLE,        //00
	BTCALL_STATE_INCOMING,    //01
	BTCALL_STATE_OUTGOING,    //02
	BTCALL_STATE_PRORESS,     //03 
	BTCALL_STATE_HOLD,        //04
	BTCALL_STATE_PARTIALHOLD, //05
};

typedef enum
{
	pbapContinue,//同步中
	pbapFinish,//同步完成
	pbapError,//同步失败
} BtDownloadStatus;


enum Phonebook_Sync_state
{
	PHONEBOOK_SYNC_STATE_BASE,
	PHONEBOOK_SYNC_STATE_PHONE,
	PHONEBOOK_SYNC_STATE_SIM,
	PHONEBOOK_SYNC_STATE_FRESH
};

enum AD_BTDeviceState
{
	AD_BT_DEVICE_UNPAIRED, //00/*!<remote device is unpaired (not bonded)*/
	AD_BT_DEVICE_DISCONNECTED, //01/*!<remote device is paired (bonded) but not connect to any profiles*/
	AD_BT_DEVICE_CONNECTED, //02/*!<remote device is connected to some profile*/
	AD_BT_DEVICE_CONNECTING //03/*!<remote device is connecting to some profile*/
};

enum ACC_ON_BT_RECONNECT
{
	ACC_ON_BT_SHALL_NOT_RECONNECT, //00
	ACC_ON_BT_SHALL_RECONNECT,  //01
};

enum AUDIO_STATUS_TO_IVIAPP
{
    AUDIO_STATUS_TO_IVIAPP_BTMUSIC_NOT_REPLAY, //00
	AUDIO_STATUS_TO_IVIAPP_BTMUSIC_REPLAY, //01
};

enum SET_ACC_OFF_BT_AUDIO_PLAY_STATE
{
	SET_ACC_OFF_BT_AUDIO_PLAY_STATE_OFF,
    SET_ACC_OFF_BT_AUDIO_PLAY_STATE_ON,
};

enum BTMUSIC_REPLAY
{
	BTMUSIC_SHALL_NOT_REPLAY, //00
	BTMUSIC_SHALL_REPLAY,  //01
};

enum SLEEP_WAKE_UP_BT_RECONNECT
{
	SLEEP_WAKE_UP_BT_SHALL_NOT_RECONNECT, //00
	SLEEP_WAKE_UP_BT_SHALL_RECONNECT,  //01
};

enum SLEEP_WAKE_UP_BTMUSIC_REPLAY
{
	SLEEP_WAKE_UP_BTMUSIC_SHALL_NOT_REPLAY, //00
	SLEEP_WAKE_UP_BTMUSIC_SHALL_REPLAY, //01
} ;

typedef enum
{
	BtSuccess, /*!<operation success (not the function is success, some operation should check related event callback to see the function result)*/
	BtFail, /*!<operation fail*/
} BtModuleResult;


typedef enum
{
	TellCell,/*!<call type is cell*/
	TellHome,/*!<call type is home*/
	TellWork,/*!<call type is work*/
	TellOther,/*!<call type is other*/
} BtPhoneTelType;


enum WifiMode
{
	AD_UNKOWN_MODE,
	AD_AP_MODE,
	AD_P2P_MODE,
	AD_STA_MODE,
	AD_WIFI_DISCONNECTED
};

enum WIFI_STATE
{
	WIFI_STATE_UNKNOW,                     //UNKNOW 状态
	WIFI_STATE_AP_CONNECTED,		        //AP 已连接					
	WIFI_STATE_P2P_CONNECTED,				//P2P 已连接
	WIFI_STATE_STATION_CONNECTED,		    //STA 已连接
	WIFI_STATE_STA_CONNECTION_FAILED,       //STA Connection Failed
	WIFI_STATE_DISCONNECTED					//WiFi 已断开

};

enum STA_EVENT
{
	WIFI_STA_EVENT_SCAN_SUCCESS,            //SCAN_SUCCESS
	WIFI_STA_EVENT_SCAN_FAILURE,            //SCAN_FAILURE
	WIFI_STA_EVENT_CONNECT_SUCCESS,         //CONNECT_SUCCESS
	WIFI_STA_EVENT_CONNECT_FAILURE,         //CONNECT_FAILURE
	WIFI_STA_EVENT_DISCONNECTED,            //DISCONNECTED
	WIFI_STA_EVENT_UPDATE_NETWORKS,         //UPDATE_NETWORKS
	WIFI_STA_EVENT_UNKNOWN                  //UPDATE_UNKNOWN
};

enum STA_CONNECT_FAILURE_REASON
{
	STA_CONNECT_FAILURE_UNKNOWN,            //*   Default value, unknown reason    */
	STA_CONNECT_FAILURE_WRONG_KEY,          //*   The station connection failed because the password is wrong    */              
	STA_CONNECT_FAILURE_AUTH_FAILED,        //*   The station connection failed because authentication failed    */
	STA_CONNECT_FAILURE_CONN_FAILED,        //*   The station connection failed because the association failed    */
};

enum SoundSourceState
{
	NO_SOUND_SOURCE = 0,
	CP_SOUND_SOURCE = 1,
	AA_SOUND_SOURCE = 2,
	YL_SOUND_SOURCE = 3,
	RADIO_SOUND_SOURCE = 4,
	BT_SOUND_SOURCE = 5,
};

/*enum BTAudioStatus
{
 AUDIO_STATUS_STOP = 0,
 AUDIO_STATUS_PLAY = 1,
 AUDIO_STATUS_PAUSE = 2,
 AUDIO_STATUS_FWD_SEEK = 3,
 AUDIO_STATUS_REV_SEEK = 4,
 AUDIO_STATUS_UNKNOWN = 0xFF,
};*/

typedef struct _BtSrvEvent {
	BTAddr addr;
	BTServiceEvent Event;
	void*  pValue;
	
	char name[BT_MAX_REMOTE_NAME_LENGTH]; /*!< device name */
    unsigned int pass_key; /* !< passkey */
	/*
	_BtSrvEvent()
	{
		memset(&Event, 0, sizeof(Event));
		pValue = NULL;
	}

	_BtSrvEvent(const _BtSrvEvent & a)
	{
		addr = a.addr;
		Event = a.Event;
		pValue = a.pValue;
	}

	_BtSrvEvent(BTAddr * addr, BTServiceEvent e, void *value)
	{
		memcpy(&(this->addr), addr, sizeof(BTAddr));
		Event = e;
		this->pValue = value;
	}
	*/
}BtSrvEvent_t;

typedef struct _wirelessEventData{
	WIRELESS_EVENT event;
	void *pData;
	~_wirelessEventData()
	{
		event = (WIRELESS_EVENT)0;
		pData = NULL;
	}
	
}WirelessEventData_t;


typedef struct
{
	int status;
	unsigned int recordNum;
}BtSyncResultData_t;

typedef struct {
	BTAddr addr;
	BTProfileType profileType;
	BTConnState state;
	BTState btstate;
}BtConnEvent_t;

typedef enum {
    pair_passkey_confirmation, /*!< passkey display for confirmation */
    pair_pin_request, /*!< PIN code request */
} BtPairingConfirmVariant;

typedef struct BtPairingReq_t_ {
    BTAddr addr; /*!< device address */
    char name[BT_MAX_REMOTE_NAME_LENGTH]; /*!< device name */
    unsigned int cod; /* !< class of device */
    BtPairingConfirmVariant pairing_variant; /* !< pairing confirmation type */
    unsigned int pass_key; /* !< passkey */
} BtPairingReq_t;

typedef struct BTPairingReqInfo_t_ {
    char name[BT_MAX_REMOTE_NAME_LENGTH]; /*!< device name */
    unsigned int pass_key; /*device pass key*/
    int state;        // Window state
    BTAddr addr;
} BTPairingRegInfo_t;

//device status
typedef enum {
    BtDeviceUnPair, /*!<remote device is unpaired (not bonded)*/
    BtDeviceDisConnect, /*!<remote device is paired (bonded) but not connect to any profiles*/
    btDeviceConnect, /*!<remote device is connected to some profile*/
} BtDeviceState;

typedef struct    {
	BTAddr addr;
	BTPhonebookEvent event;
	void*  pValue;
}BtPhonebookEvent_t;


typedef struct    {
	BTAddr addr;
	BTPhoneEventType event;
	int value;
}BtPhoneEvent_t;


typedef struct    {
	BTAddr addr;
	BTAudioEventType event;
	void*  pValue;
}BTAudioEvent_t;

typedef struct _BtCallInfo{

	BTAddr addr;
	unsigned char phoneNumber[SPBT_MAX_PHONENUMBERLIST_LENGTH];
	unsigned char name[BT_MAX_CALLNAME_LENGTH];
	unsigned char fName[BT_MAX_CALLNAME_LENGTH];
	int  state;
	
	void __RESET()
	{
		memset(&addr,0,sizeof(BTAddr));
		memset(&phoneNumber,0,sizeof(MAX_PHONENUMBERLIST_LENGTH));
		memset(&name,0,sizeof(BT_MAX_CALLNAME_LENGTH));
		memset(&fName,0,sizeof(BT_MAX_CALLNAME_LENGTH));
		state = 0;
	}

}BtCallInfo_t;

typedef struct _BtCallInfoAA{

	unsigned char BTCharAddr[6];
	unsigned char phoneNumber[SPBT_MAX_PHONENUMBERLIST_LENGTH];
	int  state;
	
	void __RESET()
	{
		memset(BTCharAddr, 0, 6);
		memset(&phoneNumber,0,sizeof(MAX_PHONENUMBERLIST_LENGTH));
		state = 0;
	}
}BtCallInfoAA_t;

/*
typedef struct _BtLocalInfo{

	unsigned char BTName[30];
	unsigned char BTCharAddr[128];
	//BTAddr addr;
	int nBTState;
	void __RESET()
	{
		memset(BTName, 0, 30);
		nBTState = 0;
		//memset(&addr,0,sizeof(BTAddr));
		memset(BTCharAddr, 0, 128);
	}

}BtLocalInfo_t;*/




typedef struct
{
	unsigned char number[BT_MAX_CALLNUMBER_LENGTH];/*!<contact number*/
	BtPhoneTelType type;	/*!<contact was from cell, home, or work, refer to BTPhoneTelType_e enum*/
} BtPhonebookTelephoneAttr;

typedef struct
{
	unsigned char N_name[BT_MAX_CALLNAME_LENGTH];   /*!<contact N name*/
	unsigned char FN_name[BT_MAX_CALLNAME_LENGTH];  /*!<contact FN name*/
	BtPhonebookTelephoneAttr telephoneAttr;/*!<telephone attr*/
	unsigned int groupID;/*!<group id,start from 1*/
} BtPhonebookRecord;


typedef struct _BTMusicInfo{
	BTAddr addr;
    unsigned int dCurrentTrack;/*!<current track*/
	unsigned int dTotalTracks;/*!<total tracks*/
	unsigned int dCurrentTime;/*!<current time(millisec)*/
	unsigned int dTotalTime;/*!<total time(millisec)*/
    unsigned char abMusicTitle[SPBT_MAX_AVRCP13_INFO_CHAR_SIZE];/*!<title name*/
	BTAudioStatus state;
	void __RESET()
	{
		memset(&addr, 0, sizeof(BTAddr));
        dCurrentTrack = 0;
		dTotalTracks = 0;
		dCurrentTime = 0;
		dTotalTime = 0;
		memset(&abMusicTitle, 0, sizeof(SPBT_MAX_AVRCP13_INFO_CHAR_SIZE));
        memset(&state, 0, sizeof(BTAudioStatus));
	}
} BTMusicInfo_t;


typedef struct {
    unsigned int dCurrentTrack;/*!<current track*/
    unsigned int dTotalTracks;/*!<total tracks*/
    unsigned int dPlayingTime;/*!<total playing time(millisec)*/
    unsigned char abTitle[BT_MAX_AVRCP13_INFO_CHAR_SIZE];/*!<title name*/
    unsigned char abArtist[BT_MAX_AVRCP13_INFO_CHAR_SIZE];/*!<artist name*/
    unsigned char abAlbum[BT_MAX_AVRCP13_INFO_CHAR_SIZE];/*!<album name*/
    unsigned char abGenre[BT_MAX_AVRCP13_INFO_CHAR_SIZE];/*!<genre name*/
} BtMusicInfo;

typedef struct {
    unsigned int dCurrentTrack;/*!<current track*/
    unsigned int dTotalTracks;/*!<total tracks*/
    unsigned int dPlayingTime;/*!<total playing time(millisec)*/
    char abTitle[BT_MAX_AVRCP13_INFO_CHAR_SIZE];/*!<title name*/
    unsigned char abArtist[BT_MAX_AVRCP13_INFO_CHAR_SIZE];/*!<artist name*/
    char abAlbum[BT_MAX_AVRCP13_INFO_CHAR_SIZE];/*!<album name*/
    unsigned char abGenre[BT_MAX_AVRCP13_INFO_CHAR_SIZE];/*!<genre name*/
} BtMusicInfo_t;


typedef struct {
    unsigned int dCurrentTime;/*!<current time(millisec)*/
    unsigned int dTotalTime;/*!<total time(millisec)*/
} BtPlaybackTime;

typedef struct {
    unsigned int dCurrentTime;/*!<current time(millisec)*/
    unsigned int dTotalTime;/*!<total time(millisec)*/
} BtPlaybackTime_t;

typedef struct
{
	unsigned int curNumOfRecord;/*!<the number of records at BTPhonebookRecord_t* pstRecord,it indicates number of telephone*/
	BtPhonebookRecord pstRecord[BT_MAX_PHONEBOOKNUM];/*!<return user requested phonebook data stored at pstRecord */
} BtPhoneBookData;

//add by zzp 
#define BT_MAX_CALLNUMBER_LENGTH (36) /*!<max contact number length*/
#define BT_MAX_CALLNAME1_LENGTH   (64)//(256) /*!<max contact name length*/
typedef struct
{
        unsigned char number[BT_MAX_CALLNUMBER_LENGTH];/*!<contact number*/
        unsigned char N_name[BT_MAX_CALLNAME1_LENGTH];   /*!<contact N name*/
} BtPhoneBookSyncData;

typedef struct
{
		unsigned int index;
        unsigned int curNum;/*!<the number of records at BTPhonebookRecord_t* pstRecord,it indicates number of telephone*/
		unsigned int packnum;
        BtPhoneBookSyncData BtPhoneBookInfo[20];
} BtPhoneBookSync;

typedef struct
{		unsigned int BtPhoneContacts;
        BtPhoneBookSyncData BtPhoneBookInfo[1000];		//最多显示1000联系人
} BtPhoneContactData;

typedef struct
{
        unsigned char number[BT_MAX_CALLNUMBER_LENGTH];/*!<contact number*/
        unsigned char N_name[BT_MAX_CALLNAME1_LENGTH];/*!<contact  N_name*/
} BtRecordInfo_t;

typedef struct
{
        unsigned char count;
        unsigned char state;
        BtRecordInfo_t RecordInfo[20];        
} BtRecordData_t;

typedef struct _BTAddr
{
	uint8_t address[BT_ADDR_LENGTH];

	bool operator==(const _BTAddr& rhs) const {
        return memcmp(address, rhs.address, BT_ADDR_LENGTH) == 0;
    }

    // 重载 != 运算符（可选）
    bool operator!=(const _BTAddr& rhs) const {
        return !(*this == rhs);
	}
} BtAddr;
const BtAddr EMPTY_BT_ADDR = {0};

typedef enum
{
	DisConnected, /*!<profile dieconnected state*/
	Connecting, /*!<profile connecting state*/
	Connected, /*!<profile connected state*/
	Disconnecting, /*!<profile disconnecting state*/
} BtConnState;
typedef struct _BtBondedDeviceInfo
{
        BtAddr addr;
        char name[BT_MAX_REMOTE_NAME_LENGTH];
        BtConnState connState;
} BtBondedDeviceInfo;

typedef struct _BtBondedDevice 
{
    int num;
    BtBondedDeviceInfo *deviceinfo;
} BtBondedDevice;

typedef struct _BtPairRequestFirst 
{
    int index;
    AD_IVI_BTCONNECT_CMD state;
} BtPairRequestFirst;

typedef struct
{
    int num;
	int index[BT_PAIRLIST_MAX_MUN];
	int connState[BT_PAIRLIST_MAX_MUN];
	char BTDeviceName[BT_PAIRLIST_MAX_MUN][30];
	BtAddr BTCharAddr[BT_PAIRLIST_MAX_MUN];
}BtBondedDeviceToAppInfo_t;

typedef struct
{
    int num;
	int index[BT_PAIREDLIST_MAX_MUN];
	int connState[BT_PAIREDLIST_MAX_MUN];
	char BTDeviceName[BT_PAIREDLIST_MAX_MUN][30];
	uint8_t BTCharAddr[BT_PAIREDLIST_MAX_MUN][6];
}BtPairedDeviceToAppInfo_t;

//联系人信息
typedef struct{
	unsigned int groupID;
	unsigned int telType;
	string name;
	string fName;
	string phoneNum;	
}contactInfo_t;

typedef struct{
    string mIfName;                 /*!< Name of the network device that reported this event */
    string mStaMacAddr;             /*!< MAC address of the WiFi station */
    WifiApEventType mApEventType;   /*!< AP event type */

	void _Reset()
	{
		mIfName ="";
		mStaMacAddr ="";
		mApEventType = AP_EVENT_UNKNOWN;
	}
	
}WifiAPEvent_t;


typedef struct{
    string mIfName;      /*!< Name of the network device that reported this event */
    string mPeerMacAddr; /*!< MAC address of the peer P2P device */
    WifiP2pEventType mP2pEventType;  /*!< P2P event type */
    WifiP2pDevice* p2pDevice;        /*!< Peer P2P device information */
	void _Reset()
	{
		mIfName = "";
		mPeerMacAddr = "";
		mP2pEventType = P2P_EVENT_UNKNOWN;
		p2pDevice = NULL;
	}
}WifiP2pEvent_t;


typedef struct{
    string mIfName;                 /*!< Name of the network device that reported this event */
    string mApSsid;                 /*!< The name of the peer wifi AP */
    string mBssid;                  /*!< BSSID, Equivalent to the MAC address of the WiFi AP */
    WifiStaEventType mStaEventType; /*!< Station event type */
    WifiStaReasonType mReason;      /*!< Reasons why certain actions failed */

	void _Reset()
	{
		mIfName = "";
		mApSsid = "";
		mBssid = "";
		mStaEventType = STA_EVENT_UNKNOWN;
		mReason = STA_REASON_UNKNOWN;
	}
}WifiStaEvent_t;

typedef struct _wifiInfo{
	char name[WIFI_MAX_NAME_LEN];//64
	char ifName[WIFI_MAX_IF_NAME];//16
	char password[WIFI_MAX_PASSWORD_LEN];//64
	char hwMode[WIFI_DEF_LEN];//ap           g：2.4G             n:5G
	char security[WIFI_DEF_LEN];
	char mac[WIFI_DEF_MAC_LEN];
	uint8_t channel;//信道
	uint8_t mode;//ap ,sta,p2p	
	uint8_t freq;// P2P 2: 2G 5:5G

	uint8_t beaconInt;
	

	void __RESET()
	{
		memset(name,0,sizeof(WIFI_MAX_NAME_LEN));
		memset(ifName,0,sizeof(WIFI_MAX_IF_NAME));
		memset(password,0,sizeof(WIFI_MAX_PASSWORD_LEN));
		memset(hwMode,0,sizeof(WIFI_DEF_LEN));
		memset(security,0,sizeof(WIFI_DEF_LEN));
		memset(mac,0,sizeof(WIFI_DEF_MAC_LEN));
		channel = 0;
		mode = 0;
		freq = 0;
		
		beaconInt = 0;
	}
}ADWifiInfo_t;

typedef struct _apConfig{
	string ssid;
	string passWord;
	string channel;
	string ifName;
}ApConfigInfo_t;


#endif /* SYSTEMDEF_H_ */
