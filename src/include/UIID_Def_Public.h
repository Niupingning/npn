/*
 * UIID_Def_Home.h
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#ifndef INCLUDE_UIID_DEF_PUBLIC_H_
#define INCLUDE_UIID_DEF_PUBLIC_H_


 /* window Id*/
const int UIID_WNDID_MEDIA_LOADING = 6;
const int UIID_WNDID_MEDIA_AUDIO_PLAY   = 11;
const int UIID_WNDID_MEDIA_VIDEO_PLAY   = 7;
const int UIID_WNDID_MEDIA_AUDIO_LIST   = 8;
const int UIID_WNDID_MEDIA_VIDEO_LIST   = 9;
const int UIID_WNDID_MEDIA_VIDEO_PARKING   = 10;
const int UIID_WNDID_MEDIA_FAIL = 12;
const int UIID_WNDID_MAIN = 1;
const int UIID_WNDID_BLACK = 13;
const int UIID_WNDID_APP = 98;      //carplay
const int UIID_WNDID_BT_PHONE = 4;      //蓝牙电话
const int UIID_WNDID_BT_MUSIC = 3;      //蓝牙音频
const int UIID_WNDID_RADIO = 2;      //radio
const int UIID_WNDID_SETTING = 16;   //setting
const int UIID_WNDID_AVM = 19;   //AVM
const int UIID_WNDID_AIRCOND = 20;   //AIRCONDITIONAR
const int UIID_WNDID_AUDIOBOOST = 21;   //eq
const int UIID_WNDID_DTS = 22;   //dts
const int UIID_WNDID_DEMO = 23;   //demo window
const int UIID_WNDID_NETWORK_MODE = 24;    //network mode
const int UIID_WINID_TI = 25;   //TI

const int UIID_WNDID_SPLIT = 30;   //窗口30 CP/AA分屏

/* 窗口30 顶层元件 */
const int UIID_SPLIT_CPAA_AREA      = 1;    //CP/AA显示区域
const int UIID_SPLIT_MEDIA_AREA     = 2;    //媒体源显示区域
const int UIID_SPLIT_MEDIA_NAME     = 3;    //媒体源名称

/* 窗口30 收音分组 1017 */
const int UIID_SPLIT_GROUP_RADIO    = 1017;
const int UIID_SPLIT_RADIO_BAND     = 1004; //AM/FM
const int UIID_SPLIT_RADIO_PAGE     = 1005; //标签页码
const int UIID_SPLIT_RADIO_BSM      = 1006; //BSM
const int UIID_SPLIT_RADIO_PS       = 1010; //PS
const int UIID_SPLIT_RADIO_DX       = 1011; //DX
const int UIID_SPLIT_RADIO_FREQ     = 1007; //频道显示
const int UIID_SPLIT_RADIO_UNIT     = 1008; //MHz / kHz
const int UIID_SPLIT_RADIO_PREV     = 1013; //上一曲
const int UIID_SPLIT_RADIO_SEARCHDN = 1014; //上一个
const int UIID_SPLIT_RADIO_SEARCHUP = 1015; //下一个
const int UIID_SPLIT_RADIO_NEXT     = 1016; //下一曲

/* 窗口30 USB音乐分组 1029 */
const int UIID_SPLIT_GROUP_USB      = 1029;
const int UIID_SPLIT_MEDIA_TITLE    = 1030; //歌曲名称
const int UIID_SPLIT_MEDIA_ARTIST   = 1031; //歌手名称
const int UIID_SPLIT_MEDIA_ALBUM    = 1032; //专辑名称
const int UIID_SPLIT_MEDIA_PIC      = 1033; //专辑封面
const int UIID_SPLIT_MEDIA_PROGRESS = 1034; //扩展进度条
const int UIID_SPLIT_MEDIA_TIME     = 1039; //播放时间
const int UIID_SPLIT_MEDIA_TOTAL    = 1040; //歌曲总时间
const int UIID_SPLIT_MEDIA_PREV     = 1035; //上一曲
const int UIID_SPLIT_MEDIA_PLAY     = 1036; //播放
const int UIID_SPLIT_MEDIA_PAUSE    = 1037; //暂停
const int UIID_SPLIT_MEDIA_NEXT     = 1038; //下一曲
const int UIID_SPLIT_MEDIA_LOOP     = 1041; //循环播放
const int UIID_SPLIT_MEDIA_SHUFFLE  = 1042; //随机播放
const int UIID_SPLIT_MEDIA_FLODER   = 1043; //文件夹循环播放
const int UIID_SPLIT_MEDIA_SINGLE   = 1044; //单曲循环播放

