/*
 * SettingView.h
 *
 *  Created on: 2021-08-03
 *      Author: Y
 */

#ifndef SETTINGVIEW_H_
#define SETTINGVIEW_H_

//View的实体，实现显示功能。
#include "IVIController.h"
#include "SettingData.h"
//#include "HandleViewUpdate.h"

//Dependent on external header files
#include "CompBase.h"
#include "IViewMessage.h"
#include "AppDatadef.h"
#include "FileBase.h"
#include<deque>
#include <ini.h>

#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

using std::map;
using FY_NAMESPACE::T_APP_MSG;
using FY_NAMESPACE::TimerBase;

#define MAX_PROTOCOL_CNT 2000  // 最大支持2000个协议配置，按需调整

class SettingView;
typedef bool (SettingView::*pUpdateSettingMsg)(T_APP_MSG* msg);

class  SettingView : public CompBase, public TimerBase
{
public:
	SettingView(ViewBase* p_ViewBase);
	virtual ~SettingView();

	/*
	 * call back UserMessge*/
	virtual bool SettingOnUserMessage(T_APP_MSG* msg);


	/*
	 * call back show window,Initialize user information*/
	virtual void SettingOnShow(LONG dwWndId);

	/*
	 * call back GUI operation status*/
	virtual void SettingOnComponent(CUIWndMng* pUI, PWINDOW_INFO pInfo);


	virtual void SettingOnListInfoMessage(CUIWndMng *pUI, int nWndId, int nCtrlId, PLIST_INFO pInfo);

	/*
	 *	界面触摸处理函数
	 *  pUI[IN] 窗口资源指针
	 *  nWndID[IN] 窗口ID 
	 *  nCtrlID[IN] 操作发生的控件ID
	 *  nBtState[IN] 控件的操作状态，包括按下、抬起、弹起时响应操作
	 */
	virtual void SettingOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);

	void SettingAttachController(IVIController* pController)
	{
		if(pController == NULL)
		{
			LOGDBG("pController == NULL");
		}
		else
		{
			LOGDBG("pController != NULL");
			m_ViewController = pController;
		}

	}

	void SetCurSysNode(T_UI_NODE nNode);
	void GetCurSysNode(T_UI_NODE &nNode);

	void UpdateVolumePage(T_APP_MSG* msg=nullptr);
	void UpdateComboboxVolumePage();
	bool SetMute(bool nValue, int mediovol, int btvol);

	void UpdateVersionGroup();

	//zxw
	void SaveCurInWnd(int nWnd);
	void SettingInitWorkWnd();
	void onClick_SettingList();
	void onClick_SettingBtn();
	void onClick_AudioBtn();
	void onClick_CarBtn();
	void onClick_ReturnBtn();
	void onClick_RestOKBtn();
	void onClick_RestCancelBtn();
	void onClick_DateTimeBtn(int nCtrlId);

	void UpateUI_DateTime(T_STR_TIME_INFO tTimeInfo);
	void onClick_BrightValue(int nToWard);
	void UpateUI_Version();

	void onClick_MainVolume(int nToWard);
	void onClick_BtVolume(int nToWard);
	void onClick_DtsSwitch();
	void onClick_RoadsideParkSwitch();
	void onClick_StorageParkSwitch();
	void onClick_RaderSoundSwitch();

	void OnHide_AudioBtn();
	void OnUpdateUI_AudioCtrl();

	void onClick_CloseView();

	void OnUpdate_DemoModeSwitch(bool bSwitch);

	int LoadProtoConfig(const char* ini_path);
	static int IniParseHandler(void* user, const char* section, const char* name, const char* value);

	// 生成ID文件相关
	int readCurrIdFromTxt(uint16_t* out_id);
	int writeCurrIdToTxt(uint16_t id);
	int ensureDirExists();
 
	T_CANBOX_PROTO_CONFIG m_sCBProto_Cur; //当前选的车型信息

protected:

	/*
	 * call back timer*/
	virtual void OnTimer(uint32_t id);

