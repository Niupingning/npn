/*
 * AirConditionData.h
 *
 *  Created on: 2021-08-03
 *      Author: Y
 */
 
#ifndef AirConditionData_H_
#define AirConditionData_H_

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

class AirConditionData 
{
public:
	AirConditionData();
	virtual ~AirConditionData();

	static AirConditionData &GetInstance()
	{
   	 	static AirConditionData instance;
    	return instance;
	}

	void SaveLeftTemperature(const long temp);
	long GetLeftTemperature();
	void SaveRightTemperature(const long temp);
	void GetRightTemperature(long  &temp);

private:
	void Lock(){pthread_mutex_lock( &m_Lock );}
	void UnLock(){pthread_mutex_unlock( &m_Lock );}

private:
	pthread_mutex_t m_Lock;
	long  m_nLeftTemperature;
	long  m_nRightTemperature;
};


#endif /* AirConditionData_H_ */
