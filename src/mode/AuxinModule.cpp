#include "AuxinModule.h"
//#include "generalMacroDef.h"
//#include "logger.h"
#include "log.h"
#include <math.h>

#define PI 3.14159265

static SPReverseAssistLine *assline;


AuxinModule::AuxinModule():
    videoInstance(nullptr),
    m_DispCtrl(nullptr),
     m_srcPlane(User_VPP0),
      m_bright(0)
     , m_contrast(0)

{
    initial();
}

int AuxinModule::initial()
{
    m_DispCtrl = SPDispControl::GetInstance();
    videoInstance = SPVideoIn::GetInstance();
    assline = SPReverseAssistLine::GetInstance();
    memset(&m_color, 0, sizeof(User_VPP_Favorite_Color));


//    m_DispCtrl->DispSrv_Get_Video_Luma(m_srcPlane, &m_bright, &m_contrast);
//    m_DispCtrl->DispSrv_Get_Video_FCA(m_srcPlane, &m_color);


    return 0;
}


void AuxinModule::changeToParkinMode()//模式切换函数
{
    setAVInMode(AuxinModule::ParkIn);
}


void AuxinModule::setVideoContrast(int contrast)//对比度调整
{

    bool valid = true;

    if(!valid)
    {
        return;
    }
    m_contrast = contrast;
    //DISP_D( "set contrast" << contrast << m_bright << m_contrast);
    DispResult_e ret = m_DispCtrl->DispSrv_Set_Video_Luma(m_srcPlane, m_bright, contrast);
    if(ret == DISPC_SUCCESS)
    {
    }
    else
    {
    }
}

void AuxinModule::setVideoSaturation(int saturation)//饱和度调整
{
    bool valid = true;

    if(!valid) {
        return;
    }

    m_color.saturation = saturation;
    m_color.satsin = (int)(512 * sin(m_color.hue * PI / 180.0) * (m_color.saturation / 100.00));
    m_color.satcos = (int)(512 * cos(m_color.hue * PI / 180.0)  * (m_color.saturation / 100.00));
    //DISP_D( "set saturation" << saturation << m_color.hue << m_color.saturation << "sin" << m_color.satsin << "cos" << m_color.satcos);
    DispResult_e ret = m_DispCtrl->DispSrv_Set_Video_FCA(m_srcPlane, &m_color);
    if(ret == DISPC_SUCCESS)
    {

    }
    else
    {

    }
}
void AuxinModule::setVideoHue(int hue)//色调调整
{
    bool valid = true;


    if(!valid) {
        return;
    }
    m_color.hue = hue;
    m_color.satsin = (int)(512 * sin(m_color.hue * PI / 180.0) * (m_color.saturation / 100.00));
    m_color.satcos = (int)(512 * cos(m_color.hue * PI / 180.0)  * (m_color.saturation / 100.00));
   // DISP_D( "set hue" << hue << m_color.hue << m_color.saturation << "sin" << m_color.satsin << "cos" << m_color.satcos);
    DispResult_e ret = m_DispCtrl->DispSrv_Set_Video_FCA(m_srcPlane, &m_color);

    if(ret == DISPC_SUCCESS)
    {
    }
    else
    {
    }
}
void AuxinModule::setAVInMode(AuxinModule::AVInMode mode)//核心模式设置函数
{
    //CHECK_POINT_NULL(videoInstance);
   //CHECK_POINT_NULL(m_DispCtrl);

    // 1. 选择CVBS0通道（复合视频信号）
    VideoInStatus_e eRet = VIDEOIN_FAILED;
    eRet = videoInstance->selectChannel(SP_VIDEO_CH_CVBS0);

    // 2. 设置视频源到主显示
    unsigned int videoSource = User_VPP_SRC_AVIN0; //User_VPP_SRC_AVIN0
    DispResult_e ret = m_DispCtrl->DispSrv_SetVideoSrcToDisp(videoSource, User_MainDisp);

    // 3. 设置显示窗口（右半屏显示）
    LOGDBG("@@@set main DispSrv_SetVideoSrcToDisp ret = %d.\n", ret);
    // User_DISP_Win_t pixelsRect = {static_cast<unsigned int>(640), 0,static_cast<unsigned int>(1280), \
    //                       static_cast<unsigned int>(720)};//640
    // User_DISP_Win_t pixelsRect = {0, 0,static_cast<unsigned int>(1024), \
    //                       static_cast<unsigned int>(600)};
    User_DISP_Win_t pixelsRect = {0, 0,
                          static_cast<unsigned int>(1920),
                          static_cast<unsigned int>(720)};
    ret = m_DispCtrl->DispSrv_SetDispWin("VideoIn", &pixelsRect);

    LOGDBG("@@@set main DispSrv_SetDispWin ret = %d.\n", ret);

}

void AuxinModule::streamOn()//启动视频流
{
    //CHECK_POINT_NULL(videoInstance);
     videoInstance->streamOn();
}

void AuxinModule::streamOff()//停止视频流
{
    //CHECK_POINT_NULL(videoInstance);
    //CHECK_POINT_NULL(m_DispCtrl);

    videoInstance->streamOff();
    unsigned int videoSource = User_VPP_SRC_AVIN0_TO_NONE;
    DispResult_e ret = m_DispCtrl->DispSrv_SetVideoSrcToDisp(videoSource, User_MainDisp);
}

void AuxinModule::stopEcosCVBS()//ECU通信控制函数
{
    /*LOGDBG("#stopEcosCVBS\n");
    CHECK_POINT_NULL(assline);
    // 检查UART连接状态
    bool disconnected;
    ReverseCtrlStatus_e ret = assline->isRemoteUartDisconnected(&disconnected);
    LOGDBG(" isRemoteUartDisconnected ret = %d, connet:%d \n", ret, disconnected);
    // 通知MCU断开UART连接
    assline->notifyDisconnectMcuUart();*/
}


//-ldispap
