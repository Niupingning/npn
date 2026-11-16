#ifndef SPLITVIEW_H_
#define SPLITVIEW_H_
#include "CompBase.h"
#include "IVIController.h"

class SplitView : public CompBase
{
public:
	SplitView(ViewBase* p_ViewBase);
	void SplitAttachController(IVIController* p){ m_ViewController = p; }

	void SplitInitWorkWnd();
	void SplitSetPage(int nPage);
	void SplitOnClick(CUIWndMng* pUI, long nCtrlId, int nBtState);
	bool SplitOnUserMessage(T_APP_MSG* msg);
	void SplitSwipe(int nDir);

private:
	void SplitUpdateRadio();
	void SplitUpdateBTMusic();
	void SplitUpdateUSB();
	void SplitUpdatePageDot();
	void SplitUpdateNoDevice(int nPage);
	int  SplitPickDefaultPage();
	int  MapSplitToBT(int nCtrlId);
	int  MapSplitToMedia(int nCtrlId);

	IVIController* m_ViewController;
	int  m_nCurPage;
	bool m_bUSBMounted;   // U盘是否挂载
};
#endif