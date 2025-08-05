#pragma once

#include "types.h"

void k_load_GDTR(uint64_t GDTR_addr);
void k_load_TR(uint8_t TSS_segment_offset);
void k_load_IDTR(uint64_t IDTR_addr);