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
#ifndef FACTORY_UIID_DEF_H_
#define FACTORY_UIID_DEF_H_

/**设置项列表*/
const int UIID_SETTING_LIST = 1300;     //列表

/**顶栏显示*/
const int UIID_SETTING_RETURN = 1003;   //返回
const int UIID_SETTING_CLOSE = 1004;   //关闭

/**侧边栏显示*/
const int UIID_SETTING_SETTING = 1103; //设置
const int UIID_SETTING_AUDIO = 1104; //音量
const int UIID_SETTING_CAR = 1105; //车辆

/**分组*/
const int UIID_GROUP_SETTING = 1201; //设置项列表分组
const int UIID_GROUP_DATE = 1301;   //日期分组
const int UIID_GROUP_BKLIGHT = 1401; //背光分组
const int UIID_GROUP_RESET = 1501; //复位分组
const int UIID_GROUP_SYSTEM = 1601; //系统分组
const int UIID_GROUP_AUDIO = 1701; //音量分组
const int UIID_GROUP_SIDE = 1101; //侧边分组
const int UIID_GROUP_CANBUS_PASSWORD = 1801; //密码输入
const int UIID_GROUP_CANBUS_SETTINGS = 1901; //canbus 设置
const int UIID_GROUP_CANBUS_POP = 2001; //canbus 弹窗
const int UIID_GROUP_CAR_SETTING = 2101; //车辆设置
const int UUID_GROUP_CANBOX_CAR = 2401;  //canbox
const int UUID_GROUP_CANBOX_CONNECT = 2501; //canbox 连接

/** 复位 */
const int UIID_SETTING_REST_OK = 1505;  //复位ok按钮
const int UIID_SETTING_REST_CANCEL = 1504; //复位cancel按钮

/**日期*/
const int UIID_SETTING_DATETIME_DATE_UP = 1303;  //日期日按钮向上
const int UIID_SETTING_DATETIME_DATE = 1304;  //日期日按钮
const int UIID_SETTING_DATETIME_DATE_DN = 1305;  //日期日按钮向下

const int UIID_SETTING_DATETIME_MONTH_UP = 1306;  //日期月按钮向上
const int UIID_SETTING_DATETIME_MONTH = 1307;  //日期月按钮
const int UIID_SETTING_DATETIME_MONTH_DN = 1308;  //日期月按钮向下

const int UIID_SETTING_DATETIME_YEAR_UP = 1309;  //日期年按钮向上
const int UIID_SETTING_DATETIME_YEAR = 1310;  //日期年按钮
const int UIID_SETTING_DATETIME_YEAR_DN = 1311;  //日期年按钮向下

const int UIID_SETTING_DATETIME_HOURS_UP = 1312;  //日期时按钮向上
const int UIID_SETTING_DATETIME_HOURS = 1313;  //日期时按钮
const int UIID_SETTING_DATETIME_HOURS_DN = 1314;  //日期时按钮向下

const int UIID_SETTING_DATETIME_MIN_UP = 1315;  //日期分按钮向上
const int UIID_SETTING_DATETIME_MIN = 1316;  //日期分按钮
const int UIID_SETTING_DATETIME_MIN_DN = 1317;  //日期分按钮向下

const int UIID_SETTING_DATETIME_MODE_UP = 1318;  //日期时制按钮向上
const int UIID_SETTING_DATETIME_MODE = 1319;  //日期时制按钮
const int UIID_SETTING_DATETIME_MODE_DN = 1320;  //日期时制按钮向下

const int UIID_SETTING_DATETIME_DDMM = 1325;  //dd-mm-yy
const int UIID_SETTING_DATETIME_MMDD = 1326;  //mm-dd-yy

/** 背光 */
const int UIID_SETTING_BRIGHT_BAR = 1409;  //背光进度条
const int UIID_SETTING_BRIGHT_VALUE = 1404;  //背光亮度值
const int UIID_SETTING_BRIGHT_LEFT = 1405;  //背光左按钮
const int UIID_SETTING_BRIGHT_RIGHT = 1407;  //背光左按钮

/** 版本 */
const int UIID_SETTING_VERSION_LIST = 1700;  //版本列表

