#include <SerialTalks.h>

#include "Control.h"


Control::Control(WheeledBase& base, Odometry& odometry)
:	m_linearVelocityPID (LINEAR_VELOCITY_PID_ADDRESS)
,	m_angularVelocityPID(ANGULAR_VELOCITY_PID_ADDRESS)

,	m_targetVelocities(0, 0)
,	m_targetStance(0, 0, 0)
,	m_target(VELOCITIES)

,	m_base(base)
,	m_odometry(odometry)
{
	//                             | Kp | Ki | Kd |
//	m_linearVelocityPID .setTunings(  8,   2,   0 );
//	m_angularVelocityPID.setTunings(  8,   0,   0 );
	disable();
}

void Control::setTargetVelocities(float linear, float angular)
{
	m_targetVelocities = Movement(linear, angular);
	m_target = VELOCITIES;
}

void Control::setTargetStance(float x, float y, float theta, float linear, float angular)
{
	m_targetStance     = State(x, y, theta);
	m_targetVelocities = Movement(linear, angular);
	m_target = STANCE;
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
	m_linearVelocityPID .disable();
	m_angularVelocityPID.disable();
}

void Control::step()
{
	if (m_enabled)
	{
		// Set velocities setpoints depending on the current control target
		switch (m_target)
		{
		case VELOCITIES:
			setVelocitiesSetpointsDependingOnTargetVelocities();
			break;
		case STANCE:
			setVelocitiesSetpointsDependingOnTargetStance();
			break;
		}

		// Get the linear and angular output velocities
		const Movement& m = m_odometry.getMovement();
		m_linearVelocityPID .setInput(m.linear);
		m_angularVelocityPID.setInput(m.angular);
		m_linearVelocityPID .step();
		m_angularVelocityPID.step();
		const float linearVelocity  = m_linearVelocityPID .getOutput();
		const float angularVelocity = m_angularVelocityPID.getOutput();

		// Convert the linear and angular velocities into wheels' speed
		m_base.leftMotor .setSpeed(linearVelocity - angularVelocity * m_base.axleTrack / 2);
		m_base.rightMotor.setSpeed(linearVelocity + angularVelocity * m_base.axleTrack / 2);
	}
}

void Control::setVelocitiesSetpointsDependingOnTargetVelocities()
{
	m_linearVelocityPID .setSetpoint(m_targetVelocities.linear);
	m_angularVelocityPID.setSetpoint(m_targetVelocities.angular);
}

void Control::setVelocitiesSetpointsDependingOnTargetStance()
{

}
