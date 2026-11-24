/*
 * BTView.cpp
 *
 *  Created on: 2025-09-25
 *      Author: Y6915
 */

#include "BTView.h"
#include "UIID_Def_BT.h"
#include "AssertDefine.h"

// Dependent on external header files
#include <math.h>
#include <algorithm>
#include <math.h>
#include <iconv.h>
#include <errno.h>
#include <wchar.h>
#include "IVIView.h"

using FY_NAMESPACE::APPMSG_DEVICESTATUS;
using FY_NAMESPACE::APPS_STATUS_HIDEONLY;
using FY_NAMESPACE::APPS_STATUS_PAUSE;
using FY_NAMESPACE::APPS_STATUS_QUIT;
using FY_NAMESPACE::APPS_STATUS_RESUME;
using FY_NAMESPACE::APPSMSG_APPDATA;
using FY_NAMESPACE::APPSMSG_APPSTATUS;
using FY_NAMESPACE::APPSMSG_DATETIME;
using FY_NAMESPACE::APPSMSG_TIMER;
using FY_NAMESPACE::CLOSE_FORYOUAPP;
using FY_NAMESPACE::CUIWndMgrWrapper;
using FY_NAMESPACE::LAUNCH_FORYOUAPP;
using std::make_pair;

#define IMPERIAL_VALUE 0.621371192

BTView::BTView(ViewBase* p_ViewBase)
: CompBase(p_ViewBase)
, m_nCurrWnd(0)
, m_ViewController(NULL)
{
	m_nCurrWnd = 0;
	m_nCurPlayType = 0;
	m_nPlayAuido_Total = 0;
	m_nPlayAudio_pos = 0;
	m_nCurPlayStatus = 0;
	m_nMemPairedWnd = 0;
	memset(m_InputKeyboard,0,sizeof(m_InputKeyboard));
	// TODO Auto-generated constructor stub
	BTInitUpdateMsgMap();
}

BTView::~BTView()
{
	// TODO Auto-generated destructor stub
}

void BTView::BTInitUpdateMsgMap()
{
	m_MsgMap.clear();	
	/*
 	*顶栏*/
	//m_MsgMap.insert(make_pair(METER_VMSG_BT_MUSIC_INFO, &BTView::UpdateBTMusicInfo));

	/*
	*中间内容*/
	//m_MsgMap.insert(make_pair(METER_VMSG_ACCSTATUS, &BTView::Update_CP_Show));
	//bool UpdateBTMusicInfo(T_APP_MSG *msg = NULL);
	m_MsgMap.insert(make_pair(METER_VMSG_BT_MUSIC_INFO, &BTView::UpdateBTMusicInfo));
	m_MsgMap.insert(make_pair(METER_VMSG_BT_MUSIC_TIME, &BTView::UpdateBTMusicTime));
	m_MsgMap.insert(make_pair(METER_VMSG_BT_MUSICPLAY_STATE, &BTView::UpdateBTMusicPlay));
	m_MsgMap.insert(make_pair(METER_VMSG_BT_CONNECTED_STATE, &BTView::UpdateBTConnectSts));
	m_MsgMap.insert(make_pair(SYS_VMSG_UPDATE_BT_LIST, &BTView::UpdateBTConnectDevice));
	m_MsgMap.insert(make_pair(METER_VMSG_BT_LOCAL_INFO, &BTView::UpdateBTLocalInfo));
	m_MsgMap.insert(make_pair(METER_VMSG_BT_DISCOVER_INFO, &BTView::UpdateBTDiscoverList));
	m_MsgMap.insert(make_pair(WIFI_VMSG_BTPAIRINGREQ, &BTView::UpdateBTPairReq));

	//bt phone
	m_MsgMap.insert(make_pair(METER_VMSG_BT_PHONE_INFO, &BTView::UpdateBTCallInfo));	//更新蓝牙通话信息
	m_MsgMap.insert(make_pair(METER_VMSG_BT_PHONE_TIME_INFO, &BTView::UpdateBTCallTimeInfo));	//更新蓝牙通话信息
	m_MsgMap.insert(make_pair(WIFI_VMSG_BT_PHONE_CALLMUTE, &BTView::UpdateBTCallMuteInfo));	//更新蓝牙通话静音状态
	m_MsgMap.insert(make_pair(WIFI_VMSG_BT_PHONE_CALLMODE, &BTView::UpdateBTCallHandfreeInfo));	//更新蓝牙通话免提方式
	m_MsgMap.insert(make_pair(METER_VMSG_BT_PHONE_BOOK_INFO, &BTView::UpdateBTCallContactInfo));	//更新蓝牙通讯录
	m_MsgMap.insert(make_pair(METER_VMSG_BT_PHONE_HISTORY_INFO, &BTView::UpdateBTCallHistoryInfo));	//更新蓝牙通讯记录
	m_MsgMap.insert(make_pair(METER_VMSG_PH_BATTERY, &BTView::UpdateBTBatteryInfo));	//更新手机电量
	m_MsgMap.insert(make_pair(SYS_VMSG_UPDATE_BT_NAME, &BTView::UpdateBTDeviceName));	//更新IVI蓝牙名称
}

void BTView::BTGetUpdateMessageMap(map<int, pUpdateBTMsg> &nMap)
{
	nMap = m_MsgMap;
}

bool BTView::BTOnUserMessage(T_APP_MSG *msg)
{
	if (nullptr == msg)
	{
		return false;
	}
	//LOGDBG("BTOnUserMessage pthread_ID = %d",(int)pthread_self());

	LOGDBG("Message = %d,param0=%d,param1=%d,param2=%d\n",
		   msg->Message, msg->Param0, msg->Param1, msg->Param2);
	map<int, pUpdateBTMsg> nUpdateMap;
	nUpdateMap.clear();
	BTGetUpdateMessageMap(nUpdateMap);
	auto Itr = nUpdateMap.find(msg->Message);
	if (Itr != nUpdateMap.end())
	{
		(this->*(Itr->second))(msg);
	}

	return true;
}


void BTView::BTOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	BTData &mData = BTData::GetInstance();
	LOGDBG("nWndId=%d,nCtrlId=%d,nBtState=%d",nWndId,nCtrlId,nBtState);

    // 关闭按钮与蓝牙连接状态无关，任何状态都必须能退出去
    if (UIID_BT_APPTOPBAR_CLOSE == nCtrlId)
    {
        GetViewBase()->ShowWnd(UIID_WNDID_BT_PHONE, false);
        GetViewBase()->ShowWnd(UIID_WNDID_BT_MUSIC, false);
		GetViewBase()->ShowWnd(UIID_WNDID_BT_PAIRLIST, false);
        GetViewBase()->ShowWnd(UIID_WNDID_BT_PAIRREQ,  false);
        GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
        return;
    }

	if(1 == nBtState)
	{
		BTUpdateHomeContenInfo(nWndId,nCtrlId,nBtState,1);
	}
}


void BTView::BTOnComponent(CUIWndMng *pUI, PWINDOW_INFO pInfo)
{
	// LOGDBG("pInfo.nInfoType=%d,nWindowID=%d,nCtrlID=%d ,status=%d\n",\
	// 		pInfo->nInfoType, pInfo->nWindowID, pInfo->nCtrlID, pInfo->nOperationStatus);
}

void BTView::OnTimer(uint32_t id)
{
	//	LOGDBG("BTView pthread_ID = %d",(int)pthread_self());
	//	LOGDBG("BTView::OnTimer id = %d", id);
	static bool bFlag = true;
	if (bFlag)
	{
		bFlag = false;
		int ret = pthread_setname_np(pthread_self(), "ViewTimer");
		if (ret != EOK)
		{
			bFlag = true;
			LOGDBG("ViewMain Timer thread name fail!");
		}
	}
}

