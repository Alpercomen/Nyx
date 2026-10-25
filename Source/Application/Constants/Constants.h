#pragma once
#include <Application/Core/Core.h>

/*===========================================================
    UNIVERSAL CONSTANTS
===========================================================*/
#define G                              6.67430e-11              // Gravitational constant [m^3 kg^-1 s^-2]
#define AU                             1.495978707e11           // Astronomical Unit [m]
#define SOL_SYSTEM_RADIUS              299.2e11                 // Simulation bounding radius [m]
#define METERS_PER_UNIT                1.0e9
#define METER_PER_KILOMETER            1000.0
#define CAMERA_DEFAULT_MINIMUM_ZOOM    10.0
#define CAMERA_DEFAULT_MAXIMUM_ZOOM    1000.0
#define CAMERA_DEFAULT_NEAR_PLANE      0.001
#define CAMERA_DEFAULT_FAR_PLANE       10000.0
#define CAMERA_FOCUS_NEAR_PLANE        0.000001
#define CAMERA_FOCUS_FAR_PLANE         100.0
#define CAMERA_FOCUS_RADIUS            10000.0
#define CAMERA_MINIMUM_ZOOM_MULTIPLIER 1.0
#define CAMERA_MAXIMUM_ZOOM_MULTIPLIER 500.0
#define DELTA_TIME                     0.01666666666666666666666666666667
#define TIME_LIMIT                     1000
#define DESIRED_DT_MAX                 5.0
#define DESIRED_DT_MIN                 0.01
#define GLOBAL_DT_SCALE                0.5
#define TIGHT_ORBIT_SCALE              0.5

/*===========================================================
    ORBITAL ELEMENT CONVENTIONS

    Planetary orbital elements below are heliocentric and
    referenced to the J2000 mean ecliptic/equinox.

    a       = semi-major axis [m]
    e       = eccentricity [-]
    i       = orbital inclination [deg]
    Omega   = longitude of ascending node [deg]
    omega   = argument of periapsis [deg]
    M       = mean anomaly at J2000 [deg]
    nu      = true anomaly at J2000 [deg]
    q       = periapsis distance [m]
    Q       = apoapsis distance [m]

    IMPORTANT:
    Orbital inclination is NOT the same as axial tilt.
    The existing *_INCLINATION constants below are retained
    as axial tilt values for backwards compatibility.

    The orbital elements are the JPL approximate planetary
    elements around J2000.0. They are intended for initializing
    a Keplerian orbit / simulation, not replacing a high-precision
    ephemeris.
===========================================================*/
#define ORBITAL_EPOCH_J2000            2451545.0
#define ORBITAL_EPOCH_J2000_JD         2451545.0
#define ECLIPTIC_J2000_OBLIQUITY       23.43928

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
    STAR: SUN
===========================================================*/
#define SUN_RADIUS                     1391000000.0
#define SUN_MASS                       1.989e30

/*===========================================================
    ORBITAL INCLINATION vs AXIAL TILT

    ORBITAL_INCLINATION = inclination of the planet's orbit
                          relative to the reference ecliptic plane.

    AXIAL_TILT           = tilt of the planet's rotation axis
                          relative to its orbital normal.

    Legacy *_INCLINATION names are retained as aliases to
    *_AXIAL_TILT for compatibility with existing Nyx code.
===========================================================*/

/*===========================================================
    MERCURY
===========================================================*/
#define MERCURY_EQUATORAL_RADIUS       2439700.0
#define MERCURY_SEMIMINOR_AXIS         2439300.0
#define MERCURY_MEAN_RADIUS            2439700.0
#define MERCURY_AXIAL_TILT                 0.034
#define MERCURY_INCLINATION                MERCURY_AXIAL_TILT
#define MERCURY_MASS                   3.3011e23
#define MERCURY_PERIHELION             46e9
#define MERCURY_APHELION               70e9
#define MERCURY_ANGULAR_VELOCITY_RADIANS 1.24e-6

