/*
 * SettingView.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y
 */

#include "SettingView.h"
#include "UIID_Def_Setting.h"
#include "AssertDefine.h"
#include <locale.h>

// Dependent on external header files
#include <math.h>
#include <iconv.h>
#include <errno.h>
#include <wchar.h>

#include "IVIView.h"

// 初始化静态配置数组和计数
T_CANBOX_PROTO_CONFIG SettingView::m_sCBProto_Configs[MAX_PROTOCOL_CNT] = {0};
int SettingView::g_proto_config_cnt = 0;

// 固定配置（可根据车机实际路径修改）
#define CONFIG_DIR "/media/flash/userdata/"     // 文件系统目录
#define PROTOCOL_DIR "/application/etc/"        // 文件系统目录
#define ID_FILE_PATH CONFIG_DIR "curr_id.txt"   // id文件名称
#define PROTO_FILE_PATH PROTOCOL_DIR "protocol_config.ini"   // id文件名称
#define TMP_FILE_PATH CONFIG_DIR "curr_id.tmp"  // 临时文件（原子写入用）


char g_strCANBoxName[4][128] = {"Hiworld",
								"Raise",
								"SimpleSoft",
								"None"};
                                    
char g_strCarSeries[6][128] = {"BWM",
							 "Alfa Romeo",
							 "Benz",
							 "VW",
                             "Buick",
							 "Toyota"};

char g_strBwmName[5][128] = {"BMW3,E46 1997-2006",
									  "BMW5,E39 1995~2004",
									  "BMWX3,E83 2003-2009",
									  "B-Class,W246 2012-2017,Gen2",
									  "X5 (E53) 1999-2005"};
char g_strAlfaName[10][128] = {"Alfa Romeo 147 2000-2010",
										"Alfa Romeo 159 2004-2011",
										"Giulia(952) 2015-Present",
										"Giulietta (940) | 2010-2013",
										"Giulietta(940) II 2014-Present",
										"MiTo 2008〜2013",
										"MiTo 2014〜2018",
										"Stelvio (949) 2016"
										"STELVIO 2017",
										"Stelvio 2018-~2022"};
char g_strBenzName[10][128] = {"A-Class,W169 2004-2012,Gen2",
										"A-Class,W176 2012-2017,Gen3",
										"B-Class,W245 2005-2011,Gen1",
										"B-Class,W246 2012-2017,Gen2",
										"C-Class,W204 2008-2014,Gen3",
										"C207 E-Class 2009-2015,Gen4",
										"CLK Class,W209 2002-2009. Gen1",
										"GL350,400,450,X164 2005~2013,Gen2",
										"GLK-Class, X204 2008-2015,Gen 1",
										"M-Class,W166 2011-2016,Gen3"};
char g_strVmName[10][128] = {"Passat B8 2016-Present",
									  "Magotan/Passat B8 2016-Present",
									  "Sagitar/Jetta 2019-Present",
									  "Plus Lavida Plus 2018-Present",
									  "Golf MK7 2013-Present",
									  "Golf GTI MK7 2013-Present",
									  "Golf Sportsvan 2016-Present",
									  "Touran 2015-Present",
									  "Tharu 2018-Present",
									  "Tayron 2018-Present"};
char g_strBulckVmName[10][128] = {"21 Excelle 2020-Present",
										   "Encore 2012-2015",
										   "Encore 2012-2015,Gen1",
										   "Encore 2016-2019",
										   "Encore 2016-2019,Gen2",
										   "Regal 2009-2013,Gen5",
										   "Regal 2014~2016,Gen6",
										   "Regal 2017-Present,Gen7",
										   "Verano 2015-Present,Gen 1",
										   "Verano GS 2016-Present,Gen 1"};
char g_strToyotaName[3][128] = {"hiworld 13-21 Corolla",
								"Simple Toyota 2012&2015 Camry",
								"Raise VM PQ All Car Type"};

char g_strProtoName[3][128]= {""};


//    id     供应商hiwold     车系      车型      库名      做文件
//  

using FY_NAMESPACE::APPMSG_DEVICESTATUS;
using FY_NAMESPACE::APPS_STATUS_HIDEONLY;
using FY_NAMESPACE::APPS_STATUS_PAUSE;
using FY_NAMESPACE::APPS_STATUS_QUIT;
using FY_NAMESPACE::APPS_STATUS_RESUME;
using FY_NAMESPACE::APPSMSG_APPDATA;
using FY_NAMESPACE::APPSMSG_APPSTATUS;
using FY_NAMESPACE::APPSMSG_DATETIME;
using FY_NAMESPACE::APPSMSG_TIMER;
using FY_NAMESPACE::CLOSE_FORYOUAPP;
using FY_NAMESPACE::CUIWndMgrWrapper;
using FY_NAMESPACE::LAUNCH_FORYOUAPP;
using std::make_pair;


SettingView::SettingView(ViewBase* p_ViewBase)
	: CompBase(p_ViewBase)
	, m_ViewController(NULL)
//	, m_HandleViewUpdate(this)
	, m_nSetCurrWnd(0)
	, mis_Need(true)
	, m_Mute(false)
	, m_LastMute(false)
	, m_KeyVol(false)
	, m_LastKeyVol(true)
	, m_AutoBg(false)
	, m_LastAutoBg(false)
	, m_LastReadLight(false)
	, m_LastSyncTime(false)
	, m_bTimeflag(false)
	, m_Line(-1)
	, m_LastIndex(1)
	, m_nCanBoxPassWorld("")
	, m_nCanBoxCtrlId(0)
	, m_nCanBoxTwoCtrlId(0)
	, m_nCanBoxThreeCtrlId(0)
{
	// TODO Auto-generated constructor stub
	m_nBtnIdForQuickClick = 0;
	m_nBtnIdForQuickClick_OnComponent = 0;
	InitUpdateMsgMap();
	m_nDateTime_mode = TIMEDATE_DD_MM_YY;

	//加载CanBox协议库配置文件 protocol_config.ini
	printf("loding canbox protocol protocol_config.ini \n");
	char iniStr[128];
	strncpy(iniStr, PROTO_FILE_PATH, sizeof(iniStr) - 1);
	iniStr[sizeof(iniStr) - 1] = '\0'; // 确保字符串终止
	LoadProtoConfig(iniStr);

	//获取当前车型ID
	uint16_t curr_id;
	readCurrIdFromTxt(&curr_id); 
	printf("curr_id %d \n",curr_id);

	for(int i=0;i<g_proto_config_cnt;i++){
		//获取当前车型信息
		if(m_sCBProto_Configs[i].id == curr_id ){
			m_sCBProto_Cur.id=m_sCBProto_Configs[i].id;
			// 字符数组用 strncpy 赋值（防越界）
			strncpy(m_sCBProto_Cur.vendor, m_sCBProto_Configs[i].vendor, sizeof(m_sCBProto_Cur.vendor)-1);
			m_sCBProto_Cur.vendor[sizeof(m_sCBProto_Cur.vendor)-1] = '\0'; // 确保字符串终止
			
			strncpy(m_sCBProto_Cur.car_series, m_sCBProto_Configs[i].car_series, sizeof(m_sCBProto_Cur.car_series)-1);
			m_sCBProto_Cur.car_series[sizeof(m_sCBProto_Cur.car_series)-1] = '\0';
			
			strncpy(m_sCBProto_Cur.car_type, m_sCBProto_Configs[i].car_type, sizeof(m_sCBProto_Cur.car_type)-1);
			m_sCBProto_Cur.car_type[sizeof(m_sCBProto_Cur.car_type)-1] = '\0';
			
			strncpy(m_sCBProto_Cur.so_name, m_sCBProto_Configs[i].so_name, sizeof(m_sCBProto_Cur.so_name)-1);
			m_sCBProto_Cur.so_name[sizeof(m_sCBProto_Cur.so_name)-1] = '\0';

			printf("m_sCBProto_Cur.id[%d],m_sCBProto_Cur.vendor[%s],m_sCBProto_Cur.car_series[%s],m_sCBProto_Cur.car_type[%s],m_sCBProto_Cur.so_name[%s]\n",
					m_sCBProto_Cur.id,
					m_sCBProto_Cur.vendor,
					m_sCBProto_Cur.car_series,
					m_sCBProto_Cur.car_type,
					m_sCBProto_Cur.so_name);
			break;
		}
	}

	// 获取协议供应商
	int HiNu = 0, SiNu = 0, RaNu = 0;
	memset(g_strCANBoxName[0], 0x00, 128);
	memset(g_strCANBoxName[1], 0x00, 128);
	memset(g_strCANBoxName[2], 0x00, 128);
	strcpy(g_strCANBoxName[3], "None");
	for(int i = 0; i < g_proto_config_cnt; i++) {
		if (m_sCBProto_Configs[i].vendor != nullptr && strcmp(m_sCBProto_Configs[i].vendor, "Hiworld") == 0 && HiNu == 0) 
		{
			HiNu = 1;
			strncpy(g_strCANBoxName[0], m_sCBProto_Configs[i].vendor, 127);
			g_strCANBoxName[0][127] = '\0'; 
		}
		if (m_sCBProto_Configs[i].vendor != nullptr && strcmp(m_sCBProto_Configs[i].vendor, "Raise") == 0 && RaNu == 0) 
		{
			RaNu = 1;
			strncpy(g_strCANBoxName[1], m_sCBProto_Configs[i].vendor, 127);
			g_strCANBoxName[1][127] = '\0';
		}
		if (m_sCBProto_Configs[i].vendor != nullptr && strcmp(m_sCBProto_Configs[i].vendor, "SimpleSoft") == 0 && SiNu == 0) 
		{
			SiNu = 1;
			strncpy(g_strCANBoxName[2], m_sCBProto_Configs[i].vendor, 127);
			g_strCANBoxName[2][127] = '\0';
		}

	}


}

SettingView::~SettingView()
{
	// TODO Auto-generated destructor stub
}

void SettingView::InitUpdateMsgMap()
{
	m_MsgMap.clear();
	m_MsgMap.insert(make_pair(APP_VMSG_SET_SYSTEM, &SettingView::SetSystemInfo));

	m_MsgMap.insert(make_pair(SYS_VMSG_UPDATE_VOLUME_VALUE, &SettingView::UpdateVolumeBar));
	m_MsgMap.insert(make_pair(SYS_VMSG_UPDATE_BACKLIGHT_LEVEL, &SettingView::UpdateBackLightBar));
}
//=======================加载车型车系相关 start============================
// 加载车型配置文件
int SettingView::LoadProtoConfig(const char* ini_path) {
    // 初始化
    memset(m_sCBProto_Configs, 0, sizeof(m_sCBProto_Configs));
    g_proto_config_cnt = 0; 
    ParseContext ctx = {0};
    // 打印绝对路径，确认文件位置（车机中INI文件通常在/etc/或当前目录）
    LOGDBG("ini_path: %s\n", ini_path);

    // 调用inih库解析文件
    if (ini_parse(ini_path, IniParseHandler, &ctx) < 0) {
        printf("解析配置文件失败: %s\n", ini_path);
        return -1;
    }

    LOGDBG("解析完成！共加载 %d 个协议配置\n", g_proto_config_cnt);
    if (g_proto_config_cnt > 0) {
        // 改用英文表头（避免中文乱码），或确保系统编码为GBK
        LOGDBG("----------------------------------------\n");
        LOGDBG("ID\tVendor\t\tCarSeries\tCarType\t\t\tSOName\n");
        LOGDBG("----------------------------------------\n");
        for (int i = 0; i < g_proto_config_cnt; i++) {
            T_CANBOX_PROTO_CONFIG* cfg = &m_sCBProto_Configs[i];
            // 格式化打印：用%s确保空字符串不打印乱码，用\t对齐
            LOGDBG("%d\t%s\t\t%s\t\t%s\t\t%s\n",
                   cfg->id,
                   cfg->vendor[0] ? cfg->vendor : "", // 空值显示空字符串
                   cfg->car_series[0] ? cfg->car_series : "",
                   cfg->car_type[0] ? cfg->car_type : "",
                   cfg->so_name[0] ? cfg->so_name : "");
        }
        LOGDBG("----------------------------------------\n");
    } else {
        LOGDBG("警告：未加载到任何协议配置！\n");
    }
    return 0;
}

