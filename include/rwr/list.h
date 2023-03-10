SOURCE(
    F16,
    "F-16",
    "16",
    RADAR_SOURCE_AIR,
    7.0f,
    {
        .prf = 1200,
        .on_s = 0.2,
        .off_s = 0.5
    }
)

SOURCE(
    SA10,
    "SA-10",
    "S",
    RADAR_SOURCE_SURFACE_HIGH_ALT,
    25.0f,
    {
        .prf = 856,
        .on_s = 0.1,
        .off_s = 0.1
    }
)