// J2000 heliocentric orbital elements
#define MERCURY_SEMI_MAJOR_AXIS        (0.38709927 * AU)
#define MERCURY_ECCENTRICITY           0.20563593
#define MERCURY_ORBITAL_INCLINATION    7.00497902
#define MERCURY_LONGITUDE_ASCENDING_NODE 48.33076593
#define MERCURY_LONGITUDE_PERIHELION   77.45779628
#define MERCURY_ARGUMENT_OF_PERIAPSIS  29.12703035
#define MERCURY_MEAN_ANOMALY           174.79252722
#define MERCURY_TRUE_ANOMALY           176.49286181
#define MERCURY_ORBITAL_PERIOD_DAYS    87.9691
#define MERCURY_MEAN_MOTION_DEG_PER_DAY (360.0 / MERCURY_ORBITAL_PERIOD_DAYS)
#define MERCURY_PERIAPSIS_DISTANCE     (MERCURY_SEMI_MAJOR_AXIS * (1.0 - MERCURY_ECCENTRICITY))
#define MERCURY_APOAPSIS_DISTANCE      (MERCURY_SEMI_MAJOR_AXIS * (1.0 + MERCURY_ECCENTRICITY))

/*===========================================================
    VENUS
===========================================================*/
#define VENUS_EQUATORAL_RADIUS         6051800.0
#define VENUS_SEMIMINOR_AXIS           6051800.0
#define VENUS_MEAN_RADIUS              6051800.0
#define VENUS_AXIAL_TILT                 177.36
#define VENUS_INCLINATION                VENUS_AXIAL_TILT
#define VENUS_MASS                     4.8675e24
#define VENUS_PERIHELION               107.48e9
#define VENUS_APHELION                 108.2e9
#define VENUS_ANGULAR_VELOCITY_RADIANS -2.99e-7

// J2000 heliocentric orbital elements
#define VENUS_SEMI_MAJOR_AXIS          (0.72333566 * AU)
#define VENUS_ECCENTRICITY             0.00677672
#define VENUS_ORBITAL_INCLINATION      3.39467605
#define VENUS_LONGITUDE_ASCENDING_NODE 76.67984255
#define VENUS_LONGITUDE_PERIHELION     131.60246718
#define VENUS_ARGUMENT_OF_PERIAPSIS    54.92262463
#define VENUS_MEAN_ANOMALY             50.37663232
#define VENUS_TRUE_ANOMALY             50.97801332
#define VENUS_ORBITAL_PERIOD_DAYS      224.701
#define VENUS_MEAN_MOTION_DEG_PER_DAY  (360.0 / VENUS_ORBITAL_PERIOD_DAYS)
#define VENUS_PERIAPSIS_DISTANCE       (VENUS_SEMI_MAJOR_AXIS * (1.0 - VENUS_ECCENTRICITY))
#define VENUS_APOAPSIS_DISTANCE        (VENUS_SEMI_MAJOR_AXIS * (1.0 + VENUS_ECCENTRICITY))

/*===========================================================
    EARTH
===========================================================*/
#define EARTH_EQUATORAL_RADIUS         6378137.0
#define EARTH_SEMIMINOR_AXIS_RADIUS    6356752.3141
#define EARTH_POLAR_RADIUS_OF_CURVATURE_RADIUS 6399593.6259
#define EARTH_MEAN_RADIUS_RADIUS       6371008.7714
#define EARTH_AXIAL_TILT                 23.4393
#define EARTH_INCLINATION                EARTH_AXIAL_TILT
#define EARTH_MASS                     5.972e24
#define EARTH_PERIHELION               147.09e9
#define EARTH_APHELION                 152.09e9
#define EARTH_ANGULAR_VELOCITY_RADIANS 7.2921159e-5

