#include "DCMotorsWheeledBase.h"
#include "SerialTalks.h"


void DCMotorsWheeledBase::setLinearVelocity(float linearVelocity)
{
	m_linearVelocitySetpoint = linearVelocity;
}

void DCMotorsWheeledBase::setAngularVelocity(float angularVelocity)
{
	m_angularVelocitySetpoint = angularVelocity;
}

void DCMotorsWheeledBase::setWheels(DCMotor& leftWheel, DCMotor& rightWheel)
{
	m_leftWheel  = &leftWheel;
	m_rightWheel = &rightWheel;
}

void DCMotorsWheeledBase::setOdometry(Odometry& odometry)
{
	m_odometry = &odometry;
}

void DCMotorsWheeledBase::setPIDControllers(PID& linear, PID& angular)
{
	m_linearVelocityController  = &linear;
	m_angularVelocityController = &angular;
}

void DCMotorsWheeledBase::enable()
{
	m_enabled = true;
}

void DCMotorsWheeledBase::disable()
{
	m_enabled = false;
}

void DCMotorsWheeledBase::update()
{
	if (m_enabled)
	{
		// Alias linear and angular velocities setpoints and inputs
		const float LVSetpoint = m_linearVelocitySetpoint;
		const float AVSetpoint = m_angularVelocitySetpoint;
		const float LVInput = m_odometry->getLinearVelocity();
		const float AVInput = m_odometry->getAngularVelocity();

		// Compute linear and angular velocities outputs
		float LVOutput;
		float AVOutput;
		if (m_linearVelocityController ->compute(LVSetpoint, LVInput, LVOutput) | // single pipe IS important
			m_angularVelocityController->compute(AVSetpoint, AVInput, AVOutput))
		{
#if SHOW_CONTROL_VARIABLES
			talks.out << millis() << "\t";
			talks.out << LVSetpoint << "\t" << LVInput << "\t" << LVOutput << "\t";
			talks.out << AVSetpoint << "\t" << AVInput << "\t" << AVOutput << "\n";
#endif
			// Convert linear and angular velocities into wheels' velocities
			m_leftWheel ->setVelocity(LVOutput - AVOutput * m_axleTrack / 2);
			m_rightWheel->setVelocity(LVOutput + AVOutput * m_axleTrack / 2);
		}
	}
}
