#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

// DC motors constants

#define DCMOTORS_VELOCITY_CONSTANT 622 // RPM
#define DCMOTORS_REDUCTION_RATIO    19
#define DCMOTORS_SUPPLIED_VOLTAGE   11 // V

// Codewheels constants

#define CODEWHEELS_COUNTS_PER_REVOLUTION 10000

// Mechanical constants

#define LEFT_WHEEL_RADIUS   62.0 // mm
#define RIGHT_WHEEL_RADIUS  62.0 // mm
#define WHEELS_AXLE_TRACK  198.0 // mm

#define LEFT_CODEWHEEL_RADIUS   22.7 // mm
#define RIGHT_CODEWHEEL_RADIUS  23.0 // mm
#define CODEWHEELS_AXLE_TRACK  318.0 // mm

// Timesteps

#define ODOMETRY_TIMESTEP          5e-3 // s
#define PID_CONTROLLERS_TIMESTEP  20e-3 // s

#endif // __CONSTANTS_H__
