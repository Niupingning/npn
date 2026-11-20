#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>

#include "FileList.h"

#include "MediaData.h"

/* 判断两个文件是否在同一个目录下 */
static bool IsSameDirectory(const char *pFileA, const char *pFileB)
{
	if (NULL == pFileA || NULL == pFileB)
	{
		return false;
	}

	const char *pA = strrchr(pFileA, '/');
	const char *pB = strrchr(pFileB, '/');
	if (NULL == pA || NULL == pB)
	{
		return false;
	}

	size_t nLenA = (size_t)(pA - pFileA);
	size_t nLenB = (size_t)(pB - pFileB);
	if (nLenA != nLenB)
	{
		return false;
	}

	return (0 == strncmp(pFileA, pFileB, nLenA));
}

/* 文件夹循环：在当前文件所在目录内按 nDir(1:下一首 -1:上一首) 查找，找不到则回到当前文件 */
static int FindFolderPlay(const std::vector<Element> &lst, const char *pCurFile, int nDir)
{
	int nSum = (int)lst.size();
	if (nSum <= 0)
	{
		return -1;
	}

	int nCur = -1;
	for (int i = 0; i < nSum; i++)
	{
		if (0 == strcmp(pCurFile, lst[i].m_sFilePath))
		{
			nCur = i;
			break;
		}
	}
	if (nCur < 0)
	{
		return -1;
	}

	for (int nOff = 1; nOff < nSum; nOff++)
	{
		int nIdx = (nCur + nDir * nOff) % nSum;
		if (nIdx < 0)
		{
			nIdx += nSum;
		}
		if (IsSameDirectory(pCurFile, lst[nIdx].m_sFilePath))
		{
			return nIdx;
		}
	}

	// 文件夹内只有当前文件：重复播放当前文件
	return nCur;
}


Element::Element(void)
{
	memset(m_sFilePath, 0, sizeof(m_sFilePath));
	m_nFileType = OTHER_FILE_TYPE;

	m_nLevel = 0;
}
Element::~Element()
{

}


void Element::PathFindFileName(char* pFileName)
{
	std::string str = m_sFilePath;
	int nlen = strlen(m_sFilePath);
	char ch = '/';
	size_t pos = str.rfind(ch);


	if (pos != std::string::npos)
	{
		if(pos < nlen)
		{
			strcpy(pFileName,m_sFilePath+pos+1);
		}
	}
}


CFileList::CFileList()
{
	int nArr = 0;
	m_ppAudioFixArr[nArr++] = "mp3";
	m_ppAudioFixArr[nArr++] = "wav";

	m_nAudioFixArr = nArr;
    
	nArr = 0;
	m_ppVideoFixArr[nArr++] = "avi";
	m_ppVideoFixArr[nArr++] = "divx";
	m_ppVideoFixArr[nArr++] = "mp4";
	m_ppVideoFixArr[nArr++] = "VID";
	m_ppVideoFixArr[nArr++] = "ASF";
	m_ppVideoFixArr[nArr++] = "WMV";
	m_ppVideoFixArr[nArr++] = "MPG";
	m_ppVideoFixArr[nArr++] = "MPEG";
	m_ppVideoFixArr[nArr++] = "MP4";
	m_ppVideoFixArr[nArr++] = "3G2";
	m_ppVideoFixArr[nArr++] = "mpg";

	m_nVideoFixArr = nArr;

	nArr = 0;
	m_ppPhotoFixArr[nArr++] = "jpg";
	m_ppPhotoFixArr[nArr++] = "jpeg";
	m_ppPhotoFixArr[nArr++] = "bmp";
	m_ppPhotoFixArr[nArr++] = "png";
	m_nPhotoFixArr = nArr;

	m_DirList.clear();
	m_AudioFileList.clear();
	m_VideoFileList.clear();
	m_PhotoFileList.clear();

}

CFileList::~CFileList()
{
	m_DirList.clear();
	m_AudioFileList.clear();
	m_VideoFileList.clear();
	m_PhotoFileList.clear();
}

