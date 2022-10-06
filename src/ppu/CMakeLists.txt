SET(SRCS
    ppu_memory.cpp
    ppu_memory.h
    ppu_registers.cpp
    ppu_registers.h
    ppu.h
    ppu.cpp
)

target_sources(mnes PRIVATE ${SRCS})