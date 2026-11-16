/*
 * CompBase.h
 *
 *  Created on: 2021-08-03
 *      Author: Y3553
 */

#ifndef COMPBASE_H_
#define COMPBASE_H_

#include "ViewBase.h"

using FY_NAMESPACE::ViewBase;

class CompBase {
public:
	CompBase(ViewBase* p_ViewBase);
	virtual ~CompBase();

	ViewBase* GetViewBase();
private:
	ViewBase *m_pViewBase;
};

#endif /* COMPBASE_H_ */
