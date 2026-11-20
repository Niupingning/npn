/*
 * HomeData.h
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */
 
#ifndef MEDIADATA_H_
#define MEDIADATA_H_

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>

#include "FileBase.h"
#include "AppDatadef.h"
#include "log.h"
#include "SPPlatformControl.h"
using namespace spplfctrl;
using FY_NAMESPACE::MutexLock;
using namespace std;

enum PlayerState
{
	Player_Idle = 0,
	Player_Initialized,
	Player_Stop,
	Player_Preparing,
	Player_Prepared,
	Player_Play,
	Player_FF,
	Player_FB,
	Player_SF,
	Player_SB,
	Player_Pause,
	Player_Error,
	Player_Finish
};

enum
{
    PLAY_CMD_IDLE  = 0,
    PLAY_CMD_PLAY  = 1,
    PLAY_CMD_PAUSE = 2,
    PLAY_CMD_SEEK  = 3,
    PLAY_CMD_FAST  = 4,
    PLAY_CMD_LOOP  = 5,
    PLAY_CMD_STOP  = 6,
};

enum
{
    PLAY_MODE_NORMAL  = 0,   //列表循环
    PLAY_MODE_ONE     = 1,   //单曲循环
    PLAY_MODE_FOLDER  = 2,   //文件夹循环
    PLAY_MODE_SHUFFLE = 3,   //随机播放
};

enum
{
    TIMER_SEARCH_FILES  = 100,
    TIMER_UPDATE_TIME   = 101,
	TIMER_PLAYFAIL_NEXT = 102,
};

enum
{
    UI_FROM_IDLE        = 0,
    UI_FROM_LOADING     = 1,
    UI_FROM_AUDIO_LIST  = 2,
    UI_FROM_AUDIO_PLAY  = 3,
    UI_FROM_VEDIO_LIST  = 4,
    UI_FROM_VIDEO_PLAY  = 5,
    UI_FROM_NO_FILE     = 6,
    UI_FROM_NO_DEVICE   = 7,
};

#define MAX_FILE_LEN   128

typedef struct
{
	int nParam;
	int nPlayType;
	char strPlayUrl[MAX_FILE_LEN];
}T_PLAYMEDIA_URL;

typedef struct
{
	int nPlayCmd;
	int nParam;
}T_PLAYMEDIA_CMD;

enum
{
	PLAY_TYPE_IDLE  = 0,
	PLAY_TYPE_AUDIO = 1,
	PLAY_TYPE_VIDEO = 2,
};

class MediaData 
{
public:
	MediaData();
	virtual ~MediaData();

	static MediaData &GetInstance()
	{
   	 	static MediaData instance;
    	return instance;
	}

	void InsertPacket(const T_STR_DATA_PACKET tPacket);
	void GetPopPacket(T_STR_DATA_PACKET &tPacket);
	int GetVecPacket_Size();
	void WaitForPacket();


	void SetCurrWnd(int nWndID);
	int GetCurrWnd();

	void SetLastWnd(int nWndID);
	int GetLastWnd();

	void SetMountPath(const char *pMount,int nLen);
	void GetMountPath(char *pMount,int nLen);

    void SetCurPlayUrl(const char *pPlayUrl);
    void GetCurPlayUrl(char *pPlayUrl);

private:
	void Lock(){pthread_mutex_lock( &m_Lock );}
	void UnLock(){pthread_mutex_unlock( &m_Lock );}

private:

	std::vector <T_STR_DATA_PACKET> m_vecDataPacket;

	int m_nCurrWnd;
	int m_nLastWnd;
	pthread_cond_t m_cond;
	pthread_mutex_t m_Lock;

	char m_strMountPath[128];

    char m_strPlayUrl[MAX_FILE_LEN];
};


#endif /* HOMEDATA_H_ */
