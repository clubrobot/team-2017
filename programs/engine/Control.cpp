#include "Control.h"


Control::Control(WheeledBase& base, Odometry& odometry)
//						| Kp | Ki | Kd |
:	m_velocityController(  1,   0,   0 )
,	m_omegaController	(  1,   0,   0 )

,	m_base(base)
,	m_odometry(odometry)
{

}

void Control::setVelocitySetpoint(float setpoint)
{
	m_velocityController.setSetpoint(setpoint);
}

void Control::setOmegaSetpoint(float setpoint)
{
	m_omegaController.setSetpoint(setpoint);
}

void Control::step()
{
	const State&	s = m_odometry.getState();
	const Movement& m = m_odometry.getMovement();

	m_velocityController.setInput(sqrt(m.dx_dt * m.dx_dt + m.dy_dt + m.dy_dt));
	m_omegaController   .setInput(m.omega);

	m_velocityController.Compute();
	m_omegaController   .Compute();

	const float velocityOutput	= m_velocityController.getOutput();
	const float omegaOutput		= m_omegaController   .getOutput();

	const float K_L = 1 / (622 * 2 * M_PI * m_base.leftWheelRadius);
	const float K_R = 1 / (622 * 2 * M_PI * m_base.rightWheelRadius);

	m_base.leftMotor .setSpeed(K_L * (velocityOutput - omegaOutput * m_base.axleTrack / 2));
	m_base.rightMotor.setSpeed(K_R * (velocityOutput + omegaOutput * m_base.axleTrack / 2));
}
