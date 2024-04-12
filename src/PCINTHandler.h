
#ifndef PCINTHandler_H
#define PCINTHandler_H

#include "Extensions.h"

#define abstract


Interface PCINTHandler{
	virtual ~PCINTHandler(){}
	virtual void intCheck() =0;
};

#endif