/** 音量设置 */
const int UIID_SETTING_AUDIO_MAIN_VALUE = 1704;  //音量值
const int UIID_SETTING_AUDIO_MAIN_SUB = 1705;  //音量值-
const int UIID_SETTING_AUDIO_MAIN_ADD = 1706;  //音量值+
const int UIID_SETTING_AUDIO_MAIN_BAR = 1707;  //进度条

const int UIID_SETTING_AUDIO_BT_VALUE = 1709;  //音量值
const int UIID_SETTING_AUDIO_BT_SUB = 1710;  //音量值-
const int UIID_SETTING_AUDIO_BT_ADD = 1711;  //音量值+
const int UIID_SETTING_AUDIO_BT_BAR = 1712;  //进度条

/** CANBOX 车型选择 */
const int UIID_SETTING_CANBOX_CAR_LIST = 2500;  //列表
const int UIID_SETTING_CANBOX_NAME = 2503;  //hiworld
const int UIID_SETTING_CANBOX_CARTYPE = 2504;  //车型名称
const int UIID_SETTING_CANBOX_PRO = 2505;   //车型型号
const int UIID_SETTING_CANBOX_CONNECT = 3001;   //connection


/**
 * 公共组件使用  设置栏
 */

const int UIID_LEFT_BAR_GROUP = 1;
const int UIID_SETTING_GROUP = 7;   // seting
const int UIID_BOTTOM_BAR_GROUP = 3;  // 底栏
const int UIID_APP_RADIO_BG_BTN = 5;
const int UIID_SETTING_BG_BTN = 6;

const int UIID_BACK_BTN = 130;


/**
 * left bar 左侧栏
 */

const int UIID_APP_BTN = 11;
const int UIID_APP_TEXT = 12;
const int UIID_RADIO_BTN = 13;
const int UIID_RADIO_TEXT = 14;
const int UIID_HOME_BTN = 15;
const int UIID_HOME_TEXT = 16;


/**
 * bottom bar 底栏
 */
const int UIID_BOTTOM_GROUP 	= 7;
const int UIID_VOLUME_BTN 		= 35;
const int UIID_BRIGHT_BTN 		= 36;
const int UIID_BT_BTN 		    = 37;
const int UIID_WIFI_BTN 		= 38;
const int UIID_CAR_BTN 		    = 39;
const int UIID_SYSTEM_BTN 		= 40;


/**
 * volume adjust 音量调节
 */
const int UIID_VOLUME_ADJUST_GROUP 			= 132;
const int UIID_VOLUME_PAGE_TITLE_TEXT	    = 148;
const int UIID_VOLUME_ICON_BTN	 	        = 142;

const int UIID_VOLUME_MUTE_SWITCH_BTN	 	= 155;
const int UIID_VOLUME_MUTE_TITLE_TEXT	 	= 156;
const int UIID_VOLUME_MUTE_STATUS_TEXT	    = 157;

const int UIID_VOLUME_MAIN_TITLE_TEXT	 	= 158;
const int UIID_VOLUME_PROGRESS_BAR  	    = 159;   // 进度条

const int UIID_VOLUME_KEYTONE_TITLE_TEXT	= 160;
const int UIID_VOLUME_KEYTONE_SWITCH_BTN	= 161;
const int UIID_VOLUME_DATA_TEXT         	= 162;

const int UIID_VOLUME_BTVOLUME_TITLE_TEXT	 = 301;
const int UIID_VOLUME_BTVOLUME_BAR  	     = 302;   // 进度条
const int UIID_VOLUME_BTVOLUME_DATA_TEXT     = 303;

const int UIID_VOLUME_COMBOBOX_GROUP 			    = 700;
const int UIID_VOLUME_MUTE_SWITCH_COMBOBOX_BTN	 	= 709;
const int UIID_VOLUME_DATA_COMBOBOX_TEXT         	= 703;
const int UIID_VOLUME_PROGRESS_COMBOBOX_BAR  	    = 706;   // 进度条
const int UIID_VOLUME_BTVOLUME_COMBOBOX_BAR  	    = 707;   // 进度条
const int UIID_VOLUME_BTVOLUME_DATA_COMBOBOX_TEXT   = 705;

