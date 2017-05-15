#ifndef __AXERROR_H__
#define __AXERROR_H__

#include "PeriodicProcess.h"
#include "AX12.h"

class AxError : public PeriodicProcess{

private : 
	bool m_ok;
	AX12 m_ax;
	int m_CCWmax;
	int m_CWmax;

public :

	AxError(AX12 &ax);
	bool getStatus(){return m_ok;};
	void setStatus(bool stat){m_ok = stat;};


protected:
	
	virtual void process(float timestep);
};

#endif