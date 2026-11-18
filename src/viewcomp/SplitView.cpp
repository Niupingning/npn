#include "SplitView.h"
#include "IVIView.h"
#include "HomeData.h"
#include "RadioData.h"
#include "BTData.h"
#include "MediaView.h"
#include "MediaData.h"

#include <wchar.h>

using namespace FY_NAMESPACE;

SplitView::SplitView(ViewBase* p_ViewBase)
: CompBase(p_ViewBase), m_ViewController(NULL), m_nCurPage(0), m_bUSBMounted(false) {}

/* 进分屏时调用：选一个默认页并刷新 */
void SplitView::SplitInitWorkWnd()
{
	// 进分屏时主动获取 U 盘真实挂载状态，避免初始 false 导致误显示 No Device
	MediaView* pMedia = ((IVIView*)GetViewBase())->GetMediaView();
	if (NULL != pMedia)
	{
		m_bUSBMounted = (1 == pMedia->GetUsbInsert());
	}
	// 如果 MediaView 还没刷新，用 MediaData 里的挂载路径判断
	if (!m_bUSBMounted)
	{
		char szMount[128] = {0};
		MediaData::GetInstance().GetMountPath(szMount, sizeof(szMount));
		m_bUSBMounted = (szMount[0] != '\0');
	}

	m_nCurPage = SplitPickDefaultPage();
	SplitSetPage(m_nCurPage);
	SplitEnableTextFeature();
}

/* 文字控件统一设为单行 + 超长滚动，防止换行/竖排 */
void SplitView::SplitEnableTextFeature()
{
	CUIAnalysis* p = GetViewBase()->GetUIWndMgrWrapper()->GetUIAnalysisObj();
	if (NULL == p) return;

	const int arrId[] = {
		UIID_SPLIT_MEDIA_TITLE,  UIID_SPLIT_MEDIA_ARTIST, UIID_SPLIT_MEDIA_ALBUM,
		UIID_SPLIT_BT_TITLE,     UIID_SPLIT_BT_ARTIST,    UIID_SPLIT_BT_ALBUM,
		UIID_SPLIT_RADIO_FREQ,   UIID_SPLIT_RADIO_PS,
	};

	for (int i = 0; i < (int)(sizeof(arrId) / sizeof(arrId[0])); i++)
	{
		BOOL bSingle = TRUE;
		p->SetItemParameter(UIID_WNDID_SPLIT, arrId[i], SINGLE_LINE_MODE, (void*)&bSingle);

		BOOL bScroll = TRUE;
		p->SetItemParameter(UIID_WNDID_SPLIT, arrId[i], BTN_SCROLL_TEXT, (void*)&bScroll);

		unsigned int nMode = CLSSAIC_SCROLL_MODE;
		p->SetItemParameter(UIID_WNDID_SPLIT, arrId[i], CTRL_DISPLAY_SETTING, (void*)&nMode);
	}
}

/* 切页：显示对应组、隐藏其它组、刷分页点、刷内容 */
void SplitView::SplitSetPage(int nPage)
{
	CUIAnalysis* p = GetViewBase()->GetUIWndMgrWrapper()->GetUIAnalysisObj();
	if (NULL == p) return;
	m_nCurPage = nPage;
	HomeData::GetInstance().SaveINTData(INT_SPLIT_PAGE, nPage);

	p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_GROUP_RADIO, SPLIT_PAGE_RADIO == nPage);
	p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_GROUP_BT,    SPLIT_PAGE_BT    == nPage);
	p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_GROUP_USB,   SPLIT_PAGE_USB   == nPage);

	// 顶部媒体源名称随分页切换
	const wchar_t *pName = L"Radio";
	if(SPLIT_PAGE_BT  == nPage)
	{
		pName = L"Bluetooth Audio";
	}
	else if(SPLIT_PAGE_USB == nPage)
	{
		pName = L"USB";
	}
	p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_NAME, pName);

	SplitUpdatePageDot();
	SplitUpdateNoDevice(nPage);

	switch (nPage)
	{
		case SPLIT_PAGE_RADIO: SplitUpdateRadio();   break;
		case SPLIT_PAGE_BT:    SplitUpdateBTMusic(); break;
		case SPLIT_PAGE_USB:   SplitUpdateUSB();     break;
	}
}