void CFileList::Search_AllDirectory(const char* pDirName)
{
	struct dirent *entry;
    DIR *dir = opendir(pDirName);
    struct stat file_stat;
    
    if (dir == NULL) 
	{
        LOGERR("&&&---opendir--\n");
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) 
	{
        char full_path[MAX_FILE_PATH] = {0};
        snprintf(full_path, sizeof(full_path), "%s/%s", pDirName, entry->d_name);
		//printf("&&&---full_path:%s\n",full_path);
        
        if (stat(full_path, &file_stat) == -1) 
		{
            printf("&&&---stat---\n");
            continue;
        }
        
        if (S_ISDIR(file_stat.st_mode) && entry->d_name[0] != '.') 
		{
			//printf("&&&---d_name:%s\n",entry->d_name); 
			// 如果是目录且不是隐藏目录，则递归搜索
			Search_AllDirectory(full_path);
        }
		else if (S_ISREG(file_stat.st_mode))
		{ 
			// 如果是文件，则打印出来
			if (AUDIO_FILE_TYPE == JudgeFileType(full_path))
			{
				//文件太长过滤
				if(strlen(full_path) < BINDER_FILE_LEN)
				{
					LOGDBG("&&&---audio--file_path:%s,len:%d\n", full_path,strlen(full_path));

					Element ment;
					memcpy(ment.m_sFilePath,full_path,sizeof(full_path));
					ment.m_nFileType = AUDIO_FILE_TYPE;

					m_AudioFileList.push_back(ment);
				}
			}
			else if (VIDEO_FILE_TYPE == JudgeFileType(full_path))
			{
				//文件太长过滤
				if(strlen(full_path) < BINDER_FILE_LEN)
				{
					LOGDBG("&&&---video--file_path:%s,len:%d\n", full_path,strlen(full_path));

					Element ment;
					memcpy(ment.m_sFilePath,full_path,sizeof(full_path));
					ment.m_nFileType = VIDEO_FILE_TYPE;

					m_VideoFileList.push_back(ment);
				}
			}
			/*else if (PHOTO_FILE_TYPE == JudgeFileType(full_path))
			{
				Element ment;
				memcpy(ment.m_sFilePath,full_path,sizeof(full_path));
				ment.m_nFileType = PHOTO_FILE_TYPE;

				m_PhotoFileList.push_back(ment);
			}*/
        }
    }
    
    closedir(dir);
}
void CFileList::Search_Folder(const char* pDirName)
{
	struct dirent *entry;
    DIR *dir = opendir(pDirName);
    struct stat file_stat;
    
    if (dir == NULL) 
	{
        LOGERR("&&&---opendir--\n");
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) 
	{
        char full_path[MAX_FILE_PATH] = {0};
        snprintf(full_path, sizeof(full_path), "%s/%s", pDirName, entry->d_name);
        
        if (stat(full_path, &file_stat) == -1) 
		{
            LOGERR("&&&---stat---\n");
            continue;
        }
        
        if (S_ISDIR(file_stat.st_mode) && entry->d_name[0] != '.') 
		{ 
			// 目录
			Element ment;
			memcpy(ment.m_sFilePath,full_path,sizeof(full_path));
			ment.m_nFileType = FOLDER_FILE_TYPE;
			ment.m_nLevel = 0;

			m_DirList.push_back(ment);
        }
		else if (S_ISREG(file_stat.st_mode))
		{ 
			// 如果是文件，则打印出来
            //printf("&&&---Search_Folder--full_path:%s\n", full_path);
			if (VIDEO_FILE_TYPE == JudgeFileType(full_path))
			{
				Element ment;
				memcpy(ment.m_sFilePath,full_path,sizeof(full_path));
				ment.m_nFileType = VIDEO_FILE_TYPE;
				ment.m_nLevel = 1;

				m_DirList.push_back(ment);
			}
			else if (PHOTO_FILE_TYPE == JudgeFileType(full_path))
			{
				Element ment;
				memcpy(ment.m_sFilePath,full_path,sizeof(full_path));
				ment.m_nFileType = PHOTO_FILE_TYPE;
				ment.m_nLevel = 2;

				m_DirList.push_back(ment);
			}
        }
    }
    
    closedir(dir);
}
FILE_TYPE CFileList::JudgeFileType(char* pFileName)
{
	bool bFind = false;
	std::string str = pFileName;
	char ch = '.';
	size_t pos = str.rfind(ch);
	if (pos != std::string::npos)
	{
		bFind = true;
	}

	if(bFind)
	{
		//printf("&&&---len:%d\n",strlen(pFileName));
		if(pos >= strlen(pFileName))
		{
			return OTHER_FILE_TYPE;
		}
		//printf("&&&---file:%s\n",pFileName+pos+1);

		for (int i = 0; i < m_nAudioFixArr; i++)
		{
			if (0 == strcmp(pFileName+pos+1, m_ppAudioFixArr[i].c_str()))
			{
				return AUDIO_FILE_TYPE;
			}
		}

		for (int i = 0; i < m_nVideoFixArr; i++)
		{
			if (0 == strcmp(pFileName+pos+1, m_ppVideoFixArr[i].c_str()))
			{
				return VIDEO_FILE_TYPE;
			}
		}

		for (int i = 0; i < m_nPhotoFixArr; i++)
		{
			if (0 == strcmp(pFileName+pos+1, m_ppPhotoFixArr[i].c_str()))
			{
				return PHOTO_FILE_TYPE;
			}
		}
	}

	return OTHER_FILE_TYPE;
}
void CFileList::EnumFolder(const char* pDirName)
{
	m_strFoldPath = pDirName;
	Search_Folder(pDirName);

	//排序
	std::sort(m_DirList.begin(), m_DirList.end(), xElementSorter());
}