// INI解析回调：逐行解析，存入结构体数组
int SettingView::IniParseHandler(void* user, const char* section, const char* name, const char* value) {
    ParseContext* ctx = (ParseContext*)user;
    T_CANBOX_PROTO_CONFIG* cfg = NULL;

    LOGDBG("Strat In.\n");
    // 空值保护
    if (!section || !name || name[0] == '\0') { // 纯段名行直接返回（不再依赖段名行创建配置项）
        return 1;
    }
    LOGDBG("In1. section=%s, name=%s, value=%s\n", section, name, value ? value : "NULL");

    uint16_t current_id = 0;
    // 解析section中的十进制ID（[1]/[2]）
    if (isdigit(section[0])) {
        current_id = (uint16_t)atoi(section);
    }
    // 解析十六进制ID（兼容[0x01]）
    else if (strstr(section, "0x") == section) {
        current_id = (uint16_t)strtol(section, NULL, 16);
    }

    // 如果ID无效，直接返回
    if (current_id == 0) {
        LOGDBG("无效ID：%s\n", section);
        return 1;
    }

    int cfg_index = -1;
    // 1. 先查找是否已有该ID的配置项（避免重复创建）
    for (int i = 0; i < g_proto_config_cnt; i++) {
        if (m_sCBProto_Configs[i].id == current_id) {
            cfg_index = i;
            break;
        }
    }
    // 2. 没有则创建新配置项
    if (cfg_index == -1) {
        if (g_proto_config_cnt >= MAX_PROTOCOL_CNT) {
            LOGDBG("配置项数量超限！\n");
            return 1;
        }
        cfg_index = g_proto_config_cnt;
        cfg = &m_sCBProto_Configs[cfg_index];
        memset(cfg, 0, sizeof(T_CANBOX_PROTO_CONFIG));
        cfg->id = current_id;
        g_proto_config_cnt++;
        LOGDBG("创建配置项 ID=%d，索引=%d\n", current_id, cfg_index);
    }
    // 3. 指向当前ID的配置项
    cfg = &m_sCBProto_Configs[cfg_index];

    // ========== 字段赋值逻辑 ==========
    LOGDBG("In3. 字段行\n");
    // 清理name首尾空格
    char clean_name[64] = {0};
    strncpy(clean_name, name, sizeof(clean_name)-1);
    char* start = clean_name;
    while (*start == ' ' || *start == '\t') start++;
    char* end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t')) end--;
    *(end+1) = '\0';
    LOGDBG("In4. 清理后name=%s\n", start);

    // 字段赋值（确保value不为空）
    if (value == NULL) value = "";
    LOGDBG("In5. 给ID=%d 赋值，value=%s\n", cfg->id, value);

    if (strcmp(start, "vendor") == 0) {
        strncpy(cfg->vendor, value, sizeof(cfg->vendor) - 1);
        cfg->vendor[sizeof(cfg->vendor) - 1] = '\0';
        LOGDBG("vendor赋值完成：%s\n", cfg->vendor);
    } else if (strcmp(start, "car_series") == 0) {
        strncpy(cfg->car_series, value, sizeof(cfg->car_series) - 1);
        cfg->car_series[sizeof(cfg->car_series) - 1] = '\0';
    } else if (strcmp(start, "car_type") == 0) {
        strncpy(cfg->car_type, value, sizeof(cfg->car_type) - 1);
        cfg->car_type[sizeof(cfg->car_type) - 1] = '\0';
    } else if (strcmp(start, "so_name") == 0) {
        strncpy(cfg->so_name, value, sizeof(cfg->so_name) - 1);
        cfg->so_name[sizeof(cfg->so_name) - 1] = '\0';
    } else {
        LOGDBG(" 未知字段：%s\n", start);
    }

    return 1;
}
//=======================加载车型车系相关 end============================


//======================加载当前车型ID文件相关 start=========================

/**
 * @brief 工具函数：确保目录存在（不存在则创建）
 * @return 0-成功，-1-失败
 */
int SettingView::ensureDirExists() {
    struct stat dir_stat;
    if (stat(CONFIG_DIR, &dir_stat) != 0) {
        char cmd[128] = {0};
        snprintf(cmd, sizeof(cmd), "mkdir -p %s", CONFIG_DIR);
        int ret = system(cmd);
        if (ret != 0) {
            LOGDBG("创建目录失败：%s，错误码：%d\n", CONFIG_DIR, ret);
            return -1;
        }
    } else if (!S_ISDIR(dir_stat.st_mode)) {
        LOGDBG("错误：%s 不是目录！\n", CONFIG_DIR);
        return -1;
    }
    return 0;
}

/**
 * @brief 纯文本写入ID（先删旧文件，再原子创建新文件）
 * @param id 要存储的16位无符号ID
 * @return 0-成功，-1-目录创建失败，-2-临时文件写入失败，-3-原子替换失败
 */
int SettingView::writeCurrIdToTxt(uint16_t id) {
    if (ensureDirExists() != 0) {
        return -1;
    }
    if (access(ID_FILE_PATH, F_OK) == 0) { 
        if (remove(ID_FILE_PATH) == 0) {
            LOGDBG("成功删除旧文件 delete old mikd：%s\n", ID_FILE_PATH);
        } else {
            perror("删除旧文件失败 delete fial.");
            LOGDBG("警告：删除旧文件失败，但继续尝试写入新文件\n");
        }
    }
    // 2. 写入临时文件（原子操作核心，避免断电损坏）
    FILE* tmp_fp = fopen(TMP_FILE_PATH, "w");
    if (tmp_fp == nullptr) {
        LOGDBG("打开临时文件失败 Open temp file fial：%s\n", TMP_FILE_PATH);
        return -2;
    }
    int write_ret = fprintf(tmp_fp, "%d", id);
    fclose(tmp_fp);
    // 3. 校验临时文件写入结果
    if (write_ret < 0) {
        LOGDBG("写入临时文件失败！write temp file fial !\n");
        remove(TMP_FILE_PATH); // 删除无效临时文件
        return -2;
    }
    // 4. 原子替换创建新的curr_id.txt
    if (rename(TMP_FILE_PATH, ID_FILE_PATH) != 0) {
        perror("替换创建新文件失败");
        remove(TMP_FILE_PATH);
        return -3;
    }

    LOGDBG("纯文本写入ID成功(write id success!)：%s -> %d（已先删除旧文件）\n", ID_FILE_PATH, id);
    return 0;
}

/**
 * @brief 纯文本读取ID（仅解析数字）
 * @param out_id 输出参数：读取到的ID
 * @return 0-成功，-1-参数无效，-2-文件不存在/打开失败，-3-解析失败
 */
int SettingView::readCurrIdFromTxt(uint16_t* out_id) {
	LOGDBG("read curr car type id ");
    if (out_id == nullptr) {
        LOGDBG("错误：out_id为空指针！ out_id null\n");
        return -1;
    }

    *out_id = 0; // 初始化避免脏数据
    FILE* fp = fopen(ID_FILE_PATH, "r");
    if (fp == nullptr) {
        LOGDBG("文件不存在或打开失败（no file or open fail）：%s\n", ID_FILE_PATH);
        return -2;
    }
    int read_ret = fscanf(fp, "%hu", out_id); // %hu 匹配uint16_t
    fclose(fp);
    if (read_ret != 1) {
        LOGDBG("ID解析失败：文件内容非有效数字！（id parse fail ）\n");
        return -3;
    }
    LOGDBG("纯文本读取ID成功（read success!）：%s -> %d\n", ID_FILE_PATH, *out_id);
    return 0;
}

//======================加载当前车型ID文件相关 endl=========================


//
void SettingView::GetUpdateMessageMap(map<int, pUpdateSettingMsg> &nMap)
{
	nMap = m_MsgMap;
}

bool SettingView::SettingOnUserMessage(T_APP_MSG *msg)
{
	if (nullptr == msg)
	{
		return false;
	}
	
	LOGDBG("jgwen--- Message = %d,param0=%d,param1=%d,param2=%d\n", \
			msg->Message, msg->Param0,msg->Param1,msg->Param2);

	map<int, pUpdateSettingMsg> nUpdateMap;
	nUpdateMap.clear();
	GetUpdateMessageMap(nUpdateMap);
	auto Itr = nUpdateMap.find(msg->Message);
	if (Itr != nUpdateMap.end())
	{
		(this->*(Itr->second))(msg);
	}
	else
	{
		switch(msg->Message)
		{
			case SYS_VMSG_UPDATE_BACKGROUND:
			case SYS_VMSG_UPDATE_READLIGHT:
			{
				SetbackLightMode(msg->Param0, m_LastAutoBg);  // 大灯控制背光
			}
			break;
			case SYS_VMSG_UPDATE_LANGUAGE: // 语言
			{
				//
			}
			break;
			case SYS_VMSG_UPDATE_DATETIME:  // 时间
			{
				T_STR_TIME_INFO tTimeDataInfo;
				SettingData &mData = SettingData::GetInstance();

				mData.GetTimeData(tTimeDataInfo);
				m_tTime = tTimeDataInfo;

				UpateUI_DateTime(tTimeDataInfo);
			}
			break;
			case SYS_VMSG_UPDATE_VERSION:  // 版本
			{
				//更新版本
				UpateUI_Version();
			}
			break;
			default:
				break;
		}		
	}
	return true;
}


void SettingView::SettingOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
	LOGDBG("jgwen--- nWndId=%d,nCtrlId=%d,nBtState=%d",nWndId,nCtrlId,nBtState);
	switch(nCtrlId)
	{
		case UIID_SETTING_LIST:
		{
			if(1 == nBtState)
			{
				//点击列表
				onClick_SettingList();
			}
		}
		break;
		case UIID_SETTING_RETURN:
		{
			if(1 == nBtState)
			{
				//返回
				onClick_ReturnBtn();
			}
		}
		break;
		case UIID_SETTING_CLOSE:
		{
			if(1 == nBtState)
			{
				//关闭
				onClick_CloseView();
			}
		}
		break;
		case UIID_SETTING_SETTING:
		{
			if(1 == nBtState)
			{
				onClick_SettingBtn();
			}
		}
		break;
		case UIID_SETTING_AUDIO:
		{
			if(1 == nBtState)
			{
				onClick_AudioBtn();
			}
		}
		break;
		case UIID_SETTING_CAR:
		{
			if(1 == nBtState)
			{
				onClick_CarBtn();
			}
		}
		break;
		case UIID_SETTING_REST_OK:
		{
			//复位ok
			if(1 == nBtState)
			{
				onClick_RestOKBtn();
			}
		}
		break;
		case UIID_SETTING_REST_CANCEL:
		{
			//复位cancel
			if(1 == nBtState)
			{
				onClick_RestCancelBtn();
			}
		}
		break;
		case UIID_SETTING_DATETIME_DATE_UP:
		case UIID_SETTING_DATETIME_DATE_DN:
		case UIID_SETTING_DATETIME_MONTH_UP:
		case UIID_SETTING_DATETIME_MONTH_DN:
		case UIID_SETTING_DATETIME_YEAR_UP:
		case UIID_SETTING_DATETIME_YEAR_DN:
		case UIID_SETTING_DATETIME_HOURS_UP:
		case UIID_SETTING_DATETIME_HOURS_DN:
		case UIID_SETTING_DATETIME_MIN_UP:
		case UIID_SETTING_DATETIME_MIN_DN:
		case UIID_SETTING_DATETIME_MODE_UP:
		case UIID_SETTING_DATETIME_MODE_DN:
		case UIID_SETTING_DATETIME_DDMM:
		case UIID_SETTING_DATETIME_MMDD:
		{
			if(1 == nBtState)
			{
				onClick_DateTimeBtn(nCtrlId);
			}
		}
		break;
		case UIID_SETTING_BRIGHT_LEFT:
		{
			if(1 == nBtState)
			{
				onClick_BrightValue(0);
			}
		}
		break;
		case UIID_SETTING_BRIGHT_RIGHT:
		{
			if(1 == nBtState)
			{
				onClick_BrightValue(1);
			}
		}
		break;
		case UIID_SETTING_AUDIO_MAIN_SUB:
		{
			if(1 == nBtState)
			{
				onClick_MainVolume(0);
			}
		}
		break;
		case UIID_SETTING_AUDIO_MAIN_ADD:
		{
			if(1 == nBtState)
			{
				onClick_MainVolume(1);
			}
		}
		break;
		case UIID_SETTING_AUDIO_BT_SUB:
		{
			if(1 == nBtState)
			{
				onClick_BtVolume(0);
			}
		}
		break;
		case UIID_SETTING_AUDIO_BT_ADD:
		{
			if(1 == nBtState)
			{
				onClick_BtVolume(1);
			}
		}
		break;
		case UIID_DTS_BUTTON_SWITCH:
		{
			if(1 == nBtState)
			{
				onClick_DtsSwitch();
			}
		}
		break;
		case UIID_ROADSIDEPARK_BUTTON_SWITCH:
		{
			if(1 == nBtState)
			{
				onClick_RoadsideParkSwitch();
			}
		}
		break;
		case UIID_STORAGEPARK_BUTTON_SWITCH:
		{
			if(1 == nBtState)
			{
				onClick_StorageParkSwitch();
			}
		}	
		break;
		case UIID_RADERSOUND_BUTTON_SWITCH:
		{
			if(1 == nBtState)
			{
				onClick_RaderSoundSwitch();
			}
		}			
		break;
		case UIID_SETTING_CANBOX_CAR_LIST:
		{
			if(1 == nBtState)
			{
				onClick_CanBoxList();
			}
		}
		break;
		case UIID_SETTING_CANBOX_CONNECT:
		{
			if(1 == nBtState)
			{
				onClick_CanBoxConnect();
			}
		}
		break;
		case UIID_DEMO_MODE_SWITCH:
		{
			if(1 == nBtState)
			{
				SettingData &mData = SettingData::GetInstance();

				bool bDemoMode = false;
				bDemoMode = mData.GetDemoMode();

				if(bDemoMode)
				{
					bDemoMode = false;
					mData.SetDemoMode(false);
					((IVIView*)GetViewBase())->OnHide_DemoView(false);
				}
				else
				{
					bDemoMode = true;
					mData.SetDemoMode(true);
					((IVIView*)GetViewBase())->OnHide_DemoView(true);
				}

				OnUpdate_DemoModeSwitch(bDemoMode);
			}
		}
		break;
		default:
		{
			if(1 == nBtState)
			{
				//CanBoxOnClick(pUI, nWndId, nCtrlId, nBtState);
			}
		}
		break;
	}
}

