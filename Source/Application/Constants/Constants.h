#pragma once
/*===========================================================
    TIME & SIMULATION
===========================================================*/
extern float TIME_SCALE;

#define DELTA_TIME                     0.01666666666666666666666666666667

/*===========================================================
    UNIVERSAL CONSTANTS
===========================================================*/
#define G                              6.67430e-11            // Gravitational constant
#define AU                             1.496e+11              // Astronomical Unit (m)
#define SOL_SYSTEM_RADIUS              299.2e11               // Simulation bounding radius
#define METERS_PER_UNIT                10000000.0
#define METER_PER_KILOMETER            1000.0

/*===========================================================
    SPECIAL CONSTANTS / FLAGS
===========================================================*/
#define NO_INDEX                       -1
#define NO_ID                          0

/*===========================================================
    LIGHTING LIMITS
===========================================================*/
#define MAX_DIRECTIONAL_LIGHTS         4
#define MAX_POINT_LIGHTS               16
#define MAX_SPOT_LIGHTS                8

/*===========================================================
    STAR: SUN4
===========================================================*/
#define SUN_RADIUS                     1391000000.0
#define SUN_MASS                       1.989e30

/*===========================================================
    MERCURY
===========================================================*/
#define MERCURY_EQUATORAL_RADIUS       2439700.0
#define MERCURY_SEMIMINOR_AXIS         2439300.0
#define MERCURY_MEAN_RADIUS            2439700.0
#define MERCURY_INCLINATION            0.034
#define MERCURY_MASS                   3.3011e23
#define MERCURY_SUN_DISTANCE           70e9
#define MERCURY_ANGULAR_VELOCITY_RADIANS 1.24e-6

/*===========================================================
    VENUS
===========================================================*/
#define VENUS_EQUATORAL_RADIUS         6051800.0
#define VENUS_SEMIMINOR_AXIS           6051800.0
#define VENUS_MEAN_RADIUS              6051800.0
#define VENUS_INCLINATION              2.64
#define VENUS_MASS                     4.8675e24
#define VENUS_SUN_DISTANCE             108.2e9
#define VENUS_ANGULAR_VELOCITY_RADIANS -2.99e-7

/*===========================================================
    EARTH
===========================================================*/
#define EARTH_EQUATORAL_RADIUS         6378137.0
#define EARTH_SEMIMINOR_AXIS_RADIUS    6356752.3141
#define EARTH_POLAR_RADIUS_OF_CURVATURE_RADIUS 6399593.6259
#define EARTH_MEAN_RADIUS_RADIUS       6371008.7714
#define EARTH_INCLINATION              23.5
#define EARTH_MASS                     5.972e24
#define EARTH_SUN_DISTANCE             149.6e9
#define EARTH_ANGULAR_VELOCITY_RADIANS 7.2921159e-5

/*===========================================================
    MOON
===========================================================*/
#define MOON_EQUATORAL_RADIUS          1738100.0
#define MOON_SEMIMINOR_AXIS_RADIUS     1736000.0
#define MOON_MEAN_RADIUS_RADIUS        1737400.0
#define MOON_MASS                      7.342e22
#define EARTH_MOON_DISTANCE            384400000.0

/*===========================================================
    MARS
===========================================================*/
#define MARS_EQUATORAL_RADIUS          3396200.0
#define MARS_SEMIMINOR_AXIS            3376200.0
#define MARS_MEAN_RADIUS               3389500.0
#define MARS_INCLINATION               25.19
#define MARS_MASS                      6.4171e23
#define MARS_SUN_DISTANCE              227.94e9
#define MARS_ANGULAR_VELOCITY_RADIANS  7.088e-5

/*===========================================================
    JUPITER
===========================================================*/
#define JUPITER_EQUATORAL_RADIUS       71492000.0
#define JUPITER_SEMIMINOR_AXIS         66854000.0
#define JUPITER_MEAN_RADIUS            69911000.0
#define JUPITER_INCLINATION            3.13
#define JUPITER_MASS                   1.898e27
#define JUPITER_SUN_DISTANCE           778.57e9
#define JUPITER_ANGULAR_VELOCITY_RADIANS 1.7585e-4

/*===========================================================
    SATURN
===========================================================*/
#define SATURN_EQUATORAL_RADIUS        60268000.0
#define SATURN_SEMIMINOR_AXIS          54364000.0
#define SATURN_MEAN_RADIUS             58232000.0
#define SATURN_INCLINATION             26.73
#define SATURN_MASS                    5.683e26
#define SATURN_SUN_DISTANCE            1.43353e12
#define SATURN_ANGULAR_VELOCITY_RADIANS 1.637e-4

/*===========================================================
    URANUS
===========================================================*/
#define URANUS_EQUATORAL_RADIUS        25559000.0
#define URANUS_SEMIMINOR_AXIS          24973000.0
#define URANUS_MEAN_RADIUS             25362000.0
#define URANUS_INCLINATION             97.77
#define URANUS_MASS                    8.681e25
#define URANUS_SUN_DISTANCE            2.87246e12
#define URANUS_ANGULAR_VELOCITY_RADIANS (-1.012e-4)

/*===========================================================
    NEPTUNE
===========================================================*/
#define NEPTUNE_EQUATORAL_RADIUS       24764000.0
#define NEPTUNE_SEMIMINOR_AXIS         24341000.0
#define NEPTUNE_MEAN_RADIUS            24622000.0
#define NEPTUNE_INCLINATION            28.32
#define NEPTUNE_MASS                   1.024e26
#define NEPTUNE_SUN_DISTANCE           4.49506e12
#define NEPTUNE_ANGULAR_VELOCITY_RADIANS 1.083e-4