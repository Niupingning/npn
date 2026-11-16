#ifndef _RESOURCE_H_
#define _RESOURCE_H_


#define SCR_RSC "MainScreen"
#define AUD_RSC "MainAudio"
#define AUD_REAR_RSC "RearAudio"
#define ACT_RSC "Activation"

//#define DUAL_SCREEN

typedef enum RESOURCE_PRIORITY {
    EmptyPriority = 6,          //undefine
    EmergencyPriority = 5,      //rear camera,flat tire
    UrgentPriority = 4,             //reserved
    ImportantPriority = 3,          //phone calling
    HighPriority = 2,           //speech recognize
    NormalPriority = 1,             //attach,detach,notifier
    LowPriority = 0,            //nothing,like home
} ResourcePriority;



#endif