void SettingView::onClick_CanBoxList()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));
	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_SETTING,UIID_SETTING_CANBOX_CAR_LIST,UItem);
	CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;

	if(NULL == pList)
	{
		return;
	}

	int nLine = 0;
	int ItemData = 0;
	int nCol = 0;

	pList->GetCurSel(nLine,ItemData,nCol);
	LOGDBG("SettingView::onClick_CanBoxList,nLine=%d,ItemData=%d,nCol=%d \n",nLine,ItemData,nCol);
	if(1 == m_nCanBox_Level)
	{
		m_nCanBox_FirstLine=nLine; // 记下一级列表点击的
		m_nCanBox_Level = 2;
		m_strCanBox_Name = g_strCANBoxName[nLine];
		OnInList_CanboxData(m_nCanBox_Level,nLine);
	}
	else if(2 == m_nCanBox_Level)
	{
		m_nCanBox_Level = 3;
		m_strCanBox_CarType = g_strCarSeries[nLine];
	
		OnInList_CanboxData(m_nCanBox_Level,nLine,ItemData);
	}
	else if(3 == m_nCanBox_Level)
	{

		for(int i=0; i<g_proto_config_cnt; i++) {
			if(m_sCBProto_Configs[i].id == ItemData) {

				wchar_t wszTempBuf[TEXT_BUF_LEN] = {0};

				swprintf(wszTempBuf, TEXT_BUF_LEN, L"%s", m_strCanBox_Name.c_str());
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_CANBOX_NAME, wszTempBuf);

				swprintf(wszTempBuf, TEXT_BUF_LEN, L"%s", m_strCanBox_CarType.c_str());
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_CANBOX_CARTYPE, wszTempBuf);

				swprintf(wszTempBuf, TEXT_BUF_LEN, L"%s", m_sCBProto_Configs[i].car_type);
				pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_CANBOX_PRO, wszTempBuf);

				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CONNECT,true);
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CAR,false);

				break; // ID唯一，找到后直接退出循环
			}
		}
		



		// if(0 == strcmp("BWM",m_strCanBox_CarType.c_str()))
		// {
		// 	m_strCanBox_CarProctol = g_strBwmName[nLine];
		// }
		// else if(0 == strcmp("Alfa Romeo",m_strCanBox_CarType.c_str()))
		// {
		// 	m_strCanBox_CarProctol = g_strAlfaName[nLine];
		// }
		// else if(0 == strcmp("Benz",m_strCanBox_CarType.c_str()))
		// {
		// 	m_strCanBox_CarProctol = g_strBenzName[nLine];
		// }
		// else if(0 == strcmp("VW",m_strCanBox_CarType.c_str()))
		// {
		// 	m_strCanBox_CarProctol = g_strVmName[nLine];
		// }
		// else if(0 == strcmp("Buick",m_strCanBox_CarType.c_str()))
		// {
		// 	m_strCanBox_CarProctol = g_strBulckVmName[nLine];
		// }else if(0==strcmp("Toyota",m_strCanBox_CarType.c_str())){
		// 	m_strCanBox_CarProctol = g_strToyotaName[nLine];
		// }

		//update
		// wchar_t wszTempBuf[TEXT_BUF_LEN] = {0};

		// swprintf(wszTempBuf, TEXT_BUF_LEN, L"%s", m_strCanBox_Name.c_str());
		// pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_CANBOX_NAME, wszTempBuf);

		// swprintf(wszTempBuf, TEXT_BUF_LEN, L"%s", m_strCanBox_CarType.c_str());
		// pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_CANBOX_CARTYPE, wszTempBuf);

		// swprintf(wszTempBuf, TEXT_BUF_LEN, L"%s", m_strCanBox_CarProctol.c_str());
		// pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_CANBOX_PRO, wszTempBuf);

		// pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CONNECT,true);
		// pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CAR,false);
	}
}

// 选择车型后点击Connection按钮 事件
void SettingView::onClick_CanBoxConnect()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));
	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_SETTING,UIID_SETTING_CANBOX_CAR_LIST,UItem);
	CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;

	if(NULL == pList)
	{
		return;
	}

	int nLine = 0;
	int ItemData = 0;
	int nCol = 0;

	pList->GetCurSel(nLine,ItemData,nCol);
	int car_id=pList->GetItemData(nLine);




	LOGDBG("SettingView::onClick_CanBoxConnect car_id=%d,\n",car_id);
	if((1==car_id) || (2==car_id) || (3== car_id) )
	{

		writeCurrIdToTxt(car_id); // 写入车型ID
		//发给MCU重启
		if(NULL != m_ViewController  )
		{
			m_ViewController->Mpu_McuRest();
		}		
	}
	else
	{
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CONNECT,false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CAR,true);
	}


}

void SettingView::CanBoxOnClick(CUIWndMng* pUI, long nWndId, long nCtrlId, int nBtState)
{
    LOGDBG("CanBoxOnClick--- nWndId=%d,nCtrlId=%d,nBtState=%d",nWndId,nCtrlId,nBtState);
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	switch(nCtrlId)
	{
		case UIID_CANBUS_PASSWORD_ONE:
		case UIID_CANBUS_PASSWORD_TWO:
		case UIID_CANBUS_PASSWORD_THREE:
		case UIID_CANBUS_PASSWORD_FOUR:
		case UIID_CANBUS_PASSWORD_FIVE:
		case UIID_CANBUS_PASSWORD_SIX:
		case UIID_CANBUS_PASSWORD_SEVEN:
		case UIID_CANBUS_PASSWORD_EIGHT:
		case UIID_CANBUS_PASSWORD_NINE:
		case UIID_CANBUS_PASSWORD_ZERO:
		{
            std::string keyValue = GetPassWorldKey(nCtrlId);
			LOGERR("CanBoxOnClick  m_nCanBoxPassWorld = %s keyValue = %s", m_nCanBoxPassWorld.c_str(), keyValue.c_str());
            m_nCanBoxPassWorld+=keyValue;
			std::string encodePassWorld = GetEncodePassWorld(m_nCanBoxPassWorld);
			wchar_t wszBufPassWorld[TEXT_BUF_LEN] = {0};
			swprintf(wszBufPassWorld, TEXT_BUF_LEN, L"%s", encodePassWorld.c_str());
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_PASSWORD_INPUT, wszBufPassWorld);
		}
		break;
	    case UIID_CANBUS_PASSWORD_DEL:
		{
			m_nCanBoxPassWorld = "";
			wchar_t wszBufPassWorld[TEXT_BUF_LEN] = {0};
			swprintf(wszBufPassWorld, TEXT_BUF_LEN, L"%s", m_nCanBoxPassWorld.c_str());
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_PASSWORD_INPUT, wszBufPassWorld);
		}
		break;
		case UIID_CANBUS_PASSWORD_ENTER:
		{
			m_nCanBoxPassWorld = "";
			wchar_t wszBufPassWorld[TEXT_BUF_LEN] = {0};
			swprintf(wszBufPassWorld, TEXT_BUF_LEN, L"%s", m_nCanBoxPassWorld.c_str());
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_PASSWORD_INPUT, wszBufPassWorld);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_KIA, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HONDA, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FORD, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_VW, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_JEEP, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FLAT, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_206, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NEXT, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_EDIT_WDN, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_CANBUS_PASSWORD, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_CANBUS_SETTINGS, true);
			m_nCanBoxCtrlId = 0;
		}
		break;
		case UIID_CANBUS_SETTINGS_BUTTON_HIWORLD:
		{
			wchar_t wszNissan[TEXT_BUF_LEN] = {0};
			swprintf(wszNissan, TEXT_BUF_LEN, L"%s", "Nissan");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, wszNissan);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_KIA, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HONDA, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FORD, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_VW, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_JEEP, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FLAT, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_SELECT_ONE, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_SELECT_TWO, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_206, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NEXT, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_EDIT_WDN, false);
			SetOneListCurClickId(nCtrlId);
		}
		break;
		case UIID_CANBUS_SETTINGS_BUTTON_NOCAN:
		{
			wchar_t wszBufDefualt[TEXT_BUF_LEN] = {0};
			swprintf(wszBufDefualt, TEXT_BUF_LEN, L"%s", "Defualt");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, wszBufDefualt);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_KIA, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HONDA, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FORD, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_VW, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_JEEP, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FLAT, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_206, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NEXT, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_EDIT_WDN, false);
		    SetOneListCurClickId(nCtrlId);
		}
		break;
		case UIID_CANBUS_SETTINGS_BUTTON_XFY:
		{
			wchar_t wszToyota[TEXT_BUF_LEN] = {0};
			swprintf(wszToyota, TEXT_BUF_LEN, L"%s", "Toyota");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, wszToyota);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, true);
			wchar_t wszHonda[TEXT_BUF_LEN] = {0};
			swprintf(wszHonda, TEXT_BUF_LEN, L"%s", "Honda");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI, wszHonda);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI, true);
            wchar_t wszHyundai[TEXT_BUF_LEN] = {0};
			swprintf(wszHyundai, TEXT_BUF_LEN, L"%s", "Hyundai");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_KIA, wszHyundai);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_KIA, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HONDA, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FORD, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_VW, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_JEEP, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FLAT, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_206, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NEXT, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_EDIT_WDN, false);
		    SetOneListCurClickId(nCtrlId);
		}
		break;
		case UIID_CANBUS_SETTINGS_BUTTON_OUDI:
		{
			wchar_t wszBufBmw[TEXT_BUF_LEN] = {0};
			swprintf(wszBufBmw, TEXT_BUF_LEN, L"%s", "BMW");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, wszBufBmw);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, true);
			wchar_t wszBufHonda[TEXT_BUF_LEN] = {0};
			swprintf(wszBufHonda, TEXT_BUF_LEN, L"%s", "Honda");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI, wszBufHonda);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI, true);
			wchar_t wszBufHaval[TEXT_BUF_LEN] = {0};
			swprintf(wszBufHaval, TEXT_BUF_LEN, L"%s", "Haval");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_KIA, wszBufHaval);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_KIA, true);
			wchar_t wszBufBaic[TEXT_BUF_LEN] = {0};
			swprintf(wszBufBaic, TEXT_BUF_LEN, L"%s", "BAIC");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_HONDA, wszBufBaic);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HONDA, true);
			wchar_t wszBufByd[TEXT_BUF_LEN] = {0};
			swprintf(wszBufByd, TEXT_BUF_LEN, L"%s", "BYD");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_FORD, wszBufByd);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FORD, true);
			wchar_t wszBufNissan[TEXT_BUF_LEN] = {0};
			swprintf(wszBufNissan, TEXT_BUF_LEN, L"%s", "Nissan");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_VW, wszBufNissan);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_VW, true);
			wchar_t wszBufSubaru[TEXT_BUF_LEN] = {0};
			swprintf(wszBufSubaru, TEXT_BUF_LEN, L"%s", "SUBARU");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_JEEP, wszBufSubaru);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_JEEP, true);
			wchar_t wszBufPorsche[TEXT_BUF_LEN] = {0};
			swprintf(wszBufPorsche, TEXT_BUF_LEN, L"%s", "Porsche");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_FLAT, wszBufPorsche);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FLAT, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_206, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NEXT, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_EDIT_WDN, false);
		    SetOneListCurClickId(nCtrlId);
		}
		break;	
		case UIID_CANBUS_SETTINGS_BUTTON_SIMPLE:
		{
			wchar_t wszBufPeugeot[TEXT_BUF_LEN] = {0};
			swprintf(wszBufPeugeot, TEXT_BUF_LEN, L"%s", "Peugeot");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, wszBufPeugeot);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, true);
			wchar_t wszBufVm[TEXT_BUF_LEN] = {0};
			swprintf(wszBufVm, TEXT_BUF_LEN, L"%s", "VM (PQ)");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI, wszBufVm);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI, true);
			wchar_t wszBufSkoda[TEXT_BUF_LEN] = {0};
			swprintf(wszBufSkoda, TEXT_BUF_LEN, L"%s", "Skoda");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_KIA, wszBufSkoda);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_KIA, true);
			wchar_t wszBufSeat[TEXT_BUF_LEN] = {0};
			swprintf(wszBufSeat, TEXT_BUF_LEN, L"%s", "Seat");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_HONDA, wszBufSeat);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HONDA, true);
			wchar_t wszBufvMMQB[TEXT_BUF_LEN] = {0};
			swprintf(wszBufvMMQB, TEXT_BUF_LEN, L"%s", "WM (MQB)");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_FORD, wszBufvMMQB);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FORD, true);
			wchar_t wszBufToyota[TEXT_BUF_LEN] = {0};
			swprintf(wszBufToyota, TEXT_BUF_LEN, L"%s", "Toyota");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_VW, wszBufToyota);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_VW, true);
			wchar_t wszBufSubaru[TEXT_BUF_LEN] = {0};
			swprintf(wszBufSubaru, TEXT_BUF_LEN, L"%s", "SUBARU");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_JEEP, wszBufSubaru);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_JEEP, true);
			wchar_t wszBufNissan[TEXT_BUF_LEN] = {0};
			swprintf(wszBufNissan, TEXT_BUF_LEN, L"%s", "Nissan");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_FLAT, wszBufNissan);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FLAT, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_206, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NEXT, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_EDIT_WDN, false);
		    SetOneListCurClickId(nCtrlId);
		}
		break;	
		case UIID_CANBUS_SETTINGS_BUTTON_BINARY:
		{
			wchar_t wszBufToyota[TEXT_BUF_LEN] = {0};
			swprintf(wszBufToyota, TEXT_BUF_LEN, L"%s", "Toyota");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, wszBufToyota);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NISSAN, true);
			wchar_t wszBufHonda[TEXT_BUF_LEN] = {0};
			swprintf(wszBufHonda, TEXT_BUF_LEN, L"%s", "Honda");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI, wszBufHonda);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI, true);
			wchar_t wszBufMazda[TEXT_BUF_LEN] = {0};
			swprintf(wszBufMazda, TEXT_BUF_LEN, L"%s", "Mazda");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_KIA, wszBufMazda);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_KIA, true);
			wchar_t wszBufHyundai[TEXT_BUF_LEN] = {0};
			swprintf(wszBufHyundai, TEXT_BUF_LEN, L"%s", "Hyundai");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_HONDA, wszBufHyundai);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_HONDA, true);
			wchar_t wszBufBunick[TEXT_BUF_LEN] = {0};
			swprintf(wszBufBunick, TEXT_BUF_LEN, L"%s", "Bunick");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_FORD, wszBufBunick);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FORD, true);
			wchar_t wszBufVm[TEXT_BUF_LEN] = {0};
			swprintf(wszBufVm, TEXT_BUF_LEN, L"%s", "VM");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_VW, wszBufVm);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_VW, true);
			wchar_t wszBufLexuw[TEXT_BUF_LEN] = {0};
			swprintf(wszBufLexuw, TEXT_BUF_LEN, L"%s", "Lexuw");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_JEEP, wszBufLexuw);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_JEEP, true);
			wchar_t wszBufFord[TEXT_BUF_LEN] = {0};
			swprintf(wszBufFord, TEXT_BUF_LEN, L"%s", "Ford");
			pUIAnalysis->SetItemText(nWndId, UIID_CANBUS_SETTINGS_BUTTON_FLAT, wszBufFord);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_FLAT, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_206, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NEXT, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_EDIT_WDN, false);
		    SetOneListCurClickId(nCtrlId);
		}
		break;							
		case UIID_CANBUS_SETTINGS_BUTTON_NISSAN:
		case UIID_CANBUS_SETTINGS_BUTTON_HYUANDAI:
		case UIID_CANBUS_SETTINGS_BUTTON_KIA:
		case UIID_CANBUS_SETTINGS_BUTTON_HONDA:
		case UIID_CANBUS_SETTINGS_BUTTON_FORD:
		case UIID_CANBUS_SETTINGS_BUTTON_VW:
		case UIID_CANBUS_SETTINGS_BUTTON_JEEP:
		case UIID_CANBUS_SETTINGS_BUTTON_FLAT:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_SELECT_TWO, false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_EDIT_WDN, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_206, true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_CANBUS_SETTINGS_BUTTON_NEXT, true);
			m_nCanBoxTwoCtrlId = nCtrlId;
		}
		break;
		case UIID_CANBUS_SETTINGS_BUTTON_NEXT:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_CANBUS_POP, true);
		}
		break;
		case UIID_CANBUS_POP_BUTTON_CANCLE:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_CANBUS_POP, false);
		}
		break;
		case UIID_CANBUS_POP_BUTTON_OK:
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_CANBUS_POP, false);
			// 发给MCU重启
		}
		break;
		default:
		break;
	}
}

