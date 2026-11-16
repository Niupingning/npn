/*
 * EqView.h
 *
 *  Created on: 2025-10-21
 *      Author: junjie.feng
 */

#ifndef EQVIEW_H_
#define EQVIEW_H_

//View的实体，实现显示功能。
#include "IVIController.h"
#include "EqData.h"
//#include "HandleViewUpdate.h"

//Dependent on external header files
#include "CompBase.h"
#include "IViewMessage.h"
#include "AppDatadef.h"
#include "UIID_Def_AudioBoost.h"
#include "fyaudiocontrol.h"
#include "ConfigHelper.h"
#include "FileBase.h"
#include<deque>

using std::map;
using FY_NAMESPACE::T_APP_MSG;
using FY_NAMESPACE::TimerBase;

class EqView;
typedef bool (EqView::*pUpdateEqMsg)(T_APP_MSG* msg);

static struct AudioEqInfoStruct adEqInfo[EqModeMax]={
    {EqSuperBass, 13, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {8, 8, 6, 6, 2, 0, 0, 0, 0, 2, 2, 4, 2}},
    {EqPowerful, 13,  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {2, 6, 4, 2, 0, -2, 0, 2, 2, 2, 4, 4, 6}},
    {EqNatural, 13,   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {-6, 2, 2, 2, 0, 0, 2, 2, -2, -2, 2, 2, 4}},
    {EqVocal, 13,     {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {-6, -2, 0, 4, 6, 6, 2, 0, 2, 4, 4, 2, 0}},
    {EqFlat, 13,      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {EqCustom1, 13,   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {EqCustom2, 13,   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}       
};

static int freq[11] = {25, 315, 40, 50, 63, 80, 100, 125, 160, 200, 250};
static int slope[6] = {-6, -12, -18, -24, -30, -36};

class  EqView : public CompBase, public TimerBase
{
public:
	EqView(ViewBase* p_ViewBase);
	virtual ~EqView();

	/*
	 * call back UserMessge*/
	virtual bool EqOnUserMessage(T_APP_MSG* msg);


	/*
	 * call back show window,Initialize user information*/
	virtual void EqOnShow(LONG dwWndId);

	/*
	 * call back GUI operation status*/
	virtual void EqOnComponent(CUIWndMng* pUI, PWINDOW_INFO pInfo);


	virtual void EqOnListInfoMessage(CUIWndMng *pUI, int nWndId, int nCtrlId, PLIST_INFO pInfo);

	/*
	 *	界面触摸处理函数
	 *  pUI[IN] 窗口资源指针
	 *  nWndID[IN] 窗口ID 
	 *  nCtrlID[IN] 操作发生的控件ID
	 *  nBtState[IN] 控件的操作状态，包括按下、抬起、弹起时响应操作
	 */
	virtual void EqOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);

	void EqAttachController(IVIController* pController)
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

	static EqView *getInstance();
	static ConfigHelper *FyAudioConfig;
protected:

	/*
	 * call back timer*/
	virtual void OnTimer(uint32_t id);
public:
	    void EqInitWorkWnd();
		void onClick_SettingList();
		void onClick_ReturnBtn();
		void onClick_LoudnesBtn(long nCtrlId);
		void onClick_Listening(long nCtrlId);
		void onClick_Speaker(long nCtrlId);
		void onClick_Time_Alignment(long nCtrlId);
		void onClick_Time_Crossover(long nCtrlId);

		void InitAudioConfig();

private:
	void OnInit_EqView();
	void DrawAllItem();

	void SetSetCurrWnd(int nWndID);
	int GetSetCurrWnd();
	void Lock(){m_Lock.Lock();}
	void UnLock(){m_Lock.Unlock();}	


private:

	IVIController* m_ViewController;
	FyAudioControl* m_AudioControl;
	ConfigHelper*	m_ConfigHelper;
	//HandleViewUpdate  m_HandleViewUpdate;

	map<int, pUpdateEqMsg> m_MsgMap;
	MutexLock m_Lock;
	int m_nInWnd;      
	int m_nSetCurrWnd;
	EqData * m_nEqData;
	int m_nspeaker;


};

#endif /* EQVIEW_H_ */
