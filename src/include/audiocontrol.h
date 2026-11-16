#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H
#include "audiodef.h"
#include "SPAudioTypes.h"
#include "SPAudioSetting.h"
#include "DataType.h"
#include "log.h"
#include <string>
#include <map>

using namespace std;
using std::string;
using namespace audio;
USING_FGE_NAMESPACE

class AudioControl
{
public:
    enum EqMode 
    {
        EqClassic,
        EqRock,
        EqJazz,
        EqElectric,
        EqPop,
        EqSoft,
        EqStandard,
        EqModeMax
    };

    enum SubMode 
    {
        Subleft,
        Subright,
        SubLRModeMax
    };

    enum AudioInChannel 
    {
        AudioInChannel0,
        AudioInChannel1,
        AudioInChannel2,
        AudioInChannel3,
        AudioInChannel4,
        AudioInChannel5
    };

    enum AudioOutChannel 
    {
        AudioFrontLeft,
        AudioFrontRight,
        AudioRearLeft,
        AudioRearRight,
        AudioCenter,
        AudioSubwoofer,
        AudioChannelMax
    };

    enum AudioSeatState 
    {
        AudioSeatStateInit,       /*!< init state of audio seat */
        AudioSeatStateStop,       /*!< stop state of audio seat */
        AudioSeatStatePlay       /*!< play state of audio seat */
    };

    enum AudioQuality
    {
        AudioQualityNormal,
        AudioQualityMiddle,
        AudioQualityBest
    };

    enum AudioTag 
    {
        AudioTagMain,
        AudioTagSecond,
        AudioTagAlter,
        AudioTagUplink,
        AudioTagInput0,
        AudioTagInput1,
        AudioTagInput2,
        AudioTagIMMain,
        AudioTagMax
    };


    enum AudioFilterMethod 
    {
        AudioLinkwitz,
        AudioButterworth,
        AudioMethodMax
    };

    enum AudioFilterType 
    {
        AudioLowPass,
        AudioHighPass,
        AudioThrough
    };

    struct AudioFilter_t 
    {
        short level;               /*!< 0 ~ (-15) dB */
        AudioFilterMethod method;
        AudioFilterType type;
        unsigned short slope;      /*!< 6, 12, 18, 24 (dB/oct) */
        unsigned short phase;       /*!< 0 or 180 degree */
        unsigned short frequency;  /*!< 20hz~640hz */
    };


    static AudioControl *getInstance();
    /**
     * @fn int setMainAudioDunkingMode(unsigned int mode)
     * @brief dunk/undunk
     * @param mode 0:undunk 1:dunk
     * @return 0 if success, else error number.
     */
    void setMainAudioDunkingMode(unsigned int mode);

    /**
     * @fn int setMainAudioDunkingTargetGain(unsigned char volume)
     * @brief gain of dunk
     * @param volume  range is [0, 100]
     * @return 0 if success, else error number.
     */
    void setMainAudioDunkingTargetGain(unsigned char volume);

    int initEqValue(audio_eq_parameter_t &eq);

    int initEqSystemVal16(audio_ext_eq_parameter_t &eq);

    int initEqSystemVal(audio_eq_parameter_t &eq);

    int setEqGain(audio_eq_parameter_t &eq);

    int setEqGain16(audio_ext_eq_parameter_t &eq);

    void setEqMode(AudioControl::EqMode mode);

    int getGainVal(int x, int y, int gainX, int gainY);

    /*
     * set fade/balance,fade:-15~15 balance:-15~15 (FADE_BALANCE_STEP_CNT:15)
    */
    void setFadeBalance(int fade, int balance);

    /*
     * set volume,the volume must <= VOLUME_MAX_VAL(100)
    */
    void setStreamVolume(const char *tagSp, const char *src, BYTE volume);
    BYTE getStreamVolume(const char *tagSp, const char *src);

    int setMasterVolume(BYTE volume);
    BYTE getMasterVolume();

    int setAlterVolume(BYTE volume);
    BYTE getAlterVolume();

    int changeCurTrackVolume(const char* tagSp, bool isAdd, string &srcName, int Addvol,int VolMax);

    /*
     * get actived source number of alternative
    */
    int getActivedAlterNumber();
 
    /*
     * set audio source
    */
    void setOutputSource(const char *tag, const char *src);

    void setInputDestination(const char *tag, const char *dest);

    void getInputgain(audio_input_mux_id_e channel, float *left_gain_value, float *right_gain_value);

    void setInputgain(audio_input_mux_id_e channel, float left_gain_value, float right_gain_value);

    void setOutputFrontSource(const char *tag, const char *src, bool is_set);

    /*
    * set output source volume
    */
    void initTrackVolume(const char* tagSp, map<string,int> &mapvols);

    audio_input_select_pin_e getSPAudioADCSelectPin(AudioControl::AudioInChannel channel);

    /*
     *  set auxin channel 0~4(notice:4 was used for mic in sunplus cfg)
    */
    void setAuxinChannel(AudioControl::AudioInChannel channel);
    AudioControl::AudioInChannel getAuxinChannel();

    AudioControl::AudioInChannel getAudioInChannel(audio_input_select_pin_e pin);

    /*
     *  set i2sin
    */
    void setI2SINChannel();


    /*
     * mute system volume
    */
    void muteAudio(const char *tag, bool enable);
    void setMute(bool mute);

    /*
     * set micro phone volume
    */
    void setMicroPhoneVolume(int volume);

    /*
     * set back earphone
    */
    void muteBackEarphone(bool mute);


    /*
     * set audio filter
    */
    audio_channel_idx_e getSPAudioChannel(AudioControl::AudioOutChannel channel);
    void setFilter(AudioOutChannel channel, const AudioFilter_t &value);

    /*
     * speaker delay
     * data list:
     *     please following AudioOutChannel order:
     *     AudioFrontLeft,AudioFrontRight,AudioRearLeft,AudioRearRight,AudioCenter, AudioSubwoofer.
     * value:0.0~20.0ms
     *
    */
    void setSpeakerDealy(AudioOutChannel channel, float value);

    /*
     * microphone test
    */
    void setMicLeftRightSwitch(bool on);

    /*
    * db +-
    */
    void setSourceLevel(const char *tagSp, const char *src, BYTE volume);
    
protected:
    AudioControl();
    ~AudioControl();
private:
    static AudioControl *adCtrl;
};
#endif


