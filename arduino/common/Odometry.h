#ifndef __ODOMETRY_H__
#define __ODOMETRY_H__

#include "WheeledBase.h"
#include "Clock.h"


struct Position
{
	Position() : x(0), y(0), theta(0){}
	Position(float x, float y, float theta) : x(x), y(y), theta(theta){}

	float x, y;
	float theta;
};

class Odometry
{
public:

	virtual ~Odometry(){}

	const Position&	getPosition() const;
	float getLinearVelocity () const;
	float getAngularVelocity() const;

	void calibrateXAxis(float x);
	void calibrateYAxis(float y);
	void calibrateOrientation(float theta);

	void setAxleTrack(float axleTrack);

	void setTimestep(float timestep);

	virtual void update();

protected:

	virtual float getLeftWheelTraveledDistance () = 0;
	virtual float getRightWheelTraveledDistance() = 0;

	Clock m_clock;
	float m_timestep;

	Position m_position;

	float m_linearVelocity;
	float m_angularVelocity;

	float m_axleTrack;
};

#endif // __ODOMETRY_H__
