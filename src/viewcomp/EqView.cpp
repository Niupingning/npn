/*
 * EqView.h
 *
 *  Created on: 2025-10-21
 *      Author: junjie.feng
 */


#include "EqView.h"
#include "EqData.h"
#include "UIID_Def_Setting.h"
#include "AssertDefine.h"
#include <locale.h>
#include <float.h>

// Dependent on external header files
#include <math.h>
#include <iconv.h>
#include <errno.h>
#include <wchar.h>

#include "IVIView.h"

ConfigHelper *EqView::FyAudioConfig =ConfigHelper::getInstance();

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


EqView::EqView(ViewBase* p_ViewBase)
	: CompBase(p_ViewBase)
	, m_nSetCurrWnd(0)
	, m_ViewController(NULL)
	, m_AudioControl(NULL)
	, m_ConfigHelper(NULL)
	, m_nspeaker(-1)
{
   m_nEqData = &EqData::GetInstance();
   InitAudioConfig();
}

EqView::~EqView()
{
	// TODO Auto-generated destructor stub
}

void EqView::InitAudioConfig()
{
	if(access("/media/flash/userdata/audio_boost.conf", F_OK) != 0)
	{
		m_ConfigHelper->set("balance", 0);
		m_ConfigHelper->set("channel0_l1_speaker_level", 0);
		m_ConfigHelper->set("channel0_l2_speaker_level", 0);
		m_ConfigHelper->set("channel0_l3_speaker_level", 0);
		m_ConfigHelper->set("channel0_l4_speaker_level", 0);
		m_ConfigHelper->set("channel1_l1_speaker_level", 0);
		m_ConfigHelper->set("channel1_l2_speaker_level", 0);
		m_ConfigHelper->set("channel1_l3_speaker_level", 0);
		m_ConfigHelper->set("channel1_l4_speaker_level", 0);
		m_ConfigHelper->set("channel2_l1_speaker_level", 0);
		m_ConfigHelper->set("channel2_l2_speaker_level", 0);
		m_ConfigHelper->set("channel2_l3_speaker_level", 0);
		m_ConfigHelper->set("channel2_l4_speaker_level", 0);
		m_ConfigHelper->set("channel3_l1_speaker_level", 0);
		m_ConfigHelper->set("channel3_l2_speaker_level", 0);
		m_ConfigHelper->set("channel3_l3_speaker_level", 0);
		m_ConfigHelper->set("channel3_l4_speaker_level", 0);
		m_ConfigHelper->set("channel5_l1_speaker_level", 0);
		m_ConfigHelper->set("channel5_l2_speaker_level", 0);
		m_ConfigHelper->set("channel5_l3_speaker_level", 0);
		m_ConfigHelper->set("channel5_l4_speaker_level", 0);
		m_ConfigHelper->set("crossover0", 0);
		m_ConfigHelper->set("crossover0_freq_id", 0);
		m_ConfigHelper->set("crossover0_slope_id", 0);
		m_ConfigHelper->set("crossover1", 0);
		m_ConfigHelper->set("crossover1_freq_id", 0);
		m_ConfigHelper->set("crossover1_slope_id", 0);
		m_ConfigHelper->set("crossover2", 0);
		m_ConfigHelper->set("crossover2_freq_id", 0);
		m_ConfigHelper->set("crossover2_slope_id", 0);
		m_ConfigHelper->set("crossover_channel", 0);
		m_ConfigHelper->set("custom1_index0", 2);
		m_ConfigHelper->set("custom1_index1", 4);
		m_ConfigHelper->set("custom1_index2", -2);
		m_ConfigHelper->set("custom1_index3", -10);
		m_ConfigHelper->set("custom1_index4", 2);
		m_ConfigHelper->set("custom1_index5", 0);
		m_ConfigHelper->set("custom1_index6", 0);
		m_ConfigHelper->set("custom1_index7", 2);
		m_ConfigHelper->set("custom1_index8", 2);
		m_ConfigHelper->set("custom1_index9", 4);
		m_ConfigHelper->set("custom1_index10", 2);
		m_ConfigHelper->set("custom1_index11", 4);
		m_ConfigHelper->set("custom1_index12", 6);
		m_ConfigHelper->set("eq_mode", 5);
		m_ConfigHelper->set("fade", 0);
		m_ConfigHelper->set("listening_position", 0);
		m_ConfigHelper->set("loudness", 0);
		m_ConfigHelper->set("speakerMode", 0);
		m_ConfigHelper->set("subwoofer_on", 0);
		m_ConfigHelper->set("time_alignment_on", 0);

		m_ConfigHelper->setFloat("f_channel0_time_alignment_p0", 0.0);
		m_ConfigHelper->setFloat("f_channel0_time_alignment_p1", 0.0);
		m_ConfigHelper->setFloat("f_channel0_time_alignment_p2", 0.0);
		m_ConfigHelper->setFloat("f_channel0_time_alignment_p3", 0.0);
		m_ConfigHelper->setFloat("f_channel0_time_alignment_p4", 0.0);
		m_ConfigHelper->setFloat("f_channel1_time_alignment_p0", 0.0);
		m_ConfigHelper->setFloat("f_channel1_time_alignment_p1", 0.0);
		m_ConfigHelper->setFloat("f_channel1_time_alignment_p2", 0.0);
		m_ConfigHelper->setFloat("f_channel1_time_alignment_p3", 0.0);
		m_ConfigHelper->setFloat("f_channel1_time_alignment_p4", 0.0);
		m_ConfigHelper->setFloat("f_channel2_time_alignment_p0", 0.0);
		m_ConfigHelper->setFloat("f_channel2_time_alignment_p1", 0.0);
		m_ConfigHelper->setFloat("f_channel2_time_alignment_p2", 0.0);
		m_ConfigHelper->setFloat("f_channel2_time_alignment_p3", 0.0);
		m_ConfigHelper->setFloat("f_channel2_time_alignment_p4", 0.0);
		m_ConfigHelper->setFloat("f_channel3_time_alignment_p0", 0.0);
		m_ConfigHelper->setFloat("f_channel3_time_alignment_p1", 0.0);
		m_ConfigHelper->setFloat("f_channel3_time_alignment_p2", 0.0);
		m_ConfigHelper->setFloat("f_channel3_time_alignment_p3", 0.0);
		m_ConfigHelper->setFloat("f_channel3_time_alignment_p4", 0.0);
		m_ConfigHelper->setFloat("f_channel5_time_alignment_p0", 0.0);
		m_ConfigHelper->setFloat("f_channel5_time_alignment_p1", 0.0);
		m_ConfigHelper->setFloat("f_channel5_time_alignment_p2", 0.0);
		m_ConfigHelper->setFloat("f_channel5_time_alignment_p3", 0.0);
		m_ConfigHelper->setFloat("f_channel5_time_alignment_p4", 0.0);

		if(FyAudioConfig->writeConfig())
		{
			LOGDBG("write config sucessfull!");
		}
		else
		{
			LOGDBG("write config false!");
		}		
	}
	else{}
	
}

bool EqView::EqOnUserMessage(T_APP_MSG *msg)
{
	if (nullptr == msg)
	{
		return false;
	}
	
	LOGDBG("jgwen--- Message = %d,param0=%d,param1=%d,param2=%d\n", \
			msg->Message, msg->Param0,msg->Param1,msg->Param2);
	return true;
}


