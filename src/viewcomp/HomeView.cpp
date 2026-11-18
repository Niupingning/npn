/*
 * HomeView.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include "HomeView.h"
#include "UIID_Def_Home.h"
#include "UIID_Def_AudioBoost.h"
#include "AssertDefine.h"

// Dependent on external header files
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

HomeView::HomeView(ViewBase* p_ViewBase)
: CompBase(p_ViewBase)
, m_nCurrWnd(0)
, m_ViewController(NULL)
{
	// TODO Auto-generated constructor stub
	HomeInitUpdateMsgMap();

	m_nUsb_Insert = 0;
	m_bAvmStart = false;

	m_nCurAVM_View = 1;
}

HomeView::~HomeView()
{
	// TODO Auto-generated destructor stub
}

void HomeView::HomeInitUpdateMsgMap()
{
	m_MsgMap.clear();	
	/*
 	*顶栏*/
	//m_MsgMap.insert(make_pair(METER_VMSG_BT_MUSICPLAY_STATE, &HomeView::UpdateBTMusicPlay));
	//m_MsgMap.insert(make_pair(METER_VMSG_BT_MUSIC_INFO, &HomeView::UpdateBTMusicInfo));

	m_MsgMap.insert(make_pair(METER_VMSG_ACCSTATUS, &HomeView::Update_CP_Show));
	m_MsgMap.insert(make_pair(CARPLAY_VMSG_CONNECTSTATUS, &HomeView::Update_CP_Connet));
	m_MsgMap.insert(make_pair(ANDRIOD_VMSG_CONNECTSTATUS, &HomeView::Update_AA_Connet));
	m_MsgMap.insert(make_pair(SYS_VMSG_USBMOUNT, &HomeView::Update_USB_Insert));	
}

void HomeView::HomeGetUpdateMessageMap(map<int, pUpdateMsg> &nMap)
{
	nMap = m_MsgMap;
}

bool HomeView::HomeOnUserMessage(T_APP_MSG *msg)
{
	if (nullptr == msg)
	{
		return false;
	}
	//LOGDBG("HomeOnUserMessage pthread_ID = %d",(int)pthread_self());

	LOGDBG("Message = %d,param0=%d,param1=%d,param2=%d\n",
		   msg->Message, msg->Param0, msg->Param1, msg->Param2);
	map<int, pUpdateMsg> nUpdateMap;
	nUpdateMap.clear();
	HomeGetUpdateMessageMap(nUpdateMap);
	auto Itr = nUpdateMap.find(msg->Message);
	if (Itr != nUpdateMap.end())
	{
		(this->*(Itr->second))(msg);
	}

	return true;
}

bool HomeView::Update_CP_Show(T_APP_MSG* msg)
{
	if(UIID_WNDID_MAIN == msg->Param0)
	{
		int nCurWnd = ((IVIView*)GetViewBase())->GetCurrWnd();
		if((nCurWnd == UIID_WNDID_AVM) || (nCurWnd == UIID_WNDID_BLACK))
		{
			GetViewBase()->ShowWnd(UIID_WNDID_SPLIT, false);
			GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
		}
		else if (UIID_WNDID_MAIN != nCurWnd)
		{
			GetViewBase()->ShowWnd(nCurWnd, true);
		}

		KillTimer(TIMERID_CPAA_SIGN);
	}
	else if(UIID_WNDID_BLACK == msg->Param0)
	{
		LOGDBG("&&&----cp---show");
		LOGFILE("cp---show Param0=%d", msg->Param0);
		int nCurSource = ((IVIView*)GetViewBase())->Get_CurSource();
		if(nCurSource != IVI_SOURCE_CPAA)
		{
			((IVIView*)GetViewBase())->OnHandle_Source(nCurSource);
			((IVIView*)GetViewBase())->Save_CurSource(IVI_SOURCE_CPAA);
		}
		GetViewBase()->ShowWnd(UIID_WNDID_BLACK, true);

		HomeData &mData = HomeData::GetInstance();
		int nAppStatus = APP_STATUS_NO;
		int nSplit = 0;
		mData.GetINTData(INT_APPSTATUS, nAppStatus);
		if (APP_STATUS_CP == nAppStatus)
			mData.GetINTData(INT_CP_SPLITSTATUS, nSplit);
		else if (APP_STATUS_AA == nAppStatus)
			mData.GetINTData(INT_AA_SPLITSTATUS, nSplit);
		((IVIView*)GetViewBase())->ShowSplitWnd(1 == nSplit);

		KillTimer(TIMERID_CPAA_SIGN);
		SetTimer(TIMERID_CPAA_SIGN,2000);
	}
	/*
	HomeData &mData = HomeData::GetInstance();
	int nAAShowStatus = 0;

	mData.GetINTData(INT_AASHOWSTATUS, nAAShowStatus);
	*/
	return true;
}

