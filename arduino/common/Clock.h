#ifndef __CLOCK_H__
#define __CLOCK_H__


class Clock
{
public:

	Clock();

	float getElapsedTime() const;

	float restart();

private:

	unsigned long m_startTime;
};

#endif // __CLOCK_H__
