/*
 * AirConditionData.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include "AirConditionData.h"
#include "SourceDef.h"

AirConditionData::AirConditionData() : m_nLeftTemperature(26)
, m_nRightTemperature(26)
{
    pthread_mutex_init( &m_Lock, NULL );
}

AirConditionData::~AirConditionData()
{
    pthread_mutex_destroy( &m_Lock );
}

void AirConditionData::SaveLeftTemperature(const long temp)
{
    Lock();
    m_nLeftTemperature = temp;
    UnLock();
}
long AirConditionData::GetLeftTemperature()
{
    Lock();
    long temp = m_nLeftTemperature;
    UnLock();
    return temp;
}

void AirConditionData::SaveRightTemperature(const long temp)
{
    Lock();
    m_nRightTemperature = temp;
    UnLock();
}
void AirConditionData::GetRightTemperature(long &temp)
{
    Lock();
    temp = m_nRightTemperature;
    UnLock();
}



