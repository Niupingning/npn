/*
 * RadioView.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include "RadioView.h"
#include "UIID_Def_Radio.h"
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

const int32_t STRING_BUF_SIZE  = 1024;//256
const int32_t FM_PBAR_DELTA    = 10;
const int32_t AM_PBAR_MAX_FREQ_POS_NEXT = 214;
const int32_t AM_PBAR_MAX_FREQ_POS = 215;

#define MPU_AMDATE_FILEPATH "/media/flash/userdata/AM.txt"
#define MPU_FMDATE_FILEPATH "/media/flash/userdata/FM.txt"

const static char* ArrRadioPTYList[]=
{
	"None","News","Affairs","Info","Sport","Educate","Drama","Culture",\
	"Science","Varied","Pop M","Rock M","Easy M","Light M","Classics","Other M",\
	"Weather","Finance","Children","Social","Religion","Phone in","Travel","Leisure",\
	"Jazz","Country","Nation M","Oldies","Folk M","Document","test","Alarm"
};

RadioView::RadioView(ViewBase* p_ViewBase)
: CompBase(p_ViewBase)
, m_pRadioViewController(NULL)
, m_nCurrWnd(0)
, m_SationListSatus(0)
{
	// TODO Auto-generated constructor stub
	
	InitUpdateMsgMap();

	m_nCurTiFreq = 1620;
	m_nCurBand = FM;
	m_nCurFreq = 8750;
	m_bShowTiView = false;
}

RadioView::~RadioView()
{
	// TODO Auto-generated destructor stub
}

void RadioView::InitUpdateMsgMap()
{
	m_MsgMap.clear();	
	m_MsgMap.insert(make_pair(METER_VMSG_UPDATE_RADIOINFO, &RadioView::MCUUpdateRadioInfo));	
}

void RadioView::GetUpdateMessageMap(map<int, pUpdateRadioMsg> &nMap)
{
	nMap = m_MsgMap;
}

bool RadioView::RadioOnUserMessage(T_APP_MSG *msg)
{
	LOGDBG("Message = %d,param0=%d,param1=%d,param2=%d\n",
		   msg->Message, msg->Param0, msg->Param1, msg->Param2);
	map<int, pUpdateRadioMsg> nUpdateMap;
	nUpdateMap.clear();
	GetUpdateMessageMap(nUpdateMap);
	auto Itr = nUpdateMap.find(msg->Message);
	if (Itr != nUpdateMap.end())
	{
		(this->*(Itr->second))(msg);
	}
	else
	{}
	return true;
}

void RadioView::RadioInitWorkWnd()
{
	int nCurSource = ((IVIView*)GetViewBase())->Get_CurSource();
	//上个源暂停
	((IVIView*)GetViewBase())->OnHandle_Source(nCurSource);

	//保存为当前源
	((IVIView*)GetViewBase())->Save_CurSource(IVI_SOURCE_RADIO);
	
	if(NULL != m_pRadioViewController)
	{
		m_pRadioViewController->InitAudio();

		m_pRadioViewController->SendMeterInfoData(RADIO_MUTE,0,0,1);
	}

	int nCurWnd = ((IVIView*)GetViewBase())->GetCurrWnd();

	if(UIID_WINID_TI == nCurWnd)
	{
		//Ti
		OnHandle_TiFreq(m_nCurTiFreq);

		m_bShowTiView = true;
	}
	else
	{
		LOGDBG("&&&----m_nCurBand:%d,m_nCurFreq:%d\n",m_nCurBand,m_nCurFreq);
		//radio
		if(NULL != m_pRadioViewController)
		{
			if(m_bShowTiView)
			{
				m_bShowTiView = false;
				m_pRadioViewController->SendMeterInfoData(RADIO_TUNE_FREQ,m_nCurBand,m_nCurFreq,0,0);
			}
		}
	}
}

void RadioView::RadioOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	LOGDBG("nWndId=%d,nCtrlId=%d,nBtState=%d",nWndId,nCtrlId,nBtState);
	//static int nOldBtState = -1;
	if(UIID_WINID_TI == nWndId)
	{
		//TI
		OnClickTiView(pUI,nWndId,nCtrlId,nBtState);
	}
	else
	{
		OnClickUpdateRadioGroup(pUI,nWndId,nCtrlId,nBtState);
	}
}

void RadioView::OnClickTiView(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	switch(nCtrlId)
	{
		case UIID_TI_PREV:
		case UIID_TI_NEXT:
		{
			if(1 == nBtState)
			{
				if(1620 == m_nCurTiFreq)
				{
					m_nCurTiFreq = 1629;
				}
				else
				{
					m_nCurTiFreq = 1620;
				}
				OnHandle_TiFreq(m_nCurTiFreq);
			}
		}
		break;
		case UIID_PUBLIC_SETTINGH_BIN:
		{
			if(1 == nBtState)
			{
				int nCurWnd = ((IVIView*)GetViewBase())->GetCurrWnd();
				if(UIID_WINID_TI == nCurWnd)
				{
					((IVIView*)GetViewBase())->SaveIn_SettingWnd(UIID_WINID_TI);
				}
				else
				{
					((IVIView*)GetViewBase())->SaveIn_SettingWnd(UIID_WNDID_RADIO);
				}
				//setting
				GetViewBase()->ShowWnd(UIID_WNDID_SETTING, true);
			}
		}
		break;
		default:
		break;
	}
}

void RadioView::OnHandle_TiFreq(int nTiFreq)
{
	if(NULL != m_pRadioViewController)
	{
		m_pRadioViewController->SendMeterInfoData(RADIO_TUNE_FREQ,AM,nTiFreq,0,0);
	}

	CUIWndMgrWrapper *pUIMgr = NULL;
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	wchar_t wszBufFreq[TEXT_BUF_LEN] = {0};
	swprintf(wszBufFreq, TEXT_BUF_LEN, L"%d", m_nCurTiFreq);

	pUIAnalysis->SetItemText(UIID_WINID_TI,UIID_TI_FREQ_TEXT,wszBufFreq);
}

void RadioView::OnClickUpdateRadioGroup(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	switch(nCtrlId)
	{
		case UIID_RADIO_FMAM:
		case UIID_RADIO_FMAM_PAGE:
		{
			if(nBtState == 1)
			{
				SendMeterInfoDataOFBaneFreq();
			}
			break;
		}
		case UIID_RADIO_PREV:     //上一频点
		{
			if(nBtState == 1)
			{
				SendMeterInfoDataOFSeekUP();
			}
			break;
		}
		case UIID_RADIO_NEXT:  //下一频点
		{
			if(nBtState == 1)
			{
				SendMeterInfoDataOFSeekDown();
			}
			break;
		}
		case UIID_RADIO_SEEK_NEXT:
		{
			if((nBtState == 4) || (nBtState == 1))
			{
				m_pRadioViewController->SendMeterInfoData(RADIO_TUNING_UP,0,0,0);
			}
			break;
		}
		case UIID_RADIO_SEEK_PREV:
		{
			if((nBtState == 4) || (nBtState == 1))
			{
				m_pRadioViewController->SendMeterInfoData(RADIO_TUNING_DOWN,0,0,0);
			}			
			break;
		}
		case UIID_RADIO_BSM:
		{
			if(nBtState == 1)
			{
				SendMeterInfoDataOFAutoSeek();
			}
			break;
		}
		case UIID_RADIO_LOCAL:
		{
			if(nBtState == 1)
			{
				SendMeterInfoDataOF_LOC();
			}
			break;
		}
		case UIID_RADIO_SOURCE:   ////source
		{
			if(nBtState == 1)
			{
				GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
			}
			break;
		}
		case UIID_PUBLIC_SETTINGH_BIN:
		{
			//setting
			((IVIView*)GetViewBase())->SaveIn_SettingWnd(UIID_WNDID_RADIO);
			GetViewBase()->ShowWnd(UIID_WNDID_SETTING, true);

			break;
		}
		default:
		{
			break;
		}
	}
}

void RadioView::SendMeterInfoDataOFBaneFreq()
{
	T_RADIO_CURR_FREQ tCurrFreq; 
	RadioData &mData = RadioData::GetInstance();
	mData.GetCurrFreq(tCurrFreq);
	if(NULL != m_pRadioViewController)
	{
		m_pRadioViewController->SendMeterInfoData(RADIO_BAND_FREQ,tCurrFreq.nCurrBand,0,0);
	}
	else
	{
		LOGERR("m_pRadioViewController is NULL");
	}
}

void RadioView::SendMeterInfoDataOFSeekUP()
{
	T_RADIO_CURR_FREQ tCurrFreq; 
	RadioData &mData = RadioData::GetInstance();
	mData.GetCurrFreq(tCurrFreq);
	if(NULL != m_pRadioViewController)
	{
		m_pRadioViewController->SendMeterInfoData(RADIO_SEEK_UP,tCurrFreq.nCurrBand,0,0);
	}
	else
	{
		LOGERR("m_pRadioViewController is NULL");
	}
}

void RadioView::SendMeterInfoDataOFSeekDown()
{
	T_RADIO_CURR_FREQ tCurrFreq; 
	RadioData &mData = RadioData::GetInstance();
	mData.GetCurrFreq(tCurrFreq);
	if(NULL != m_pRadioViewController)
	{
		m_pRadioViewController->SendMeterInfoData(RADIO_SEEK_DOWN,tCurrFreq.nCurrBand,0,0);
	}
	else
	{
		LOGERR("m_pRadioViewController is NULL");
	}
}

void RadioView::SendMeterInfoDataOFMute()
{
	T_RADIO_CURR_FREQ tCurrFreq; 
	RadioData &mData = RadioData::GetInstance();
	mData.GetCurrFreq(tCurrFreq);

	if(NULL != m_pRadioViewController)
	{
		m_pRadioViewController->SendMeterInfoData(RADIO_MUTE,tCurrFreq.nCurrBand,0,tCurrFreq.nCurrMute);
	}
	else
	{
		LOGERR("m_pRadioViewController is NULL");
	}
}

void RadioView::SendMeterInfoDataOFAutoSeek()
{
	T_RADIO_CURR_FREQ tCurrFreq; 
	RadioData &mData = RadioData::GetInstance();
	mData.GetCurrFreq(tCurrFreq);
	if(NULL != m_pRadioViewController)
	{
		m_pRadioViewController->SendMeterInfoData(RADIO_AS,tCurrFreq.nCurrBand,0,0);
	}
	else
	{
		LOGERR("m_pRadioViewController is NULL");
	}
}

void RadioView::SendMeterInfoDataOFBreakAuto()
{
	T_RADIO_CURR_FREQ tCurrFreq; 
	RadioData &mData = RadioData::GetInstance();
	mData.GetCurrFreq(tCurrFreq);
	if(NULL != m_pRadioViewController)
	{
		m_pRadioViewController->SendMeterInfoData(RADIO_BREAK,tCurrFreq.nCurrBand,0,0);
	}
	else
	{
		LOGERR("m_pRadioViewController is NULL");
	}
}

void RadioView::SendMeterInfoDataOF_LOC()
{
	T_RADIO_CURR_FREQ tCurrFreq; 
	RadioData &mData = RadioData::GetInstance();
	mData.GetCurrFreq(tCurrFreq);

	int nLoc = 0;
	if(0 == tCurrFreq.nRadioFlagLOCAL)
	{
		nLoc = 1;
	}
	else if(1 == tCurrFreq.nRadioFlagLOCAL)
	{
		nLoc = 0;
	}

	if(NULL != m_pRadioViewController)
	{
		m_pRadioViewController->SendMeterInfoData(RADIO_LOC,tCurrFreq.nCurrBand,0,0,nLoc);
	}
	else
	{
		LOGERR("m_pRadioViewController is NULL");
	}

	CUIWndMgrWrapper *pUIMgr = NULL;
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	if(1 == nLoc)
	{
		//近程
		pUIAnalysis->SetItemIcon(UIID_WNDID_RADIO,UIID_RADIO_LOCAL,103,ITEM_STATE_NORMAL);

		pUIAnalysis->SetItemState(UIID_WNDID_RADIO,UIID_RADIO_LOCAL,ITEM_STATE_SELECTED);
	}
	else if(0 == nLoc)
	{
		pUIAnalysis->SetItemIcon(UIID_WNDID_RADIO,UIID_RADIO_LOCAL,101,ITEM_STATE_NORMAL);

		pUIAnalysis->SetItemState(UIID_WNDID_RADIO,UIID_RADIO_LOCAL,ITEM_STATE_NORMAL);
	}	
}

void RadioView::RadioOnComponent(CUIWndMng *pUI, PWINDOW_INFO pInfo)
{
	// LOGDBG("pInfo.nInfoType=%d,nWindowID=%d,nCtrlID=%d ,status=%d\n",\
	// 		pInfo->nInfoType, pInfo->nWindowID, pInfo->nCtrlID, pInfo->nOperationStatus);
	LOGDBG("RadioOnComponent");
	//int nWndID = pInfo->nWindowID;
	//int nCtrlID = pInfo->nCtrlID;
	//int nStatus = pInfo->nOperationStatus;
	//int nWndType = pInfo->nInfoType;
}

void RadioView::OnTimer(uint32_t id)
{
	//	LOGDBG("HomeView pthread_ID = %d",(int)pthread_self());
	//	LOGDBG("HomeView::OnTimer id = %d", id);

	switch (id)
	{
		case TIMERID_RADIO_NEXT:
		{
			m_pRadioViewController->SendMeterInfoData(RADIO_TUNING_UP,0,0,0);
			break;
		}
		case TIMERID_RADIO_LAST:
		{
			m_pRadioViewController->SendMeterInfoData(RADIO_TUNING_DOWN,0,0,0);
			break;
		}
		default:
		{
			break;
		}
	}
}

int RadioView::GetCurrWnd()
{
	HomeData &mData = HomeData::GetInstance();
	Lock();
	m_nCurrWnd = mData.GetCurrWnd();
	UnLock();
	return m_nCurrWnd;
}

void RadioView::RadioOnShow(LONG dwWndId)
{
	LOGDBG("OnShow Wnd ID Is %d", dwWndId);
}

bool RadioView::MCUUpdateRadioInfo(T_APP_MSG* msg)
{
	LOGDBG("MCUUpdateRadioInfo");
	bool bRet = false;
	RadioData &mData = RadioData::GetInstance();
	T_RADIO_CURR_FREQ tCurrFreq; 
	mData.GetCurrFreq(tCurrFreq);	

	LOGDBG("tCurrFreq.nValid = %d",tCurrFreq.nValid);
	LOGDBG("tCurrFreq.nCurrMute = %d",tCurrFreq.nCurrMute);
	LOGDBG("tCurrFreq.nRadioStatus = %d",tCurrFreq.nRadioStatus);
	LOGDBG("tCurrFreq.nWorkStatus = %d",tCurrFreq.nWorkStatus);

	
	//实时根据MCU的数据更新Radio界面更新AM/FM
	MCUUpdateRadioCurrFreq(tCurrFreq.nCurrBand,tCurrFreq.nCurrFreq,tCurrFreq.nRadioStatus);

	//立体声/单声道
	//MCUUpdateRadioStereoMono(tCurrFreq.nStereoMono);

	return bRet;
}

bool RadioView::MCUUpdateRadioCurrFreq(const int nCurrBand,const int nCurrFreq,const int nRadioStatus)
{
	bool bRet = false;
	int nCurWnd = ((IVIView*)GetViewBase())->GetCurrWnd();

	if(UIID_WINID_TI == nCurWnd)
	{
		LOGDBG("&&---UIID_WINID_TI----");
		return true;
	}

	static int nOldFreq = -1;
	static int nOldBand = -1;
	if(nCurrFreq == nOldFreq && nCurrBand == nOldBand)
	{
		LOGDBG("Freq NO Change");
		return bRet;
	}

	CUIWndMgrWrapper *pUIMgr = NULL;
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return bRet;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return bRet;
	}
	int nBandText = UIID_TEXTID_RADIO_FM1_TEXT;
	float nFMFreqValue = 0;

	int nAMFreqValue = 0;
	wchar_t wszBuf[TEXT_BUF_LEN] = {0};
	int nLen = FM_POINTER_LEN;
	int nPos = 0;

	RadioData &mData = RadioData::GetInstance();
	T_RADIO_CURR_FREQ tCurrFreq; 
	mData.GetCurrFreq(tCurrFreq);
	
	if(nCurrBand == FM)    //FM
	{
		//频段类型
		nBandText = UIID_TEXTID_RADIO_FM1_TEXT;
		//频段值
		if(FM_FREQ_MIN >= nCurrFreq)
		{
			nFMFreqValue = FM_FREQ_MIN / 100.0;
		}
		else if(FM_FREQ_MAX <= nCurrFreq)
		{
			nFMFreqValue = FM_FREQ_MAX / 100.0;
		}
		else
		{
			nFMFreqValue = nCurrFreq / 100.0;
		}
		swprintf(wszBuf, TEXT_BUF_LEN, L"%0.1f", nFMFreqValue);

		//单位
		pUIAnalysis->SetItemText(UIID_WNDID_RADIO,UIID_RADIO_UINT,110);

		//设置进度条长度 1080-875 = 205 (87.5~108)
		//nLen = FM_POINTER_LEN;
		//进度条指针位置
		//nPos = (nFMFreqValue *10) - (FM_FREQ_MIN / 10);

		//显示local
		pUIAnalysis->ShowItem(UIID_WNDID_RADIO, UIID_RADIO_LOCAL,true);

		if(1 == tCurrFreq.nRadioFlagLOCAL)
		{
			//近程
			pUIAnalysis->SetItemIcon(UIID_WNDID_RADIO,UIID_RADIO_LOCAL,103,ITEM_STATE_NORMAL);

			pUIAnalysis->SetItemState(UIID_WNDID_RADIO,UIID_RADIO_LOCAL,ITEM_STATE_SELECTED);
		}
		else if(0 == tCurrFreq.nRadioFlagLOCAL)
		{
			pUIAnalysis->SetItemIcon(UIID_WNDID_RADIO,UIID_RADIO_LOCAL,101,ITEM_STATE_NORMAL);

			pUIAnalysis->SetItemState(UIID_WNDID_RADIO,UIID_RADIO_LOCAL,ITEM_STATE_NORMAL);
		}

		pUIAnalysis->SetItemState(UIID_WNDID_RADIO,UIID_RADIO_FMAM,ITEM_STATE_NORMAL);
	}
	else if(nCurrBand == AM)   //AM
	{
		//频段类型
		nBandText = UIID_TEXTID_RADIO_AM1_TEXT;
		//频段值
		if(AM_FREQ_MIN >= nCurrFreq)
		{
			nAMFreqValue = AM_FREQ_MIN;
		}
		else if(AM_FREQ_MAX <= nCurrFreq)
		{
			nAMFreqValue = AM_FREQ_MAX;
		}
		else
		{
			nAMFreqValue = nCurrFreq;
		}
		swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nAMFreqValue);

		//设置进度条长度 (1602-531)/9 = 119 (531~1602)
		//nLen = AM_POINTER_LEN;
		//进度条指针位置
		//nPos = (nAMFreqValue - AM_FREQ_MIN) / 9;

		//单位
		pUIAnalysis->SetItemText(UIID_WNDID_RADIO,UIID_RADIO_UINT,108);

		//隐藏local
		pUIAnalysis->ShowItem(UIID_WNDID_RADIO, UIID_RADIO_LOCAL,false);

		pUIAnalysis->SetItemState(UIID_WNDID_RADIO,UIID_RADIO_FMAM,ITEM_STATE_NORMAL);
	}

	//刷新频段
	pUIAnalysis->SetItemText(UIID_WNDID_RADIO,UIID_RADIO_FMAM,nBandText);

	//刷新频点
	pUIAnalysis->SetItemText(UIID_WNDID_RADIO,UIID_RADIO_FREVALUE,wszBuf);

	//刷新BSM停止搜台
	if(0 == tCurrFreq.nRadioStatus)
	{
		pUIAnalysis->SetItemState(UIID_WNDID_RADIO,UIID_RADIO_BSM,ITEM_STATE_NORMAL);
	}

	nOldFreq = nCurrFreq;
	nOldBand = nCurrBand;

	m_nCurBand = nCurrBand;
	m_nCurFreq = nCurrFreq;

	return bRet;
}

bool RadioView::MCUUpdateRadioCurrMute(const int nCurrMute)
{
	static int OldMute = -1;
	if(OldMute == nCurrMute)
	{
		LOGDBG("MCUUpdateRadioCurrMute Same");
		return false;
	}
	CUIWndMgrWrapper *pUIMgr = NULL;
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return false;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return false;
	}
	HomeData &mHomeData = HomeData::GetInstance();
	int nResourceType = mHomeData.GetResourceType();

	//0不静音 1静音
	if(nCurrMute)
	{
		pUIAnalysis->SetItemIcon(UIID_WNDID_RADIO,UIID_CONTENT_RADIO_PLAYSTOP_BIN,150,ALL_STATE);
		pUIAnalysis->SetItemIcon(UIID_WNDID_RADIO,UIID_CONTENT_RADIO_PLAYSTOP_BIN,151,ITEM_STATE_PUSH);
	}
	else
	{
		pUIAnalysis->SetItemIcon(UIID_WNDID_RADIO,UIID_CONTENT_RADIO_PLAYSTOP_BIN,36,ALL_STATE);
		pUIAnalysis->SetItemIcon(UIID_WNDID_RADIO,UIID_CONTENT_RADIO_PLAYSTOP_BIN,245,ITEM_STATE_PUSH);
		m_pRadioViewController->SendResourceType(RESOERCE_TYPE_RD);
		m_pRadioViewController->InitAudio();
	}
	LOGDBG("nCurrMute = %d",nCurrMute);
	OldMute = nCurrMute;
	return true;
}

bool RadioView::MCUUpdateRadioStereoMono(const int nStereoMono)
{
	static int OldStereoMono= -1;
	if(OldStereoMono == nStereoMono)
	{
		return false;
	}
	CUIWndMgrWrapper *pUIMgr = NULL;
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return false;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return false;
	}

	pUIAnalysis->ShowItem(UIID_WNDID_RADIO, 219, nStereoMono);
	LOGDBG("nStereoMono = %d",nStereoMono);
	OldStereoMono = nStereoMono;
	return true;
}

void RadioView::KillRadioTimer()
{
	KillTimer(TIMERID_RADIO_NEXT);
	KillTimer(TIMERID_RADIO_LAST);
}

void RadioView::ConverDecode(char *pInBuf,char *pOutBuf,size_t& nOutLen)
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


