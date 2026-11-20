/*
 * MediaView.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include "MediaView.h"
#include "UIID_Def_Media.h"
#include "AssertDefine.h"

#include "IVIView.h"

// Dependent on external header files
#include <math.h>
#include <iconv.h>
#include <errno.h>
#include <wchar.h>

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

MediaView::MediaView(ViewBase* p_ViewBase)
: CompBase(p_ViewBase)
, m_nCurrWnd(0)
, m_ViewController(NULL)
{
	m_nCurUIfrome = UI_FROM_IDLE;
	m_bLoadFinish = false;
	m_nUsbInsert = 0;

	m_nPlayAudio_pos = 0;
	m_nPlayVideo_pos = 0;
	m_nPlayAuido_Total = 0;
	m_nPlayVideo_Total = 0;

	m_nCurPlayType = PLAY_TYPE_IDLE;

	m_nCurPlayStatus = Player_Idle;

	memset(m_strPlayTitle,0,sizeof(m_strPlayTitle));
	memset(m_strPlayArtist,0,sizeof(m_strPlayArtist));
	memset(m_strPlayAlbum,0,sizeof(m_strPlayAlbum));

	m_nPlayMode = PLAY_MODE_NORMAL;
	m_bPlayShuffle = false;

	m_nPlayMode_Video = PLAY_MODE_NORMAL;

	m_bAudioList_Update = false;
	m_bVideoList_Update = false;

	memset(m_PrePlayAudioUrl,0,sizeof(m_PrePlayAudioUrl));

	//m_SPMetaData = SPMetaData::create("SpMetadata");

	m_SPMetaData = NULL;

	// TODO Auto-generated constructor stub
	MediaInitUpdateMsgMap();

	SetTimer(TIMER_UPDATE_TIME, 1000);
}

MediaView::~MediaView()
{
	// TODO Auto-generated destructor stub

	if(NULL != m_SPMetaData)
	{
		SPMetaData::destroy(m_SPMetaData);

		m_SPMetaData = NULL;
	}
}

void MediaView::MediaInitUpdateMsgMap()
{
	m_MsgMap.clear();	
	/*
 	*顶栏*/
	//m_MsgMap.insert(make_pair(METER_VMSG_BT_MUSICPLAY_STATE, &HomeView::UpdateBTMusicPlay));
	//m_MsgMap.insert(make_pair(METER_VMSG_BT_MUSIC_INFO, &HomeView::UpdateBTMusicInfo));

	m_MsgMap.insert(make_pair(SYS_VMSG_USBMOUNT, &MediaView::OnHandle_Mount));
	m_MsgMap.insert(make_pair(MEDIA_VMSG_PLAYSTATUS, &MediaView::OnPlayUrl_Status));	
	m_MsgMap.insert(make_pair(MEDIA_VMSG_PLAYTIME, &MediaView::OnPlayUrl_Time));
}

void MediaView::MediaGetUpdateMessageMap(map<int, pUpdateMediaMsg> &nMap)
{
	nMap = m_MsgMap;
}

bool MediaView::MediaOnUserMessage(T_APP_MSG *msg)
{
	if (nullptr == msg)
	{
		return false;
	}
	//LOGDBG("MediaOnUserMessage pthread_ID = %d",(int)pthread_self());

	LOGDBG("Message = %d,param0=%d,param1=%d,param2=%d\n",
		   msg->Message, msg->Param0, msg->Param1, msg->Param2);
	map<int, pUpdateMediaMsg> nUpdateMap;
	nUpdateMap.clear();
	MediaGetUpdateMessageMap(nUpdateMap);
	auto Itr = nUpdateMap.find(msg->Message);
	if (Itr != nUpdateMap.end())
	{
		(this->*(Itr->second))(msg);
	}

	return true;
}

void MediaView::MediaOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	LOGDBG("nWndId=%d,nCtrlId=%d,nBtState=%d",nWndId,nCtrlId,nBtState);
	if(1 == nBtState)
	{
		MediaUpdateHomeContenInfo(nWndId,nCtrlId,nBtState,1);
	}
}

void MediaView::MediaOnComponent(CUIWndMng *pUI, PWINDOW_INFO pInfo)
{
	// LOGDBG("pInfo.nInfoType=%d,nWindowID=%d,nCtrlID=%d ,status=%d\n",\
	// 		pInfo->nInfoType, pInfo->nWindowID, pInfo->nCtrlID, pInfo->nOperationStatus);
}

void MediaView::OnMedia_WinShow()
{
	if(m_nUsbInsert)
	{
		if(m_bLoadFinish)
		{
			if((m_FileList.GetAudioFileNum() > 0) || (m_FileList.GetVideoFileNum() > 0))
			{
				if(PLAY_TYPE_AUDIO == m_nCurPlayType)
				{
					//show audio play
					GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_AUDIO_PLAY, true);
				}
				else if(PLAY_TYPE_VIDEO == m_nCurPlayType)
				{
					GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_VIDEO_PLAY, true);
				}
				else
				{
					//todo
					if(m_FileList.GetAudioFileNum() > 0)
					{
						m_nCurUIfrome = UI_FROM_AUDIO_PLAY;

						GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_AUDIO_PLAY, true);
						//播放音频
						OnPlay_Audio();
					}
					else if(m_FileList.GetVideoFileNum() > 0)
					{
						m_nCurUIfrome = UI_FROM_VIDEO_PLAY;

						GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_VIDEO_PLAY, true);
						//播放视频
						OnPlay_Video();
					}
				}
			}
			else
			{
				//显示无媒体文件
				GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_FAIL, true);
			}
		}
		else
		{
			m_nCurUIfrome = UI_FROM_LOADING;
			//显示loading界面
			GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_LOADING, true);
		}
	}
	else
	{
		//未能插入U盘
		GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_FAIL, true);
	}
}

