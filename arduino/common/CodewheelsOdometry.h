#ifndef __CODEWHEELSODOMETRY_H__
#define __CODEWHEELSODOMETRY_H__

#include "Odometry.h"
#include "Codewheel.h"


class CodewheelsOdometry : public Odometry
{
public:

	void setWheels(Codewheel& leftWheel, Codewheel& rightWheel);

protected:

	virtual float getLeftWheelTraveledDistance ();
	virtual float getRightWheelTraveledDistance();

	Codewheel* m_leftWheel;
	Codewheel* m_rightWheel;
};

#endif // __CODEWHEELSODOMETRY_H__
