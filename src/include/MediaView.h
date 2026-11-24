/*
 * HomeView.h
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#ifndef MEDIAVIEW_H_
#define MEDIAVIEW_H_

//View的实体，实现显示功能。
#include "MediaData.h"

#include "CompBase.h"
#include "IViewMessage.h"
#include "AppDatadef.h"
#include "FileBase.h"
#include "IVIController.h"
#include "FileList.h"

#include "SPMetaData.h"

using namespace playback;
using std::map;
using FY_NAMESPACE::T_APP_MSG;
using FY_NAMESPACE::TimerBase;

class MediaView;
typedef bool (MediaView::*pUpdateMediaMsg)(T_APP_MSG* msg);

class MediaView : public CompBase, public TimerBase
{
public:
	MediaView(ViewBase* p_ViewBase);
	virtual ~MediaView();

	virtual void MediaOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState);
	
	/*
	 * call back UserMessge*/
	virtual bool MediaOnUserMessage(T_APP_MSG* msg);

	/*
	 * call back GUI operation status*/
	virtual void MediaOnComponent(CUIWndMng* pUI, PWINDOW_INFO pInfo);

	/*
	 * call back timer*/
	virtual void OnTimer(uint32_t id);
	
	void MediaInitWorkWnd(int dwWndId);

	void MediaAttachController(IVIController* pController)
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
	void ConverDecode(char *pInBuf,char *pOutBuf,size_t& nOutLen);

	bool OnHandle_Mount(T_APP_MSG* msg);
	bool OnPlayUrl_Status(T_APP_MSG* msg);
	bool OnPlayUrl_Time(T_APP_MSG* msg);   //播放时间消息

	void OnViewDetect();

	void OnMedia_WinShow();   //home直接调用显示

	void OnReset_Play(int nPlayType);

	void UpdateUI_PlayVideo_Error();

	void OnPlayFile_Sel(int nPlayType);

	void OnNeed_Pause();

	void OnNeed_PauseVideo();

	void OnPlay_Seek(int nPlayType);

	void VideoPlayTool_ShowHide();

	void GetPlayInfo(int &nStatus, int &nPos, int &nTotal, char *pszUrl, int nLen);
	int  GetPlayMode()    { return m_nPlayMode; }
	bool GetPlayShuffle() { return m_bPlayShuffle; }

	//供分屏/外部调用：显式播放或暂停，并同步图标
	void SetPlayPause(bool bPlay);
	//设置播放模式（列表循环/单曲循环/文件夹循环/随机）
	void SetPlayMode(int nPlayMode, bool bShuffle);
	
	int  GetUsbInsert(){ return m_nUsbInsert; }

	void GetPlayID3Info(wchar_t *pTitle, wchar_t *pArtist, wchar_t *pAlbum, int nLen);

private:

	/*
	 * Initialize update message map*/
	void MediaInitUpdateMsgMap();
	void MediaGetUpdateMessageMap(map<int,pUpdateMediaMsg> &nMap);

	void Lock(){m_Lock.Lock();}
	void UnLock(){m_Lock.Unlock();}

	int GetCurrWnd();

	void MediaUpdateHomeContenInfo(long nWndId,const int nCtrlId, const int nBtStat, const bool nShow);

	void OnClick_AudioPlayWnd(const int nCtrlId, const int nBtStat, const bool nShow);  //点击响应音频播放
	void OnClick_AudioListWnd(const int nCtrlId, const int nBtStat, const bool nShow);  //点击响应音频列表
	void OnClick_VideoPlayWnd(const int nCtrlId, const int nBtStat, const bool nShow);  //点击响应视频播放

	void OnPlay_Audio();
	void OnPlay_Video();

	void OnPasePlayID3(char *pPlayUrl);

	void OnUpdate_PlayUI(const char *pszUrl,int nPlayStatus);
	void OnHandle_FileName(const char *pszUrl,char *pFileName);
	void OnUpdate_PlayPause(int nPlayStatus);

	void OnPlay_Next();
	void OnPlay_Prev();

	void OnUpdateUI_PlayMode(int nPlayMode);

	void CheckAudioEnd();   //检测当前音频是否播放结束
	void CheckVideoEnd();   //检测当前视频是否播放结束

	void OnUpdateUI_AudioList();
	void OnUpdateUI_VideoList();

	void OnClick_Wnd_AudioList();
	void OnClick_Wnd_VideoList();

	void OnUpdate_PlayUI_Video(const char *pszUrl,int nPlayStatus);
	void OnUpdate_PlayPause_Video(int nPlayStatus);
	void OnPlay_Next_Video();
	void OnPlay_Prev_Video();
	int OnPase_TotalTime(char *pPlayUrl);
	void OnUpdateUI_PlayMode_Video(int nPlayMode);
	void OnHandle_PlayPause_Video(int nPlayStatus);

	void OnUpdateUI_Fail_Text();

private:
	IVIController * m_ViewController;
	map<int,pUpdateMediaMsg> m_MsgMap;

	MutexLock m_Lock;
	int m_nCurrWnd;

	CFileList m_FileList;

	bool m_bLoadFinish;     //U盘目录扫描

	int m_nCurUIfrome;

	int m_nUsbInsert;    //0:未插入U盘 1:插入U盘

	char m_PrePlayAudioUrl[MAX_FILE_LEN]; 
	char m_PrePlayVideoUrl[MAX_FILE_LEN];

	int m_nCurPlayStatus;

	int m_nCurPlayType;
	int m_nPlayAudio_pos;
	int m_nPlayVideo_pos;

	int m_nPlayAuido_Total;
	int m_nPlayVideo_Total;
	wchar_t m_strPlayTitle[256];    // 当前歌曲名（UCS-4，宽字符）
	wchar_t m_strPlayArtist[256];   // 当前歌手
	wchar_t m_strPlayAlbum[256];    // 当前专辑

	int m_nPlayMode;
	bool m_bPlayShuffle;

	SPMetaData* m_SPMetaData;

	bool m_bAudioList_Update;   //记忆音频列表已插入
	bool m_bVideoList_Update;   //记忆视频列表已插入

	int m_nPlayMode_Video;

	bool m_bAudioEndHandled;   //当前音频结束已处理，避免重复切歌
	bool m_bVideoEndHandled;   //当前视频结束已处理
};

#endif /* HOMEVIEW_H_ */