bool MediaView::OnHandle_Mount(T_APP_MSG* msg)
{
	bool bRet = false;

	int nCurWnd = GetCurrWnd();
	bool bInMediaWnd = (UIID_WNDID_MEDIA_AUDIO_PLAY == nCurWnd) ||
	                   (UIID_WNDID_MEDIA_AUDIO_LIST == nCurWnd) ||
	                   (UIID_WNDID_MEDIA_VIDEO_PLAY == nCurWnd) ||
	                   (UIID_WNDID_MEDIA_VIDEO_LIST == nCurWnd) ||
	                   (UIID_WNDID_MEDIA_LOADING    == nCurWnd) ||
	                   (UIID_WNDID_MEDIA_FAIL       == nCurWnd);
	if(0 == msg->Param0)
	{
		//USB remove
		m_FileList.DelFileList();

		m_nUsbInsert = 0;

		m_bVideoList_Update = false;
		m_bAudioList_Update = false;

		T_PLAYMEDIA_CMD tmpCmd;
		tmpCmd.nPlayCmd = PLAY_CMD_STOP;
		tmpCmd.nParam = 0;
		m_ViewController->SetMediaPlay_Cmd(tmpCmd);

		m_bLoadFinish = false;

		GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
	}
	else if(1 == msg->Param0)
	{
		//USB insert
		m_FileList.DelFileList();

		m_nUsbInsert = 1;

		//MediaData &mData = MediaData::GetInstance();
		//char strMount[128] = {0};
		//mData.GetMountPath(strMount,128);

		//LOGDBG("&&&---OnHandle_Mount--strMount:%s\n",strMount);
		//search files
		//m_FileList.Search_AllDirectory(strMount);

		if ((bInMediaWnd) || (UIID_WNDID_MAIN == nCurWnd))
		{
			GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_LOADING, true);

			m_nCurUIfrome = UI_FROM_LOADING;
		}

		m_nCurPlayType = PLAY_TYPE_IDLE;

		// 后台扫描文件，扫描完成后进媒体页由 OnMedia_WinShow 决定显示播放页还是无媒体页
		KillTimer(TIMER_SEARCH_FILES);
		SetTimer(TIMER_SEARCH_FILES, 1000);
	}

	return bRet;
}
bool MediaView::OnPlayUrl_Time(T_APP_MSG* msg)
{
	LOGDBG("&&&----pos:%d,total:%d,playtype:%d",msg->Param0,msg->Param1,msg->Param2);

	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return false;
	}

	if(PLAY_TYPE_AUDIO == msg->Param2)
	{
		m_nPlayAudio_pos = msg->Param0;

		//m_nPlayAuido_Total = msg->Param1;

        //更新总播放时间
		int nMin = m_nPlayAuido_Total / 60;
		int sec = m_nPlayAuido_Total % 60;
		wchar_t szTip[MAX_FILE_LEN] = {0};
		swprintf(szTip,MAX_FILE_LEN,L"%02d:%02d",nMin,sec);

		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_TOTALTIME, szTip);
	}
	else if(PLAY_TYPE_VIDEO == msg->Param2)
	{
		m_nPlayVideo_pos = msg->Param0;
		
        //更新总播放时间
		int nMin = m_nPlayVideo_Total / 60;
		int sec = m_nPlayVideo_Total % 60;
		wchar_t szTip[MAX_FILE_LEN] = {0};
		swprintf(szTip,MAX_FILE_LEN,L"%02d:%02d",nMin,sec);

		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_TOTALTIME, szTip);		
	}

	return true;
}
bool MediaView::OnPlayUrl_Status(T_APP_MSG* msg)
{
	bool bRet = false;
    char tmpUrl[MAX_FILE_LEN] = {0};
	MediaData &mData = MediaData::GetInstance();

	if(PLAY_TYPE_AUDIO == msg->Param1)
	{
		m_nCurPlayType = PLAY_TYPE_AUDIO;

		mData.GetCurPlayUrl(tmpUrl);

		if(0 != strcmp(m_PrePlayAudioUrl,tmpUrl))
		{
			strcpy(m_PrePlayAudioUrl,tmpUrl);

			//解析ID3
			OnPasePlayID3(tmpUrl);

			//update UI
			OnUpdate_PlayUI(tmpUrl,msg->Param0);
		}

		switch(msg->Param0)
		{
			case Player_Play:
			{
				m_nCurPlayStatus = Player_Play;

				OnUpdate_PlayPause(m_nCurPlayStatus);
			}
			break;
			case Player_Pause:
			{
				m_nCurPlayStatus = Player_Pause;

				OnUpdate_PlayPause(m_nCurPlayStatus);
			}
			break;
			case Player_Finish:
			{
				m_nCurPlayStatus = Player_Finish;
				OnPlay_Next();
			}
			break;
			case Player_Error:
			{
				m_nCurPlayStatus = Player_Error;
				OnPlay_Next();
			}
			break;
			case Player_Initialized:
			{
				OnReset_Play(PLAY_TYPE_AUDIO);
			}
			break;
			default:
			break;
		}
	}
	else if(PLAY_TYPE_VIDEO == msg->Param1)
	{
		m_nCurPlayType = PLAY_TYPE_VIDEO;
		m_nCurUIfrome = UI_FROM_VIDEO_PLAY;

		mData.GetCurPlayUrl(tmpUrl);
		if(0 != strcmp(m_PrePlayVideoUrl,tmpUrl))
		{
			strcpy(m_PrePlayVideoUrl,tmpUrl);

			//获取总时间
			m_nPlayVideo_Total = OnPase_TotalTime(tmpUrl);
			//update UI
			OnUpdate_PlayUI_Video(tmpUrl,msg->Param0);
		}
		switch(msg->Param0)
		{
			case Player_Play:
			{
				m_nCurPlayStatus = Player_Play;

				OnUpdate_PlayPause_Video(m_nCurPlayStatus);
			}
			break;
			case Player_Pause:
			{
				m_nCurPlayStatus = Player_Pause;

				OnUpdate_PlayPause_Video(m_nCurPlayStatus);
			}
			break;
			case Player_Finish:
			{
				m_nCurPlayStatus = Player_Finish;
				OnPlay_Next_Video();
			}
			break;
			case Player_Error:
			{
				UpdateUI_PlayVideo_Error();
				m_nCurPlayStatus = Player_Error;

				SetTimer(TIMER_PLAYFAIL_NEXT, 3000);
				//OnPlay_Next_Video();
			}
			break;
			case Player_Initialized:
			{
				OnReset_Play(PLAY_TYPE_VIDEO);
			}
			break;
			default:
			break;
		}
	}

	return bRet;
}
int MediaView::OnPase_TotalTime(char *pPlayUrl)
{
	int nReTime = 0;

	if(NULL == m_SPMetaData)
	{
		m_SPMetaData = SPMetaData::create("SpMetadata");
	}

	if(NULL != m_SPMetaData)
	{
		int nRet = m_SPMetaData->setUrl(pPlayUrl);

		if(0 != nRet)
		{
			LOGDBG("&&&---m_SPMetaData setUrl--fail");
		}

		//获取播放时间
		long long Total_Time = 0;
		Total_Time = m_SPMetaData->duration();
		nReTime = Total_Time / 1000;		
	}

	return nReTime;
}

void MediaView::OnPasePlayID3(char *pPlayUrl)
{
	LOGDBG("&&&----playfile:%s",pPlayUrl);
	int nRet = -1;

	char* song = NULL;
	char* artist = NULL;
	char* album = NULL;

	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	if(NULL == m_SPMetaData)
	{
		m_SPMetaData = SPMetaData::create("SpMetadata");
	}
    
	if(NULL != m_SPMetaData)
	{
		LOGDBG("&&&--0000-----");
		nRet = m_SPMetaData->setUrl(pPlayUrl);
		LOGDBG("&&&--0000---nRet:%d",nRet);
		if(0 != nRet)
		{
			LOGDBG("&&&---m_SPMetaData setUrl--fail");
		}
        /*song = m_SPMetaData->metadata("title_v1");
		if(NULL != song)
		{
			LOGDBG("&&&---title_v1:%s",song);
			//gui update
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_TITLE, L"zxwzxw");
		}*/
		song = m_SPMetaData->metadata("title");
		if (NULL != song && song[0] != '\0')
		{
			char szDecoded[2*1024] = {0};
			size_t nLen = 1024;
			ConverDecode(song, szDecoded, nLen);
			wcsncpy(m_strPlayTitle, (const wchar_t*)szDecoded, 255);
			m_strPlayTitle[255] = 0;
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_TITLE, m_strPlayTitle);
		}
		else
		{
			// 没有 ID3 title，用文件名
			char szFileName[MAX_FILE_LEN] = {0};
			OnHandle_FileName(pPlayUrl, szFileName);
			char szDecoded[2*1024] = {0};
			size_t nLen = 1024;
			ConverDecode(szFileName, szDecoded, nLen);
			wcsncpy(m_strPlayTitle, (const wchar_t*)szDecoded, 255);
			m_strPlayTitle[255] = 0;
		}

		//获取播放时间
		long long Total_Time = 0;
		Total_Time = m_SPMetaData->duration();
		m_nPlayAuido_Total = Total_Time / 1000;

		LOGDBG("&&&----m_nPlayAuido_Total:%d",m_nPlayAuido_Total);

		artist = m_SPMetaData->metadata("artist");
		if(NULL != artist)
		{
			LOGDBG("&&&---artist:%s",artist);

			char szTemp[2*1024] = {0};
			size_t nLen = 1024;
			ConverDecode(artist,szTemp,nLen);

			//gui update
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_ARTIST, (wchar_t*)szTemp);
			wcsncpy(m_strPlayArtist, (const wchar_t*)szTemp, 255);
			m_strPlayArtist[255] = 0;
		}
		else
		{
			/*artist = m_SPMetaData->metadata("artist_v1");

			if(NULL != artist)
			{
				LOGDBG("&&&---artist_v1:%s",artist);

				char szTemp[2*1024] = {0};
				size_t nLen = 1024;
				ConverDecode(artist,szTemp,nLen);

				//gui update
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_ARTIST, (wchar_t*)szTemp);
			}
			else
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_ARTIST, L"unknow");
			}*/

			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_ARTIST, L"unknow");
			wcscpy(m_strPlayArtist, L"unknown");
		}

		album = m_SPMetaData->metadata("album");
		if(NULL != album)
		{
			LOGDBG("&&&---album:%s",album);

			char szTemp[2*1024] = {0};
			size_t nLen = 1024;
			ConverDecode(album,szTemp,nLen);

			//gui update
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_ALBUM, (wchar_t*)szTemp);
			wcsncpy(m_strPlayAlbum, (const wchar_t*)szTemp, 255);
			m_strPlayAlbum[255] = 0;
		}
		else
		{
			/*album = m_SPMetaData->metadata("album");

			if(NULL != album)
			{
				LOGDBG("&&&---album:%s",album);

				char szTemp[2*1024] = {0};
				size_t nLen = 1024;
				ConverDecode(album,szTemp,nLen);

				//gui update
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_ALBUM, (wchar_t*)szTemp);
			}
			else
			{
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_ALBUM, L"unknow");
			}*/

			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_ALBUM, L"unknow");
			wcscpy(m_strPlayAlbum, L"unknown");
		}

		//专辑封面
		/*SPMetaData::Thumbnail tPic;
		memset(&tPic,0,sizeof(SPMetaData::Thumbnail));

		tPic.fmt = SPMetaData::PIXEL_FMT_COMPRESSED;
		if(-1 != m_SPMetaData->thumbnail(&tPic,230,230))
		{
			LOGDBG("&&&---pic--size:%d",tPic.size[0]);
			//gui update
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PIC,tPic.data[0],tPic.size[0],IMAGETYPE_JPG,ALL_STATE,230,230);
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PIC,204,ALL_STATE);
		}*/
	}

}