/* 窗口30 分页点 / 无设备 */
const int UIID_SPLIT_PAGE_DOT_RADIO = 1045; //分页符-1(Radio)
const int UIID_SPLIT_PAGE_DOT_USB   = 1046; //分页符-2(USB)
const int UIID_SPLIT_NO_DEVICE      = 1048; //No Device

 /* title public Id*/
// const int UIID_PUBLIC_SETTINGH_BIN = 5;  //顶栏设置按钮
// const int UIID_PUBLIC_DATE_BIN = 2;      //日期
// const int UIID_PUBLIC_WEEK_BIN = 3;      //星期
// const int UIID_PUBLIC_TIME_BIN = 4;      //时间
const int UIID_PUBLIC_SETTINGH_BIN = 1306;  //设置
const int UIID_PUBLIC_DATE_BIN = 2009;      //日期
const int UIID_PUBLIC_WEEK_BIN = 2010;      //星期
const int UIID_PUBLIC_TIME_BIN = 2011;      //时间
const int UIID_PUBLIC_CLOSE_BIN = 2212;     //顶栏关闭（回主界面）

/** audio popo public id */
const int UIID_PUBLIC_AUIDO_POP_SURCE_NAME= 409; //音量条显示源名称
const int UIID_PUBLIC_AUIDO_POP_BAR= 408;      //音量BAR
const int UIID_PUBLIC_AUIDO_POP_VALUE= 407;    //音量值
const int UIID_PUBLIC_AUIDO_POP_ADD= 406;    //音量+
const int UIID_PUBLIC_AUIDO_POP_SUB= 405;    //音量值-
const int UIID_PUBLIC_AUIDO_POP_PIC= 404;    //音量图片
// const int UIID_PUBLIC_AUIDO_POP_BAR= 2018;      //音量BAR
// const int UIID_PUBLIC_AUIDO_POP_VALUE= 2017;    //音量值
// const int UIID_PUBLIC_AUIDO_POP_ADD= 2016;    //音量+
// const int UIID_PUBLIC_AUIDO_POP_SUB= 2015;    //音量值-
// const int UIID_PUBLIC_AUIDO_POP_PIC= 2014;    //音量图片

const int UIID_PUBLIC_AUIDO_POP_GROUP= 400;    //音量分组

const int UIID_AVM_RETURN= 1004;    //按钮返回
const int UIID_AVM_3D = 1005;  //3D
const int UIID_AVM_REAR = 1006; //后
const int UIID_AVM_FRONT = 1007; //前
const int UIID_AVM_LEFT = 1008; //左
const int UIID_AVM_RIGHT = 1009; //右

const int UUID_BACK_PIC = 2320;   //黑屏背景图片ID

enum
{
    IVI_SOURCE_IDLE       = 0,
    IVI_SOURCE_HOME       = 1,
    IVI_SOURCE_MEDIA      = 2,
    IVI_SOURCE_BT_MUSIC   = 3,
    IVI_SOURCE_BT_PHONE   = 4,
    IVI_SOURCE_SETTING    = 5,
    IVI_SOURCE_AVM    = 6,
    IVI_SOURCE_RADIO    = 7,
    IVI_SOURCE_CPAA    = 8,
};

/* 分屏右侧卡片页 */
enum
{
    SPLIT_PAGE_RADIO = 0,
    SPLIT_PAGE_USB   = 1,
};

/*
 * Text ID*/

#endif /* INCLUDE_UIID_DEF_PUBLIC_H_ */
