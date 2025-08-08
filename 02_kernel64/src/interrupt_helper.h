#pragma once

#include "types.h"

void k_enable_interrupt(void);
void k_disable_interrupt(void);
uint64_t k_read_RFLAGS(void);
void k_int_3(void);