void MediaView::OnPlayFile_Sel(int nPlayType)
{
	int nCount = 0;
	char tmpUrl[MAX_FILE_LEN] = {0};
	MediaData &mData = MediaData::GetInstance();

	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	mData.GetCurPlayUrl(tmpUrl);

	if(PLAY_TYPE_AUDIO == nPlayType)
	{
		UIITEMDATA UItem;
		memset(&UItem,0,sizeof(UItem));
		pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_MEDIA_AUDIO_LIST,UUID_AUDIO_PLAYLIST_LIST,UItem);
		CUI_BtList *pAudioList = (CUI_BtList*) UItem.m_pCtrl;

		if(NULL == pAudioList)
		{
			return;
		}

		nCount = m_FileList.GetAudioFileNum();

		for(int i = 0;i < nCount;i++)
		{
			if(0 == strcmp(tmpUrl,m_FileList.m_AudioFileList[i].m_sFilePath))
			{
				int nTop = pAudioList->GetTopIndex();
				LOGDBG("&&&&----ntop:%d\n",nTop);
				pAudioList->SetCurSel(i);
				if((0 == i) || ((i - nTop) > 5))
				{
					pAudioList->SetTopIndex(i);
				}
				break;
			}
		}

		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MEDIA_AUDIO_LIST,UUID_AUDIO_PLAYLIST_AUDIO,ITEM_STATE_SELECTED);
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MEDIA_AUDIO_LIST,UUID_AUDIO_PLAYLIST_VIDEO,ITEM_STATE_NORMAL);
	}
	else if(PLAY_TYPE_VIDEO == nPlayType)
	{
		UIITEMDATA UItem;
		memset(&UItem,0,sizeof(UItem));
		pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_MEDIA_AUDIO_LIST,UUID_AUDIO_PLAYLIST_LIST,UItem);
		CUI_BtList *pVideoList = (CUI_BtList*) UItem.m_pCtrl;

		if(NULL == pVideoList)
		{
			return;
		}

		nCount = m_FileList.GetVideoFileNum();

		for(int i = 0;i < nCount;i++)
		{
			if(0 == strcmp(tmpUrl,m_FileList.m_VideoFileList[i].m_sFilePath))
			{
				int nTop = pVideoList->GetTopIndex();
				LOGDBG("&&&&--video--ntop:%d\n",nTop);
				pVideoList->SetCurSel(i);
				if((0 == i) || ((i - nTop) > 5))
				{
					pVideoList->SetTopIndex(i);
				}
				break;
			}
		}

		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MEDIA_AUDIO_LIST,UUID_AUDIO_PLAYLIST_VIDEO,ITEM_STATE_SELECTED);
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MEDIA_AUDIO_LIST,UUID_AUDIO_PLAYLIST_AUDIO,ITEM_STATE_NORMAL);
	}
}

void MediaView::OnUpdate_PlayUI(const char *pszUrl,int nPlayStatus)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	//更新文件名
	char szFileName[MAX_FILE_LEN] = {0};
	char szTemp[2*1024] = {0};
	size_t nLen = 1024;

	OnHandle_FileName(pszUrl,szFileName);

	ConverDecode(szFileName,szTemp,nLen);

	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_TITLE, (wchar_t*)szTemp);

	//更新曲目数
	int nIndex = m_FileList.GetPlayFile_Index(pszUrl,PLAY_TYPE_AUDIO);
	int nSum = m_FileList.GetAudioFileNum();
	LOGDBG("&&&--nIndex:%d,nSum:%d",nIndex,nSum);

	wchar_t szTip[MAX_FILE_LEN] = {0};
	swprintf(szTip,MAX_FILE_LEN,L"%02d/%02d",nIndex+1,nSum);

	//LOGDBG("&&&&---index : %s",szTip);

    //pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_NO,true);
	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_NO, szTip);

	//更新播放暂停图标
	/*if(Player_Play == nPlayStatus)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAY,211,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAY,212,ITEM_STATE_PUSH);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAY,211,ITEM_STATE_SELECTED);

		//m_pUIAnalysis->drawItem(Wnd::WND_ID_PLAY_VIDEO,Ctrl::CTRL_ID_VIDEO_PLAY_PAUSE);
	}
	else
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAY,214,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAY,215,ITEM_STATE_PUSH);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAY,214,ITEM_STATE_SELECTED);
	}*/
}

void MediaView::OnUpdate_PlayUI_Video(const char *pszUrl,int nPlayStatus)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	//更新文件名
	char szFileName[MAX_FILE_LEN] = {0};
	char szTemp[2*1024] = {0};
	size_t nLen = 1024;

	OnHandle_FileName(pszUrl,szFileName);

	ConverDecode(szFileName,szTemp,nLen);

	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_FILENAME, (wchar_t*)szTemp);

	//更新曲目数
	int nIndex = m_FileList.GetPlayFile_Index(pszUrl,PLAY_TYPE_VIDEO);
	int nSum = m_FileList.GetVideoFileNum();
	LOGDBG("&&&--nIndex:%d,nSum:%d",nIndex,nSum);

	wchar_t szTip[MAX_FILE_LEN] = {0};
	swprintf(szTip,MAX_FILE_LEN,L"%02d/%02d",nIndex+1,nSum);

	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_NO, szTip);

	//更新播放暂停图标
}

void MediaView::UpdateUI_PlayVideo_Error()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_FILENAME, L"File Error");

	OnUpdate_PlayPause_Video(Player_Pause);
}

