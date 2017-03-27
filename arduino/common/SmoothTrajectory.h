#ifndef __SMOOTHTRAJECTORY_H__
#define __SMOOTHTRAJECTORY_H__

#include "PositionController.h"


class SmoothTrajectory : public AbstractMoveStrategy
{
public:

	void reset();

	void setThresholdRadius(float thresholdRadius){m_thresholdRadius = thresholdRadius;}

	float getThresholdRadius() const {return m_thresholdRadius;}

	void load(int address);
	void save(int address) const;

protected:

	virtual void computeVelSetpoints(float timestep);
	virtual bool getPositionReached();

	// Computation variables
	bool m_underPosThresholds;

	// Trajectory tunings
	float m_thresholdRadius;
};

#endif // __SMOOTHTRAJECTORY_H__
