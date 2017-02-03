#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

// DC motors constants

#define DCMOTORS_VELOCITY_CONSTANT 622 // RPM
#define DCMOTORS_REDUCTION_RATIO    19
#define DCMOTORS_SUPPLIED_VOLTAGE   11 // V

// Codewheels constants

#define CODEWHEELS_COUNTS_PER_REVOLUTION 10000

// Mechanical constants

#define LEFT_WHEEL_RADIUS   23.8 // mm
#define RIGHT_WHEEL_RADIUS  23.8 // mm
#define WHEELS_AXLE_TRACK  125.0 // mm

#define LEFT_CODEWHEEL_RADIUS   22.74 // mm
#define RIGHT_CODEWHEEL_RADIUS  22.75 // mm
#define CODEWHEELS_AXLE_TRACK  209.26 // mm

// Trajectory constants

#define MAX_LINEAR_VELOCITY      200 // mm/s
#define MAX_ANGULAR_VELOCITY     3.0 // rad/s^2
#define MAX_LINEAR_ACCELERATION  250 // mm/s^2
#define MAX_ANGULAR_ACCELERATION 3.0 // rad/s^2

// Timesteps

#define ODOMETRY_TIMESTEP          5e-3 // s
#define PID_CONTROLLERS_TIMESTEP  20e-3 // s
#define TRAJECTORY_TIMESTEP       50e-3 // s

#endif // __CONSTANTS_H__
