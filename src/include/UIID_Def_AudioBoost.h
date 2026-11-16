///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* @file UIID_Def.h
*
* @brief 界面ID定义，与GUI资源ID对应
*
* @author Y4498
*
* @version 1.0.0
*
* @date 2021-10-16
*
* 修改记录：
*
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef INCLUDE_UIID_DEF_AUDIOBOOST_H_
#define INCLUDE_UIID_DEF_AUDIOBOOST_H_

#include "UIID_Def_Public.h"

/**设置项列表*/
const int UIID_AUDIO_BOOST_LIST = 1300;     //列表

/**设置项*/
const int UIID_AUDIOBOOST_LIST_FADER    = 1212;
const int UIID_AUDIOBOOST_LIST_LOUDNES  = 1214;
const int UIID_AUDIOBOOST_LIST_SUBFOOER = 1214;


/**顶栏显示*/
const int UIID_AUDIO_BOOST_RETURN = 1003;   //返回
const int UIID_AUDIO_BOOST_CLOSE = 1004;   //关闭


/**分组*/
const int UIID_GROUP_MAINLIST = 1201; //设置项列表分组
const int UIID_GROUP_FADER = 1301;   
const int UIID_GROUP_LOUDNES = 1401; 
const int UIID_GROUP_SPEAKERLEVEL = 1501;
const int UIID_GROUP_LISTENING = 1601;
const int UIID_GROUP_TIME = 1701;
const int UIID_GROUP_EQ = 1801;
const int UIID_GROUP_SUBFOOER = 1901;
const int UIID_GROUP_CROSSVOER = 2001;
const int UIID_GROUP_SOUND = 400;

const int UIID_GROUP_FADER_FRONT = 1303;
const int UIID_GROUP_FADER_REAR  = 1304;
const int UIID_GROUP_FADER_LEFT  = 1005;
const int UIID_GROUP_FADER_RIGHT = 1006;
const int UIID_GROUP_FADER_CURSOR = 1007;
const int UIID_GROUP_FADER_FRONT_VALUE = 1010;
const int UIID_GROUP_FADER_REAR_VALUE  = 1012;
const int UIID_GROUP_FADER_LEFT_VALUE  = 1014;
const int UIID_GROUP_FADER_RIGHT_VALUE = 1016;

/**LOUDNES控件*/
const int UIID_GROUP_LOUDNES_OFF = 1411; 
const int UIID_GROUP_LOUDNES_LOW = 1412; 
const int UIID_GROUP_LOUDNES_MID = 1413; 
const int UIID_GROUP_LOUDNES_HIGH = 1414; 

const int UIID_GROUP_SPEAKER_TYPE   = 1504;
const int UIID_GROUP_SPEAKER_TYPE_L = 1505;
const int UIID_GROUP_SPEAKER_TYPE_R = 1506;
const int UIID_GROUP_SPEAKER_FL_L   = 1508;
const int UIID_GROUP_SPEAKER_FL_R   = 1509;
const int UIID_GROUP_SPEAKER_FL_V   = 1510;
const int UIID_GROUP_SPEAKER_FR_L   = 1513;
const int UIID_GROUP_SPEAKER_FR_R   = 1514;
const int UIID_GROUP_SPEAKER_FR_V   = 1515;
const int UIID_GROUP_SPEAKER_RL_L   = 1518;
const int UIID_GROUP_SPEAKER_RL_R   = 1519;
const int UIID_GROUP_SPEAKER_RL_V   = 1520;
const int UIID_GROUP_SPEAKER_RR_L   = 1523;
const int UIID_GROUP_SPEAKER_RR_R   = 1524;
const int UIID_GROUP_SPEAKER_RR_V   = 1525;
const int UIID_GROUP_SPEAKER_SUB_L  = 1528;
const int UIID_GROUP_SPEAKER_SUB_R  = 1529;
const int UIID_GROUP_SPEAKER_SUB_V  = 1530;
const int UIID_GROUP_SPEAKER_BG     = 1532;