bool HomeView::Update_CP_Connet(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}
	HomeData &mData = HomeData::GetInstance();

	int nCPConnectStatus = 0;
	mData.GetINTData(INT_CARPLAYCONNECTSTATUS, nCPConnectStatus);

	LOGDBG("&&&&----nCPConnectStatus:%d",nCPConnectStatus);
	LOGFILE("CPConn=%d", nCPConnectStatus);

	if(1 == nCPConnectStatus)
	{
		//cp connect icon show
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MAIN,UIID_HOME_CP,ITEM_STATE_NORMAL);
	}
	else
	{
		//cp diconnect icon disable
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MAIN,UIID_HOME_CP,16,ITEM_STATE_DISABLE);
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MAIN,UIID_HOME_CP,ITEM_STATE_DISABLE);

		int nAppStatus = APP_STATUS_NO;
		mData.GetINTData(INT_APPSTATUS, nAppStatus);
		if ((nullptr != msg) && (APP_STATUS_CP == nAppStatus))
		{
			((IVIView*)GetViewBase())->CloseSplitWnd();
		}
	}

	return true;
}

bool HomeView::Update_AA_Connet(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}
	HomeData &mData = HomeData::GetInstance();

	int nAAConnectStatus = 0;
	mData.GetINTData(INT_ANDRIODCONNECTSTATUS, nAAConnectStatus);

	LOGDBG("&&&&----nAAConnectStatus:%d",nAAConnectStatus);

	if(1 == nAAConnectStatus)
	{
		//cp connect icon show
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MAIN,UIID_HOME_AA,ITEM_STATE_NORMAL);
	}
	else
	{
		//cp diconnect icon disable
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MAIN,UIID_HOME_AA,15,ITEM_STATE_DISABLE);
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MAIN,UIID_HOME_AA,ITEM_STATE_DISABLE);

		int nAppStatus = APP_STATUS_NO;
		mData.GetINTData(INT_APPSTATUS, nAppStatus);
		if ((nullptr != msg) && (APP_STATUS_AA == nAppStatus))
		{
			((IVIView*)GetViewBase())->CloseSplitWnd();
		}
	}

	return true;
}

bool HomeView::Update_USB_Insert(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}

	if(NULL != msg)
	{
		m_nUsb_Insert = msg->Param0;
	}
	else
	{
    	MediaView* pMedia = ((IVIView*)GetViewBase())->GetMediaView();
    	if (NULL != pMedia)
    	{
        	m_nUsb_Insert = pMedia->GetUsbInsert();
    	}
	}

	if(1 == m_nUsb_Insert)
	{
		//media icon show
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MAIN,UIID_HOME_MEDIA,ITEM_STATE_NORMAL);
	}
	else
	{
		//media icon disable
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MAIN,UIID_HOME_MEDIA,17,ITEM_STATE_DISABLE);
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MAIN,UIID_HOME_MEDIA,ITEM_STATE_DISABLE);
	}

	return true;
}

void HomeView::Update_Avm_Icon()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	switch(m_nCurAVM_View)
	{
		case 1:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AVM,UIID_AVM_3D,ITEM_STATE_SELECTED);
		}
		break;
		case 2:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AVM,UIID_AVM_FRONT,ITEM_STATE_SELECTED);
		}
		break;
		case 3:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AVM,UIID_AVM_REAR,ITEM_STATE_SELECTED);
		}
		break;
		case 4:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AVM,UIID_AVM_LEFT,ITEM_STATE_SELECTED);
		}
		break;
		case 5:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AVM,UIID_AVM_RIGHT,ITEM_STATE_SELECTED);
		}
		break;
		default:
		break;
	}
}
void HomeView::HomeOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	LOGDBG("nWndId=%d,nCtrlId=%d,nBtState=%d",nWndId,nCtrlId,nBtState);
	if(1 == nBtState)
	{
		HomeUpdateHomeContenInfo(nCtrlId,nBtState,1);
	}
}

