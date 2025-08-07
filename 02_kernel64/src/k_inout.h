#pragma once

#include "types.h"

uint8_t k_inb(uint16_t port_num);
void k_outb(uint16_t port_num, uint8_t what_to_write);
