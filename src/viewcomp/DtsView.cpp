/*
 * RadioView.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include "DtsView.h"
#include "UIID_Def_Public.h"
#include "UIID_Def_Dts.h"
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


DtsView::DtsView(ViewBase* p_ViewBase)
: CompBase(p_ViewBase)
, m_pViewController(NULL)
{
	// TODO Auto-generated constructor stub
	InitUpdateMsgMap();
}

DtsView::~DtsView()
{
	// TODO Auto-generated destructor stub
}

void DtsView::InitUpdateMsgMap()
{
	m_MsgMap.clear();	
	m_MsgMap.insert(make_pair(SYSTEM_VMSG_DTSEQ_SETTING, &DtsView::UpdateDtsInfo));	
}

void DtsView::GetUpdateMessageMap(map<int, pUpdateDtsMsg> &nMap)
{
	nMap = m_MsgMap;
}

bool DtsView::DtsOnUserMessage(T_APP_MSG *msg)
{
	LOGDBG("Message = %d,param0=%d,param1=%d,param2=%d\n",
		   msg->Message, msg->Param0, msg->Param1, msg->Param2);
	map<int, pUpdateDtsMsg> nUpdateMap;
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

void DtsView::DtsInitWorkWnd()
{
	if(NULL != m_pViewController)
	{
		m_pViewController->Dts_QueEQInfo();
	}
}

void DtsView::DtsOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	if(1 != nBtState)
	{
		return;
	}

	HomeData &mData = HomeData::GetInstance();
	T_DTS_EQ_SETTING tDtsSetting;
	mData.GetDts_EQSetting(tDtsSetting);

	switch(nCtrlId)
	{
		case UIID_DTS_OPEN:
		{
			if(0 != tDtsSetting.nEq_mode)
			{
				OnClick_EQMode(DTS_EQ_CLOSE);
			}
			else
			{
				if(0 == tDtsSetting.nEq_mode)
				{
					OnClick_EQMode(DTS_EQ_CLEAR);
				}
				else
				{
					OnClick_EQMode(tDtsSetting.nEq_mode);
				}
			}
		}
		break;
		case UIID_DTS_LISTEN_DRIVER:
		{
			if(NULL != m_pViewController)
			{
				m_pViewController->Dts_SetEQListen_Pos(1);
			}
		}
		break;
		case UIID_DTS_LISTEN_ALL:
		{
			if(NULL != m_pViewController)
			{
				m_pViewController->Dts_SetEQListen_Pos(0);
			}
		}
		break;
		case UIID_DTS_RELAX:
		{
			if(NULL != m_pViewController)
			{
				m_pViewController->Dts_SetEQMode(DTS_EQ_RELAXING);
			}
		}
		break;
		case UIID_DTS_NATURAL:
		{
			if(NULL != m_pViewController)
			{
				m_pViewController->Dts_SetEQMode(DTS_EQ_NATURAL);
			}
		}
		break;
		case UIID_DTS_VOCAL:
		{
			if(NULL != m_pViewController)
			{
				m_pViewController->Dts_SetEQMode(DTS_EQ_CLEAR);
			}
		}
		break;
		case UIID_DTS_BASS:
		{
			if(NULL != m_pViewController)
			{
				m_pViewController->Dts_SetEQMode(DTS_EQ_BASS);
			}
		}
		break;
		case UIID_DTS_SURROUND_SUB:
		{
			OnClick_Surroud_Level(0,tDtsSetting.nSurround_Level);
		}
		break;
		case UIID_DTS_SURROUND_ADD:
		{
			OnClick_Surroud_Level(1,tDtsSetting.nSurround_Level);
		}
		break;
		case UIID_DTS_DEEP_SUB:
		{
			OnClick_Deep_Level(0,tDtsSetting.nDeep_Level);
		}
		break;
		case UIID_DTS_DEEP_ADD:
		{
			OnClick_Deep_Level(1,tDtsSetting.nDeep_Level);
		}
		break;
		case UIID_DTS_EVELATES_SUB:
		{
			OnClick_Evelates_Level(0,tDtsSetting.nEvelate_Level);
		}
		break;
		case UIID_DTS_EVELATES_ADD:
		{
			OnClick_Evelates_Level(1,tDtsSetting.nEvelate_Level);
		}
		break;
		case UIID_DTS_UI_RETURN:
		case UIID_DTS_UI_CLOSE:
		{
			GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
		}
		break;
		default:
		{
			break;
		}
	}
}
void DtsView::OnClick_Surroud_Level(int nOption,int nLevel)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	if(1 == nOption)
	{
		//add
		nLevel++;
		if(nLevel > 6)
		{
			nLevel = 6;
		}
	}
	else
	{
		//sub
		nLevel--;
		if(nLevel < 0)
		{
			nLevel = 0;
		}		
	}

	wchar_t wszBuf[TEXT_BUF_LEN] = {0};
	swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nLevel);
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_DTS, UIID_DTS_SURROUND_TEXT, wszBuf);

	if(NULL != m_pViewController)
	{
		m_pViewController->Dts_SetEQSurround_Level(nLevel);
	}
}

void DtsView::OnClick_Deep_Level(int nOption,int nLevel)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	if(1 == nOption)
	{
		//add
		nLevel++;
		if(nLevel > 6)
		{
			nLevel = 6;
		}
	}
	else
	{
		//sub
		nLevel--;
		if(nLevel < 0)
		{
			nLevel = 0;
		}		
	}

	wchar_t wszBuf[TEXT_BUF_LEN] = {0};
	swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nLevel);
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_DTS, UIID_DTS_DEEP_TEXT, wszBuf);

	if(NULL != m_pViewController)
	{
		m_pViewController->Dts_SetEQBass_Level(nLevel);
	}
}

void DtsView::OnClick_Evelates_Level(int nOption,int nLevel)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	if(1 == nOption)
	{
		//add
		nLevel++;
		if(nLevel > 6)
		{
			nLevel = 6;
		}
	}
	else
	{
		//sub
		nLevel--;
		if(nLevel < 0)
		{
			nLevel = 0;
		}		
	}

	wchar_t wszBuf[TEXT_BUF_LEN] = {0};
	swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nLevel);
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_DTS, UIID_DTS_EVELATES_TEXT, wszBuf);

	if(NULL != m_pViewController)
	{
		m_pViewController->Dts_SetEvelates_Level(nLevel);
	}
}

void DtsView::OnClick_EQMode(int nMode)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	/*
	switch(nMode)
	{
		case DTS_EQ_CLOSE:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,131,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,131,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_OPEN,ITEM_STATE_NORMAL);

			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_NATURAL,ITEM_STATE_DISABLE);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_DISABLE);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_BASS,ITEM_STATE_DISABLE);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_DISABLE);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,ITEM_STATE_DISABLE);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,ITEM_STATE_DISABLE);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_SUB,ITEM_STATE_DISABLE);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_ADD,ITEM_STATE_DISABLE);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_SUB,ITEM_STATE_DISABLE);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_ADD,ITEM_STATE_DISABLE);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_SUB,ITEM_STATE_DISABLE);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_ADD,ITEM_STATE_DISABLE);
		}
		break;
		case DTS_EQ_NATURAL:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_OPEN,ITEM_STATE_SELECTED);

			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_NATURAL,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_BASS,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_ADD,ITEM_STATE_NORMAL);
		}
		break;
		case DTS_EQ_CLEAR:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_OPEN,ITEM_STATE_SELECTED);

			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_NATURAL,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_BASS,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_ADD,ITEM_STATE_NORMAL);

		}
		break;
		case DTS_EQ_BASS:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_OPEN,ITEM_STATE_SELECTED);

			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_NATURAL,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_BASS,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_ADD,ITEM_STATE_NORMAL);
		}
		break;
		case DTS_EQ_RELAXING:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_OPEN,ITEM_STATE_SELECTED);

			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_NATURAL,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_BASS,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_SELECTED);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_ADD,ITEM_STATE_NORMAL);
		}
		break;
		default:
		{
			break;
		}
	}*/

	if(NULL != m_pViewController)
	{
		m_pViewController->Dts_SetEQMode(nMode);
	}
}