void SettingView::SetOneListCurClickId(int nCtrlID)
{
	switch(nCtrlID)
	{
		case UIID_CANBUS_SETTINGS_BUTTON_NOCAN:
		{
			m_nCanBoxCtrlId = UIID_CANBUS_SETTINGS_BUTTON_NOCAN;
		}
		break;
		case UIID_CANBUS_SETTINGS_BUTTON_HIWORLD:
		{
			m_nCanBoxCtrlId = UIID_CANBUS_SETTINGS_BUTTON_HIWORLD;
		}
		break;
		case UIID_CANBUS_SETTINGS_BUTTON_XFY:
		{
			m_nCanBoxCtrlId = UIID_CANBUS_SETTINGS_BUTTON_XFY;
		}
		break;
		case UIID_CANBUS_SETTINGS_BUTTON_OUDI:
		{
			m_nCanBoxCtrlId = UIID_CANBUS_SETTINGS_BUTTON_OUDI;
		}
		break;
		case UIID_CANBUS_SETTINGS_BUTTON_SIMPLE:
		{
			m_nCanBoxCtrlId = UIID_CANBUS_SETTINGS_BUTTON_SIMPLE;
		}
		break;
		case UIID_CANBUS_SETTINGS_BUTTON_BINARY:
		{
			m_nCanBoxCtrlId = UIID_CANBUS_SETTINGS_BUTTON_BINARY;
		}
		break;
		default:
		break;
	}
    m_nCanBoxTwoCtrlId = 0;
	m_nCanBoxThreeCtrlId = 0;
}

std::string SettingView::GetEncodePassWorld(std::string passWorld)
{
	std::string encodePassWorld = "";
	for (int i = 0; i < passWorld.length(); i++) {
		encodePassWorld+=".";
	}
    return encodePassWorld;
}

string SettingView::GetPassWorldKey(long nCtrlID) 
{
	string keyValue = "";
	switch(nCtrlID)
	{
		case UIID_CANBUS_PASSWORD_ONE:
		{
           keyValue = "1";
		}
		break;
		case UIID_CANBUS_PASSWORD_TWO:
		{
           keyValue = "2";
		}
		break;
		case UIID_CANBUS_PASSWORD_THREE:
		{
           keyValue = "3";
		}
		break;
		case UIID_CANBUS_PASSWORD_FOUR:
		{
           keyValue = "4";
		}
		break;
		case UIID_CANBUS_PASSWORD_FIVE:
		{
           keyValue = "5";
		}
		break;
		case UIID_CANBUS_PASSWORD_SIX:
		{
           keyValue = "6";
		}
		break;
		case UIID_CANBUS_PASSWORD_SEVEN:
		{
           keyValue = "7";
		}
		break;
		case UIID_CANBUS_PASSWORD_EIGHT:
		{
           keyValue = "8";
		}
		break;
		case UIID_CANBUS_PASSWORD_NINE:
		{
           keyValue = "9";
		}
		break;
		case UIID_CANBUS_PASSWORD_ZERO:
		{
           keyValue = "0";
		}
		break;
		default:
		break;
	}
    return keyValue;
}
void SettingView::SettingOnComponent(CUIWndMng *pUI, PWINDOW_INFO pInfo)
{
	LOGDBG("jgwen--- pInfo.nInfoType=%d,nWindowID=%d,nCtrlID=%d ,status=%d\n",\
			pInfo->nInfoType, pInfo->nWindowID, pInfo->nCtrlID, pInfo->nOperationStatus);

	//int nWndID = pInfo->nWindowID;
	//int nCtrlID = pInfo->nCtrlID;
	//int nStatus = pInfo->nOperationStatus;
	//int nWndType = pInfo->nInfoType;
}

void SettingView::SettingOnListInfoMessage(CUIWndMng *pUI, int nWndId, int nCtrlId, PLIST_INFO pInfo)
{
	//LOGDBG("SettingView ListInfo: nWndId=%d, nCtrlId=%d \n", nWndId, nCtrlId);
}

void SettingView::SettingInitWorkWnd()
{
	//update
	//OnHide_AudioBtn();
	SettingData &mData = SettingData::GetInstance();

	OnUpdate_DemoModeSwitch(mData.GetDemoMode());
	//更新音量控制进度
	OnUpdateUI_AudioCtrl();
}

void SettingView::OnHide_AudioBtn()
{
	//隐藏不需要的按钮
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, 1713,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, 1714,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, 1715,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, 1716,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, 1717,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, 1718,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, 1719,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, 1720,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, 1721,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, 1722,false);
}

void SettingView::SettingOnShow(LONG dwWndId)
{
	LOGDBG("jgwen--- OnShow Wnd ID Is %d", dwWndId);
	// EnableAlphaBlending(true);
	SetSetCurrWnd(dwWndId);
}

void SettingView::OnTimer(uint32_t id)
{
	LOGDBG("jgwen--- OnTimer id :%d", id);
	switch (id)
	{
		case TIMERID_SET_RSET:
		{
			KillTimer(TIMERID_SET_RSET);
			T_APP_MSG mMsg = {0,0,0,0};
			mMsg.Message = SYS_VMSG_UPDATE_TIRE_RESET;
			mMsg.Param0 = 1;
			GetViewBase()->SendViewMessage(&mMsg);
		}
		break;
		default:
		{
			break;
		}
	}
}

void SettingView::onClick_SettingList()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));
	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_SETTING,UIID_SETTING_LIST,UItem);
	CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;

	if(NULL == pList)
	{
		return;
	}

	int nLine = 0;
	int ItemData = 0;
	int nCol = 0;

	pList->GetCurSel(nLine,ItemData,nCol);

	switch(nLine)
	{
		case 0:
		{
			//语言
		}
		break;
		case 1:
		{
			//date
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SETTING,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_DATE,true);
		}
		break;
		case 2:
		{
			//背光
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SETTING,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SIDE,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_BKLIGHT,true);
		}
		break;
		case 3:
		{
			//复位
			//pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SETTING,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_RESET,true);
		}
		break;
		case 4:
		{
			//system
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SETTING,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SYSTEM,true);
		}
		break;
		case 5:
		{
			LOGDBG("UIID_WNDID_SETTING  CANBOX SETTING");
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SIDE,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SETTING,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_DATE,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_BKLIGHT,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_RESET,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SYSTEM,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CONNECT,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CAR,true);

			//canbox top
			m_nCanBox_Level = 1;
			OnInList_CanboxData(m_nCanBox_Level);
		}
		break;
		default:
		break;
	}
}