// J2000 heliocentric orbital elements
#define EARTH_SEMI_MAJOR_AXIS          (1.00000261 * AU)
#define EARTH_ECCENTRICITY             0.01671123
#define EARTH_ORBITAL_INCLINATION      -0.00001531
#define EARTH_LONGITUDE_ASCENDING_NODE 0.0
#define EARTH_LONGITUDE_PERIHELION     102.93768193
#define EARTH_ARGUMENT_OF_PERIAPSIS    102.93768193
#define EARTH_MEAN_ANOMALY             357.52688973
#define EARTH_TRUE_ANOMALY             357.44249829
#define EARTH_ORBITAL_PERIOD_DAYS      365.2568983
#define EARTH_MEAN_MOTION_DEG_PER_DAY  (360.0 / EARTH_ORBITAL_PERIOD_DAYS)
#define EARTH_PERIAPSIS_DISTANCE       (EARTH_SEMI_MAJOR_AXIS * (1.0 - EARTH_ECCENTRICITY))
#define EARTH_APOAPSIS_DISTANCE        (EARTH_SEMI_MAJOR_AXIS * (1.0 + EARTH_ECCENTRICITY))

/*===========================================================
    MOON

    Mean orbital elements, epoch J2000.0, relative to Earth.
===========================================================*/
#define MOON_EQUATORAL_RADIUS          1738100.0
#define MOON_SEMIMINOR_AXIS_RADIUS     1736000.0
#define MOON_MEAN_RADIUS_RADIUS        1737400.0
#define MOON_MASS                      7.342e22
#define EARTH_PERIGEE                  363300000.0
#define EARTH_APOGEE                   405504000.0
#define MOON_ANGULAR_VELOCITY_RADIANS  2.730e-6

#define MOON_SEMI_MAJOR_AXIS           384400000.0
#define MOON_ECCENTRICITY              0.0554
#define MOON_ORBITAL_INCLINATION       5.16
#define MOON_ARGUMENT_OF_PERIAPSIS     318.15
#define MOON_MEAN_ANOMALY              135.27
#define MOON_TRUE_ANOMALY              139.52402595
#define MOON_LONGITUDE_ASCENDING_NODE  125.08
#define MOON_ORBITAL_PERIOD_DAYS       27.322
#define MOON_MEAN_MOTION_DEG_PER_DAY   (360.0 / MOON_ORBITAL_PERIOD_DAYS)
#define MOON_PERIAPSIS_DISTANCE        (MOON_SEMI_MAJOR_AXIS * (1.0 - MOON_ECCENTRICITY))
#define MOON_APOAPSIS_DISTANCE         (MOON_SEMI_MAJOR_AXIS * (1.0 + MOON_ECCENTRICITY))

/*===========================================================
    MARS
===========================================================*/
#define MARS_EQUATORAL_RADIUS          3396200.0
#define MARS_SEMIMINOR_AXIS            3376200.0
#define MARS_MEAN_RADIUS               3389500.0
#define MARS_AXIAL_TILT                 25.19
#define MARS_INCLINATION                MARS_AXIAL_TILT
#define MARS_MASS                      6.4171e23
#define MARS_PERIHELION                206.6e9
#define MARS_APHELION                 249.2e9
#define MARS_ANGULAR_VELOCITY_RADIANS  7.088e-5

// J2000 heliocentric orbital elements
#define MARS_SEMI_MAJOR_AXIS           (1.52371034 * AU)
#define MARS_ECCENTRICITY              0.09339410
#define MARS_ORBITAL_INCLINATION       1.84969142
#define MARS_LONGITUDE_ASCENDING_NODE  49.55953891
#define MARS_LONGITUDE_PERIHELION      -23.94362959
#define MARS_ARGUMENT_OF_PERIAPSIS     286.49683150
#define MARS_MEAN_ANOMALY              19.39019754
#define MARS_TRUE_ANOMALY              23.37723882
#define MARS_ORBITAL_PERIOD_DAYS       686.980
#define MARS_MEAN_MOTION_DEG_PER_DAY   (360.0 / MARS_ORBITAL_PERIOD_DAYS)
#define MARS_PERIAPSIS_DISTANCE        (MARS_SEMI_MAJOR_AXIS * (1.0 - MARS_ECCENTRICITY))
#define MARS_APOAPSIS_DISTANCE         (MARS_SEMI_MAJOR_AXIS * (1.0 + MARS_ECCENTRICITY))