void CFileList::DelFileList()
{
	m_AudioFileList.clear();
	m_VideoFileList.clear();
	m_PhotoFileList.clear();
}

void CFileList::DelFolderList()
{
	m_DirList.clear();
}

int CFileList::GetPlayNext(const char *pPlayFile,int nPlayType,int nPlayMode,Element& Ele)
{
	int nNextPlay = 0;
	int nSum = 0;
	int nRet = -1;

	if(PLAY_TYPE_AUDIO == nPlayType)
	{
		nSum = GetAudioFileNum();
		if(nSum <= 0)
		{
			return -1;
		}
		if(PLAY_MODE_NORMAL == nPlayMode)
		{
			for(int i = 0;i<nSum;i++)
			{
				if(0 == strcmp(pPlayFile,m_AudioFileList[i].m_sFilePath))
				{
					nNextPlay = i+1;
					if(nNextPlay >= nSum)
					{
						nNextPlay = 0;
					}
					Ele = m_AudioFileList[nNextPlay];
					nRet = nNextPlay;
					break;
				}
			}
		}
		else if(PLAY_MODE_SHUFFLE == nPlayMode)
		{
			nNextPlay = rand() % nSum;
			Ele = m_AudioFileList[nNextPlay];

			nRet = nNextPlay;
		}
		else if(PLAY_MODE_FOLDER == nPlayMode)
		{
			nNextPlay = FindFolderPlay(m_AudioFileList, pPlayFile, 1);
			if(nNextPlay >= 0)
			{
				Ele = m_AudioFileList[nNextPlay];
				nRet = nNextPlay;
			}
		}
		else if(PLAY_MODE_ONE == nPlayMode)
		{
			for(int i = 0;i<nSum;i++)
			{
				if(0 == strcmp(pPlayFile,m_AudioFileList[i].m_sFilePath))
				{
					nNextPlay = i;
					Ele = m_AudioFileList[nNextPlay];
					nRet = nNextPlay;
					break;
				}
			}
		}
	}
	else if(PLAY_TYPE_VIDEO == nPlayType)
	{
		nSum = GetVideoFileNum();
		if(nSum <= 0)
		{
			return -1;
		}
		if(PLAY_MODE_NORMAL == nPlayMode)
		{
			for(int i = 0;i<nSum;i++)
			{
				if(0 == strcmp(pPlayFile,m_VideoFileList[i].m_sFilePath))
				{
					nNextPlay = i+1;
					if(nNextPlay >= nSum)
					{
						nNextPlay = 0;
					}

					Ele = m_VideoFileList[nNextPlay];
					nRet = nNextPlay;
					break;
				}
			}
		}
		else if(PLAY_MODE_SHUFFLE == nPlayMode)
		{
			nNextPlay = rand() % nSum;
			Ele = m_VideoFileList[nNextPlay];

			nRet = nNextPlay;
		}
		else if(PLAY_MODE_FOLDER == nPlayMode)
		{
			nNextPlay = FindFolderPlay(m_VideoFileList, pPlayFile, 1);
			if(nNextPlay >= 0)
			{
				Ele = m_VideoFileList[nNextPlay];
				nRet = nNextPlay;
			}
		}
		else if(PLAY_MODE_ONE == nPlayMode)
		{
			for(int i = 0;i<nSum;i++)
			{
				if(0 == strcmp(pPlayFile,m_VideoFileList[i].m_sFilePath))
				{
					nNextPlay = i;
					Ele = m_VideoFileList[nNextPlay];
					nRet = nNextPlay;
					break;
				}
			}
		}
	}

	return nRet;
}