/**
 * 设置下 CanBox协议的目录(共三级目录)
*/
void SettingView::OnInList_CanboxData(int nLevel,int nLine,int nItemData)
{
	LOGDBG("SettingView::OnInList_CanboxData nLevel=%d, nLine=%d,nItemData=%d\n",nLevel,nLine,nItemData);
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}

	int nCount = 0;
	char szFileName[MAX_FILE_LEN] = {0};

	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));
	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_SETTING,UIID_SETTING_CANBOX_CAR_LIST,UItem);
	CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;

	if(pList!=NULL)
	{
		pList->DeleteAllItems();
	}

	if(1 == nLevel) // 一级目录 (协议供应商 hiworld、Raise 等)
	{
		for(int i = 0;i < 4;i++)
		{
			char szTemp[2*1024] = {0};
			size_t nLen = 1024;
			
			//转换
			ConverDecode(g_strCANBoxName[i],szTemp,nLen);

			pList->InsertItem(i);
			pList->SetItemString(i,(wchar_t*)szTemp);
		}
	}
	else if(2 == nLevel) // 二级目录 (车系 奔驰、宝马、奥迪 等等)
	{
		// ==============年后再开这个功能=============================
		char tem[MAX_PROTOCOL_CNT][128] = {0};    // 存储去重后的car_series
		int temId[MAX_PROTOCOL_CNT] = {0};       // 存储去重后的car_id
		memset(tem, 0x00, sizeof(tem));
		memset(temId, 0x00, sizeof(temId));
		int tem_cnt = 0; 

		// 遍历所有协议配置
		for (int i = 0; i < g_proto_config_cnt; i++) {
			// 1. 筛选vendor=Hiworld的条目（空指针+字符串比较防护）
			if (m_sCBProto_Configs[i].vendor == nullptr || 
				m_sCBProto_Configs[i].car_series == nullptr ||
				strcmp(m_sCBProto_Configs[i].vendor, g_strCANBoxName[m_nCanBox_FirstLine]) != 0) 
				{ 
					continue;  // 非Hiworld/空值，跳过
				}
			LOGDBG("add le 添加\n");
			// 2. 提取当前car_series（避免空字符串）
			const char* curr_series = m_sCBProto_Configs[i].car_series;
			if (curr_series[0] == '\0') {
				continue;  // 空字符串，跳过
			}
			LOGDBG("add le1：当前car_id:%d, car_series=%s\n", m_sCBProto_Configs[i].id,curr_series);
			// 3. 去重判断：检查当前car_series是否已在tem数组中
			bool is_duplicate = false;
			for (int j = 0; j < tem_cnt; j++) {
				if (strcmp(tem[j], curr_series) == 0) {
					LOGDBG("add le2：%s已存在，去重跳过\n", curr_series);
					is_duplicate = true;
					break;  // 已存在，无需重复添加
				}
			}
			LOGDBG("add le2 添加\n");
			// 4. 非重复且数组未超限，加入tem数组
			if (!is_duplicate && tem_cnt < MAX_PROTOCOL_CNT) {

				temId[tem_cnt] = m_sCBProto_Configs[i].id; // 记下这个协议的id

				// 安全拷贝（防字符串超长越界）
				strncpy(tem[tem_cnt], curr_series, 127);
				tem[tem_cnt][127] = '\0';  // 确保字符串结束符
				tem_cnt++;  // 计数+1
		        LOGDBG("成功添加：tem[%d]=%s，当前tem_cnt=%d\n", tem_cnt-1, curr_series, tem_cnt);
			}
		}

		memset(g_strCarSeries, 0x00, sizeof(g_strCarSeries));
		for (int i = 0; i < tem_cnt; i++) {
			strncpy(g_strCarSeries[i], tem[i], 127);
			g_strCarSeries[i][127] = '\0';
		}

		LOGDBG("成功将tem中%d个车系赋值到g_strCarSeries：\n", tem_cnt);
		// for (int j = 0; j < 6; j++) {
		// 	if (g_strCarSeries[j][0] != '\0') {  // 只打印非空元素
		// 		printf("g_strCarSeries[%d] = %s\n", j, g_strCarSeries[j]);
		// 	} else {
		// 		printf("g_strCarSeries[%d] = (空)\n", j);
		// 	}
		// }


		for(int i = 0;i < tem_cnt;i++)
		{
			char szTemp[2*1024] = {0};
			size_t nLen = 1024;
			//转换
			ConverDecode(g_strCarSeries[i],szTemp,nLen);

			pList->InsertItem(temId[i]); // 赋值id
			LOGDBG("temId[%d]=%d\n",i,temId[i]);
			pList->SetItemString(i,(wchar_t*)szTemp);
		}		


		// 原来的 
		// for(int i = 0;i < 6;i++)
		// {
		// 	char szTemp[2*1024] = {0};
		// 	size_t nLen = 1024;
		// 	//转换
		// 	ConverDecode(g_strCarSeries[i],szTemp,nLen);

		// 	pList->InsertItem(i);
		// 	pList->SetItemString(i,(wchar_t*)szTemp);

		// }		
	}
	else if(3 == nLevel) // 三级目录（车型 卡罗拉21款 丰田21-22款 等等）
	{
		if(0 == nLine)
		{
			nCount = 5;
		}
		else
		{
			nCount = 10;
		}

		LOGDBG("two level onclick  id=%d \n",nItemData);
		// 通过nItemData(也是协议唯一ID)去识别是什么车系、什么供应商，然后这个下面显示都是这个对应的
		memset(&m_tmpSelectVen, 0, sizeof(T_CANBOX_PROTO_CONFIG));
		
		// 2. 根据选中的ID找到对应的筛选条件（供应商、车系、车型）
		int find_flag = 0; // 标记是否找到匹配的ID
		for(int i=0; i<g_proto_config_cnt; i++) {
			if(m_sCBProto_Configs[i].id == nItemData) {
				// 正确赋值字符数组（用strncpy防止缓冲区溢出）
				m_tmpSelectVen.id = m_sCBProto_Configs[i].id;
				strncpy(m_tmpSelectVen.vendor, m_sCBProto_Configs[i].vendor, sizeof(m_tmpSelectVen.vendor)-1);
				strncpy(m_tmpSelectVen.car_series, m_sCBProto_Configs[i].car_series, sizeof(m_tmpSelectVen.car_series)-1);
				strncpy(m_tmpSelectVen.car_type, m_sCBProto_Configs[i].car_type, sizeof(m_tmpSelectVen.car_type)-1);
				strncpy(m_tmpSelectVen.so_name, m_sCBProto_Configs[i].so_name, sizeof(m_tmpSelectVen.so_name)-1);
				
				// 手动补字符串结束符（防止strncpy未截断时无结束符）
				m_tmpSelectVen.vendor[sizeof(m_tmpSelectVen.vendor)-1] = '\0';
				m_tmpSelectVen.car_series[sizeof(m_tmpSelectVen.car_series)-1] = '\0';
				m_tmpSelectVen.car_type[sizeof(m_tmpSelectVen.car_type)-1] = '\0';
				m_tmpSelectVen.so_name[sizeof(m_tmpSelectVen.so_name)-1] = '\0';
				
				find_flag = 1;
				LOGDBG("找到匹配的ID: %d，供应商: %s，车系: %s\n", nItemData, m_tmpSelectVen.vendor, m_tmpSelectVen.car_series);
				break; // ID唯一，找到后直接退出循环
			}
		}
		
		// 未找到匹配ID时直接返回，避免无效筛选
		if(!find_flag) {
			LOGDBG("未找到ID=%d对应的协议配置\n", nItemData);
			return;
		}
		
		// 3. 筛选所有匹配「供应商+车系+车型」的协议，存入三级列表临时数组
		int cunt = 0; // 三级列表有效数据计数器（作为临时数组索引）
		// 先清空临时数组，避免旧数据干扰
		memset(m_tmpSeAllProto, 0, sizeof(m_tmpSeAllProto));
		
		for(int i=0; i<g_proto_config_cnt; i++) {
			// 正确比较字符串（strcmp返回0表示相等）
			if(strcmp(m_tmpSelectVen.vendor, m_sCBProto_Configs[i].vendor) == 0 &&
			strcmp(m_tmpSelectVen.car_series, m_sCBProto_Configs[i].car_series) == 0 ) {
				
				// 用cunt作为索引，保证临时数组数据连续无空洞
				strncpy(m_tmpSeAllProto[cunt].vendor, m_sCBProto_Configs[i].vendor, sizeof(m_tmpSeAllProto[cunt].vendor)-1);
				strncpy(m_tmpSeAllProto[cunt].car_series, m_sCBProto_Configs[i].car_series, sizeof(m_tmpSeAllProto[cunt].car_series)-1);
				strncpy(m_tmpSeAllProto[cunt].car_type, m_sCBProto_Configs[i].car_type, sizeof(m_tmpSeAllProto[cunt].car_type)-1);
				strncpy(m_tmpSeAllProto[cunt].so_name, m_sCBProto_Configs[i].so_name, sizeof(m_tmpSeAllProto[cunt].so_name)-1);
				m_tmpSeAllProto[cunt].id = m_sCBProto_Configs[i].id;
				
				// 补字符串结束符
				m_tmpSeAllProto[cunt].vendor[sizeof(m_tmpSeAllProto[cunt].vendor)-1] = '\0';
				m_tmpSeAllProto[cunt].car_series[sizeof(m_tmpSeAllProto[cunt].car_series)-1] = '\0';
				m_tmpSeAllProto[cunt].car_type[sizeof(m_tmpSeAllProto[cunt].car_type)-1] = '\0';
				m_tmpSeAllProto[cunt].so_name[sizeof(m_tmpSeAllProto[cunt].so_name)-1] = '\0';
				
				cunt++;
				// 防止数组越界
				if(cunt >= MAX_PROTOCOL_CNT) {
					LOGDBG("警告：三级列表协议数量超过最大值，已截断\n");
					break;
				}
			}
		}

		printf("there cunt = %d\n",cunt);
		// 渲染三级列表
		for(int i = 0;i < cunt;i++)
		{
			char szTemp[2*1024] = {0};
			size_t nLen = 1024;

			ConverDecode(m_tmpSeAllProto[i].car_type,szTemp,nLen);

			pList->InsertItem(m_tmpSeAllProto[i].id);
			pList->SetItemString(i,(wchar_t*)szTemp);
		}



		// for(int i = 0;i < nCount;i++)
		// {
		// 	char szTemp[2*1024] = {0};
		// 	size_t nLen = 1024;
		// 	switch(nLine)
		// 	{
		// 		case 0:
		// 		{
		// 			ConverDecode(g_strBwmName[i],szTemp,nLen);
		// 		}
		// 		break;
		// 		case 1:
		// 		{
		// 			ConverDecode(g_strAlfaName[i],szTemp,nLen);
		// 		}
		// 		break;
		// 		case 2:
		// 		{
		// 			ConverDecode(g_strBenzName[i],szTemp,nLen);
		// 		}
		// 		break;
		// 		case 3:
		// 		{
		// 			ConverDecode(g_strVmName[i],szTemp,nLen);
		// 		}
		// 		break;
		// 		case 4:
		// 		{
		// 			ConverDecode(g_strBulckVmName[i],szTemp,nLen);
		// 		}
		// 		break;
		// 		case 5:
		// 		{
		// 			ConverDecode(g_strToyotaName[i],szTemp,nLen);
		// 		}
		// 		default:
		// 		break;
		// 	}
		// 	pList->InsertItem(i);
		// 	pList->SetItemString(i,(wchar_t*)szTemp);
		// }
	}
}

void SettingView::onClick_SettingBtn()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

    pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_CAR_SETTING,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SYSTEM,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_RESET,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_BKLIGHT,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_DATE,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_AUDIO,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SETTING,true);
}

void SettingView::onClick_AudioBtn()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

    pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_CAR_SETTING,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SYSTEM,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_RESET,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_BKLIGHT,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_DATE,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SETTING,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_AUDIO,true);
}

void SettingView::onClick_CarBtn()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SYSTEM,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_RESET,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_BKLIGHT,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_DATE,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SETTING,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_AUDIO,false);
	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_CAR_SETTING,true);
}

void SettingView::onClick_CloseView()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	bool bTimeDate_Visble = false;

	bTimeDate_Visble = pUIMgr->GetUIAnalysisObj()->IsItemVisible(UIID_WNDID_SETTING,UIID_GROUP_DATE);

	if(bTimeDate_Visble)
	{
		//下发给MCU设置时间
		SetCurTime(m_tTime);
	}

	GetViewBase()->ShowWnd(m_nInWnd, true);	
}

void SettingView::onClick_ReturnBtn()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	bool bSetting_Visble = false;
	bool bAudio_Visble = false;
	bool bBklight_Visble = false;
	bool bTimeDate_Visble = false;
	bool bCanBusConnect_Visble = false;
	bool bCanBusSetting_Visble = false;
	bool bCarSet_Visble = false;

	bSetting_Visble = pUIMgr->GetUIAnalysisObj()->IsItemVisible(UIID_WNDID_SETTING,UIID_GROUP_SETTING);
	bAudio_Visble = pUIMgr->GetUIAnalysisObj()->IsItemVisible(UIID_WNDID_SETTING,UIID_GROUP_AUDIO);
	bBklight_Visble = pUIMgr->GetUIAnalysisObj()->IsItemVisible(UIID_WNDID_SETTING,UIID_GROUP_BKLIGHT);

	bTimeDate_Visble = pUIMgr->GetUIAnalysisObj()->IsItemVisible(UIID_WNDID_SETTING,UIID_GROUP_DATE);
	bCarSet_Visble = pUIMgr->GetUIAnalysisObj()->IsItemVisible(UIID_WNDID_SETTING,UIID_GROUP_CAR_SETTING);
	bCanBusConnect_Visble = pUIMgr->GetUIAnalysisObj()->IsItemVisible(UIID_WNDID_SETTING,UUID_GROUP_CANBOX_CONNECT);
	bCanBusSetting_Visble = pUIMgr->GetUIAnalysisObj()->IsItemVisible(UIID_WNDID_SETTING,UUID_GROUP_CANBOX_CAR);

	if(bTimeDate_Visble)
	{
		//下发给MCU设置时间
		SetCurTime(m_tTime);
	}

	if(bBklight_Visble)
	{
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_BKLIGHT,false);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SIDE,true);
		pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SETTING,true);
	}
	else
	{
		if(bSetting_Visble || bAudio_Visble || bCarSet_Visble)
		{
			GetViewBase()->ShowWnd(m_nInWnd, true);
		}
		else if(bCanBusConnect_Visble)
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CONNECT,false);
		    pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CAR,true);
		}
		else if(bCanBusSetting_Visble)
		{
			if(1 == m_nCanBox_Level)
			{
				m_nCanBox_Level = 0;
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SIDE,true);
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SETTING,true);
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CAR,false);
			}
			else if(2 == m_nCanBox_Level)
			{
				m_nCanBox_Level = 1;
				OnInList_CanboxData(m_nCanBox_Level);
			}
			else if(3 == m_nCanBox_Level)
			{
				m_nCanBox_Level = 2;
				OnInList_CanboxData(m_nCanBox_Level);
			}
			else
			{
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SIDE,true);
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SETTING,true);
				pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CAR,false);		
			}
		}
		else
		{
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SYSTEM,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_RESET,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_BKLIGHT,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_DATE,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_AUDIO,false);
			//pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_CANBUS_SETTINGS,false);
			//pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_CANBUS_PASSWORD,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CONNECT,false);
		    pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UUID_GROUP_CANBOX_CAR,false);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SIDE,true);
			pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_SETTING,true);
		}
	}

}