void MediaView::OnUpdate_PlayPause(int nPlayStatus)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	//更新播放暂停图标
	if(Player_Play == nPlayStatus)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAY,211,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAY,212,ITEM_STATE_PUSH);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAY,211,ITEM_STATE_SELECTED);

		//m_pUIAnalysis->drawItem(Wnd::WND_ID_PLAY_VIDEO,Ctrl::CTRL_ID_VIDEO_PLAY_PAUSE);
	}
	else
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAY,214,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAY,215,ITEM_STATE_PUSH);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAY,214,ITEM_STATE_SELECTED);
	}
}

void MediaView::OnUpdate_PlayPause_Video(int nPlayStatus)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	//更新播放暂停图标
	if(Player_Play == nPlayStatus)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_VIDEO_PLAY,UUID_VIDEO_PLAY_PLAY,211,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_VIDEO_PLAY,UUID_VIDEO_PLAY_PLAY,212,ITEM_STATE_PUSH);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_VIDEO_PLAY,UUID_VIDEO_PLAY_PLAY,211,ITEM_STATE_SELECTED);

		//隐藏文件名和序号控件
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_FILENAME,false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_NO,false);
	}
	else if(Player_Pause == nPlayStatus)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_VIDEO_PLAY,UUID_VIDEO_PLAY_PLAY,214,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_VIDEO_PLAY,UUID_VIDEO_PLAY_PLAY,215,ITEM_STATE_PUSH);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_VIDEO_PLAY,UUID_VIDEO_PLAY_PLAY,214,ITEM_STATE_SELECTED);

		//显示文件名和序号控件
		//pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_FILENAME,true);
		//pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_NO,true);
	}
}

void MediaView::OnHandle_FileName(const char *pszUrl,char *pFileName)
{
	std::string str = pszUrl;
	int nlen = strlen(pszUrl);
	char ch = '/';
	size_t pos = str.rfind(ch);


	if (pos != std::string::npos)
	{
		if(pos < nlen)
		{
			strcpy(pFileName,pszUrl+pos+1);
		}
	}
}

void MediaView::OnTimer(uint32_t id)
{
	switch (id)
	{
		case TIMER_SEARCH_FILES:
		{
			KillTimer(TIMER_SEARCH_FILES);
			MediaData &mData = MediaData::GetInstance();
			char strMount[128] = {0};
			mData.GetMountPath(strMount,128);

			m_FileList.Search_AllDirectory(strMount);
			m_bLoadFinish = true;
			break;
		}
		case TIMER_UPDATE_TIME:
		{
			OnViewDetect();
			break;
		}
		case TIMER_PLAYFAIL_NEXT:
		{
			KillTimer(TIMER_PLAYFAIL_NEXT);
			OnPlay_Next_Video();
			break;
		}
		default:
		{
			break;
		}
	}
}

void MediaView::OnViewDetect()
{
	switch(m_nCurUIfrome)
	{
		case UI_FROM_LOADING:
		{
			if(1 == m_nUsbInsert)
			{
				LOGDBG("&&&---m_bLoadFinish:%d",m_bLoadFinish);
				if(m_bLoadFinish && (GetCurrWnd() == UIID_WNDID_MEDIA_LOADING))
				{
					if(m_FileList.GetAudioFileNum() > 0)
					{
						m_nCurUIfrome = UI_FROM_AUDIO_PLAY;

						GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_AUDIO_PLAY, true);
						//播放音频
						OnPlay_Audio();
					}
					else if(m_FileList.GetVideoFileNum() > 0)
					{
						m_nCurUIfrome = UI_FROM_VIDEO_PLAY;
						GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_VIDEO_PLAY, true);
						//播放视频
						OnPlay_Video();
					}
					else
					{
						//显示无文件
						GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_FAIL, true);
					}
				}
			}
		}
		break;
		default:
		break;
	}

	if(Player_Play == m_nCurPlayStatus)
	{
		//UI 刷新播放时间
		CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
		if (nullptr == pUIMgr)
		{
			LOGDBG("nullptr == pUIMgr Error!!");
			return;
		}

		if(PLAY_TYPE_AUDIO == m_nCurPlayType)
		{
			m_nPlayAudio_pos++;

			int nMin = m_nPlayAudio_pos / 60;
			int sec = m_nPlayAudio_pos % 60;
			wchar_t szTip[MAX_FILE_LEN] = {0};
			swprintf(szTip,MAX_FILE_LEN,L"%02d:%02d",nMin,sec);

			int nPos = (m_nPlayAudio_pos * 100) / m_nPlayAuido_Total;

			//LOGDBG("&&&--nMin:%d,sec:%d,nPos:%d",nMin,sec,nPos);
            
			//pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_TIME,true);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_TIME, szTip);

			//进度
			pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAYBAR,nPos);

			//分屏显示时每秒同步刷新分屏媒体卡片（否则只有收到消息才刷新）
			((IVIView*)GetViewBase())->UpdateSplitMediaView();
		}
		else if(PLAY_TYPE_VIDEO == m_nCurPlayType)
		{
			m_nPlayVideo_pos++;

			int nMin = m_nPlayVideo_pos / 60;
			int sec = m_nPlayVideo_pos % 60;
			wchar_t szTip[MAX_FILE_LEN] = {0};
			swprintf(szTip,MAX_FILE_LEN,L"%02d:%02d",nMin,sec);

			int nPos = (m_nPlayVideo_pos * 100) / m_nPlayVideo_Total;

			//LOGDBG("&&&--nMin:%d,sec:%d,nPos:%d",nMin,sec,nPos);
            
			//pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_MEDIA_AUDIO_PLAY, UUID_PLAY_TIME,true);
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_TIME, szTip);

			//进度
			pUIMgr->GetUIAnalysisObj()->SetProgressItemPos(UIID_WNDID_MEDIA_VIDEO_PLAY,UUID_VIDEO_PLAY_BAR,nPos);			
		}
	}
}

void MediaView::OnPlay_Audio()
{
	if(PLAY_TYPE_AUDIO == m_nCurPlayType)
	{
		if(Player_Pause == m_nCurPlayStatus)
		{
			if(NULL != m_ViewController)
			{
				T_PLAYMEDIA_CMD tmpCmd;
				tmpCmd.nPlayCmd = PLAY_CMD_PLAY;
				tmpCmd.nParam = 0;
				m_ViewController->SetMediaPlay_Cmd(tmpCmd);
			}
		}
	}
	else 
	{
		T_PLAYMEDIA_URL tmpUrl;

		tmpUrl.nPlayType = PLAY_TYPE_AUDIO;

		//if(strlen(m_PrePlayAudioUrl) > 0)
		if(0)
		{
			tmpUrl.nParam = m_nPlayAudio_pos;
			memcpy(tmpUrl.strPlayUrl,m_PrePlayAudioUrl,MAX_FILE_LEN);
		}
		else
		{
			//播放第一首
			tmpUrl.nParam = 0;
			memcpy(tmpUrl.strPlayUrl,m_FileList.m_AudioFileList[0].m_sFilePath,MAX_FILE_LEN);

			LOGDBG("&&&----strPlayUrl:%s",m_FileList.m_AudioFileList[0].m_sFilePath);
		}

		if(NULL != m_ViewController)
		{
			m_ViewController->SetMediaPlay_Url(tmpUrl);
		}
	}
}

