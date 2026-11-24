/*
 * IVIView.h
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#ifndef IVIVIEW_H_
#define IVIVIEW_H_

//View的实体，实现显示功能。
#include "IVIController.h"
#include "HomeData.h"
//Dependent on external header files
#include "ViewBase.h"

#include "HomeView.h"
#include "SettingView.h"
#include "RadioView.h"
#include "MediaView.h"
#include "BTView.h"
#include "EqView.h"
#include "AirConditionar.h"
#include "DtsView.h"
#include "SplitView.h"

#include "CanBoxProto_Base.h"
#include "CanBoxProto_Def.h"
#include <dlfcn.h>


using std::map;
using FY_NAMESPACE::T_APP_MSG;
using FY_NAMESPACE::TimerBase;
using FY_NAMESPACE::ViewBase;

class IVIView : public ViewBase, public TimerBase{
public:
	IVIView();
	virtual ~IVIView();

	static IVIView &GetInstance()
	{
   	 	static IVIView instance;
    	return instance;
	}

	/*
	 * Initialize View parameter*/
	bool Init(int argc, char **argv,
			std::string path,
			std::string appName,
			std::string resourceName,
			std::string strScreenID,
			std::string strFontPath,
			int32_t nWinWidth = UIWND_WIDTH_UNKNOWN,
			int32_t nWinHeight = UIWND_HEIGHT_UNKNOWN,
			DWORD nGuiRenderMode = 0/* CLASSIC_RENDER_MODE */);

	/*
	 * Binding Controller object*/
	void AttachController(IVIController* pController)
	 {
		m_ViewController = pController;
		m_SettingView.SettingAttachController(pController);
		m_HomeView.HomeAttachController(pController);
		m_RadioView.RadioAttachController(pController);
		m_MediaView.MediaAttachController(pController);
		m_BTPhoneView.BTAttachController(pController);
		m_EqView.EqAttachController(pController);
		m_AirConditionar.AirAttachController(pController);
		m_DtsView.DtsAttachController(pController);
		m_SplitView.SplitAttachController(pController);
	}

	/*
	 * Show First Window*/
	void ShowFirstWnd(std::string strParam);

	virtual void OnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);
	
	void SendResourceType(const int nType);

	void OnIVI_WinShow(int nSource);

	void SaveIn_SettingWnd(int nWnd);

	void OnHandleKey(T_APP_MSG *msg);

	void OnSplitScreenChange(int nSplit, int nAppType);

	void OnHandle_PopAudio(int nToWard);
	void OnHideAudio_Pop();

	void OnUpdateTitle_Time();
	bool isLeapYear(int year);
	int CalculateWeekday(int year, int month, int day);

	void SendMedia_Pause();
	void SetVideoLayerVisible(bool bVisible);
	void SetWindow_BK(int nWnd,int nPicId);

	void Save_CurSource(int nSource);
	int Get_CurSource();

	void OnHandle_Source(int nSource);

	void SendBtAudio_Pause();
	
	int GetCurrWnd();

	int CanBoxOnInit();

	void OnHide_DemoView(bool bCntShow = true);

	CanDataBase* pCanBoxProto;

	RadioView*  GetRadioView()  { return &m_RadioView; }
	BTView*     GetBTView()     { return &m_BTPhoneView; }
	MediaView*  GetMediaView()  { return &m_MediaView; }
	SplitView*  GetSplitView()  { return &m_SplitView; }
	void        ShowSplitWnd(bool bSplit);   // 分屏(30)/全屏CP-AA(13) 统一切换
	void        CloseSplitWnd();             // CP/AA断开时收起分屏（不清状态）
	void        UpdateSplitMediaView();   // 分屏在显示时刷新媒体卡片
protected:
	/*
	 * call back APPS Message info*/
	virtual void OnAppsMessage(T_APP_MSG* msg);

	/*
	 * call back UserMessge*/
	virtual bool OnUserMessage(T_APP_MSG* msg);

	/*
	 * call back GUI operation status*/
	virtual void OnComponent(CUIWndMng* pUI, PWINDOW_INFO pInfo);

	virtual void OnListInfoMessage(CUIWndMng *pUI, int nWndId, int nCtrlId, PLIST_INFO pInfo);

	/*
	 * call back show window,Initialize user information*/
	virtual void OnShow(LONG dwWndId, bool bShow, int nStyle = 0);

	/*
	 * call back timer*/
	virtual void OnTimer(uint32_t id);

		//鼠标或者触摸屏移动消息，如果有特殊处理可以重载
	//如果返回值为false，则不再向GUI层传递此消息，消息被拦截
	virtual bool OnMouseMove(POINT point);

	//鼠标或者触摸屏移动消息，如果有特殊处理可以重载
	//如果返回值为false，则不再向GUI层传递此消息，消息被拦截
	virtual bool OnLButtonDown(POINT point);

	//鼠标或者触摸屏移动消息，如果有特殊处理可以重载
	//如果返回值为false，则不再向GUI层传递此消息，消息被拦截
	virtual bool OnLButtonUp(POINT point);
	
private:

	void DrawAllItem();


	void Lock(){m_Lock.Lock();}
	void UnLock(){m_Lock.Unlock();}

	void InitWorkWnd(int dwWndId);

	/*
	 * Save Current Window ID; */
	void SetCurrWnd(int nWndID);
	
	void SetLastWnd(int nWndID);
	int GetLastWnd();
	
	bool UpdateWndChange(T_APP_MSG* msg=nullptr);
	
	void UpdateHomeContenInfo(const int nCtrlId, const int nBtStat, const bool nShow);

	void OnTimer_DemoView();
private:
	IVIController* m_ViewController;

	int m_nCurrWnd;
	int m_nLastWnd;
	bool m_bClosingSplit = false;
	int m_nSplitRepaintCnt = 0;      // 分屏补画剩余次数
	int m_nSplitFixCnt = 0;          // 分屏看护连续抬层次数（防止无限抬层闪烁）
	MutexLock m_MutexSelfCheck;
	MutexLock m_MutexAutoTest;
	MutexLock m_Lock;


	HomeView m_HomeView;
	SettingView m_SettingView;
	RadioView m_RadioView;
	MediaView m_MediaView;
	BTView m_BTPhoneView;
    EqView m_EqView;
	AirConditionar m_AirConditionar;
	SplitView m_SplitView;
	DtsView m_DtsView;

	int m_DownpointX;
	int m_DownpointY;
	int m_Uppoint;
	map<int, string> m_charKerbordMap; //<ID, KEY>

	int m_nCurSource;

	int m_nCurPicIndex;


};

#endif /* IVIVIEW_H_ */