/*===========================================================
    JUPITER
===========================================================*/
#define JUPITER_EQUATORAL_RADIUS       71492000.0
#define JUPITER_SEMIMINOR_AXIS         66854000.0
#define JUPITER_MEAN_RADIUS            69911000.0
#define JUPITER_AXIAL_TILT                 3.13
#define JUPITER_INCLINATION                JUPITER_AXIAL_TILT
#define JUPITER_MASS                   1.898e27
#define JUPITER_PERIHELION             740.6e9
#define JUPITER_APHELION              816.4e9
#define JUPITER_ANGULAR_VELOCITY_RADIANS 1.7585e-4

// J2000 heliocentric orbital elements
#define JUPITER_SEMI_MAJOR_AXIS        (5.20288700 * AU)
#define JUPITER_ECCENTRICITY           0.04838624
#define JUPITER_ORBITAL_INCLINATION    1.30439695
#define JUPITER_LONGITUDE_ASCENDING_NODE 100.47390909
#define JUPITER_LONGITUDE_PERIHELION   14.72847983
#define JUPITER_ARGUMENT_OF_PERIAPSIS  274.25457074
#define JUPITER_MEAN_ANOMALY           19.66796068
#define JUPITER_TRUE_ANOMALY           21.64613293
#define JUPITER_ORBITAL_PERIOD_DAYS    4332.589
#define JUPITER_MEAN_MOTION_DEG_PER_DAY (360.0 / JUPITER_ORBITAL_PERIOD_DAYS)
#define JUPITER_PERIAPSIS_DISTANCE     (JUPITER_SEMI_MAJOR_AXIS * (1.0 - JUPITER_ECCENTRICITY))
#define JUPITER_APOAPSIS_DISTANCE      (JUPITER_SEMI_MAJOR_AXIS * (1.0 + JUPITER_ECCENTRICITY))

/*===========================================================
    SATURN
===========================================================*/
#define SATURN_EQUATORAL_RADIUS        60268000.0
#define SATURN_SEMIMINOR_AXIS          54364000.0
#define SATURN_MEAN_RADIUS             58232000.0
#define SATURN_AXIAL_TILT                 26.73
#define SATURN_INCLINATION                SATURN_AXIAL_TILT
#define SATURN_MASS                    5.683e26
#define SATURN_PERIHELION              1.35255e12
#define SATURN_APHELION               1.51450e12
#define SATURN_ANGULAR_VELOCITY_RADIANS 1.637e-4

// J2000 heliocentric orbital elements
#define SATURN_SEMI_MAJOR_AXIS         (9.53667594 * AU)
#define SATURN_ECCENTRICITY            0.05386179
#define SATURN_ORBITAL_INCLINATION     2.48599187
#define SATURN_LONGITUDE_ASCENDING_NODE 113.66242448
#define SATURN_LONGITUDE_PERIHELION    92.59887831
#define SATURN_ARGUMENT_OF_PERIAPSIS   338.93645383
#define SATURN_MEAN_ANOMALY            317.35536592
#define SATURN_TRUE_ANOMALY            312.96103680
#define SATURN_ORBITAL_PERIOD_DAYS     10759.22
#define SATURN_MEAN_MOTION_DEG_PER_DAY (360.0 / SATURN_ORBITAL_PERIOD_DAYS)
#define SATURN_PERIAPSIS_DISTANCE      (SATURN_SEMI_MAJOR_AXIS * (1.0 - SATURN_ECCENTRICITY))
#define SATURN_APOAPSIS_DISTANCE       (SATURN_SEMI_MAJOR_AXIS * (1.0 + SATURN_ECCENTRICITY))

