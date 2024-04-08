SET(SRCS
    control_flags.cpp
    control_flags.h
    Cyclediff.cpp
    FileLoader.cpp
    FileLoader.h
    ines_format.h
    Log.cpp
    Log.h
)

target_sources(mnes PRIVATE ${SRCS})