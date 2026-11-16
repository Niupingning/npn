/*
 * AirConditionar.h
 *
 *  Created on: 2025-10-21
 *      Author: junjie.feng
 */

#ifndef AIRCONDITIONAR_H_
#define AIRCONDITIONAR_H_

//View的实体，实现显示功能。
#include "IVIController.h"
#include "AirConditionData.h"
//#include "HandleViewUpdate.h"

//Dependent on external header files
#include "CompBase.h"
#include "IViewMessage.h"
#include "AppDatadef.h"
#include "FileBase.h"
#include "CanBoxProto_Def.h"
#include<deque>

using std::map;
using FY_NAMESPACE::T_APP_MSG;
using FY_NAMESPACE::TimerBase;

#define ARRY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

class AirConditionar;
typedef bool (AirConditionar::*pUpdateAirMsg)(T_APP_MSG* msg);

class  AirConditionar : public CompBase, public TimerBase
{
public:
	AirConditionar(ViewBase* p_ViewBase);
	virtual ~AirConditionar();

	/*
	 * call back UserMessge*/
	virtual bool AirOnUserMessage(T_APP_MSG* msg);


	/*
	 * call back show window,Initialize user information*/
	virtual void AirOnShow(LONG dwWndId);

	/*
	 * call back GUI operation status*/
	virtual void AirOnComponent(CUIWndMng* pUI, PWINDOW_INFO pInfo);


	virtual void AirOnListInfoMessage(CUIWndMng *pUI, int nWndId, int nCtrlId, PLIST_INFO pInfo);

	/*
	 *	界面触摸处理函数
	 *  pUI[IN] 窗口资源指针
	 *  nWndID[IN] 窗口ID 
	 *  nCtrlID[IN] 操作发生的控件ID
	 *  nBtState[IN] 控件的操作状态，包括按下、抬起、弹起时响应操作
	 */
	virtual void AirOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);

	void AirAttachController(IVIController* pController)
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

protected:

	/*
	 * call back timer*/
	virtual void OnTimer(uint32_t id);
public:
	    void AirInitWorkWnd();

private:
	void OnHide_AudioBtn();
	void DrawAllItem();

	void SetSetCurrWnd(int nWndID);
	int GetSetCurrWnd();
	void Lock(){m_Lock.Lock();}
	void UnLock(){m_Lock.Unlock();}	

	void OnHandle_AirShow();
	void OnHandle_CarShow();
	void OnHandle_CarSetShow();
private:

	IVIController* m_ViewController;
	//HandleViewUpdate  m_HandleViewUpdate;

	map<int, pUpdateAirMsg> m_MsgMap;
	MutexLock m_Lock;
	int m_nInWnd;
	int m_nSetCurrWnd;
	AirConditionData * m_nAirConditionData;
	T_CANBOX_AIR_DATA m_tCanBoxData;
	T_CANBOX_CAR_DATA m_tCanBoxCarData;
	T_CANBOX_CAR_SETTING m_tCanBoxCarSetData;

	// 库方式
	AirConditionInfo m_sCanBoxProData;
	
};

#endif /* AIRCONDITIONAR_H_ */
