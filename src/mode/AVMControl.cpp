#include "AVMControl.h"
#include "log.h"

AVMControl::AVMControl():spInstance(nullptr),m_nAuxinObj()
{
    m_nAuxinObj.changeToParkinMode();
    spInstance = SPVideoIn::GetInstance();
    if(NULL == spInstance)
    {
        LOGERR("SPVideoIn GetInstance failed \n");
    }
    else
    {
        spInstance->RegisterCallback(this);
    }  
}

void AVMControl::onCVBSSignalChanged(int status)
{
    LOGDBG("CVBS status,%d \n", status);

    const int CVBS_SIG_LOSS = 0;
    const int CVBS_SIG_WON  = 1;

    HomeData &mHomeData = HomeData::GetInstance();
  
    switch (status)
    {
        case CVBS_SIG_LOSS: // loss
        {
            mHomeData.SaveINTData(INT_AVM_SIGN,0);
            break;
        }
        case CVBS_SIG_WON:
        {
             mHomeData.SaveINTData(INT_AVM_SIGN,1);
            break;
        }
        default:
        {
            break;
        } 
    }
}

void AVMControl::streamOn()
{
    return m_nAuxinObj.streamOn();
}

void AVMControl::streamOff()
{
    return m_nAuxinObj.streamOff();
}

void AVMControl::InitDispWin()
{
    m_nAuxinObj.changeToParkinMode();
}