void BTView::OnBtMusic_WinShow()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	//初始化蓝牙开关状态、蓝牙连接状态
	BTData &mData = BTData::GetInstance();

	int nBTOpenStatus = 0;
	int nBTConnectStatus = 0;
	BtPairedDeviceToAppInfo_t m_nBTPairedList;
	memset(&m_nBTPairedList,0,sizeof(m_nBTPairedList));

	//获取蓝牙开关状态、蓝牙连接状态
	m_nBTAppType = 1;//记录进入的是音乐
	nBTOpenStatus = mData.GetBtOpenStatus();
	nBTConnectStatus = mData.GetBtConnectStatus();
	mData.GetBTPairedListInfo(m_nBTPairedList);

	LOGDBG("-----XSJXSJXSJ-----nBTOpenStatus = %d---nBTConnectStatus = %d-----", nBTOpenStatus, nBTConnectStatus);

	if(0 == nBTOpenStatus)//蓝牙开
	{
		if(1 == nBTConnectStatus||0 == nBTConnectStatus)//蓝牙未连接
		{
			m_nMemPairedWnd = 3;
			GetViewBase()->ShowWnd(UIID_WNDID_BT_PAIRLIST, true);
			if(m_nBTPairedList.num > 0)//有配对历史
			{
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NOPAIRNOTICE,false);
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST,true);//旧配对列表
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NEWLIST,false);
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST_UPDATE,false);
			}
			else//无配对历史
			{
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NOPAIRNOTICE,true);//无配对列表提示
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST,false);
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NEWLIST,false);
			}
		}
		else if(2 == nBTConnectStatus)//蓝牙连接
		{
			GetViewBase()->ShowWnd(UIID_WNDID_BT_MUSIC, true);//蓝牙音乐
		}
	}
	else//蓝牙关闭：也要有页面，不能点了没反应
	{
		m_nMemPairedWnd = 3;
		GetViewBase()->ShowWnd(UIID_WNDID_BT_PAIRLIST, true);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NOPAIRNOTICE,true);//无配对/请打开蓝牙提示
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST,false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NEWLIST,false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST_UPDATE,false);
	}
}

void BTView::OnBtCall_WinShow()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	//初始化蓝牙开关状态、蓝牙连接状态
	BTData &mData = BTData::GetInstance();

	int nBTOpenStatus = 0;
	int nBTConnectStatus = 0;
	BtPairedDeviceToAppInfo_t m_nBTPairedList;
	memset(&m_nBTPairedList,0,sizeof(m_nBTPairedList));

	//获取蓝牙开关状态、蓝牙连接状态
	m_nBTAppType = 2;//记录进入的是电话
	nBTOpenStatus = mData.GetBtOpenStatus();
	nBTConnectStatus = mData.GetBtConnectStatus();
	mData.GetBTPairedListInfo(m_nBTPairedList);

	LOGDBG("-----XSJXSJXSJ-----nBTOpenStatus = %d---nBTConnectStatus = %d-----", nBTOpenStatus, nBTConnectStatus);

	if(0 == nBTOpenStatus)//蓝牙开
	{
		if(1 == nBTConnectStatus||0 == nBTConnectStatus)
		{
			m_nMemPairedWnd = 4;
			GetViewBase()->ShowWnd(UIID_WNDID_BT_PAIRLIST, true);
			if(m_nBTPairedList.num > 0)
			{
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NOPAIRNOTICE,false);
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST,true);//旧配对列表
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NEWLIST,false);
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST_UPDATE,false);
			}
			else
			{
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NOPAIRNOTICE,true);//无配对列表提示
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST,false);
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NEWLIST,false);
			}
		}
		else if(2 == nBTConnectStatus)
		{
			GetViewBase()->ShowWnd(UIID_WNDID_BT_PHONE, true);//BT
		}
	}
	else//蓝牙关闭：进配对页，避免点了没反应
	{
		m_nMemPairedWnd = 4;
		GetViewBase()->ShowWnd(UIID_WNDID_BT_PAIRLIST, true);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NOPAIRNOTICE,true);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST,false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NEWLIST,false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST_UPDATE,false);
	}
}

int BTView::GetCurrWnd()
{
	BTData &mData = BTData::GetInstance();
	Lock();
	m_nCurrWnd = mData.GetCurrWnd();
	UnLock();
	return m_nCurrWnd;
}

void BTView::BTInitMusicWorkWnd()
{
	LOGDBG("BTInitMusicWorkWnd");
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MAIN,UIID_LEFTCOLUMN_APPICON_BIN,UUID_PICID_LEFTCOLUMN_APPICON_PIC,ALL_STATE);

	int nCurSource = ((IVIView*)GetViewBase())->Get_CurSource();
	if(IVI_SOURCE_BT_MUSIC != nCurSource)
	{
			//上个源暂停
		((IVIView*)GetViewBase())->OnHandle_Source(nCurSource);

		//保存为当前源
		((IVIView*)GetViewBase())->Save_CurSource(IVI_SOURCE_BT_MUSIC);
		
		//播放
		int nBTConnectStatus = 0;
		BTData &mData = BTData::GetInstance();
		nBTConnectStatus = mData.GetBtConnectStatus();

		if(2 == nBTConnectStatus)
		{
			OnNeed_Play();
		}
	}
}

void BTView::BTUpdateHomeContenInfo(long nWndId,const int nCtrlId, const int nBtStat, const bool nShow)
{
	
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	
    LOGDBG("&&&---wnd:%d,ctrlid:%d",nWndId,nCtrlId);
	BTData &mData = BTData::GetInstance();
	mData.SetCurrWnd(nWndId);
	switch(nWndId)
	{
		case UIID_WNDID_BT_MUSIC:
		{
			//GetViewBase()->ShowWnd(UIID_WNDID_BT_MUSIC, true);
			OnClick_BTMusicPlayWnd(nCtrlId,nBtStat,nShow);
		}
		break;
		case UIID_WNDID_BT_PAIRLIST:
		{
			OnClick_BTPairedList(nCtrlId,nBtStat,nShow);
		}
		break;
		case UIID_WNDID_BT_PAIRREQ:
		{
			OnClick_BTPairedReq(nWndId,nCtrlId,nBtStat,nShow);
		}
		break;
		default:
		break;
	}
}

bool BTView::UpdateBTMusicInfo(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return false;
	}
	
	BTData &mData = BTData::GetInstance();
	BtMusicInfo_t tBTMusic;
	memset(&tBTMusic,0,sizeof(tBTMusic));
	mData.GetBTMusicInfo(tBTMusic);
	LOGDBG("&&&&----xsj----tBTMusic.abTitle---len:0\n", tBTMusic.abTitle);
	LOGDBG("&&&&----xsj----tBTMusic.abArtist---len:%s0\n", tBTMusic.abArtist);
	LOGDBG("&&&&----xsj----tBTMusic.abAlbum---len:%s0\n", tBTMusic.abAlbum);

	char szTempMsc[2*1024] = {0};
	char szTempArt[2*1024] = {0};
	char szTempAlb[2*1024] = {0};
	size_t nLen = 1024;
	ConverDecode(tBTMusic.abTitle,szTempMsc,nLen);
	ConverDecode((char*)tBTMusic.abArtist,szTempArt,nLen);
	ConverDecode(tBTMusic.abAlbum,szTempAlb,nLen);

	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_MUSIC, UIID_BT_PAGECONTENT_MUSICNAME, (wchar_t*)szTempMsc);//歌名
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_MUSIC, UIID_BT_PAGECONTENT_SINGERNAME, (wchar_t*)szTempArt);//歌手名
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_MUSIC, UIID_BT_PAGECONTENT_ALBUMNAME, (wchar_t*)szTempAlb);//专辑

	return true;
}