/*===========================================================
    URANUS
===========================================================*/
#define URANUS_EQUATORAL_RADIUS        25559000.0
#define URANUS_SEMIMINOR_AXIS          24973000.0
#define URANUS_MEAN_RADIUS             25362000.0
#define URANUS_AXIAL_TILT                 97.77
#define URANUS_INCLINATION                URANUS_AXIAL_TILT
#define URANUS_MASS                    8.681e25
#define URANUS_PERIHELION              2.732696e12
#define URANUS_APHELION                3.001390e12
#define URANUS_ANGULAR_VELOCITY_RADIANS (-1.012e-4)

// J2000 heliocentric orbital elements
#define URANUS_SEMI_MAJOR_AXIS         (19.18916464 * AU)
#define URANUS_ECCENTRICITY            0.04725744
#define URANUS_ORBITAL_INCLINATION     0.77263783
#define URANUS_LONGITUDE_ASCENDING_NODE 74.01692503
#define URANUS_LONGITUDE_PERIHELION    170.95427630
#define URANUS_ARGUMENT_OF_PERIAPSIS   96.93735127
#define URANUS_MEAN_ANOMALY            142.28382821
#define URANUS_TRUE_ANOMALY            145.44690495
#define URANUS_ORBITAL_PERIOD_DAYS     30688.5
#define URANUS_MEAN_MOTION_DEG_PER_DAY (360.0 / URANUS_ORBITAL_PERIOD_DAYS)
#define URANUS_PERIAPSIS_DISTANCE      (URANUS_SEMI_MAJOR_AXIS * (1.0 - URANUS_ECCENTRICITY))
#define URANUS_APOAPSIS_DISTANCE       (URANUS_SEMI_MAJOR_AXIS * (1.0 + URANUS_ECCENTRICITY))

/*===========================================================
    NEPTUNE
===========================================================*/
#define NEPTUNE_EQUATORAL_RADIUS       24764000.0
#define NEPTUNE_SEMIMINOR_AXIS         24341000.0
#define NEPTUNE_MEAN_RADIUS            24622000.0
#define NEPTUNE_AXIAL_TILT                 28.32
#define NEPTUNE_INCLINATION                NEPTUNE_AXIAL_TILT
#define NEPTUNE_MASS                   1.024e26
#define NEPTUNE_PERIHELION             4.444450e12
#define NEPTUNE_APHELION               4.545670e12
#define NEPTUNE_ANGULAR_VELOCITY_RADIANS 1.083e-4

// J2000 heliocentric orbital elements
#define NEPTUNE_SEMI_MAJOR_AXIS        (30.06992276 * AU)
#define NEPTUNE_ECCENTRICITY           0.00859048
#define NEPTUNE_ORBITAL_INCLINATION    1.77004347
#define NEPTUNE_LONGITUDE_ASCENDING_NODE 131.78422574
#define NEPTUNE_LONGITUDE_PERIHELION   44.96476227
#define NEPTUNE_ARGUMENT_OF_PERIAPSIS  273.18053653
#define NEPTUNE_MEAN_ANOMALY           204.91520969
#define NEPTUNE_TRUE_ANOMALY           258.94788579
#define NEPTUNE_ORBITAL_PERIOD_DAYS    60182.0
#define NEPTUNE_MEAN_MOTION_DEG_PER_DAY (360.0 / NEPTUNE_ORBITAL_PERIOD_DAYS)
#define NEPTUNE_PERIAPSIS_DISTANCE     (NEPTUNE_SEMI_MAJOR_AXIS * (1.0 - NEPTUNE_ECCENTRICITY))
#define NEPTUNE_APOAPSIS_DISTANCE      (NEPTUNE_SEMI_MAJOR_AXIS * (1.0 + NEPTUNE_ECCENTRICITY))

/*===========================================================
    International Space Station (ISS)
===========================================================*/
#define ISS_MASS 419725
#define ISS_SOLAR_ARRAY_LENGTH 109
#define ISS_MODULE_LENGTH 51
#define ISS_TRUSS_LENGTH 94
#define ISS_LONGITUDE_SOLAR_ARRAY 73
#define ISS_INCLINATION 51.6
#define ISS_PERIGEE_DISTANCE 6741000
#define ISS_APOGEE_DISTANCE 6831000
#define ISS_SPEED 7600
