#include "DifferentialController.h"


void DifferentialController::setInputs(float linearInput, float angularInput)
{
	m_linearInput  = linearInput;
	m_angularInput = angularInput;
}

void DifferentialController::setSetpoints(float linearSetpoint, float angularSetpoint)
{
	m_linearSetpoint  = linearSetpoint;
	m_angularSetpoint = angularSetpoint;
}

void DifferentialController::setAxleTrack(float axleTrack)
{
	m_axleTrack = axleTrack;
}

void DifferentialController::setWheels(AbstractMotor& leftWheel, AbstractMotor& rightWheel)
{
	m_leftWheel  = &leftWheel;
	m_rightWheel = &rightWheel;
}

void DifferentialController::setControllers(PID& linearController, PID& angularController)
{
	m_linearController  = &linearController;
	m_angularController = &angularController;
}

void DifferentialController::process(float timestep)
{
	// Compute linear and angular velocities outputs
	float linearOutput  = m_linearController ->compute(m_linearSetpoint,  m_linearInput,  timestep);
	float angularOutput = m_angularController->compute(m_angularSetpoint, m_angularInput, timestep);

#if OUTPUT_CONTROL_VARIABLES
	talks.out << millis() << "\t";
	talks.out << m_linearSetpoint  << "\t" << m_linearInput  << "\t" << linearOutput  << "\t";
	talks.out << m_angularSetpoint << "\t" << m_angularInput << "\t" << angularOutput << "\n";
#endif

	// Convert linear and angular velocities into wheels' velocities
	m_leftWheel ->setVelocity(linearOutput - angularOutput * m_axleTrack / 2);
	m_rightWheel->setVelocity(linearOutput + angularOutput * m_axleTrack / 2);
}

void DifferentialController::onProcessEnabling()
{
	m_linearController ->reset();
	m_angularController->reset();
}
