/*
 * IVIView.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include "IVIView.h"
#include "SPDisplay.h"
#include "UIID_Def_Home.h"
#include "UIID_Def_Setting.h"
#include "UIID_Def_Radio.h"
#include "UIID_Def_BT.h"
#include "AssertDefine.h"
#include <stdio.h>

// Dependent on external header files
#include <math.h>

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
using namespace dispctrl;

int g_nPicArry[11] = {50,51,52,53,54,55,56,57,58,59,60};

IVIView::IVIView()
: m_ViewController(NULL)
, m_nCurrWnd(0)
, m_nLastWnd(1)
, m_HomeView(this)
, m_SettingView(this)
, m_RadioView(this)
, m_MediaView(this)
, m_BTPhoneView(this)
, m_EqView(this)
, m_AirConditionar(this)
, m_DtsView(this)
, m_SplitView(this)
, m_DownpointX(0)
, m_DownpointY(0)
, m_Uppoint(0)
, pCanBoxProto(NULL)
{
	// TODO Auto-generated constructor stub
	m_charKerbordMap = {
			{UIID_SCREEN_UNLOCK_KEY_NU0_BTN,"0"},
			{UIID_SCREEN_UNLOCK_KEY_Q_BTN,"Q"},	{UIID_SCREEN_UNLOCK_KEY_W_BTN,"W"},	{UIID_SCREEN_UNLOCK_KEY_E_BTN,"E"},
			{UIID_SCREEN_UNLOCK_KEY_R_BTN,"R"},	{UIID_SCREEN_UNLOCK_KEY_T_BTN,"T"},	{UIID_SCREEN_UNLOCK_KEY_Y_BTN,"Y"},
			{UIID_SCREEN_UNLOCK_KEY_U_BTN,"U"},	{UIID_SCREEN_UNLOCK_KEY_I_BTN,"I"},	{UIID_SCREEN_UNLOCK_KEY_O_BTN,"O"},
			{UIID_SCREEN_UNLOCK_KEY_P_BTN,"P"},	{UIID_SCREEN_UNLOCK_KEY_A_BTN,"A"},	{UIID_SCREEN_UNLOCK_KEY_S_BTN,"S"},
			{UIID_SCREEN_UNLOCK_KEY_D_BTN,"D"},	{UIID_SCREEN_UNLOCK_KEY_F_BTN,"F"},	{UIID_SCREEN_UNLOCK_KEY_G_BTN,"G"},
			{UIID_SCREEN_UNLOCK_KEY_H_BTN,"H"},	{UIID_SCREEN_UNLOCK_KEY_J_BTN,"J"},	{UIID_SCREEN_UNLOCK_KEY_K_BTN,"K"},
			{UIID_SCREEN_UNLOCK_KEY_L_BTN,"L"},	{UIID_SCREEN_UNLOCK_KEY_Z_BTN,"Z"},	{UIID_SCREEN_UNLOCK_KEY_X_BTN,"X"},
			{UIID_SCREEN_UNLOCK_KEY_C_BTN,"C"},	{UIID_SCREEN_UNLOCK_KEY_V_BTN,"V"},	{UIID_SCREEN_UNLOCK_KEY_B_BTN,"B"},
			{UIID_SCREEN_UNLOCK_KEY_N_BTN,"N"},	{UIID_SCREEN_UNLOCK_KEY_M_BTN,"M"}
	};

	m_nCurSource = IVI_SOURCE_IDLE;
	m_nCurPicIndex = 0;
}

IVIView::~IVIView()
{
	// TODO Auto-generated destructor stub
}

bool IVIView::Init(int argc, char **argv, std::string path, std::string appName,
					std::string resourceName, std::string strScreenID,
					std::string strFontPath, int32_t nWinWidth, int32_t nWinHeight,
					DWORD nGuiRenderMode)
{
	bool bRet = true;
	
	//	LOGDBG("path is %s",path.c_str());
	//	LOGDBG("appName is %s",appName.c_str());
	//	LOGDBG("resourceName is %s",resourceName.c_str());
	//	LOGDBG("strScreenID is %s",strScreenID.c_str());
	//	LOGDBG("strFontPath is %s",strFontPath.c_str());
	//	LOGDBG("nGuiRenderMode is %d",nGuiRenderMode);
	bRet = ViewBase::Init(argc, argv, path, appName, resourceName, strScreenID, strFontPath,
						  nWinWidth, nWinHeight, nGuiRenderMode);

	using FGE::FYCAR::CUIWndMgrWrapper;
	CUIWndMgrWrapper *pUIMgr = GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}

	DWORD dwRenderingMode = CYCLE_RENDERING_MODE;
	pUIMgr->SetRuntimeParameter(UI_RENDERING_MODE, (void*)&dwRenderingMode);        

	DWORD dwFps = 1000;
	pUIMgr->SetRuntimeParameter(CYCLE_RENDERING_FPS, (void*)&dwFps);
	

	CanBoxOnInit();

	return bRet;
}

//==============================================

typedef CanDataBase* (*OnInitFunc)(PublicController*, ViewBase*);
typedef void (*OnUninitFunc)(CanDataBase*);

// class SettingView;
void* lib_handle = nullptr;
OnInitFunc CanBoxProtoOnInit = nullptr;

// 调用CanBoxProto初始化
int IVIView::CanBoxOnInit() 
{
    printf("IVIView::CanBoxOnInit start1. so_name[%s] \n",m_SettingView.m_sCBProto_Cur.so_name);
    
    //  加载动态库
    if (lib_handle != nullptr) {
        printf("动态库已加载，无需重复加载.\n");
        return 0;
    }

    char strP[256];
	snprintf(strP, sizeof(strP), "/application/lib/%s", m_SettingView.m_sCBProto_Cur.so_name);
	lib_handle = dlopen(strP, RTLD_LAZY);
    // lib_handle = dlopen(m_SettingView.m_sCBProto_Cur.so_name, RTLD_LAZY);
    // lib_handle = dlopen("./hw_ftms_typ001.so", RTLD_LAZY);
    if (lib_handle == nullptr) 
    {
        printf("动态库加载失败：loding CanBoxProtoLib fail ! 详情：%s\n", dlerror());
        return -1;
    }
    else
    {
        printf("动态库加载成功：loding CanBoxProtoLib success !\n");
    }

    // 获取符号
    dlerror(); // 清空之前的错误状态
    CanBoxProtoOnInit = (OnInitFunc)dlsym(lib_handle, "OnInit");
    char* dlsym_err = dlerror();
    if (CanBoxProtoOnInit == nullptr || dlsym_err != nullptr) 
    {
        printf("获取符号失败：get CanBoxProto OnInit fail ! 详情：%s\n", dlsym_err);
        // dlclose(lib_handle);
        lib_handle = nullptr; // 重置句柄，避免无效指针
        return -1;
    }

    // 3. 调用动态库函数（这行代码本身，类型匹配则无错）
    pCanBoxProto = CanBoxProtoOnInit(m_ViewController, this);
	printf("CanBoxOnInit m_ViewController %p\n",m_ViewController);
	printf("CanBoxOnInit m_View %p\n",this);
    if (pCanBoxProto == nullptr) {
        printf("初始化失败 OnInit CanBoxProto OnInit fail \n");
        lib_handle = nullptr; // 重置句柄
        return -1;
    }


    return 0;
}
//==============================================



void IVIView::ShowFirstWnd(std::string strParam)
{
	if (access("/media/flash/userdata/speaker_mode", F_OK) == 0)
	{
		ShowWnd(UIID_WNDID_NETWORK_MODE, true);
	} 
	else
	{
		ShowWnd(UIID_WNDID_MAIN, true);
	}
}

bool IVIView::OnMouseMove(POINT point)
{
	return ViewBase::OnMouseMove(point);
}

bool IVIView::OnLButtonDown(POINT point)
{
	m_DownpointX = point.x;
	m_DownpointY = point.y;
	LOGDBG("&&&&---OnLButtonDown--point.x:%d,point.y:%d",point.x,point.y);
	return ViewBase::OnLButtonDown(point);
}

bool IVIView::OnLButtonUp(POINT point)
{
	// 分屏右面板左右滑动切页
	if (UIID_WNDID_SPLIT == GetCurrWnd() && m_DownpointX >= 1280)
	{
		int dx = point.x - m_DownpointX;
		int dy = point.y - m_DownpointY;
		int nAbsDx = dx < 0 ? -dx : dx;
		int nAbsDy = dy < 0 ? -dy : dy;
		if (nAbsDx > 50 && nAbsDx > nAbsDy)
		{
			// 左滑(dx<0)下一页，右滑(dx>0)上一页
			m_SplitView.SplitSwipe(dx < 0 ? 1 : -1);
			return true;
		}
	}
	/*
	int nCurWnd = GetCurrWnd();

	if(UIID_WNDID_AVM == nCurWnd)
	{
		if(NULL != m_ViewController)
		{
			// 原坐标系: 右上角原点(0,0), 向左X+, 向下Y+, 1024×600
			// 目标坐标系: 左上角原点(0,0), 向右X+, 向下Y+, 720×480
			int x_orig = point.x; 
			int y_orig = point.y;
			
			// X方向转换: x_orig * 720 / 1024
			int x_target = (x_orig * 720 + 512) / 1024;
		
			// Y方向转换: y_orig * 480 / 600
			int y_target = (y_orig * 480 + 300) / 600;
			
			// 边界检查
			x_target = std::max(0, std::min(x_target, 720));
			y_target = std::max(0, std::min(y_target, 480));

			LOGDBG("point.x[%d] ; point.y[%d] ---> NewPoint.x[%d] ; NewPoint.y[%d]\n",point.x,point.y,x_target,y_target);

			m_ViewController->SetSystem_AVMSetting(1,x_target,y_target);
		}
	}
	*/
	return ViewBase::OnLButtonUp(point);
}

