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

#if CONTROL_IN_POSITION
void DCMotorsWheeledBase::setPIDControllers(PID& linear, PID& angular)
{
	m_linearPositionController  = &linear;
	m_angularPositionController = &angular;
}
#else
void DCMotorsWheeledBase::setPIDControllers(PID& linear, PID& angular)
{
	m_linearVelocityController  = &linear;
	m_angularVelocityController = &angular;
}
#endif

void DCMotorsWheeledBase::enable()
{
	if (!m_enabled)
	{
		m_enabled = true;
	#if CONTROL_IN_POSITION
		m_clock.restart();
		m_traveledDistanceSetpoint = m_odometry->getTraveledDistance();
		m_thetaSetpoint            = m_odometry->getPosition().theta;
	#endif
	}
}

void DCMotorsWheeledBase::disable()
{
	m_enabled = false;
}

void DCMotorsWheeledBase::update()
{
	if (m_enabled)
	{
#if CONTROL_IN_POSITION
		// Compute linear and angular position setpoints
		float timestep = m_clock.restart();
		m_traveledDistanceSetpoint += m_linearVelocitySetpoint  * timestep;
		m_thetaSetpoint            += m_angularVelocitySetpoint * timestep;

		// TODO: Clamp setpoints if they are too far from the current inputs
		
		// Alias linear and angular velocities setpoints and inputs
		const float linearSetpoint  = m_traveledDistanceSetpoint;
		const float angularSetpoint = m_thetaSetpoint;
		const float linearInput  = m_odometry->getTraveledDistance();
		const float angularInput = m_odometry->getPosition().theta;
#else
		// Alias linear and angular velocities setpoints and inputs
		const float linearSetpoint  = m_linearVelocitySetpoint;
		const float angularSetpoint = m_angularVelocitySetpoint;
		const float linearInput  = m_odometry->getLinearVelocity();
		const float angularInput = m_odometry->getAngularVelocity();
#endif // CONTROL_IN_POSITION

		// Compute linear and angular velocities outputs
		float linearOutput;
		float angularOutput;
#if CONTROL_IN_POSITION
		if (m_linearPositionController ->compute(linearSetpoint,  linearInput,  linearOutput) | // single pipe IS important
			m_angularPositionController->compute(angularSetpoint, angularInput, angularOutput))
#else
		if (m_linearVelocityController ->compute(linearSetpoint,  linearInput,  linearOutput) | // single pipe IS important
			m_angularVelocityController->compute(angularSetpoint, angularInput, angularOutput))
#endif // CONTROL_IN_POSITION
		{
#if OUTPUT_CONTROL_VARIABLES
			talks.out << millis() << "\t";
			talks.out << linearSetpoint  << "\t" << linearInput  << "\t" << linearOutput  << "\t";
			talks.out << angularSetpoint << "\t" << angularInput << "\t" << angularOutput << "\n";
#endif
			// Convert linear and angular velocities into wheels' velocities
			m_leftWheel ->setVelocity(linearOutput - angularOutput * m_axleTrack / 2);
			m_rightWheel->setVelocity(linearOutput + angularOutput * m_axleTrack / 2);
		}
	}
}