void EqView::EqOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	LOGDBG("jgwen--- nWndId=%d,nCtrlId=%d,nBtState=%d",nWndId,nCtrlId,nBtState);
	if(1 != nBtState)
	{
       return;
	}
	if(nWndId == UIID_WNDID_AUDIOBOOST)
	{
		switch(nCtrlId)
		{
			case UIID_AUDIO_BOOST_RETURN:
			{
				onClick_ReturnBtn();
			}
			break;
			case UIID_AUDIO_BOOST_CLOSE:
			{
				//关闭
				GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
			}
			break;
			case UIID_AUDIO_BOOST_LIST:
			{
				//点击列表
				onClick_SettingList();
			}
			break;
			case UIID_GROUP_LOUDNES_OFF:
			case UIID_GROUP_LOUDNES_LOW:
			case UIID_GROUP_LOUDNES_MID:
			case UIID_GROUP_LOUDNES_HIGH:
			{
			onClick_LoudnesBtn(nCtrlId);
			break;
			}
			case UIID_GROUP_SPEAKER_TYPE_L:
			case UIID_GROUP_SPEAKER_TYPE_R:
			case UIID_GROUP_SPEAKER_FL_L:
			case UIID_GROUP_SPEAKER_FL_R:
			case UIID_GROUP_SPEAKER_FR_L:
			case UIID_GROUP_SPEAKER_FR_R:
			case UIID_GROUP_SPEAKER_RL_L:
			case UIID_GROUP_SPEAKER_RL_R:
			case UIID_GROUP_SPEAKER_RR_L:
			case UIID_GROUP_SPEAKER_RR_R:
			case UIID_GROUP_SPEAKER_SUB_L:
			case UIID_GROUP_SPEAKER_SUB_R:
			{
				onClick_Speaker(nCtrlId);
				break;
			}
			case UIID_GROUP_LISTENING_FRONT:
			case UIID_GROUP_LISTENING_FRONTL:
			case UIID_GROUP_LISTENING_FRONTR:
			case UIID_GROUP_LISTENING_ALL:
			case UIID_GROUP_LISTENING_OFF:
			{
				onClick_Listening(nCtrlId);
				break;
			}
			case UIID_GROUP_FADER_FRONT:
			{
				CUIWndMgrWrapper *pUIMgr = NULL; 
				pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
				if (NULL == pUIMgr)
				{
					LOGERR("pUIMgr is NULL");
					return;
				}

				int m_nfader = m_ConfigHelper->get("fade");
				int m_nbalance = m_ConfigHelper->get("balance");
				wchar_t text1[16] = {0};
				wchar_t text2[16] = {0};
				if(m_nfader >= -15 && m_nfader < 15)
				{
					m_nfader++;
					if(m_nfader >= 0 && m_nfader <= 15)
					{	
						swprintf(text1,16, L"%d",m_nfader);
						swprintf(text2,16, L"%d",0);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_FRONT_VALUE, text1);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_REAR_VALUE, text2);
					}
					else if(m_nfader >= -15 && m_nfader <= 0)
					{
						swprintf(text1,16, L"%d", abs(m_nfader));
						swprintf(text2,16, L"%d",0);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_REAR_VALUE, text1);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_FRONT_VALUE, text2);
					}

					LOGDBG("m_nfader = %d",m_nfader);
					pUIMgr->GetUIAnalysisObj()->SetCursorPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_CURSOR, m_nbalance + 15, m_nfader + 15);
					
					m_AudioControl->setFadeBalance(m_nfader,m_nbalance);
				}						
				break;
			}
			case UIID_GROUP_FADER_REAR:
			{
				CUIWndMgrWrapper *pUIMgr = NULL; 
				pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
				if (NULL == pUIMgr)
				{
					LOGERR("pUIMgr is NULL");
					return;
				}

				int m_nfader = m_ConfigHelper->get("fade");
				int m_nbalance = m_ConfigHelper->get("balance");
				wchar_t text1[16] = {0};
				wchar_t text2[16] = {0};
				if(m_nfader > -15 && m_nfader <= 15)
				{
					m_nfader--;
					if(m_nfader >= 0 && m_nfader <= 15)
					{	
						swprintf(text1,16, L"%d",m_nfader);
						swprintf(text2,16, L"%d",0);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_FRONT_VALUE, text1);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_REAR_VALUE, text2);
					}
					else if(m_nfader >= -15 && m_nfader <= 0)
					{
						swprintf(text1,16, L"%d", abs(m_nfader));
						swprintf(text2,16, L"%d",0);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_REAR_VALUE, text1);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_FRONT_VALUE, text2);
					}

					LOGDBG("m_nfader = %d",m_nfader);
					pUIMgr->GetUIAnalysisObj()->SetCursorPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_CURSOR, m_nbalance + 15, m_nfader + 15);

					m_AudioControl->setFadeBalance(m_nfader,m_nbalance);
				}						
				break;
			}
			case UIID_GROUP_FADER_LEFT:
			{
				CUIWndMgrWrapper *pUIMgr = NULL; 
				pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
				if (NULL == pUIMgr)
				{
					LOGERR("pUIMgr is NULL");
					return;
				}

				int m_nfader = m_ConfigHelper->get("fade");
				int m_nbalance = m_ConfigHelper->get("balance");
				wchar_t text1[16] = {0};
				wchar_t text2[16] = {0};
				if(m_nbalance > -15 && m_nbalance <= 15)
				{
					m_nbalance--;
					if(m_nbalance >= 0 && m_nbalance <= 15)
					{	
						swprintf(text1,16, L"%d",m_nbalance);
						swprintf(text2,16, L"%d",0);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_RIGHT_VALUE, text1);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_LEFT_VALUE, text2);
					}
					else if(m_nbalance >= -15 && m_nbalance <= 0)
					{
						swprintf(text1,16, L"%d", abs(m_nbalance));
						swprintf(text2,16, L"%d",0);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_LEFT_VALUE, text1);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_RIGHT_VALUE, text2);
					}

					LOGDBG("m_nbalance = %d",m_nbalance);
					pUIMgr->GetUIAnalysisObj()->SetCursorPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_CURSOR, m_nbalance + 15, m_nfader + 15);
					m_AudioControl->setFadeBalance(m_nfader,m_nbalance);
				}						
				break;
			}
			case UIID_GROUP_FADER_RIGHT:
			{
				CUIWndMgrWrapper *pUIMgr = NULL; 
				pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
				if (NULL == pUIMgr)
				{
					LOGERR("pUIMgr is NULL");
					return;
				}

				int m_nfader = m_ConfigHelper->get("fade");
				int m_nbalance = m_ConfigHelper->get("balance");
				wchar_t text1[16] = {0};
				wchar_t text2[16] = {0};
				if(m_nbalance >= -15 && m_nbalance < 15)
				{
					m_nbalance++;
					if(m_nbalance >= 0 && m_nbalance <= 15)
					{	
						swprintf(text1,16, L"%d",m_nbalance);
						swprintf(text2,16, L"%d",0);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_RIGHT_VALUE, text1);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_LEFT_VALUE, text2);
					}
					else if(m_nbalance >= -15 && m_nbalance <= 0)
					{
						swprintf(text1,16, L"%d", abs(m_nbalance));
						swprintf(text2,16, L"%d",0);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_LEFT_VALUE, text1);
						pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_RIGHT_VALUE, text2);
					}

					LOGDBG("m_nbalance = %d",m_nbalance);
					pUIMgr->GetUIAnalysisObj()->SetCursorPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_CURSOR, m_nbalance + 15, m_nfader + 15);
					m_AudioControl->setFadeBalance(m_nfader,m_nbalance);
				}						
				break;
			}
			case UIID_GROUP_SUBFOOER_OFF:
			{
				CUIWndMgrWrapper *pUIMgr = NULL; 
				pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
				if (NULL == pUIMgr)
				{
					LOGERR("pUIMgr is NULL");
					return;
				}
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, nCtrlId, 907, ALL_STATE);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SUBFOOER_ON, 0, ALL_STATE);
				UIITEMDATA UItem;
				memset(&UItem,0,sizeof(UItem));
				pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_AUDIOBOOST,UIID_AUDIO_BOOST_LIST,UItem);
				CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;
				pList->SetItemString(6, 608, 1);
				m_AudioControl->setSubwoofer(0);
				break;
			}		
			case UIID_GROUP_SUBFOOER_ON:
			{
				CUIWndMgrWrapper *pUIMgr = NULL; 
				pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
				if (NULL == pUIMgr)
				{
					LOGERR("pUIMgr is NULL");
					return;
				}
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, nCtrlId, 907, ALL_STATE);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SUBFOOER_OFF, 0, ALL_STATE);
				UIITEMDATA UItem;
				memset(&UItem,0,sizeof(UItem));
				pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_AUDIOBOOST,UIID_AUDIO_BOOST_LIST,UItem);
				CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;
				pList->SetItemString(6, 609, 1);
				m_AudioControl->setSubwoofer(1);
				break;
			}		
			case UIID_GROUP_EQ_GAIN_1:
			case UIID_GROUP_EQ_GAIN_2:
			case UIID_GROUP_EQ_GAIN_3:
			case UIID_GROUP_EQ_GAIN_4:
			case UIID_GROUP_EQ_GAIN_5:
			case UIID_GROUP_EQ_GAIN_6:
			case UIID_GROUP_EQ_GAIN_7:
			case UIID_GROUP_EQ_GAIN_8:
			case UIID_GROUP_EQ_GAIN_9:
			case UIID_GROUP_EQ_GAIN_10:
			case UIID_GROUP_EQ_GAIN_11:
			case UIID_GROUP_EQ_GAIN_12:
			case UIID_GROUP_EQ_GAIN_13:
			case UIID_GROUP_EQ_LEFT:
			case UIID_GROUP_EQ_RIGHT:
			{
				CUIWndMgrWrapper *pUIMgr = NULL; 
				pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
				if (NULL == pUIMgr)
				{
					LOGERR("pUIMgr is NULL");
					return;
				}

				/*
				UIITEMDATA UItem;
				memset(&UItem,0,sizeof(UItem));
				pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_AUDIOBOOST,UIID_GROUP_EQ_CONT,UItem);
				CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;

				int nline = pList->GetTopIndex();
				LOGDBG("GetTopIndex = %d",nline);
				*/

				int m_eqmode = m_ConfigHelper->get("eq_mode");
				vector<int> eq_custom;
				eq_custom.reserve(13);
				char config_item_name[32] = {0};
				
				for(int i = 0; i < 13; i++)
				{
					sprintf(config_item_name, "custom1_index%d", i);
					eq_custom.push_back(m_ConfigHelper->get(config_item_name));
				}

				switch(nCtrlId)
				{
					case UIID_GROUP_EQ_GAIN_1:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_1);
						if(m_eqmode == 5)
						{	
							eq_custom[0] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[0] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}
					case UIID_GROUP_EQ_GAIN_2:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_2);
						if(m_eqmode == 5)
						{	
							eq_custom[1] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[2] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}
					case UIID_GROUP_EQ_GAIN_3:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_3);
						if(m_eqmode == 5)
						{	
							eq_custom[2] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[2] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}
					case UIID_GROUP_EQ_GAIN_4:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_4);
						if(m_eqmode == 5)
						{	
							eq_custom[3] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[3] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}
					case UIID_GROUP_EQ_GAIN_5:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_5);
						if(m_eqmode == 5)
						{	
							eq_custom[4] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[4] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}
					case UIID_GROUP_EQ_GAIN_6:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_6);
						if(m_eqmode == 5)
						{	
							eq_custom[5] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[5] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}
					case UIID_GROUP_EQ_GAIN_7:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_7);
						if(m_eqmode == 5)
						{	
							eq_custom[6] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[6] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}
					case UIID_GROUP_EQ_GAIN_8:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_8);
						if(m_eqmode == 5)
						{	
							eq_custom[7] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[7] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}
					case UIID_GROUP_EQ_GAIN_9:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_9);
						if(m_eqmode == 5)
						{	
							eq_custom[8] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[8] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}
					case UIID_GROUP_EQ_GAIN_10:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_10);
						if(m_eqmode == 5)
						{	
							eq_custom[9] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[9] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}
					case UIID_GROUP_EQ_GAIN_11:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_11);
						if(m_eqmode == 5)
						{	
							eq_custom[10] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[10] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}	
					case UIID_GROUP_EQ_GAIN_12:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_12);
						if(m_eqmode == 5)
						{	
							eq_custom[11] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[11] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}
					case UIID_GROUP_EQ_GAIN_13:
					{
						int eq_gain = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_GAIN_13);
						if(m_eqmode == 5)
						{	
							eq_custom[12] = eq_gain*2 - 12;
						}
						else
						{
							adEqInfo[m_eqmode].eq_gain_array[12] = eq_gain*2 - 12;
							for(int i = 0; i < 13; i++)
							{
								eq_custom[i] = adEqInfo[m_eqmode].eq_gain_array[i];
							}
							//pList->SetCurSel(5, 0);
						}
						m_AudioControl->setEqValue(5, eq_custom);
						break;
					}
					case UIID_GROUP_EQ_LEFT:
					{
						/*if(nline == 3)
						{
							//pList->SetTopIndex(0);
							pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_LEFT, ITEM_STATE_DISABLE);
							pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_RIGHT, ITEM_STATE_NORMAL);
						}
						else
						{

						}*/
						break;
					}	
					case UIID_GROUP_EQ_RIGHT:
					{
						/*
						if(nline == 0 || nline == 1 || nline == 2)
						{
							//pList->SetTopIndex(3);
							pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_RIGHT, ITEM_STATE_DISABLE);
							pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ_LEFT, ITEM_STATE_NORMAL);
						}
						else 
						{

						}*/
						break;
					}																																																										
					default:
					{
						break;		
					}				
				}
				
				break;		
			}
			case UIID_GROUP_EQ_SUPERBASS:
			{
				vector<int> gain;
				gain.reserve(13);

				int tmp[] = {8, 8, 6, 6, 2, 0, 0, 0, 0, 2, 2, 4, 2};
				for(int i = 0; i < 13; i++)
				{
					GetViewBase()->GetUIWndMgrWrapper()->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(tmp[i]+12)/2);
				}
				m_AudioControl->setEqValue(0, gain);
				break;
			}
			case UIID_GROUP_EQ_POWERFUL:
			{
				vector<int> gain;
				gain.reserve(13);

				int tmp[] = {2, 6, 4, 2, 0, -2, 0, 2, 2, 2, 4, 4, 6};
				for(int i = 0; i < 13; i++)
				{
					GetViewBase()->GetUIWndMgrWrapper()->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(tmp[i]+12)/2);
				}
				m_AudioControl->setEqValue(1, gain);
				break;
			}
			case UIID_GROUP_EQ_NATURAL:
			{
				vector<int> gain;
				gain.reserve(13);

				int tmp[] = {-6, 2, 2, 2, 0, 0, 2, 2, -2, -2, 2, 2, 4};
				for(int i = 0; i < 13; i++)
				{
					GetViewBase()->GetUIWndMgrWrapper()->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(tmp[i]+12)/2);
				}				
				m_AudioControl->setEqValue(2, gain);
				break;
			}
			case UIID_GROUP_EQ_VOCAL:
			{
				vector<int> gain;
				gain.reserve(13);

				int tmp[] = {-6, -2, 0, 4, 6, 6, 2, 0, 2, 4, 4, 2, 0};
				for(int i = 0; i < 13; i++)
				{
					GetViewBase()->GetUIWndMgrWrapper()->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(tmp[i]+12)/2);
				}				
				m_AudioControl->setEqValue(3, gain);
				break;
			}
			case UIID_GROUP_EQ_FLAT:
			{
				vector<int> gain;
				gain.reserve(13);

				int tmp[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
				for(int i = 0; i < 13; i++)
				{
					GetViewBase()->GetUIWndMgrWrapper()->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(tmp[i]+12)/2);
				}				
				m_AudioControl->setEqValue(4, gain);
				break;
			}
			case UIID_GROUP_EQ_CUSTOM1:
			case UIID_GROUP_EQ_CUSTOM2:
			{
				vector<int> gain;
				gain.reserve(13);

				char config_item_name[32] = {0};
			
				for(int i = 0; i < 13; i++)
				{
					sprintf(config_item_name, "custom1_index%d", i);
					gain.push_back(m_ConfigHelper->get(config_item_name));
				}
				for(int i = 0; i < 13; i++)
				{
					GetViewBase()->GetUIWndMgrWrapper()->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(gain[i]+12)/2);
				}				
				m_AudioControl->setEqValue(5, gain);
				break;
			}
			case UIID_GROUP_EQ_CONT:
			{
				/*
				CUIWndMgrWrapper *pUIMgr = NULL; 
				pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
				if (NULL == pUIMgr)
				{
					LOGERR("pUIMgr is NULL");
					return;
				}

				UIITEMDATA UItem;
				memset(&UItem,0,sizeof(UItem));
				pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_AUDIOBOOST,UIID_GROUP_EQ_CONT,UItem);
				CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;
				int Line = 0;
				int Col	 = 0;
				int data = 0;
				vector<int> gain;
				gain.reserve(13);

				pList->GetCurSel(Line, data, Col);
				LOGDBG("Line = %d---Col = %d",Line,Col);

				if(Line == 0)
				{
					int tmp[] = {8, 8, 6, 6, 2, 0, 0, 0, 0, 2, 2, 4, 2};
					for(int i = 0; i < 13; i++)
					{
						pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(tmp[i]+12)/2);
					}
					m_AudioControl->setEqValue(0, gain);
				}
				else if(Line == 1)
				{
					int tmp[] = {2, 6, 4, 2, 0, -2, 0, 2, 2, 2, 4, 4, 6};
					for(int i = 0; i < 13; i++)
					{
						pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(tmp[i]+12)/2);
					}
					m_AudioControl->setEqValue(1, gain);
				}
				else if(Line == 2)
				{
					int tmp[] = {-6, 2, 2, 2, 0, 0, 2, 2, -2, -2, 2, 2, 4};
					for(int i = 0; i < 13; i++)
					{
						pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(tmp[i]+12)/2);
					}				
					m_AudioControl->setEqValue(2, gain);
				}
				else if(Line == 3)
				{
					int tmp[] = {-6, -2, 0, 4, 6, 6, 2, 0, 2, 4, 4, 2, 0};
					for(int i = 0; i < 13; i++)
					{
						pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(tmp[i]+12)/2);
					}				
					m_AudioControl->setEqValue(3, gain);
				}
				else if(Line == 4)
				{
					int tmp[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
					for(int i = 0; i < 13; i++)
					{
						pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(tmp[i]+12)/2);
					}				
					m_AudioControl->setEqValue(4, gain);
				}
				else if(Line == 5)
				{
					char config_item_name[32] = {0};
				
					for(int i = 0; i < 13; i++)
					{
						sprintf(config_item_name, "custom1_index%d", i);
						gain.push_back(m_ConfigHelper->get(config_item_name));
					}
					for(int i = 0; i < 13; i++)
					{
						pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(gain[i]+12)/2);
					}				
					m_AudioControl->setEqValue(5, gain);
				}
				else{}
				*/
				break;
			}
			case UIID_GROUP_TIME_TYPE_L:
			case UIID_GROUP_TIME_TYPE_R:
			case UIID_GROUP_TIME_FL_L:
			case UIID_GROUP_TIME_FL_R:
			case UIID_GROUP_TIME_FR_L:
			case UIID_GROUP_TIME_FR_R:
			case UIID_GROUP_TIME_RL_L:
			case UIID_GROUP_TIME_RL_R:
			case UIID_GROUP_TIME_RR_L:
			case UIID_GROUP_TIME_RR_R:
			case UIID_GROUP_TIME_SUB_L:
			case UIID_GROUP_TIME_SUB_R:
			case UIID_GROUP_TIME_ONOFF:
			{
				onClick_Time_Alignment(nCtrlId);
				break;
			}
			case UIID_GROUP_CROSSOVER_L:
			case UIID_GROUP_CROSSOVER_R:
			case UIID_GROUP_CROSSOVER_ONOFF:
			case UIID_GROUP_CROSSOVER_H:
			case UIID_GROUP_CROSSOVER_V:
			{
				onClick_Time_Crossover(nCtrlId);
				break;
			}
			default:
			break;
		}
	}
	else
	{
		char config_item_name[32] = {0};
		sprintf(config_item_name, "speakerMode");
		switch(nCtrlId)
		{
			case UIID_GROUP_SPEAKER_2WAY:		//点击Standard Mode
			{
				m_nspeaker = 0;				
				break;
			}
			case UIID_GROUP_SPEAKER_3WAY:		//点击NetWork Mode
			{
				m_nspeaker == 1;			
				break;
			}
			case UIID_GROUP_SPEAKER_OK:
			{
				system("rm /media/flash/userdata/speaker_mode &");

				if(m_nspeaker == 1)
				{
					m_ConfigHelper->set(config_item_name,1);	//选择NetWork Mode后,点击OK按键则设置为3way
				}
				else
				{
					m_ConfigHelper->set(config_item_name,0);	//选择Standard Mode后或不选择,点击OK按键则设置为2way
				}

				if(FyAudioConfig->writeConfig())
				{
					LOGDBG("write config sucessfull!");
				}
				GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
				break;
			}
			default:
			break;

		}		
	}
	
}

