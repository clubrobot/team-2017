#include "DCMotorsBase.h"


void DCMotorsBase::setLinearVelocity(float linearVelocity)
{
	m_linearVelocitySetpoint = linearVelocity;
}

void DCMotorsBase::setAngularVelocity(float angularVelocity)
{
	m_angularVelocitySetpoint = angularVelocity;
}

void DCMotorsBase::setWheels(DCMotorWheel& left, DCMotorWheel& right)
{
	m_leftWheel  = &left;
	m_rightWheel = &right;
}

void DCMotorsBase::setOdometry(Odometry& odometry)
{
	m_odometry = &odometry;
}

void DCMotorsBase::update()
{
	// Alias linear and angular velocities setpoints and inputs
	const float LVSetpoint = m_linearVelocitySetpoint;
	const float AVSetpoint = m_angularVelocitySetpoint;
	const float LVInput = m_odometry->getLinearVelocity();
	const float AVInput = m_odometry->getAngularVelocity();

	// Compute linear and angular velocities outputs
	const float LVOutput = m_linearVelocityController .compute(LVSetpoint, LVInput);
	const float AVOutput = m_angularVelocityController.compute(AVSetpoint, AVInput);

	// Convert linear and angular velocities into wheels' speed
	m_leftWheel ->setSpeed(LVOutput - AVOutput * m_axleTrack / 2);
	m_rightWheel->setSpeed(LVOutput + AVOutput * m_axleTrack / 2);
}