bool BTView::UpdateBTMusicPlay(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}
	BTData &mData = BTData::GetInstance();

	int nBTMusicPlayStatus = 0;
	nBTMusicPlayStatus = mData.GetBTMusicStatus();
	LOGDBG("&&&&----xsj----nBTMusicPlayStatus---len:%d\n", nBTMusicPlayStatus);

	if(1 == nBTMusicPlayStatus)
	{
		//播放
		//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_MUSIC,UIID_BT_PAGECONTENT_PLAYPAUSE,214,ITEM_STATE_NORMAL);

		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_MUSIC,UIID_BT_PAGECONTENT_PLAYPAUSE,211,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_MUSIC,UIID_BT_PAGECONTENT_PLAYPAUSE,212,ITEM_STATE_PUSH);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_MUSIC,UIID_BT_PAGECONTENT_PLAYPAUSE,211,ITEM_STATE_SELECTED);
	}
	else if(0 == nBTMusicPlayStatus)
	{
		//暂停
		//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_MUSIC,UIID_BT_PAGECONTENT_PLAYPAUSE,211,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_MUSIC,UIID_BT_PAGECONTENT_PLAYPAUSE,214,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_MUSIC,UIID_BT_PAGECONTENT_PLAYPAUSE,215,ITEM_STATE_PUSH);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_MUSIC,UIID_BT_PAGECONTENT_PLAYPAUSE,214,ITEM_STATE_SELECTED);
	}

	return true;
}

bool BTView::UpdateBTMusicTime(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return false;
	}
	//BTData &mData = BTData::GetInstance();
	//BtPlaybackTime_t tBTMusicTime;
	//mData.GetBTMusicTime(tBTMusicTime);
	int m_nPlayAudio_pos = (msg->Param0) / 1000;
	int m_nPlayAuido_Total = (msg->Param1) / 1000;
	//LOGDBG("&&&&----xsj----tBTMusicTime.dCurrentTime---%d\n", m_nPlayAudio_pos);
	//LOGDBG("&&&&----xsj----tBTMusicTime.dTotalTime---%d\n", m_nPlayAuido_Total);

	//当前播放时间
	int nMin = m_nPlayAudio_pos / 60;
	int sec = m_nPlayAudio_pos % 60;
	wchar_t szTip[TEXT_BUF_LEN] = {0};
	swprintf(szTip,TEXT_BUF_LEN,L"%02d:%02d",nMin,sec);

	//总时间
	int nTMin = m_nPlayAuido_Total / 60;
	int Tsec = m_nPlayAuido_Total % 60;
	wchar_t szTip2[TEXT_BUF_LEN] = {0};
	swprintf(szTip2,TEXT_BUF_LEN,L"%02d:%02d",nTMin,Tsec);

	int nPos = 0;
	if(m_nPlayAuido_Total > 0)
	{
		nPos = (m_nPlayAudio_pos * 100) / m_nPlayAuido_Total;
	}

	//LOGDBG("&&&--nMin:%d,sec:%d,nPos:%d",nMin,sec,nPos);
	//LOGDBG("&&&---szTip:%s,szTip2:%s",szTip,szTip2);
        
	//pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_MUSIC, UIID_BT_PAGECONTENT_CURRENTTM,true);
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_MUSIC, UIID_BT_PAGECONTENT_CURRENTTM, (wchar_t*)szTip);
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_MUSIC, UIID_BT_PAGECONTENT_TOTALTM, (wchar_t*)szTip2);

	//进度
	pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_BT_MUSIC,UIID_BT_PAGECONTENT_PLAYPROGRESS,nPos);
	return true;
}

bool BTView::UpdateBTLocalInfo(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return false;
	}
	
	BTData &mData = BTData::GetInstance();
	BtLocalInfo_t tBTInfo;
	memset(&tBTInfo,0,sizeof(tBTInfo));
	mData.GetBtLocalInfo(tBTInfo);
	//LOGDBG("-----XSJBTLocalInfo-----tBTInfo->BTName = %s", tBTInfo.BTName);

	char szTempBTName[2*1024] = {0};
	size_t nLen = 1024;
	ConverDecode(tBTInfo.BTName,szTempBTName,nLen);
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PAIRLIST, UIID_WNDID_BT_PAIRLIST_LOCALNAME, (wchar_t*)szTempBTName);//设备名
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PAIRLIST, UIID_WNDID_BT_DISCOVERLIST_LOCALNAME, (wchar_t*)szTempBTName);//设备名

	return true;
}

bool BTView::UpdateBTDiscoverList(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return false;
	}
	BTData &mData = BTData::GetInstance();
	BtBondedDeviceToAppInfo_t tBTDiscoverList;
	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));

	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_BT_PAIRLIST,1100,UItem);
	CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;
	if(NULL == pList)
	{
		return false;
	}

	pList->DeleteAllItems();

	//获取发现列表，显示
	mData.GetBtBtDiscoverList(tBTDiscoverList);

	int nCount = tBTDiscoverList.num;



	char szTemp[2*1024] = {0};
	size_t nLen = 1024;

	for(int i = 0; i < nCount; i++)
	{
		memset(szTemp,0,sizeof(szTemp));
		ConverDecode(tBTDiscoverList.BTDeviceName[i],szTemp,nLen);

		pList->InsertItem(i);
		pList->SetItemString(i,(wchar_t*)szTemp);
		pList->SetItemString(i,319,1);
	}

	return true;
}

bool BTView::UpdateBTConnectSts(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return false;
	}

	BTData &mData = BTData::GetInstance();
	int nBTConnectStatus = 0;
	nBTConnectStatus = mData.GetBtConnectStatus();

	LOGDBG("-----BTConnectStatus=%d, m_nCallStatus=%d-----", nBTConnectStatus, m_nCallStatus);

	if(2 == nBTConnectStatus)
	{
		HomeData &mHData = HomeData::GetInstance();
		int nCurrWnd = mHData.GetCurrWnd();
		if((UIID_WNDID_BT_PHONE == nCurrWnd) || (UIID_WNDID_BT_MUSIC == nCurrWnd)
			|| (UIID_WNDID_BT_PAIRREQ == nCurrWnd) || (UIID_WNDID_BT_PAIRLIST == nCurrWnd))
		{
			if(m_nBTAppType == 1)
			{
				GetViewBase()->ShowWnd(UIID_WNDID_BT_MUSIC, true);
			}
			else if(m_nBTAppType == 2)
			{
				GetViewBase()->ShowWnd(UIID_WNDID_BT_PHONE, true);
			}
			else
			{
				GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
			}
		}
	}
	else
	{
		// R8：蓝牙断开 → 通话/响铃/拨号界面整体复位，然后回主界面
		LOGDBG("BT disconnected, reset call UI, prev m_nCallStatus=%d", m_nCallStatus);
		ResetCallUI();
		GetViewBase()->ShowWnd(UIID_WNDID_BT_PHONE, false);
		GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
	}

	return true;
}

bool BTView::UpdateBTPairReq(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return false;
	}
	BTPairingRegInfo_t tBTBairReqInfo;
	memset(&tBTBairReqInfo,0,sizeof(tBTBairReqInfo));
	BTData &mData = BTData::GetInstance();
	mData.GetBtPairingReq(tBTBairReqInfo);

	if(tBTBairReqInfo.state == 1)
	{
		//显示请求弹窗
		GetViewBase()->ShowWnd(UIID_WNDID_BT_PAIRREQ, true);
		char szTemp1[2*1024] = {0};//连接设备名
		wchar_t szTip2[TEXT_BUF_LEN] = {0};//连接设备passcode
		size_t nLen = 1024;
		memset(szTemp1,0,sizeof(szTemp1));
		memset(szTip2,0,sizeof(szTip2));
		LOGDBG("-----XSJXSJBTPairReq---tBTBairReqInfo.name = %s---tBTBairReqInfo.pass_key = %6d-----", tBTBairReqInfo.name, tBTBairReqInfo.pass_key);
		
		swprintf(szTip2 ,TEXT_BUF_LEN, L"%6d",tBTBairReqInfo.pass_key);
		ConverDecode(tBTBairReqInfo.name,szTemp1,nLen);
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PAIRREQ, 2106, (wchar_t*)szTemp1);
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PAIRREQ, 2107, (wchar_t*)szTip2);
	}
	return true;
}

