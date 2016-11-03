#include "WheeledBase.h"

WheeledBase::WheeledBase()
//					| EN | IN1 | IN2 |
:	leftMotor		(  6,    5,    3 )
,	rightMotor		(  9,   10,   11 )

//					| RESET | FAULT |
,	driver			(    12,     A7 )

//					|             axis | XY | SEL1 | SEL2 | OE | RST |  Q | PL | CP | counts per revoltion | radius in mm |
,	leftEncoder		( RotaryEncoder::Y,  A0,     7,     8,  A5,   A3,  13,   4,   2,                 10000,         -22.7 )
,	rightEncoder	( RotaryEncoder::X,  A0,     7,     8,  A5,   A2,  13,   4,   2,                 10000,         22.67 )

,	leftWheelRadius	( 62 ) // mm
,	rightWheelRadius( 62 ) // mm
,	axleTrack		( 318 ) // mm
{
	// Add extra code to fully initialize the wheeled base here
}


