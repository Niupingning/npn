/*
 * BTView.h
 *
 *  Created on: 2025-09-25
 *      Author: Y6915
 */
 
 #ifndef BTVIEW_H_
 #define BTVIEW_H_
 
 //View的实体，实现显示功能。
#include "BTData.h"

#include "CompBase.h"
#include "IViewMessage.h"
#include "AppDatadef.h"
#include "FileBase.h"
#include "FileList.h"
#include "IVIController.h"
#include <vector>
 
using std::map;
using FY_NAMESPACE::T_APP_MSG;
using FY_NAMESPACE::TimerBase;

class BTView;
typedef bool (BTView::*pUpdateBTMsg)(T_APP_MSG* msg);

class BTView : public CompBase, public TimerBase{
public:
	BTView(ViewBase* p_ViewBase);
	virtual ~BTView();

	virtual void BTOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);
	virtual void OnClick_BTUnconnected(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);
	
	virtual void BTPhoneOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);

	/*
	 * call back UserMessge*/
	virtual bool BTOnUserMessage(T_APP_MSG* msg);

	/*
	 * call back GUI operation status*/
	virtual void BTOnComponent(CUIWndMng* pUI, PWINDOW_INFO pInfo);

	/*
	 * call back timer*/
	virtual void OnTimer(uint32_t id);
	
	void BTInitMusicWorkWnd();
	void BTInitPhoneWorkWnd();

	bool UpdateBTMusicInfo(T_APP_MSG* msg);
	bool UpdateBTMusicTime(T_APP_MSG* msg);
	bool UpdateBTMusicPlay(T_APP_MSG* msg);
	bool UpdateBTConnectSts(T_APP_MSG* msg);
	bool UpdateBTConnectDevice(T_APP_MSG* msg);
	bool UpdateBTLocalInfo(T_APP_MSG* msg);
	bool UpdateBTDiscoverList(T_APP_MSG* msg);
	bool UpdateBTPairReq(T_APP_MSG* msg);

	void BTAttachController(IVIController* pController)
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
	void ConverDecode(char *pInBuf,char *pOutBuf,size_t& nOutLen);

	void OnBtMusic_WinShow();
	void OnBtCall_WinShow();
	void ResetCallUI();            // 通话界面与状态整体复位
	void HideAllCallPages();       // 三个整页通话分组互斥
	void OnNeed_Pause();
	void OnNeed_Play();
private:

	/*
	 * Initialize update message map*/
	void BTInitUpdateMsgMap();
	void BTGetUpdateMessageMap(map<int,pUpdateBTMsg> &nMap);

	void Lock(){m_Lock.Lock();}
	void UnLock(){m_Lock.Unlock();}

	int GetCurrWnd();

	//void OnClick_BTUnconnected(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);
	void OnClick_BTPhoneWnd(const int nCtrlId, const int nBtStat, const bool nShow);
	void OnClick_BTMusicPlayWnd(const int nCtrlId, const int nBtStat, const bool nShow);
	void OnClick_BTPairedList(const int nCtrlId, const int nBtStat, const bool nShow);
	void OnClick_BTPairedReq(long nWndId, const int nCtrlId, const int nBtStat, const bool nShow);
	void OnClick_BTPhoneKeyborad(long nWndId, const int nCtrlId, const int nBtStat, const bool nShow);
	int InputLen(wchar_t *KbInput);
	
	void BTUpdateHomeContenInfo(long nWndId,const int nCtrlId, const int nBtStat, const bool nShow);
	void BTPhoneUpdateHomeContenInfo(long nWndId,const int nCtrlId, const int nBtStat, const bool nShow);

	void UpdateMusicPlayProgress();

	bool UpdateBTCallInfo(T_APP_MSG* msg);

	bool UpdateBTCallTimeInfo(T_APP_MSG* msg);
	bool UpdateBTCallMuteInfo(T_APP_MSG* msg);
	bool UpdateBTCallHandfreeInfo(T_APP_MSG* msg);
	bool UpdateBTCallContactInfo(T_APP_MSG* msg);
	bool UpdateBTCallHistoryInfo(T_APP_MSG* msg);
	bool UpdateBTBatteryInfo(T_APP_MSG* msg);
	bool UpdateBTDeviceName(T_APP_MSG* msg);
	bool UpdateBTCallHistoryClassInfo(int history_class);

	void SetBtKeyboard(const bool Status);

private:
	IVIController * m_ViewController;
	map<int,pUpdateBTMsg> m_MsgMap;

	MutexLock m_Lock;
	int m_nCurrWnd;
	int m_nCurPlayStatus;
	int m_nCurPlayType;
	int m_nPlayAuido_Total;
	int m_nPlayAudio_pos;
	int m_nMemPairedWnd;
	char m_InputKeyboard[TEXT_BUF_LEN];

	int m_nBTAppType = 0;//1、音乐 2、电话
	//bt phone
	int m_nCallMuteMode = 0;
	int m_nCallHandfreeMode = 0;
	int m_nCallStatus = 0;
	int m_nTabStatus = 0;	//0:dail button, 1: contacts, 2: call history
};
 
 
 #endif /* BTVIEW_H_ */