/**
 * brightness adjust 亮度调节
 */
const int UIID_BRIGHT_GROUP 			    = 133;
const int UIID_BRIGHT_PAGE_TITLE_TEXT	 	= 149;
const int UIID_BRIGHT_ICON_BTN	 	        = 143;

const int UIID_BRIGHT_AUTO_TITLE_TEXT	 	= 162;
const int UIID_BRIGHT_AUTO_SWITCH_BTN	 	= 163;

const int UIID_BRIGHT_CURR_TITLE_TEXT	 	= 164;
const int UIID_BRIGHT_PROGRESS_BAR	 	    = 165;   // 进度条
const int UIID_BRIGHT_DATA_TEXT	 	        = 166;   

/**
 * Bluetooth Set 蓝牙设置
 */
const int UIID_BT_SET_GROUP 				= 134;
const int UIID_BT_PAGE_TITLE_TEXT	 		= 150;
const int UIID_BT_ICON_BTN	 			    = 144;

const int UIID_BT_STATUS_TEXT	 			= 167;
const int UIID_BT_SWITCH_BTN	 			= 166;
const int UIID_BT_NAME_TITLE_TEXT	 		= 168;
const int UIID_BT_NAME_TEXT	 			    = 169;
const int UIID_BT_BLUE_BTN	 			    = 183;
const int UIID_BT_PAIRED_DEVICE_TEXT	 	= 184;

const int UIID_BT_DEVICE1_BTN	 			= 185;
const int UIID_BT_DEVICE2_BTN	 			= 186;
const int UIID_BT_DEVICE3_BTN	 			= 187;
const int UIID_BT_DEVICE4_BTN	 			= 188;
const int UIID_BT_DEVICE5_BTN	 			= 189;

const int UIID_BT_VECTOR_LIST_BAR	 		= 190;
const int UIID_BT_AREA_BTN	 			    = 191;
const int UIID_BT_CONNECT_STATUS_BTN	 	= 192;
const int UIID_BT_CONNECTED_ICON_BTN	 	= 193;
const int UIID_BT_LIST_GROUP	 		    = 202;
const int UIID_BT_UNOPEN_BTN	 		    = 203;

const int UIID_BT_MANAGE_GROUP 				= 8;
const int UIID_BT_MANAGE_POPUP_BG	 		= 204;
const int UIID_BT_MANAGE_POPUP_TITLE_TEXT	= 205;
const int UIID_BT_MANAGE_TEXT	 		    = 206;
const int UIID_BT_MANAGE_CANCEL	 			= 207;
const int UIID_BT_MANAGE_DISCONNECT	 	    = 208;
const int UIID_BT_MANAGE_FORGET	 	        = 209;


const int UIID_BT_PAIRE_GROUP 				= 9;
const int UIID_BT_PAIRE_POPUP_BG	 		= 211;
const int UIID_BT_PAIRE_POPUP_TITLE_TEXT	= 212;
const int UIID_BT_PAIRE_TEXT	 		    = 213;
const int UIID_BT_PAIRE_CANCEL	 			= 214;
const int UIID_BT_PAIRE_PAIRE	 	        = 215;
const int UIID_BT_PAIRE_NO_DEVICE	 	    = 216;

/**
 * Wifi Set Wifi设置
 */
const int UIID_WIFI_SET_GROUP 				= 131;
const int UIID_WIFI_SET_TITLE_TEXT	 		= 132;
const int UIID_WIFI_ICON_BTN	 		    = 133;
const int UIID_WIFI_STATUS_TEXT	 		    = 134;
const int UIID_WIFI_SWITCH_BTN	 		    = 135;
const int UIID_WIFI_BLUE_BTN	 			= 138;
const int UIID_WIFI_PAIRED_DEVICE_TEXT	 	= 139;

const int UIID_WIFI_DEVICE1_BTN	 			= 146;
const int UIID_WIFI_DEVICE2_BTN	 			= 140;
const int UIID_WIFI_DEVICE3_BTN	 			= 141;
const int UIID_WIFI_DEVICE4_BTN	 			= 142;
const int UIID_WIFI_DEVICE5_BTN	 			= 143;

