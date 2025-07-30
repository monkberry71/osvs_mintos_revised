#pragma once

#include "types.h"

void k_read_CPUID(uint32_t dwEAX, uint32_t* pdwEAX, uint32_t* pdwEBX, uint32_t* pdwECX, uint32_t* pdwEDX);
void k_switch_kernel64(void);