void SettingView::onClick_RestOKBtn()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_RESET,false);

	//发给MCU重启
	if(NULL != m_ViewController)
	{
		system("touch /media/flash/userdata/speaker_mode &");
		system("rm /media/flash/userdata/audio_boost.conf &");
		m_ViewController->Mpu_McuRest();
	}
}

void SettingView::onClick_RestCancelBtn()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	pUIMgr->GetUIAnalysisObj()->ShowItem(UIID_WNDID_SETTING, UIID_GROUP_RESET,false);
}

void SettingView::onClick_DateTimeBtn(int nCtrlId)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();

	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	switch(nCtrlId)
	{
		case UIID_SETTING_DATETIME_DATE_UP:
		{
			if(TIMEDATE_DD_MM_YY == m_nDateTime_mode)
			{
				int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);

				m_tTime.nDay++;

				if(nDayMax <= m_tTime.nDay)
				{
					m_tTime.nDay = 1;
				}
			}
			else
			{
				//调节月
				m_tTime.nMonth++;
				if(m_tTime.nMonth > 12)
				{
					m_tTime.nMonth = 1;
				}

				int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
				if(m_tTime.nDay >  nDayMax)
				{
					m_tTime.nDay = nDayMax;
				}
			}
		}
		break;
		case UIID_SETTING_DATETIME_DATE_DN:
		{
			if(TIMEDATE_DD_MM_YY == m_nDateTime_mode)
			{
				int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);

				m_tTime.nDay--;

				if(m_tTime.nDay < 1)
				{
					m_tTime.nDay = nDayMax;
				}
			}
			else
			{
				//调节月
				m_tTime.nMonth--;
				if(m_tTime.nMonth < 1)
				{
					m_tTime.nMonth = 12;
				}
				int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
				if(m_tTime.nDay >  nDayMax)
				{
					m_tTime.nDay = nDayMax;
				}				
			}
		}
		break;
		case UIID_SETTING_DATETIME_MONTH_UP:
		{
			if(TIMEDATE_DD_MM_YY == m_nDateTime_mode)
			{
				m_tTime.nMonth++;
				if(m_tTime.nMonth > 12)
				{
					m_tTime.nMonth = 1;
				}

				int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
				if(m_tTime.nDay >  nDayMax)
				{
					m_tTime.nDay = nDayMax;
				}
			}
			else
			{
				//调节日
				int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);

				m_tTime.nDay++;

				if(nDayMax <= m_tTime.nDay)
				{
					m_tTime.nDay = 1;
				}				
			}
		}
		break;
		case UIID_SETTING_DATETIME_MONTH_DN:
		{
			if(TIMEDATE_DD_MM_YY == m_nDateTime_mode)
			{
				m_tTime.nMonth--;
				if(m_tTime.nMonth < 1)
				{
					m_tTime.nMonth = 12;
				}
				int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
				if(m_tTime.nDay >  nDayMax)
				{
					m_tTime.nDay = nDayMax;
				}
			}
			else
			{
				int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
				m_tTime.nDay--;
				if(m_tTime.nDay < 1)
				{
					m_tTime.nDay = nDayMax;
				}
			}
		}
		break;
		case UIID_SETTING_DATETIME_YEAR_UP:
		{
			m_tTime.nYear++;
		    if(m_tTime.nYear < SETTING_DATE_YEAR_MIN)
			{
				m_tTime.nYear = SETTING_DATE_YEAR_MIN;
			}
			if(m_tTime.nYear  > SETTING_DATE_YEAR_MAX)
			{
				m_tTime.nYear = SETTING_DATE_YEAR_MAX;
			}
			int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
			if(m_tTime.nDay >  nDayMax)
			{
				m_tTime.nDay = nDayMax;
			}
		}
		break;
		case UIID_SETTING_DATETIME_YEAR_DN:
		{
			m_tTime.nYear--;
			if(m_tTime.nYear < SETTING_DATE_YEAR_MIN)
			{
				m_tTime.nYear = SETTING_DATE_YEAR_MIN;
			}
			if(m_tTime.nYear  > SETTING_DATE_YEAR_MAX)
			{
				m_tTime.nYear = SETTING_DATE_YEAR_MAX;
			}
			int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
			if(m_tTime.nDay >  nDayMax)
			{
				m_tTime.nDay = nDayMax;
			}
		}
		break;
		case UIID_SETTING_DATETIME_HOURS_UP:
		{
			m_tTime.nHours++;
			if(m_tTime.nHours > 23)
			{
				m_tTime.nHours = 0;
			}
		}
		break;
		case UIID_SETTING_DATETIME_HOURS_DN:
		{
			m_tTime.nHours--;
			if(m_tTime.nHours < 0)
			{
				m_tTime.nHours = 23;
			}			
		}
		break;
		case UIID_SETTING_DATETIME_MIN_UP:
		{
			m_tTime.nMinutes++;
			if(m_tTime.nMinutes > 59)
			{
				m_tTime.nMinutes = 0;
			}
		}
		break;
		case UIID_SETTING_DATETIME_MIN_DN:
		{
			m_tTime.nMinutes--;
			if(m_tTime.nMinutes < 0)
			{
				m_tTime.nMinutes = 59;
			}
		}
		break;
		case UIID_SETTING_DATETIME_MODE_UP:
		case UIID_SETTING_DATETIME_MODE_DN:
		{
			if(0 == m_tTime.nClockMode)
			{
				m_tTime.nClockMode = 1;
			}
			else
			{
				m_tTime.nClockMode = 0;
			}
		}
		break;
		case UIID_SETTING_DATETIME_DDMM:
		{
			m_nDateTime_mode = TIMEDATE_DD_MM_YY;
		}
		break;
		case UIID_SETTING_DATETIME_MMDD:
		{
			m_nDateTime_mode = TIMEDATE_MM_DD_YY;
		}
		break;
		default:
		break;
	}

	//更新UI
	UpateUI_DateTime(m_tTime);
}

void SettingView::UpateUI_DateTime(T_STR_TIME_INFO tTimeInfo)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();

	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	wchar_t BufYear[TEXT_BUF_LEN] = {0};
	wchar_t BufMonth[TEXT_BUF_LEN] = {0};
	wchar_t BufDay[TEXT_BUF_LEN] = {0};
	wchar_t BufHours[TEXT_BUF_LEN] = {0};
	wchar_t BufMin[TEXT_BUF_LEN] = {0};

	swprintf(BufYear, TEXT_BUF_LEN, L"%d", m_tTime.nYear);
	swprintf(BufMonth, TEXT_BUF_LEN, L"%d", m_tTime.nMonth);
	swprintf(BufDay, TEXT_BUF_LEN, L"%d", m_tTime.nDay);
	swprintf(BufHours, TEXT_BUF_LEN, L"%2d", m_tTime.nHours);
	swprintf(BufMin, TEXT_BUF_LEN, L"%2d", m_tTime.nMinutes);

	if(TIMEDATE_DD_MM_YY == m_nDateTime_mode)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_YEAR, BufYear);
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_MONTH, BufMonth);
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_DATE, BufDay);

		//更新DD-MM-YY按钮
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_SETTING,UIID_SETTING_DATETIME_DDMM,ITEM_STATE_SELECTED);
	}
	else
	{
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_YEAR, BufYear);
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_MONTH, BufDay);
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_DATE, BufMonth);

		//更新MM-DD-YY按钮
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_SETTING,UIID_SETTING_DATETIME_MMDD,ITEM_STATE_SELECTED);
	}
    
	if(0 == tTimeInfo.nClockMode)
	{
		//24小时制
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_HOURS, BufHours);
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_MIN, BufMin);

		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_MODE, 509);
	}
	else
	{
		//12小时制
		wchar_t TempHours[TEXT_BUF_LEN] = {0};
		int nTempH = m_tTime.nHours%12;

		swprintf(TempHours, TEXT_BUF_LEN, L"%2d",nTempH);

		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_HOURS, TempHours);
		pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_MIN, BufMin);

		if(m_tTime.nHours <= 12)
		{
			//PM
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_MODE, 511);
		}
		else
		{
			//AM
			pUIMgr->GetUIAnalysisObj()->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_DATETIME_MODE, 510);
		}
	}
}

void SettingView::UpateUI_Version()
{
	CUIWndMgrWrapper *pUIMgr = NULL;
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	SettingData &mData = SettingData::GetInstance();
	wchar_t wszBufMcu[TEXT_BUF_LEN] = {0};
	wchar_t wszBufMpu[40] = {0};

	char tempMpu[11] = {0};
	string strMPUver = "";
	mData.GetMPUVersion(strMPUver);
	memcpy(tempMpu,strMPUver.c_str(),10);

	swprintf(wszBufMpu, 40, L"MPU:%s",tempMpu);

	string strMCUver = "";
	mData.GetMCUVersion(strMCUver);
	swprintf(wszBufMcu, TEXT_BUF_LEN, L"MCU:%s", strMCUver.c_str());

	LOGDBG("jgwen--- MPU =%s, MCU = %s\n", tempMpu, strMCUver.c_str());

	//更新在列表
	UIITEMDATA UItem;
	memset(&UItem,0,sizeof(UItem));
	pUIMgr->GetUIAnalysisObj()->GetItemData(UIID_WNDID_SETTING,UIID_SETTING_VERSION_LIST,UItem);
	CUI_BtList *pList = (CUI_BtList*) UItem.m_pCtrl;

	if(NULL == pList)
	{
		return;
	}
	pList->DeleteAllItems();

	pList->InsertItem(0);
	pList->SetItemString(0,wszBufMpu);

	pList->InsertItem(1);
	pList->SetItemString(1,wszBufMcu);
}

void SettingView::onClick_BrightValue(int nToWard)
{
	CUIWndMgrWrapper *pUIMgr = NULL;
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	wchar_t wszBuf[TEXT_BUF_LEN] = {0};
	SettingData &mData = SettingData::GetInstance();
	int nBacklight = 0;
	mData.GetINTData(INT_BACKLIGHT_LEVEL, nBacklight);

	if(0 == nToWard)
	{
		//向左调节
		nBacklight--;
		if(nBacklight < 1)
		{
			nBacklight = 1;
		}
	}
	else
	{
		//向右调节
		nBacklight++;
		if(nBacklight > 12)
		{
			nBacklight = 12;
		}
	}

	pUIAnalysis->SetProgressItemValues(UIID_WNDID_SETTING, UIID_SETTING_BRIGHT_BAR, 11, nBacklight-1);
	swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nBacklight);
	pUIAnalysis->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_BRIGHT_VALUE, wszBuf); 

	//下发给MCU
	SetbackLight(nBacklight);

	//mData.SaveINTData(INT_BACKLIGHT_LEVEL, nBacklight);
}

void SettingView::OnUpdateUI_AudioCtrl()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	SettingData &mData = SettingData::GetInstance();
	int nMedioVolume = 0;
	int nBtVolume = 0;

	mData.GetINTData(INT_VOLUME_LEVEL, nMedioVolume);
	mData.GetINTData(INT_BT_VOLUME_LEVEL, nBtVolume);

	wchar_t wszMainBuf[TEXT_BUF_LEN] = {0};
	swprintf(wszMainBuf, TEXT_BUF_LEN, L"%d", nMedioVolume);
	pUIAnalysis->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_AUDIO_MAIN_VALUE, wszMainBuf); 
	pUIAnalysis->SetProgressItemValues(UIID_WNDID_SETTING, UIID_SETTING_AUDIO_MAIN_BAR, 39, nMedioVolume-1);

	wchar_t wszBtBuf[TEXT_BUF_LEN] = {0};
	swprintf(wszBtBuf, TEXT_BUF_LEN, L"%d", nBtVolume);
	pUIAnalysis->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_AUDIO_BT_VALUE, wszBtBuf); 
	pUIAnalysis->SetProgressItemValues(UIID_WNDID_SETTING, UIID_SETTING_AUDIO_BT_BAR, 39, nBtVolume-1);
}

void SettingView::onClick_MainVolume(int nToWard)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}
	SettingData &mData = SettingData::GetInstance();
	int nMedioVolume = 0;
	mData.GetINTData(INT_VOLUME_LEVEL, nMedioVolume);

	if(0 == nToWard)
	{
		//音量-
		nMedioVolume--;
		if(nMedioVolume < 0)
		{
			nMedioVolume = 0;
		}
	}
	else
	{
		//音量+
		nMedioVolume++;
		if(nMedioVolume > 40)
		{
			nMedioVolume = 40;
		}
	}

	wchar_t wszBuf[TEXT_BUF_LEN] = {0};
	swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nMedioVolume);
	pUIAnalysis->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_AUDIO_MAIN_VALUE, wszBuf); 
	pUIAnalysis->SetProgressItemValues(UIID_WNDID_SETTING, UIID_SETTING_AUDIO_MAIN_BAR, 39, nMedioVolume-1);

	SetMedioVolume(nMedioVolume);

	mData.SaveINTData(INT_VOLUME_LEVEL, nMedioVolume);
}