void EqView::onClick_Time_Crossover(long nCtrlId)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	int channel = m_ConfigHelper->get("crossover_channel");

	switch(nCtrlId)
	{
		case UIID_GROUP_CROSSOVER_L:
		{
			if(channel == 0)
			{
				int freq_1 = m_ConfigHelper->get("crossover1_freq_id");
				int slope_1 = m_ConfigHelper->get("crossover1_slope_id");
				int rear_hpf = m_ConfigHelper->get("crossover1");
				wchar_t buf[16] = {0};
				wchar_t buf1[16] = {0};

				if(freq_1 == 1)
				{
					swprintf(buf,16, L"%.1f", 31.5);
				}
				else
				{
					swprintf(buf,16, L"%d",freq[freq_1]);
				}
				swprintf(buf1,16,L"%d",slope[slope_1]);
				if(rear_hpf == 0)
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 641);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_NORMAL);
				}
				else
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 720);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_SELECTED);
				}
				
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_SLOPE, buf1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_HPF, buf);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_TYPE, 653);//Rear
				pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, 3, slope_1);
				pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, freq_1);
//				pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, slope_1);
				m_AudioControl->setCrossover(1, freq_1, slope_1);
			}
			else if(channel == 1)
			{
				int freq_2 = m_ConfigHelper->get("crossover2_freq_id");
				int slope_2 = m_ConfigHelper->get("crossover2_slope_id");
				int lpf = m_ConfigHelper->get("crossover2");
				wchar_t buf[16] = {0};
				wchar_t buf1[16] = {0};

				if(freq_2 == 1)
				{
					swprintf(buf,16, L"%.1f", 31.5);
				}
				else
				{
					swprintf(buf,16, L"%d",freq[freq_2]);
				}
				swprintf(buf1,16,L"%d",slope[slope_2]);
				if(lpf == 0)
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 721);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_NORMAL);
				}
				else
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 642);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_SELECTED);
				}				
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_SLOPE, buf1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_HPF, buf);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_TYPE, 606);//Subwoofer
				pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, 5, slope_2);
				pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, freq_2);
//				pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, slope_2);
				m_AudioControl->setCrossover(2, freq_2, slope_2);
			}
			else if(channel == 2)
			{
				int freq_0 = m_ConfigHelper->get("crossover0_freq_id");
				int slope_0 = m_ConfigHelper->get("crossover0_slope_id");
				int front_hpf = m_ConfigHelper->get("crossover0");
				wchar_t buf[16] = {0};
				wchar_t buf1[16] = {0};

				if(freq_0 == 1)
				{
					swprintf(buf,16, L"%.1f", 31.5);
				}
				else
				{
					swprintf(buf,16, L"%d",freq[freq_0]);
				}
				swprintf(buf1,16,L"%d",slope[slope_0]);
				if(front_hpf == 0)
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 641);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_NORMAL);
				}
				else
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 720);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_SELECTED);
				}				
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_SLOPE, buf1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_HPF, buf);				
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_TYPE, 627);//Front
				pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, 3, slope_0);
				pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, freq_0);
//				pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, slope_0);
				m_AudioControl->setCrossover(0, freq_0, slope_0);
			}
			else{}

			break;
		}
		case UIID_GROUP_CROSSOVER_R:
		{
			if(channel == 0)
			{
				int freq_2 = m_ConfigHelper->get("crossover2_freq_id");
				int slope_2 = m_ConfigHelper->get("crossover2_slope_id");
				int lpf = m_ConfigHelper->get("crossover2");
				wchar_t buf[16] = {0};
				wchar_t buf1[16] = {0};

				if(freq_2 == 1)
				{
					swprintf(buf,16, L"%.1f", 31.5);
				}
				else
				{
					swprintf(buf,16, L"%d",freq[freq_2]);
				}
				swprintf(buf1,16,L"%d",slope[slope_2]);
				if(lpf == 0)
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 721);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_NORMAL);
				}
				else
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 642);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_SELECTED);
				}
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_SLOPE, buf1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_HPF, buf);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_TYPE, 606);//Subwoofer
				pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, 5, slope_2);
				pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, freq_2);
//				pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, slope_2);
				m_AudioControl->setCrossover(2, freq_2, slope_2);
			}
			else if(channel == 1)
			{
				int freq_0 = m_ConfigHelper->get("crossover0_freq_id");
				int slope_0 = m_ConfigHelper->get("crossover0_slope_id");
				int front_hpf = m_ConfigHelper->get("crossover0");
				wchar_t buf[16] = {0};
				wchar_t buf1[16] = {0};

				if(freq_0 == 1)
				{
					swprintf(buf,16, L"%.1f", 31.5);
				}
				else
				{
					swprintf(buf,16, L"%d",freq[freq_0]);
				}
				swprintf(buf1,16,L"%d",slope[slope_0]);
				if(front_hpf == 0)
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 641);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_NORMAL);
				}
				else
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 720);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_SELECTED);
				}				
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_SLOPE, buf1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_HPF, buf);				
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_TYPE, 627);//Front
				pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, 3, slope_0);
				pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, freq_0);
