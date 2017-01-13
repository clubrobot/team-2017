#ifndef __WHEELEDBASE_H__
#define __WHEELEDBASE_H__


class WheeledBase
{
public:

	virtual ~WheeledBase(){}

	virtual void setLinearVelocity () = 0;
	virtual void setAngularVelocity() = 0;

	float getAxleTrack() const {return m_axleTrack;}

	void setAxleTrack(float axleTrack){m_axleTrack = axleTrack;}

	virtual void update(){}

protected:

	float m_axleTrack;
};

#endif // __WHEELEDBASE_H__