void SettingView::onClick_BtVolume(int nToWard)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}
	SettingData &mData = SettingData::GetInstance();
	int nBtVolume = 0;
	mData.GetINTData(INT_BT_VOLUME_LEVEL, nBtVolume);

	if(0 == nToWard)
	{
		//音量-
		nBtVolume--;
		if(nBtVolume < 0)
		{
			nBtVolume = 0;
		}
	}
	else
	{
		//音量+
		nBtVolume++;
		if(nBtVolume > 40)
		{
			nBtVolume = 40;
		}
	}

	wchar_t wszBuf[TEXT_BUF_LEN] = {0};
	swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nBtVolume);
	pUIAnalysis->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_AUDIO_BT_VALUE, wszBuf); 
	pUIAnalysis->SetProgressItemValues(UIID_WNDID_SETTING, UIID_SETTING_AUDIO_BT_BAR, 39, nBtVolume-1);

	SetBtVolume(nBtVolume);

	mData.SaveINTData(INT_BT_VOLUME_LEVEL, nBtVolume);
}

void SettingView::onClick_DtsSwitch()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	SettingData &mData = SettingData::GetInstance();
	bool dtsSwitch = mData.GetDtsOnclick();
	if (dtsSwitch) 
	{
        mData.SetDtsOnclick(false);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_SETTING, UIID_DTS_BUTTON_SWITCH, UIID_PICID_DTS_SWITCH_ON, ALL_STATE);
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_SETTING,UIID_DTS_BUTTON_MODE1,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_SETTING,UIID_DTS_BUTTON_MODE2,ITEM_STATE_NORMAL);
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_SETTING,UIID_DTS_BUTTON_MODE3,ITEM_STATE_NORMAL);
	}
	else 
	{
        mData.SetDtsOnclick(true);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_SETTING, UIID_DTS_BUTTON_SWITCH, UIID_PICID_DTS_SWITCH_OFF, ALL_STATE);
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_SETTING,UIID_DTS_BUTTON_MODE1,ITEM_STATE_DISABLE);
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_SETTING,UIID_DTS_BUTTON_MODE2,ITEM_STATE_DISABLE);
		pUIMgr->GetUIAnalysisObj()->SetItemState(UIID_WNDID_SETTING,UIID_DTS_BUTTON_MODE3,ITEM_STATE_DISABLE);
	}
}

void SettingView::onClick_RoadsideParkSwitch()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	SettingData &mData = SettingData::GetInstance();
	bool dtsSwitch = mData.GetRoadsideParkOnclick();
	if (dtsSwitch) 
	{
        mData.SetRoadsideParkOnclick(false);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_SETTING, UIID_ROADSIDEPARK_BUTTON_SWITCH, UIID_PICID_DTS_SWITCH_ON, ALL_STATE);
	}
	else 
	{
        mData.SetRoadsideParkOnclick(true);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_SETTING, UIID_ROADSIDEPARK_BUTTON_SWITCH, UIID_PICID_DTS_SWITCH_OFF, ALL_STATE);
	}
}

void SettingView::onClick_StorageParkSwitch()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	SettingData &mData = SettingData::GetInstance();
	bool dtsSwitch = mData.GetStorageParkOnclick();
	if (dtsSwitch) 
	{
        mData.SetStorageParkOnclick(false);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_SETTING, UIID_STORAGEPARK_BUTTON_SWITCH, UIID_PICID_DTS_SWITCH_ON, ALL_STATE);
	}
	else 
	{
        mData.SetStorageParkOnclick(true);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_SETTING, UIID_STORAGEPARK_BUTTON_SWITCH, UIID_PICID_DTS_SWITCH_OFF, ALL_STATE);
	}
}

void SettingView::onClick_RaderSoundSwitch()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	SettingData &mData = SettingData::GetInstance();
	bool dtsSwitch = mData.GetRaderSoundOnclick();
	if (dtsSwitch) 
	{
        mData.SetRaderSoundOnclick(false);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_SETTING, UIID_RADERSOUND_BUTTON_SWITCH, UIID_PICID_DTS_SWITCH_ON, ALL_STATE);
	}
	else 
	{
        mData.SetRaderSoundOnclick(true);
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_SETTING, UIID_RADERSOUND_BUTTON_SWITCH, UIID_PICID_DTS_SWITCH_OFF, ALL_STATE);
	}
}

void SettingView::OnUpdate_DemoModeSwitch(bool bSwitch)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	if(bSwitch)
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_SETTING, UIID_DEMO_MODE_SWITCH, UIID_PICID_DTS_SWITCH_ON, ALL_STATE);
	}
	else
	{
		pUIMgr->GetUIAnalysisObj()->SetItemIcon(UIID_WNDID_SETTING, UIID_DEMO_MODE_SWITCH, UIID_PICID_DTS_SWITCH_OFF, ALL_STATE);
	}
}
void SettingView::onClickSystemView(CUIWndMng *pUI, long nWndId, long nCtrlId, int32_t nBtState, bool is_Need)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}
	SettingData &mData = SettingData::GetInstance();

}

// 音量
void SettingView::UpdateVolumePage(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
}
bool SettingView::UpdateVolumeBar(T_APP_MSG* msg)
{
	bool bRet = true;
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return false;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return false;
	}

	SettingData &mData = SettingData::GetInstance();
	int nMedioVolume = 0;
	int nBtVolume = 0;
	mData.GetINTData(INT_VOLUME_LEVEL, nMedioVolume);
	mData.GetINTData(INT_BT_VOLUME_LEVEL, nBtVolume);
	LOGDBG("jgwen---nMedioVolume=%d, nBtVolume=%d \n",nMedioVolume, nBtVolume);

	if(mData.GetMuteState() == 1) // 获取静音状态
	{
		nMedioVolume = 0;
		nBtVolume = 0;
	}

	if((nMedioVolume >= 0) && (nMedioVolume <= 40))
	{
		//多媒体
		wchar_t wszBuf[TEXT_BUF_LEN] = {0};
		swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nMedioVolume);
		pUIAnalysis->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_AUDIO_MAIN_VALUE, wszBuf); 
		pUIAnalysis->SetProgressItemValues(UIID_WNDID_SETTING, UIID_SETTING_AUDIO_MAIN_BAR, 39, nMedioVolume-1);
	}
	
	if((nBtVolume >= 0) && (nBtVolume <= 40))
	{
		int val = 0;
		if(nBtVolume < 1)
		{
			val = 1;
		}
		else
		{
			val = nBtVolume;
		}
		//蓝牙电话
		wchar_t wszBuf[TEXT_BUF_LEN] = {0};
		swprintf(wszBuf, TEXT_BUF_LEN, L"%d", val);
		pUIAnalysis->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_AUDIO_BT_VALUE, wszBuf); 
		pUIAnalysis->SetProgressItemValues(UIID_WNDID_SETTING, UIID_SETTING_AUDIO_BT_BAR, 39, nBtVolume-1);
	}
		
	return true;
}

	// 亮度
void SettingView::UpdateBackLightPage(T_APP_MSG* msg)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}

	CAutoApiSync sync(pUIMgr);
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}	
	int nWndId = GetSetCurrWnd();
	SettingData &mData = SettingData::GetInstance();
	int nBacklight = 0;
	int nPosLight = 0;
	int nAutoBg = 0;
	mData.GetINTData(INT_BACKLIGHT_LEVEL, nBacklight);			
	mData.GetINTData(INT_POSLIGHT, nPosLight);
	mData.GetINTData(INT_READ_LIGHT, nAutoBg);
	LOGDBG("jgwen---nWndId=%d, nBacklight=%d, nPosLight=%d\n",nWndId, nBacklight, nPosLight);

}
bool SettingView::UpdateBackLightBar(T_APP_MSG* msg)
{
	bool bRet = true;
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return false;
	}
	CAutoApiSync sync(pUIMgr);
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return false;
	}
	wchar_t wszBuf[TEXT_BUF_LEN] = {0};
	SettingData &mData = SettingData::GetInstance();
	int nBacklight = 0;
	mData.GetINTData(INT_BACKLIGHT_LEVEL, nBacklight);
	LOGDBG("jgwen--- nBacklight=%d", nBacklight);

	if((nBacklight >= 1) && (nBacklight <= 12))
	{
		pUIAnalysis->SetProgressItemValues(UIID_WNDID_SETTING, UIID_SETTING_BRIGHT_BAR, 11, nBacklight-1);
		swprintf(wszBuf, TEXT_BUF_LEN, L"%d", nBacklight);
		pUIAnalysis->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_BRIGHT_VALUE, wszBuf); 
	}
	else
	{
		pUIAnalysis->SetProgressItemValues(UIID_WNDID_SETTING, UIID_SETTING_BRIGHT_BAR, 11, 4);
		swprintf(wszBuf, TEXT_BUF_LEN, L"%d", 5);
		pUIAnalysis->SetItemText(UIID_WNDID_SETTING, UIID_SETTING_BRIGHT_VALUE, wszBuf);			
	}

	return true;
}

void SettingView::UpdateTimePag()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}
}

void SettingView::HandleDateSet(int CtrlId)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	switch(CtrlId)
	{
		case UIID_SYSTEM_DTAE_YEAR_UP_BTN:
		{	
			m_tTime.nYear++;
			if(m_tTime.nYear < SETTING_DATE_YEAR_MIN)
			{
				m_tTime.nYear = SETTING_DATE_YEAR_MIN;
			}
			if(m_tTime.nYear  > SETTING_DATE_YEAR_MAX)
			{
				m_tTime.nYear = SETTING_DATE_YEAR_MAX;
			}
			int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
			if(m_tTime.nDay >  nDayMax)	
			{
				m_tTime.nDay = nDayMax;
			}	
		}
		break;
		case UIID_SYSTEM_DTAE_YEAR_DOWN_BTN:
		{
			m_tTime.nYear--;
			if(m_tTime.nYear < SETTING_DATE_YEAR_MIN)
			{
				m_tTime.nYear = SETTING_DATE_YEAR_MIN;
			}
			if(m_tTime.nYear  > SETTING_DATE_YEAR_MAX)
			{
				m_tTime.nYear = SETTING_DATE_YEAR_MAX;
			}
			int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
			if(m_tTime.nDay >  nDayMax)	
			{
				m_tTime.nDay = nDayMax;
			}				
		}
		break;
		case UIID_SYSTEM_DTAE_MONTH_UP_BTN:
		{
			m_tTime.nMonth++;
			if(m_tTime.nMonth >= 13)
			{
				m_tTime.nMonth = 1;
			}
			int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
			if(m_tTime.nDay >  nDayMax)	
			{
				m_tTime.nDay = nDayMax;
			}		
		}
		break;
		case UIID_SYSTEM_DTAE_MONTH_DOWN_BTN:
		{
			if(m_tTime.nMonth <= 1)
			{
				m_tTime.nMonth = 12;
			}
			else
			{
				m_tTime.nMonth--;
			}
			int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
			if(m_tTime.nDay >  nDayMax)	
			{
				m_tTime.nDay = nDayMax;
			}		
		}
		break;
		case UIID_SYSTEM_DTAE_DAY_UP_BTN:
		{
			int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
			if(nDayMax <= m_tTime.nDay)
			{
				m_tTime.nDay = 1;
			}
			else
			{
				m_tTime.nDay++;
			}
		}
		break;
		case UIID_SYSTEM_DTAE_DAY_DOWN_BTN:
		{
			int nDayMax = GetMaxDays(m_tTime.nYear, m_tTime.nMonth);
			if(1 == m_tTime.nDay)
			{
				m_tTime.nDay = nDayMax;
			}
			else
			{
				m_tTime.nDay--;
			}
		}
		break;
		default:
		break;
	}

	wchar_t BufYear[TEXT_BUF_LEN] = {0};
	wchar_t BufMonth[TEXT_BUF_LEN] = {0};
	wchar_t BufDay[TEXT_BUF_LEN] = {0};

	swprintf(BufYear, TEXT_BUF_LEN, L"%d", m_tTime.nYear);
	//pUIAnalysis->SetItemText(SETTING_WNDID_SYSTEM, UIID_SYSTEM_DTAE_YEAR_DATA_BTN, BufYear);
	swprintf(BufMonth, TEXT_BUF_LEN, L"%d", m_tTime.nMonth);
	//pUIAnalysis->SetItemText(SETTING_WNDID_SYSTEM, UIID_SYSTEM_DTAE_MONTH_DATA_BTN, BufMonth);
	swprintf(BufDay, TEXT_BUF_LEN, L"%d", m_tTime.nDay);
	//pUIAnalysis->SetItemText(SETTING_WNDID_SYSTEM, UIID_SYSTEM_DTAE_DAY_DATA_BTN, BufDay);

}
int SettingView::GetMaxDays(int nYear, int nMonth)
{
	int nDays = 0;
	if((nMonth < 1)||(nMonth > 12))
	{
		LOGERR("SettingView::GetMaxDays ,parameter error: nMonth:%d\n",nMonth);
	}
	switch(nMonth)
	{
       	case 2:
       	{
			int remainder4   = nYear % 4;
			int remainder100 = nYear % 100;
			int remainder400 = nYear % 400;
			if ((( 0 == remainder4) && (0 != remainder100)) || (0 == remainder400))
			{
				nDays = 29;
			}
			else
			{
				nDays = 28;
			}
       	}
       	break;
       	case 4:
       	case 6:
       	case 9:
       	case 11:
		{
			nDays = 30;
		}
       	break;
       	default:   //
		{
			nDays = 31;
		}
       	break;
    }
	return nDays;
}