//				pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, slope_0);
				m_AudioControl->setCrossover(0, freq_0, slope_0);
			}
			else if(channel == 2)
			{
				int freq_1 = m_ConfigHelper->get("crossover1_freq_id");
				int slope_1 = m_ConfigHelper->get("crossover1_slope_id");
				int rear_hpf = m_ConfigHelper->get("crossover1");
				wchar_t buf[16] = {0};
				wchar_t buf1[16] = {0};

				if(freq_1 == 1)
				{
					swprintf(buf,16, L"%.1f", 31.5);
				}
				else
				{
					swprintf(buf,16, L"%d",freq[freq_1]);
				}
				swprintf(buf1,16,L"%d",slope[slope_1]);
				if(rear_hpf == 0)
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 641);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_NORMAL);
				}
				else
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 720);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_SELECTED);
				}
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_SLOPE, buf1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_HPF, buf);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_TYPE, 653);//Rear
				pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, 3, slope_1);
				pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, freq_1);
				pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, slope_1);
				m_AudioControl->setCrossover(1, freq_1, slope_1);
			}
			else{}

			break;
		}
		case UIID_GROUP_CROSSOVER_ONOFF:
		{
			if(channel == 0)
			{
				int front = m_ConfigHelper->get("crossover0");

				if(front == 0)
				{					
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 720);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_SELECTED);
					pUIMgr->GetUIAnalysisObj()->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, true);
					pUIMgr->GetUIAnalysisObj()->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, true);
					m_AudioControl->setCrossoverOn(0,1);
				}
				else
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 641);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_NORMAL);
					pUIMgr->GetUIAnalysisObj()->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, false);
					pUIMgr->GetUIAnalysisObj()->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, false);
					m_AudioControl->setCrossoverOn(0,0);
				}
			}
			else if(channel == 1)
			{
				int rear = m_ConfigHelper->get("crossover1");

				if(rear == 0)
				{					
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 720);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_SELECTED);
					pUIMgr->GetUIAnalysisObj()->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, true);
					pUIMgr->GetUIAnalysisObj()->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, true);					
					m_AudioControl->setCrossoverOn(1,1);
				}
				else
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 641);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_NORMAL);
					pUIMgr->GetUIAnalysisObj()->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, false);
					pUIMgr->GetUIAnalysisObj()->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, false);
					m_AudioControl->setCrossoverOn(1,0);
				}
			}
			else if(channel == 2)
			{
				int sub = m_ConfigHelper->get("crossover2");

				if(sub == 0)
				{					
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 642);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_SELECTED);
					pUIMgr->GetUIAnalysisObj()->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, true);
					pUIMgr->GetUIAnalysisObj()->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, true);					
					m_AudioControl->setCrossoverOn(2,1);
				}
				else
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 721);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_NORMAL);
					pUIMgr->GetUIAnalysisObj()->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, false);
					pUIMgr->GetUIAnalysisObj()->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, false);					
					m_AudioControl->setCrossoverOn(2,0);
				}				
			}
			else{}
		}	
		case UIID_GROUP_CROSSOVER_H:
		{
			wchar_t buf[16] = {0};
			int freq_id = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H);			
			if(channel == 0)
			{
				int slope_0 = m_ConfigHelper->get("crossover0_slope_id");
				if(freq_id == 1)
				{
					swprintf(buf,16, L"%.1f", 31.5);
				}
				else
				{
					swprintf(buf,16, L"%d", freq[freq_id]);
				}
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_HPF, buf);
				m_AudioControl->setCrossover(0, freq_id, slope_0);
			}
			else if(channel == 1)
			{
				int slope_1 = m_ConfigHelper->get("crossover1_slope_id");
				if(freq_id == 1)
				{
					swprintf(buf,16, L"%.1f", 31.5);
				}
				else
				{
					swprintf(buf,16, L"%d", freq[freq_id]);
				}
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_HPF, buf);
				m_AudioControl->setCrossover(1, freq_id, slope_1);
			}
			else if(channel == 2)
			{
				int slope_2 = m_ConfigHelper->get("crossover2_slope_id");
				if(freq_id == 1)
				{
					swprintf(buf,16, L"%.1f", 31.5);
				}
				else
				{
					swprintf(buf,16, L"%d", freq[freq_id]);
				}
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_HPF, buf);
				m_AudioControl->setCrossover(2, freq_id, slope_2);				
			}
			else{}
			
			break;
		}
		case UIID_GROUP_CROSSOVER_V:
		{
			wchar_t buf[16] = {0};
			int slope_id = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V);			                                                                                                                                                                 
			if(channel == 0)
			{
				int freq_0 = m_ConfigHelper->get("crossover0_freq_id");
				swprintf(buf,16, L"%d", slope[slope_id]);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_SLOPE, buf);
				m_AudioControl->setCrossover(0, freq_0, slope_id);
			}
			else if(channel == 1)
			{
				int freq_1 = m_ConfigHelper->get("crossover1_freq_id");
				swprintf(buf,16, L"%d", slope[slope_id]);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_SLOPE, buf);				
				m_AudioControl->setCrossover(1, freq_1, slope_id);
			}
			else if(channel == 2)
			{
				int freq_2 = m_ConfigHelper->get("crossover2_freq_id");
				swprintf(buf,16, L"%d", slope[slope_id]);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_SLOPE, buf);				
				m_AudioControl->setCrossover(2, freq_2, slope_id);				
			}
			else{}

			break;
		}	
		default:
		{
			break;
		}
	}
}