/* 三个分页点的选中态 */
void SplitView::SplitUpdatePageDot()
{
	CUIAnalysis* p = GetViewBase()->GetUIWndMgrWrapper()->GetUIAnalysisObj();
	p->SetItemState(UIID_WNDID_SPLIT, UIID_SPLIT_PAGE_DOT_RADIO,
		(SPLIT_PAGE_RADIO == m_nCurPage) ? ITEM_STATE_SELECTED : ITEM_STATE_NORMAL);
	p->SetItemState(UIID_WNDID_SPLIT, UIID_SPLIT_PAGE_DOT_BT,
		(SPLIT_PAGE_BT == m_nCurPage) ? ITEM_STATE_SELECTED : ITEM_STATE_NORMAL);
	p->SetItemState(UIID_WNDID_SPLIT, UIID_SPLIT_PAGE_DOT_USB,
		(SPLIT_PAGE_USB == m_nCurPage) ? ITEM_STATE_SELECTED : ITEM_STATE_NORMAL);
}

/* No Device 显示规则：Radio 永远有；BT 未连接显示；USB 未挂载显示 */
void SplitView::SplitUpdateNoDevice(int nPage)
{
	CUIAnalysis* p = GetViewBase()->GetUIWndMgrWrapper()->GetUIAnalysisObj();
	if (NULL == p) return;
	bool bNoDevice = false;
	if (SPLIT_PAGE_BT == nPage)
	{
		bNoDevice = (2 != BTData::GetInstance().GetBtConnectStatus());
	}
	else if (SPLIT_PAGE_USB == nPage)
	{
		bNoDevice = (!m_bUSBMounted);
	}

	if (SPLIT_PAGE_BT == nPage)
	{
		p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_GROUP_BT, !bNoDevice);
	}
	else if (SPLIT_PAGE_USB == nPage)
	{
		p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_GROUP_USB, !bNoDevice);
	}

	p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_NO_DEVICE, bNoDevice);
}

void SplitView::SplitOnClick(CUIWndMng* pUI, long nCtrlId, int nBtState)
{
	switch (nCtrlId)
	{
		case UIID_SPLIT_PAGE_DOT_RADIO: SplitSetPage(SPLIT_PAGE_RADIO); return;
		case UIID_SPLIT_PAGE_DOT_BT:    SplitSetPage(SPLIT_PAGE_BT);    return;
		case UIID_SPLIT_PAGE_DOT_USB:   SplitSetPage(SPLIT_PAGE_USB);   return;
		default: break;
	}

	IVIView *pView = (IVIView*)GetViewBase();
	if (SPLIT_PAGE_RADIO == m_nCurPage)
	{
		pView->GetRadioView()->RadioOnClick(pUI, UIID_WNDID_RADIO, nCtrlId, nBtState);
	}
	else if (SPLIT_PAGE_BT == m_nCurPage)
	{
		pView->GetBTView()->BTOnClick(pUI, UIID_WNDID_BT_MUSIC, MapSplitToBT(nCtrlId), nBtState);
	}
	else
	{
		pView->GetMediaView()->MediaOnClick(pUI, UIID_WNDID_MEDIA_AUDIO_PLAY, MapSplitToMedia(nCtrlId), nBtState);
	}
}