const int UIID_WIFI_VECTOR_LIST_BAR	 		= 144;
const int UIID_WIFI_AREA_BTN	 			= 145;
const int UIID_WIFI_CONNECT_STATUS_BTN	 	= 147;
const int UIID_WIFI_CONNECTED_ICON_BTN	 	= 148;
const int UIID_WIFI_LIST_GROUP	 		    = 149;

const int UIID_WIFI_UNOPEN_BTN	 		    = 203;

/**
 * Car Set 车辆设置
 */
const int UIID_CAR_SET_GROUP 				= 136;
const int UIID_CAR_SET_TITLE_TEXT	 		= 152;
const int UIID_CAR_SET_ICON_BTN	 			= 146;


const int UIID_CAR_TRIP_RESET_BTN	 		= 163;
const int UIID_CAR_UNIT_1_BTN	 			= 164;
const int UIID_CAR_UNIT_2_BTN	 			= 166;
const int UIID_CAR_READ_SWITCH_BTN	 	    = 168;
const int UIID_CAR_WHEEL_1_BTN	 			= 169;
const int UIID_CAR_WHEEL_2_BTN	 			= 171;
const int UIID_CAR_MAINTIAN_MILEAGE_BTN	 	= 173;
const int UIID_CAR_MAINTIAN_RESET_BTN	 	= 174;

/**
 * Mantain Set 保养里程设置
 */
const int UIID_MAINTAIN_MILEAGE_GROUP 		    = 517;

const int UIID_MAINTAIN_MILEAGE_BG 		        = 518;
const int UIID_MAINTAIN_MILEAGE_BG_BTN 		    = 519;
const int UIID_MAINTAIN_MILEAGE_TITLE_TEXT 		= 520;
const int UIID_MAINTAIN_MILEAGE_CANCEL 		    = 521;
const int UIID_MAINTAIN_MILEAGE_OK 		        = 522;
const int UIID_MAINTAIN_MILEAGE_TIP_TEXT 		= 523;
const int UIID_MAINTAIN_MILEAGE_UP 		        = 525;
const int UIID_MAINTAIN_MILEAGE_DOWN 		    = 526;
const int UIID_MAINTAIN_MILEAGE_MID 		    = 527;
const int UIID_MAINTAIN_MILEAGE_LIST 		    = 529;

/*
*胎压复位
*/
const int UIID_CAR_TIRE_RESET_TEXT_BTN	 	= 636;
const int UIID_CAR_TIRE_RESET_1_SEQ	 	= 637;
const int UIID_CAR_TIRE_RESET_1_TEXT_BTN	= 638;

const int UIID_CAR_TIRE_RESET_1_BTN	 	= 627;
const int UIID_CAR_TIRE_RESET_2_BTN	 	= 628;
const int UIID_CAR_TIRE_RESET_BTN	 	= 629;
const int UIID_CAR_TIRE_RESET_SEQ	 	= 635;

/*
*转向模式
*/
const int UIID_CAR_TURN_MOD_TEXT_BAR 	= 642;
const int UIID_CAR_TURN_MOD_BAR 	= 643;
const int UIID_CAR_TURN_MOD_1_TEXT_BAR 	= 753;
const int UIID_CAR_TURN_MOD_1_BAR 	= 754;

const int UIID_CAR_TURN_MOD_2_TEXT_BAR 	= 632;
const int UIID_CAR_TURN_MOD_2_BAR 	= 633;

/*
*滚动视窗
*/
const int UIID_CAR_WIND_BTN 	= 660;
const int UIID_CAR_WIND_BAR     = 752;


/**
 * 胎压复位提示
 * */
const int UIID_TIRE_RSET_GROUP 		          =  780;
const int UIID_TIRE_RSET_FAIL_ICON_BIN	      =  783;
const int UIID_TIRE_RSET_FAIL_TEXT_BIN	      =  784;
const int UIID_TIRE_RSET_WARN_ICON_BIN	      =  785;
const int UIID_TIRE_RSET_WARN_TEXT_BIN	      =  786;
const int UIID_TIRE_RSET_FINISH_ICON_BIN	  =  787;
const int UIID_TIRE_RSET_FINISH_TEXT_BIN	  =  788;
const int UIID_TIRE_RSET_ONCLICK_ICON_BIN	  =  789;