void EqView::onClick_Time_Alignment(long nCtrlId)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	wchar_t text[16] = {0};
	char config_item_name[32] = {0};
	int listening_position = m_ConfigHelper->get("listening_position");

	switch(nCtrlId)
	{
		case UIID_GROUP_TIME_TYPE_L:
		{
			if(listening_position == 0)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 629);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1026, ALL_STATE);
				listening_position = 2;

				wchar_t fl_buf1[16] = {0};
				float m_fl = m_ConfigHelper->getFloat("f_channel0_time_alignment_p2");
				swprintf(fl_buf1,16, L"%.1f",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, fl_buf1);

				wchar_t fl_buf2[16] = {0};
				float m_fr = m_ConfigHelper->getFloat("f_channel1_time_alignment_p2");
				swprintf(fl_buf2,16, L"%.1f",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, fl_buf2);

				wchar_t fl_buf3[16] = {0};
				float m_rl = m_ConfigHelper->getFloat("f_channel2_time_alignment_p2");
				swprintf(fl_buf3,16, L"%.1f",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, fl_buf3);

				wchar_t fl_buf4[16] = {0};
				float m_rr = m_ConfigHelper->getFloat("f_channel3_time_alignment_p2");
				swprintf(fl_buf4,16, L"%.1f",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, fl_buf4);

				wchar_t fl_buf5[16] = {0};
				float m_sub = m_ConfigHelper->getFloat("f_channel5_time_alignment_p2");
				swprintf(fl_buf5,16, L"%.1f",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, fl_buf5);				
			}
			else if(listening_position == 1)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 626);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1025, ALL_STATE);
				listening_position = 4;

				wchar_t all_buf1[16] = {0};
				float m_fl = m_ConfigHelper->getFloat("f_channel0_time_alignment_p4");
				swprintf(all_buf1,16, L"%.1f",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, all_buf1);

				wchar_t all_buf2[16] = {0};
				float m_fr = m_ConfigHelper->getFloat("f_channel1_time_alignment_p4");
				swprintf(all_buf2,16, L"%.1f",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, all_buf2);

				wchar_t all_buf3[16] = {0};
				float m_rl = m_ConfigHelper->getFloat("f_channel2_time_alignment_p4");
				swprintf(all_buf3,16, L"%.1f",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, all_buf3);

				wchar_t all_buf4[16] = {0};
				float m_rr = m_ConfigHelper->getFloat("f_channel3_time_alignment_p4");
				swprintf(all_buf4,16, L"%.1f",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, all_buf4);

				wchar_t all_buf5[16] = {0};
				float m_sub = m_ConfigHelper->getFloat("f_channel5_time_alignment_p4");
				swprintf(all_buf5,16, L"%.1f",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, all_buf5);								
			}
			else if(listening_position == 2)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 628);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1027, ALL_STATE);
				listening_position = 3;	

				wchar_t fr_buf1[16] = {0};
				float m_fl = m_ConfigHelper->getFloat("f_channel0_time_alignment_p3");
				swprintf(fr_buf1,16, L"%.1f",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, fr_buf1);

				wchar_t fr_buf2[16] = {0};
				float m_fr = m_ConfigHelper->getFloat("f_channel1_time_alignment_p3");
				swprintf(fr_buf2,16, L"%.1f",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, fr_buf2);

				wchar_t fr_buf3[16] = {0};
				float m_rl = m_ConfigHelper->getFloat("f_channel2_time_alignment_p3");
				swprintf(fr_buf3,16, L"%.1f",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, fr_buf3);

				wchar_t fr_buf4[16] = {0};
				float m_rr = m_ConfigHelper->getFloat("f_channel3_time_alignment_p3");
				swprintf(fr_buf4,16, L"%.1f",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, fr_buf4);

				wchar_t fr_buf5[16] = {0};
				float m_sub = m_ConfigHelper->getFloat("f_channel5_time_alignment_p3");
				swprintf(fr_buf5,16, L"%.1f",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, fr_buf5);							
			}
			else if(listening_position == 3)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 627);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1030, ALL_STATE);
				listening_position = 1;

				wchar_t f_buf1[16] = {0};
				float m_fl = m_ConfigHelper->getFloat("f_channel0_time_alignment_p1");
				swprintf(f_buf1,16, L"%.1f",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, f_buf1);

				wchar_t f_buf2[16] = {0};
				float m_fr = m_ConfigHelper->getFloat("f_channel1_time_alignment_p1");
				swprintf(f_buf2,16, L"%.1f",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, f_buf2);

				wchar_t f_buf3[16] = {0};
				float m_rl = m_ConfigHelper->getFloat("f_channel2_time_alignment_p1");
				swprintf(f_buf3,16, L"%.1f",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, f_buf3);

				wchar_t f_buf4[16] = {0};
				float m_rr = m_ConfigHelper->getFloat("f_channel3_time_alignment_p1");
				swprintf(f_buf4,16, L"%.1f",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, f_buf4);

				wchar_t f_buf5[16] = {0};
				float m_sub = m_ConfigHelper->getFloat("f_channel5_time_alignment_p1");
				swprintf(f_buf5,16, L"%.1f",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, f_buf5);								
			}
			else if(listening_position == 4)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 630);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1032, ALL_STATE);
				listening_position = 0;

				wchar_t off_buf1[16] = {0};
				swprintf(off_buf1,16, L"%.1f",0.0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, off_buf1);

				wchar_t off_buf2[16] = {0};
				swprintf(off_buf2,16, L"%.1f",0.0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, off_buf2);

				wchar_t off_buf3[16] = {0};
				swprintf(off_buf3,16, L"%.1f",0.0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, off_buf3);

				wchar_t off_buf4[16] = {0};
				swprintf(off_buf4,16, L"%.1f",0.0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, off_buf4);

				wchar_t off_buf5[16] = {0};
				swprintf(off_buf5,16, L"%.1f",0.0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, off_buf5);									
			}
			else{}

			m_AudioControl->setListeningPosition(listening_position);
			break;
		}
		case UIID_GROUP_TIME_TYPE_R:
		{
			if(listening_position == 0)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 626);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1025, ALL_STATE);
				listening_position = 4;

				wchar_t all_buf1[16] = {0};
				float m_fl = m_ConfigHelper->getFloat("f_channel0_time_alignment_p4");
				swprintf(all_buf1,16, L"%.1f",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, all_buf1);

				wchar_t all_buf2[16] = {0};
				float m_fr = m_ConfigHelper->getFloat("f_channel1_time_alignment_p4");
				swprintf(all_buf2,16, L"%.1f",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, all_buf2);

				wchar_t all_buf3[16] = {0};
				float m_rl = m_ConfigHelper->getFloat("f_channel2_time_alignment_p4");
				swprintf(all_buf3,16, L"%.1f",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, all_buf3);

				wchar_t all_buf4[16] = {0};
				float m_rr = m_ConfigHelper->getFloat("f_channel3_time_alignment_p4");
				swprintf(all_buf4,16, L"%.1f",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, all_buf4);

				wchar_t all_buf5[16] = {0};
				float m_sub = m_ConfigHelper->getFloat("f_channel5_time_alignment_p4");
				swprintf(all_buf5,16, L"%.1f",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, all_buf5);				
			}
			else if(listening_position == 1)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 628);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1027, ALL_STATE);
				listening_position = 3;

				wchar_t fr_buf1[16] = {0};
				float m_fl = m_ConfigHelper->getFloat("f_channel0_time_alignment_p3");
				swprintf(fr_buf1,16, L"%.1f",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, fr_buf1);

				wchar_t fr_buf2[16] = {0};
				float m_fr = m_ConfigHelper->getFloat("f_channel1_time_alignment_p3");
				swprintf(fr_buf2,16, L"%.1f",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, fr_buf2);

				wchar_t fr_buf3[16] = {0};
				float m_rl = m_ConfigHelper->getFloat("f_channel2_time_alignment_p3");
				swprintf(fr_buf3,16, L"%.1f",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, fr_buf3);

				wchar_t fr_buf4[16] = {0};
				float m_rr = m_ConfigHelper->getFloat("f_channel3_time_alignment_p3");
				swprintf(fr_buf4,16, L"%.1f",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, fr_buf4);

				wchar_t fr_buf5[16] = {0};
				float m_sub = m_ConfigHelper->getFloat("f_channel5_time_alignment_p3");
				swprintf(fr_buf5,16, L"%.1f",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, fr_buf5);									
			}
			else if(listening_position == 2)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 630);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1032, ALL_STATE);
				listening_position = 0;	

				wchar_t off_buf1[16] = {0};
				swprintf(off_buf1,16, L"%.1f",0.0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, off_buf1);

				wchar_t off_buf2[16] = {0};
				swprintf(off_buf2,16, L"%.1f",0.0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, off_buf2);

				wchar_t off_buf3[16] = {0};
				swprintf(off_buf3,16, L"%.1f",0.0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, off_buf3);

				wchar_t off_buf4[16] = {0};
				swprintf(off_buf4,16, L"%.1f",0.0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, off_buf4);

				wchar_t off_buf5[16] = {0};
				swprintf(off_buf5,16, L"%.1f",0.0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, off_buf5);							
			}
			else if(listening_position == 3)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 629);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1026, ALL_STATE);
				listening_position = 2;

				wchar_t fl_buf1[16] = {0};
				float m_fl = m_ConfigHelper->getFloat("f_channel0_time_alignment_p2");
				swprintf(fl_buf1,16, L"%.1f",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, fl_buf1);

				wchar_t fl_buf2[16] = {0};
				float m_fr = m_ConfigHelper->getFloat("f_channel1_time_alignment_p2");
				swprintf(fl_buf2,16, L"%.1f",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, fl_buf2);

				wchar_t fl_buf3[16] = {0};
				float m_rl = m_ConfigHelper->getFloat("f_channel2_time_alignment_p2");
				swprintf(fl_buf3,16, L"%.1f",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, fl_buf3);

				wchar_t fl_buf4[16] = {0};
				float m_rr = m_ConfigHelper->getFloat("f_channel3_time_alignment_p2");
				swprintf(fl_buf4,16, L"%.1f",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, fl_buf4);

				wchar_t fl_buf5[16] = {0};
				float m_sub = m_ConfigHelper->getFloat("f_channel5_time_alignment_p2");
				swprintf(fl_buf5,16, L"%.1f",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, fl_buf5);									
			}
			else if(listening_position == 4)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 627);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1030, ALL_STATE);
				listening_position = 1;	

				wchar_t f_buf1[16] = {0};
				float m_fl = m_ConfigHelper->getFloat("f_channel0_time_alignment_p1");
				swprintf(f_buf1,16, L"%.1f",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, f_buf1);

				wchar_t f_buf2[16] = {0};
				float m_fr = m_ConfigHelper->getFloat("f_channel1_time_alignment_p1");
				swprintf(f_buf2,16, L"%.1f",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, f_buf2);

				wchar_t f_buf3[16] = {0};
				float m_rl = m_ConfigHelper->getFloat("f_channel2_time_alignment_p1");
				swprintf(f_buf3,16, L"%.1f",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, f_buf3);

				wchar_t f_buf4[16] = {0};
				float m_rr = m_ConfigHelper->getFloat("f_channel3_time_alignment_p1");
				swprintf(f_buf4,16, L"%.1f",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, f_buf4);

				wchar_t f_buf5[16] = {0};
				float m_sub = m_ConfigHelper->getFloat("f_channel5_time_alignment_p1");
				swprintf(f_buf5,16, L"%.1f",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, f_buf5);								
			}
			else{}

			m_AudioControl->setListeningPosition(listening_position);
			break;
		}
		case UIID_GROUP_TIME_FL_L:
		{
			LOGDBG("listening_position = %d--------",listening_position);
			sprintf(config_item_name, "f_channel0_time_alignment_p%d", listening_position);
			float m_fl = m_ConfigHelper->getFloat(config_item_name);
			LOGDBG("m_fl = %.1f---config_item_name = %s",m_fl,config_item_name);
			if(m_fl + FLT_EPSILON >= 1.4 )
			{
				m_fl -= 1.4;
			}
			else
			{
				m_fl = 0.0;
			}
			m_AudioControl->setTimeAlignment(listening_position, 0, m_fl);
			swprintf(text,16, L"%.1f",m_fl);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, text);
			break;
		}
		case UIID_GROUP_TIME_FL_R:
		{
			LOGDBG("listening_position = %d--------",listening_position);
			sprintf(config_item_name, "f_channel0_time_alignment_p%d", listening_position);
			float m_fl = m_ConfigHelper->getFloat(config_item_name);
			LOGDBG("m_fl = %.1f---config_item_name = %s",m_fl,config_item_name);
			if(m_fl == -1.0f)
			{
				m_fl = 0.0;		
			}
			if(m_fl >= 0 && m_fl < 490.0)
			{
				m_fl += 1.4;
			}
			else{}
			m_AudioControl->setTimeAlignment(listening_position, 0, m_fl);
			swprintf(text,16, L"%.1f",m_fl);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, text);
			break;
		}
		case UIID_GROUP_TIME_FR_L:
		{
			sprintf(config_item_name, "f_channel1_time_alignment_p%d", listening_position);
			float m_fr = m_ConfigHelper->getFloat(config_item_name);
			if(m_fr + FLT_EPSILON >= 1.4 )
			{
				m_fr -= 1.4;
			}
			else
			{
				m_fr = 0.0;
			}
			m_AudioControl->setTimeAlignment(listening_position, 1, m_fr);
			swprintf(text,16, L"%.1f",m_fr);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, text);			
			break;
		}
		case UIID_GROUP_TIME_FR_R:
		{
			sprintf(config_item_name, "f_channel1_time_alignment_p%d", listening_position);
			float m_fr = m_ConfigHelper->getFloat(config_item_name);
			if(m_fr == -1.0f)
			{
				m_fr = 0.0;		
			}
			if(m_fr >= 0 && m_fr < 490.0)
			{
				m_fr += 1.4;
			}
			else{}
			m_AudioControl->setTimeAlignment(listening_position, 1, m_fr);
			swprintf(text,16, L"%.1f",m_fr);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, text);			
			break;
		}
		case UIID_GROUP_TIME_RL_L:
		{
			sprintf(config_item_name, "f_channel2_time_alignment_p%d", listening_position);
			float m_rl = m_ConfigHelper->getFloat(config_item_name);
			if(m_rl + FLT_EPSILON >= 1.4 )
			{
				m_rl -= 1.4;
			}
			else
			{
				m_rl = 0.0;
			}
			m_AudioControl->setTimeAlignment(listening_position, 2, m_rl);
			swprintf(text,16, L"%.1f",m_rl);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, text);
			break;
		}
		case UIID_GROUP_TIME_RL_R:
		{
			sprintf(config_item_name, "f_channel2_time_alignment_p%d", listening_position);
			float m_rl = m_ConfigHelper->getFloat(config_item_name);
			if(m_rl == -1.0f)
			{
				m_rl = 0.0;		
			}
			if(m_rl >= 0 && m_rl < 490.0)
			{
				m_rl += 1.4;
			}
			else{}
			m_AudioControl->setTimeAlignment(listening_position, 2, m_rl);
			swprintf(text,16, L"%.1f",m_rl);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, text);
			break;
		}
		case UIID_GROUP_TIME_RR_L:
		{
			sprintf(config_item_name, "f_channel3_time_alignment_p%d", listening_position);
			float m_rr = m_ConfigHelper->getFloat(config_item_name);
			if(m_rr + FLT_EPSILON >= 1.4 )
			{
				m_rr -= 1.4;
			}
			else
			{
				m_rr = 0.0;
			}
			m_AudioControl->setTimeAlignment(listening_position, 3, m_rr);
			swprintf(text,16, L"%.1f",m_rr);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, text);
			break;
		}
		case UIID_GROUP_TIME_RR_R:
		{
			sprintf(config_item_name, "f_channel3_time_alignment_p%d", listening_position);
			float m_rr = m_ConfigHelper->getFloat(config_item_name);
			if(m_rr == -1.0f)
			{
				m_rr = 0.0;		
			}
			if(m_rr >= 0 && m_rr < 490.0)
			{
				m_rr += 1.4;
			}
			else{}
			m_AudioControl->setTimeAlignment(listening_position, 3, m_rr);
			swprintf(text,16, L"%.1f",m_rr);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, text);	
			break;
		}
		case UIID_GROUP_TIME_SUB_L:
		{
			sprintf(config_item_name, "f_channel5_time_alignment_p%d", listening_position);
			float m_sub = m_ConfigHelper->getFloat(config_item_name);
			if(m_sub + FLT_EPSILON >= 1.4 )
			{
				m_sub -= 1.4;
			}
			else
			{
				m_sub = 0.0;
			}
			m_AudioControl->setTimeAlignment(listening_position, 5, m_sub);
			swprintf(text,16, L"%.1f",m_sub);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, text);
			break;
		}
		case UIID_GROUP_TIME_SUB_R:
		{
			sprintf(config_item_name, "f_channel5_time_alignment_p%d", listening_position);
			float m_sub = m_ConfigHelper->getFloat(config_item_name);
			if(m_sub == -1.0f)
			{
				m_sub = 0.0;		
			}
			if(m_sub >= 0 && m_sub < 490.0)
			{
				m_sub += 1.4;
			}
			m_AudioControl->setTimeAlignment(listening_position, 5, m_sub);
			swprintf(text,16, L"%.1f",m_sub);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, text);
			break;
		}
		case UIID_GROUP_TIME_ONOFF:
		{
			int status = m_ConfigHelper->get("time_alignment_on");
			LOGDBG("time_alignment_on = %d",status);

			if(status == 1)
			{
				m_AudioControl->setTimeAlignmentOn(0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_ONOFF, 639);
			}
			else
			{
				m_AudioControl->setTimeAlignmentOn(1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_ONOFF, 638);
			}			
			break;
		}							
		default:
		{
			break;
		}
		
	}
}