bool BTView::UpdateBTConnectDevice(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return false;
	}
	BTData &mData = BTData::GetInstance();
	BtPairedDeviceToAppInfo_t tBTPairedList;
	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));

	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_BT_PAIRLIST,1300,UItem);
	CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;
	if(NULL == pList)
	{
		return false;
	}

	pList->DeleteAllItems();

	//获取配对列表，显示
	mData.GetBTPairedListInfo(tBTPairedList);

	int nCount = tBTPairedList.num;


	char szTemp[2*1024] = {0};
	size_t nLen = 1024;

	for(int i = 0; i < nCount; i++)
	{
		memset(szTemp,0,sizeof(szTemp));
		ConverDecode(tBTPairedList.BTDeviceName[i],szTemp,nLen);

		pList->InsertItem(i);
		pList->SetItemString(i,(wchar_t*)szTemp);
		pList->SetItemString(i,319,1);
	}

	return true;
}

void BTView::ConverDecode(char *pInBuf,char *pOutBuf,size_t& nOutLen)
{
	Lock();
	const char *encTo = "UCS-4LE";
	const char *encFrom = "UTF-8";
	iconv_t cd = iconv_open(encTo, encFrom);

	if(cd == (iconv_t)-1)
	{
		perror("iconv_open");
	}
	size_t srclen = strlen(pInBuf);

	LOGDBG("-----------srclen = %d\n",srclen);

	char* srcstart = pInBuf;
	char* tmpoutbuf = pOutBuf;

	size_t ret = iconv(cd, &srcstart, &srclen, &tmpoutbuf, &nOutLen);

	if(ret == -1)
	{
		switch(errno)
		{
			case E2BIG:
				LOGDBG("-------------E2BIG\n");
			break;
			case EILSEQ:
				LOGDBG("-------------EILSEQ\n");
			break;
			case EINVAL:
				LOGDBG("-------------EINVAL\n");
			break;
 		}
		perror("iconv");
	}
	
	LOGDBG("inbuf = %s , srclen = %d , output = %s , outlen = %d\n",pInBuf,srclen,pOutBuf,nOutLen);

	iconv_close(cd);
	UnLock();
}

void BTView::OnClick_BTPairedList(const int nCtrlId, const int nBtStat, const bool nShow)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	switch(nCtrlId)
	{
		case 1003:
		case 1103:
		case 1204:
		{
			GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
		}
		break;
		case UIID_BT_TOPBAR_SETTING:
		{
			GetViewBase()->ShowWnd(UIID_WNDID_SETTING, true);
		}
		break;
		case 1004:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NOPAIRNOTICE,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST,true);//旧配对列表
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NEWLIST,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST_UPDATE,false);

			m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_BT_DISCOVER_REQ,2); //请求停止蓝牙搜索
		}
		break;
		case 1106:
		case 1207:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NOPAIRNOTICE,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_HISTORYLIST,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_NEWLIST,true);//新配对列表
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PAIRLIST,UIID_WNDID_BT_PAIRLIST_PAIRLIST_UPDATE,true);

			m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_BT_DISCOVER_REQ,1); //请求开始蓝牙搜索30s
		}
		break;
		case 1006:
		{
			m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_BT_DISCOVER_REQ,1); //请求开始蓝牙搜索30s
		}
		break;
		case 1100:
		case 1300:
		{
			UIITEMDATA UItem;
			BtPairedDeviceToAppInfo_t tBTPairedList;
			BtBondedDeviceToAppInfo_t tBTDiscoverList;
			memset(&UItem,0,sizeof(UItem));
			memset(&tBTPairedList,0,sizeof(tBTPairedList));
			memset(&tBTDiscoverList,0,sizeof(tBTDiscoverList));
			pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_BT_PAIRLIST,nCtrlId,UItem);
			BTData &mData = BTData::GetInstance();

			CUI_BtList *pBTList = (CUI_BtList*) UItem.m_pCtrl;
			if(NULL == pBTList)
			{
				LOGERR("&&&---get CUI_BtList is NULL");
				break;
			}

			int nLine = 0;
			int ItemData = 0;
			int nCol = 0;

			pBTList->GetCurSel(nLine,ItemData,nCol);
			mData.GetBTPairedListInfo(tBTPairedList);  //历史列表获取
			mData.GetBtBtDiscoverList(tBTDiscoverList); //添加列表获取

			//把历史列表的数据放到容器里
			BtAddr nMacAddr[nLine] = {};
			for(int j = 0; j < 6; j++)
			{
				nMacAddr[nLine].address[j] = tBTPairedList.BTCharAddr[nLine][j];
			}

			if(nCtrlId == 1100)
			{
				LOGDBG("-----XSJXSJXSJ-----tBTDiscoverList.BTCharAddr[%d] = %02x:%02x:%02x:%02x:%02x:%02x",nLine,
				tBTDiscoverList.BTCharAddr[nLine].address[0], tBTDiscoverList.BTCharAddr[nLine].address[1], tBTDiscoverList.BTCharAddr[nLine].address[2],
				tBTDiscoverList.BTCharAddr[nLine].address[3], tBTDiscoverList.BTCharAddr[nLine].address[4], tBTDiscoverList.BTCharAddr[nLine].address[5]);
				m_ViewController->SendWirelessPairInfo(WIRELESS_CMD_SET_INITIATIVE_PAIRING,IVI_BTCONNECT_CMD_CONNECT,nLine,tBTDiscoverList.BTCharAddr[nLine]); //发送发现列表配对请求 
			}
			else if(nCtrlId == 1300)
			{
				LOGDBG("-----XSJXSJXSJ-----tBTPairedList.BTCharAddr[%d] = %02x:%02x:%02x:%02x:%02x:%02x",nLine,
				tBTPairedList.BTCharAddr[nLine][0], tBTPairedList.BTCharAddr[nLine][1], tBTPairedList.BTCharAddr[nLine][2],
				tBTPairedList.BTCharAddr[nLine][3], tBTPairedList.BTCharAddr[nLine][4], tBTPairedList.BTCharAddr[nLine][5]);
				m_ViewController->SendWirelessPairInfo(WIRELESS_CMD_SET_INITIATIVE_PAIRING,IVI_BTCONNECT_CMD_CONNECT,nLine,nMacAddr[nLine]); //发送历史列表配对请求
			}
			



		}
		break;
		default:
		break;
	}
}

void BTView::OnClick_BTPairedReq(long nWndId, const int nCtrlId, const int nBtStat, const bool nShow)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	BTData &mData = BTData::GetInstance();
	int nBTConnectStatus = 0;

	//获取蓝牙连接状态
	nBTConnectStatus = mData.GetBtConnectStatus();

	switch(nCtrlId)
	{
		case 2104: //Cancel
		{
			m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_BT_PAIRING_REQ,1);
			GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
		}
		break;
		case 2105: //Agree
		{
			m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_BT_PAIRING_REQ,0);
			// GetViewBase()->ShowWnd(IVI_SOURCE_HOME, true);
			/*if(nBTConnectStatus == 2)
			{
				GetViewBase()->ShowWnd(m_nMemPairedWnd, true);
			}
			else
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PAIRREQ,2105,238,ITEM_STATE_PUSH);
			}*/
		}
		break;
		default:
		break;
	}
}

