#ifndef _AUXIN_MODULE_H_
#define _AUXIN_MODULE_H_

#include <sys/types.h>
#include <unistd.h>
#include <mutex>
#include <cstring>

#include "SPVideoIn.h"
//#include "displaycontrol.h"
#include "SPDisplay.h"
#include "Singleton.h"


#include "SPReverseAssistLine.h"
using namespace  reversectrl;

using namespace videoin;
using namespace dispctrl;

class AuxinModule:public Singleton<AuxinModule>
{
public:
    enum AVInMode {
        AVIn,
        CameraIn,
        ParkIn,
        AuxIn,
        InvalidAVIn
    };

    enum AudioChannel {
        AudioChannel0 = 0,
        AudioChannel1,
        AudioChannel2,
        AudioChannel3,
        AudioChannel4,
        AudioChannelInvalid
    };

    enum VideoChannel {
        VideoChannel0 = 0,
        VideoChannel1,
        VideoChannel2,
        VideoChannel3,
        VideoChannel4,
        VideoChannelInvalid
    };


    AuxinModule();
    void changeToParkinMode();
    void setAVInMode(AuxinModule::AVInMode mode);
    void streamOn();
    void streamOff();
    void stopEcosCVBS();

    void setVideoHue(int hue);
    void setVideoSaturation(int saturation);
    void setVideoContrast(int contrast);


private:
    int initial();


private:
    SPVideoIn *videoInstance;// 视频输入控制实例
   // DisplayControl *dispCtrl;
    SPDispControl *m_DispCtrl;// 显示控制实例
    std::mutex    ecosControlMutx_;// ECU控制互斥锁
    unsigned int m_srcPlane;// 视频源平面(User_VPP0)
    User_VPP_Favorite_Color m_color;// 颜色调整参数
    unsigned int m_bright;// 亮度值
    unsigned int m_contrast;// 对比度值




};



#endif