void IVIView::OnAppsMessage(T_APP_MSG *msg)
{
	if (nullptr == msg)
	{
		return;
	}
	// LOGDBG("OnAppsMessage pthread_ID = %d",(int)pthread_self());
	// LOGDBG("HomeApp:OnAppsMessage:param0=%d\n",  msg->Param0);
	switch (msg->Param0)
	{
	case APPSMSG_TIMER:
	{
		// OnTimer(msg->Param1);
		break;
	}
	default:
	{
		break;
	}
	}
}

bool IVIView::OnUserMessage(T_APP_MSG *msg)
{
	if (nullptr == msg)
	{
		return false;
	}

	LOGDBG("Message = %d,param0=%d,param1=%d,param2=%d\n",
		   msg->Message, msg->Param0, msg->Param1, msg->Param2);

	if (UIID_WNDID_SPLIT == GetCurrWnd())
	{
		switch (msg->Message)
		{
			case METER_VMSG_UPDATE_RADIOINFO:
			case MEDIA_VMSG_PLAYSTATUS:
			case MEDIA_VMSG_PLAYTIME:
			case METER_VMSG_BT_MUSIC_INFO:
			case METER_VMSG_BT_MUSIC_TIME:
			case METER_VMSG_BT_MUSICPLAY_STATE:
			case METER_VMSG_BT_CONNECTED_STATE:
			case SYS_VMSG_USBMOUNT:
				m_SplitView.SplitOnUserMessage(msg);
				break;
			default: break;
		}
	}

	switch (msg->Message)
	{
		case METER_VMSG_ACCSTATUS:
		case CARPLAY_VMSG_CONNECTSTATUS:
		case ANDRIOD_VMSG_CONNECTSTATUS:
		{
			m_HomeView.HomeOnUserMessage(msg);
			break;
		}
		case SYS_VMSG_USBMOUNT:
		{
			SettingData &mData = SettingData::GetInstance();
			if(mData.GetDemoMode())
			{
				OnHide_DemoView();
			}
			m_SplitView.SplitSetUsbMount(msg->Param0);  //分屏不在前台也要同步挂载状态
			m_HomeView.HomeOnUserMessage(msg);
			m_MediaView.MediaOnUserMessage(msg);
		}
		break;
		case MEDIA_VMSG_PLAYSTATUS:
		case MEDIA_VMSG_PLAYTIME:
		{
			LOGDBG("&&&---param0:%d\n",msg->Param0);
			m_MediaView.MediaOnUserMessage(msg);
			break;
		}
		case METER_VMSG_BT_MUSIC_INFO:
		case METER_VMSG_BT_MUSIC_TIME:
		case METER_VMSG_BT_MUSICPLAY_STATE:
		case METER_VMSG_BT_CONNECTED_STATE:
		case METER_VMSG_BT_LOCAL_INFO:
		case METER_VMSG_BT_DISCOVER_INFO:
		case SYS_VMSG_UPDATE_BT_LIST:
		case SYS_VMSG_UPDATE_BT_NAME:
		case WIFI_VMSG_BTPAIRINGREQ:
		{
			m_BTPhoneView.BTOnUserMessage(msg);
			break;
		}
		case SYS_VMSG_UPDATE_DATETIME:
		{
			OnUpdateTitle_Time();
			m_SettingView.SettingOnUserMessage(msg);
			break;
		}
		case SYS_VMSG_UPDATE_VERSION:
		case SYSTEMSRV_CMD_BACKLIGHT_SET:
		case SYSTEMSRV_CMD_VOLUAME_SET:
		case SYS_VMSG_UPDATE_BACKLIGHT_LEVEL:
		case SYS_VMSG_UPDATE_VOLUME_VALUE:
		{
			m_SettingView.SettingOnUserMessage(msg);
			break;
		}
		case METER_VMSG_KEY_CHANGE:
		{
			//按键
			OnHandleKey(msg);
			break;
		}
		case METER_VMSG_BT_PHONE_BOOK_INFO:
		case METER_VMSG_BT_PHONE_TIME_INFO:
		case WIFI_VMSG_BT_PHONE_CALLMUTE:
		case WIFI_VMSG_BT_PHONE_CALLMODE:
		case METER_VMSG_BT_PHONE_HISTORY_INFO:
		{
			m_BTPhoneView.BTOnUserMessage(msg);
			break;
		}
		case METER_VMSG_UPDATE_RADIOINFO:
		{
			m_RadioView.RadioOnUserMessage(msg);
			break;
		}
		case CANBOX_VMSG_AIRDATA_INFO:
		case CANBOX_VMSG_CARDATA_INFO:
		case CANBOX_VMSG_AIRCANBOXPROTO_INFO:
		case CANBOX_VMSG_CARDATA_SETTING:
		{
			m_AirConditionar.AirOnUserMessage(msg);
			break;
		}
		case SYSTEM_VMSG_DTSEQ_SETTING:
		{
			m_DtsView.DtsOnUserMessage(msg);
			break;
		}
		default:
		{
			//int  CurrWnd = GetCurrWnd();

			if(msg->Message == METER_VMSG_BT_PHONE_INFO)
			{
				if(msg->Param0 > 0)
				{				
					LOGDBG("jump to bt phone page");	
					//OnShow(UIID_WNDID_BT_PHONE, 1, 0);
					//ShowWnd(UIID_WNDID_BT_PHONE, true);
					//SetCurrWnd(UIID_WNDID_BT_PHONE);
					//CurrWnd = UIID_WNDID_BT_PHONE;
					ShowWnd(UIID_WNDID_BT_PHONE, true);
				}				
			}
			int  CurrWnd = GetCurrWnd();
			
			LOGDBG("CurrWnd = %d",CurrWnd);
			switch (CurrWnd)
			{
				case UIID_WNDID_MAIN:
				{
					m_HomeView.HomeOnUserMessage(msg);
					break;
				}
				case UIID_WNDID_RADIO:
				{
					m_RadioView.RadioOnUserMessage(msg);
					break;
				}
				case UIID_WNDID_MEDIA_AUDIO_PLAY:
				{
					m_MediaView.MediaOnUserMessage(msg);
					break;
				}
				case UIID_WNDID_BT_MUSIC:
				{
					m_BTPhoneView.BTOnUserMessage(msg);
					break;
				}
				case UIID_WNDID_AUDIOBOOST:
		        {
			       m_EqView.EqOnUserMessage(msg);
                   break;
		        }
				case UIID_WNDID_BT_PHONE:
				{
					m_BTPhoneView.BTOnUserMessage(msg);
					break;
				}
				default:
				{
					m_SettingView.SettingOnUserMessage(msg);
					break;
				}
			}
			break;
		}
	}
	return true;
}