void MediaView::OnPlay_Video()
{
	if(PLAY_TYPE_VIDEO == m_nCurPlayType)
	{
		if(Player_Pause == m_nCurPlayStatus)
		{
			if(NULL != m_ViewController)
			{
				T_PLAYMEDIA_CMD tmpCmd;
				tmpCmd.nPlayCmd = PLAY_CMD_PLAY;
				tmpCmd.nParam = 0;
				m_ViewController->SetMediaPlay_Cmd(tmpCmd);
			}
		}
	}
	else 
	{
		T_PLAYMEDIA_URL tmpUrl;

		tmpUrl.nPlayType = PLAY_TYPE_VIDEO;

		//if(strlen(m_PrePlayVideoUrl) > 0)
		if(0)
		{
			tmpUrl.nParam = m_nPlayVideo_pos;
			memcpy(tmpUrl.strPlayUrl,m_PrePlayVideoUrl,MAX_FILE_LEN);
		}
		else
		{
			//播放第一首
			tmpUrl.nParam = 0;
			memcpy(tmpUrl.strPlayUrl,m_FileList.m_VideoFileList[0].m_sFilePath,MAX_FILE_LEN);

			LOGDBG("&&&----strPlayUrl:%s",m_FileList.m_VideoFileList[0].m_sFilePath);
		}

		if(NULL != m_ViewController)
		{
			m_ViewController->SetMediaPlay_Url(tmpUrl);
		}
	}
}

void MediaView::OnReset_Play(int nPlayType)
{
	T_PLAYMEDIA_URL tmpUrl;
	char tmpFile[MAX_FILE_LEN] = {0};

	MediaData &mData = MediaData::GetInstance();

	tmpUrl.nPlayType = nPlayType;
	mData.GetCurPlayUrl(tmpFile);

	memcpy(tmpUrl.strPlayUrl,tmpFile,MAX_FILE_LEN);

	if(NULL != m_ViewController)
	{
		m_ViewController->SetMediaPlay_Url(tmpUrl);
	}
}

int MediaView::GetCurrWnd()
{
	HomeData &mData = HomeData::GetInstance();
	Lock();
	m_nCurrWnd = mData.GetCurrWnd();
	UnLock();
	return m_nCurrWnd;
}

void MediaView::MediaInitWorkWnd(int dwWndId)
{
	LOGDBG("MediaInitWorkWnd");

	switch(dwWndId)
	{
		case UIID_WNDID_MEDIA_AUDIO_PLAY:
		{
			OnUpdateUI_PlayMode(m_nPlayMode);
		}
		break;
		case UIID_WNDID_MEDIA_AUDIO_LIST:
		{
			//音频列表
			if(UI_FROM_AUDIO_PLAY == m_nCurUIfrome)
			{
				if(!m_bAudioList_Update)
				{
					OnUpdateUI_AudioList();
				}
			}
			else if(UI_FROM_VIDEO_PLAY == m_nCurUIfrome)
			{
				if(!m_bVideoList_Update)
				{
					OnUpdateUI_VideoList();
				}
			}
		}
		break;
		case UIID_WNDID_MEDIA_VIDEO_PLAY:
		{
			OnUpdateUI_PlayMode_Video(m_nPlayMode_Video);
		}
		break;
		case UIID_WNDID_MEDIA_FAIL:
		{
			//提示失败原因
			OnUpdateUI_Fail_Text();
		}
		break;
		default:
		break;
	}

	int nCurSource = ((IVIView*)GetViewBase())->Get_CurSource();

	if(IVI_SOURCE_MEDIA != nCurSource)
	{
		//上个源暂停
		((IVIView*)GetViewBase())->OnHandle_Source(nCurSource);

		//保存为当前源
		((IVIView*)GetViewBase())->Save_CurSource(IVI_SOURCE_MEDIA);

		//播放
		if(UIID_WNDID_MEDIA_AUDIO_PLAY == dwWndId)
		{
			if(NULL != m_ViewController)
			{
				T_PLAYMEDIA_CMD tPlayCmd;
				memset(&tPlayCmd,0,sizeof(T_PLAYMEDIA_CMD));

				tPlayCmd.nPlayCmd = PLAY_CMD_PLAY;
				tPlayCmd.nParam = 0;

				m_ViewController->SetMediaPlay_Cmd(tPlayCmd);
			}
		}
		else if(UIID_WNDID_MEDIA_VIDEO_PLAY == dwWndId)
		{
			OnHandle_PlayPause_Video(Player_Play);
		}
	}
}

void MediaView::OnClick_AudioPlayWnd(const int nCtrlId, const int nBtStat, const bool nShow)
{
	switch(nCtrlId)
	{
		case UUID_PLAY_PLAY:
		{
			LOGDBG("&&&---m_nCurPlayStatus:%d",m_nCurPlayStatus);
			if(Player_Play == m_nCurPlayStatus)
			{
				if(NULL != m_ViewController)
				{
					T_PLAYMEDIA_CMD tPlayCmd;
					memset(&tPlayCmd,0,sizeof(T_PLAYMEDIA_CMD));

					tPlayCmd.nPlayCmd = PLAY_CMD_PAUSE;
					tPlayCmd.nParam = 0;

					m_ViewController->SetMediaPlay_Cmd(tPlayCmd);
				}
			}
			else if(Player_Pause == m_nCurPlayStatus)
			{
				if(NULL != m_ViewController)
				{
					T_PLAYMEDIA_CMD tPlayCmd;
					memset(&tPlayCmd,0,sizeof(T_PLAYMEDIA_CMD));

					tPlayCmd.nPlayCmd = PLAY_CMD_PLAY;
					tPlayCmd.nParam = 0;

					m_ViewController->SetMediaPlay_Cmd(tPlayCmd);
				}
			}
		}
		break;
		case UUID_PLAY_PREV:
		{
			OnPlay_Prev();
		}
		break;
		case UUID_PLAY_NEXT:
		{
			OnPlay_Next();
		}
		break;
		case UUID_PLAY_LIST:
		{
			//显示列表
			m_nCurUIfrome = UI_FROM_AUDIO_PLAY;
			OnPlayFile_Sel(PLAY_TYPE_AUDIO);

			if(!m_bAudioList_Update)
			{
				OnUpdateUI_AudioList();
			}

			GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_AUDIO_LIST, true);
		}
		break;
		case UUID_PLAY_MODE:
		{
			//play mode
			if(PLAY_MODE_NORMAL == m_nPlayMode)
			{
				m_nPlayMode = PLAY_MODE_ONE;
			}
			else if(PLAY_MODE_ONE == m_nPlayMode)
			{
				m_nPlayMode = PLAY_MODE_NORMAL;
			}
			m_bPlayShuffle = false;
			OnUpdateUI_PlayMode(m_nPlayMode);
		}
		break;
		case UUID_PLAY_SHUFFLE:
		{
			if(m_bPlayShuffle)
			{
				m_bPlayShuffle = false;
			}
			else
			{
				m_bPlayShuffle = true;
			}

			OnUpdateUI_PlayMode(m_nPlayMode);
		}
		break;
		case UUID_PLAY_SOURCE:
		{
			GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
		}
		break;
		case UIID_PUBLIC_SETTINGH_BIN:
		{
			//setting
			((IVIView*)GetViewBase())->SaveIn_SettingWnd(UIID_WNDID_MEDIA_AUDIO_PLAY);
			GetViewBase()->ShowWnd(UIID_WNDID_SETTING, true);
		}
		break;
		case UUID_PLAY_PLAYBAR:
		{
			//seek
			OnPlay_Seek(PLAY_TYPE_AUDIO);
		}
		break;
		default:
		break;
	}
}

void MediaView::OnClick_AudioListWnd(const int nCtrlId, const int nBtStat, const bool nShow)
{
	switch(nCtrlId)
	{
		case UUID_AUDIO_PLAYLIST_RETURN:
		{
			if(UI_FROM_AUDIO_PLAY == m_nCurUIfrome)
			{
				GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_AUDIO_PLAY, true);
			}
			else if(UI_FROM_VIDEO_PLAY == m_nCurUIfrome)
			{
				GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_VIDEO_PLAY, true);
			}
		}
		break;
		case UUID_AUDIO_PLAYLIST_VIDEO:
		{
			if(!m_bVideoList_Update)
			{
				OnUpdateUI_VideoList();
			}
		}
		break;
		case UUID_AUDIO_PLAYLIST_AUDIO:
		{
			if(!m_bAudioList_Update)
			{
				OnUpdateUI_AudioList();
			}
		}
		break;
		case UUID_AUDIO_PLAYLIST_LIST:
		{
			if(m_bAudioList_Update)
			{
				if(m_FileList.GetAudioFileNum() > 0)
				{
					OnClick_Wnd_AudioList();

					GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_AUDIO_PLAY, true);
				}
			}
			else if(m_bVideoList_Update) 
			{
				if(m_FileList.GetVideoFileNum() > 0)
				{
					OnClick_Wnd_VideoList();

					GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_VIDEO_PLAY, true);
				}
			}
		}
		break;
		default:
		break;
	}
}

