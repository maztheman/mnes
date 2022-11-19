SET(SRCS
    ppu_memory.cpp
    ppu_registers.cpp
    ppu.cpp
)

target_sources(mnes PRIVATE ${SRCS})