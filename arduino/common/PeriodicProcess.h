#ifndef __PERIODICPROCESS_H__
#define __PERIODICPROCESS_H__

#include "Clock.h"


class PeriodicProcess
{
public:

	virtual ~PeriodicProcess(){}

	bool update()
	{
		if (m_enabled && m_clock.getElapsedTime() > m_timestep)
		{
			float timestep = m_clock.restart();
			process(timestep);
			return true;
		}
		return false;
	}

	bool isEnabled() const {return m_enabled;}

	void enable()
	{
		if (!m_enabled)
		{
			m_enabled = true;
			m_clock.restart();
			onProcessEnabling();
		}
	}

	void disable()
	{
		if (m_enabled)
		{
			m_enabled = false;
			onProcessDisabling();
		}
	}

	float getTimestep() const {return m_timestep;}

	void setTimestep(float timestep){m_timestep = timestep;} 

protected:

	virtual void process(float timestep) = 0;

	virtual void onProcessEnabling(){}
	virtual void onProcessDisabling(){}

private:

	bool  m_enabled;

	Clock m_clock;
	float m_timestep;
};

#endif // __PERIODICPROCESS_H__
