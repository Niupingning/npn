/*
 * CompBase.cpp
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#include "CompBase.h"


CompBase::CompBase(ViewBase* p_ViewBase):
m_pViewBase(p_ViewBase)
{
	// TODO Auto-generated constructor stub

}

CompBase::~CompBase() {
	// TODO Auto-generated destructor stub
}

ViewBase* CompBase::GetViewBase()
{
	return m_pViewBase;
}
