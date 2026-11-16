#ifndef MEDIARESOURCECONTROL_H
#define MEDIARESOURCECONTROL_H
#include "ResourceManager.h"

using namespace resourcemanager;

enum ResourceType 
{
    ResInvalid    = 0,
    ResScreen     = 0x0001,
    ResAudio      = 0x0002,
    ResRearAudio  = 0x0004,
    ResVpp        = 0x0008,
    ResSecondDisp = 0x0010
};

class MediaResourceControl
{
public:
    MediaResourceControl();
    ~MediaResourceControl();
    shared_ptr<ResourceHolder> getHolder(ResourceType type);

    void associateResource(shared_ptr<ResourceTransferProgram> cbk, ResourceType type);
    void disassociateResource(ResourceType type);

    void setResourceBit(ResourceType res);
    void unsetResourceBit(ResourceType res);
    bool checkResourceBit(ResourceType res);
private:
    shared_ptr<ResourceHolder> audioHolder;
    shared_ptr<ResourceHolder> audioRearHolder;
    shared_ptr<ResourceHolder> screenHolder;
    int resourceMax;
    int curResourceBit;

};

class ResourceTransfer
{
public:
    ResourceTransfer() {}
    virtual ~ResourceTransfer() {}

    virtual void taked(ResourceType type) = 0;
    virtual void untaked(ResourceType type) = 0;
    virtual void borrowed(ResourceType type) = 0;
    virtual void unborrowed(ResourceType type) = 0;
    virtual void action(ResourceType type, ResourceAction &action) = 0;
};

class ResourceTransferProgramCbk: public ResourceTransferProgram
{
public:
    ResourceTransferProgramCbk(ResourceTransfer *rt, ResourceType type);
    virtual ~ResourceTransferProgramCbk();

    virtual void taked();
    virtual void untaked();
    virtual void borrowed();
    virtual void unborrowed();
    virtual void action(ResourceAction &action);
private:
    ResourceTransfer *transfer;
    ResourceType resourceType;
};



#endif // MEDIARESOURCECONTROL_H