bool SplitView::SplitOnUserMessage(T_APP_MSG* msg)
{
	if (SYS_VMSG_USBMOUNT == msg->Message)
	{
		m_bUSBMounted = (1 == msg->Param0);
	}
	switch (m_nCurPage)
	{
		case SPLIT_PAGE_RADIO:
			if (METER_VMSG_UPDATE_RADIOINFO == msg->Message) SplitUpdateRadio();
			break;
		case SPLIT_PAGE_BT:
			if (METER_VMSG_BT_CONNECTED_STATE == msg->Message)
			{
				// 连接/断开要切换 No Device 与正常卡片
				SplitUpdateNoDevice(SPLIT_PAGE_BT);
				SplitUpdateBTMusic();
			}
			else if (METER_VMSG_BT_MUSIC_INFO == msg->Message ||
				METER_VMSG_BT_MUSIC_TIME == msg->Message ||
				METER_VMSG_BT_MUSICPLAY_STATE == msg->Message)
			{
				SplitUpdateBTMusic();
			}
			break;
		case SPLIT_PAGE_USB:
			if (SYS_VMSG_USBMOUNT == msg->Message)
			{
				// 插拔 U 盘要切换 No Device 与正常卡片
				SplitUpdateNoDevice(SPLIT_PAGE_USB);
				SplitUpdateUSB();
			}
			else if (MEDIA_VMSG_PLAYSTATUS == msg->Message ||
				MEDIA_VMSG_PLAYTIME == msg->Message)
			{
				SplitUpdateUSB();
			}
			break;
	}
	return true;
}

/* 默认页：按当前音源选 */
int SplitView::SplitPickDefaultPage()
{
	int nSrc = ((IVIView*)GetViewBase())->Get_CurSource();
	if (IVI_SOURCE_BT_MUSIC == nSrc) return SPLIT_PAGE_BT;
	if (IVI_SOURCE_MEDIA    == nSrc) return SPLIT_PAGE_USB;
	return SPLIT_PAGE_RADIO;
}

/* 滑动切页 */
void SplitView::SplitSwipe(int nDir)
{
	int nNext = m_nCurPage + nDir;
	if (nNext < 0) nNext = SPLIT_PAGE_USB;
	if (nNext > SPLIT_PAGE_USB) nNext = SPLIT_PAGE_RADIO;
	SplitSetPage(nNext);
}

/* ==================== ID 映射：分屏卡片ID → 原窗口ID ==================== */
int SplitView::MapSplitToBT(int nCtrlId)
{
	switch (nCtrlId)
	{
		case 1019: return 1004;  //歌名
		case 1020: return 1005;  //歌手
		case 1021: return 1006;  //专辑
		case 1022: return 1007;  //封面
		case 1023: return 1008;  //进度条
		case 1012: return 1012;  //当前时间
		case 1027: return 1013;  //总时长
		case 1009: return 1009;  //上一曲
		case 1024: return 1010;  //播放
		case 1025: return 1010;  //暂停
		case 1026: return 1011;  //下一曲
		default: return nCtrlId;
	}
}

int SplitView::MapSplitToMedia(int nCtrlId)
{
	switch (nCtrlId)
	{
		case 1030: return 1004;  //歌名
		case 1031: return 1005;  //歌手
		case 1032: return 1006;  //专辑
		case 1033: return 1007;  //封面
		case 1034: return 1008;  //进度条
		case 1039: return 1016;  //当前时间
		case 1040: return 1017;  //总时长
		case 1035: return 1009;  //上一曲
		case 1036: return 1010;  //播放
		case 1037: return 1010;  //暂停
		case 1038: return 1011;  //下一曲
		case 1041: return 1012;  //循环
		case 1042: return 1013;  //随机
		default: return nCtrlId;
	}
}