void EqView::onClick_Speaker(long nCtrlId)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	wchar_t text[16] = {0};
	char config_item_name[32] = {0};
	int listening_position = m_ConfigHelper->get("listening_position");

	switch(nCtrlId)
	{
		case UIID_GROUP_SPEAKER_TYPE_L:
		{
			if(listening_position == 0)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 629);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1026, ALL_STATE);
				listening_position = 2;

				wchar_t fl_buf1[16] = {0};
				int m_fl = m_ConfigHelper->get("channel0_l2_speaker_level");
				swprintf(fl_buf1,16, L"%d",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, fl_buf1);

				wchar_t fl_buf2[16] = {0};
				int m_fr = m_ConfigHelper->get("channel1_l2_speaker_level");
				swprintf(fl_buf2,16, L"%d",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, fl_buf2);

				wchar_t fl_buf3[16] = {0};
				int m_rl = m_ConfigHelper->get("channel2_l2_speaker_level");
				swprintf(fl_buf3,16, L"%d",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, fl_buf3);

				wchar_t fl_buf4[16] = {0};
				int m_rr = m_ConfigHelper->get("channel3_l2_speaker_level");
				swprintf(fl_buf4,16, L"%d",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, fl_buf4);

				wchar_t fl_buf5[16] = {0};
				int m_sub = m_ConfigHelper->get("channel5_l2_speaker_level");
				swprintf(fl_buf5,16, L"%d",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, fl_buf5);				
			}
			else if(listening_position == 1)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 626);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1025, ALL_STATE);
				listening_position = 4;

				wchar_t all_buf1[16] = {0};
				int m_fl = m_ConfigHelper->get("channel0_l4_speaker_level");
				swprintf(all_buf1,16, L"%d",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, all_buf1);

				wchar_t all_buf2[16] = {0};
				int m_fr = m_ConfigHelper->get("channel1_l4_speaker_level");
				swprintf(all_buf2,16, L"%d",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, all_buf2);

				wchar_t all_buf3[16] = {0};
				int m_rl = m_ConfigHelper->get("channel2_l4_speaker_level");
				swprintf(all_buf3,16, L"%d",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, all_buf3);

				wchar_t all_buf4[16] = {0};
				int m_rr = m_ConfigHelper->get("channel3_l4_speaker_level");
				swprintf(all_buf4,16, L"%d",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, all_buf4);

				wchar_t all_buf5[16] = {0};
				int m_sub = m_ConfigHelper->get("channel5_l4_speaker_level");
				swprintf(all_buf5,16, L"%d",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, all_buf5);								
			}
			else if(listening_position == 2)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 628);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1027, ALL_STATE);
				listening_position = 3;

				wchar_t fr_buf1[16] = {0};
				int m_fl = m_ConfigHelper->get("channel0_l3_speaker_level");
				swprintf(fr_buf1,16, L"%d",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, fr_buf1);

				wchar_t fr_buf2[16] = {0};
				int m_fr = m_ConfigHelper->get("channel1_l3_speaker_level");
				swprintf(fr_buf2,16, L"%d",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, fr_buf2);

				wchar_t fr_buf3[16] = {0};
				int m_rl = m_ConfigHelper->get("channel2_l3_speaker_level");
				swprintf(fr_buf3,16, L"%d",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, fr_buf3);

				wchar_t fr_buf4[16] = {0};
				int m_rr = m_ConfigHelper->get("channel3_l3_speaker_level");
				swprintf(fr_buf4,16, L"%d",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, fr_buf4);

				wchar_t fr_buf5[16] = {0};
				int m_sub = m_ConfigHelper->get("channel5_l3_speaker_level");
				swprintf(fr_buf5,16, L"%d",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, fr_buf5);								
			}
			else if(listening_position == 3)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 627);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1030, ALL_STATE);
				listening_position = 1;

				wchar_t f_buf1[16] = {0};
				int m_fl = m_ConfigHelper->get("channel0_l1_speaker_level");
				swprintf(f_buf1,16, L"%d",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, f_buf1);

				wchar_t f_buf2[16] = {0};
				int m_fr = m_ConfigHelper->get("channel1_l1_speaker_level");
				swprintf(f_buf2,16, L"%d",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, f_buf2);

				wchar_t f_buf3[16] = {0};
				int m_rl = m_ConfigHelper->get("channel2_l1_speaker_level");
				swprintf(f_buf3,16, L"%d",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, f_buf3);

				wchar_t f_buf4[16] = {0};
				int m_rr = m_ConfigHelper->get("channel3_l1_speaker_level");
				swprintf(f_buf4,16, L"%d",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, f_buf4);

				wchar_t f_buf5[16] = {0};
				int m_sub = m_ConfigHelper->get("channel5_l1_speaker_level");
				swprintf(f_buf5,16, L"%d",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, f_buf5);								
			}
			else if(listening_position == 4)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 630);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1032, ALL_STATE);
				listening_position = 0;

				wchar_t off_buf1[16] = {0};
				swprintf(off_buf1,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, off_buf1);

				wchar_t off_buf2[16] = {0};
				swprintf(off_buf2,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, off_buf2);

				wchar_t off_buf3[16] = {0};
				swprintf(off_buf3,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, off_buf3);

				wchar_t off_buf4[16] = {0};
				swprintf(off_buf4,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, off_buf4);

				wchar_t off_buf5[16] = {0};
				swprintf(off_buf5,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, off_buf5);									
			}
			else{}

			m_AudioControl->setListeningPosition(listening_position);
			break;
		}
		case UIID_GROUP_SPEAKER_TYPE_R:
		{
			if(listening_position == 0)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 626);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1025, ALL_STATE);
				listening_position = 4;

				wchar_t all_buf1[16] = {0};
				int m_fl = m_ConfigHelper->get("channel0_l4_speaker_level");
				swprintf(all_buf1,16, L"%d",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, all_buf1);

				wchar_t all_buf2[16] = {0};
				int m_fr = m_ConfigHelper->get("channel1_l4_speaker_level");
				swprintf(all_buf2,16, L"%d",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, all_buf2);

				wchar_t all_buf3[16] = {0};
				int m_rl = m_ConfigHelper->get("channel2_l4_speaker_level");
				swprintf(all_buf3,16, L"%d",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, all_buf3);

				wchar_t all_buf4[16] = {0};
				int m_rr = m_ConfigHelper->get("channel3_l4_speaker_level");
				swprintf(all_buf4,16, L"%d",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, all_buf4);

				wchar_t all_buf5[16] = {0};
				int m_sub = m_ConfigHelper->get("channel5_l4_speaker_level");
				swprintf(all_buf5,16, L"%d",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, all_buf5);				
			}
			else if(listening_position == 1)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 628);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1027, ALL_STATE);
				listening_position = 3;	

				wchar_t fr_buf1[16] = {0};
				int m_fl = m_ConfigHelper->get("channel0_l3_speaker_level");
				swprintf(fr_buf1,16, L"%d",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, fr_buf1);

				wchar_t fr_buf2[16] = {0};
				int m_fr = m_ConfigHelper->get("channel1_l3_speaker_level");
				swprintf(fr_buf2,16, L"%d",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, fr_buf2);

				wchar_t fr_buf3[16] = {0};
				int m_rl = m_ConfigHelper->get("channel2_l3_speaker_level");
				swprintf(fr_buf3,16, L"%d",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, fr_buf3);

				wchar_t fr_buf4[16] = {0};
				int m_rr = m_ConfigHelper->get("channel3_l3_speaker_level");
				swprintf(fr_buf4,16, L"%d",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, fr_buf4);

				wchar_t fr_buf5[16] = {0};
				int m_sub = m_ConfigHelper->get("channel5_l3_speaker_level");
				swprintf(fr_buf5,16, L"%d",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, fr_buf5);							
			}
			else if(listening_position == 2)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 630);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1032, ALL_STATE);
				listening_position = 0;

				wchar_t off_buf1[16] = {0};
				swprintf(off_buf1,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, off_buf1);

				wchar_t off_buf2[16] = {0};
				swprintf(off_buf2,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, off_buf2);

				wchar_t off_buf3[16] = {0};
				swprintf(off_buf3,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, off_buf3);

				wchar_t off_buf4[16] = {0};
				swprintf(off_buf4,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, off_buf4);

				wchar_t off_buf5[16] = {0};
				swprintf(off_buf5,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, off_buf5);								
			}
			else if(listening_position == 3)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 629);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1026, ALL_STATE);
				listening_position = 2;	

				wchar_t fl_buf1[16] = {0};
				int m_fl = m_ConfigHelper->get("channel0_l2_speaker_level");
				swprintf(fl_buf1,16, L"%d",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, fl_buf1);

				wchar_t fl_buf2[16] = {0};
				int m_fr = m_ConfigHelper->get("channel1_l2_speaker_level");
				swprintf(fl_buf2,16, L"%d",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, fl_buf2);

				wchar_t fl_buf3[16] = {0};
				int m_rl = m_ConfigHelper->get("channel2_l2_speaker_level");
				swprintf(fl_buf3,16, L"%d",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, fl_buf3);

				wchar_t fl_buf4[16] = {0};
				int m_rr = m_ConfigHelper->get("channel3_l2_speaker_level");
				swprintf(fl_buf4,16, L"%d",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, fl_buf4);

				wchar_t fl_buf5[16] = {0};
				int m_sub = m_ConfigHelper->get("channel5_l2_speaker_level");
				swprintf(fl_buf5,16, L"%d",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, fl_buf5);							
			}
			else if(listening_position == 4)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 627);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1030, ALL_STATE);
				listening_position = 1;	

				wchar_t f_buf1[16] = {0};
				int m_fl = m_ConfigHelper->get("channel0_l1_speaker_level");
				swprintf(f_buf1,16, L"%d",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, f_buf1);

				wchar_t f_buf2[16] = {0};
				int m_fr = m_ConfigHelper->get("channel1_l1_speaker_level");
				swprintf(f_buf2,16, L"%d",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, f_buf2);

				wchar_t f_buf3[16] = {0};
				int m_rl = m_ConfigHelper->get("channel2_l1_speaker_level");
				swprintf(f_buf3,16, L"%d",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, f_buf3);

				wchar_t f_buf4[16] = {0};
				int m_rr = m_ConfigHelper->get("channel3_l1_speaker_level");
				swprintf(f_buf4,16, L"%d",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, f_buf4);

				wchar_t f_buf5[16] = {0};
				int m_sub = m_ConfigHelper->get("channel5_l1_speaker_level");
				swprintf(f_buf5,16, L"%d",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, f_buf5);								
			}
			else{}
			m_AudioControl->setListeningPosition(listening_position);

			break;
		}
		case UIID_GROUP_SPEAKER_FL_L:
		{
			sprintf(config_item_name, "channel0_l%d_speaker_level", listening_position);
			int m_fl = m_ConfigHelper->get(config_item_name);
			if(m_fl > -24)
			{
				m_fl--;
				m_AudioControl->setSpeakerLevel(listening_position, 0, m_fl);
				swprintf(text,16, L"%d",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, text);
			}
			break;
		}
		case UIID_GROUP_SPEAKER_FL_R:
		{
			sprintf(config_item_name, "channel0_l%d_speaker_level", listening_position);
			int m_fl = m_ConfigHelper->get(config_item_name);
			if(m_fl < 10)
			{
				m_fl++;
				m_AudioControl->setSpeakerLevel(listening_position, 0, m_fl);
				swprintf(text,16, L"%d",m_fl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, text);
			}
			break;
		}
		case UIID_GROUP_SPEAKER_FR_L:
		{
			sprintf(config_item_name, "channel1_l%d_speaker_level", listening_position);
			int m_fr = m_ConfigHelper->get(config_item_name);
			if(m_fr > -24)
			{
				m_fr--;
				m_AudioControl->setSpeakerLevel(listening_position, 1, m_fr);
				swprintf(text,16, L"%d",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, text);
			}
			break;
		}
		case UIID_GROUP_SPEAKER_FR_R:
		{
			sprintf(config_item_name, "channel1_l%d_speaker_level", listening_position);
			int m_fr = m_ConfigHelper->get(config_item_name);
			if(m_fr < 10)
			{
				m_fr++;
				m_AudioControl->setSpeakerLevel(listening_position, 1, m_fr);
				swprintf(text,16, L"%d",m_fr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, text);
			}
			break;
		}
		case UIID_GROUP_SPEAKER_RL_L:
		{
			sprintf(config_item_name, "channel2_l%d_speaker_level", listening_position);
			int m_rl = m_ConfigHelper->get(config_item_name);
			if(m_rl > -24)
			{
				m_rl--;
				m_AudioControl->setSpeakerLevel(listening_position, 2, m_rl);
				swprintf(text,16, L"%d",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, text);
			}
			break;
		}
		case UIID_GROUP_SPEAKER_RL_R:
		{
			sprintf(config_item_name, "channel2_l%d_speaker_level", listening_position);
			int m_rl = m_ConfigHelper->get(config_item_name);
			if(m_rl < 10)
			{
				m_rl++;
				m_AudioControl->setSpeakerLevel(listening_position, 2, m_rl);
				swprintf(text,16, L"%d",m_rl);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, text);
			}
			break;
		}
		case UIID_GROUP_SPEAKER_RR_L:
		{
			sprintf(config_item_name, "channel3_l%d_speaker_level", listening_position);
			int m_rr = m_ConfigHelper->get(config_item_name);
			if(m_rr > -24)
			{
				m_rr--;
				m_AudioControl->setSpeakerLevel(listening_position, 3, m_rr);
				swprintf(text,16, L"%d",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, text);
			}
			break;
		}
		case UIID_GROUP_SPEAKER_RR_R:
		{
			sprintf(config_item_name, "channel3_l%d_speaker_level", listening_position);
			int m_rr = m_ConfigHelper->get(config_item_name);
			if(m_rr < 10)
			{
				m_rr++;
				m_AudioControl->setSpeakerLevel(listening_position, 3, m_rr);
				swprintf(text,16, L"%d",m_rr);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, text);
			}
			break;
		}
		case UIID_GROUP_SPEAKER_SUB_L:
		{
			sprintf(config_item_name, "channel5_l%d_speaker_level", listening_position);
			int m_sub = m_ConfigHelper->get(config_item_name);
			if(m_sub > -24)
			{
				m_sub--;
				m_AudioControl->setSpeakerLevel(listening_position, 5, m_sub);
				swprintf(text,16, L"%d",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, text);
			}

			break;
		}
		case UIID_GROUP_SPEAKER_SUB_R:
		{
			sprintf(config_item_name, "channel5_l%d_speaker_level", listening_position);
			int m_sub = m_ConfigHelper->get(config_item_name);
			if(m_sub < 10)
			{
				m_sub++;
				m_AudioControl->setSpeakerLevel(listening_position, 5, m_sub);
				swprintf(text,16, L"%d",m_sub);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, text);
			}
			break;
		}								
		default:
		{
			break;
		}
		
	}
}