void IVIView::OnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	LOGDBG("nWndId=%d,nCtrlId=%d,nBtState=%d",nWndId,nCtrlId,nBtState);
	switch(nWndId)
	{
		case UIID_WNDID_MAIN:
		{
			m_HomeView.HomeOnClick(pUI,nWndId,nCtrlId,nBtState);
		}
		break;
		case UIID_WNDID_MEDIA_AUDIO_PLAY:
		case UIID_WNDID_MEDIA_AUDIO_LIST:
		case UIID_WNDID_MEDIA_VIDEO_PLAY:
		{
			m_MediaView.MediaOnClick(pUI,nWndId,nCtrlId,nBtState);
		}
		break;
		case UIID_WNDID_BT_MUSIC:
		case UIID_WNDID_BT_PAIRLIST:
		case UIID_WNDID_BT_PAIRREQ:
		{
			m_BTPhoneView.BTOnClick(pUI,nWndId,nCtrlId,nBtState);
		}
		break;
		case UIID_WNDID_BT_PHONE:
		{
			m_BTPhoneView.BTPhoneOnClick(pUI,nWndId,nCtrlId,nBtState);
		}
		break;
		case UIID_WNDID_RADIO:
		case UIID_WINID_TI:
		{
			m_RadioView.RadioOnClick(pUI,nWndId,nCtrlId,nBtState);
		}
		break;
		case UIID_WNDID_SETTING:
		{
			//setting
			m_SettingView.SettingOnClick(pUI,nWndId,nCtrlId,nBtState);
		}
		break;
		case UIID_WNDID_AVM:
		{
			//AVM
			if(1 == nBtState)
			{
				if(UIID_AVM_RETURN == nCtrlId)
				{
					ShowWnd(UIID_WNDID_MAIN,true);

					m_HomeView.KillTimer_AvmShow();

					m_HomeView.OnStartAvm_View(0);
				}
				/*
				else if(UIID_AVM_3D == nCtrlId)
				{
					m_HomeView.Set_AvmViewID(1);
					m_HomeView.OnStartAvm_View(1);
				}
				else if(UIID_AVM_FRONT == nCtrlId)
				{
					m_HomeView.Set_AvmViewID(2);
					m_HomeView.OnStartAvm_View(2);
				}
				else if(UIID_AVM_REAR == nCtrlId)
				{
					m_HomeView.Set_AvmViewID(3);
					m_HomeView.OnStartAvm_View(3);
				}
				else if(UIID_AVM_LEFT == nCtrlId)
				{
					m_HomeView.Set_AvmViewID(4);
					m_HomeView.OnStartAvm_View(4);
				}
				else if(UIID_AVM_RIGHT == nCtrlId)
				{
					m_HomeView.Set_AvmViewID(5);
					m_HomeView.OnStartAvm_View(5);
				}*/
			}
		}
		break;
		case UIID_WNDID_AUDIOBOOST:
		case UIID_WNDID_NETWORK_MODE:
		{
			//eq
			m_EqView.EqOnClick(pUI,nWndId,nCtrlId,nBtState);
		}
		break;
		case UIID_WNDID_AIRCOND:
		{
			//eq
			m_AirConditionar.AirOnClick(pUI,nWndId,nCtrlId,nBtState);
		}
		break;
		case UIID_WNDID_DTS:
		{
			//dts
			m_DtsView.DtsOnClick(pUI,nWndId,nCtrlId,nBtState);
		}
		break;
		case UIID_WNDID_SPLIT:
		{
			//split
			m_SplitView.SplitOnClick(pUI,nCtrlId,nBtState);
		}
		break;
		default:
		break;
	}

	//公共按钮
	if(1 == nBtState)
	{
		if(UIID_PUBLIC_AUIDO_POP_SUB == nCtrlId)
		{
			OnHandle_PopAudio(0);
		}
		else if(UIID_PUBLIC_AUIDO_POP_ADD == nCtrlId)
		{
			OnHandle_PopAudio(1);
		}
	}
}

