/*
 * AirConditionar.h
 *
 *  Created on: 2025-10-21
 *      Author: junjie.feng
 */


#include "AirConditionar.h"
#include "UIID_Def_AirCondition.h"
#include "AssertDefine.h"
#include <locale.h>

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


AirConditionar::AirConditionar(ViewBase* p_ViewBase)
	: CompBase(p_ViewBase)
	, m_nSetCurrWnd(0)
	, m_ViewController(NULL)
{
	m_nAirConditionData = &AirConditionData::GetInstance();

	memset(&m_tCanBoxData,0,sizeof(T_CANBOX_AIR_DATA));
}

AirConditionar::~AirConditionar()
{
	// TODO Auto-generated destructor stub
}


bool AirConditionar::AirOnUserMessage(T_APP_MSG *msg)
{
	if (nullptr == msg)
	{
		return false;
	}
	
	LOGDBG("jgwen--- Message = %d,param0=%d,param1=%d,param2=%d\n", \
			msg->Message, msg->Param0,msg->Param1,msg->Param2);
	if(CMD_ID_AIR_CONDITIONER == msg->Message)
	{
		OnHandle_AirShow();
	}
	if(CANBOX_VMSG_CARDATA_INFO == msg->Message)
	{
		OnHandle_CarShow();
	}
	if(CANBOX_VMSG_CARDATA_SETTING == msg->Message)
	{
		OnHandle_CarSetShow();
	}

	return true;
}

void AirConditionar::OnHandle_CarSetShow()
{
	HomeData &mData = HomeData::GetInstance();
	T_CANBOX_CAR_SETTING tmpCarSetData;
	mData.GetCanBox_CarSetting(tmpCarSetData);

	if(tmpCarSetData.turn_left != m_tCanBoxCarSetData.turn_left)
	{
		//左转向灯
		if(tmpCarSetData.turn_left)
		{
			LOGDBG("左转向灯开\n");
		}else
		{
			LOGDBG("左转向灯关\n");
		}
	}

	if(tmpCarSetData.turn_right != m_tCanBoxCarSetData.turn_right)
	{
		if(tmpCarSetData.turn_right)
		{
			LOGDBG("右转向灯开\n");
		}else
		{
			LOGDBG("右转向灯关\n");
		}
	}

	m_tCanBoxCarSetData = tmpCarSetData;
}

void AirConditionar::OnHandle_CarShow()
{
	HomeData &mData = HomeData::GetInstance();
	T_CANBOX_CAR_DATA tmpCarData;
	mData.GetCanBox_CarInfo(tmpCarData);

	if(tmpCarData.car_key != m_tCanBoxCarData.car_key)
	{
		if(0x01 == tmpCarData.car_key)
		{
			LOGDBG("音量加\n");
		}else if(0x02 == tmpCarData.car_key)
		{
			LOGDBG("音量减\n");
		}else if(0x08 == tmpCarData.car_key)
		{
			LOGDBG("上一曲\n");
		}else if(0x09 == tmpCarData.car_key)
		{
			LOGDBG("下一曲\n");
		}else
		{
			LOGDBG("按键值:0x%02X",tmpCarData.car_key);
		}
	}

	if(tmpCarData.car_park != m_tCanBoxCarData.car_park)
	{
		//泊车
		if(tmpCarData.car_park)
		{
			LOGDBG("手刹拉起:%d\n",tmpCarData.car_park);
		}else
		{
			LOGDBG("手刹释放:%d\n",tmpCarData.car_park);
		}
	}

	if(tmpCarData.car_rev != m_tCanBoxCarData.car_rev)
	{
		//倒车
		if(tmpCarData.car_rev)
		{
			LOGDBG("倒挡状态:%d\n",tmpCarData.car_rev);
		}else
		{
			LOGDBG("非倒挡状态:%d\n",tmpCarData.car_rev);
		}
	}
	//(can夹具测试)
	// if(tmpCarData.wheel_angle != m_tCanBoxCarData.wheel_angle)
	// {
	// 	//方向盘转角
	// 	int16_t wheel_angle =static_cast<int16_t>(tmpCarData.wheel_angle);
	// 	if(wheel_angle >0)
	// 	{
	// 		LOGDBG("右转：%d度\n",wheel_angle);
	// 	}else if(wheel_angle <0)
	// 	{
	// 		LOGDBG("左转:%d度\n",wheel_angle);
	// 	}else
	// 	{
	// 		LOGDBG("无转角\n");
	// 	}
	// }
	if(tmpCarData.wheel_angle != m_tCanBoxCarData.wheel_angle)
	{
		//方向盘转角
		int16_t wheel_angle =static_cast<int16_t>(tmpCarData.wheel_angle);
		if(wheel_angle >0)
		{
			LOGDBG("左转：%d度\n",-wheel_angle);
		}else if(wheel_angle <0)
		{
			LOGDBG("右转:%d度\n",-wheel_angle);
		}else
		{
			LOGDBG("无转角\n");
		}
	}

	m_tCanBoxCarData = tmpCarData;
}

