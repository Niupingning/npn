/*
 * EqData.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include "EqData.h"
#include "SourceDef.h"
#include "UIID_Def_AudioBoost.h"

EqData::EqData() : m_nLoudness(608)
, m_nSubwoofer(608)
{
    m_nLoundnessButs.clear();
	m_nLoundnessButs.insert(make_pair(UIID_GROUP_LOUDNES_OFF, 608));
    m_nLoundnessButs.insert(make_pair(UIID_GROUP_LOUDNES_LOW, 610));
    m_nLoundnessButs.insert(make_pair(UIID_GROUP_LOUDNES_MID, 611));
    m_nLoundnessButs.insert(make_pair(UIID_GROUP_LOUDNES_HIGH, 612));

    pthread_mutex_init( &m_Lock, NULL );
}

EqData::~EqData()
{
    pthread_mutex_destroy( &m_Lock );
}

void EqData::SaveLoudness(const long loudness)
{
    Lock();
    m_nLoudness = loudness;
    UnLock();
}
void EqData::GetLoudness(long &loudness)
{
    Lock();
    loudness = m_nLoudness;
    UnLock();
}

void EqData::SaveSubwoofer(const long subwoofer)
{
    Lock();
    m_nSubwoofer = subwoofer;
    UnLock();
}
void EqData::GetSubwoofer(long  &subwoofer)
{
    Lock();
    subwoofer = m_nSubwoofer;
    UnLock();
}


void EqData::GetLoundnessButs(map<long, long> &LoundnessButs)
{
    Lock();
    LoundnessButs = m_nLoundnessButs;
    UnLock();
}