void MediaView::OnClick_VideoPlayWnd(const int nCtrlId, const int nBtStat, const bool nShow)
{
	switch(nCtrlId)
	{
		case UUID_VIDEO_PLAY_PLAY:
		{
			LOGDBG("&&&---m_nCurPlayStatus:%d",m_nCurPlayStatus);
			if(Player_Play == m_nCurPlayStatus)
			{
				OnHandle_PlayPause_Video(Player_Pause);
			}
			else if(Player_Pause == m_nCurPlayStatus)
			{
				OnHandle_PlayPause_Video(Player_Play);
			}
		}
		break;
		case UUID_VIDEO_PLAY_NEXT:
		{
			KillTimer(TIMER_PLAYFAIL_NEXT);

			OnPlay_Next_Video();
		}
		break;
		case UUID_VIDEO_PLAY_PREV:
		{
			KillTimer(TIMER_PLAYFAIL_NEXT);

			OnPlay_Prev_Video();
		}
		break;
		case UUID_VIDEO_PLAY_LIST:
		{
			//显示列表
			m_nCurUIfrome = UI_FROM_VIDEO_PLAY;
			OnPlayFile_Sel(PLAY_TYPE_VIDEO);

			if(!m_bVideoList_Update)
			{
				OnUpdateUI_VideoList();
			}

			GetViewBase()->ShowWnd(UIID_WNDID_MEDIA_AUDIO_LIST, true);			
		}
		break;
		case UUID_VIDEO_PLAY_MODE:
		{
			//play mode
			if(PLAY_MODE_NORMAL == m_nPlayMode_Video)
			{
				m_nPlayMode_Video = PLAY_MODE_ONE;
			}
			else if(PLAY_MODE_ONE == m_nPlayMode_Video)
			{
				m_nPlayMode_Video = PLAY_MODE_NORMAL;
			}

			OnUpdateUI_PlayMode_Video(m_nPlayMode_Video);
		}
		break;
		case UUID_VIDEO_PLAY_SOURCE:
		{
			GetViewBase()->ShowWnd(UIID_WNDID_MAIN, true);
		}
		break;
		case UUID_VIDEO_PLAY_BK:
		{
			//工具栏隐藏显示
			VideoPlayTool_ShowHide();
		}
		break;
		case UUID_VIDEO_PLAY_BAR:
		{
			OnPlay_Seek(PLAY_TYPE_VIDEO);
		}
		break;
		default:
		break;
	}
}

void MediaView::VideoPlayTool_ShowHide()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	bool bToolBar = false;

	bToolBar = pUIMgr->GetUIAnalysisObj()->IsItemVisible(UIID_WNDID_MEDIA_VIDEO_PLAY,UUID_VIDEO_PLAY_GROUP);

	if(bToolBar)
	{
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_GROUP,false);
	}
	else
	{
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_GROUP,true);
	}
}

void MediaView::GetPlayInfo(int &nStatus, int &nPos, int &nTotal, char *pszUrl, int nLen)
{
	nStatus = m_nCurPlayStatus;
	nPos    = m_nPlayAudio_pos;
	nTotal  = m_nPlayAuido_Total;
	if (pszUrl && nLen > 0)
	{
		memset(pszUrl, 0, nLen);
		MediaData &mData = MediaData::GetInstance();
		mData.GetCurPlayUrl(pszUrl);
	}
}

void MediaView::GetPlayID3Info(wchar_t *pTitle, wchar_t *pArtist, wchar_t *pAlbum, int nLen)
{
	// nLen 为宽字符个数；内部按 wchar_t 拷贝，避免 UCS-4 数据被 strncpy 截断成 1 个字
	if (pTitle)
	{
		memset(pTitle, 0, nLen * sizeof(wchar_t));
		wcsncpy(pTitle, m_strPlayTitle, nLen - 1);
	}
	if (pArtist)
	{
		memset(pArtist, 0, nLen * sizeof(wchar_t));
		wcsncpy(pArtist, m_strPlayArtist, nLen - 1);
	}
	if (pAlbum)
	{
		memset(pAlbum, 0, nLen * sizeof(wchar_t));
		wcsncpy(pAlbum, m_strPlayAlbum, nLen - 1);
	}
}

void MediaView::OnNeed_Pause()
{
	if(PLAY_TYPE_AUDIO == m_nCurPlayType)
	{
		if(Player_Play == m_nCurPlayStatus)
		{
			if(NULL != m_ViewController)
			{
				T_PLAYMEDIA_CMD tPlayCmd;
				memset(&tPlayCmd,0,sizeof(T_PLAYMEDIA_CMD));

				tPlayCmd.nPlayCmd = PLAY_CMD_PAUSE;;
				tPlayCmd.nParam = 0;

				m_ViewController->SetMediaPlay_Cmd(tPlayCmd);
			}
		}
	}
	else if(PLAY_TYPE_VIDEO == m_nCurPlayType)
	{
		if(Player_Play == m_nCurPlayStatus)
		{
			OnHandle_PlayPause_Video(Player_Pause);
		}
	}
}

void MediaView::OnPlay_Seek(int nPlayType)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	int nPos = 0;
	int nPlay_Positon = 0;

	if(PLAY_TYPE_AUDIO == nPlayType)
	{
		nPos = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_PLAYBAR);

		nPlay_Positon = (nPos * m_nPlayAuido_Total) / 100;
	}
	else if(PLAY_TYPE_VIDEO == nPlayType)
	{
		nPos = pUIMgr->GetUIAnalysisObj()->GetProgressItemPos(UIID_WNDID_MEDIA_VIDEO_PLAY,UUID_VIDEO_PLAY_BAR);

		nPlay_Positon = (nPos * m_nPlayVideo_Total) / 100;
	}

	LOGDBG("&&&---bar--nPlay_Positon:%d",nPlay_Positon);

	if(NULL != m_ViewController)
	{
		T_PLAYMEDIA_CMD tmpCmd;
		tmpCmd.nPlayCmd = PLAY_CMD_SEEK;
		tmpCmd.nParam = nPlay_Positon;
		m_ViewController->SetMediaPlay_Cmd(tmpCmd);
	}
}

void MediaView::MediaUpdateHomeContenInfo(long nWndId,const int nCtrlId, const int nBtStat, const bool nShow)
{
    LOGDBG("&&&---wnd:%d,ctrlid:%d",nWndId,nCtrlId);
	switch(nWndId)
	{
		case UIID_WNDID_MEDIA_AUDIO_PLAY:
		{
			OnClick_AudioPlayWnd(nCtrlId,nBtStat,nShow);
		}
		break;
		case UIID_WNDID_MEDIA_AUDIO_LIST:
		{
			//列表窗口
			OnClick_AudioListWnd(nCtrlId,nBtStat,nShow);
		}
		break;
		case UIID_WNDID_MEDIA_VIDEO_PLAY:
		{
			//播放窗口
			OnClick_VideoPlayWnd(nCtrlId,nBtStat,nShow);
		}
		break;
		default:
		break;
	}
}