/* ==================== Radio 卡片数据刷新 ==================== */
void SplitView::SplitUpdateRadio()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr) return;
	CUIAnalysis* p = pUIMgr->GetUIAnalysisObj();
	if (NULL == p) return;

	RadioData &mData = RadioData::GetInstance();
	T_RADIO_CURR_FREQ tFreq;          // 正确类型
	mData.GetCurrFreq(tFreq);

	int nCurrBand = tFreq.nCurrBand;   // 正确字段
	int nCurrFreq = tFreq.nCurrFreq;   // 正确字段
	wchar_t wszBuf[64] = {0};

	if (FM == nCurrBand)
	{
		float fFreq = 0;
		if (FM_FREQ_MIN >= nCurrFreq)      fFreq = FM_FREQ_MIN / 100.0;
		else if (FM_FREQ_MAX <= nCurrFreq) fFreq = FM_FREQ_MAX / 100.0;
		else                               fFreq = nCurrFreq / 100.0;

		swprintf(wszBuf, 64, L"%0.1f", fFreq);
		p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_RADIO_BAND, UIID_TEXTID_RADIO_FM1_TEXT);
		p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_RADIO_UNIT, 110);   // MHz
	}
	else
	{
		int nAMFreq = nCurrFreq;
		if (AM_FREQ_MIN >= nCurrFreq)      nAMFreq = AM_FREQ_MIN;
		else if (AM_FREQ_MAX <= nCurrFreq) nAMFreq = AM_FREQ_MAX;

		swprintf(wszBuf, 64, L"%d", nAMFreq);
		p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_RADIO_BAND, UIID_TEXTID_RADIO_AM1_TEXT);
		p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_RADIO_UNIT, 108);   // kHz
	}
	p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_RADIO_FREQ, wszBuf);
}

/* ==================== BT 音乐卡片数据刷新 ==================== */
void SplitView::SplitUpdateBTMusic()
{
	CUIAnalysis* p = GetViewBase()->GetUIWndMgrWrapper()->GetUIAnalysisObj();
	if (NULL == p) return;
	BTData &mBT = BTData::GetInstance();

	// 歌名/歌手/专辑
	BtMusicInfo_t tInfo;
	mBT.GetBTMusicInfo(tInfo);
	char szTitle[2*1024] = {0}, szArtist[2*1024] = {0}, szAlbum[2*1024] = {0};
	size_t nLen = 1024;
	BTView *pBTView = ((IVIView*)GetViewBase())->GetBTView();
	pBTView->ConverDecode((char*)tInfo.abTitle,  szTitle,  nLen);
	nLen = 1024;
	pBTView->ConverDecode((char*)tInfo.abArtist, szArtist, nLen);
	nLen = 1024;
	pBTView->ConverDecode((char*)tInfo.abAlbum,  szAlbum,  nLen);
	p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_BT_TITLE,  (wchar_t*)szTitle);
	p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_BT_ARTIST, (wchar_t*)szArtist);
	p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_BT_ALBUM,  (wchar_t*)szAlbum);

	// 播放/暂停图标
	int nStatus = mBT.GetBTMusicStatus();
	if (1 == nStatus)
	{
		p->SetItemIcon(UIID_WNDID_SPLIT, UIID_SPLIT_BT_PAUSE, 211, ITEM_STATE_NORMAL);
		p->SetItemIcon(UIID_WNDID_SPLIT, UIID_SPLIT_BT_PAUSE, 212, ITEM_STATE_PUSH);
		p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_BT_PLAY,  false);
		p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_BT_PAUSE, true);
	}
	else
	{
		p->SetItemIcon(UIID_WNDID_SPLIT, UIID_SPLIT_BT_PLAY, 214, ITEM_STATE_NORMAL);
		p->SetItemIcon(UIID_WNDID_SPLIT, UIID_SPLIT_BT_PLAY, 215, ITEM_STATE_PUSH);
		p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_BT_PLAY,  true);
		p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_BT_PAUSE, false);
	}

	// 时间
	BtPlaybackTime_t tTime;
	mBT.GetBTMusicTime(tTime);
	int nCur   = tTime.dCurrentTime / 1000;    // ← 毫秒转秒
	int nTotal = tTime.dTotalTime   / 1000;    // ← 毫秒转秒
	wchar_t wszTime[32] = {0}, wszTotal[32] = {0};
	swprintf(wszTime,  32, L"%02d:%02d", nCur   / 60, nCur   % 60);
	swprintf(wszTotal, 32, L"%02d:%02d", nTotal / 60, nTotal % 60);
	p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_BT_TIME,  wszTime);
	p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_BT_TOTAL, wszTotal);

	// 进度条
	int nPercent = (nTotal > 0) ? (nCur * 100 / nTotal) : 0;
	p->SetProgressItemPos(UIID_WNDID_SPLIT, UIID_SPLIT_BT_PROGRESS, nPercent);
}

