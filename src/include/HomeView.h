/*
 * HomeView.h
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#ifndef HOMEVIEW_H_
#define HOMEVIEW_H_

//View的实体，实现显示功能。
#include "HomeData.h"

#include "CompBase.h"
#include "IViewMessage.h"
#include "AppDatadef.h"
#include "FileBase.h"
#include "IVIController.h"

using std::map;
using FY_NAMESPACE::T_APP_MSG;
using FY_NAMESPACE::TimerBase;

class HomeView;
typedef bool (HomeView::*pUpdateMsg)(T_APP_MSG* msg);

class HomeView : public CompBase, public TimerBase{
public:
	HomeView(ViewBase* p_ViewBase);
	virtual ~HomeView();

	virtual void HomeOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);
	
	/*
	 * call back UserMessge*/
	virtual bool HomeOnUserMessage(T_APP_MSG* msg);

	/*
	 * call back GUI operation status*/
	virtual void HomeOnComponent(CUIWndMng* pUI, PWINDOW_INFO pInfo);

	/*
	 * call back timer*/
	virtual void OnTimer(uint32_t id);
	
	void HomeInitWorkWnd();

	void HomeAttachController(IVIController* pController)
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

	bool UpdateHomeContenAnimaInfo(T_APP_MSG* msg=nullptr);

	void KillTimer_AvmShow();
	void KillTimer_LinkShowConfirm();
	void OnStartAvm_View(int nView);
	void OnStart_AVM();

	void Set_AvmViewID(int nViewID);
	void Update_Avm_Icon();
private:

	/*
	 * Initialize update message map*/
	void HomeInitUpdateMsgMap();
	void HomeGetUpdateMessageMap(map<int,pUpdateMsg> &nMap);

	void Lock(){m_Lock.Lock();}
	void UnLock(){m_Lock.Unlock();}

	int GetCurrWnd();

	void HomeUpdateHomeContenInfo(const int nCtrlId, const int nBtStat, const bool nShow);

	bool Update_CP_Show(T_APP_MSG* msg);
	bool Update_CP_Connet(T_APP_MSG* msg=nullptr);
	bool Update_AA_Connet(T_APP_MSG* msg=nullptr);
	bool Update_USB_Insert(T_APP_MSG* msg=NULL);

	void RefreshCPAAIcons();          // CP/AA 图标各自独立重算（连接状态+有效性）
	void StartLinkShowConfirm(int nLink);   // 点击互联后等待前台确认，超时收回（防黑屏）
	bool Update_CPAA_RefreshIcon(T_APP_MSG* msg);   // 互联资源加载/释放时只重算图标
	
private:
	IVIController * m_ViewController;
	map<int,pUpdateMsg> m_MsgMap;

	MutexLock m_Lock;
	int m_nCurrWnd;

	int m_nUsb_Insert;
	bool m_bAvmStart;

	int m_nCurAVM_View;
	int m_nWaitShowLink;      // 0=无等待 1=等CP前台 2=等AA前台
};

#endif /* HOMEVIEW_H_ */