void MediaView::OnPlay_Next()
{
	T_PLAYMEDIA_URL tmpUrl;
	char tmpPlayFile[MAX_FILE_LEN] = {0};
	Element Ele;
	int nRet = -1;

	MediaData &mData = MediaData::GetInstance();
	mData.GetCurPlayUrl(tmpPlayFile);

	LOGDBG("&&&---curPlayFile:%s",tmpPlayFile);

	LOGDBG("&&&----m_bPlayShuffle:%d",m_bPlayShuffle);

	if(m_bPlayShuffle)
	{
		nRet = m_FileList.GetPlayNext(tmpPlayFile,PLAY_TYPE_AUDIO,PLAY_MODE_SHUFFLE,Ele);
	}
	else
	{
		nRet = m_FileList.GetPlayNext(tmpPlayFile,PLAY_TYPE_AUDIO,m_nPlayMode,Ele);
	}

	LOGDBG("&&&---Next_file:%s",Ele.m_sFilePath);

	if(-1 == nRet)
	{
		return;
	}
	
	memcpy(tmpUrl.strPlayUrl,Ele.m_sFilePath,MAX_FILE_LEN);
	tmpUrl.nPlayType = PLAY_TYPE_AUDIO;
	tmpUrl.nParam = 0;

	//播放下一曲
	if(NULL != m_ViewController)
	{
		m_ViewController->SetMediaPlay_Url(tmpUrl);
	}
}

void MediaView::OnPlay_Next_Video()
{
	T_PLAYMEDIA_URL tmpUrl;
	char tmpPlayFile[MAX_FILE_LEN] = {0};
	Element Ele;
	int nRet = -1;

	MediaData &mData = MediaData::GetInstance();
	mData.GetCurPlayUrl(tmpPlayFile);

	LOGDBG("&&&---curPlayFile:%s",tmpPlayFile);

	nRet = m_FileList.GetPlayNext(tmpPlayFile,PLAY_TYPE_VIDEO,m_nPlayMode_Video,Ele);

	LOGDBG("&&&---Next_file:%s",Ele.m_sFilePath);

	if(-1 == nRet)
	{
		return;
	}

	memcpy(tmpUrl.strPlayUrl,Ele.m_sFilePath,MAX_FILE_LEN);
	tmpUrl.nPlayType = PLAY_TYPE_VIDEO;
	tmpUrl.nParam = 0;

	//播放下一曲
	if(NULL != m_ViewController)
	{
		m_ViewController->SetMediaPlay_Url(tmpUrl);
	}
}

void MediaView::OnPlay_Prev()
{
	T_PLAYMEDIA_URL tmpUrl;
	char tmpPlayFile[MAX_FILE_LEN] = {0};
	Element Ele;
	int nRet = -1;

	MediaData &mData = MediaData::GetInstance();
	mData.GetCurPlayUrl(tmpPlayFile);

	LOGDBG("&&&---curPlayFile:%s",tmpPlayFile);

	if(m_bPlayShuffle)
	{
		nRet = m_FileList.GetPlayPrev(tmpPlayFile,PLAY_TYPE_AUDIO,PLAY_MODE_SHUFFLE,Ele);
	}
	else
	{
		nRet = m_FileList.GetPlayPrev(tmpPlayFile,PLAY_TYPE_AUDIO,m_nPlayMode,Ele);
	}
	LOGDBG("&&&---Pre_file:%s",Ele.m_sFilePath);

	if(-1 == nRet)
	{
		return;
	}
	
	if(m_nPlayAudio_pos > 3)
	{
		memcpy(tmpUrl.strPlayUrl,tmpPlayFile,MAX_FILE_LEN);
	}
	else
	{
		memcpy(tmpUrl.strPlayUrl,Ele.m_sFilePath,MAX_FILE_LEN);
	}

	tmpUrl.nPlayType = PLAY_TYPE_AUDIO;
	tmpUrl.nParam = 0;

	//播放上一曲
	if(NULL != m_ViewController)
	{
		m_ViewController->SetMediaPlay_Url(tmpUrl);
	}
}

void MediaView::OnPlay_Prev_Video()
{
	T_PLAYMEDIA_URL tmpUrl;
	char tmpPlayFile[MAX_FILE_LEN] = {0};
	Element Ele;
	int nRet = -1;

	MediaData &mData = MediaData::GetInstance();
	mData.GetCurPlayUrl(tmpPlayFile);

	LOGDBG("&&&---curPlayFile:%s",tmpPlayFile);

	nRet = m_FileList.GetPlayPrev(tmpPlayFile,PLAY_TYPE_VIDEO,m_nPlayMode_Video,Ele);

	LOGDBG("&&&---Pre_file:%s",Ele.m_sFilePath);

	if(-1 == nRet)
	{
		return;
	}

	memcpy(tmpUrl.strPlayUrl,Ele.m_sFilePath,MAX_FILE_LEN);
	tmpUrl.nPlayType = PLAY_TYPE_VIDEO;
	tmpUrl.nParam = 0;

	//播放上一曲
	if(NULL != m_ViewController)
	{
		m_ViewController->SetMediaPlay_Url(tmpUrl);
	}
}

void MediaView::OnUpdateUI_PlayMode(int nPlayMode)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	int nPicID = 0;
	int nPicID_Dn = 0;
	int nPicID_Fs = 0;

	if(PLAY_MODE_NORMAL == nPlayMode)
	{
		nPicID = 220;
		nPicID_Dn = 221;
		nPicID_Fs = 220;
	}
	else if(PLAY_MODE_ONE == nPlayMode)
	{
		nPicID = 224;
		nPicID_Dn = 225;
		nPicID_Fs = 224;
	}

	pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_MODE,nPicID,ITEM_STATE_NORMAL);
	pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_MODE,nPicID_Dn,ITEM_STATE_PUSH);
	pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_MODE,nPicID_Fs,ITEM_STATE_SELECTED);

	if(m_bPlayShuffle)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_MODE,ITEM_STATE_NORMAL);

		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_SHUFFLE,234,ITEM_STATE_NORMAL);

		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_SHUFFLE,ITEM_STATE_SELECTED);
	}
	else
	{
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_MODE,ITEM_STATE_SELECTED);

		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_SHUFFLE,232,ITEM_STATE_NORMAL);

		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_MEDIA_AUDIO_PLAY,UUID_PLAY_SHUFFLE,ITEM_STATE_NORMAL);
	}
	((IVIView*)GetViewBase())->UpdateSplitMediaView();
}

void MediaView::OnUpdateUI_PlayMode_Video(int nPlayMode)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	int nPicID = 0;
	int nPicID_Dn = 0;
	int nPicID_Fs = 0;

	if(PLAY_MODE_NORMAL == nPlayMode)
	{
		nPicID = 220;
		nPicID_Dn = 221;
		nPicID_Fs = 220;
	}
	else if(PLAY_MODE_ONE == nPlayMode)
	{
		nPicID = 224;
		nPicID_Dn = 225;
		nPicID_Fs = 224;
	}

	pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_VIDEO_PLAY,UUID_VIDEO_PLAY_MODE,nPicID,ITEM_STATE_NORMAL);
	pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_VIDEO_PLAY,UUID_VIDEO_PLAY_MODE,nPicID_Dn,ITEM_STATE_PUSH);
	pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_MEDIA_VIDEO_PLAY,UUID_VIDEO_PLAY_MODE,nPicID_Fs,ITEM_STATE_SELECTED);

}