void IVIView::OnComponent(CUIWndMng *pUI, PWINDOW_INFO pInfo)
{
	// LOGDBG("pInfo.nInfoType=%d,nWindowID=%d,nCtrlID=%d ,status=%d\n",\
	// 		pInfo->nInfoType, pInfo->nWindowID, pInfo->nCtrlID, pInfo->nOperationStatus);

}

void IVIView::OnListInfoMessage(CUIWndMng *pUI, int nWndId, int nCtrlId, PLIST_INFO pInfo)
{
	//LOGDBG("IVIView ListInfo: nWndId=%d, nCtrlId=%d \n", nWndId, nCtrlId);

}

void IVIView::OnShow(LONG dwWndId, bool bShow, int nStyle)
{
	// LOGDBG("OnShow Wnd ID Is %d", dwWndId);
	LOGDBG("OnShow Wnd ID Is %d bShow=%d", dwWndId, (int)bShow);
	LOGFILE("OnShow wnd=%d bShow=%d", dwWndId, (int)bShow);
	//EnableAlphaBlending(true);
	//m_RadioView.SendMeterInfoDataOFBreakAuto();
	SetCurrWnd(dwWndId);
	InitWorkWnd(dwWndId);
}

void IVIView::OnTimer(uint32_t id)
{
	LOGDBG("jgwen--- OnTimer id :%d", id);
	switch (id)
	{
		case TIMERID_AUDIO_POP_HIDE:
		{
			KillTimer(TIMERID_AUDIO_POP_HIDE);
			OnHideAudio_Pop();
		}
		break;
		case TIMERID_DEMO_SET_PIC:
		{
			OnTimer_DemoView();
		}
		break;
		case TIMERID_DEMO_WIN_SHOW:
		{
			if((UIID_WNDID_MEDIA_VIDEO_PLAY != GetCurrWnd()) && (UIID_WNDID_BLACK != GetCurrWnd())
				&& (UIID_WNDID_AVM != GetCurrWnd()) && (UIID_WNDID_AIRCOND != GetCurrWnd()))
			{
				KillTimer(TIMERID_DEMO_WIN_SHOW);
				ShowWnd(UIID_WNDID_DEMO,true);
				SetTimer(TIMERID_DEMO_SET_PIC,3*1000);
			}
		}
		break;
		case TIMERID_SPLIT_REPAINT:
		{
			KillTimer(TIMERID_SPLIT_REPAINT);
			// 用 App 层的 GetCurrWnd 判断，它在 OnShow 里同步更新，比 GetCurWndID 可靠
			// 注意：不要在这里调 SplitInitWorkWnd()，它会 SplitPickDefaultPage 把用户停的页重置
			if (UIID_WNDID_SPLIT == GetCurrWnd())
			{
				RECT rc = {1280, 0, 1920, 720};
				GetUIWndMgrWrapper()->drawAllItem(rc);
			}
			if (m_nSplitRepaintCnt > 0)
			{
				--m_nSplitRepaintCnt;
				SetTimer(TIMERID_SPLIT_REPAINT, 300);
			}
		}
		break;
		case TIMERID_SPLIT_WATCHDOG:
		{
			KillTimer(TIMERID_SPLIT_WATCHDOG);
			// App 层不认为在分屏，说明用户已经切走，停止看护
			if (UIID_WNDID_SPLIT != GetCurrWnd())
			{
				break;
			}

			CUIWndMgrWrapper* pUI = GetUIWndMgrWrapper();
			if (NULL == pUI)
			{
				break;
			}

			long nTop = (long)pUI->GetCurWndID();
			RECT rc = {1280, 0, 1920, 720};
			if (UIID_WNDID_SPLIT != nTop)
			{
				// App 认为是分屏，GUI 最上层却不是30 → 被13等窗口盖住
				LOGDBG("split watchdog: top=%ld != 30, re-raise cnt=%d", nTop, m_nSplitFixCnt);
				if (m_nSplitFixCnt < 5)
				{
					++m_nSplitFixCnt;
					ShowWnd(UIID_WNDID_SPLIT, false);
					ShowWnd(UIID_WNDID_SPLIT, true);
				}
				pUI->drawAllItem(rc);
			}
			else
			{
				pUI->drawAllItem(rc);   // 层级正常，轻量补画一次
			}

			SetTimer(TIMERID_SPLIT_WATCHDOG, 500);
		}
		break;
		default:
		{
			break;
		}
	}
}