void BTView::OnClick_BTMusicPlayWnd(const int nCtrlId, const int nBtStat, const bool nShow)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	switch(nCtrlId)
	{
		case UIID_BT_APPTOPBAR_CLOSE:   //顶栏关闭(1109)
		case UIID_PUBLIC_CLOSE_BIN:     //顶栏关闭(2212)
		{
			GetViewBase()->ShowWnd(UIID_WNDID_BT_MUSIC, false);
			GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
		}
		break;
		case UIID_BT_PAGECONTENT_COMPNAME:
		{
			if(1 == nBtStat)
			{
				GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
			}
		}
		break;
		case UIID_BT_TOPBAR_SETTING:
		{
			GetViewBase()->ShowWnd(UIID_WNDID_SETTING, true);
		}
		break;
		case UIID_BT_PAGECONTENT_PLAYPAUSE: //播放&暂停
		{
			if(1 == nBtStat)
			{
				BTData &mData = BTData::GetInstance();
				m_nCurPlayStatus = mData.GetBTMusicStatus();
				LOGDBG("&&&---m_nCurPlayStatus:%d",m_nCurPlayStatus);
				if(1 == m_nCurPlayStatus)
				{
					T_PLAYMUSIC_CMD tPlayCmd;
					memset(&tPlayCmd,0,sizeof(T_PLAYMUSIC_CMD));

					tPlayCmd.nPlayCmd = CMD_PAUSE;
					tPlayCmd.nParam = 0;

					m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_AUDIO_STATE,CMD_PAUSE);
				}
				else if(0 == m_nCurPlayStatus)
				{
					T_PLAYMUSIC_CMD tPlayCmd;
					memset(&tPlayCmd,0,sizeof(T_PLAYMUSIC_CMD));

					tPlayCmd.nPlayCmd = CMD_PLAY;
					tPlayCmd.nParam = 0;

					m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_AUDIO_STATE,CMD_PLAY);
				}
			}
		}
		break;
		case UIID_BT_PAGECONTENT_NEXTSONG: //下一曲
		{
			if(nBtStat == 1)
			{
				m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_AUDIO_STATE,CMD_NEXT);
			}			
		}
		break;
		case UIID_BT_PAGECONTENT_LASTSONG: //上一曲
		{
			if(nBtStat == 1)
			{
				m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_AUDIO_STATE,CMD_PREV);
			}
		}
		break;
		default:
		break;
	}
}

void BTView::OnNeed_Pause()
{
	BTData &mData = BTData::GetInstance();
	m_nCurPlayStatus = mData.GetBTMusicStatus();
	LOGDBG("&&&---m_nCurPlayStatus:%d",m_nCurPlayStatus);
	if(1 == m_nCurPlayStatus)
	{
		T_PLAYMUSIC_CMD tPlayCmd;
		memset(&tPlayCmd,0,sizeof(T_PLAYMUSIC_CMD));

		tPlayCmd.nPlayCmd = CMD_PAUSE;
		tPlayCmd.nParam = 0;

		m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_AUDIO_STATE,CMD_PAUSE);
	}
}
void BTView::OnNeed_Play()
{
	BTData &mData = BTData::GetInstance();
	m_nCurPlayStatus = mData.GetBTMusicStatus();
	LOGDBG("&&&---m_nCurPlayStatus:%d",m_nCurPlayStatus);
	if(0 == m_nCurPlayStatus)
	{
		T_PLAYMUSIC_CMD tPlayCmd;
		memset(&tPlayCmd,0,sizeof(T_PLAYMUSIC_CMD));

		tPlayCmd.nPlayCmd = CMD_PLAY;
		tPlayCmd.nParam = 0;

		m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_AUDIO_STATE,CMD_PLAY);
	}
}

void BTView::BTInitPhoneWorkWnd()
{
	LOGDBG("BTInitPhoneWorkWnd");
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_NUMKEYBOARD,328,ITEM_STATE_NORMAL);

}

void BTView::BTPhoneOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	LOGDBG("new nWndId=%d,nCtrlId=%d,nBtState=%d",nWndId,nCtrlId,nBtState);

	if ((UIID_BT_APPTOPBAR_CLOSE == nCtrlId) || (UIID_PUBLIC_CLOSE_BIN == nCtrlId))
	{
		GetViewBase()->ShowWnd(UIID_WNDID_BT_PHONE, false);
		GetViewBase()->ShowWnd(UIID_WNDID_BT_MUSIC, false);
		GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
		return;
	}

	if(nCtrlId == UIID_BT_KEYBOARD_DIALBUTTON)
	{
		//拔打
		if(m_ViewController != NULL)
		{
			int len = strlen(m_InputKeyboard);
			unsigned char phone_number[256] = {0};
			memcpy(phone_number, m_InputKeyboard, len);
			m_ViewController->SendCallInfoData(WIRELESS_CMD_SET_DIAL_REQ, phone_number, len);
		}
	}else if(nCtrlId == UIID_BT_CALLING_HANGUP || nCtrlId == UIID_BT_COMECALL_HANGUP || nCtrlId == UIID_BT_ONDIALING_HANGUP)
	{
		//挂断
		if(m_ViewController != NULL)
		{
			m_ViewController->SendWirelessInfoData(WIRELESS_CMD_CALL_ACTION, 1,0);
		}
	}else if(nCtrlId == UIID_BT_COMECALL_ANSWER)
	{
		//接听
		if(m_ViewController != NULL)
		{
			m_ViewController->SendWirelessInfoData(WIRELESS_CMD_CALL_ACTION, 0,0);
		}
	}else if(nCtrlId == UIID_BT_CALLING_MICROPHONE)
	{
		//麦克风
		if(1 == nBtState)
		{
			if(m_ViewController != NULL)
			{
				if(m_nCallMuteMode)
				{
					m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_BTPHONE_MIC_MUTE, 0,0);
				}else{
					m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_BTPHONE_MIC_MUTE, 1,0);
				}

			}else{
				LOGDBG("err, m_ViewController is NULL");
			}
		}
		
	}else if(nCtrlId == UIID_BT_CALLING_PHONEANSWER)
	{
		//免提
		if(m_ViewController != NULL)
		{
			if(m_nCallHandfreeMode)
			{
				m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_BTPHONE_CALL_MODE, 0,0);
			}else{
				m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SET_BTPHONE_CALL_MODE, 1,0);
			}
			
		}else{
			LOGDBG("err, m_ViewController is NULL");
		}
	}else if(nCtrlId == UIID_BT_ADDRESSBK_PRESSBOTTON)
	{	
		if(1 == nBtState)
		{
			UIITEMDATA data;
			pUI->GetUIAnalysisObj()->GetItemData(nWndId, nCtrlId, data);
			if ( data.m_pCtrl )
			{
					CUI_BtList *pList = (CUI_BtList*)data.m_pCtrl;
					int nLine = -1, itemdata = 0, nCol = -1;
					/*
					pList->GetCurClick(nLine, itemdata, nCol);
					if ( nLine != -1 )
					{
							LOGDBG("press line: %d", nLine);						
					}
					*/
					nLine = nCol = -1;
					pList->GetCurSel(nLine, itemdata, nCol);
					LOGDBG("select line: %d", nLine);
					if ( nLine != -1 )
					{						
							BTData &mdata = BTData::GetInstance();
							BtPhoneContactData phone_book;
							mdata.GetBtContacts(phone_book);						
							
							int len = strlen((char *)phone_book.BtPhoneBookInfo[nLine].number);
							m_ViewController->SendCallInfoData(WIRELESS_CMD_SET_DIAL_REQ, phone_book.BtPhoneBookInfo[nLine].number, len);
					}
			}
		}
	}else if(nCtrlId == UIID_BT_CALLRECORD_PRESSBOTTON)
	{	
		if(1 == nBtState)
		{
			UIITEMDATA data;
			pUI->GetUIAnalysisObj()->GetItemData(nWndId, nCtrlId, data);
			if ( data.m_pCtrl )
			{
					CUI_BtList *pList = (CUI_BtList*)data.m_pCtrl;
					int nLine = -1, itemdata = 0, nCol = -1;
					
					nLine = nCol = -1;
					pList->GetCurSel(nLine, itemdata, nCol);
					LOGDBG("select record line: %d", nLine);
					if ( nLine != -1 )
					{						
							BTData &mData = BTData::GetInstance();						
							BtRecordData_t call_history;

							CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
							if (nullptr != pUIMgr)
							{
								int select = 1; 
								int button_status = 1;
								button_status = pUIMgr->GetUIAnalysisObj()->GetItemState(4, UIID_BT_CALLRECORD_ACCEPT);
								if(button_status == 3)
								{
									select = 1;
								}

								button_status = pUIMgr->GetUIAnalysisObj()->GetItemState(4, UIID_BT_CALLRECORD_DIAL);
								if(button_status == 3)
								{
									select = 2;
								}

								button_status = pUIMgr->GetUIAnalysisObj()->GetItemState(4, UIID_BT_CALLRECORD_NOANSWERCALL);
								if(button_status == 3)
								{
									select = 3;
								}
								LOGDBG("select record class: %d", select);
								if(select == 1)
								{
									mData.GetBtCallHistoryAnswer(call_history);
								}else if(select == 2)
								{
									mData.GetBtCallHistoryDial(call_history);
								}else if(select == 3)
								{
									mData.GetBtCallHistoryMiss(call_history);
								}						
								
								int len = strlen((char *)call_history.RecordInfo[nLine].number);
								m_ViewController->SendCallInfoData(WIRELESS_CMD_SET_DIAL_REQ, call_history.RecordInfo[nLine].number, len);
							}						
					}
			}
		}	
	}else if(nCtrlId == UIID_BT_CALLRECORD_ACCEPT || nCtrlId == UIID_BT_CALLRECORD_DIAL || nCtrlId == UIID_BT_CALLRECORD_NOANSWERCALL)
	{
		LOGDBG("call history class select");
		UpdateBTCallHistoryClassInfo(nCtrlId - UIID_BT_CALLRECORD_ACCEPT + 1);
	}else if(nCtrlId == UIID_BT_APPTOPBAR_RENOVATE)
	{
		LOGDBG("request contact refresh");
		if(1 == nBtState)
		{
			if(m_ViewController != NULL)
			{
				m_ViewController->SendWirelessInfoData(WIRELESS_CMD_SYNC_CONTACT, 0,0);				
			}
		}
	}else if(1 == nBtState)
	{
		BTPhoneUpdateHomeContenInfo(nWndId,nCtrlId,nBtState,1);
		OnClick_BTPhoneKeyborad(nWndId,nCtrlId,nBtState,1);
	}
	else if(0 == nBtState)
	{}
}