int CFileList::GetPlayPrev(const char *pPlayFile,int nPlayType,int nPlayMode,Element& Ele)
{
	int nPrevPlay = 0;
	int nSum = 0;
	int nRet = -1;

	if(PLAY_TYPE_AUDIO == nPlayType)
	{
		nSum = GetAudioFileNum();
		if(nSum <= 0)
		{
			return -1;
		}
		if(PLAY_MODE_NORMAL == nPlayMode)
		{
			for(int i = 0;i<nSum;i++)
			{
				if(0 == strcmp(pPlayFile,m_AudioFileList[i].m_sFilePath))
				{
					nPrevPlay = i - 1;
					if(nPrevPlay < 0)
					{
						nPrevPlay = nSum - 1;
					}
					Ele = m_AudioFileList[nPrevPlay];
					nRet = nPrevPlay;
					break;
				}
			}
		}
		else if(PLAY_MODE_SHUFFLE == nPlayMode)
		{
			nPrevPlay = rand() % nSum;
			Ele = m_AudioFileList[nPrevPlay];

			nRet = nPrevPlay;
		}
		else if(PLAY_MODE_FOLDER == nPlayMode)
		{
			nPrevPlay = FindFolderPlay(m_AudioFileList, pPlayFile, -1);
			if(nPrevPlay >= 0)
			{
				Ele = m_AudioFileList[nPrevPlay];
				nRet = nPrevPlay;
			}
		}
		else if(PLAY_MODE_ONE == nPlayMode)
		{
			for(int i = 0;i<nSum;i++)
			{
				if(0 == strcmp(pPlayFile,m_AudioFileList[i].m_sFilePath))
				{
					nPrevPlay = i;
					Ele = m_AudioFileList[nPrevPlay];
					nRet = nPrevPlay;
					break;
				}
			}
		}
	}
	else if(PLAY_TYPE_VIDEO == nPlayType)
	{
		nSum = GetVideoFileNum();
		if(nSum <= 0)
		{
			return -1;
		}
		if(PLAY_MODE_NORMAL == nPlayMode)
		{
			for(int i = 0;i<nSum;i++)
			{
				if(0 == strcmp(pPlayFile,m_VideoFileList[i].m_sFilePath))
				{
					nPrevPlay = i - 1;

					if(nPrevPlay <= 0)
					{
						nPrevPlay = nSum - 1;
					}
					Ele = m_VideoFileList[nPrevPlay];
					nRet = nPrevPlay;
					break;
				}
			}
		}
		else if(PLAY_MODE_SHUFFLE == nPlayMode)
		{
			nPrevPlay = rand() % nSum;
			Ele = m_VideoFileList[nPrevPlay];

			nRet = nPrevPlay;
		}
		else if(PLAY_MODE_FOLDER == nPlayMode)
		{
			nPrevPlay = FindFolderPlay(m_VideoFileList, pPlayFile, -1);
			if(nPrevPlay >= 0)
			{
				Ele = m_VideoFileList[nPrevPlay];
				nRet = nPrevPlay;
			}
		}
		else if(PLAY_MODE_ONE == nPlayMode)
		{
			for(int i = 0;i<nSum;i++)
			{
				if(0 == strcmp(pPlayFile,m_VideoFileList[i].m_sFilePath))
				{
					nPrevPlay = i;

					if(nPrevPlay <= 0)
					{
						nPrevPlay = nSum - 1;
					}
					Ele = m_VideoFileList[nPrevPlay];
					nRet = nPrevPlay;
					break;
				}
			}
		}
	}

	return nRet;
}

int CFileList::GetAudioFileNum()
{
	return m_AudioFileList.size();
}

int CFileList::GetVideoFileNum()
{
	return m_VideoFileList.size();
}

int CFileList::GetPhotoFileNum()
{
	return m_PhotoFileList.size();
}

int CFileList::GetDirectoryNum()
{
	return m_DirList.size();
}

bool CFileList::OnCheck_PlayFile(const char* url,int nPlayType)
{
	int nSum = 0;
	bool bRet = false;
	if(PLAY_TYPE_AUDIO == nPlayType)
	{
		nSum = GetAudioFileNum();

		for(int i = 0;i<nSum;i++)
		{
			if(0 == strcmp(url,m_AudioFileList[i].m_sFilePath))
			{
				bRet = true;
				break;
			}
		}
	}
	else if(PLAY_TYPE_VIDEO == nPlayType)
	{
		nSum = GetVideoFileNum();

		for(int i = 0;i<nSum;i++)
		{
			if(0 == strcmp(url,m_VideoFileList[i].m_sFilePath))
			{
				bRet = true;
				break;
			}
		}

	}

	return bRet;
}

int CFileList::GetPlayFile_Index(const char* url,int nPlayType)
{
	int nSum = 0;
	int nRet = 0;
	if(PLAY_TYPE_AUDIO == nPlayType)
	{
		nSum = GetAudioFileNum();

		for(int i = 0;i<nSum;i++)
		{
			if(0 == strcmp(url,m_AudioFileList[i].m_sFilePath))
			{
				nRet = i;
				break;
			}
		}
	}
	else if(PLAY_TYPE_VIDEO == nPlayType)
	{
		nSum = GetVideoFileNum();

		for(int i = 0;i<nSum;i++)
		{
			if(0 == strcmp(url,m_VideoFileList[i].m_sFilePath))
			{
				nRet = i;
				break;
			}
		}
	}

	return nRet;
}