void IVIView::OnSplitScreenChange(int nSplit, int nAppType)
{
	int nAppStatus = APP_STATUS_NO;
	HomeData::GetInstance().GetINTData(INT_APPSTATUS, nAppStatus);
	LOGDBG("OnSplitScreenChange split=%d appType=%d appStatus=%d", nSplit, nAppType, nAppStatus);

	if (nAppType == nAppStatus)
	{
		if (1 == nSplit)
		{
			// 分屏前先把 USB 视频停掉，分屏里不显示视频
			m_MediaView.OnNeed_PauseVideo();
		}

		ShowSplitWnd(1 == nSplit);
		
		if (nSplit)
		{
			RECT rc = {1280, 0, 1920, 720};
			GetUIWndMgrWrapper()->drawAllItem(rc);
			LOGDBG("OnSplitScreenChange repaint right done");

			// 立即画之外再连补几次，避免窗口还没 map 完时这一帧被丢掉
			m_nSplitRepaintCnt = 6;
			KillTimer(TIMERID_SPLIT_REPAINT);
			SetTimer(TIMERID_SPLIT_REPAINT, 200);
		}
	}
}

void IVIView::OnHandleKey(T_APP_MSG *msg)
{
	//按键值 0:空按键 1:主菜单 2:确定 3:返回/退出 4:向左 5:向右 6:向上 7:向下 8:ivi显示仪表信息控制 9:语音 10:模式/主题切换
	int nKeyValue = msg->Param0; 

	//按键操作状态 0:只有按键值 1:按下 2:弹出 3:长按
	int nKeyStatus = msg->Param1;
	LOGDBG("lcz---keyValue = %d , keyStatus = %d \n" , nKeyStatus , nKeyStatus);
	if(1 == nKeyStatus)
	{
		//按下
		switch(nKeyValue)
		{
			case 1:
			{
				//主界面
				OnHideAudio_Pop();  //隐藏音量弹窗
				ShowWnd(UIID_WNDID_SPLIT, false);
				ShowWnd(UIID_WNDID_MAIN, true);
			}
			break;
			case 3:
			{
				//关背光
			}
			break;
			case 6:
			{
				//音量+
				OnHandle_PopAudio(1);
			}
			break;
			case 7:
			{
				//音量-
				OnHandle_PopAudio(0);
			}
			break;
			case 18:
			{
				//音量+
				OnHandle_PopAudio(1);
			}
			break;
			case 19:
			{
				//音量-
				OnHandle_PopAudio(0);
			}
			break;
			case 21: // VR 按键
			{
				// 
				// m_HomeView.HomeOnClick(nullptr, UIID_WNDID_MAIN, UIID_HOME_AVM, 1);
			}
			break;
			default:
			break;
		}
	}
}

