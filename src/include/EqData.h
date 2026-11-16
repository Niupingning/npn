/*
 * EqData.h
 *
 *  Created on: 2021-08-03
 *      Author: Y
 */
 
#ifndef EQDATA_H_
#define EQDATA_H_

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>

#include "FileBase.h"
#include "AppDatadef.h"
#include "log.h"

using FY_NAMESPACE::MutexLock;
using namespace std;

class EqData 
{
public:
	EqData();
	virtual ~EqData();

	static EqData &GetInstance()
	{
   	 	static EqData instance;
    	return instance;
	}

	void SaveLoudness(const long loudness);
	void GetLoudness(long  &loudness);
	void SaveSubwoofer(const long subwoofer);
	void GetSubwoofer(long  &subwoofer);
	void GetLoundnessButs(map<long, long> &LoundnessButs);


private:
	void Lock(){pthread_mutex_lock( &m_Lock );}
	void UnLock(){pthread_mutex_unlock( &m_Lock );}

private:
	pthread_mutex_t m_Lock;
    map<long, long> m_nLoundnessButs;
	long m_nLoudness;
	long m_nSubwoofer;
};


#endif /* EQDATA_H_ */
