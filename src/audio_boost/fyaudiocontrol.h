#ifndef FY_AUDIOCONTROL_H
#define FY_AUDIOCONTROL_H

#include <vector>
#include "SPAudioSetting.h"
#include "SPAudioEffectEqualizer.h"
#include "ConfigHelper.h"
using namespace std;
using namespace audio;

#define FY_EQ_FREQUENCY_CNT         (13)    //当前最多支持13个
#define FY_FADE_BALANCE_STEP_CNT    (15)

#define FY_VOLUME_MAX_VAL           (100)
#define FY_MICRO_PHONE_VOLUME_MAX   (100)

enum FyEqMode {
    EqSuperBass,
    EqPowerful,
    EqNatural,
    EqVocal,
    EqFlat,
    EqCustom1,
    EqCustom2,
    EqModeMax
};

struct AudioEqInfoStruct
{
    FyEqMode eq_mode;
    int eq_freq_cnt;
    int eq_freq_index[FY_EQ_FREQUENCY_CNT];
    int eq_gain_array[FY_EQ_FREQUENCY_CNT];
};

enum AudioLoudnessMode 
{
    FyAudioLoudnessOff = 0,
    FyAudioLoudnessLow,
    FyAudioLoudnessMid,
    FyAudioLoudnessHigh,
};

struct AudioLoudnessInfoStruct 
{
    enum AudioLoudnessMode Loudness_mode;
    unsigned char FreqIndex;        //频率索引
    float gain;    
};

enum FyAudioOutChannel 
{
    FyAudioFrontLeft = 0,
    FyAudioFrontRight,
    FyAudioRearLeft,
    FyAudioRearRight,
    FyAudioCenter,
    FyAudioSubwoofer,
    FyAudioChannelMax
};

struct AudioSpeakerLevelInfoStruct
{
    enum FyAudioOutChannel out_channel;
    unsigned int gain_value;               //输出DB值
    float distance_value;           //时间对齐，距离值cm
};
   
class FyAudioControl
{   
public:
    enum AudioListeningPosition
    {
        ListeningPositionOff = 0,
        ListeningPositionFront,
        ListeningPositionFrontLeft,
        ListeningPositionFrontRight,
        ListeningPositionAll,
        ListeningPositionUnknow,
    };    
   
    enum AudioFilterMethod 
    {
        AudioLinkwitz = 0,
        AudioButterworth,
        AudioMethodMax
    };

    enum AudioFilterType 
    {
        AudioLowPass = 0,
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


    static FyAudioControl *getInstance();
    static void FyAudioControlInit();
    void setDpsEnable(bool en);
    /*
     1. set fade/balance,fade:-15~15 balance:-15~15 (FADE_BALANCE_STEP_CNT:15)
    */
    void setFadeBalance(int fade, int balance);
    int getGainVal(int x, int y, int gainX, int gainY);

    /*
     2. set loudnes, OFF/LOW/MID/HIGH
    */
    void setLoudnes(int loudnes_mode);
    void setLoudnessBassShelf(int loudness_mode);
    
    /*
     3. set speaker level, channel：fl, fr, rl,rr, DB Range：-24-10，Step size is 1
    */
    void setSpeakerLevel(int listening_position, int channel, unsigned gain);
    void setSpeakerLevelMode(int listening_position);

    /*
     4. set listening position, FRONT/FRONT-L/FRONT-R/ALL/OFF
    */
    void setListeningPosition(unsigned char listening_position);
    audio_channel_idx_e getSPAudioChannel(FyAudioOutChannel channel);

    /*
     5. set time alignment, position:FRONT/FRONT-L/FRONT-R/SUBWOOFER/OFF, alignment:eg 100cm
    */
    void setTimeAlignment(int listening_position, int channel, float distance_cm);
    void setTimeAlignmentMode(int listening_position);
    void setTimeAlignmentOn(int on);
    /*
     * speaker delay
     * data list:
     *     please following AudioOutChannel order:
     *     AudioFrontLeft,AudioFrontRight,AudioRearLeft,AudioRearRight,AudioCenter, AudioSubwoofer.
     * value:0.0~20.0ms
     *
    */
    void setSpeakerDealy(FyAudioOutChannel channel, float value);    

    /*
     6. eq set
    */
    void setEqValue(int eq_mode, const vector<int> &eq_value);
    void initEqSystemVal(audio_ext_eq_parameter_t &eq);
    int doSetEq(audio_ext_eq_parameter_t &eq);    

    /*
     7. set Subwoofer, ON/OFF
    */
    void setSubwoofer(int on);

    /*
     8. set Crossover, channel: FRONT/REAR/Subwoofer, LPF 80 Hz Slope -18 db/oct, HPF 250 Hz Slope -12 db/oct
    */
    void setCrossover(int channel, int freq_id, int slope_id);

    void setCrossoverOn(int channel, int filter_on);    
    void setHFPCrossoverOn(int channel, int filter_on);
    void setLFPCrossoverOn(int channel, int filter_on);

    void set2WayHFPCrossover(int channel, int freq_id, int slope_id);
    void set3WayHFPCrossover(int channel, int freq_id, int slope_id);
    
    void set2WayLFPCrossover(int channel, int freq_id, int slope_id);
    void set3WayLFPCrossover(int channel, int freq_id, int slope_id);
    
protected:
    FyAudioControl();
    ~FyAudioControl();
     
public:
    static FyAudioControl *adCtrl;    
    static ConfigHelper *FyAudioConfig;

/*audio boost 配置数据
    fade=	0-15		
    balance=	0-15		
    loudness=	mode:0-3		
    channel%d_speaker_level=		各喇叭音量大小	
    		
    listening_position=			
    channel%d_time_alignment=			0-490cm	时间对齐	
    			
    eq_mode=		EQ	
    custom1_index[n]=		n:0-12	
    custom2_index[n]=			
    subwoofer_on=		低音炮	
    crossover1=	front(2way standar)/high(3way network)		
    crossover2=	real(2way standar)/mid(3way network)		
    crossover3=	subwoofer		   
*/    
   
    unsigned char speakerMode = 0;          //0:2way standard mode, 1: 3way network mode
    
};
#endif