/**
 * 功能： 空调信息显示
*/
void AirConditionar::OnHandle_AirShow()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	//printf("[OnHandle_AirShow] in .\n");
	bool bNeedShow = false;

	HomeData &mData = HomeData::GetInstance();
	IVIView &mView = IVIView::GetInstance();

	T_CANBOX_AIR_DATA tmpAirData;
	mData.GetCanBox_AirData(tmpAirData);

	// 空调信息
	AirConditionInfo s_AirInof;
	uint8_t airBuf[sizeof(AirConditionInfo)] = {0};
	mView.pCanBoxProto->Get_StructureData(CMD_ID_AIR_CONDITIONER,airBuf,1);
	memcpy(&s_AirInof, airBuf, sizeof(s_AirInof));

	//空调开关
	if(s_AirInof.AirSwitch != m_sCanBoxProData.AirSwitch){
		printf("s_AirInof.AirSwitch=%d\n",s_AirInof.AirSwitch);
		if(s_AirInof.AirSwitch==1)
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_POWER,ITEM_STATE_SELECTED);
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_POWER,ITEM_STATE_NORMAL);
		}
		bNeedShow = true;
	}

	//内外循环
	if(s_AirInof.InExCir != m_sCanBoxProData.InExCir)
	{
		if(s_AirInof.InExCir==1)
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_INOUT_SET,ITEM_STATE_SELECTED);
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_INOUT_SET,ITEM_STATE_NORMAL);
		}
		bNeedShow = true;
	}

	//Front window defogging 前窗除雾
	if(s_AirInof.FrontWindDefogg != m_sCanBoxProData.FrontWindDefogg)
	{
		if(s_AirInof.FrontWindDefogg)
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_FRONT_REMOVE,ITEM_STATE_SELECTED);
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_FRONT_REMOVE,ITEM_STATE_NORMAL);
		}
		bNeedShow = true;
	}

	//Rear window defogging 后窗除雾
	if(s_AirInof.RearWindDefogg != m_sCanBoxProData.RearWindDefogg)
	{	
		if(s_AirInof.RearWindDefogg==1){
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_REAR_REMOVE,ITEM_STATE_SELECTED);
		}else{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_REAR_REMOVE,ITEM_STATE_NORMAL);
		}
		bNeedShow = true;
	}
	

	int btn_status;
	//DUAL 
	if(s_AirInof.Dual != m_sCanBoxProData.Dual){
		if(s_AirInof.Dual == 1)
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_DUAL,ITEM_STATE_SELECTED);
			btn_status= pUIMgr->GetUIAnalysisObj()->GetItemState(UIID_WNDID_AIRCOND, UIID_AIR_DUAL);
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_DUAL,ITEM_STATE_NORMAL);
			btn_status= pUIMgr->GetUIAnalysisObj()->GetItemState(UIID_WNDID_AIRCOND, UIID_AIR_DUAL);
		}
		LOGDBG("[OnHandle_AirShow]: s_AirInof.Dual按钮状态%d\n",btn_status);
		bNeedShow = true;
	}

	//AC MAX
	if(s_AirInof.AC_MAX != m_sCanBoxProData.AC_MAX){
		if(s_AirInof.AC_MAX ==1 )
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_ACCMAX,ITEM_STATE_SELECTED);
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_ACCMAX,ITEM_STATE_NORMAL);
		}
			bNeedShow = true;
	}

	//AUTO
	if(s_AirInof.Auto != m_sCanBoxProData.Auto){
		if(s_AirInof.Auto ==1 )
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_AUTO,ITEM_STATE_SELECTED);
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_AUTO,ITEM_STATE_NORMAL);
		}
		bNeedShow = true;
	}

	//SYNC
	if(s_AirInof.Sync != m_sCanBoxProData.Sync)
	{
		if(s_AirInof.Sync==1)
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_SYNC,ITEM_STATE_SELECTED);
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_SYNC,ITEM_STATE_NORMAL);
		}
		bNeedShow = true;
	}

	//Max Heat
	if(s_AirInof.MaxHeat != m_sCanBoxProData.MaxHeat)
	{
		if(s_AirInof.MaxHeat == 1){
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_MAXHEAT,ITEM_STATE_SELECTED);
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_MAXHEAT,ITEM_STATE_NORMAL);
		}
		bNeedShow = true;
	}

	//Seat Heating 前左座椅加热
	if(s_AirInof.Front_L_Heat != m_sCanBoxProData.Front_L_Heat )
	{
		if(s_AirInof.Front_L_Heat >= 1 ){
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_LEFT_HOT,ITEM_STATE_SELECTED);
		}else{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_LEFT_HOT,ITEM_STATE_NORMAL);
		}
		bNeedShow = true;
	}

	//Seat Heating right 前右座椅加热
	if(s_AirInof.Front_R_Heat != m_sCanBoxProData.Front_R_Heat){
		if(s_AirInof.Front_R_Heat >= 1 )
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_RIGHT_HOT,ITEM_STATE_SELECTED);
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_RIGHT_HOT,ITEM_STATE_NORMAL);
		}
		bNeedShow = true;
	}

	//Outside temperature display 外部温度
	if(s_AirInof.ExternalTemp != m_sCanBoxProData.ExternalTemp )
	{
		float fTemp = s_AirInof.ExternalTemp / 2.0f;

		wchar_t szTempValue[MAX_FILE_LEN] = {0};

		// 正确判断是否为 .0 整数（修复 % 错误）
		if (fTemp == (int)fTemp) {
			swprintf(szTempValue, ARRY_SIZE(szTempValue), L"%d", (int)fTemp);
		} 
		else 
		{
			swprintf(szTempValue, ARRY_SIZE(szTempValue), L"%.1f", fTemp);
		}

		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AIRCOND, UIID_AIR_EXTER_TEMP, szTempValue);

		bNeedShow = true;
	}
	
	//前排右设定温度
	if(s_AirInof.FrontRowLeftTemp != m_sCanBoxProData.FrontRowLeftTemp)
	{
    	wchar_t szTempValue[MAX_FILE_LEN] = {0};
    	if(s_AirInof.FrontRowLeftTemp == 0xFE) 
    	{
        	swprintf(szTempValue,ARRY_SIZE(szTempValue),L"LOW");
    	} 
		else if(s_AirInof.FrontRowLeftTemp == 0xFF) 
    	{
        	swprintf(szTempValue,ARRY_SIZE(szTempValue),L"HIGH");
    	} 
		else 
    	{
        	float fTemp = s_AirInof.FrontRowLeftTemp/2.0f;
			if (fTemp >= 17.0f && fTemp <= 32.0f)
			{
				// 17-32 之间，显示℃
				if(s_AirInof.FrontRowLeftTemp % 2 == 0) 
				{
					swprintf(szTempValue,ARRY_SIZE(szTempValue), L"%d℃", (int)fTemp);
				}
				else 
				{
					swprintf(szTempValue,ARRY_SIZE(szTempValue), L"%.1f℃",fTemp);
				}
			}
			else if (fTemp >= 60.0f && fTemp <= 85.0f)
			{
				// 60-85 之间，显示℉
				if(s_AirInof.FrontRowLeftTemp % 2 == 0) 
				{
					swprintf(szTempValue,ARRY_SIZE(szTempValue), L"%d℉", (int)fTemp);
				}
				else 
				{
					swprintf(szTempValue,ARRY_SIZE(szTempValue), L"%.1f℉",fTemp);
				}
			}
			else
			{
				// 其他范围，不显示单位
				if(s_AirInof.FrontRowLeftTemp % 2 == 0) 
				{
					swprintf(szTempValue,ARRY_SIZE(szTempValue), L"%d", (int)fTemp);
				}
				else 
				{
					swprintf(szTempValue,ARRY_SIZE(szTempValue), L"%.1f", fTemp);
				}
			}
		}
    	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AIRCOND, UIID_AIR_RIGHT_TEMP, szTempValue);

    	bNeedShow = true;
	}

	//前排左设定温度
	if(s_AirInof.FrontRowRightTemp != m_sCanBoxProData.FrontRowRightTemp)
	{
    	wchar_t szTempValue[MAX_FILE_LEN] = {0};
    	if(s_AirInof.FrontRowRightTemp == 0xFE) 
    	{
        	swprintf(szTempValue,ARRY_SIZE(szTempValue),L"LOW");
    	} 
		else if(s_AirInof.FrontRowRightTemp == 0xFF)
		{
        	swprintf(szTempValue,ARRY_SIZE(szTempValue),L"HIGH");
    	} 
		else 
    	{
        	float fTemp = s_AirInof.FrontRowRightTemp /2.0f;
			if (fTemp >= 17.0f && fTemp <= 32.0f)
			{
				// 17-32 之间，显示℃
				if(s_AirInof.FrontRowRightTemp % 2 == 0) 
				{
					swprintf(szTempValue,ARRY_SIZE(szTempValue), L"%d℃", (int)fTemp);
				}else 
				{
					swprintf(szTempValue,ARRY_SIZE(szTempValue), L"%.1f℃",fTemp);
				}
			}
			else if (fTemp >= 60.0f && fTemp <= 85.0f)
			{
				// 60-85 之间，显示℉
				if(s_AirInof.FrontRowRightTemp % 2 == 0) 
				{
					swprintf(szTempValue,ARRY_SIZE(szTempValue), L"%d℉", (int)fTemp);
				}else 
				{
					swprintf(szTempValue,ARRY_SIZE(szTempValue), L"%.1f℉",fTemp);
				}
			}
			else
			{
				// 其他范围，不显示单位
				if(s_AirInof.FrontRowRightTemp % 2 == 0) 
				{
					swprintf(szTempValue,ARRY_SIZE(szTempValue),L"%d", (int)fTemp);
				}else 
				{
					swprintf(szTempValue,ARRY_SIZE(szTempValue), L"%.1f", fTemp);
				}
			}
		}
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AIRCOND, UIID_AIR_LEFT_TEMP, szTempValue);

		bNeedShow = true;
	}

	// 前排吹风模式 
	if(s_AirInof.FrontRowVenMode != m_sCanBoxProData.FrontRowVenMode)
	{
		if(AIR_MODE_BODY_FOOT == s_AirInof.FrontRowVenMode)
		{
			//吹身吹脚
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_WINDOW,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_FOOT,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_BODY,ITEM_STATE_SELECTED);
		}
		else if(AIR_MODE_FOOT == s_AirInof.FrontRowVenMode)
		{
			//吹脚
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_BODY,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_FOOT,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_WINDOW,ITEM_STATE_NORMAL);			
		}
		else if(AIR_MODE_BODY == s_AirInof.FrontRowVenMode)
		{
			//吹身
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_BODY,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_FOOT,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_WINDOW,ITEM_STATE_NORMAL);			
		}
		else if(AIR_MODE_WIN == s_AirInof.FrontRowVenMode)
		{
			//吹窗
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_BODY,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_FOOT,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_WINDOW,ITEM_STATE_SELECTED);	
		}
		else if(AIR_MODE_WIN_BODY == s_AirInof.FrontRowVenMode)
		{
			//吹身吹窗
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_BODY,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_FOOT,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_WINDOW,ITEM_STATE_SELECTED);	
		}
		else if(AIR_MODE_WIN_BODY == s_AirInof.FrontRowVenMode)
		{
			//吹脚吹窗
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_BODY,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_FOOT,ITEM_STATE_SELECTED);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_WINDOW,ITEM_STATE_SELECTED);
		}
		else if(AIR_MODE_AUTO == s_AirInof.FrontRowVenMode)
		{
			// 自动送风
		}
		else
		{   // 都没打开
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_BODY,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_FOOT,ITEM_STATE_NORMAL);
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_WINDOW,ITEM_STATE_NORMAL);	
		}

		bNeedShow = true;
	}

	// 前左座椅吹风 
	if(s_AirInof.Front_L_BlowHair != m_sCanBoxProData.Front_L_BlowHair)
	{
		if(s_AirInof.Front_L_BlowHair > 0)
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_LEFT_AIR,ITEM_STATE_SELECTED);
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_LEFT_AIR,ITEM_STATE_NORMAL);
		}

		bNeedShow = true;
	}

	//前右座椅吹风 
	if(s_AirInof.Front_R_BlowHair != m_sCanBoxProData.Front_R_BlowHair)
	{
		if(s_AirInof.Front_R_BlowHair > 0)
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_RIGHT_AIR,ITEM_STATE_SELECTED);
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_RIGHT_AIR,ITEM_STATE_NORMAL);
		}

		bNeedShow = true;
	}

	// 前排风量(风速)             0 1  2   3   4   5   6   7       
	const int AIR_SPEED_POS[] = {0,0,14, 29, 43, 57, 71, 86, 100}; 
	if(s_AirInof.FrontRowLevel != m_sCanBoxProData.FrontRowLevel)
	{
		//风速
		int pos = 0;
		// pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AIRCOND,UIID_AIR_VOLUME,pos);
		if(s_AirInof.FrontRowLevel >= 1 && s_AirInof.FrontRowLevel <= 8)
		{
			pos = AIR_SPEED_POS[s_AirInof.FrontRowLevel];
			pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AIRCOND,UIID_AIR_VOLUME,100,0);
		}else
		{
			pos = AIR_SPEED_POS[0];
		}
		LOGDBG("风速 s_AirInof.FrontRowLevel= %d， 进度条值Pos=%d \n",s_AirInof.FrontRowLevel,pos);
		pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AIRCOND,UIID_AIR_VOLUME,pos);
		pUIMgr->GetUIAnalysisObj()->drawItem(UIID_WNDID_AIRCOND, UIID_AIR_VOLUME);
		bNeedShow = true;
	}


	// ECO
	if(s_AirInof.ECO != m_sCanBoxProData.ECO)
	{
		//ECO
		int btn_status =0;
		if(s_AirInof.ECO > 0)
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_ECO,ITEM_STATE_SELECTED);
			btn_status= pUIMgr->GetUIAnalysisObj()->GetItemState(UIID_WNDID_AIRCOND, UIID_AIR_ECO);
			LOGDBG("按钮状态%d\n",btn_status);
		}else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_ECO,ITEM_STATE_NORMAL);
			btn_status= pUIMgr->GetUIAnalysisObj()->GetItemState(UIID_WNDID_AIRCOND, UIID_AIR_ECO);
			LOGDBG(": 按钮状态%d\n",btn_status);
		}
		bNeedShow = true;
	}
	// 自动净化
	if(s_AirInof.AutoMonitorPur != m_sCanBoxProData.AutoMonitorPur)
	{
		//自动监测净化
		if(s_AirInof.AutoMonitorPur > 0)
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_QUALITY_MON,ITEM_STATE_SELECTED);
		}else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AIRCOND,UIID_AIR_QUALITY_MON,ITEM_STATE_NORMAL);

		}
		bNeedShow = true;
	}

	m_sCanBoxProData = s_AirInof;

	if(bNeedShow)
	{
		//KillTimer(TIMERID_AIR_POP_HIDE);

		//SetTimer(TIMERID_AIR_POP_HIDE,5*1000);
		//GetViewBase()->ShowWnd(UIID_WNDID_AIRCOND, true);
	}

}

