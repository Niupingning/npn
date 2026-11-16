
#include "fyaudiocontrol.h"

#include "log.h"

FyAudioControl *FyAudioControl::adCtrl = FyAudioControl::getInstance();
//ConfigHelper *FyAudioControl::FyAudioConfig =  new ConfigHelper("/media/flash/userdata/audio_boost.conf");
ConfigHelper *FyAudioControl::FyAudioConfig =ConfigHelper::getInstance();

using namespace audio;

FyAudioControl::FyAudioControl()
{    
//    FyAudioConfig = new ConfigHelper("/media/flash/userdata/audio_boost.conf");    
}

FyAudioControl::~FyAudioControl()
{
   
}

void FyAudioControl::FyAudioControlInit()
{
    //read configure and setup dsp
    FyAudioConfig->readConfig();

    if(adCtrl == nullptr)
        return;
    //1. fade/balance
    int fade = 0, balance = 0;
    fade = FyAudioConfig->get("fade");   
    balance = FyAudioConfig->get("balance");
    if(fade != 0 || balance != 0)
    {
        LOGDBG("%s,fade=%d,balance=%d", __func__, fade, balance);
        adCtrl->setFadeBalance(fade, balance);
    }

    //2. loudness
    int loudness_mode = 0;
    loudness_mode = FyAudioConfig->get("loudness");
    if(loudness_mode != 0)
    {   
        LOGDBG("%s,loudness_mode=%d", __func__,loudness_mode);
        adCtrl->setLoudnes(loudness_mode);
    }
}