/*
*能量回收
*/
const int UIID_CAR_ENERGY_RECOVER_BIN	 	= 630;
const int UIID_CAR_ENERGY_RECOVER_BAR	 	= 631;

/**
 * System Set 系统设置
 */
const int UIID_SYSTEM_SYS_GROUP 				= 7;
const int UIID_SYSTEM_SYS_BACK_BTN 				= 130;
const int UIID_SYSTEM_SYS_SET_GROUP 			= 137;
const int UIID_SYSTEM_SYS_TITLE_TEXT	 		= 153;
const int UIID_SYSTEM_SYS_ICON_BTN	 		    = 147;

const int UIID_SYSTEM_SYS_NEXT_1_BTN	 		= 164;
const int UIID_SYSTEM_SYS_NEXT_2_BTN	 		= 165;
const int UIID_SYSTEM_SYS_NEXT_3_BTN	 		= 166;
const int UIID_SYSTEM_SYS_NEXT_4_BTN	 		= 167;
const int UIID_SYSTEM_SYS_NEXT_5_BTN	 		= 168;

const int UIID_SYSTEM_SYS_CHECKBOX_1_BTN	 	= 169;
const int UIID_SYSTEM_SYS_CHECKBOX_2_BTN	 	= 170;
const int UIID_SYSTEM_SYS_CHECKBOX_3_BTN	 	= 171;
const int UIID_SYSTEM_SYS_CHECKBOX_4_BTN	 	= 172;
const int UIID_SYSTEM_SYS_CHECKBOX_5_BTN		= 173;


const int UIID_SYSTEM_SYS_TITLE_1_BTN	 		= 154;
const int UIID_SYSTEM_SYS_TITLE_2_BTN	 		= 155;
const int UIID_SYSTEM_SYS_TITLE_3_BTN	 		= 156;
const int UIID_SYSTEM_SYS_TITLE_4_BTN	 		= 157;
const int UIID_SYSTEM_SYS_TITLE_5_BTN	 		= 158;


/**
 * System Set 系统设置------版本信息
 */
const int UIID_SYSTEM_VER_GROUP 		    = 8;
const int UIID_SYSTEM_VER_BACK_BTN          = 175;          
const int UIID_SYSTEM_VER_SET_GROUP 		= 176;
const int UIID_SYSTEM_VER_TITLE_TEXT	 	= 177;
const int UIID_SYSTEM_VER_ICON_BTN	 	    = 198;

const int UIID_SYSTEM_VER_NEXT_BTN	 		= 429;

const int UIID_SYSTEM_VER_TITLE_1_BTN	 		= 178;
const int UIID_SYSTEM_VER_TITLE_2_BTN	 		= 179;
const int UIID_SYSTEM_VER_TITLE_3_BTN	 		= 180;
const int UIID_SYSTEM_VER_TITLE_4_BTN	 		= 181;
const int UIID_SYSTEM_VER_TITLE_5_BTN	 		= 182;

const int UIID_SYSTEM_VER_LINE_1_BTN	 		= 183;
const int UIID_SYSTEM_VER_LINE_2_BTN	 		= 184;
const int UIID_SYSTEM_VER_LINE_3_BTN	 		= 185;
const int UIID_SYSTEM_VER_LINE_4_BTN	 		= 186;
const int UIID_SYSTEM_VER_LINE_5_BTN	 		= 187;

const int UIID_SYSTEM_VER_CHECKBOX_1_BTN	 		= 193;
const int UIID_SYSTEM_VER_CHECKBOX_2_BTN	 		= 194;
const int UIID_SYSTEM_VER_CHECKBOX_3_BTN	 		= 195;
const int UIID_SYSTEM_VER_CHECKBOX_4_BTN	 		= 196;
const int UIID_SYSTEM_VER_CHECKBOX_5_BTN	 		= 197;


/**
 * System Set 系统设置------屏幕版本信息
 */