private:
		/*
	 * Initialize update message map*/
	void InitUpdateMsgMap();
	void GetUpdateMessageMap(map<int,pUpdateSettingMsg> &nMap);

	void onClickSystemView(CUIWndMng *pUI, long nWndId, long nCtrlId, int32_t nBtState, bool is_Need);

	void DrawAllItem();

	bool SetMedioVolume(int nVolume);
	bool SetBtVolume(int nVolume);
	bool SetKeyMute(bool nValue);
	bool SetbackLight(int nbackLight);
	bool SetbackLightMode(int Light, bool bAutoBg);
	bool SetBluetooth(int nBtState);

	bool SetUnitMode(int nUnitMode);
	bool SetReadLight(int nState);

	bool SetSystemInfo(T_APP_MSG* msg=nullptr);
	bool SetLanguage(int nlanguage);
	bool SetSyncTime(int state);
	bool SetCurTime(T_STR_TIME_INFO ntTime);

	bool UpdateVolumeBar(T_APP_MSG* msg=nullptr);
	bool UpdateBackLightBar(T_APP_MSG* msg=nullptr);

	void UpdateBackLightPage(T_APP_MSG* msg=nullptr);

	void UpdateTimePag();

	void HandleDateSet(int CtrlId);
	int GetMaxDays(int32_t nYear, int32_t nMonth);
	void HandleTimeSet(int CtrlId);

	void UpdateScreenVerPag();

	void OnClickBtVolBarList(int nOper, int nCtrlID);

	void OnClickComboboxBtVolBarList(int nWndId, int nOper, int nCtrlID);

	void SetSetCurrWnd(int nWndID);
	int GetSetCurrWnd();

	void SaveCurClickLine(const int nLine);
	void GetCurClickLine(int &nLine);

	void Lock(){m_Lock.Lock();}
	void UnLock(){m_Lock.Unlock();}	

	void ConverDecode(char *pInBuf,char *pOutBuf,size_t& nOutLen);

	void SetComboboxVolBarList(int nCtrlID,int nVolume);
	std::string GetPassWorldKey(long nCtrlID);
	void CanBoxOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);
	std::string GetEncodePassWorld(std::string passWorld);
	void SetOneListCurClickId(int nCtrlID);

	void OnInList_CanboxData(int nLevel,int nLine = 0, int nItemData = 0);

	void onClick_CanBoxList();

	void onClick_CanBoxConnect();
private:

	IVIController* m_ViewController;
	//HandleViewUpdate  m_HandleViewUpdate;

	map<int, pUpdateSettingMsg> m_MsgMap;
	int m_nSetCurrWnd;
	bool mis_Need;
	
	bool m_Mute;
	bool m_LastMute;
	bool m_KeyVol;
	bool m_LastKeyVol;
	bool m_AutoBg;
	bool m_LastAutoBg;
	bool m_LastBtSwitch;
	bool m_LastReadLight;
	bool m_LastSyncTime;
	int m_nBtnIdForQuickClick;
	int m_nBtnIdForQuickClick_OnComponent;
	T_STR_TIME_INFO m_tTime;
	bool m_bTimeflag;
	UI_Node m_CurrNode;
	MutexLock m_Lock;

	int m_LastIndex;

	int m_Line;

	int m_nInWnd;      //进入setting窗口

	int m_nDateTime_mode;
    string m_nCanBoxPassWorld;
	long m_nCanBoxCtrlId;
	long m_nCanBoxTwoCtrlId;
	long m_nCanBoxThreeCtrlId;
	int m_nCanBox_Level;
	int m_nCanBox_FirstLine; // 一级选择的行（协议厂商）
	string m_strCanBox_Name;
	string m_strCanBox_CarType;
	string m_strCanBox_CarProctol;

	static T_CANBOX_PROTO_CONFIG m_sCBProto_Configs[MAX_PROTOCOL_CNT]; //存所有配置信息
	static int g_proto_config_cnt;  // 实际解析到的协议 数量
	T_CANBOX_PROTO_CONFIG m_selectProto; //所选的车型号
	T_CANBOX_PROTO_CONFIG m_tmpSelectVen;// 二级列表选的 存放三级列表要显示的所有协议信息
	T_CANBOX_PROTO_CONFIG m_tmpSeAllProto[MAX_PROTOCOL_CNT];//  存放三级列表要显示的所有协议信息
};

#endif /* SETTINGVIEW_H_ */