void EqView::onClick_Listening(long nCtrlId)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	if(nCtrlId == UIID_GROUP_LISTENING_FRONT)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_BG, 1030, ALL_STATE);
		m_AudioControl->setListeningPosition(1);
	}
	else if(nCtrlId == UIID_GROUP_LISTENING_FRONTL)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_BG, 1026, ALL_STATE);
		m_AudioControl->setListeningPosition(2);
	}
	else if(nCtrlId == UIID_GROUP_LISTENING_FRONTR)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_BG, 1027, ALL_STATE);
		m_AudioControl->setListeningPosition(3);
	}
	else if(nCtrlId == UIID_GROUP_LISTENING_ALL)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_BG, 1025, ALL_STATE);
		m_AudioControl->setListeningPosition(4);
	}
	else if(nCtrlId == UIID_GROUP_LISTENING_OFF)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_BG, 1032, ALL_STATE);
		m_AudioControl->setListeningPosition(0);
	}
	else{}

} 

void EqView::onClick_LoudnesBtn(long nCtrlId) 
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	map<long, long> m_nLoundnessButs;
	m_nEqData->GetLoundnessButs(m_nLoundnessButs);
    for (const auto& itr : m_nLoundnessButs) 
	{
		if (itr.first == nCtrlId) 
		{
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, itr.first, 907, ALL_STATE);
			UIITEMDATA UItem;
			memset(&UItem,0,sizeof(UItem));
			pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_AUDIOBOOST,UIID_AUDIO_BOOST_LIST,UItem);
			CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;
			pList->SetItemString(1, itr.second, 1);
		   
			if(itr.first == UIID_GROUP_LOUDNES_OFF)
			{
				m_AudioControl->setLoudnes(0);
			}
			else if(itr.first == UIID_GROUP_LOUDNES_LOW)
			{
				m_AudioControl->setLoudnes(1);
			}
			else if(itr.first == UIID_GROUP_LOUDNES_MID)
			{
				m_AudioControl->setLoudnes(2);
			}
			else if(itr.first == UIID_GROUP_LOUDNES_HIGH)
		   {
			   m_AudioControl->setLoudnes(3);
		   }
		   else
		   {

		   }
		   
		}
		else 
		{
           pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, itr.first, 0, ALL_STATE);
		}
	}
}
void EqView::onClick_ReturnBtn()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	bool bMianList_Visble = false;

	bMianList_Visble = pUIMgr->GetUIAnalysisObj()->IsItemVisible(UIID_WNDID_AUDIOBOOST, UIID_GROUP_MAINLIST);
	if(bMianList_Visble)
	{
        GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
	}
	else
	{
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER, false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LOUDNES, false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKERLEVEL, false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING, false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME, false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ, false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SUBFOOER, false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSVOER, false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_MAINLIST, true);
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_AUDIO_BOOST_RETURN, 18);

		UIITEMDATA UItem;
		memset(&UItem,0,sizeof(UItem));
		pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_AUDIOBOOST,UIID_AUDIO_BOOST_LIST,UItem);
		CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;

		int m_fade = m_ConfigHelper->get("fade");
		int m_balance = m_ConfigHelper->get("balance");
		wchar_t text[16] = {0};
		swprintf(text,16, L"F%dR%d",abs(m_fade),abs(m_balance));
		pList->SetItemString(0, text, 1);

		int m_loudness = m_ConfigHelper->get("loudness");
		if(m_loudness == 0)
		{
			pList->SetItemString(1, 608, 1);
		}
		else if(m_loudness == 1)
		{
			pList->SetItemString(1, 610, 1);
		}
		else if(m_loudness == 1)
		{
			pList->SetItemString(1, 611, 1);
		}
		else if(m_loudness == 1)
		{
			pList->SetItemString(1, 612, 1);
		}
		else{}

		int m_eqmode = m_ConfigHelper->get("eq_mode");
		if(m_eqmode == 0)
		{
			pList->SetItemString(5, 620, 1);
		}
		else if(m_eqmode == 1)
		{
			pList->SetItemString(5, 621, 1);
		}
		else if(m_eqmode == 2)
		{
			pList->SetItemString(5, 622, 1);
		}
		else if(m_eqmode == 3)
		{
			pList->SetItemString(5, 615, 1);
		}
		else if(m_eqmode == 4)
		{
			pList->SetItemString(5, 547, 1);
		}
		else if(m_eqmode == 5)
		{
			pList->SetItemString(5, 623, 1);
		}
		else{}

		int m_listening = m_ConfigHelper->get("listening_position");
		if(m_listening == 0)
		{
			pList->SetItemString(3, 630, 1);
		}
		else if(m_listening == 1)
		{
			pList->SetItemString(3, 627, 1);
		}
		else if(m_listening == 2)
		{
			pList->SetItemString(3, 634, 1);
		}
		else if(m_listening == 3)
		{
			pList->SetItemString(3, 635, 1);
		}
		else if(m_listening == 4)
		{
			pList->SetItemString(3, 636, 1);
		}
		else{}

		int status = m_ConfigHelper->get("time_alignment_on");
		if(status == 1)
		{
			pList->SetItemString(4, 609, 1);
		}
		else
		{
			pList->SetItemString(4, 608, 1);
		}
				
	}

}