void BTView::BTPhoneUpdateHomeContenInfo(long nWndId,const int nCtrlId, const int nBtState, const bool nShow)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	switch(nCtrlId)
	{
		// case UIID_BT_APPTOPBAR_CLOSE://关闭
		// {
		// 	GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
		// }
		// break;
		case UIID_BT_APPTOPBAR_RENOVATE://刷新
		{
			GetViewBase()->ShowWnd(UIID_WNDID_BT_PHONE, true);
		}
		break;
		case UIID_BT_LEFTBAR_NUMKEYBOARD://键盘
		{
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_ADDRESSBOOK,330,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_CALLRECORD,334,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_NUMKEYBOARD,326,ITEM_STATE_PUSH);
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_NUMKEYBOARD,326,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_NUMKEYBOARD,326,ITEM_STATE_SELECTED);
			int call_status = m_nCallStatus;
			// 来电/通话页显示时收起拨号盘，避免键盘和整页通话界面重叠
			bool bHideKeyboard = (call_status == BTCALL_STATE_INCOMING ||
								  call_status == BTCALL_STATE_PRORESS  ||
								  call_status == BTCALL_STATE_HOLD     ||
								  call_status == BTCALL_STATE_PARTIALHOLD);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_KEYBOARD_ALL,!bHideKeyboard);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_APPTOPBAR_DEVICENAME,true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ADDRESSBK_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_CALLRECORD_ALL,false);

			HideAllCallPages();          // 先清空，保证单页
			switch(call_status)
			{				
				case BTCALL_STATE_INCOMING:				
					pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_COMECALL_ALL,true);
					break;
				case BTCALL_STATE_OUTGOING:			
					pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ONDIALING_ALL,true);
					break;
				case BTCALL_STATE_PRORESS:
				case BTCALL_STATE_HOLD:
				case BTCALL_STATE_PARTIALHOLD:
					pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_ALL,true);
					break;
			}
			m_nTabStatus = 0;
		}
		break;
		case UIID_BT_LEFTBAR_ADDRESSBOOK://电话本
		{
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_NUMKEYBOARD,326,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_CALLRECORD,334,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_ADDRESSBOOK,330,ITEM_STATE_PUSH);
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_ADDRESSBOOK,330,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_ADDRESSBOOK,330,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_KEYBOARD_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_APPTOPBAR_DEVICENAME,true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ADDRESSBK_ALL,true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_CALLRECORD_ALL,false);
			//hide call info
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ONDIALING_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_COMECALL_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_ALL,false);
			m_nTabStatus = 1;
		}
		break;
		case UIID_BT_LEFTBAR_CALLRECORD://通话记录
		{
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_NUMKEYBOARD,326,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_ADDRESSBOOK,330,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_CALLRECORD,334,ITEM_STATE_PUSH);
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_CALLRECORD,334,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_LEFTBAR_CALLRECORD,334,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_KEYBOARD_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_APPTOPBAR_DEVICENAME,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ADDRESSBK_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_CALLRECORD_ALL,true);
			//hide call info
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ONDIALING_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_COMECALL_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_ALL,false);
			m_nTabStatus = 2;
		}
		break;
		default:
		break;
	}
}

