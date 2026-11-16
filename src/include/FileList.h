#pragma once

#include <vector>
#include <string>


#define SUPPORT_FILE_MEDIA_NUM_MAX		10000
#define MAX_FILE_PATH 1024
//#define MAX_FILE_LEN  256
#define BINDER_FILE_LEN  128

typedef enum
{
	AUDIO_FILE_TYPE = 0x0,
	PHOTO_FILE_TYPE,
	VIDEO_FILE_TYPE,
	FOLDER_FILE_TYPE,
	OTHER_FILE_TYPE
}FILE_TYPE;

typedef enum
{
	FILE_SYSTEM_NONE = 0x0,
	FILE_SYSTEM_LOADING,
	FILE_SYSTEM_COMPLETE,
	FILE_SYSTEM_ERROR
}FILE_SYSTEM_STATE;

class Element
{
public:
	Element();
	~Element();
	void PathFindFileName(char* pFileName);

public:
	char m_sFilePath[MAX_FILE_PATH];
	FILE_TYPE m_nFileType;
	int m_nLevel;   //根据等级排序
};

class xElementSorter
{
public:
	// Return whether first element is less than the second   
	bool operator () (const Element& a,const Element& b) const
	{
		return (a.m_nLevel < b.m_nLevel);
	};
};

class CFileList
{
public:
	CFileList();
	~CFileList();

	void Search_AllDirectory(const char* pDirName);
	void Search_Folder(const char* pDirName);

	FILE_TYPE JudgeFileType(char* pFileName);
	void EnumFolder(const char* pDirName);

	int GetAudioFileNum();
	int GetVideoFileNum();
	int GetPhotoFileNum();
	int GetDirectoryNum();

	void DelFileList();
	void DelFolderList();

	int GetPlayNext(const char *pPlayFile,int nPlayType,int nPlayMode,Element& Ele);
	int GetPlayPrev(const char *pPlayFile,int nPlayType,int nPlayMode,Element& Ele);

	int ID3Parser(const char* url);

	int DemoParser(const char* url,const char* filePath);

	void OnGetMediaID3_Info(const char* url,Element& ment);
	void OnGetMediaID3_PIC(const char* url);
	void GetAudioVec_ID3(const char* url,char *pArtist,char *pFileName);

	bool OnCheck_PlayFile(const char* url,int nPlayType);
	int GetPlayFile_Index(const char* url,int nPlayType);

public:

	std::vector<Element> m_DirList;
	std::vector<Element> m_AudioFileList;
	std::vector<Element> m_VideoFileList;
	std::vector<Element> m_PhotoFileList;

	std::string m_strFoldPath;
	std::string m_ppAudioFixArr[26];
	std::string m_ppVideoFixArr[26];
	std::string m_ppPhotoFixArr[16];
	int m_nAudioFixArr;
	int m_nVideoFixArr;
	int m_nPhotoFixArr;

	//char m_strArtist[MAX_FILE_LEN];
	//char m_strAlbum[MAX_FILE_LEN];
	//char m_strTitle[MAX_FILE_LEN];

};


