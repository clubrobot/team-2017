#include "Control.h"


Control::Control(WheeledBase& base, Odometry& odometry)
:	m_base(base)
,	m_odometry(odometry)
{

}

void Control::step()
{
	const State&	s = m_odometry.getState();
	const Movement& m = m_odometry.getMovement();

	// A lot of things to do here. So stop writing useless comments and go to work!
}
