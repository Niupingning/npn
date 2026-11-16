/*
 * SettingView.h
 *
 *  Created on: 2021-08-03
 *      Author: Y
 */

#ifndef DTSVIEW_H_
#define DTSVIEW_H_

//View的实体，实现显示功能。
#include "IVIController.h"
#include "HomeData.h"
//#include "HandleViewUpdate.h"

//Dependent on external header files
#include "CompBase.h"
#include "IViewMessage.h"
#include "AppDatadef.h"
#include "FileBase.h"

using std::map;
using FY_NAMESPACE::T_APP_MSG;
using FY_NAMESPACE::TimerBase;

class DtsView;
typedef bool (DtsView::*pUpdateDtsMsg)(T_APP_MSG* msg);

class  DtsView : public CompBase, public TimerBase
{
public:
	DtsView(ViewBase* p_ViewBase);
	virtual ~DtsView();

	/*
	 * call back UserMessge*/
	virtual bool DtsOnUserMessage(T_APP_MSG* msg);


	/*
	 * call back show window,Initialize user information*/
	virtual void DtsOnShow(LONG dwWndId);

	/*
	 * call back GUI operation status*/
	virtual void DtsOnComponent(CUIWndMng* pUI, PWINDOW_INFO pInfo);

	/*
	 *	界面触摸处理函数
	 *  pUI[IN] 窗口资源指针
	 *  nWndID[IN] 窗口ID 
	 *  nCtrlID[IN] 操作发生的控件ID
	 *  nBtState[IN] 控件的操作状态，包括按下、抬起、弹起时响应操作
	 */
	virtual void DtsOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);

	void DtsAttachController(IVIController* pController)
	{
		if(pController == NULL)
		{
			LOGDBG("pController == NULL");
		}
		else
		{
			LOGDBG("pController != NULL");
			m_pViewController = pController;
		}

	}

	void InitUpdateMsgMap();

	void DtsInitWorkWnd();
	bool UpdateDtsInfo(T_APP_MSG* msg);

	void OnClick_Surroud_Level(int nOption,int nLevel);
	void OnClick_Deep_Level(int nOption,int nLevel);
	void OnClick_Evelates_Level(int nOption,int nLevel);
	void OnClick_EQMode(int nMode);

protected:

	/*
	 * call back timer*/
	virtual void OnTimer(uint32_t id);

private:
	void Lock(){m_Lock.Lock();}
	void UnLock(){m_Lock.Unlock();}	
	void GetUpdateMessageMap(map<int, pUpdateDtsMsg> &nMap);

private:

	IVIController* m_pViewController;

	map<int, pUpdateDtsMsg> m_MsgMap;

	MutexLock m_Lock;
};

#endif /* DTSVIEW_H_ */
