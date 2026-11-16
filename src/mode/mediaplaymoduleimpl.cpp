#include "mediaplaymoduleimpl.h"
#include "log.h"
#include "audiocontrol.h"
#include "IVIController.h"

MediaplayModuleImpl::MediaplayModuleImpl()
{
    //todo
    m_pCtrl = NULL;
}

MediaplayModuleImpl::MediaplayModuleImpl(IVIController *pCtrl)
{
    m_pCtrl = pCtrl;
}

MediaplayModuleImpl::~MediaplayModuleImpl()
{
    //todo
}

void MediaplayModuleImpl::taked(ResourceType type)
{
    LOGDBG("&&&---taked:%d\n",type);

    if(NULL != m_pCtrl)
    {
        m_pCtrl->SetAudioTake(false);
        m_pCtrl->EnableAudio(false);

        m_pCtrl->SendMeterInfoData(RADIO_BREAK,0,0,0);
        m_pCtrl->SendMeterInfoData(RADIO_MUTE,0,0,0);
    }
}

void MediaplayModuleImpl::untaked(ResourceType type)
{
    LOGDBG("&&&---untaked:%d\n",type);
}

void MediaplayModuleImpl::borrowed(ResourceType type)
{
    LOGDBG("&&&---borrowed:%d\n",type);
    if(NULL != m_pCtrl)
    {
        m_pCtrl->EnableAudio(false);

        m_pCtrl->SendMeterInfoData(RADIO_BREAK,0,0,0);
    }
}

void MediaplayModuleImpl::unborrowed(ResourceType type)
{
    LOGDBG("&&&---unborrowed:%d\n",type);

    if(NULL != m_pCtrl)
    {
        m_pCtrl->unborrowed();
    }
}

void MediaplayModuleImpl::action(ResourceType type, ResourceAction &action)
{
    LOGDBG("&&&---action:%d\n",type);
}