void IVIView::OnHandle_PopAudio(int nToWard)
{
	CUIWndMgrWrapper *pUIMgr = GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	int nCurWnd = GetCurrWnd();

	SettingData &mData = SettingData::GetInstance();
	int nMedioVolume = 0;
	int nBtVolume = 0;
	mData.GetINTData(INT_VOLUME_LEVEL, nMedioVolume);
	mData.GetINTData(INT_BT_VOLUME_LEVEL, nBtVolume);

	if(NULL == m_ViewController)
	{
		return;
	}

	vector<string> vecSourceName;
	bool bAudio_Call = false;
	bool bAudio_Radio = false;

	m_ViewController->getActiveTracksName(vecSourceName);

	LOGDBG("&&&&&----souce-size:%d",vecSourceName.size());

	for(int i = 0; i < vecSourceName.size();i++)
	{
		LOGDBG("&&&----sourcename:%s",vecSourceName[i].c_str());
		if((0 == strcmp(vecSourceName[i].c_str(),"S+BtPcmPhone")) 
		    || (0 == strcmp(vecSourceName[i].c_str(),"S+CarPlayTelephony")))
		{
			bAudio_Call = true;
			break;
		}
		else if(0 == strcmp(vecSourceName[i].c_str(),"S+AuxIn"))
		{
			bAudio_Radio = true;
		}
	}

	if(bAudio_Call)
	{
		LOGDBG("&&&&----bt-audio-----");
		//bt通话中
		if(0 == nToWard)
		{
			//音量-
			nBtVolume--;
			if(nBtVolume < 0)
			{
				nBtVolume = 0;
			}
		}
		else
		{
			//音量+
			nBtVolume++;
			if(nBtVolume > 40)
			{
				nBtVolume = 40;
			}
		}

		m_ViewController->SetBtVolume(nBtVolume);
		
		mData.SaveINTData(INT_BT_VOLUME_LEVEL, nBtVolume);

		//更新UI
		wchar_t wszBuf[TEXT_BUF_LEN] = {0};
		swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nBtVolume);
		pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_AUIDO_POP_VALUE, wszBuf); 
		pUIAnalysis->SetProgressItemValues(nCurWnd, UIID_PUBLIC_AUIDO_POP_BAR, 39, nBtVolume-1);
	}
	else
	{
		//主音量
		if(0 == nToWard)
		{
			//音量-
			nMedioVolume--;
			if(nMedioVolume < 0)
			{
				nMedioVolume = 0;
			}
		}
		else
		{
			//音量+
			nMedioVolume++;
			if(nMedioVolume > 40)
			{
				nMedioVolume = 40;
			}
		}

		if(bAudio_Radio)
		{
			m_ViewController->SetMedioVolume(nMedioVolume,1);
		}
		else
		{
			m_ViewController->SetMedioVolume(nMedioVolume,0);
		}

		mData.SaveINTData(INT_VOLUME_LEVEL, nMedioVolume);

		//更新UI
		wchar_t wszBuf[TEXT_BUF_LEN] = {0};
		swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nMedioVolume);
		pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_AUIDO_POP_VALUE, wszBuf); 
		pUIAnalysis->SetProgressItemValues(nCurWnd, UIID_PUBLIC_AUIDO_POP_BAR, 39, nMedioVolume-1);
	}

	/*switch(nCurWnd)
	{
		case UIID_WNDID_BT_PHONE:
		{
			if(0 == nToWard)
			{
				//音量-
				nBtVolume--;
				if(nBtVolume < 0)
				{
					nBtVolume = 0;
				}
			}
			else
			{
				//音量+
				nBtVolume++;
				if(nBtVolume > 40)
				{
					nBtVolume = 40;
				}
			}

			if(NULL != m_ViewController)
			{
				m_ViewController->SetBtVolume(nBtVolume);
			}
			mData.SaveINTData(INT_BT_VOLUME_LEVEL, nBtVolume);

			//更新UI
			wchar_t wszBuf[TEXT_BUF_LEN] = {0};
			swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nBtVolume);
			pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_AUIDO_POP_VALUE, wszBuf); 
			pUIAnalysis->SetProgressItemValues(nCurWnd, UIID_PUBLIC_AUIDO_POP_BAR, 39, nBtVolume-1);
		}
		break;
		default:
		{
			//主音量
			if(0 == nToWard)
			{
				//音量-
				nMedioVolume--;
				if(nMedioVolume < 0)
				{
					nMedioVolume = 0;
				}
			}
			else
			{
				//音量+
				nMedioVolume++;
				if(nMedioVolume > 40)
				{
					nMedioVolume = 40;
				}
			}

			if(NULL != m_ViewController)
			{
				m_ViewController->SetMedioVolume(nMedioVolume);
			}

			mData.SaveINTData(INT_VOLUME_LEVEL, nMedioVolume);

			//更新UI
			wchar_t wszBuf[TEXT_BUF_LEN] = {0};
			swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nMedioVolume);
			pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_AUIDO_POP_VALUE, wszBuf); 
			pUIAnalysis->SetProgressItemValues(nCurWnd, UIID_PUBLIC_AUIDO_POP_BAR, 39, nMedioVolume-1);
		}
		break;
	}*/

	pUIAnalysis->ShowItem(nCurWnd, UIID_PUBLIC_AUIDO_POP_GROUP,true);

	KillTimer(TIMERID_AUDIO_POP_HIDE);

	SetTimer(TIMERID_AUDIO_POP_HIDE,3*1000); 
}

void IVIView::OnHideAudio_Pop()
{
	CUIWndMgrWrapper *pUIMgr = GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	int nCurWnd = GetCurrWnd();

	pUIAnalysis->ShowItem(nCurWnd, UIID_PUBLIC_AUIDO_POP_GROUP,false);	
}

void IVIView::OnUpdateTitle_Time()
{
	CUIWndMgrWrapper *pUIMgr = GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	int nCurWnd = GetCurrWnd();

	SettingData &mData = SettingData::GetInstance();
	
	T_STR_TIME_INFO tTimeData;
	mData.GetTimeData(tTimeData);
	
	//日期
	wchar_t szTipMonth[MAX_FILE_LEN] = {0};
	swprintf(szTipMonth,MAX_FILE_LEN,L"%d/%d",tTimeData.nMonth,tTimeData.nDay);
	pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_DATE_BIN, szTipMonth);

	//week
	int nWeek = CalculateWeekday(tTimeData.nYear,tTimeData.nMonth,tTimeData.nDay);
	switch(nWeek)
	{
		case 0:
		{
			pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_WEEK_BIN,8);
			break;
		}
		case 1:
		{
			pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_WEEK_BIN,2);
			break;
		}
		case 2:
		{
			pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_WEEK_BIN,3);
			break;
		}
		case 3:
		{
			pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_WEEK_BIN,4);
			break;
		}
		case 4:
		{
			pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_WEEK_BIN,5);
			break;
		}
		case 5:
		{
			pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_WEEK_BIN,6);
			break;
		}
		case 6:
		{
			pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_WEEK_BIN,7);
			break;
		}
		default:
		break;
	}

	//time
	wchar_t szTipTime[MAX_FILE_LEN] = {0};
	swprintf(szTipTime,MAX_FILE_LEN,L"%02d:%02d",tTimeData.nHours,tTimeData.nMinutes);
	pUIAnalysis->SetItemText(nCurWnd, UIID_PUBLIC_TIME_BIN, szTipTime);
}
bool IVIView::isLeapYear(int year) 
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int IVIView::CalculateWeekday(int year, int month, int day)
{
    // 基准：公元1年1月1日为星期一
    int totalDays = 0;
    
    // 计算完整年份的天数
    for (int i = 1; i < year; i++) 
	{
        if (isLeapYear(i)) 
		{
            totalDays += 366;
        } 
		else 
		{
            totalDays += 365;
        }
    }
    
    // 计算完整月份的天数
    int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // 闰年2月有29天
    if (isLeapYear(year)) 
	{
        monthDays[1] = 29;
    }
    
    for (int i = 0; i < month - 1; i++) 
	{
        totalDays += monthDays[i];
    }
    
    // 加上当前月的天数
    totalDays += day - 1;
    
    // 公元1年1月1日是星期一，所以+1
    // 结果0=星期日，1=星期一，依此类推
    return (totalDays + 1) % 7;
}

