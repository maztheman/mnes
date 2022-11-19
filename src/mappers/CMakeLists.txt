SET(SRCS 
    aorom.cpp
    cnrom.cpp
    mapper.cpp
    mapper.h
    mmc1.cpp
    mmc2.cpp
    mmc3.cpp
    mmc4.cpp
    mmc5.cpp
    nrom.cpp
    unrom.cpp
)
target_sources(mnes PRIVATE ${SRCS})