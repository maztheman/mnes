SET(SRCS
    cpu_opcodes.cpp
    cpu_opcodes.h
    cpu_registers.h
    cpu.cpp
    cpu.h
    memory_registers.h
    memory.cpp
    memory.h
)


target_sources(mnes PRIVATE ${SRCS})