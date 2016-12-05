#include <SerialTalks.h>

#include "Control.h"


Control::Control(WheeledBase& base, Odometry& odometry)
:	m_linearVelocityPID (LINEAR_VELOCITY_PID_ADDRESS)
,	m_angularVelocityPID(ANGULAR_VELOCITY_PID_ADDRESS)

,	m_base(base)
,	m_odometry(odometry)

,	m_enabled(true)
{
	//                             | Kp | Ki | Kd |
//	m_linearVelocityPID .setTunings(  8,   2,   0 );
//	m_angularVelocityPID.setTunings(  8,   0,   0 );
	disable();
}

void Control::step()
{
	if (m_enabled)
	{
		const State&	s = m_odometry.getState();
		const Movement& m = m_odometry.getMovement();

		m_linearVelocityPID .setInput(m.linear);
		m_angularVelocityPID.setInput(m.angular);

		m_linearVelocityPID .step();
		m_angularVelocityPID.step();

		const float linearVelocity  = m_linearVelocityPID .getOutput();
		const float angularVelocity = m_angularVelocityPID.getOutput();

		m_base.leftMotor .setSpeed(linearVelocity - angularVelocity * m_base.axleTrack / 2);
		m_base.rightMotor.setSpeed(linearVelocity + angularVelocity * m_base.axleTrack / 2);
	}
}

void Control::enable()
{
	m_enabled = true;
	m_linearVelocityPID .enable();
	m_angularVelocityPID.enable();
}

void Control::disable()
{
	m_enabled = false;
	m_linearVelocityPID .enable();
	m_angularVelocityPID.enable();
}
