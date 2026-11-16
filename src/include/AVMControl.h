#ifndef _AVM_CONTROL_H_
#define _AVM_CONTROL_H_

#include "AuxinModule.h"
#include "AppDatadef.h"
#include "HomeData.h"

class AVMControl:public SPSignalChangeCallback
{
public:
    AVMControl();
    void onCVBSSignalChanged(int status);// CVBS信号变化回调

    void streamOn();
    void streamOff();
    void InitDispWin();     // 重新设置 AVM 视频层为 1920x720

private:
   AuxinModule m_nAuxinObj;
   SPVideoIn *spInstance;// 视频输入控制实例

};



#endif