const int UIID_GROUP_LISTENING_BG       = 1603;
const int UIID_GROUP_LISTENING_FRONT    = 1604;
const int UIID_GROUP_LISTENING_FRONTL   = 1605;
const int UIID_GROUP_LISTENING_FRONTR   = 1606;
const int UIID_GROUP_LISTENING_ALL      = 1607;
const int UIID_GROUP_LISTENING_OFF      = 1608;

const int UIID_GROUP_TIME_TYPE   = 1704;
const int UIID_GROUP_TIME_TYPE_L = 1705;
const int UIID_GROUP_TIME_TYPE_R = 1706;
const int UIID_GROUP_TIME_FL_L   = 1708;
const int UIID_GROUP_TIME_FL_R   = 1709;
const int UIID_GROUP_TIME_FL_V   = 1710;
const int UIID_GROUP_TIME_FR_L   = 1713;
const int UIID_GROUP_TIME_FR_R   = 1714;
const int UIID_GROUP_TIME_FR_V   = 1715;
const int UIID_GROUP_TIME_RL_L   = 1718;
const int UIID_GROUP_TIME_RL_R   = 1719;
const int UIID_GROUP_TIME_RL_V   = 1720;
const int UIID_GROUP_TIME_RR_L   = 1723;
const int UIID_GROUP_TIME_RR_R   = 1724;
const int UIID_GROUP_TIME_RR_V   = 1725;
const int UIID_GROUP_TIME_SUB_L  = 1728;
const int UIID_GROUP_TIME_SUB_R  = 1729;
const int UIID_GROUP_TIME_SUB_V  = 1730;
const int UIID_GROUP_TIME_BG     = 1732;
const int UIID_GROUP_TIME_ONOFF  = 1733;


/**LOUDNES控件*/
const int UIID_GROUP_SUBFOOER_OFF   = 1911; 
const int UIID_GROUP_SUBFOOER_ON    = 1912; 

const int UIID_GROUP_EQ_GAIN_1    = 1803;
const int UIID_GROUP_EQ_GAIN_2    = 1804;
const int UIID_GROUP_EQ_GAIN_3    = 1805;
const int UIID_GROUP_EQ_GAIN_4    = 1806;
const int UIID_GROUP_EQ_GAIN_5    = 1807;
const int UIID_GROUP_EQ_GAIN_6    = 1808;
const int UIID_GROUP_EQ_GAIN_7    = 1809;
const int UIID_GROUP_EQ_GAIN_8    = 1810;
const int UIID_GROUP_EQ_GAIN_9    = 1811;
const int UIID_GROUP_EQ_GAIN_10   = 1812;
const int UIID_GROUP_EQ_GAIN_11   = 1813;
const int UIID_GROUP_EQ_GAIN_12   = 1814;
const int UIID_GROUP_EQ_GAIN_13   = 1815;
const int UIID_GROUP_EQ_SUPERBASS = 1816;
const int UIID_GROUP_EQ_POWERFUL  = 1817;
const int UIID_GROUP_EQ_NATURAL   = 1818;
const int UIID_GROUP_EQ_VOCAL     = 1819;
const int UIID_GROUP_EQ_CONT      = 1820;
const int UIID_GROUP_EQ_LEFT      = 1821;
const int UIID_GROUP_EQ_RIGHT     = 1822;

const int UIID_GROUP_SPEAKER_OK     = 1004;
const int UIID_GROUP_SPEAKER_3WAY   = 1208;
const int UIID_GROUP_SPEAKER_2WAY   = 1209;

const int UIID_GROUP_CROSSOVER_L        = 2004;
const int UIID_GROUP_CROSSOVER_R        = 2005;
const int UIID_GROUP_CROSSOVER_TYPE     = 2003;
const int UIID_GROUP_CROSSOVER_HPF      = 2009;
const int UIID_GROUP_CROSSOVER_SLOPE    = 2012;
const int UIID_GROUP_CROSSOVER_ONOFF    = 2007;
const int UIID_GROUP_CROSSOVER_H        = 2015;
const int UIID_GROUP_CROSSOVER_V        = 2016;


#endif /* INCLUDE_UIID_DEF_AUDIOBOOST_H_ */