const int UIID_SYSTEM_SCREEN_GROUP 		        = 330;
const int UIID_SYSTEM_SCREEN_BACK_BTN 		    = 410;
const int UIID_SYSTEM_SCREEN_TITLE_TEXT 		= 412;

const int UIID_SYSTEM_SCREEN_TITLE_1_BTN		= 413;
const int UIID_SYSTEM_SCREEN_TITLE_2_BTN 		= 414;
const int UIID_SYSTEM_SCREEN_TITLE_3_BTN		= 415;
const int UIID_SYSTEM_SCREEN_TITLE_4_BTN		= 416;
const int UIID_SYSTEM_SCREEN_TITLE_5_BTN		= 417;

const int UIID_SYSTEM_SCREEN_LINE_1_BTN	 		= 418;
const int UIID_SYSTEM_SCREEN_LINE_2_BTN	 		= 419;
const int UIID_SYSTEM_SCREEN_LINE_3_BTN	 		= 420;
const int UIID_SYSTEM_SCREEN_LINE_4_BTN	 		= 421;
const int UIID_SYSTEM_SCREEN_LINE_5_BTN	 		= 422;

const int UIID_SYSTEM_SCREEN_CHECKBOX_1_BTN	 		= 423;
const int UIID_SYSTEM_SCREEN_CHECKBOX_2_BTN	 		= 424;
const int UIID_SYSTEM_SCREEN_CHECKBOX_3_BTN	 		= 425;
const int UIID_SYSTEM_SCREEN_CHECKBOX_4_BTN	 		= 426;
const int UIID_SYSTEM_SCREEN_CHECKBOX_5_BTN	 		= 427;

/**
 * System Set 系统设置------语言
 */
const int UIID_SYSTEM_LANG_GROUP 		    = 9;
const int UIID_SYSTEM_LANG_BACK_BTN         = 200;          
const int UIID_SYSTEM_LANG_SET_GROUP 		= 201;
const int UIID_SYSTEM_LANG_TITLE_TEXT	 	= 202;
const int UIID_SYSTEM_LANG_ICON_BTN	 	    = 218;

const int UIID_SYSTEM_LANG_TITLE_1_BTN	 		= 203;
const int UIID_SYSTEM_LANG_TITLE_2_BTN	 		= 204;
const int UIID_SYSTEM_LANG_TITLE_3_BTN	 		= 205;
const int UIID_SYSTEM_LANG_TITLE_4_BTN	 		= 206;
const int UIID_SYSTEM_LANG_TITLE_5_BTN	 		= 207;

const int UIID_SYSTEM_LANG_LINE_1_BTN	 		= 208;
const int UIID_SYSTEM_LANG_LINE_2_BTN	 		= 209;
const int UIID_SYSTEM_LANG_LINE_3_BTN	 		= 210;
const int UIID_SYSTEM_LANG_LINE_4_BTN	 		= 211;
const int UIID_SYSTEM_LANG_LINE_5_BTN	 		= 212;

const int UIID_SYSTEM_LANG_CHECKBOX_1_BTN	 		= 213;
const int UIID_SYSTEM_LANG_CHECKBOX_2_BTN	 		= 214;
const int UIID_SYSTEM_LANG_CHECKBOX_3_BTN	 		= 215;
const int UIID_SYSTEM_LANG_CHECKBOX_4_BTN	 		= 216;
const int UIID_SYSTEM_LANG_CHECKBOX_5_BTN	 		= 217;

const int UIID_SYSTEM_LANG_SINGLEBOX_1_BTN	 		= 219;
const int UIID_SYSTEM_LANG_SINGLEBOX_2_BTN	 		= 220;




/**
 * System Set 系统设置------时间设置
 */
const int UIID_SYSTEM_AUTO_SYNC_TIME_GROUP 		    = 430;
const int UIID_SYSTEM_AUTO_SYNC_TIME_BACK_BTN       = 431;  
const int UIID_SYSTEM_AUTO_SYNC_TIME_TITLE_TEXT	 	= 433;

const int UIID_SYSTEM_AUTO_SYNC_TIME_SWITCH_BTN	 	= 450;
const int UIID_SYSTEM_AUTO_SYNC_TIME_SINGLEBOX_BTN	 = 451;

