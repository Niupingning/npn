/*
 * AssertDefine.h
 *
 *  Created on: 2021-08-19
 *      Author: Y3553
 */

#ifndef ASSERTDEFINE_H_
#define ASSERTDEFINE_H_

//#include "fyassert.h"

enum Home_Assert_Code{
	HAC_GEAR_INFO = 1, //Invalid gear info ,will no display
	HAC_TRIP_CHANGEGROUP = 2,//the next display group;
	HAV_PPS_OBJ_DEL = 3,//the pps PPS_OBJECT_DELETED
	HAV_EXITBYNULLPTR = 4,//the get view is nullptr more than 3 times/sec;
	HAV_PPS_OPENERROR = 5,//the PPS Open Error
	HAV_PPS_INITERROR = 6,//the pps init Error
	HAV_PPS_REOPENERROR = 7,//the PPS Open Error
	HAV_PPS_REINITERROR = 8,//the pps init Error
	HAV_PPS_GUIPTRNULL = 9,//the GUI ptr null
	HAV_SYS_INFO_OPENERROR = 10,//SYS_INFO_DEVICE open fail
	HAV_SYS_INFO_READERROR = 11,//SYS_INFO_DEVICE read fail
	HAV_MEMORY_LINKMODE_INFO = 12, //linkMode info
	HAV_MEMORY_DRIVINGMODE_INFO = 13, //DrivingMode info
	HAV_MEMORY_UIMODE_INFO = 14, //UI style mode
	HAV_MAIN_INPUTARGC_ERROR = 15, //Input Param
	HAV_MAINVIEW_INIT_ERROR = 16, // View Init fail
	HAV_MAINCONTR_INIT_ERROR = 17, // controller Init fail
	HAV_MAINCONTR_START_ERROR = 18,// controller start fail
	HAV_MAINSTARTEVENTLOOP_ERROR = 19,//StartEventLoop fail
	HAV_MAINCONTR_STOP_ERROR = 20, // controller stop fail
	HAV_MAINCONTR_CLOSE_ERROR = 21,// controller close fail
	HAV_MAINVIEW_CLOSE_ERROR = 22, // View close fail
	HAV_MAIN_GEARPPS_OPEN_ERROR = 23, //main open gear pps fail
	HAV_MAIN_GEARPPS_READ_ERROR = 24, //main read gear pps fail
	HAV_MAIN_DECODER_PUSH_ERROR = 25, //pps decoder push error
	HAV_PPS_DECODER_INIT_ERROR = 26, //pps decoder init error
	HAV_PPS_DECODER_STATUS_ERROR = 27, //pps decoder status error
	HAV_PPS_DECODER_JSON_ERROR = 28, //pps_decoder_parse_json_str error
	HAV_MAIN_GNESTATUSPPS_OPEN_ERROR = 29, //main gen_status open fail
	HAV_MAIN_GNESTATUSPPS_READ_ERROR = 30,//main gen_status read fail
	HAV_PPS_INPUTPARAM_NULL = 31, //pps input param is null
	HAV_VIEWCONTROLLER_POINT_NULL = 32,//ViewController point is null
	HAV_RADAR_CONFIG = 33, //car config(radar);
	HAV_CRUISE_CONFIG = 34,//car config(Cruise control);
	HAV_TPMS_CONFIG = 35,//car config(Tpms);
	HAV_NAVI_CONFIG = 36,//car config(navigator);
	HAV_USEMESSAGE_NULL = 37,//usermessage input is null
	HAV_ONAPPMESSAGE_NULL = 38,//OnApp Message input is null
	HAV_MENU_LIST_POINT_NULL = 39,// Menu list point is null
	HAV_SET_LIST_POINT_NULL = 40,// setting list point is null
	HAV_VERSION_LIST_POINT_NULL = 41, // Version list point is null
	HAV_VIEWBASE_POINT_NULL = 42, //GetViewBase is null poin
	HAC_SSTOI_FAIL = 43, // home Assert code sofetystoi fail,more info see param:121-125
	HAC_STRCMP_FAIL = 44,//home Assert code safety strcmp fail,
	HAC_PPS_WRITE_ERROR = 45, //write pps error
	HAC_UPDATEANIMATION_PARAM_ERROR = 46, //update animation input param error;
	HAC_ADJUSTTEXT_INPUTPARAM_INVALID = 47, //adjust text position input param invalid;
	HAC_GET_WINDOW_ERROR = 48, //get window error;
	HAC_ONCOMPNENT_INPUTPARAM_NULL = 49,//OnComponent input param is null;
	HAC_WRITE_MEMORY_MENU = 50, //write memory menu group;
	HAC_READ_MEMORY_MENU = 51, //get memory menu group;
	HAC_PPS_GRAPH_STATUS_NULL = 52, //graph/status is null
	HAC_GEAR_DIGITAL_INFO = 53, // Gear Digital info
	HAC_ANIMATION_START = 54, // start design animation
	HAC_ANIMATION_FILISH = 55, //design animation finish
	HAC_WND_CHANGE_START = 56, //window change start
	HAC_WND_CHANGE_FILISH = 57, //window change finish
	HAC_STANDBY_ADASMIL = 58,  //standby window ADAS Mile
	HAC_STANDBY_ADASTOTALMIL = 59, //standby window ADAS Total Mile
	HAC_STANDBY_ADASRISKTIMES = 60, //standby window ADAS risk avoid times
	HAC_STANDBY_ADASRISKTOTALTIMES = 61, //standby window ADAS risk avoid total times
	HAC_IS_WORK_TO_STANDBY = 62, //whether to enter the standby window from the work window


};