void IVIView::SetCurrWnd(int nWndID)
{
	Lock();
	m_nCurrWnd = nWndID;
	HomeData &mData = HomeData::GetInstance();
	mData.SetCurrWnd(nWndID);
	UnLock();
}

int IVIView::GetCurrWnd()
{
	int nWndID  = 0;
	Lock();
	nWndID = m_nCurrWnd;
	UnLock();
	return nWndID;
}

void IVIView::DrawAllItem()
{
	CUIWndMgrWrapper *pUIMgr = GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	RECT rect;
	// SetRect(&rect, 0, 0, 1024, 600);
	SetRect(&rect, 0, 0, 1920, 720);
	pUIMgr->drawAllItem(rect);
}

void IVIView::InitWorkWnd(int dwWndId)
{
	SettingData &mData = SettingData::GetInstance();
	HomeData &HomemData = HomeData::GetInstance();
	RadioData &RadiomData = RadioData::GetInstance();

	//更新时间
	OnUpdateTitle_Time();

	switch (dwWndId)
	{
		case UIID_WNDID_MAIN:
		{
			m_HomeView.HomeInitWorkWnd();
			break;
		}
		case UIID_WNDID_RADIO:
		case UIID_WINID_TI:
		{
			m_RadioView.RadioInitWorkWnd();
			break;
		}
		case UIID_WNDID_MEDIA_AUDIO_PLAY:
		case UIID_WNDID_MEDIA_AUDIO_LIST:
		case UIID_WNDID_MEDIA_VIDEO_PLAY:
		case UIID_WNDID_MEDIA_FAIL:
		{
			m_MediaView.MediaInitWorkWnd(dwWndId);
			break;
		}
		case UIID_WNDID_SETTING:
		{
			m_SettingView.SettingInitWorkWnd();
			break;
		}
		case UIID_WNDID_BT_MUSIC:
		{
			m_BTPhoneView.BTInitMusicWorkWnd();
			break;
		}
		case UIID_WNDID_BT_PHONE:
		{
			m_BTPhoneView.BTInitPhoneWorkWnd();
			break;
		}
		case UIID_WNDID_AUDIOBOOST:
		{
			m_EqView.EqInitWorkWnd();
			break;
		}
		case UIID_WNDID_AIRCOND:
		{
			m_AirConditionar.AirInitWorkWnd();
			break;
		}
		case UIID_WNDID_DTS:
		{
			m_DtsView.DtsInitWorkWnd();
			break;
		}
		case UIID_WNDID_SPLIT:
		{
			m_SplitView.SplitInitWorkWnd();
			break;
		}
		default:
		{
			break;
		}
	}
}

void IVIView::OnIVI_WinShow(int nSource)
{
	switch(nSource)
	{
		case IVI_SOURCE_MEDIA:
		{
			m_MediaView.OnMedia_WinShow();
		}
		break;
		case IVI_SOURCE_BT_MUSIC:
		{
			m_BTPhoneView.OnBtMusic_WinShow();
		}
		break;
		case IVI_SOURCE_BT_PHONE:
		{
			m_BTPhoneView.OnBtCall_WinShow();
		}
		break;
		default:
		break;
	}
}

void IVIView::SetLastWnd(int nWndID)
{
	Lock();
	m_nLastWnd = nWndID;
	HomeData &mData = HomeData::GetInstance();
	mData.SetLastWnd(nWndID);
	UnLock();
}

int IVIView::GetLastWnd()
{
	int nWndID  = 0;
	Lock();
	HomeData &mData = HomeData::GetInstance();
	nWndID = mData.GetLastWnd();
	UnLock();
	return nWndID;
}

void IVIView::SendResourceType(const int nType)
{
	if(m_ViewController != NULL)
	{
		m_ViewController->SendResourceType(nType);
	}
}

void IVIView::SaveIn_SettingWnd(int nWnd)
{
	m_SettingView.SaveCurInWnd(nWnd);
}

void IVIView::SendMedia_Pause()
{
	m_MediaView.OnNeed_Pause();
}

void IVIView::SetVideoLayerVisible(bool bVisible)
{
	SPDispControl *pDisp = SPDispControl::GetInstance();
	if (NULL == pDisp)
	{
		LOGDBG("SetVideoLayerVisible: pDisp is NULL");
		return;
	}
	pDisp->DispSrv_SetAppLayerVisible("fileplay", bVisible);
	LOGDBG("SetVideoLayerVisible fileplay=%d", (int)bVisible);
}

void IVIView::SendBtAudio_Pause()
{
	m_BTPhoneView.OnNeed_Pause();
}

