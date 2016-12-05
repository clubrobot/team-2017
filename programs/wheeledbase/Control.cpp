#include <SerialTalks.h>

#include "Control.h"


Control::Control(WheeledBase& base, Odometry& odometry)
:	m_velocityController(VELOCITY_CONTROLLER_ADDRESS)
,	m_omegaController	(OMEGA_CONTROLLER_ADDRESS)

,	m_base(base)
,	m_odometry(odometry)

,	m_enabled(true)
{
	//                             | Kp | Ki | Kd |
//	m_velocityController.setTunings(  8,   2,   0 );
//	m_omegaController   .setTunings(  8,   0,   0 );
	disable();
}

void Control::step()
{
	if (m_enabled)
	{
		const State&	s = m_odometry.getState();
		const Movement& m = m_odometry.getMovement();

		m_velocityController.setInput(m.velocity);
		m_omegaController   .setInput(m.omega);

		m_velocityController.step();
		m_omegaController   .step();

		const float velocityOutput	= m_velocityController.getOutput();
		const float omegaOutput		= m_omegaController   .getOutput();

		m_base.leftMotor .setSpeed(velocityOutput - omegaOutput * m_base.axleTrack / 2);
		m_base.rightMotor.setSpeed(velocityOutput + omegaOutput * m_base.axleTrack / 2);
	}
}

void Control::enable()
{
	m_enabled = true;
	m_velocityController.enable();
	m_omegaController   .enable();
}

void Control::disable()
{
	m_enabled = false;
	m_velocityController.disable();
	m_omegaController   .disable();
}