void EqView::onClick_SettingList()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	CUIAnalysis *pUI = NULL;
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	else
	{
		pUI = pUIMgr->GetUIAnalysisObj();
	}
	

	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));
	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_AUDIOBOOST,UIID_AUDIO_BOOST_LIST,UItem);
	CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;

	/*UIITEMDATA data;
	memset(&data,0,sizeof(data));
	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_AUDIOBOOST,UIID_GROUP_EQ_CONT,data);
	CUI_BtList *mList = (CUI_BtList*) data.m_pCtrl;*/

	if(NULL == pList)
	{
		LOGDBG("jgwen--- onClick_SettingList");
		return;
	}

	int nLine = 0;
	int ItemData = 0;
	int nCol = 0;

	pList->GetCurSel(nLine,ItemData,nCol);

	switch(nLine)
	{
		case 0:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_MAINLIST,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER,true);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_AUDIO_BOOST_RETURN, 600);
			pUIMgr->GetUIAnalysisObj()->SetCursorMax(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_CURSOR, 30, 30);

			int m_fade = m_ConfigHelper->get("fade");
			int m_balance = m_ConfigHelper->get("balance");
			wchar_t text1[16] = {0};
			wchar_t text2[16] = {0};
			pUIMgr->GetUIAnalysisObj()->SetCursorPos(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_CURSOR, m_fade + 15, m_balance + 15);

			if(m_fade > 0 && m_fade <= 15)
			{
				swprintf(text1,16, L"%d",m_fade);
				swprintf(text2,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_FRONT_VALUE, text1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_REAR_VALUE, text2);
			}
			else if(m_fade >= -15 && m_fade < 0)
			{
				swprintf(text1,16, L"%d",abs(m_fade));
				swprintf(text2,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_FRONT_VALUE, text2);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_REAR_VALUE, text1);
			}
			else
			{
				swprintf(text1,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_FRONT_VALUE, text1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_REAR_VALUE, text1);
			}

			if(m_balance > 0 && m_balance <= 15)
			{
				swprintf(text1,16, L"%d",m_balance);
				swprintf(text2,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_RIGHT_VALUE, text1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_LEFT_VALUE, text2);
			}
			else if(m_balance >= -15 && m_balance < 0)
			{
				swprintf(text1,16, L"%d",abs(m_balance));
				swprintf(text2,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_RIGHT_VALUE, text2);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_LEFT_VALUE, text1);
			}
			else
			{
				swprintf(text1,16, L"%d",0);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_RIGHT_VALUE, text1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_FADER_LEFT_VALUE, text1);
			}			

		}
		break;
		case 1:
		{
			int m_loudness = m_ConfigHelper->get("loudness");

			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_MAINLIST,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LOUDNES,true);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_AUDIO_BOOST_RETURN, 601);

			if(m_loudness == 0)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LOUDNES_OFF, 907, ALL_STATE);
			}
			else if(m_loudness == 1)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LOUDNES_LOW, 907, ALL_STATE);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LOUDNES_OFF, 0, ALL_STATE);
			}
			else if(m_loudness == 2)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LOUDNES_MID, 907, ALL_STATE);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LOUDNES_OFF, 0, ALL_STATE);
			}			
			else if(m_loudness == 3)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LOUDNES_HIGH, 907, ALL_STATE);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LOUDNES_OFF, 0, ALL_STATE);
			}
			else{}			
		}
		break;
		case 2:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_MAINLIST,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKERLEVEL,true);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_AUDIO_BOOST_RETURN, 602);

			int m_listening = m_ConfigHelper->get("listening_position");
			if(m_listening == 0)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 630);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1032, ALL_STATE);
			}
			else if(m_listening == 1)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 627);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1030, ALL_STATE);
			}
			else if(m_listening == 2)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 629);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1026, ALL_STATE);
			}
			else if(m_listening == 3)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 628);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1027, ALL_STATE);
			}
			else if(m_listening == 4)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_TYPE, 626);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_BG, 1025, ALL_STATE);
			}
			else{}

			wchar_t text0[16] = {0};
			char config_item_name1[32] = {0};
			sprintf(config_item_name1, "channel0_l%d_speaker_level", m_listening);
			int m_speaker0 = m_ConfigHelper->get(config_item_name1);
			swprintf(text0,16, L"%d",m_speaker0);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FL_V, text0);

			wchar_t text1[16] = {0};
			char config_item_name2[32] = {0};
			sprintf(config_item_name2, "channel1_l%d_speaker_level", m_listening);
			int m_speaker1 = m_ConfigHelper->get(config_item_name2);
			swprintf(text1,16, L"%d",m_speaker1);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_FR_V, text1);

			wchar_t text2[16] = {0};
			char config_item_name3[32] = {0};
			sprintf(config_item_name3, "channel2_l%d_speaker_level", m_listening);			
			int m_speaker2 = m_ConfigHelper->get(config_item_name3);
			swprintf(text2,16, L"%d",m_speaker2);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RL_V, text2);

			wchar_t text3[16] = {0};
			char config_item_name4[32] = {0};
			sprintf(config_item_name4, "channel3_l%d_speaker_level", m_listening);				
			int m_speaker3 = m_ConfigHelper->get(config_item_name4);
			swprintf(text3,16, L"%d",m_speaker3);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_RR_V, text3);

			wchar_t text5[16] = {0};
			char config_item_name5[32] = {0};
			sprintf(config_item_name5, "channel5_l%d_speaker_level", m_listening);			
			int m_speaker5 = m_ConfigHelper->get(config_item_name5);
			swprintf(text5,16, L"%d",m_speaker5);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SPEAKER_SUB_V, text5);			
		}
		break;
		case 3:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_MAINLIST,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING,true);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_AUDIO_BOOST_RETURN, 603);
			pUI->EnableItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_BG, false);

			int m_listening = m_ConfigHelper->get("listening_position");
			if(m_listening == 0)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_BG, 1032, ALL_STATE);
				pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_OFF, ITEM_STATE_SELECTED);
			}
			else if(m_listening == 1)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_BG, 1030, ALL_STATE);
				pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_FRONT, ITEM_STATE_SELECTED);
			}
			else if(m_listening == 2)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_BG, 1026, ALL_STATE);
				pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_FRONTL, ITEM_STATE_SELECTED);
			}
			else if(m_listening == 3)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_BG, 1027, ALL_STATE);
				pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_FRONTR, ITEM_STATE_SELECTED);
			}
			else if(m_listening == 4)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_BG, 1025, ALL_STATE);
				pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_LISTENING_ALL, ITEM_STATE_SELECTED);
			}
			else{}
		}
		break;
		case 4:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_MAINLIST,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME,true);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_AUDIO_BOOST_RETURN, 604);

			int m_listening = m_ConfigHelper->get("listening_position");
			if(m_listening == 0)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 630);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1032, ALL_STATE);
			}
			else if(m_listening == 1)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 627);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1030, ALL_STATE);
			}
			else if(m_listening == 2)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 629);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1026, ALL_STATE);
			}
			else if(m_listening == 3)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 628);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1027, ALL_STATE);
			}
			else if(m_listening == 4)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_TYPE, 626);
				pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_BG, 1025, ALL_STATE);
			}
			else{}

			wchar_t text0[16] = {0};
			char config_item_name1[32] = {0};
			sprintf(config_item_name1, "f_channel0_time_alignment_p%d", m_listening);			
			float m_speaker0 = m_ConfigHelper->getFloat(config_item_name1);
			swprintf(text0,16, L"%.1f",m_speaker0);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FL_V, text0);

			wchar_t text1[16] = {0};
			char config_item_name2[32] = {0};
			sprintf(config_item_name2, "f_channel1_time_alignment_p%d", m_listening);
			float m_speaker1 = m_ConfigHelper->getFloat(config_item_name2);
			swprintf(text1,16, L"%.1f",m_speaker1);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_FR_V, text1);

			wchar_t text2[16] = {0};
			char config_item_name3[32] = {0};
			sprintf(config_item_name3, "f_channel2_time_alignment_p%d", m_listening);			
			float m_speaker2 = m_ConfigHelper->getFloat(config_item_name3);
			swprintf(text2,16, L"%.1f",m_speaker2);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RL_V, text2);

			wchar_t text3[16] = {0};
			char config_item_name4[32] = {0};
			sprintf(config_item_name4, "f_channel3_time_alignment_p%d", m_listening);
			float m_speaker3 = m_ConfigHelper->getFloat(config_item_name4);
			swprintf(text3,16, L"%.1f",m_speaker3);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_RR_V, text3);

			wchar_t text5[16] = {0};
			char config_item_name5[32] = {0};
			sprintf(config_item_name5, "f_channel5_time_alignment_p%d", m_listening);			
			float m_speaker5 = m_ConfigHelper->getFloat(config_item_name5);
			swprintf(text5,16, L"%.1f",m_speaker5);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_SUB_V, text5);

			int status = m_ConfigHelper->get("time_alignment_on");
			if(status == 1)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_ONOFF, 638);
			}
			else
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_TIME_ONOFF, 639);
			}
		}
		break;
		case 5:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_MAINLIST,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_EQ,true);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_AUDIO_BOOST_RETURN, 605);
			int line = m_ConfigHelper->get("eq_mode");
			//mList->SetCurSel(line, 0);

			// if(line == 0)
			// {
			// 	int tmp[] = {8, 8, 6, 6, 2, 0, 0, 0, 2, 4, 4};
			// 	for(int i = 0; i < 11; i++)
			// 	{
			// 		pUI->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,24,tmp[i]+12);
			// 		pUI->EnableItem(UIID_WNDID_AUDIOBOOST, 1803+i, false);
			// 	}
			// }
			// else if(line == 1)
			// {
			// 	int tmp[] = {8, 8, 6, 6, 0, -2, -2, -2, 0, 2, 2, 4};
			// 	for(int i = 0; i < 12; i++)
			// 	{
			// 		pUI->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,24,tmp[i]+12);
			// 		pUI->EnableItem(UIID_WNDID_AUDIOBOOST, 1803+i, false);
			// 	}
			// }
			// else{}
			if(line >= 0 && line <= 4)
			{
				int *gainArray = adEqInfo[line].eq_gain_array;
				for(int i = 0; i < adEqInfo[line].eq_freq_cnt; i++)
				{
					pUI->SetProgressItemValues(UIID_WNDID_AUDIOBOOST,1803+i,12,(gainArray[i]+12)/2);
	//				pUI->EnableItem(UIID_WNDID_AUDIOBOOST, 1803+i, false);				
				}
			}
			else
			{
				int tmp[] = {2, 4, -2, -10, 2, 0, 0, 2, 2, 4, 2, 4, 6};
				for(int i = 0; i < 13; i++)
				{
					pUI->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, 1803+i, 12, (tmp[i]+12)/2);
				}
			}
			

			for(int i = 0; i < 13; i++)
			{
				pUIMgr->GetUIAnalysisObj()->SetItemParameter(UIID_WNDID_AUDIOBOOST, 1803+i, CTRL_EXTPROCESS_BAR_MODE, (void*)&EXTPROCESS_BAR_SLIDER);
			}	
		}
		break;
		case 6:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_MAINLIST,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_SUBFOOER,true);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_AUDIO_BOOST_RETURN, 606);
		}
		break;
		case 7:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_MAINLIST,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSVOER,true);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_AUDIO_BOOST_RETURN, 607);
			pUIMgr->GetUIAnalysisObj()->SetItemParameter(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, CTRL_EXTPROCESS_BAR_MODE, (void*)&EXTPROCESS_BAR_SLIDER);
			pUIMgr->GetUIAnalysisObj()->SetItemParameter(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, CTRL_EXTPROCESS_BAR_MODE, (void*)&EXTPROCESS_BAR_SLIDER);

			int channel = m_ConfigHelper->get("crossover_channel");
			wchar_t buf1[16] = {0};
			wchar_t buf2[16] = {0};
			if(channel == 0)
			{
				int nfreq = m_ConfigHelper->get("crossover0_freq_id");
				int nslope = m_ConfigHelper->get("crossover0_slope_id");
				int nOnOff = m_ConfigHelper->get("crossover0");
				if(nfreq == 1)
				{
					swprintf(buf1,16, L"%.1f",31.5);
				}
				else
				{
					swprintf(buf1,16, L"%d",freq[nfreq]);
				}
				swprintf(buf2,16, L"%d",slope[nslope]);

				if(nOnOff == 0)
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 641);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_NORMAL);
				}
				else
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 720);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_SELECTED);
				}
				pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, 10, nfreq);
				pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, 3, nslope);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_HPF, buf1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_SLOPE, buf2);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_TYPE, 627);
			}
			else if(channel == 1)
			{
				int nfreq = m_ConfigHelper->get("crossover1_freq_id");
				int nslope = m_ConfigHelper->get("crossover1_slope_id");
				int nOnOff = m_ConfigHelper->get("crossover1");
				if(nfreq == 1)
				{
					swprintf(buf1,16, L"%.1f",31.5);
				}
				else
				{
					swprintf(buf1,16, L"%d",freq[nfreq]);
				}
				swprintf(buf2,16, L"%d",slope[nslope]);

				if(nOnOff == 0)
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 641);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_NORMAL);
				}
				else
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 720);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_SELECTED);
				}
				pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, 10, nfreq);
				pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, 3, nslope);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_HPF, buf1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_SLOPE, buf2);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_TYPE, 653);
			}
			else if(channel == 2)
			{
				int nfreq = m_ConfigHelper->get("crossover2_freq_id");
				int nslope = m_ConfigHelper->get("crossover2_slope_id");
				int nOnOff = m_ConfigHelper->get("crossover2");
				if(nfreq == 1)
				{
					swprintf(buf1,16, L"%.1f",31.5);
				}
				else
				{
					swprintf(buf1,16, L"%d",freq[nfreq]);
				}
				swprintf(buf2,16, L"%d",slope[nslope]);

				if(nOnOff == 0)
				{
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 721);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_NORMAL);
				}
				else
				{					
					pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, 642);
					pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_ONOFF, ITEM_STATE_SELECTED);
				}
				pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_H, 10, nfreq);
				pUIMgr->GetUIAnalysisObj()->SetProgressItemValues(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_V, 5, nslope);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_HPF, buf1);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_SLOPE, buf2);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_AUDIOBOOST, UIID_GROUP_CROSSOVER_TYPE, 606);
			}
			else{}


		}
		break;
		default:
		break;
	}
}

void EqView::EqOnComponent(CUIWndMng *pUI, PWINDOW_INFO pInfo)
{
	LOGDBG("jgwen--- pInfo.nInfoType=%d,nWindowID=%d,nCtrlID=%d ,status=%d\n",\
			pInfo->nInfoType, pInfo->nWindowID, pInfo->nCtrlID, pInfo->nOperationStatus);

	//int nWndID = pInfo->nWindowID;
	//int nCtrlID = pInfo->nCtrlID;
	//int nStatus = pInfo->nOperationStatus;
	//int nWndType = pInfo->nInfoType;
}

void EqView::EqOnListInfoMessage(CUIWndMng *pUI, int nWndId, int nCtrlId, PLIST_INFO pInfo)
{
	//LOGDBG("EqView ListInfo: nWndId=%d, nCtrlId=%d \n", nWndId, nCtrlId);
}

void EqView::EqInitWorkWnd()
{
	//update
	OnInit_EqView();
}

void EqView::OnInit_EqView()
{
	//初始化界面
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));
	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_AUDIOBOOST,UIID_AUDIO_BOOST_LIST,UItem);
	CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;

	int m_nfader = m_ConfigHelper->get("fade");
	int m_nbalance = m_ConfigHelper->get("balance");
	wchar_t fader[16] = {0};
	swprintf(fader,16, L"F%dR%d", abs(m_nfader), abs(m_nbalance));
	pList->SetItemString(0, fader, 1);

	int m_loudness = m_ConfigHelper->get("loudness");
	if(m_loudness == 0)
	{
		pList->SetItemString(1, 608, 1);
	}
	else if(m_loudness == 1)
	{
		pList->SetItemString(1, 610, 1);
	}
	else if(m_loudness == 2)
	{
		pList->SetItemString(1, 611, 1);
	}
	else if(m_loudness == 3)
	{
		pList->SetItemString(1, 612, 1);
	}
	else{}
	
	wchar_t buf[16] = {0};
	swprintf(buf,16, L"%s"," ");
	pList->SetItemString(2, buf, 1);

	int m_listening = m_ConfigHelper->get("listening_position");
	if(m_listening == 0)
	{
		pList->SetItemString(3, 630, 1);
	}
	else if(m_listening == 1)
	{
		pList->SetItemString(3, 627, 1);
	}
	else if(m_listening == 2)
	{
		pList->SetItemString(3, 634, 1);
	}
	else if(m_listening == 3)
	{
		pList->SetItemString(3, 635, 1);
	}
	else if(m_listening == 4)
	{
		pList->SetItemString(3, 636, 1);
	}
	else{}

	int status = m_ConfigHelper->get("time_alignment_on");
	if(status == 1)
	{
		pList->SetItemString(4, 609, 1);
	}
	else
	{
		pList->SetItemString(4, 608, 1);
	}

	int m_eqmode = m_ConfigHelper->get("eq_mode");
	if(m_eqmode == 0)
	{
		pList->SetItemString(5, 620, 1);
	}
	else if(m_eqmode == 1)
	{
		pList->SetItemString(5, 621, 1);
	}
	else if(m_eqmode == 2)
	{
		pList->SetItemString(5, 622, 1);
	}
	else if(m_eqmode == 3)
	{
		pList->SetItemString(5, 615, 1);
	}
	else if(m_eqmode == 4)
	{
		pList->SetItemString(5, 547, 1);
	}
	else if(m_eqmode == 5)
	{
		pList->SetItemString(5, 623, 1);
	}
	else{}	

	int  Subwoofer = m_ConfigHelper->get("subwoofer_on");
	if(Subwoofer == 0)
	{
		pList->SetItemString(6, 608, 1);
	}
	else
	{
		pList->SetItemString(6, 608, 1);
	}

	

}

void EqView::EqOnShow(LONG dwWndId)
{
	LOGDBG("jgwen--- OnShow Wnd ID Is %d", dwWndId);
	// EnableAlphaBlending(true);
	SetSetCurrWnd(dwWndId);
}

void EqView::OnTimer(uint32_t id)
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


void EqView::DrawAllItem()
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

void EqView::SetSetCurrWnd(int nWndID)
{
	Lock();
	m_nSetCurrWnd = nWndID;
	SettingData &mData = SettingData::GetInstance();
	mData.SetCurrWnd(nWndID);
	UnLock();
}

int EqView::GetSetCurrWnd()
{
	int nWndID  = 0;
	Lock();
	nWndID = m_nSetCurrWnd;
	UnLock();
	return nWndID;
}