void HomeView::HomeOnComponent(CUIWndMng *pUI, PWINDOW_INFO pInfo)
{
	// LOGDBG("pInfo.nInfoType=%d,nWindowID=%d,nCtrlID=%d ,status=%d\n",\
	// 		pInfo->nInfoType, pInfo->nWindowID, pInfo->nCtrlID, pInfo->nOperationStatus);
}

void HomeView::OnTimer(uint32_t id)
{
	//	LOGDBG("HomeView pthread_ID = %d",(int)pthread_self());
	//	LOGDBG("HomeView::OnTimer id = %d", id);
	switch(id)
	{
		case TIMERID_AVM_SIGN:
		{
			//HomeData &mHomeData = HomeData::GetInstance();
			//int nAvmSign = 0;

			//mHomeData.GetINTData(INT_AVM_SIGN,nAvmSign);
			//LOGDBG("&&&----timer---nAvmSign:%d",nAvmSign);
			KillTimer(TIMERID_AVM_SIGN);
			((IVIView*)GetViewBase())->SetWindow_BK(UIID_WNDID_AVM,11);

			/*if(1 == nAvmSign)
			{
				KillTimer(TIMERID_AVM_SIGN);
				((IVIView*)GetViewBase())->SetWindow_BK(UIID_WNDID_AVM,11);
			}*/
		}
		break;
		case TIMERID_CPAA_SIGN:
		{
			KillTimer(TIMERID_CPAA_SIGN);
			((IVIView*)GetViewBase())->SetWindow_BK(UIID_WNDID_BLACK,11);			
		}
		break;
		default:
		break;
	}

}

int HomeView::GetCurrWnd()
{
	HomeData &mData = HomeData::GetInstance();
	Lock();
	m_nCurrWnd = mData.GetCurrWnd();
	UnLock();
	return m_nCurrWnd;
}

void HomeView::HomeInitWorkWnd()
{
	LOGDBG("HomeInitWorkWnd");

	Update_CP_Connet();
	Update_AA_Connet();
	Update_USB_Insert();
	Update_Avm_Icon();

	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	pUIMgr->GetUIAnalysisObj()->SetItemState(
		UIID_WNDID_MAIN, UIID_HOME_DTS, ITEM_STATE_DISABLE);
	pUIMgr->GetUIAnalysisObj()->SetItemState(
		UIID_WNDID_MAIN, UIID_HOME_TI, ITEM_STATE_DISABLE);
}

