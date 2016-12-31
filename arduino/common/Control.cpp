#include "SerialTalks.h"

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
	reset();
	m_targetVelocities = Movement(linear, angular);
	m_target = VELOCITIES;
}

void Control::setTargetStance(float x, float y, float theta, float linear, float angular)
{
	reset();
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

void Control::reset()
{
	m_linearVelocityPID .reset();
	m_angularVelocityPID.reset();
}

void Control::setVelocitiesSetpointsDependingOnTargetVelocities()
{
	m_linearVelocityPID .setSetpoint(m_targetVelocities.linear);
	m_angularVelocityPID.setSetpoint(m_targetVelocities.angular);
}

static float truemod(float x, float y)
{
	return fmod(fmod(x, y) + y, y);
}

static float inrange(float x, float a, float b)
{
	return truemod(x - a, b - a) + a;
}

static float saturate(float x, float a, float b)
{
	if (x < a)
		return a;
	else if (x > b)
		return b;
	else
		return x;
}

void Control::setVelocitiesSetpointsDependingOnTargetStance()
{
	const float Kp_lin = 1, lin_max = m_targetVelocities.linear , lin_dz = 10;
	const float Kp_ang = 1, ang_max = m_targetVelocities.angular, ang_dz = M_PI / 6;

	State r = m_odometry.getState(); r.theta = inrange(r.theta, -M_PI, M_PI);
	State t = m_targetStance;

	const float dx = t.x - r.x;
	const float dy = t.y - r.y;

	float dist = sqrt(dx * dx + dy * dy);
	float angl = inrange(atan2(dy, dx) - r.theta, -M_PI, M_PI);

	// Let the robot go backward if the angle is greater than 90°
	if (abs(angl) > M_PI / 2)
	{
		dist = -dist;
		angl = inrange(angl + M_PI, -M_PI, M_PI);
	}

	// Get the right orientation if the robot is close enough to its destination
	if (abs(dist) < lin_dz)
	{
		angl = inrange(t.theta - r.theta, -M_PI, M_PI);
	}
	// Else if make the robot turn on the spot if it is not well oriented to reach its destination
	else if (abs(angl) > ang_dz)
	{
		dist = 0;
	}
	talks.out << millis() << "\t" << saturate(Kp_lin * dist, -lin_max, lin_max) << "\t" << saturate(Kp_ang * angl, -ang_max, ang_max) << "\n";
	m_linearVelocityPID .setSetpoint(saturate(Kp_lin * dist, -lin_max, lin_max));
	m_angularVelocityPID.setSetpoint(saturate(Kp_ang * angl, -ang_max, ang_max));
}