// 分屏/全屏切换统一入口
void IVIView::ShowSplitWnd(bool bSplit)
{
	CUIWndMgrWrapper* pUI = GetUIWndMgrWrapper();
	long nCurWnd = (NULL != pUI) ? (long)pUI->GetCurWndID() : -1;
	LOGDBG("ShowSplitWnd bSplit=%d curWnd=%ld", bSplit, nCurWnd);
	LOGFILE("ShowSplitWnd bSplit=%d curWnd=%ld", bSplit, nCurWnd);
    if (bSplit)
    {
		SetVideoLayerVisible(false);
		if ((NULL != pUI) && (UIID_WNDID_SPLIT != pUI->GetCurWndID()))
		{
			ShowWnd(UIID_WNDID_SPLIT, false);
		}
		ShowWnd(UIID_WNDID_SPLIT, true);

		// 立即画一次（大多数情况够用）
		RECT rc = {1280, 0, 1920, 720};
		GetUIWndMgrWrapper()->drawAllItem(rc);

		// 再连补几次，覆盖窗口 map / 视频层 resize 的不同耗时
		m_nSplitRepaintCnt = 6;
		KillTimer(TIMERID_SPLIT_REPAINT);
		SetTimer(TIMERID_SPLIT_REPAINT, 300);

		// 启动分屏看护：如果30被13等窗口盖住，自动重新抬层
		m_nSplitFixCnt = 0;
		KillTimer(TIMERID_SPLIT_WATCHDOG);
		SetTimer(TIMERID_SPLIT_WATCHDOG, 500);
    }
    else
    {
        ShowWnd(UIID_WNDID_SPLIT, false);
        ShowWnd(UIID_WNDID_BLACK, true);
		SetVideoLayerVisible(true);
        DrawAllItem();

        // 退出分屏，停掉补画和看护
        KillTimer(TIMERID_SPLIT_REPAINT);
        KillTimer(TIMERID_SPLIT_WATCHDOG);
        m_nSplitRepaintCnt = 0;
    }
}

void IVIView::CloseSplitWnd()
{
	if (m_bClosingSplit)
	{
		LOGDBG("CloseSplitWnd reenter, skip");
		return;
	}
	m_bClosingSplit = true;

	// 先停掉补画和看护，避免收分屏过程中被定时器又抬回来
	KillTimer(TIMERID_SPLIT_REPAINT);
	KillTimer(TIMERID_SPLIT_WATCHDOG);
	m_nSplitRepaintCnt = 0;

	CUIWndMgrWrapper* pUI = GetUIWndMgrWrapper();
	if (NULL == pUI)
	{
		m_bClosingSplit = false;
		return;
	}
	long nCurWnd = (long)pUI->GetCurWndID();
	LOGDBG("CloseSplitWnd curWnd=%ld", nCurWnd);
	LOGFILE("CloseSplitWnd curWnd=%ld", nCurWnd);

	if (UIID_WNDID_SPLIT != pUI->GetCurWndID())
	{
		LOGDBG("CloseSplitWnd skip: split not on top");
		m_bClosingSplit = false;
		return;
	}

	ShowWnd(UIID_WNDID_SPLIT, false);   // 先藏分屏
	ShowWnd(UIID_WNDID_MAIN, true);     // 回主界面，顺带盖掉卡片残影
	SetVideoLayerVisible(true);
	DrawAllItem();

	m_bClosingSplit = false;
}

void IVIView::SetWindow_BK(int nWnd,int nPicId)
{
	CUIWndMgrWrapper *pUIMgr = GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}
	pUIAnalysis->SetWindowBKPic(nWnd,nPicId);

	DrawAllItem();
}

void IVIView::Save_CurSource(int nSource)
{
	m_nCurSource = nSource;
}

int IVIView::Get_CurSource()
{
	return m_nCurSource;
}

void IVIView::OnHandle_Source(int nSource)
{
	switch(nSource)
	{
		case IVI_SOURCE_RADIO:
		{
			if(NULL != m_ViewController)
			{
				m_ViewController->SendMeterInfoData(RADIO_BREAK,0,0,0);
				m_ViewController->SendMeterInfoData(RADIO_MUTE,0,0,0);
			}
		}
		break;
		case IVI_SOURCE_MEDIA:
		{
			SendMedia_Pause();
			SetWindow_BK(UIID_WNDID_BLACK,236);
			SetWindow_BK(UIID_WNDID_AVM,236);
		}
		break;
		case IVI_SOURCE_BT_MUSIC:
		{
			SendBtAudio_Pause();
		}
		break;
		case IVI_SOURCE_CPAA:
		{
			//通知AA和CP在后台
			m_ViewController->SendAndroidautoData(APP_APPSERVICE_OPENAA_TYPE,0);
			m_ViewController->SendCarPlayData(APP_APPSERVICE_OPENCARPLAY_TYPE,0);

			SetWindow_BK(UIID_WNDID_AVM,236);
		}
		break;
		case IVI_SOURCE_AVM:
		{
			//通知AVM停止输出视频流
			m_ViewController->AVM_StreamOff();
			SetWindow_BK(UIID_WNDID_BLACK,236);
		}
		break;
		default:
		break;
	}
}

void IVIView::UpdateSplitMediaView()
{
	m_SplitView.SplitRefreshMedia();
}

void IVIView::OnUsbMediaScanFinish()
{
	//扫描完成后如果用户正停在分屏 USB 页，自动起播并刷新卡片
	m_SplitView.SplitTryAutoPlayUsb();
}

void IVIView::OnTimer_DemoView()
{
	CUIWndMgrWrapper *pUIMgr = GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	m_nCurPicIndex++;
	if(m_nCurPicIndex > 10)
	{
		m_nCurPicIndex = 0;
	}

	pUIAnalysis->SetItemIcon(UIID_WNDID_DEMO,1,g_nPicArry[m_nCurPicIndex]);
}

void IVIView::OnHide_DemoView(bool bCntShow)
{
	//set time demo
	LOGDBG("&&&----currwnd:%d,---lastwnd:%d",GetCurrWnd(),GetLastWnd());
	if(UIID_WNDID_DEMO == GetCurrWnd())
	{
		ShowWnd(GetLastWnd(),true);
	}

	KillTimer(TIMERID_DEMO_SET_PIC);
	KillTimer(TIMERID_DEMO_WIN_SHOW);
	if(bCntShow)
	{
		SetTimer(TIMERID_DEMO_WIN_SHOW,10*1000);
	}
}