void SettingView::HandleTimeSet(int CtrlId)
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	switch(CtrlId)
	{
		case UIID_SYSTEM_TIME_HOURS_UP_BTN:
		{
			if(m_tTime.nHours >= 23)
			{
				m_tTime.nHours = 0;
			}
			else
			{
				m_tTime.nHours++;
			}
		}
		break;
		case UIID_SYSTEM_TIME_HOURS_DOWN_BTN:
		{
			if(m_tTime.nHours <= 0)
			{
				m_tTime.nHours = 23;
			}
			else
			{
				m_tTime.nHours--;
			}
		}
		break;
		case UIID_SYSTEM_TIME_MINUTES_UP_BTN:
		{
			if(m_tTime.nMinutes >= 59)
			{
				m_tTime.nMinutes = 0;
			}
			else
			{
				m_tTime.nMinutes++;
			}
		}
		break;
		case UIID_SYSTEM_TIME_MINUTES_DOWN_BTN:
		{
			if(m_tTime.nMinutes <= 0)
			{
				m_tTime.nMinutes = 59;
			}
			else
			{
				m_tTime.nMinutes--;
			}
		}
		break;
		default:
			break;
	}

	wchar_t BufHours[TEXT_BUF_LEN] = {0};
	wchar_t BufMinutes[TEXT_BUF_LEN] = {0};

	swprintf(BufHours, TEXT_BUF_LEN, L"%d", m_tTime.nHours);
	//pUIAnalysis->SetItemText(SETTING_WNDID_SYSTEM, UIID_SYSTEM_TIME_HOURS_DATA_BTN, BufHours);
	swprintf(BufMinutes, TEXT_BUF_LEN, L"%d", m_tTime.nMinutes);
	//pUIAnalysis->SetItemText(SETTING_WNDID_SYSTEM, UIID_SYSTEM_TIME_MINUTES_DATA_BTN, BufMinutes);	

}

void SettingView::UpdateVersionGroup()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	int nCurwnd = GetSetCurrWnd();
	LOGDBG("jgwen--- nCurwnd = %d \n", nCurwnd);
	if(nCurwnd == UIID_WNDID_SETTING)
	{
		SettingData &mData = SettingData::GetInstance();
		T_UI_NODE mNode(UIID_WNDID_SETTING, MENU_SYSTEM_2, UIID_SYSTEM_VER_GROUP);
		SetCurSysNode(mNode);

		wchar_t wszBufHard[TEXT_BUF_LEN] = {0};
		wchar_t wszBufMcu[TEXT_BUF_LEN] = {0};
		wchar_t wszBufMpu[TEXT_BUF_LEN] = {0};

		string strMPUver = "";
		mData.GetMPUVersion(strMPUver);
		swprintf(wszBufMpu, TEXT_BUF_LEN, L"%s", strMPUver.c_str());

		string strMCUver = "";
		mData.GetMCUVersion(strMCUver);
		swprintf(wszBufMcu, TEXT_BUF_LEN, L"%s", strMCUver.c_str());

		string strHARDVer = "";
		mData.GetHARDVersion(strHARDVer);
		swprintf(wszBufHard, TEXT_BUF_LEN, L"%s", strHARDVer.c_str());

		LOGDBG("jgwen--- MPU =%s, MCU = %s, HARD = %s \n", strMPUver.c_str(), strMCUver.c_str(), strHARDVer.c_str());

		pUIAnalysis->SetItemText(nCurwnd, UIID_SYSTEM_VER_CHECKBOX_1_BTN, wszBufHard);
		pUIAnalysis->ShowItem(nCurwnd, UIID_SYSTEM_VER_CHECKBOX_1_BTN, true);
	}
}
void SettingView::UpdateScreenVerPag()
{
	CUIWndMgrWrapper *pUIMgr = NULL; 
	pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (NULL == pUIMgr)
	{
		LOGERR("pUIMgr is NULL");
		return;
	}
	CUIAnalysis* pUIAnalysis = pUIMgr->GetUIAnalysisObj();
	if (NULL == pUIAnalysis)
	{
		LOGERR("pUIAnalysis is NULL");
		return;
	}

	int nCurwnd = GetSetCurrWnd();
	LOGDBG("jgwen--- nCurwnd = %d \n", nCurwnd);
	if(nCurwnd == UIID_WNDID_SETTING)
	{
		SettingData &mData = SettingData::GetInstance();
		T_UI_NODE mNode(UIID_WNDID_SETTING, MENU_SYSTEM_3, UIID_SYSTEM_SCREEN_GROUP);
		SetCurSysNode(mNode);
	
		pUIAnalysis->ShowItem(nCurwnd, 5, true);
		pUIAnalysis->ShowItem(nCurwnd, 6, true);
		pUIAnalysis->ShowItem(nCurwnd, 1, true);
		pUIAnalysis->ShowItem(nCurwnd, 3, true);
		pUIAnalysis->ShowItem(nCurwnd, 7, false);
		pUIAnalysis->ShowItem(nCurwnd, 8, false);
		pUIAnalysis->ShowItem(nCurwnd, 330, true);
		pUIAnalysis->ShowItem(nCurwnd, 9, false);
		pUIAnalysis->ShowItem(nCurwnd, 430, false);
		pUIAnalysis->ShowItem(nCurwnd, 10, false);
		pUIAnalysis->ShowItem(nCurwnd, 4, false);
		pUIAnalysis->ShowItem(nCurwnd, 452, false);		
		pUIAnalysis->SetUseCursor(nCurwnd, 412, true);
		wchar_t wszBufHw[TEXT_BUF_LEN] = {0};
		wchar_t wszBufSotf[TEXT_BUF_LEN] = {0};
		wchar_t wszBufTp[TEXT_BUF_LEN] = {0};

		string strHwVer = "";
		mData.GetScreenHwVer(strHwVer);
		swprintf(wszBufHw, TEXT_BUF_LEN, L"%s", strHwVer.c_str());

		string strSoftVer = "";
		mData.GetScreenSoftVer(strSoftVer);
		swprintf(wszBufSotf, TEXT_BUF_LEN, L"%s", strSoftVer.c_str());

		string strTpVer = "";
		mData.GetScreenTpVer(strTpVer);
		swprintf(wszBufTp, TEXT_BUF_LEN, L"%s", strTpVer.c_str());

		LOGDBG("jgwen--- HW =%s, Sotf = %s, Tp = %s \n", strHwVer.c_str(), strSoftVer.c_str(), strTpVer.c_str());

		pUIAnalysis->SetItemText(nCurwnd, UIID_SYSTEM_SCREEN_CHECKBOX_1_BTN, wszBufSotf);
		pUIAnalysis->ShowItem(nCurwnd, UIID_SYSTEM_SCREEN_CHECKBOX_1_BTN, true);

		pUIAnalysis->SetItemText(nCurwnd, UIID_SYSTEM_SCREEN_CHECKBOX_2_BTN, wszBufHw);
		pUIAnalysis->ShowItem(nCurwnd, UIID_SYSTEM_SCREEN_CHECKBOX_2_BTN, true);

		pUIAnalysis->SetItemText(nCurwnd, UIID_SYSTEM_SCREEN_CHECKBOX_3_BTN, wszBufTp);
		pUIAnalysis->ShowItem(nCurwnd, UIID_SYSTEM_SCREEN_CHECKBOX_3_BTN, true);

	}	
}
	

bool SettingView::SetMedioVolume(int nVolume)
{
	bool bRet = false;
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("jgwen--- nullptr == pUIMgr Error!!");
		return bRet;
	}

	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetMedioVolume(nVolume);
	}
	return bRet;
}
bool SettingView::SetBtVolume(int nVolume)
{
	bool bRet = false;
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("jgwen--- nullptr == pUIMgr Error!!");
		return bRet;
	}

	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetBtVolume(nVolume);
	}
	return bRet;
}
bool SettingView::SetMute(bool nValue, int mediovol, int btvol)
{
	bool bRet = false;
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("jgwen--- nullptr == pUIMgr Error!!");
		return bRet;
	}
	int nMute = -1;
	if(nValue)
	{
		nMute = 1;
	}
	else
	{
		nMute = 0;
	}

	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetMute(nMute, mediovol, btvol);
	}
	return bRet;
}
bool SettingView::SetKeyMute(bool nKeyValue)
{
	bool bRet = false;
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("jgwen--- nullptr == pUIMgr Error!!");
		return bRet;
	}
	int nkeyMute = -1;
	if(nKeyValue)
	{
		nkeyMute = 1;
	}
	else
	{
		nkeyMute = 0;
	}

	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetKeyMute(nkeyMute);
	}
	return bRet;
}

bool SettingView::SetbackLight(int nbackLight)
{
	LOGDBG("jgwen--- SetbackLight");
	bool bRet = false;

	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetBackLight(nbackLight);
		LOGDBG("jgwen--- 1");
	}
	else
	{
		LOGDBG("jgwen--- 2");
	}
	return bRet;
}

bool SettingView::SetbackLightMode(int Light, bool bAutoBg)
{
	bool bRet = false;

	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetbackLightMode(Light, bAutoBg);
	}

	return bRet;
}

bool SettingView::SetBluetooth(int nBtState)
{
	bool bRet = false;

	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetBluetooth(nBtState);
	}
	return bRet;
}

bool SettingView::SetUnitMode(int nUnitMode)
{
	bool bRet = false;
	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetUnitMode(nUnitMode);
	}
	return bRet;
}

bool SettingView::SetReadLight(int nState)
{
	bool bRet = false;
	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetReadLight(nState);
	}
	return bRet;
}

bool SettingView::SetSystemInfo(T_APP_MSG* msg)
{
	bool bRet = false;
	if(NULL == msg)
	{
		return bRet;
	}

	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return bRet;
	}

	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetSystemInfo();
	}
	return bRet;
}
bool SettingView::SetLanguage(int nlanguage)
{
	bool bRet = false;
	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetLanguage(nlanguage);
	}
	return bRet;
}

bool SettingView::SetSyncTime(int state)
{
	bool bRet = false;
	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetSyncTime(state);
	}
	return bRet;
}

bool SettingView::SetCurTime(T_STR_TIME_INFO ntTime)
{
	bool bRet = false;
	if(NULL != m_ViewController)
	{
		bRet = m_ViewController->SetCurTime(ntTime);
	}
	return bRet;
}

void SettingView::DrawAllItem()
{
	CUIWndMgrWrapper *pUIMgr = GetViewBase()->GetUIWndMgrWrapper();
	if (nullptr == pUIMgr)
	{
		LOGDBG("nullptr == pUIMgr Error!!");
		return;
	}
	RECT rect;
	SetRect(&rect, 0, 0, 1920, 720);
	pUIMgr->drawAllItem(rect);
}

void SettingView::SetSetCurrWnd(int nWndID)
{
	Lock();
	m_nSetCurrWnd = nWndID;
	SettingData &mData = SettingData::GetInstance();
	mData.SetCurrWnd(nWndID);
	UnLock();
}

int SettingView::GetSetCurrWnd()
{
	int nWndID  = 0;
	Lock();
	nWndID = m_nSetCurrWnd;
	UnLock();
	return nWndID;
}

void SettingView::SetCurSysNode(T_UI_NODE nNode)
{
	Lock();
	m_CurrNode = nNode;
	UnLock();
}

void SettingView::GetCurSysNode(T_UI_NODE &nNode)
{
	Lock();
	nNode = m_CurrNode;
	UnLock();
}

void SettingView::SaveCurClickLine(const int nLine)
{
	Lock();
	m_Line = nLine;
	UnLock();
}

void SettingView::GetCurClickLine(int &nLine)
{
	Lock();
	nLine = m_Line;
	UnLock();
}

void SettingView::SaveCurInWnd(int nWnd)
{
	m_nInWnd = nWnd;
}

void SettingView::ConverDecode(char *pInBuf,char *pOutBuf,size_t& nOutLen)
{
	const char *encTo = "UCS-4LE";
	const char *encFrom = "UTF-8";
	iconv_t cd = iconv_open(encTo, encFrom);

	if(cd == (iconv_t)-1)
	{
		perror("iconv_open");
	}
	size_t srclen = strlen(pInBuf);

	LOGDBG("-----------srclen = %d\n",srclen);

	char* srcstart = pInBuf;
	char* tmpoutbuf = pOutBuf;

	size_t ret = iconv(cd, &srcstart, &srclen, &tmpoutbuf, &nOutLen);

	if(ret == -1)
	{
		switch(errno)
		{
			case E2BIG:
				LOGDBG("-------------E2BIG\n");
			break;
			case EILSEQ:
				LOGDBG("-------------EILSEQ\n");
			break;
			case EINVAL:
				LOGDBG("-------------EINVAL\n");
			break;
 		}
		perror("iconv");
	}
	
	LOGDBG("inbuf = %s , srclen = %d , output = %s , outlen = %d\n",pInBuf,srclen,pOutBuf,nOutLen);

	iconv_close(cd);
}
