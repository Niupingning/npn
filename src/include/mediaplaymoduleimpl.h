#ifndef _MEDIAPLAY_MODULEIMPL_H_
#define _MEDIAPLAY_MODULEIMPL_H_

#include "mediaresourcecontrol.h"

class IVIController;
class MediaplayModuleImpl: public ResourceTransfer
{
public:
    MediaplayModuleImpl();
    MediaplayModuleImpl(IVIController *pCtrl);
    virtual ~MediaplayModuleImpl();

    void taked(ResourceType type);
    void untaked(ResourceType type);
    void borrowed(ResourceType type);
    void unborrowed(ResourceType type);
    void action(ResourceType type, ResourceAction &action);

public:
    IVIController      *m_pCtrl;
};

#endif //_MEDIAPLAY_MODULEIMPL_H_