/* ==================== USB 音乐卡片数据刷新 ==================== */
void SplitView::SplitUpdateUSB()
{
	CUIAnalysis* p = GetViewBase()->GetUIWndMgrWrapper()->GetUIAnalysisObj();
	MediaView* pMedia = ((IVIView*)GetViewBase())->GetMediaView();
	if (NULL == p || NULL == pMedia) return;

	int nStatus = 0, nPos = 0, nTotal = 0;
	char szUrl[512] = {0};
	pMedia->GetPlayInfo(nStatus, nPos, nTotal, szUrl, sizeof(szUrl));

	static int s_nLastStatus = -1;
	if (s_nLastStatus != nStatus)
	{
		s_nLastStatus = nStatus;
		if (1 == nStatus)
		{
			p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_PLAY,  false);
			p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_PAUSE, true);
		}
		else
		{
			p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_PLAY,  true);
			p->ShowItem(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_PAUSE, false);
		}
	}

	// 时间
	wchar_t wszTime[32] = {0}, wszTotal[32] = {0};
	swprintf(wszTime,  32, L"%02d:%02d", nPos   / 60, nPos   % 60);
	swprintf(wszTotal, 32, L"%02d:%02d", nTotal / 60, nTotal % 60);
	p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_TIME,  wszTime);
	static wchar_t s_wszTotal[32] = {0};
	if (0 != wcscmp(s_wszTotal, wszTotal))
	{
		wcscpy(s_wszTotal, wszTotal);
		p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_TOTAL, wszTotal);
	}

	// 进度条（百分比）
	int nPercent = (nTotal > 0) ? (nPos * 100 / nTotal) : 0;
	p->SetProgressItemPos(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_PROGRESS, nPercent);

	// 歌名/歌手/专辑
	wchar_t szTitle[256] = {0}, szArtist[256] = {0}, szAlbum[256] = {0};
	pMedia->GetPlayID3Info(szTitle, szArtist, szAlbum, 256);

	static wchar_t s_szTitle[256]  = {0};
	static wchar_t s_szArtist[256] = {0};
	static wchar_t s_szAlbum[256]  = {0};
	if (0 != wcscmp(s_szTitle, szTitle))
	{
		wcscpy(s_szTitle, szTitle);
		p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_TITLE, szTitle);
	}
	if (0 != wcscmp(s_szArtist, szArtist))
	{
		wcscpy(s_szArtist, szArtist);
		p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_ARTIST, szArtist);
	}
	if (0 != wcscmp(s_szAlbum, szAlbum))
	{
		wcscpy(s_szAlbum, szAlbum);
		p->SetItemText(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_ALBUM, szAlbum);
	}

	// 播放模式 / 随机播放图标（和全屏媒体页保持一致）
	int nPicMode = (PLAY_MODE_ONE == pMedia->GetPlayMode()) ? 224 : 220;
	static int s_nLastMode = -1;
	if (s_nLastMode != nPicMode)
	{
		s_nLastMode = nPicMode;
		p->SetItemIcon(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_LOOP, nPicMode, ALL_STATE);
	}

	int nPicShuffle = pMedia->GetPlayShuffle() ? 234 : 232;
	static int s_nLastShuffle = -1;
	if (s_nLastShuffle != nPicShuffle)
	{
		s_nLastShuffle = nPicShuffle;
		p->SetItemIcon(UIID_WNDID_SPLIT, UIID_SPLIT_MEDIA_SHUFFLE, nPicShuffle, ALL_STATE);
	}
}

/* 分屏媒体卡片跟随播放刷新：MediaView 每秒 tick 时调用 */
void SplitView::SplitRefreshMedia()
{
	if (SPLIT_PAGE_USB == m_nCurPage)
	{
		SplitUpdateUSB();
	}
}