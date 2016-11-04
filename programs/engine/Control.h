#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <Arduino.h>

#include "WheeledBase.h"
#include "Odometry.h"


class Control
{
public:

	Control(WheeledBase& base, Odometry& odometry);

	void step();

private:

	WheeledBase&	m_base;
	Odometry&		m_odometry;
};

#endif //__CONTROL_H__
