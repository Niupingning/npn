/*
 * SettingView.h
 *
 *  Created on: 2021-08-03
 *      Author: Y
 */

#ifndef RADIOVIEW_H_
#define RADIOVIEW_H_

//View的实体，实现显示功能。
#include "IVIController.h"
#include "HomeData.h"
#include "RadioData.h"
//#include "HandleViewUpdate.h"

//Dependent on external header files
#include "CompBase.h"
#include "IViewMessage.h"
#include "AppDatadef.h"
#include "FileBase.h"

using std::map;
using FY_NAMESPACE::T_APP_MSG;
using FY_NAMESPACE::TimerBase;

class RadioView;
typedef bool (RadioView::*pUpdateRadioMsg)(T_APP_MSG* msg);

class  RadioView : public CompBase, public TimerBase
{
public:
	RadioView(ViewBase* p_ViewBase);
	virtual ~RadioView();

	/*
	 * call back UserMessge*/
	virtual bool RadioOnUserMessage(T_APP_MSG* msg);


	/*
	 * call back show window,Initialize user information*/
	virtual void RadioOnShow(LONG dwWndId);

	/*
	 * call back GUI operation status*/
	virtual void RadioOnComponent(CUIWndMng* pUI, PWINDOW_INFO pInfo);

	/*
	 *	界面触摸处理函数
	 *  pUI[IN] 窗口资源指针
	 *  nWndID[IN] 窗口ID 
	 *  nCtrlID[IN] 操作发生的控件ID
	 *  nBtState[IN] 控件的操作状态，包括按下、抬起、弹起时响应操作
	 */
	virtual void RadioOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);

	void RadioAttachController(IVIController* pController)
	{
		if(pController == NULL)
		{
			LOGDBG("pController == NULL");
		}
		else
		{
			LOGDBG("pController != NULL");
			m_pRadioViewController = pController;
		}

	}

	void KillRadioTimer();

	void SendMeterInfoDataOFBreakAuto();
	void SendMeterInfoDataOFSeekUP();
	void SendMeterInfoDataOFSeekDown();
	void SendMeterInfoDataOFMute();

	void RadioInitWorkWnd();

protected:

	/*
	 * call back timer*/
	virtual void OnTimer(uint32_t id);

private:
		/*
	 * Initialize update message map*/
	void InitUpdateMsgMap();
	void GetUpdateMessageMap(map<int,pUpdateRadioMsg> &nMap);
	void DrawAllItem();
	int GetCurrWnd();
	void Lock(){m_Lock.Lock();}
	void UnLock(){m_Lock.Unlock();}	

	bool MCUUpdateRadioInfo(T_APP_MSG* msg=nullptr);
	bool MCUUpdateRadioCurrFreq(const int nCurrBand,const int nCurrFreq,const int nRadioStatus);
	
	bool MCUUpdateRadioCurrMute(const int nCurrMute);
	bool MCUUpdateRadioStereoMono(const int nStereoMono);

	void OnClickUpdateRadioGroup(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);

	void SendMeterInfoDataOFBaneFreq();
	void SendMeterInfoDataOFAutoSeek();

	void OnClickUpdateCollectionvec();

	void OnClickUpdateRadioRDSGroup(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);
	void SendMeterInfoDataOFArea(int nCtrlId);


	void ConverDecode(char *pInBuf,char *pOutBuf,size_t& nOutLen);

	void SendMeterInfoDataOF_LOC();

	void OnClickTiView(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);

	void OnHandle_TiFreq(int nTiFreq);

private:

	IVIController* m_pRadioViewController;

	map<int, pUpdateRadioMsg> m_MsgMap;
	int m_nCurrWnd;
	MutexLock m_Lock;

	int m_SationListSatus;

	int m_nCurTiFreq;

	int m_nCurBand;
	int m_nCurFreq;
	bool m_bShowTiView;
};

#endif /* RADIOVIEW_H_ */
