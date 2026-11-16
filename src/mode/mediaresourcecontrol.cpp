
#include "mediaresourcecontrol.h"
#include "resource.h"
#include <string>

using std::string;

ResourceTransferProgramCbk::ResourceTransferProgramCbk(ResourceTransfer *rt, ResourceType type)
    : transfer(rt), resourceType(type)
{

}

ResourceTransferProgramCbk::~ResourceTransferProgramCbk()
{

}

void ResourceTransferProgramCbk::taked()
{
    if(transfer) 
    {
        transfer->taked(resourceType);
    }
}

void ResourceTransferProgramCbk::untaked()
{
    if(transfer) 
    {
        transfer->untaked(resourceType);
    }
}

void ResourceTransferProgramCbk::borrowed()
{
    if(transfer) 
    {
        transfer->borrowed(resourceType);
    }
}

void ResourceTransferProgramCbk::unborrowed()
{
    if(transfer) 
    {
        transfer->unborrowed(resourceType);
    }
}

void ResourceTransferProgramCbk::action(ResourceAction &action)
{
    if(transfer) 
    {
        transfer->action(resourceType, action);
    }
}


MediaResourceControl::MediaResourceControl()
    : resourceMax(ResInvalid)
    , curResourceBit(ResInvalid)
{

}

MediaResourceControl::~MediaResourceControl()
{

}

shared_ptr<ResourceHolder> MediaResourceControl::getHolder(ResourceType type)
{
    if(type == ResAudio) 
    {
        return audioHolder;
    } 
    else if(type == ResRearAudio) 
    {
        return audioRearHolder;
    } 
    else 
    {
        return screenHolder;
    }
}

void MediaResourceControl::associateResource(shared_ptr<ResourceTransferProgram> cbk, ResourceType type)
{
    if(type == ResAudio) 
    {
        string resAudio(AUD_RSC);
        audioHolder = ResourceManager::self()->associate(resAudio, \
                      (shared_ptr<ResourceTransferProgram> &)cbk);
        audioHolder->setHolderName("radio_play");
        audioHolder->setUsagePriority(NormalPriority);
    } 
    else if(type == ResRearAudio) 
    {
        string resAudio(AUD_REAR_RSC);
        audioRearHolder = ResourceManager::self()->associate(resAudio, \
                          (shared_ptr<ResourceTransferProgram> &)cbk);
        audioRearHolder->setHolderName("radio_play");
        audioRearHolder->setUsagePriority(NormalPriority);
    } else 
    {
        string resVpp(SCR_RSC);
        screenHolder    = ResourceManager::self()->associate(resVpp, \
                          (shared_ptr<ResourceTransferProgram> &)cbk);

        screenHolder->setHolderName("radio_play");
        screenHolder->setUsagePriority(NormalPriority);
    }
}

void MediaResourceControl::disassociateResource(ResourceType type)
{
    //Q_UNUSED(type);
    if(type == ResAudio)
    {
        ResourceManager::self()->disassociate(audioHolder);
    }
    else if(type == ResRearAudio)
    {
        ResourceManager::self()->disassociate(audioRearHolder);
    }
    else 
    {
        ResourceManager::self()->disassociate(screenHolder);
    }
}

void MediaResourceControl::setResourceBit(ResourceType res)
{
    curResourceBit |= res;
    //CHANGE_RES_SETTING("set resource", res, curResourceBit);
}

void MediaResourceControl::unsetResourceBit(ResourceType res)
{
    curResourceBit &= ~res;
    //CHANGE_RES_SETTING("unset resource", res, curResourceBit);
}

bool MediaResourceControl::checkResourceBit(ResourceType res)
{
    return (curResourceBit & res);
}