void HomeView::HomeUpdateHomeContenInfo(const int nCtrlId, const int nBtStat, const bool nShow)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	switch(nCtrlId)
	{
		case UIID_HOME_MEDIA:
		{
			LOGDBG("&&&-----usb----");
			//GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_AUDIO_PLAY, true);
			((IVIView*)GetViewBase())->OnIVI_WinShow(IVI_SOURCE_MEDIA);
		}
		break;
		case UIID_HOME_RADIO:
		{
			LOGDBG("&&&-----radio----");
			GetViewBase()->ShowWnd(UIID_WNDID_RADIO, true);
		}
		break;
		case UIID_HOME_BTMUSIC:
		{
			((IVIView*)GetViewBase())->OnIVI_WinShow(IVI_SOURCE_BT_MUSIC);
		}
		break;
		case UIID_HOME_BTPHONE:
		{
			((IVIView*)GetViewBase())->OnIVI_WinShow(IVI_SOURCE_BT_PHONE);
		}
		break;
		case UIID_HOME_AIRCOND:
		{
			GetViewBase()->ShowWnd(UIID_WNDID_AIRCOND, true);
		}
		break;
		case UIID_HOME_AUDIOBOOST:
		{
			GetViewBase()->ShowWnd(UIID_WNDID_AUDIOBOOST, true);
		}
		break;
		case UIID_HOME_AVM:
		{
			//avm
			if(NULL != m_ViewController)
			{
				int nCurSource = ((IVIView*)GetViewBase())->Get_CurSource();

				if(nCurSource != IVI_SOURCE_AVM)
				{
					//上个源暂停
					((IVIView*)GetViewBase())->OnHandle_Source(nCurSource);

					//保存为当前源
					((IVIView*)GetViewBase())->Save_CurSource(IVI_SOURCE_AVM);
				}

				m_ViewController->AVM_InitDispWin();
				//((IVIView*)GetViewBase())->SetWindow_BK(UIID_WNDID_AVM,11);
				//m_ViewController->SetSystem_AVMSetting(1,0,0); //发给MCU打开AVM
				//m_ViewController->AVM_StreamOn();   //AVM

				if(!m_bAvmStart)
				{
					KillTimer(TIMERID_AVM_SIGN);
					SetTimer(TIMERID_AVM_SIGN,8000);
					m_bAvmStart = true;
					OnStart_AVM();
				}
				else
				{
					KillTimer(TIMERID_AVM_SIGN);
					SetTimer(TIMERID_AVM_SIGN,200);
					OnStartAvm_View(m_nCurAVM_View);
				}
			}
			
			GetViewBase()->ShowWnd(UIID_WNDID_AVM, true);
		}
		break;
		case UIID_HOME_AA:
		{
			//AA
			m_ViewController->SendAndroidautoData(APP_APPSERVICE_OPENAA_TYPE,1);
		}
		break;
		case UIID_HOME_CP:
		{
			//CP
			m_ViewController->SendCarPlayData(APP_APPSERVICE_OPENCARPLAY_TYPE,1);
		}
		break;
		case UIID_PUBLIC_SETTINGH_BIN:
		{
			//setting
			((IVIView*)GetViewBase())->SaveIn_SettingWnd(UIID_WNDID_MAIN);
			GetViewBase()->ShowWnd(UIID_WNDID_SETTING, true);
		}
		break;
		// 原始版本
		// case UIID_HOME_DTS:
		// {
		// 	//dts
		// 	GetViewBase()->ShowWnd(UIID_WNDID_DTS, true);
		// }
		// break;
		// case UIID_HOME_TI:
		// {
		// 	//TI
		// 	GetViewBase()->ShowWnd(UIID_WINID_TI, true);
		// }
		// break;
		// 原始版本
		// 适配GUI显示该项目无该功能
		case UIID_HOME_DTS:
		case UIID_HOME_TI:
		{
			LOGDBG("DTS/TI function is disabled, ignore click, id=%d", nCtrlId);
		}
		break;
		// 适配GUI显示该项目无该功能
		default:
		break;
	}
}

void HomeView::KillTimer_AvmShow()
{
	KillTimer(TIMERID_AVM_SIGN);
}

void HomeView::OnStartAvm_View(int nView)
{
	if(0 == nView)
	{
		((IVIView*)GetViewBase())->SetWindow_BK(UIID_WNDID_AVM,243);
	}
	char view_cmd[128] = {0};
	char shell_path[] = {"/application/bin/avm_view.sh"};
	sprintf(view_cmd,"%s -a %d &",shell_path,nView);
	LOGDBG("&&&---view_cmd:%s",view_cmd);
	system(view_cmd);
}

void HomeView::OnStart_AVM()
{
	system("/application/bin/start_avm.sh &");
}

void HomeView::Set_AvmViewID(int nViewID)
{
	m_nCurAVM_View = nViewID;
}

/*void HomeView::DrawMusicName()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	
	HomeData &mData = HomeData::GetInstance();
	T_STR_BT_MUSICINFO tBTMusic;
	mData.GetBTMusicInfo(tBTMusic);

	if(strlen(tBTMusic.abMusicTitle) <= 0)
	{
		LOGDBG("&&&&----zxw----tBTMusic.abMusicTitle---len:0\n");
		return;
	}

	char szTemp[2*1024] = {0};
	size_t nLen = 1024;
	ConverDecode(tBTMusic.abMusicTitle,szTemp,nLen);

	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MAIN, UIID_CONTENT_HOME_BTMUSI_SONGNAME_BIN, (wchar_t*)szTemp);//歌名
}*/

void HomeView::ConverDecode(char *pInBuf,char *pOutBuf,size_t& nOutLen)
{
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
}