enum Home_Assert_Param{
	HAP_0001,
	HAP_0002,
	HAP_0003,
	HAP_0004,
	HAP_0005,
	HAP_0006,
	HAP_0007,
	HAP_0008,
	HAP_0009,
	HAP_0010,
	HAP_0011,
	HAP_0012,
	HAP_0013,
	HAP_0014,
	HAP_0015,
	HAP_0016,
	HAP_0017,
	HAP_0018,
	HAP_0019,
	HAP_0020,
	HAP_0021,
	HAP_0022,
	HAP_0023,
	HAP_0024,
	HAP_0025,
	HAP_0026,
	HAP_0027,
	HAP_0028,
	HAP_0029,
	HAP_0030,
	HAP_0031,
	HAP_0032,
	HAP_0033,
	HAP_0034,
	HAP_0035,
	HAP_0036,
	HAP_0037,
	HAP_0038,
	HAP_0039,
	HAP_0040,
	HAP_0041,
	HAP_0042,
	HAP_0043,
	HAP_0044,
	HAP_0045,
	HAP_0046,
	HAP_0047,
	HAP_0048,
	HAP_0049,
	HAP_0050,
	HAP_0051,
	HAP_0052,
	HAP_0053,
	HAP_0054,
	HAP_0055,
	HAP_0056,
	HAP_0057,
	HAP_0058,
	HAP_0059,
	HAP_0060,
	HAP_0061,
	HAP_0062,
	HAP_0063,
	HAP_0064,
	HAP_0065,
	HAP_0066,
	HAP_0067,
	HAP_0068,
	HAP_0069,
	HAP_0070,
	HAP_0071,
	HAP_0072,
	HAP_0073,
	HAP_0074,
	HAP_0075,
	HAP_0076,
	HAP_0077,
	HAP_0078,
	HAP_0079,
	HAP_0080,
	HAP_0081,
	HAP_0082,
	HAP_0083,
	HAP_0084,
	HAP_0085,
	HAP_0086,
	HAP_0087,
	HAP_0088,
	HAP_0089,
	HAP_0090,
	HAP_0091,
	HAP_0092,
	HAP_0093,
	HAP_0094,
	HAP_0095,
	HAP_0096,
	HAP_0097,
	HAP_0098,
	HAP_0099,
	HAP_0100,
	HAP_0101,
	HAP_0102,
	HAP_0103,
	HAP_0104,
	HAP_0105,
	HAP_0106,
	HAP_0107,
	HAP_0108,
	HAP_0109,
	HAP_0110,
	HAP_0111,
	HAP_0112,
	HAP_0113,
	HAP_0114,
	HAP_0115,
	HAP_0116,
	HAP_0117,
	HAP_0118,
	HAP_0119,
	HAP_0120,
	HAP_0121,
	HAP_0122,//in stoi imput param error;
	HAP_0123,//in stoi input param range error;
	HAP_0124,//in stoi create temp string error;
	HAP_0125,//in stoi unknow error;
	HAP_0126,//in strcmp srcstr == null;
	HAP_0127,//in strcmp dststr == null;
	HAP_0128,//in strcmp unknow error;
};

#endif /* ASSERTDEFINE_H_ */