/* band 规划 standard mode
    1. 0-12 index 用于EQ设置
    2. 13-18 index 用于loudness设置，LOW:13, MID: 14, HIGH:15
    3. 16 index 用于subwoofer
    4. 17 - 27 front/real HPF  standard mode crossover 
    5. 28 - 38 subwoofer LPF  standard mode crossover    
    */

    /* band 规划 3way network mode
    1. 0-12 index 用于EQ设置
    2. 13-18 index 用于loudness设置，LOW:13/14, MID: 15/16, HIGH:17/18
    3. 19 index 用于subwoofer
    4. 20 - 23 high HPF  3way network mode crossover 
    5. 24 - 27 mid HPF   3way network mode crossover 
    6. 28 - 31 mid LPF   3way network mode crossover 
    7. 32 - 35 low LPF   3way network mode crossover 
*/
static unsigned short adEqFreqArray[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {50,80,125,200,315,500,800,1250,2000,3150,5000,8000,12500, //eq
                                                        250, 1000, 5000,                                            //loudness
                                                        100,                                                        //subwoofer
                                                        25, 31, 40, 50, 63, 80, 100, 125, 160, 200, 250,          //17-27 front/real HPF  standard mode crossover
                                                        25, 31, 40, 50, 63, 80, 100, 125, 160, 200, 250};        //28 - 38 subwoofer LPF  standard mode crossover 

//3way netword mode frequence define
static unsigned short adEqFreqArray3[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {50,80,125,200,315,500,800,1250,2000,3150,5000,8000,12500, //eq
                                                        250, 1000, 5000,                                            //loudness
                                                        100,                                                        //subwoofer
                                                        125, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500,          //17-27 high/middle HPF  standard mode crossover
                                                        25, 31, 40, 50, 63, 80, 100, 125, 160, 200, 250};        //28 - 38 subwoofer LPF  standard mode crossover                                                         

static struct AudioEqInfoStruct adEqInfo[EqModeMax]={
    {EqSuperBass, 13, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {8, 8, 6, 6, 2, 0, 0, 0, 0, 2, 2, 4, 2}},
    {EqPowerful, 13,  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {2, 6, 4, 2, 0, -2, 0, 2, 2, 2, 4, 4, 6}},
    {EqNatural, 13,   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {-6, 2, 2, 2, 0, 0, 2, 2, -2, -2, 2, 2, 4}},
    {EqVocal, 13,     {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {-6, -2, 0, 4, 6, 6, 2, 0, 2, 4, 4, 2, 0}},
    {EqFlat, 13,      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {EqCustom1, 13,   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {EqCustom2, 13,   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}       
};

static struct AudioLoudnessInfoStruct adLoudnessInfo[4] = 
{   {FyAudioLoudnessOff, 0xff, 0xff}, 
    {FyAudioLoudnessLow, 13, 6.0f},
    {FyAudioLoudnessMid, 14, 6.0f},
    {FyAudioLoudnessHigh, 15, 6.0f}
};

static struct AudioSpeakerLevelInfoStruct  adSpeakerLevelInfo[6] =
{
    {FyAudioFrontLeft, 0, 0},
    {FyAudioFrontRight, 0, 0},
    {FyAudioRearLeft, 0, 0},
    {FyAudioRearRight, 0, 0},
    {FyAudioCenter, 0, 0},
    {FyAudioSubwoofer, 0, 0}
}; 

FyAudioControl *FyAudioControl::getInstance()
{
    if(!adCtrl) {
        adCtrl = new FyAudioControl();        
    }
    return adCtrl;
}

void FyAudioControl::setDpsEnable(bool en)
{
    int ret;
    audio::SPAudioSetting::App *appSetting;
    appSetting = audio::SPAudioSetting::createApp();
    if(en) {
        ret = appSetting->enableDps();
    } else {
        ret = appSetting->disableDps();
    }
    audio::SPAudioSetting::destroy(appSetting);
    if(0 == ret) {
        LOGDBG("Set Dps successful.");
    } else {
        LOGDBG("Set Dps error.ret:%d", ret);
    }
}

int FyAudioControl::getGainVal(int x, int y, int gainX, int gainY)
{
    int gain = FY_VOLUME_MAX_VAL;

    if(x > 0) {
        gain = gainX;
    }

    if(y < 0) {
        gain = gain - (FY_VOLUME_MAX_VAL - gainY);
        if(gain < 0) {
            gain = 0;
        }
    }

    return gain;
}

/*
 1. balance:x,fade:y
*/
void FyAudioControl::setFadeBalance(int fade, int balance)
{
    int x = balance;
    int y = fade;

    int absX = std::abs(x);
    int absY = std::abs(y);

    LOGDBG("%s,fade=%d,balance=%d", __func__, fade, balance);

    FyAudioConfig->set("fade", fade);
    FyAudioConfig->set("balance", balance);

    if (FyAudioConfig->writeConfig()) {
        LOGDBG("write config sucessfull!"); 
    } else {        
        LOGDBG("write config failure!");      
    }

    if((absX > FY_FADE_BALANCE_STEP_CNT) || (absY > FY_FADE_BALANCE_STEP_CNT)) {
        LOGERR("set value err.");
        return;
    }

    audio_channel_gain_t LFChannel;
    audio_channel_gain_t RFChannel;
    audio_channel_gain_t LSChannel;
    audio_channel_gain_t RSChannel;
    //SPAudio_channel_gain_t temp;

    int gainX = ((FY_FADE_BALANCE_STEP_CNT - absX) * FY_VOLUME_MAX_VAL + (FY_FADE_BALANCE_STEP_CNT >> 1)) / FY_FADE_BALANCE_STEP_CNT;
    int gainY = ((FY_FADE_BALANCE_STEP_CNT - absY) * FY_VOLUME_MAX_VAL + (FY_FADE_BALANCE_STEP_CNT >> 1)) / FY_FADE_BALANCE_STEP_CNT;

    LOGDBG("gainX=%d, gainY=%d", gainX, gainY);

    LFChannel.nChannel = AUDIO_CHANNEL_LF;
    LFChannel.volume = getGainVal(x, y, gainX, gainY);

    RFChannel.volume = getGainVal(-x, y, gainX, gainY);
    RFChannel.nChannel = AUDIO_CHANNEL_RF;

    LSChannel.volume = getGainVal(x, -y, gainX, gainY);
    LSChannel.nChannel = AUDIO_CHANNEL_LS;

    RSChannel.volume = getGainVal(-x, -y, gainX, gainY);
    RSChannel.nChannel = AUDIO_CHANNEL_RS;

    int sdRet = 0;
    audio::SPAudioSetting::Volume *volume;
    volume = audio::SPAudioSetting::createVolume();

    sdRet = volume->setChannelGain(&LFChannel);
    if(sdRet != 0) {
        LOGERR("set LFChannel gain err");
    }

    sdRet = volume->setChannelGain(&RFChannel);
    if(sdRet != 0) {
        LOGERR("set RFChannel gain err");
    }

    sdRet = volume->setChannelGain(&LSChannel);
    if(sdRet != 0) {
        LOGERR("set LSChannel gain err");
    }

    sdRet = volume->setChannelGain(&RSChannel);
    if(sdRet != 0) {
        LOGERR("set RSChannel gain err");
    }
    audio::SPAudioSetting::destroy(volume);
}

void FyAudioControl::setLoudnessBassShelf(int loudness_mode)
{    
    int bandOffset  = 0;
    int bandCount   = 0;
    
//    FyAudioConfig->readConfig();
    LOGDBG("last loudness =%d",FyAudioConfig->get("loudness"));
    if(FyAudioConfig != nullptr)
    {
        FyAudioConfig->set("loudness", loudness_mode); 
        FyAudioConfig->writeConfig();
    }else{
        LOGDBG("loudness config failure");
    }   
       
    uint8_t buffer[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    effect_param_t *param = (effect_param_t *)buffer;
    audio_effect_equalizer_channel_config_param_t v;

    for(int ch = AUDIO_CHANNEL_LF; ch <= AUDIO_CHANNEL_RS; ch++)
    {
        memset(buffer, 0, sizeof(buffer));
        memset(&v, 0, sizeof(v));
        v.id = static_cast<audio_channel_idx_e>(ch);
        v.validConfigCount = 3;         //全关，或关2开1

        for(int i = 1; i <= FyAudioLoudnessHigh; i++ )
        {
            audio_effect_equalizer_channel_band_config_param_t *bassConfig = &v.config[i-1];

            if(adLoudnessInfo[i].Loudness_mode == loudness_mode)
            {         
                bassConfig->bandParam.gain     = adLoudnessInfo[i].gain;
            }else{            
                bassConfig->bandParam.gain     = 0.0f;
            }
            
            bassConfig->bandParam.type     = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_BASSSHELF_Q;
            bassConfig->bandIndex          = adLoudnessInfo[i].FreqIndex;        
            
            bassConfig->bandParam.fc       = adEqFreqArray[bassConfig->bandIndex];
            LOGDBG("loudness mode=%d, freq=%d, gain=%f, index=%d", i, bassConfig->bandParam.fc, bassConfig->bandParam.gain, bassConfig->bandIndex);
            
            bassConfig->bandParam.q        = 0.7f;
            bassConfig->bandParam.slope    = AUDIO_EFFECT_EQUALIZER_BAND_SLOPE_6;
            bassConfig->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;
        }    
            
        audioTlvParameterSerilize(param, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v, sizeof(audio_effect_equalizer_channel_config_param_t));

        SPAudioSetting::App* app = SPAudioSetting::createApp();
        if(app)
        {
            int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param);
            SPAudioSetting::destroy(app);
        }    
    }    
}

/*
2. set loudnes, OFF/LOW/MID/HIGH
*/
 #ifdef USE_OLD_INTERFACE
void FyAudioControl::setLoudnes(int loudnes_mode)
{
    LOGDBG("loudness_mode=%d", loudnes_mode);

    audio_ext_eq_parameter_t eq;
    memset(&eq, 0, sizeof(audio_ext_eq_parameter_t));
    initEqSystemVal(eq);
   
    eq.nChannel = AUDIO_CHANNEL_LINK; 

    switch(loudnes_mode)
    {
        case FyAudioLoudnessOff:           
           
            for(int i = 13; i <= 18; i++)
            {
                if(i % 2 != 1)
                {
                     //BASS_EQ
                    eq.eqValue[i].eBiquadType = AUDIO_Biquad_bass_shelf;    
                    eq.eqValue[i].eMethod = AUDIO_BqMethod_Butterworth;                       
        
                    eq.eqValue[i].nFc =  adEqFreqArray[i];
                    eq.eqValue[i].sGain = 0.0;
                    eq.eqValue[i].sQ = 0;
                }else{
                    //PEAK_EQ
                    eq.eqValue[i].eBiquadType = AUDIO_Biquad_EQ;    
                    eq.eqValue[i].eMethod = AUDIO_BqMethod_Butterworth;
                   
                    eq.eqValue[i].nFc = adEqFreqArray[i];
                    eq.eqValue[i].sGain = 0.0;
                    eq.eqValue[i].sQ = 0;   
                }                                        
            }
            doSetEq(eq);
            break;
        case FyAudioLoudnessLow:
            //BASS_EQ
            eq.eqValue[13].eBiquadType = AUDIO_Biquad_bass_shelf;    
            eq.eqValue[13].eMethod = AUDIO_BqMethod_Butterworth;              
 
            eq.eqValue[13].nFc = 20;
            eq.eqValue[13].sGain = 3.0;
            eq.eqValue[13].sQ = 2.0;

           //PEAK_EQ
            eq.eqValue[14].eBiquadType = AUDIO_Biquad_EQ;    
            eq.eqValue[14].eMethod = AUDIO_BqMethod_Butterworth;

            eq.eqValue[14].nFc = 80;
            eq.eqValue[14].sGain = -3.0;
            eq.eqValue[14].sQ = 1.6;

            doSetEq(eq);     
            break;
        case FyAudioLoudnessMid:
            //BASS_EQ
            eq.eqValue[15].eBiquadType = AUDIO_Biquad_bass_shelf;    
            eq.eqValue[15].eMethod = AUDIO_BqMethod_Butterworth;    
 
            eq.eqValue[15].nFc = 300;
            eq.eqValue[15].sGain = 3.0;
            eq.eqValue[15].sQ = 2.0;

           //PEAK_EQ
            eq.eqValue[16].eBiquadType = AUDIO_Biquad_EQ;    
            eq.eqValue[16].eMethod = AUDIO_BqMethod_Butterworth;

            eq.eqValue[16].nFc = 600;
            eq.eqValue[16].sGain = -3.0;
            eq.eqValue[16].sQ = 1.6;

            doSetEq(eq);    
            break;
        case FyAudioLoudnessHigh:
            //BASS_EQ
            eq.eqValue[17].eBiquadType = AUDIO_Biquad_bass_shelf;    
            eq.eqValue[17].eMethod = AUDIO_BqMethod_Butterworth;       
 
            eq.eqValue[17].nFc = 2000;
            eq.eqValue[17].sGain = 3.0;
            eq.eqValue[17].sQ = 2.0;

           //PEAK_EQ
            eq.eqValue[18].eBiquadType = AUDIO_Biquad_EQ;    
            eq.eqValue[18].eMethod = AUDIO_BqMethod_Butterworth;

            eq.eqValue[18].nFc = 2300;
            eq.eqValue[18].sGain = -3.0;
            eq.eqValue[18].sQ = 1.6;

            doSetEq(eq);     
            break;
        default:
            LOGERR("set loudnes mode err");
    }
}
#else
void FyAudioControl::setLoudnes(int loudnes_mode)
{
    LOGDBG("loudness_mode=%d", loudnes_mode);    
    setLoudnessBassShelf(loudnes_mode);
}

#endif

/*
3. set speaker level, channel：fl, fr, rl,rr, DB Range：-24-10，Step size is 1
*/
void FyAudioControl::setSpeakerLevel(int listening_position, int channel, unsigned int gain)
{
    LOGDBG("listening_position=%d, channel=%d, gain=%d", listening_position, channel, gain);
    if(channel >= FyAudioChannelMax)
    {
        LOGERR("err");
        return;
    }

    char speaker_name[32] = {0};
    sprintf(speaker_name, "channel%d_l%d_speaker_level", channel, listening_position);
    FyAudioConfig->set(speaker_name, gain);
    if (FyAudioConfig->writeConfig()) {
        LOGDBG("write config sucessfull!");        
    } else {
        LOGDBG("write config failure!");       
    }
    gain += 24;
    gain *= 3;

    if(gain > 100)
        gain = 100;

    adSpeakerLevelInfo[channel].gain_value = gain;

    audio_channel_gain_t SpeakerChannel;
    
    SpeakerChannel.nChannel = getSPAudioChannel(static_cast<FyAudioOutChannel>(channel));      

    int sdRet = 0;
    audio::SPAudioSetting::Volume *volume = nullptr;
    volume = audio::SPAudioSetting::createVolume(); 

    SpeakerChannel.volume = adSpeakerLevelInfo[channel].gain_value;    

    sdRet = volume->setChannelGain(&SpeakerChannel);
    if(sdRet != 0) {
        LOGERR("set SpeakerChannel gain err");
    }   
    audio::SPAudioSetting::destroy(volume); 

}

void FyAudioControl::setSpeakerLevelMode(int listening_position)
{
    audio_channel_gain_t SpeakerChannel;
    audio::SPAudioSetting::Volume *volume = nullptr;
    volume = audio::SPAudioSetting::createVolume();    

    char speaker_name[32] = {0};
    int gain = 0;
    int sdRet = 0;

    for(int channel = FyAudioFrontLeft; channel <= FyAudioSubwoofer; channel++)
    {
        if(channel == FyAudioCenter)
        {
            continue;
        }
        sprintf(speaker_name, "channel%d_l%d_speaker_level", channel, listening_position);
        gain = FyAudioConfig->get(speaker_name);
        
        LOGDBG("%s=%d", speaker_name, gain);             
        
        SpeakerChannel.nChannel = getSPAudioChannel(static_cast<FyAudioOutChannel>(channel));
             
        gain += 24;
        gain *= 3;

        if(gain > 100)
            gain = 100;

        SpeakerChannel.volume = gain;    

        sdRet = volume->setChannelGain(&SpeakerChannel);
        if(sdRet != 0) {
            LOGERR("set SpeakerChannel gain err");
        } 
    }

    if(volume != nullptr)
    {
        audio::SPAudioSetting::destroy(volume);
    }        
}

/*
4. set listening position, FRONT/FRONT-L/FRONT-R/ALL(ONLY FOR 3WAY NETWORK MODE)/OFF(default)
   By setting the listening position, the listening position setting in Speaker Level and Time Alignment also be switched.
*/
void FyAudioControl::setListeningPosition(unsigned char listening_position)
{
    LOGDBG("listening_position=%d", listening_position);
    if(listening_position >= ListeningPositionUnknow)
    {
        LOGERR("err");
    }
    FyAudioConfig->set("listening_position", listening_position);
    if (FyAudioConfig->writeConfig()) {
        LOGDBG("write config sucessfull!");
    } else {
        LOGDBG("write config failure!");    
    }
    //根据监听位置，修改喇叭的增益输出和延时输出
    //每一个监听位置对应一套数据
    setSpeakerLevelMode(listening_position);
    setTimeAlignmentMode(listening_position);
    
}

audio_channel_idx_e FyAudioControl::getSPAudioChannel(FyAudioOutChannel channel)
{
    switch(channel) {
    case FyAudioFrontLeft:
        return AUDIO_CHANNEL_LF;
        break;
    case FyAudioFrontRight:
        return AUDIO_CHANNEL_RF;
        break;
    case FyAudioRearLeft:
        return AUDIO_CHANNEL_LS;
        break;
    case FyAudioRearRight:
        return AUDIO_CHANNEL_RS;
        break;
    case FyAudioSubwoofer:
        return AUDIO_CHANNEL_LFE;
        break;
    case FyAudioCenter:
        return AUDIO_CHANNEL_CENTER;
        break;
    default:
        break;
    }

    return AUDIO_CHANNEL_LINK;
}

void FyAudioControl::setSpeakerDealy(FyAudioOutChannel channel, float value)
{
    audio_channel_idx_e spChannel = getSPAudioChannel(channel);

    audio::SPAudioSetting::App *appSetting;
    appSetting = audio::SPAudioSetting::createApp();

    int ret  = appSetting->setSpeakerDelay(spChannel, value);

    audio::SPAudioSetting::destroy(appSetting);

    if(0 != ret) {
        LOGERR("set speaker delay error.ret:%d", ret);
    }
}

/*
5. set time alignment, position:FRONT/FRONT-L/FRONT-R/SUBWOOFER/OFF, alignment:eg 100cm
*/
void FyAudioControl::setTimeAlignment(int listening_position, int channel, float distance_cm)
{
    LOGDBG("listening_position=%d, channel=%d, distance_cm=%f", listening_position, channel, distance_cm);
    if(channel >= FyAudioChannelMax || distance_cm > 490)
    {
        LOGERR("err");
    }

    char config_item_name[32] = {0};
    sprintf(config_item_name, "f_channel%d_time_alignment_p%d", channel, listening_position);
    FyAudioConfig->setFloat(config_item_name, distance_cm);
    if (FyAudioConfig->writeConfig()) {        
        LOGDBG("write config sucessfull!");   
    } else {
        LOGDBG("write config failure!");    
    }

    adSpeakerLevelInfo[channel].distance_value = distance_cm;
    float value = distance_cm / 100 / 314 * 1000;
    LOGDBG("set delay_ms=%f", value);
    setSpeakerDealy(static_cast<FyAudioOutChannel>(channel),  value);
}

void FyAudioControl::setTimeAlignmentMode(int listening_position)
{
    char config_item_name[32] = {0};
    float distance_cm = 0.0f;

    for(int channel = FyAudioFrontLeft; channel <= FyAudioSubwoofer; channel++)
    {
        if(channel == FyAudioCenter)
        {
            continue;
        }
        sprintf(config_item_name, "f_channel%d_time_alignment_p%d", channel, listening_position);
        distance_cm = FyAudioConfig->getFloat(config_item_name);
        LOGDBG("%s=%f", config_item_name, distance_cm);
        if(distance_cm < 0)
        {
            continue;
        }

        float value = distance_cm / 100 / 314 * 1000;
        LOGDBG("set delay_ms=%f", value);
        setSpeakerDealy(static_cast<FyAudioOutChannel>(channel),  value);
    }    
}

//TA 
void FyAudioControl::setTimeAlignmentOn(int on)
{
    LOGDBG("setTimeAlignmentOn=%d", on);
    float distance_cm = 0.0f;
    char config_item_name[32] = {0};
    sprintf(config_item_name, "time_alignment_on", on);
    FyAudioConfig->set(config_item_name, on);
    if (FyAudioConfig->writeConfig()) {        
        LOGDBG("write config sucessfull!");   
    } else {
        LOGDBG("write config failure!");    
    }

    if(on)
    {
        int listening_position = FyAudioConfig->get("listening_position");
        for(int ch = FyAudioFrontLeft; ch <= FyAudioSubwoofer; ch++)
        {
            if(ch == FyAudioCenter)
            {
                continue;
            }

            sprintf(config_item_name, "f_channel%d_time_alignment_p%d", ch, listening_position);
            distance_cm = FyAudioConfig->getFloat(config_item_name);

            float value = distance_cm / 100 / 314 * 1000;
            LOGDBG("distance_cm=%f, set delay_ms=%f", distance_cm, value);
            setSpeakerDealy(static_cast<FyAudioOutChannel>(ch),  value);
        }
    }else{
        for(int ch = FyAudioFrontLeft; ch <= FyAudioSubwoofer; ch++)
        {
            if(ch == FyAudioCenter)
            {
                continue;
            }
            setSpeakerDealy(static_cast<FyAudioOutChannel>(ch),  0.0f);
        }
    }
}

void FyAudioControl::initEqSystemVal(audio_ext_eq_parameter_t &eq)
{                          
    for(int i = 0; i < FY_EQ_FREQUENCY_CNT; i++) {
        eq.eqValue[i].eBiquadType = AUDIO_Biquad_unchanged;
        eq.eqValue[i].eMethod = AUDIO_BqMethod_Butterworth;
        eq.eqValue[i].nFc = adEqFreqArray[i];
        eq.eqValue[i].sQ = 4;
    }
}

int FyAudioControl::doSetEq(audio_ext_eq_parameter_t &eq)
{
    int ret = -1;
    audio::SPAudioSetting::App *appSetting;
    appSetting = audio::SPAudioSetting::createApp();
    if(NULL != appSetting) {
        int sdRet = appSetting->setEqualizer(&eq);
        audio::SPAudioSetting::destroy(appSetting);
        if(sdRet != 0) {
            LOGERR("set eq error,ret=%d", sdRet);
            return ret;
        }
        ret = 0;
        LOGDBG("set eq channel:%d", eq.nChannel);
    } else {
        LOGERR("Create App failed.");
    }

    return ret;
}

/*
6. eq set
*/
void FyAudioControl::setEqValue(int eq_mode, const vector<int> &gain_array)
{
    if(eq_mode >= EqModeMax)
    {
        LOGDBG("err, eq_mode=%s", eq_mode);
        return;
    }
    /*
    if(gain_array.size() != adEqInfo[eq_mode].eq_freq_cnt) {
        LOGDBG("input eq gain array err, count=%d", gain_array.size());
        return;
    }
    */
    LOGDBG("eq mode:%d, array size:%d", eq_mode, gain_array.size()); 
        
#ifdef USE_OLD_INTERFACE
    audio_ext_eq_parameter_t eq;
    memset(&eq, 0, sizeof(audio_ext_eq_parameter_t));   
    initEqSystemVal(eq);
   
    eq.nChannel = AUDIO_CHANNEL_LINK; 

    for(int i = 0; i < adEqInfo[eq_mode].eq_freq_cnt; i++) 
    {
        if(gain_array[i] < -12.0f || gain_array[i] > 12.0f) 
        {
            LOGERR("input gain val err, index=%d,value=%d" , i, gain_array[i]);
            return;
        } 
        eq.eqValue[i].eBiquadType = AUDIO_Biquad_EQ;        

        int eq_index = adEqInfo[eq_mode].eq_freq_index[i];
        eq.eqValue[eq_index].eSlope = AUDIO_BqSlope_6;
        eq.eqValue[eq_index].ePhase = AUDIO_BqPhase_0;
        eq.eqValue[eq_index].sGain = gain_array[i];
    }
    doSetEq(eq);
#else
    uint8_t buffer[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    effect_param_t *param = (effect_param_t *)buffer;       
    audio_effect_equalizer_channel_config_param_t v;
    for(int ch = AUDIO_CHANNEL_LF; ch <= AUDIO_CHANNEL_RS; ch++)
    {
        memset(buffer, 0, sizeof(buffer));
        memset(&v, 0, sizeof(v));
        v.id = static_cast<audio_channel_idx_e>(ch);
        
        v.validConfigCount = adEqInfo[eq_mode].eq_freq_cnt;
        for(uint32_t j=0; j< v.validConfigCount; j++)
        {
            audio_effect_equalizer_channel_band_config_param_t *bandConfig = &v.config[j];
            bandConfig->bandIndex          = adEqInfo[eq_mode].eq_freq_index[j];
            bandConfig->bandParam.type     = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_EQ;
            bandConfig->bandParam.fc       = adEqFreqArray[bandConfig->bandIndex];

            if(eq_mode == EqCustom1 || eq_mode == EqCustom2)
            {
                if(gain_array.size() == FY_EQ_FREQUENCY_CNT)
                {
                    adEqInfo[eq_mode].eq_gain_array[j] = gain_array[j];
                    char config_item_name[32] = {0};
                    sprintf(config_item_name, "custom%d_index%d", eq_mode - EqCustom1 + 1, j);
                    FyAudioConfig->set(config_item_name, gain_array[j]);
                }else{
                    LOGDBG("eq mode custom failure!");
                    return;
                }
            }
            bandConfig->bandParam.gain     = adEqInfo[eq_mode].eq_gain_array[j];
            bandConfig->bandParam.q        = 2.0f;
            bandConfig->bandParam.slope    = AUDIO_EFFECT_EQUALIZER_BAND_SLOPE_6;
            bandConfig->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;
            LOGDBG("ch[%d],eq mode index:%d, fc:%d, gain:%f", ch, bandConfig->bandIndex, bandConfig->bandParam.fc, bandConfig->bandParam.gain);
        }
        
        audioTlvParameterSerilize(param, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v, sizeof(audio_effect_equalizer_channel_config_param_t));           
        
        SPAudioSetting::App* app = SPAudioSetting::createApp();
        if(app)
        {
            int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param);
            SPAudioSetting::destroy(app);
        }
    }

    FyAudioConfig->set("eq_mode", eq_mode);
    if (FyAudioConfig->writeConfig()) {        
        LOGDBG("write config sucessfull!");   
    } else {
        LOGDBG("write config failure!");    
    }
    
#endif
}

/*
7. set Subwoofer, ON/OFF
打开‌：接通电源并激活信号传输，使低音炮开始工作，增强音频系统的低频响应（通常为25Hz-150Hz）。
关闭‌：切断电源或关闭信号输入，低音炮停止发声，音频系统将不再输出超低频声音。
*/
void FyAudioControl::setSubwoofer(int on)
{
    LOGDBG("on=%d", on);
    int i = 0;
    int subwoofer_eq_index = 16;
    FyAudioConfig->set("subwoofer_on", on);
    if (FyAudioConfig->writeConfig()) {        
        LOGDBG("write config sucessfull!");
    } else {
        LOGDBG("write config failure!");       
    }
#ifdef USE_OLD_INTERFACE    
    audio_ext_eq_parameter_t eq;
    memset(&eq, 0, sizeof(audio_ext_eq_parameter_t));   
    initEqSystemVal(eq);

    eq.nChannel = AUDIO_CHANNEL_LFE;                                //subwoofer
    eq.eqValue[subwoofer_eq_index].eBiquadType = AUDIO_Biquad_bass_shelf;    
    eq.eqValue[subwoofer_eq_index].eMethod = AUDIO_BqMethod_Butterworth;
    eq.eqValue[subwoofer_eq_index].eSlope = AUDIO_BqSlope_6;
    eq.eqValue[subwoofer_eq_index].ePhase = AUDIO_BqPhase_0;         

    eq.eqValue[subwoofer_eq_index].nFc = 150;

    if(on)
    {        
        eq.eqValue[subwoofer_eq_index].sGain = 6.0;   
    
    }else{
        eq.eqValue[subwoofer_eq_index].sGain = 0.0;   
    }
    doSetEq(eq);
#else
    uint8_t buffer[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    effect_param_t *param = (effect_param_t *)buffer;

    audio_effect_equalizer_channel_config_param_t v;
    memset(&v, 0, sizeof(v));
    v.id = AUDIO_CHANNEL_LFE;
    v.validConfigCount = 1;
    audio_effect_equalizer_channel_band_config_param_t *bassConfig = &v.config[0];
    bassConfig->bandIndex          = subwoofer_eq_index;
    
    bassConfig->bandParam.fc       = adEqFreqArray[bassConfig->bandIndex];
    bassConfig->bandParam.type     = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_BASSSHELF_Q;  
    
    if(on)
    {              
        bassConfig->bandParam.gain = 6.0;
    }else{        
        bassConfig->bandParam.gain = 0.0;   
    }
    bassConfig->bandParam.q        = 0.7f;
    bassConfig->bandParam.slope    = AUDIO_EFFECT_EQUALIZER_BAND_SLOPE_6;
    bassConfig->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;
    audioTlvParameterSerilize(param, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v, sizeof(audio_effect_equalizer_channel_config_param_t));

    SPAudioSetting::App* app = SPAudioSetting::createApp();
    if(app)
    {
        int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param);
        SPAudioSetting::destroy(app);
    }

#endif
}

void FyAudioControl::setHFPCrossoverOn(int channel, int filter_on)
{
    LOGDBG("setHFPCrossoverOn channel= %d, filter_on=%d", channel, filter_on); 
    uint8_t buffer[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    effect_param_t *param = (effect_param_t *)buffer;  
    
    int freq_id = 0;
    int slope_id = 0;
    char config_item_name[32] = {0};
    int cr_channel = 0;
    if(channel == FyAudioFrontLeft || channel == FyAudioFrontRight)
    {
        cr_channel = 0;
    }else if(channel == FyAudioRearLeft || channel == FyAudioRearRight)
    {
        cr_channel = 1;
    }else{
        cr_channel = 2;
    }
    sprintf(config_item_name, "crossover%d_freq_id", cr_channel);
    freq_id = FyAudioConfig->get(config_item_name);
    memset(config_item_name, 0, 32);
    sprintf(config_item_name, "crossover%d_slope_id", cr_channel);
    slope_id = FyAudioConfig->get(config_item_name);

    if(freq_id < 0 || slope_id < 0)
        return;

    int speaker_mode = FyAudioConfig->get("speakerMode");
    LOGDBG("get speaker_mode = %d, channel=%d, filter_on=%d", speaker_mode, channel, filter_on);

    if(speaker_mode < 0)
    {
        speaker_mode = 0;
    }    
 //   speaker_mode = 1;  //for test
    audio_effect_equalizer_channel_config_param_t v;
    memset(&v, 0, sizeof(v));

    audio_channel_idx_e spChannel = getSPAudioChannel(static_cast<FyAudioOutChannel>(channel));
    v.id = spChannel;    
    v.validConfigCount = 1;               
    
    audio_effect_equalizer_channel_band_config_param_t *filterConfig = &v.config[0];
    filterConfig->bandParam.type = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_HIGHPASS_BUTTERWORTH;    
    filterConfig->bandIndex          = 17 + cr_channel;      
    if(speaker_mode == 0)
    {
        filterConfig->bandParam.fc       = adEqFreqArray[17 + freq_id];
    }else{
        filterConfig->bandParam.fc       = adEqFreqArray3[17 + freq_id];
    }
    
    filterConfig->bandParam.gain     = 0;
    if(filter_on == 1)
    {
        filterConfig->bandParam.q    = 0.1f;
        if(slope_id > 3)
            slope_id = 3;              //当前最大只支持AUDIO_EFFECT_EQUALIZER_BAND_SLOPE_24
        filterConfig->bandParam.gain     = 6;         
        filterConfig->bandParam.slope    = static_cast<audio_effect_equalizer_channel_band_slope_param_e>(slope_id);
    }else{
        filterConfig->bandParam.q    = 0.1f;
        filterConfig->bandParam.fc = 20;
        filterConfig->bandParam.slope    = static_cast<audio_effect_equalizer_channel_band_slope_param_e>(0);
    }        
    filterConfig->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;       

    audioTlvParameterSerilize(param, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v, sizeof(audio_effect_equalizer_channel_config_param_t));

    LOGDBG("setHFPCrossoverOn cr_channel=%d, channel=%d, freq_id=%d, filt_slop=%d, index=%d, fc=%d", cr_channel, channel, freq_id, filterConfig->bandParam.slope, filterConfig->bandIndex, filterConfig->bandParam.fc);        
    SPAudioSetting::App* app = SPAudioSetting::createApp();
    if(app)
    {
        int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param);
        SPAudioSetting::destroy(app);
    }
}

void FyAudioControl::set2WayHFPCrossover(int channel, int freq_id, int slope_id)
{
    uint8_t buffer[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    uint8_t buffer1[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    effect_param_t *param = (effect_param_t *)buffer;
    effect_param_t *param1 = (effect_param_t *)buffer1;

    SPAudioSetting::App* app = SPAudioSetting::createApp();

    audio_effect_equalizer_channel_config_param_t v, v1;        //复用BAND，v1用于取消原BAND设置
    memset(&v, 0, sizeof(v));
    memset(&v1, 0, sizeof(v1));

    audio_channel_idx_e spChannel = getSPAudioChannel(static_cast<FyAudioOutChannel>(channel));
    v.id = spChannel;
    v1.id = spChannel;

    char config_item_name[32] = {0};
    int cr_channel = 0;
    if(channel == FyAudioFrontLeft || channel == FyAudioFrontRight)
    {
        cr_channel = 0;
    }else if(channel == FyAudioRearLeft || channel == FyAudioRearRight)
    {
        cr_channel = 1;
    }else{
        cr_channel = 2;
    }
    sprintf(config_item_name, "crossover%d_freq_id", cr_channel);
    int old_freq_id = FyAudioConfig->get(config_item_name);

    //v1 cancel old config
    if(old_freq_id >= 0)
    {
        v1.validConfigCount = 1;        
        audio_effect_equalizer_channel_band_config_param_t *filterConfig1 = &v1.config[0];
        filterConfig1->bandParam.type = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_HIGHPASS_BUTTERWORTH;    
        filterConfig1->bandIndex          = 17 + cr_channel;      
        filterConfig1->bandParam.fc       = adEqFreqArray[17 + old_freq_id];   
        filterConfig1->bandParam.q    = 0.0f;   
        filterConfig1->bandParam.gain     = 0;         
        filterConfig1->bandParam.slope    = static_cast<audio_effect_equalizer_channel_band_slope_param_e>(0);                  
        filterConfig1->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;

        LOGDBG("set2WayHFPCrossover cr_channel=%d, channel=%d, old_freq_id=%d, index=%d, fc=%.2f", cr_channel, channel, old_freq_id, filterConfig1->bandIndex, filterConfig1->bandParam.fc);        

        audioTlvParameterSerilize(param1, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v1, sizeof(audio_effect_equalizer_channel_config_param_t));
        if(app)
        {
            int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param1);        
        }
    }else{
        LOGDBG("set2WayHFPCrossover old_freq_id=%d", old_freq_id);
    }    
    
    //v new config
    v.validConfigCount = 1;        
    audio_effect_equalizer_channel_band_config_param_t *filterConfig = &v.config[0];
    filterConfig->bandParam.type = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_HIGHPASS_BUTTERWORTH;    
    filterConfig->bandIndex          = 17 + cr_channel;      
    filterConfig->bandParam.fc       = adEqFreqArray[17 + freq_id];
   
    filterConfig->bandParam.q    = 0.1f;
    if(slope_id > 3)
        slope_id = 3;              //当前最大只支持AUDIO_EFFECT_EQUALIZER_BAND_SLOPE_24
    filterConfig->bandParam.gain     = 6;         
    filterConfig->bandParam.slope    = static_cast<audio_effect_equalizer_channel_band_slope_param_e>(slope_id);   
                  
    filterConfig->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;

    LOGDBG("set3WayHFPCrossover new_freq_id=%d, index=%d, fc=%.2f", freq_id, filterConfig->bandIndex, filterConfig->bandParam.fc);         

    audioTlvParameterSerilize(param, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v, sizeof(audio_effect_equalizer_channel_config_param_t));
    
    if(app)
    {
        int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param);
        SPAudioSetting::destroy(app);
    }
}

void FyAudioControl::set3WayHFPCrossover(int channel, int freq_id, int slope_id)
{
    uint8_t buffer[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    uint8_t buffer1[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    effect_param_t *param = (effect_param_t *)buffer;
    effect_param_t *param1 = (effect_param_t *)buffer1;

    SPAudioSetting::App* app = SPAudioSetting::createApp();

    audio_effect_equalizer_channel_config_param_t v, v1;        //复用BAND，v1用于取消原BAND设置
    memset(&v, 0, sizeof(v));
    memset(&v1, 0, sizeof(v1));

    audio_channel_idx_e spChannel = getSPAudioChannel(static_cast<FyAudioOutChannel>(channel));
    v.id = spChannel;
    v1.id = spChannel;

    char config_item_name[32] = {0};
    int cr_channel = 0;
    if(channel == FyAudioFrontLeft || channel == FyAudioFrontRight)
    {
        cr_channel = 0;
    }else if(channel == FyAudioRearLeft || channel == FyAudioRearRight)
    {
        cr_channel = 1;
    }else{
        cr_channel = 2;
    }
    sprintf(config_item_name, "crossover%d_freq_id", cr_channel);
    int old_freq_id = FyAudioConfig->get(config_item_name);

    //v1 cancel old config
    if(old_freq_id >= 0)
    {
        v1.validConfigCount = 1;        
        audio_effect_equalizer_channel_band_config_param_t *filterConfig1 = &v1.config[0];
        filterConfig1->bandParam.type = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_HIGHPASS_BUTTERWORTH;    
        filterConfig1->bandIndex          = 17 + cr_channel;      
        filterConfig1->bandParam.fc       = adEqFreqArray3[17 + old_freq_id];   
        filterConfig1->bandParam.q    = 0.0f;   
        filterConfig1->bandParam.gain     = 0;         
        filterConfig1->bandParam.slope    = static_cast<audio_effect_equalizer_channel_band_slope_param_e>(0);                  
        filterConfig1->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;

        LOGDBG("1 set3WayHFPCrossover cr_channel=%d, channel=%d, old_freq_id=%d, index=%d, fc=%d", cr_channel, channel, old_freq_id, filterConfig1->bandIndex, filterConfig1->bandParam.fc);        
        LOGDBG("set3WayHFPCrossover fc=%d", adEqFreqArray3[17 + old_freq_id]);
        audioTlvParameterSerilize(param1, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v1, sizeof(audio_effect_equalizer_channel_config_param_t));
        if(app)
        {
            int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param1);        
        }
    }else{
        LOGDBG("set3WayHFPCrossover old_freq_id=%d", old_freq_id);
    }    
    
    //v new config
    v.validConfigCount = 1;        
    audio_effect_equalizer_channel_band_config_param_t *filterConfig = &v.config[0];
    filterConfig->bandParam.type = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_HIGHPASS_BUTTERWORTH;    
    filterConfig->bandIndex          = 17 + cr_channel;      
    filterConfig->bandParam.fc       = adEqFreqArray3[17 + freq_id];
   
    filterConfig->bandParam.q    = 0.1f;
    if(slope_id > 3)
        slope_id = 3;              //当前最大只支持AUDIO_EFFECT_EQUALIZER_BAND_SLOPE_24
    filterConfig->bandParam.gain     = 6;         
    filterConfig->bandParam.slope    = static_cast<audio_effect_equalizer_channel_band_slope_param_e>(slope_id);   
                  
    filterConfig->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;

    LOGDBG("2 set3WayHFPCrossover new_freq_id=%d, index=%d, fc=%d", freq_id, filterConfig->bandIndex, filterConfig->bandParam.fc);         
    LOGDBG("set3WayHFPCrossover fc=%d", adEqFreqArray3[17 + freq_id]);

    audioTlvParameterSerilize(param, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v, sizeof(audio_effect_equalizer_channel_config_param_t));
    
    if(app)
    {
        int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param);
        SPAudioSetting::destroy(app);
    }
}


void FyAudioControl::setLFPCrossoverOn(int channel, int filter_on)
{
    LOGDBG("setLFPCrossoverOn channel= %d, filter_on=%d", channel, filter_on); 
    uint8_t buffer[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    effect_param_t *param = (effect_param_t *)buffer;  
    
    int freq_id = 0;
    int slope_id = 0;
    char config_item_name[32] = {0};
    int cr_channel = 0;
    if(channel == FyAudioFrontLeft || channel == FyAudioFrontRight)
    {
        cr_channel = 0;
    }else if(channel == FyAudioRearLeft || channel == FyAudioRearRight)
    {
        cr_channel = 1;
    }else{
        cr_channel = 2;
    }
    sprintf(config_item_name, "crossover%d_freq_id", cr_channel);
    freq_id = FyAudioConfig->get(config_item_name);
    memset(config_item_name, 0, 32);
    sprintf(config_item_name, "crossover%d_slope_id", cr_channel);
    slope_id = FyAudioConfig->get(config_item_name);

    if(freq_id < 0 || slope_id < 0)
        return;

    int speaker_mode = FyAudioConfig->get("speakerMode");
    LOGDBG("get speaker_mode = %d, channel=%d, filter_on=%d", speaker_mode, channel, filter_on);

    if(speaker_mode < 0)
    {
        speaker_mode = 0;
    } 
    audio_effect_equalizer_channel_config_param_t v;
    memset(&v, 0, sizeof(v));

    audio_channel_idx_e spChannel = getSPAudioChannel(static_cast<FyAudioOutChannel>(channel));
    v.id = spChannel;    
    v.validConfigCount = 1;               
    
    audio_effect_equalizer_channel_band_config_param_t *filterConfig = &v.config[0];
    filterConfig->bandParam.type = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_LOWPASS_BUTTERWORTH;    
    filterConfig->bandIndex          = 17 + cr_channel;      
    if(speaker_mode == 0)
    {
        filterConfig->bandParam.fc       = adEqFreqArray[28 + freq_id];
    }else{
        filterConfig->bandParam.fc       = adEqFreqArray3[28 + freq_id];
    }
    
    filterConfig->bandParam.gain     = 0;
    if(filter_on == 1)
    {
        filterConfig->bandParam.q    = 0.1f;
        if(slope_id > 3)
            slope_id = 3;              //当前最大只支持AUDIO_EFFECT_EQUALIZER_BAND_SLOPE_24
        filterConfig->bandParam.gain     = 6;         
        filterConfig->bandParam.slope    = static_cast<audio_effect_equalizer_channel_band_slope_param_e>(slope_id);
    }else{
        filterConfig->bandParam.q    = 0.1f;
        filterConfig->bandParam.fc = 20;
        filterConfig->bandParam.slope    = static_cast<audio_effect_equalizer_channel_band_slope_param_e>(0);
    }        
    filterConfig->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;       

    audioTlvParameterSerilize(param, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v, sizeof(audio_effect_equalizer_channel_config_param_t));

    LOGDBG("setLFPCrossoverOn cr_channel=%d, channel=%d, freq_id=%d, filt_slop=%d, index=%d, fc=%.2f", cr_channel, channel, freq_id, filterConfig->bandParam.slope, filterConfig->bandIndex, filterConfig->bandParam.fc);        
    SPAudioSetting::App* app = SPAudioSetting::createApp();
    if(app)
    {
        int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param);
        SPAudioSetting::destroy(app);
    }   
}

void FyAudioControl::set2WayLFPCrossover(int channel, int freq_id, int slope_id)
{
    uint8_t buffer[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    uint8_t buffer1[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    effect_param_t *param = (effect_param_t *)buffer;
    effect_param_t *param1 = (effect_param_t *)buffer1;

    SPAudioSetting::App* app = SPAudioSetting::createApp();

    audio_effect_equalizer_channel_config_param_t v, v1;        //复用BAND，v1用于取消原BAND设置
    memset(&v, 0, sizeof(v));
    memset(&v1, 0, sizeof(v1));

    audio_channel_idx_e spChannel = getSPAudioChannel(static_cast<FyAudioOutChannel>(channel));
    v.id = spChannel;
    v1.id = spChannel;

    char config_item_name[32] = {0};
    int cr_channel = 0;
    if(channel == FyAudioFrontLeft || channel == FyAudioFrontRight)
    {
        cr_channel = 0;
    }else if(channel == FyAudioRearLeft || channel == FyAudioRearRight)
    {
        cr_channel = 1;
    }else{
        cr_channel = 2;
    }
    sprintf(config_item_name, "crossover%d_freq_id", cr_channel);
    int old_freq_id = FyAudioConfig->get(config_item_name);

    //v1 cancel old config
    if(old_freq_id >= 0)
    {
        v1.validConfigCount = 1;        
        audio_effect_equalizer_channel_band_config_param_t *filterConfig1 = &v1.config[0];
        filterConfig1->bandParam.type = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_LOWPASS_BUTTERWORTH;    
        filterConfig1->bandIndex          = 17 + cr_channel;      
        filterConfig1->bandParam.fc       = adEqFreqArray[28 + old_freq_id];   
        filterConfig1->bandParam.q    = 0.0f;   
        filterConfig1->bandParam.gain     = 0;         
        filterConfig1->bandParam.slope    = static_cast<audio_effect_equalizer_channel_band_slope_param_e>(0);                  
        filterConfig1->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;

        LOGDBG("set2WayLFPCrossover cr_channel=%d, channel=%d, old_freq_id=%d, index=%d, fc=%.2f", cr_channel, channel, old_freq_id, filterConfig1->bandIndex, filterConfig1->bandParam.fc);        

        audioTlvParameterSerilize(param1, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v1, sizeof(audio_effect_equalizer_channel_config_param_t));
        if(app)
        {
            int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param1);        
        }
    }else{
        LOGDBG("set2WayLFPCrossover old_freq_id=%d", old_freq_id);
    }    
    
    //v new config
    v.validConfigCount = 1;        
    audio_effect_equalizer_channel_band_config_param_t *filterConfig = &v.config[0];
    filterConfig->bandParam.type = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_HIGHPASS_BUTTERWORTH;    
    filterConfig->bandIndex          = 17 + cr_channel;      
    filterConfig->bandParam.fc       = adEqFreqArray[28 + freq_id];
   
    filterConfig->bandParam.q    = 0.1f;
    if(slope_id > 3)
        slope_id = 3;              //当前最大只支持AUDIO_EFFECT_EQUALIZER_BAND_SLOPE_24
    filterConfig->bandParam.gain     = 6;         
    filterConfig->bandParam.slope    = static_cast<audio_effect_equalizer_channel_band_slope_param_e>(slope_id);   
                  
    filterConfig->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;

    LOGDBG("set2WayLFPCrossover new_freq_id=%d, index=%d, fc=%.2f", freq_id, filterConfig->bandIndex, filterConfig->bandParam.fc);         

    audioTlvParameterSerilize(param, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v, sizeof(audio_effect_equalizer_channel_config_param_t));
    
    if(app)
    {
        int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param);
        SPAudioSetting::destroy(app);
    }      
}

void FyAudioControl::set3WayLFPCrossover(int channel, int freq_id, int slope_id)
{
    uint8_t buffer[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    uint8_t buffer1[AUDIO_EFFECT_EQUALIZER_CHANNEL_CONFIG_PARAM_SIZE_MAX] = {0};
    effect_param_t *param = (effect_param_t *)buffer;
    effect_param_t *param1 = (effect_param_t *)buffer1;

    SPAudioSetting::App* app = SPAudioSetting::createApp();

    audio_effect_equalizer_channel_config_param_t v, v1;        //复用BAND，v1用于取消原BAND设置
    memset(&v, 0, sizeof(v));
    memset(&v1, 0, sizeof(v1));

    audio_channel_idx_e spChannel = getSPAudioChannel(static_cast<FyAudioOutChannel>(channel));
    v.id = spChannel;
    v1.id = spChannel;

    char config_item_name[32] = {0};
    int cr_channel = 0;
    if(channel == FyAudioFrontLeft || channel == FyAudioFrontRight)
    {
        cr_channel = 0;
    }else if(channel == FyAudioRearLeft || channel == FyAudioRearRight)
    {
        cr_channel = 1;
    }else{
        cr_channel = 2;
    }
    sprintf(config_item_name, "crossover%d_freq_id", cr_channel);
    int old_freq_id = FyAudioConfig->get(config_item_name);

    //v1 cancel old config
    if(old_freq_id >= 0)
    {
        v1.validConfigCount = 1;        
        audio_effect_equalizer_channel_band_config_param_t *filterConfig1 = &v1.config[0];
        filterConfig1->bandParam.type = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_LOWPASS_BUTTERWORTH;    
        filterConfig1->bandIndex          = 17 + cr_channel;      
        filterConfig1->bandParam.fc       = adEqFreqArray3[28 + old_freq_id];   
        filterConfig1->bandParam.q    = 0.0f;   
        filterConfig1->bandParam.gain     = 0;         
        filterConfig1->bandParam.slope    = static_cast<audio_effect_equalizer_channel_band_slope_param_e>(0);                  
        filterConfig1->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;

        LOGDBG("set3WayLFPCrossover cr_channel=%d, channel=%d, old_freq_id=%d, index=%d, fc=%.2f", cr_channel, channel, old_freq_id, filterConfig1->bandIndex, filterConfig1->bandParam.fc);        

        audioTlvParameterSerilize(param1, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v1, sizeof(audio_effect_equalizer_channel_config_param_t));
        if(app)
        {
            int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param1);        
        }
    }else{
        LOGDBG("set3WayLFPCrossover old_freq_id=%d", old_freq_id);
    }    
    
    //v new config
    v.validConfigCount = 1;        
    audio_effect_equalizer_channel_band_config_param_t *filterConfig = &v.config[0];
    filterConfig->bandParam.type = AUDIO_EFFECT_EQUALIZER_BAND_TYPE_LOWPASS_BUTTERWORTH;    
    filterConfig->bandIndex          = 17 + cr_channel;      
    filterConfig->bandParam.fc       = adEqFreqArray3[28 + freq_id];
   
    filterConfig->bandParam.q    = 0.1f;
    if(slope_id > 3)
        slope_id = 3;              //当前最大只支持AUDIO_EFFECT_EQUALIZER_BAND_SLOPE_24
    filterConfig->bandParam.gain     = 6;         
    filterConfig->bandParam.slope    = static_cast<audio_effect_equalizer_channel_band_slope_param_e>(slope_id);   
                  
    filterConfig->bandParam.phase    = AUDIO_EFFECT_EQUALIZER_BAND_PHASE_0;

    LOGDBG("set3WayLFPCrossover new_freq_id=%d, index=%d, fc=%.2f", freq_id, filterConfig->bandIndex, filterConfig->bandParam.fc);         

    audioTlvParameterSerilize(param, AUDIO_EFFECT_EQUALIZER_PARAM_TYPE_BAND_CHANNEL_CONFIG, &v, sizeof(audio_effect_equalizer_channel_config_param_t));
    
    if(app)
    {
        int ret = app->setEffectParameter(APP_NAME_FRONT, AUDIO_EFFECT_NAME_EQUALIZER, *param);
        SPAudioSetting::destroy(app);
    }    
}
/*
8. set Crossover, channel: FRONT/REAR/Subwoofer, LPF 80 Hz Slope -18 db/oct, HPF 250 Hz Slope -12 db/oct
    0:FRONT, 1: REAR, 2:SUBwoofer
*/
void FyAudioControl::setCrossover(int channel, int freq_id, int slope_id)
{ 
    char config_item_name[32] = {0};
    sprintf(config_item_name, "crossover%d", channel);
    int filter_on = FyAudioConfig->get(config_item_name);

    if(filter_on == 0)
    {
        LOGDBG("set2HFPCrossover err");
        return;
    }
    
    int speaker_mode = FyAudioConfig->get("speakerMode");
    LOGDBG("get speaker_mode = %d, freq_id=%d, slope_id=%d", speaker_mode, freq_id, slope_id);

    if(speaker_mode < 0)
    {
        speaker_mode = 0;
    }

//    speaker_mode = 1;       //for test
     //0:2way standard mode, 1: 3way network mode
    if(speaker_mode == 0)
    {
        switch (channel)
        {
            case 0:
                set2WayHFPCrossover(FyAudioFrontLeft, freq_id, slope_id);
                set2WayHFPCrossover(FyAudioFrontRight, freq_id, slope_id);               
                break;
            case 1:
                set2WayHFPCrossover(FyAudioRearLeft, freq_id, slope_id);
                set2WayHFPCrossover(FyAudioRearRight, freq_id, slope_id);              
                break;
            case 2:
                set2WayLFPCrossover(FyAudioSubwoofer, freq_id, slope_id);
                break;  
            default:
                break;
        }

    }else{

        switch (channel)
        {
            case 0:
                set3WayHFPCrossover(FyAudioFrontLeft, freq_id, slope_id);
                set3WayHFPCrossover(FyAudioFrontRight, freq_id, slope_id);               
                break;
            case 1:
                set3WayHFPCrossover(FyAudioRearLeft, freq_id, slope_id);
                set3WayHFPCrossover(FyAudioRearRight, freq_id, slope_id);              
                break;
            case 2:
                set3WayLFPCrossover(FyAudioSubwoofer, freq_id, slope_id);
                break;  
            default:
                break;
        }

    }
   
    memset(config_item_name, 0, 32);
    sprintf(config_item_name, "crossover%d_freq_id", channel);
    FyAudioConfig->set(config_item_name, freq_id);
    memset(config_item_name, 0, 32);
    sprintf(config_item_name, "crossover%d_slope_id", channel);
    FyAudioConfig->set(config_item_name, slope_id);
    memset(config_item_name, 0, 32);
    strcpy(config_item_name, "crossover_channel");
    FyAudioConfig->set(config_item_name, channel);

    if (FyAudioConfig->writeConfig()) {
        LOGDBG("write config sucessfull!");   
    } else {
        LOGDBG("write config failure!");    
    }
}

void FyAudioControl::setCrossoverOn(int channel, int filter_on)
{
    //0:2way standard mode, 1: 3way network mode
    char config_item_name[32] = {0};
    sprintf(config_item_name, "crossover%d", channel);
    FyAudioConfig->set(config_item_name, filter_on);
    if (FyAudioConfig->writeConfig()) {        
        LOGDBG("write config sucessfull!");  
    } else {
        LOGDBG("write config failure!");    
    }    

    switch (channel)
    {
        case 0:
            setHFPCrossoverOn(FyAudioFrontLeft, filter_on);
            setHFPCrossoverOn(FyAudioFrontRight, filter_on);               
            break;
        case 1:
            setHFPCrossoverOn(FyAudioRearLeft, filter_on);
            setHFPCrossoverOn(FyAudioRearRight, filter_on);              
            break;
        case 2:
            setLFPCrossoverOn(FyAudioSubwoofer, filter_on);
            break;  
        default:
            break;    
    }
}