const int UIID_SYSTEM_AUTO_SYNC_TIME_TITLE_1_BTN	= 434;
const int UIID_SYSTEM_AUTO_SYNC_TIME_TITLE_2_BTN	= 435;
const int UIID_SYSTEM_AUTO_SYNC_TIME_TITLE_3_BTN	= 436;
const int UIID_SYSTEM_AUTO_SYNC_TIME_TITLE_4_BTN	= 437;
const int UIID_SYSTEM_AUTO_SYNC_TIME_TITLE_5_BTN	= 438;

const int UIID_SYSTEM_AUTO_SYNC_TIME_LINE_1_BTN	 	= 439;
const int UIID_SYSTEM_AUTO_SYNC_TIME_LINE_2_BTN	 	= 440;
const int UIID_SYSTEM_AUTO_SYNC_TIME_LINE_3_BTN	 	= 441;
const int UIID_SYSTEM_AUTO_SYNC_TIME_LINE_4_BTN	 	= 442;
const int UIID_SYSTEM_AUTO_SYNC_TIME_LINE_5_BTN	 	= 443;


const int UIID_SYSTEM_AUTO_SYNC_TIME_CHECKBOX_1_BTN	 = 444;
const int UIID_SYSTEM_AUTO_SYNC_TIME_CHECKBOX_2_BTN	 = 445;
const int UIID_SYSTEM_AUTO_SYNC_TIME_CHECKBOX_3_BTN	 = 446;
const int UIID_SYSTEM_AUTO_SYNC_TIME_CHECKBOX_4_BTN	 = 447;
const int UIID_SYSTEM_AUTO_SYNC_TIME_CHECKBOX_5_BTN	 = 448;

/**
 * System Set 系统设置------日期弹框
 */
const int UIID_SYSTEM_DTAE_GROUP 		    = 10;

const int UIID_SYSTEM_DTAE_TITLE_DATA_BTN	 	= 223;
const int UIID_SYSTEM_DTAE_TITLE_TIME_BTN	 	= 224;

const int UIID_SYSTEM_DTAE_YEAR_DATA_BTN	 	= 228;
const int UIID_SYSTEM_DTAE_MONTH_DATA_BTN	 	= 229;
const int UIID_SYSTEM_DTAE_DAY_DATA_BTN	 	    = 230;

const int UIID_SYSTEM_DTAE_YEAR_UP_BTN	 		= 231;
const int UIID_SYSTEM_DTAE_YEAR_DOWN_BTN	 	= 234;

const int UIID_SYSTEM_DTAE_MONTH_UP_BTN	 		= 232;
const int UIID_SYSTEM_DTAE_MONTH_DOWN_BTN	 	= 235;

const int UIID_SYSTEM_DTAE_DAY_UP_BTN	 		= 233;
const int UIID_SYSTEM_DTAE_DAY_DOWN_BTN	 	    = 236;

const int UIID_SYSTEM_DTAE_NEXT_BTN	 	        = 225;


/**
 * System Set 系统设置------时间弹框
 */
const int UIID_SYSTEM_TIME_GROUP 		    = 4;

const int UIID_SYSTEM_TIME_TITLE_DATA_BTN	 	= 239;
const int UIID_SYSTEM_TIME_TITLE_TIME_BTN	 	= 240;

const int UIID_SYSTEM_TIME_HOURS_DATA_BTN	 	= 243;
const int UIID_SYSTEM_TIME_MINUTES_DATA_BTN	 	= 244;

const int UIID_SYSTEM_TIME_HOURS_UP_BTN	 		= 246;
const int UIID_SYSTEM_TIME_HOURS_DOWN_BTN	 	= 249;

const int UIID_SYSTEM_TIME_MINUTES_UP_BTN	 	= 247;
const int UIID_SYSTEM_TIME_MINUTES_DOWN_BTN	 	= 250;

const int UIID_SYSTEM_DTAE_FINISH_BTN	 	    = 252;

/**
 * System Set 系统设置------App下载
 */

