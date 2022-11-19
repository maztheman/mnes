SET(SRCS
    BytePtrVector.h
    control_flags.cpp
    control_flags.h
    Cyclediff.cpp
    File.h
    FileLoader.cpp
    FileLoader.h
    global.cpp
    global.h
    ines_format.h
    Log.cpp
    Log.h
)

target_sources(mnes PRIVATE ${SRCS})