void BTView::OnClick_BTPhoneKeyborad(long nWndId, const int nCtrlId, const int nBtStat, const bool nShow)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	BTData &mData = BTData::GetInstance();

	switch(nCtrlId)
	{
		case UIID_BT_KEYBOARD_ONE:
		{
			//1
			//swprintf(InputKeyboard, 60, L"%d", 1);
			//szKeyboardInput[currentLen] = '1';
			//pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PHONE, UIID_BT_KEYBOARD_INPUTBLANK, szKeyboardInput);
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '1';
			LOGDBG("---xsj---keyboard---1---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_TWO:
		{
			//2
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '2';
			LOGDBG("---xsj---keyboard---2---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_THREE:
		{
			//3
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '3';
			LOGDBG("---xsj---keyboard---3---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_FOUR:
		{
			//4
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '4';
			LOGDBG("---xsj---keyboard---4---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_FIVE:
		{
			//5
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '5';
			LOGDBG("---xsj---keyboard---5---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_SIX:
		{
			//6
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '6';
			LOGDBG("---xsj---keyboard---6---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_SEVEN:
		{
			//7
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '7';
			LOGDBG("---xsj---keyboard---7---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_EIGHT:
		{
			//8
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '8';
			LOGDBG("---xsj---keyboard---8---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_NINE:
		{
			//9
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '9';
			LOGDBG("---xsj---keyboard---9---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_ZERO:
		{
			//0
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '0';
			LOGDBG("---xsj---keyboard---0---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_ASTERISK:
		{
			//*
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '*';
			LOGDBG("---xsj---keyboard---*---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_POUNDKEY:
		{
			//#
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '#';
			LOGDBG("---xsj---keyboard---#---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_PLUS:
		{
			//+
			int nLen = strlen(m_InputKeyboard);
			m_InputKeyboard[nLen] = '+';
			LOGDBG("---xsj---keyboard---+---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			break;
		}
		case UIID_BT_KEYBOARD_DELETE:
		{
			//删除
			char strTempData[TEXT_BUF_LEN] = {0};
			int nTempLen = strlen(m_InputKeyboard);

			if(nTempLen > 0)
			{
				memcpy(strTempData,m_InputKeyboard,TEXT_BUF_LEN);
				memset(m_InputKeyboard,0,sizeof(m_InputKeyboard));
				memcpy(m_InputKeyboard,strTempData,nTempLen - 1);
			}
			break;
		}
		case UIID_BT_KEYBOARD_DIALBUTTON:
		{
			//拨通
			LOGDBG("---xsj---keyboard---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,2201,true);
			//m_ViewController->SendWirelessInfoData(WIRELESS_CMD_CALL_ACTION,0);  //下发拨电话请求(目前只有接听和挂断)
			break;
		}
		case UIID_BT_ONDIALING_HANGUP:
		{
			//挂断
			LOGDBG("---xsj---keyboard---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,2201,false);
			m_ViewController->SendWirelessInfoData(WIRELESS_CMD_CALL_ACTION,1);
			break;
		}
		case UIID_BT_ONDIALING_PHONEANSWER:
		{
			//免提
			LOGDBG("---xsj---keyboard---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			int nCallMode = mData.GetBTPhoneCallMode();
			if(nCallMode == 0)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_PHONEANSWER,334,ITEM_STATE_SELECTED);
				m_ViewController->SendWirelessInfoData(WIRELESS_EVENT_BT_PHONE_CALL_MODE,1);
			}
			else if(nCallMode == 1)
			{
				//pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_PHONEANSWER,334,ITEM_STATE_NORMAL);
				m_ViewController->SendWirelessInfoData(WIRELESS_EVENT_BT_PHONE_CALL_MODE,0);
			}
			break;
		}
		case UIID_BT_CALLING_MICROPHONE:
		{
			//关麦
			LOGDBG("---xsj---keyboard---:nWndId=%d,nCtrlId=%d,nBtStat=%d",nWndId,nCtrlId,nBtStat);
			int nCallMode = mData.GetBTMuteStatus();
			if(nCallMode == 0)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_MICROPHONE,405,ITEM_STATE_SELECTED);
				m_ViewController->SendWirelessInfoData(WIRELESS_EVENT_BT_PHONE_MIC_MUTE,1);
			}
			else if(nCallMode == 1)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_MICROPHONE,405,ITEM_STATE_NORMAL);
				m_ViewController->SendWirelessInfoData(WIRELESS_EVENT_BT_PHONE_MIC_MUTE,0);
			}
			break;
		}
		default:
		break;
	}

	wchar_t strTempInput[TEXT_BUF_LEN] = {0};
	swprintf(strTempInput, TEXT_BUF_LEN, L"%s", m_InputKeyboard);

	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PHONE, UIID_BT_KEYBOARD_INPUTBLANK, strTempInput);
}

void BTView::OnClick_BTUnconnected(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}


}

void BTView::HideAllCallPages()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE, UIID_BT_ONDIALING_ALL, false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE, UIID_BT_COMECALL_ALL,  false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE, UIID_BT_CALLING_ALL,  false);
}

void BTView::ResetCallUI()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	// 1) 收起所有通话相关整页和按钮
	HideAllCallPages();
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE, UIID_BT_ONDIALING_END,      false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE, UIID_BT_KEYBOARD_ALL,        true);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE, UIID_BT_KEYBOARD_DIALBUTTON,  true);

	// 2) 视图状态复位
	m_nCallStatus       = BTCALL_STATE_IDLE;
	m_nTabStatus        = 0;
	m_nCallMuteMode     = 0;
	m_nCallHandfreeMode = 0;
	memset(m_InputKeyboard, 0, TEXT_BUF_LEN);

	// 3) 数据状态复位，避免下次进页面还残留上一通的号码
	BTData &mData = BTData::GetInstance();
	mData.SaveBTPhoneCallState(BTCALL_STATE_IDLE);
	mData.ClearBTPhoneCallInfo();

	LOGDBG("ResetCallUI done");
}

bool BTView::UpdateBTCallInfo(T_APP_MSG* msg)
{
	unsigned char call_status = msg->Param0;
	LOGDBG("UpdateBTCallInfo, call_status=%d", msg->Param0);
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}
	BTData &mdata = BTData::GetInstance();
	T_STR_BT_CALLERINFO call_info;
	mdata.GetBTPhoneInfo(call_info);
	/*
	LOGDBG("addr = %s", tBTCallinfo->addr);
				LOGDBG("PhoneNum = %s", call_info->PhoneNum);
				LOGDBG("Name = %s", call_info->Name);
				LOGDBG("FName = %s", call_info->FName);
				LOGDBG("nState = %d", call_info->nState);
	*/
	char out_str[128] = {0};
	size_t out_len = 128;

	wchar_t strTempInput[TEXT_BUF_LEN] = {0};
	int nPrevCallStatus = m_nCallStatus;
	LOGDBG("UpdateBTCallInfo prev=%d, cur=%d", nPrevCallStatus, call_status);

	bool bBusyCall = (nPrevCallStatus == BTCALL_STATE_INCOMING    ||
					  nPrevCallStatus == BTCALL_STATE_OUTGOING    ||
					  nPrevCallStatus == BTCALL_STATE_PRORESS     ||
					  nPrevCallStatus == BTCALL_STATE_HOLD        ||
					  nPrevCallStatus == BTCALL_STATE_PARTIALHOLD);
	bool bNewCall  = (call_status == BTCALL_STATE_INCOMING ||
					  call_status == BTCALL_STATE_HOLD     ||
					  call_status == BTCALL_STATE_PARTIALHOLD);
	if(bBusyCall && bNewCall)
	{
		LOGDBG("call waiting ignored by view");
		return true;
	}

	m_nCallStatus = call_status;
	m_nTabStatus = 0;
	switch(call_status)
	{
		case BTCALL_STATE_IDLE:
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ONDIALING_END,true);
//			sleep(1);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_COMECALL_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ONDIALING_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_ALL,false);
			if(m_nTabStatus == 0)
			{
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_KEYBOARD_ALL,true);
			}
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_KEYBOARD_DIALBUTTON,true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ONDIALING_END,false);
			break;
        case BTCALL_STATE_INCOMING:		
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_KEYBOARD_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ONDIALING_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_CALLRECORD_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ADDRESSBK_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_KEYBOARD_DIALBUTTON,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ONDIALING_END,false);

			ConverDecode(call_info.Name,out_str,out_len);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PHONE, UIID_BT_COMECALL_ADDRESSNAME, (wchar_t*)out_str);
			memset(out_str, 0, 128);
			ConverDecode(call_info.PhoneNum,out_str,out_len);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PHONE, UIID_BT_COMECALL_PHONENUMBER, (wchar_t*)out_str);
			if(m_nTabStatus == 0)
			{
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_COMECALL_ALL,true);
			}
			break;
        case BTCALL_STATE_OUTGOING:
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_KEYBOARD_DIALBUTTON,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ONDIALING_END,false);

			memset(m_InputKeyboard, 0, TEXT_BUF_LEN);
            swprintf(strTempInput, TEXT_BUF_LEN, L"%s", m_InputKeyboard);
            pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PHONE, UIID_BT_KEYBOARD_INPUTBLANK, strTempInput);        //clean inputblank

			//hide contact and call history tab
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_CALLRECORD_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ADDRESSBK_ALL,false);

			if(strlen(call_info.Name) > 0)
			{
				ConverDecode(call_info.Name,out_str,out_len);
			}else{
				ConverDecode(call_info.PhoneNum,out_str,out_len);
			}			
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PHONE, UIID_BT_ONDIALING_ADDRESSNAME, (wchar_t*)out_str);
			if(m_nTabStatus == 0)
			{
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ONDIALING_ALL,true);
			}
			break;
        case BTCALL_STATE_PRORESS:
		case BTCALL_STATE_HOLD:
        case BTCALL_STATE_PARTIALHOLD:
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_COMECALL_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ONDIALING_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ONDIALING_END,false);
			//hide contact and call history tab
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_CALLRECORD_ALL,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_ADDRESSBK_ALL,false);

					
			ConverDecode(call_info.Name,out_str,out_len);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PHONE, UIID_BT_CALLING_ADDRESSNAME, (wchar_t*)out_str);
			memset(out_str, 0, 128);
			ConverDecode(call_info.PhoneNum,out_str,out_len);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PHONE, UIID_BT_CALLING_PHONENUMBER, (wchar_t*)out_str);
			if(m_nTabStatus == 0)
			{
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_ALL,true);
			}
			break;
	}
	return true;
}
bool BTView::UpdateBTCallTimeInfo(T_APP_MSG* msg)
{
	if(msg == NULL)
	{
		return false;
	}
	size_t out_len = 32;
	char time_str[32]= {0}, time_str_out[32]= {0};
	if(msg->Param0 > 0)
	{
		sprintf(time_str, "%02d:%02d:%02d", msg->Param0, msg->Param1, msg->Param2);
	}else{
		sprintf(time_str, "%02d:%02d", msg->Param1, msg->Param2);
	}
	
	LOGDBG("%s, %s", __func__, time_str);

	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}

	ConverDecode(time_str,time_str_out,out_len);
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PHONE, UIID_BT_CALLING_CALLTIME, (wchar_t*)time_str_out);
	
	return true;
}