void DtsView::DtsOnComponent(CUIWndMng *pUI, PWINDOW_INFO pInfo)
{
	// LOGDBG("pInfo.nInfoType=%d,nWindowID=%d,nCtrlID=%d ,status=%d\n",\
	// 		pInfo->nInfoType, pInfo->nWindowID, pInfo->nCtrlID, pInfo->nOperationStatus);
	LOGDBG("RadioOnComponent");
	//int nWndID = pInfo->nWindowID;
	//int nCtrlID = pInfo->nCtrlID;
	//int nStatus = pInfo->nOperationStatus;
	//int nWndType = pInfo->nInfoType;
}

void DtsView::OnTimer(uint32_t id)
{
	//	LOGDBG("HomeView pthread_ID = %d",(int)pthread_self());
	//	LOGDBG("HomeView::OnTimer id = %d", id);
}


void DtsView::DtsOnShow(LONG dwWndId)
{
	LOGDBG("OnShow Wnd ID Is %d", dwWndId);
}

bool DtsView::UpdateDtsInfo(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}
	HomeData &mData = HomeData::GetInstance();

	T_DTS_EQ_SETTING tDtsSetting;
	mData.GetDts_EQSetting(tDtsSetting);

	switch(tDtsSetting.nEq_mode)
	{
		case DTS_EQ_CLOSE:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,131,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,131,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_OPEN,ITEM_STATE_NORMAL);

			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_NATURAL,ITEM_STATE_DISABLE);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_DISABLE);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_BASS,ITEM_STATE_DISABLE);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_VOCAL,ITEM_STATE_DISABLE);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,ITEM_STATE_DISABLE);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,ITEM_STATE_DISABLE);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_SUB,ITEM_STATE_DISABLE);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_ADD,ITEM_STATE_DISABLE);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_SUB,ITEM_STATE_DISABLE);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_ADD,ITEM_STATE_DISABLE);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_SUB,ITEM_STATE_DISABLE);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_ADD,ITEM_STATE_DISABLE);
		}
		break;
		case DTS_EQ_NATURAL:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_OPEN,ITEM_STATE_SELECTED);

			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_NATURAL,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_BASS,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_VOCAL,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_ADD,ITEM_STATE_NORMAL);
		}
		break;
		case DTS_EQ_CLEAR:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_OPEN,ITEM_STATE_SELECTED);

			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_NATURAL,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_VOCAL,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_BASS,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_ADD,ITEM_STATE_NORMAL);
		}
		break;
		case DTS_EQ_BASS:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_OPEN,ITEM_STATE_SELECTED);

			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_NATURAL,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_BASS,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_VOCAL,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_ADD,ITEM_STATE_NORMAL);
		}
		break;
		case DTS_EQ_RELAXING:
		{
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_OPEN,132,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_OPEN,ITEM_STATE_SELECTED);

			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_NATURAL,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_VOCAL,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_BASS,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_RELAX,ITEM_STATE_SELECTED);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,ITEM_STATE_NORMAL);

			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_SURROUND_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_DEEP_ADD,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_SUB,ITEM_STATE_NORMAL);
			//pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_EVELATES_ADD,ITEM_STATE_NORMAL);
		}
		break;
		default:
		{
			break;
		}
	}

	if(0 == tDtsSetting.nListen_Pos)
	{
		//all
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,739,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,740,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,ITEM_STATE_SELECTED);

	}
	else
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_LISTEN_ALL,739,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,740,ITEM_STATE_NORMAL);

		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_DTS,UIID_DTS_LISTEN_DRIVER,ITEM_STATE_SELECTED);
	}

	wchar_t wszBuf[TEXT_BUF_LEN] = {0};
	swprintf(wszBuf, TEXT_BUF_LEN, L"%d", tDtsSetting.nSurround_Level);
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_DTS, UIID_DTS_SURROUND_TEXT, wszBuf);

	memset(wszBuf,0,sizeof(wszBuf));
	swprintf(wszBuf, TEXT_BUF_LEN, L"%d", tDtsSetting.nDeep_Level);
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_DTS, UIID_DTS_DEEP_TEXT, wszBuf);

	memset(wszBuf,0,sizeof(wszBuf));
	swprintf(wszBuf, TEXT_BUF_LEN, L"%d", tDtsSetting.nEvelate_Level);
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_DTS, UIID_DTS_EVELATES_TEXT, wszBuf);
	
	return true;
}