const int UIID_SYSTEM_APP_GROUP 		    = 452;
const int UIID_SYSTEM_APP_BG_BTN	 	    = 453;
const int UIID_SYSTEM_APP_TITLE_BTN	 	    = 454;
const int UIID_SYSTEM_APP_QR_BTN	 	    = 459;
const int UIID_SYSTEM_APP_COMPLETE_BTN	 	= 457;
const int UIID_SYSTEM_APP_TITLE_TEXT	 	= 455;


/*
 * Text ID
 */
const int UIID_TEXTID_BT_CLOSE = 38;
const int UIID_TEXTID_BT_OPEN = 25;

/*
* CANBUS PASSWORD
*/
const int UIID_CANBUS_PASSWORD_INPUT = 1804;
const int UIID_CANBUS_PASSWORD_DEL = 1805;
const int UIID_CANBUS_PASSWORD_ENTER = 1821;
const int UIID_CANBUS_PASSWORD_ONE = 1811;
const int UIID_CANBUS_PASSWORD_TWO = 1812;
const int UIID_CANBUS_PASSWORD_THREE = 1813;
const int UIID_CANBUS_PASSWORD_FOUR = 1814;
const int UIID_CANBUS_PASSWORD_FIVE = 1815;
const int UIID_CANBUS_PASSWORD_SIX = 1816;
const int UIID_CANBUS_PASSWORD_SEVEN = 1817;
const int UIID_CANBUS_PASSWORD_EIGHT = 1818;
const int UIID_CANBUS_PASSWORD_NINE = 1819;
const int UIID_CANBUS_PASSWORD_ZERO = 1820;


/*
* CANBUS SETTINGS
*/
const int UIID_CANBUS_SETTINGS_BUTTON_NOCAN = 1908;
const int UIID_CANBUS_SETTINGS_BUTTON_HIWORLD = 1909;
const int UIID_CANBUS_SETTINGS_BUTTON_XFY = 1910;
const int UIID_CANBUS_SETTINGS_BUTTON_OUDI = 1911;
const int UIID_CANBUS_SETTINGS_BUTTON_SIMPLE = 1912;
const int UIID_CANBUS_SETTINGS_BUTTON_BINARY = 1913;
const int UIID_CANBUS_SETTINGS_BUTTON_NISSAN = 1914;
const int UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI = 1915;
const int UIID_CANBUS_SETTINGS_BUTTON_KIA = 1916;
const int UIID_CANBUS_SETTINGS_BUTTON_HONDA = 1917;
const int UIID_CANBUS_SETTINGS_BUTTON_FORD = 1918;
const int UIID_CANBUS_SETTINGS_BUTTON_VW = 1919;
const int UIID_CANBUS_SETTINGS_BUTTON_JEEP = 1920;
const int UIID_CANBUS_SETTINGS_BUTTON_FLAT = 1921;
const int UIID_CANBUS_SETTINGS_BUTTON_206 = 1922;
const int UIID_CANBUS_SETTINGS_BUTTON_NEXT = 1923;
const int UIID_CANBUS_SETTINGS_EDIT_WDN = 1925;
const int UIID_CANBUS_SETTINGS_SELECT_ONE = 1926;
const int UIID_CANBUS_SETTINGS_SELECT_TWO = 1927;

/*
* CANBUS POP
*/
const int UIID_CANBUS_POP_BUTTON_CANCLE = 2004;
const int UIID_CANBUS_POP_BUTTON_OK = 2005;

const int UIID_DTS_BUTTON_SWITCH = 1723;
const int UIID_DTS_BUTTON_MODE1 = 1725;
const int UIID_DTS_BUTTON_MODE2 = 1726;
const int UIID_DTS_BUTTON_MODE3 = 1727;

const int UIID_ROADSIDEPARK_BUTTON_SWITCH = 2103;
const int UIID_STORAGEPARK_BUTTON_SWITCH = 2104;
const int UIID_RADERSOUND_BUTTON_SWITCH = 2105;
const int UIID_DEMO_MODE_SWITCH = 2016;

const int UIID_PICID_DTS_SWITCH_OFF = 131;
const int UIID_PICID_DTS_SWITCH_ON = 132;

#endif /* FACTORY_UIID_DEF_H_ */