void MediaView::OnHandle_PlayPause_Video(int nPlayStatus)
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	T_PLAYMEDIA_CMD tPlayCmd;
	bool bRet = false;

	if(Player_Play == nPlayStatus)
	{
		memset(&tPlayCmd,0,sizeof(T_PLAYMEDIA_CMD));

		tPlayCmd.nPlayCmd = PLAY_CMD_PLAY;
		tPlayCmd.nParam = 0;

		bRet = true;
	}
	else if(Player_Pause == nPlayStatus)
	{
		memset(&tPlayCmd,0,sizeof(T_PLAYMEDIA_CMD));

		tPlayCmd.nPlayCmd = PLAY_CMD_PAUSE;
		tPlayCmd.nParam = 0;

		bRet = true;

		//显示文件名和序号控件
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_FILENAME,true);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_MEDIA_VIDEO_PLAY, UUID_VIDEO_PLAY_NO,true);
	}
	else if(Player_Stop == nPlayStatus)
	{
		memset(&tPlayCmd,0,sizeof(T_PLAYMEDIA_CMD));

		tPlayCmd.nPlayCmd = PLAY_CMD_STOP;
		tPlayCmd.nParam = 0;

		bRet = true;
	}

	if(bRet)
	{
		if(NULL != m_ViewController)
		{
			m_ViewController->SetMediaPlay_Cmd(tPlayCmd);
		}
	}

}

void MediaView::OnUpdateUI_AudioList()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	int nCount = 0;
	char szFileName[MAX_FILE_LEN] = {0};
	//char szTemp[2*1024] = {0};
	//size_t nLen = 1024;
	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));
	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_MEDIA_AUDIO_LIST,UUID_AUDIO_PLAYLIST_LIST,UItem);
	CUI_BtList *pAudioList = (CUI_BtList*) UItem.m_pCtrl;

	if(pAudioList!=NULL)
	{
		pAudioList->DeleteAllItems();
	}

	nCount = m_FileList.GetAudioFileNum();

	for(int i = 0;i < nCount;i++)
	{
		memset(szFileName,0,sizeof(szFileName));
		//memset(szTemp,0,sizeof(szTemp));

		m_FileList.m_AudioFileList[i].PathFindFileName(szFileName);

		LOGDBG("&&&---szFileName:%s",szFileName);

		char szTemp[2*1024] = {0};
		size_t nLen = 1024;
		//转换
		ConverDecode(szFileName,szTemp,nLen);

		pAudioList->InsertItem(i);
		pAudioList->SetItemString(i,(wchar_t*)szTemp);

		pAudioList->SetItemICon(i,LIST_AUIDO_PIC,ALL_STATE);
	}

	m_bAudioList_Update = true;
	m_bVideoList_Update = false;
}

void MediaView::OnUpdateUI_VideoList()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	int nCount = 0;
	char szFileName[MAX_FILE_LEN] = {0};
	//char szTemp[2*1024] = {0};
	//size_t nLen = 1024;
	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));
	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_MEDIA_AUDIO_LIST,UUID_AUDIO_PLAYLIST_LIST,UItem);
	CUI_BtList *pAudioList = (CUI_BtList*) UItem.m_pCtrl;

	if(pAudioList!=NULL)
	{
		pAudioList->DeleteAllItems();
	}

	nCount = m_FileList.GetVideoFileNum();

	LOGDBG("&&&---video count:%d",nCount);

	for(int i = 0;i < nCount;i++)
	{
		memset(szFileName,0,sizeof(szFileName));
		//memset(szTemp,0,sizeof(szTemp));

		m_FileList.m_VideoFileList[i].PathFindFileName(szFileName);

		LOGDBG("&&&---szFileName:%s",szFileName);

		char szTemp[4*1024] = {0};
		size_t nLen = 1024;

		//转换
		ConverDecode(szFileName,szTemp,nLen);

		pAudioList->InsertItem(i);
		pAudioList->SetItemString(i,(wchar_t*)szTemp);

		pAudioList->SetItemICon(i,LIST_VIDEO_PIC,ALL_STATE);
	}

	m_bAudioList_Update = false;
	m_bVideoList_Update = true;
}

void MediaView::OnClick_Wnd_AudioList()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	char tmpPlayFile[MAX_FILE_LEN] = {0};

	MediaData &mData = MediaData::GetInstance();
	mData.GetCurPlayUrl(tmpPlayFile);

	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));
	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_MEDIA_AUDIO_LIST,UUID_AUDIO_PLAYLIST_LIST,UItem);
	CUI_BtList *pAudioList = (CUI_BtList*) UItem.m_pCtrl;

	if(NULL == pAudioList)
	{
		LOGERR("&&&---get CUI_BtList is NULL");
		return;
	}

	int nLine = 0;
	int ItemData = 0;
	int nCol = 0;

	pAudioList->GetCurSel(nLine,ItemData,nCol);

	Element Ele = m_FileList.m_AudioFileList[nLine];

	if(0 == strcmp(Ele.m_sFilePath,tmpPlayFile))
	{
		//下发播放命令
		T_PLAYMEDIA_CMD tmpCmd;
		tmpCmd.nPlayCmd = PLAY_CMD_PLAY;
		tmpCmd.nParam = 0;

		if(NULL != m_ViewController)
		{
			m_ViewController->SetMediaPlay_Cmd(tmpCmd);
		}
	}
	else
	{
		//下发选择歌曲url
		T_PLAYMEDIA_URL tmpUrl;
		memcpy(tmpUrl.strPlayUrl,Ele.m_sFilePath,MAX_FILE_LEN);
		tmpUrl.nPlayType = PLAY_TYPE_AUDIO;
		tmpUrl.nParam = 0;

		//播放上一曲
		if(NULL != m_ViewController)
		{
			m_ViewController->SetMediaPlay_Url(tmpUrl);
		}
	}
}

void MediaView::OnClick_Wnd_VideoList()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	char tmpPlayFile[MAX_FILE_LEN] = {0};

	MediaData &mData = MediaData::GetInstance();
	mData.GetCurPlayUrl(tmpPlayFile);

	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));
	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_MEDIA_AUDIO_LIST,UUID_AUDIO_PLAYLIST_LIST,UItem);
	CUI_BtList *pVideoList = (CUI_BtList*) UItem.m_pCtrl;

	if(NULL == pVideoList)
	{
		LOGERR("&&&---get CUI_BtList is NULL");
		return;
	}

	int nLine = 0;
	int ItemData = 0;
	int nCol = 0;

	pVideoList->GetCurSel(nLine,ItemData,nCol);

	Element Ele = m_FileList.m_VideoFileList[nLine];

	if(0 == strcmp(Ele.m_sFilePath,tmpPlayFile))
	{
		//下发播放命令
		T_PLAYMEDIA_CMD tmpCmd;
		tmpCmd.nPlayCmd = PLAY_CMD_PLAY;
		tmpCmd.nParam = 0;

		if(NULL != m_ViewController)
		{
			m_ViewController->SetMediaPlay_Cmd(tmpCmd);
		}
	}
	else
	{
		//下发选择视频url
		T_PLAYMEDIA_URL tmpUrl;
		memcpy(tmpUrl.strPlayUrl,Ele.m_sFilePath,MAX_FILE_LEN);
		tmpUrl.nPlayType = PLAY_TYPE_VIDEO;
		tmpUrl.nParam = 0;

		//播放上一曲
		if(NULL != m_ViewController)
		{
			m_ViewController->SetMediaPlay_Url(tmpUrl);
		}
	}
}

void MediaView::OnUpdateUI_Fail_Text()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	int nTextID = 0;

	if(0 == m_nUsbInsert)
	{
		nTextID = TEXT_USB_NO_DEVICE;
	}
	else
	{
		if(m_bLoadFinish)
		{
			if((0 == m_FileList.GetAudioFileNum()) && (0 == m_FileList.GetVideoFileNum()))
			{
				nTextID = TEXT_USB_NO_FILE;
			}
		}
	}

	pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_MEDIA_FAIL,UUID_FAIL_TEXT_TIP,nTextID);
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

void MediaView::ConverDecode(char *pInBuf,char *pOutBuf,size_t& nOutLen)
{
	const char *encTo = "UCS-4LE";
	const char *encFrom = "UTF-8";    //UTF-8 ISO-8859-1
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