void AirConditionar::AirOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	printf("jgwen--- nWndId=%d,nCtrlId=%d,nBtState=%d \n",nWndId,nCtrlId,nBtState);

	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	IVIView &mView = IVIView::GetInstance();
	

	switch(nCtrlId)
	{
		case UIID_AIRCONDITION_RETURN:
		{
			if(1 == nBtState)
			{
				//关闭
				GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
			}
		}
		break;
		case UIID_AIRCONDITION_LEFTUP_BTN:
		{
			//左设定温度加
			if(NULL != m_ViewController)
			{
				if(0 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_LEFT_TEMP_ADD,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_LEFT_TEMP_ADD,0x01);
				}
			}
		}
		break;
		case UIID_AIRCONDITION_LEFTDOWN_BTN:
		{
			//左设定温度减
			if(NULL != m_ViewController)
			{
				if(0 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_LEFT_TEMP_REDU,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_LEFT_TEMP_REDU,0x01);
				}
			}			
		}
		break;
		case UIID_AIRCONDITION_RIGHTUP_BTN:
		{
			//右设定温度加
			if(NULL != m_ViewController)
			{
				if(0 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_RIGHT_TEMP_ADD,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_RIGHT_TEMP_ADD,0x01);
				}
			}			
		}
		break;
		case UIID_AIRCONDITION_RIGHTDOWN_BTN:
		{
			//右设定温度减
			if(NULL != m_ViewController)
			{
				if(0 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_RIGHT_TEMP_REDU,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_RIGHT_TEMP_REDU,0x01);
				}
			}			
		}
		break;
		case UIID_AIR_POWER:
		{
			//空调开关
			if(NULL != m_ViewController)
			{
				if(0 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_AIRSWITCH,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_AIRSWITCH,0x01);
				}
			}
		}
		break;
		case UIID_AIR_SYNC:
		{
			//SYNC
			if(NULL != m_ViewController)
			{
				if(0 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_SYNC,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_SYNC,0x01);
				}
			}
		}
		break;
		case UIID_AIR_AUTO:
		{
			//Auto 开关
			if(NULL != m_ViewController)
			{
				if(0 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_AUTO,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_AUTO,0x01);
				}
			}
		}
		break;
		case UIID_AIR_INOUT_SET:
		{
			//内外循环
			if(NULL != m_ViewController)
			{
				if(0 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_INEXCIR,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_INEXCIR,0x01);
				}
			}
			break;
		}
		case UIID_AIR_ACCMAX:
		{
			//AC MAX
			if(NULL != m_ViewController)
			{
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_AC_MAX,0x00);
					// m_ViewController->SendCanBox_Air_Control(0x1E,0x00);
				}else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_AC_MAX,0x01);
					// m_ViewController->SendCanBox_Air_Control(0x1E,0x01);
				}
			}
			break;
		}
		case UIID_AIR_FRONT_REMOVE:
		{
			//前窗除雾
			if(NULL != m_ViewController)
			{
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_FRONTWINDDEFOGG,0x00);
				}else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_FRONTWINDDEFOGG,0x01);
				}
			}
			break;		
		}
		case UIID_AIR_REAR_REMOVE:
		{
			//后窗除雾
			if(NULL != m_ViewController)
			{
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_REARWINDDEFOGG,0x00);
				}else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_REARWINDDEFOGG,0x01);
				}
			}
			break;		
		}
		case UIID_AIR_DUAL:
		{
			//DUAL
			if(NULL != m_ViewController)
			{
				int btn_status =0;
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_DUAL,0x00);
				}else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_DUAL,0x01);
				}
			}
			break;		
		}
		case UIID_AIR_MAXHEAT:
		{
			//MAXHEAT
			if(NULL != m_ViewController)
			{
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_MAX_HEAT,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_MAX_HEAT,0x01);
				}
			}
			break;			
		}
		case UIID_AIR_FOOT:
		{
			//吹脚
			if(NULL != m_ViewController)
			{
				//int btn_status;
				//btn_status= pUIMgr->GetUIAnalysisObj()->GetItemState(UIID_WNDID_AIRCOND, UIID_AIR_WINDOW);
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_VEN_MODE_FOOT,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_VEN_MODE_FOOT,0x01);
				}

				
			}
			break;		
		}
		case UIID_AIR_BODY:
		{
			//吹身
			if(NULL != m_ViewController)
			{
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_VEN_MODE_BODY,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_VEN_MODE_BODY,0x01);
				}
			}
			break;			
		}
		case UIID_AIR_WINDOW:
		{
			//吹窗
			if(NULL != m_ViewController)
			{
				if(0 == nBtState)  
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_VEN_MODE_WIND,0x00);
				}else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_VEN_MODE_WIND,0x01);
				}
			}
			break;			
		}
		case UIID_AIR_LEFT_HOT:
		{
			//前左座椅加热
			if(NULL != m_ViewController)
			{
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_L_SEAT_HEAT,0x00);
				}else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_L_SEAT_HEAT,0x01);
				}
			}
			break;			
		}
		case UIID_AIR_RIGHT_HOT:
		{
			//前右座椅加热
			if(NULL != m_ViewController)
			{
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_R_SEAT_HEAT,0x00);
				}else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_R_SEAT_HEAT,0x01);
				}
			}
			break;		
		}
		case UIID_AIR_LEFT_AIR:
		{
			//前左座椅吹风
			if(NULL != m_ViewController)
			{
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_L_SEAT_BLOW_HAIR,0x00);
				}else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_L_SEAT_BLOW_HAIR,0x01);
				}
			}
			break;			
		}
		case UIID_AIR_RIGHT_AIR:
		{
			//前右座椅吹风
			if(NULL != m_ViewController)
			{
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_R_SEAT_BLOW_HAIR,0x00);
				}else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_R_SEAT_BLOW_HAIR,0x01);
				}
			}
			break;	
		}
		case UIID_AIR_SPEED_ADD:
		{
			//前排风速加
			if(NULL != m_ViewController)
			{
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_WIND_LEVEL_ADD,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_WIND_LEVEL_ADD,0x01);
				}
			}
			break;			
		}
		case UIID_AIR_SPEED_REDUCE:
		{
			//前排风速减
			if(0 == nBtState) 
			{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_WIND_LEVEL_REDU,0x00);
			}
			else if(1 == nBtState)
			{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_WIND_LEVEL_REDU,0x01);
			}
			break;
		}
		case UIID_AIR_SET:
		{
			//设置
			if(0 == nBtState)
			{
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AIRCOND, UIID_AIR_SET_MODE,true);
			}
			break;
		}
		case UIID_AIR_ECO:
		{
			//ECO
			if(NULL != m_ViewController)
			{
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_ECO,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_ECO,0x01);
				}
			}
			break;				
		}
		case UIID_AIR_QUALITY_MON:
		{
			//自动监测净化
			if(NULL != m_ViewController)
			{
				if(0 == nBtState) 
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_AUTO_MONITOR_PUR,0x00);
				}
				else if(1 == nBtState)
				{
					mView.pCanBoxProto->Send_ItemData(CMD_ID_WR_AIRCONDITIONER_SET,CMD_ID_SUB_WR_AUTO_MONITOR_PUR,0x01);
				}
			}
			break;
		}
		case UIID_AIR_CANCEL:
		{
			if(1 == nBtState)
			{
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AIRCOND, UIID_AIR_SET_MODE,false);
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AIRCOND, UIID_AIR_CONTROL,true);
			}
			break;
		}
		default:
		break;
	}
}

