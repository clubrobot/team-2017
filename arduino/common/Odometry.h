#ifndef __ODOMETRY_H__
#define __ODOMETRY_H__

#include "PeriodicProcess.h"


struct Position
{
	Position() : x(0), y(0), theta(0){}
	Position(float x, float y, float theta) : x(x), y(y), theta(theta){}

	float x, y;
	float theta;
};

class AbstractCodewheel
{
public:

	virtual ~AbstractCodewheel(){}

	virtual float getTraveledDistance() = 0;
	virtual float restart() = 0;
};

class Odometry : public PeriodicProcess
{
public:

	const Position&	getPosition() const;
	float getTraveledDistance() const;
	float getLinearVelocity () const;
	float getAngularVelocity() const;

	void calibrateXAxis(float x);
	void calibrateYAxis(float y);
	void calibrateOrientation(float theta);

	void setAxleTrack(float axleTrack);
	void setWheels(AbstractCodewheel& leftWheel, AbstractCodewheel& rightWheel);

protected:

	virtual void process(float timestep);

	Position m_position;
	float m_traveledDistance;
	float m_linearVelocity;
	float m_angularVelocity;

	float m_axleTrack;
	AbstractCodewheel* m_leftWheel;
	AbstractCodewheel* m_rightWheel;
};

#endif // __ODOMETRY_H__
