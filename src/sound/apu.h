#pragma once

using uint = unsigned int;

#ifdef __cplusplus
extern "C" {
#endif

void apu_initialize();
void apu_destroy();
void apu_reset();
void apu_do_cycle();

void apu_memory_write(uint addr, uint value);
uint apu_memory_read(uint address);

#ifdef __cplusplus
}
#endif