bool BTView::UpdateBTCallMuteInfo(T_APP_MSG* msg)
{
	LOGDBG("call mute_status=%d", msg->Param0);
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}
	m_nCallMuteMode = msg->Param0;
	if(m_nCallMuteMode == 0)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_MICROPHONE,405,ALL_STATE);		
	}
	else if(m_nCallMuteMode == 1)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_MICROPHONE,407,ALL_STATE);		
	}
	return true;
}

bool BTView::UpdateBTCallHandfreeInfo(T_APP_MSG* msg)
{
	LOGDBG("call handfree_status=%d", msg->Param0);
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}
	m_nCallHandfreeMode = msg->Param0;
	if(m_nCallHandfreeMode == 0)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_CALLING_PHONEANSWER,393,ALL_STATE);
		

	}
	else if(m_nCallHandfreeMode == 1)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE, UIID_BT_CALLING_PHONEANSWER, 395, ALL_STATE);
	}
	
	return true;
}

bool BTView::UpdateBTCallContactInfo(T_APP_MSG* msg)
{
	LOGDBG("%s", __func__);
	UIITEMDATA dat;

	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	} 
	pUIMgr->GetUIAnalysisObj()->GetItemData(4, UIID_BT_ADDRESSBK_PRESSBOTTON, dat);

	int name_pos = 0;
	int phonenumber_pos = 1;
	size_t out_len = 2048;

	char name_str[256] = {0};
	char phone_str[256] = {0};
	char name_str_out[2048] = {0};
	char phone_str_out[2048] = {0};

	if ( dat.m_pCtrl )
	{
			CUI_BtList *pList = (CUI_BtList *)dat.m_pCtrl;
			pList->DeleteAllItems();
			BTData &mdata = BTData::GetInstance();
			BtPhoneContactData phone_book;
			mdata.GetBtContacts(phone_book);
			LOGDBG("xxxxxxxxxxxxxxxxxxxxx contact_number= %d",phone_book.BtPhoneContacts);
			for(int i = 0; i < phone_book.BtPhoneContacts;  i++)
			{
					strcpy(name_str, (char * )phone_book.BtPhoneBookInfo[i].N_name);
					strcpy(phone_str ,(char *)phone_book.BtPhoneBookInfo[i].number);

					out_len = 2048;
					ConverDecode(name_str, name_str_out, out_len);
					out_len = 2048;
					ConverDecode(phone_str, phone_str_out, out_len);

					pList->InsertEmptyItem(i);
					pList->SetItemString(i, (wchar_t*)name_str_out, name_pos);
					pList->SetItemString(i, (wchar_t*)phone_str_out, phonenumber_pos);	

					memset(name_str, 0, 128);
					memset(phone_str, 0, 128);
					memset(name_str_out, 0, 1024);
					memset(phone_str_out, 0, 1024);
			}

	}

	return true;
}

bool BTView::UpdateBTCallHistoryClassInfo(int history_class)
{
	LOGDBG("%s, history class=%d", __func__, history_class);
	UIITEMDATA dat;	
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	} 
	pUIMgr->GetUIAnalysisObj()->GetItemData(4, UIID_BT_CALLRECORD_PRESSBOTTON, dat);

	int name_pos = 0;	
	size_t out_len = 1024;

	char name_str[128] = {0};	
	char name_str_out[1024] = {0};	

	if ( dat.m_pCtrl )
	{
			CUI_BtList *pList = (CUI_BtList *)dat.m_pCtrl;
			pList->DeleteAllItems();

			BTData &mdata = BTData::GetInstance();
			BtRecordData_t call_history;
			if(history_class == 1)
			{
				mdata.GetBtCallHistoryAnswer(call_history);
			}else if(history_class == 2)
			{
				mdata.GetBtCallHistoryDial(call_history);
			}else{
				mdata.GetBtCallHistoryMiss(call_history);
			}			

			for( int i=0; i<call_history.count; i++ )
			{				
					if(strlen((char *)call_history.RecordInfo[i].N_name))
					{
						strcpy(name_str, (char *)call_history.RecordInfo[i].N_name);		
					}else{
						strcpy(name_str, (char *)call_history.RecordInfo[i].number);		
					}								
					out_len = 1024;
					ConverDecode(name_str, name_str_out, out_len);					

					pList->InsertEmptyItem(i);
					pList->SetItemString(i, (wchar_t*)name_str_out, name_pos);	
					memset(name_str, 0, 128);
					memset(name_str_out, 0, 1024);			
			}
	}

	return true;
}
//#define BT_PHONE_TEST
#ifdef BT_PHONE_TEST
bool BTView::UpdateBTCallHistoryInfo()
#else
bool BTView::UpdateBTCallHistoryInfo(T_APP_MSG* msg)
#endif
{
	LOGDBG("%s test\n", __func__);
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}
	int select = 0; 
	select = pUIMgr->GetUIAnalysisObj()->GetItemState(4, UIID_BT_CALLRECORD_ACCEPT);
	if(select == 3)
	{
		UpdateBTCallHistoryClassInfo(1);
		return true;
	}

	select = pUIMgr->GetUIAnalysisObj()->GetItemState(4, UIID_BT_CALLRECORD_DIAL);
	if(select == 3)
	{
		UpdateBTCallHistoryClassInfo(2);
		return true;
	}

	select = pUIMgr->GetUIAnalysisObj()->GetItemState(4, UIID_BT_CALLRECORD_NOANSWERCALL);
	if(select == 3)
	{
		UpdateBTCallHistoryClassInfo(3);
		return true;
	}

	UpdateBTCallHistoryClassInfo(1);
	return true;
	
}

bool BTView::UpdateBTBatteryInfo(T_APP_MSG* msg)
{
	LOGDBG("%s, battery=%d", __func__, msg->Param0);

	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}
	int battery = msg->Param0;
	if(battery > 5)
	{
		battery = 5;
	}
	int img_id = 303 + battery * 2;

	pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_BT_PHONE,UIID_BT_APPTOPBAR_BATTERY,img_id,ITEM_STATE_NORMAL);		
	
	return true;
}

bool BTView::UpdateBTDeviceName(T_APP_MSG* msg)
{
	LOGDBG("%s", __func__);


	BTData &mData = BTData::GetInstance();
	
	int nBTConnectStatus = 0;
	char bt_device_name[BT_MAX_LOCAL_NAME_LENGTH] = {"pioneer"};
	mData.GetBtDeviceName(bt_device_name);
	nBTConnectStatus = mData.GetBtConnectStatus();

	LOGDBG("bt device name:%s", bt_device_name);

	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}
	char out_str[128] = {0};
	size_t out_len = 128;

	if(nBTConnectStatus == 2)
	{
		if(strlen(bt_device_name) > 0)
		{
			ConverDecode(bt_device_name, out_str, out_len);	
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_BT_PHONE, UIID_BT_APPTOPBAR_DEVICENAME, (wchar_t*)out_str);
		}
	}				

	return true;
}