void AirConditionar::AirOnComponent(CUIWndMng *pUI, PWINDOW_INFO pInfo)
{
	LOGDBG("jgwen--- pInfo.nInfoType=%d,nWindowID=%d,nCtrlID=%d ,status=%d\n",\
			pInfo->nInfoType, pInfo->nWindowID, pInfo->nCtrlID, pInfo->nOperationStatus);

	//int nWndID = pInfo->nWindowID;
	//int nCtrlID = pInfo->nCtrlID;
	//int nStatus = pInfo->nOperationStatus;
	//int nWndType = pInfo->nInfoType;
}

void AirConditionar::AirOnListInfoMessage(CUIWndMng *pUI, int nWndId, int nCtrlId, PLIST_INFO pInfo)
{
	//LOGDBG("AirConditionar ListInfo: nWndId=%d, nCtrlId=%d \n", nWndId, nCtrlId);
}

void AirConditionar::AirInitWorkWnd()
{
	//update
	OnHide_AudioBtn();
}

void AirConditionar::OnHide_AudioBtn()
{
	//隐藏不需要的按钮
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	// pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_EQ, 1713,false);

}

void AirConditionar::AirOnShow(LONG dwWndId)
{
	LOGDBG("jgwen--- OnShow Wnd ID Is %d", dwWndId);
	// EnableAlphaBlending(true);
	SetSetCurrWnd(dwWndId);
}

void AirConditionar::OnTimer(uint32_t id)
{
	LOGDBG("jgwen--- OnTimer id :%d", id);
	switch (id)
	{
		case TIMERID_SET_RSET:
		{
			KillTimer(TIMERID_SET_RSET);
			T_APP_MSG mMsg = {0,0,0,0};
			mMsg.Message = SYS_VMSG_UPDATE_TIRE_RESET;
			mMsg.Param0 = 1;
			GetViewBase()->SendViewMessage(&mMsg);
		}
		break;
		default:
		{
			break;
		}
	}
}


void AirConditionar::DrawAllItem()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	RECT rect;
	SetRect(&rect, 0, 0, 1920, 720);
	pUIMgr->drawAllItem(rect);
}

void AirConditionar::SetSetCurrWnd(int nWndID)
{
	Lock();
	m_nSetCurrWnd = nWndID;
	SettingData &mData = SettingData::GetInstance();
	mData.SetCurrWnd(nWndID);
	UnLock();
}

int AirConditionar::GetSetCurrWnd()
{
	int nWndID  = 0;
	Lock();
	nWndID = m_nSetCurrWnd;
	UnLock();
	return nWndID;
}

