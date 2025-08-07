#include "interrupt_handler.h"
#include "PIC.h"
#include "util.h"

void k_common_exception_handler(int vec_num, uint64_t e_code) {
    char vc_buffer[] = "[EXP:  ,?]";

    vc_buffer[5] = '0' + vec_num / 10;
    vc_buffer[6] = '0' + vec_num % 10;

    k_print(70,0, vc_buffer);
    k_print(0,0, "GOODBYE CRUEL WORlD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    for(;;);
}

void k_common_interrupt_handler(int vec_num) {
    char vc_buffer[] = "[INT:  , ]";
    static int g_common_interrupt_count = 0;

    vc_buffer[5] = '0' + vec_num / 10;
    vc_buffer[6] = '0' + vec_num % 10;
    vc_buffer[8] = '0' + g_common_interrupt_count;
    g_common_interrupt_count = (g_common_interrupt_count + 1) % 10;
    k_print(70,1, vc_buffer);

    k_send_EOI_to_PIC(vec_num - PIC_IRQ_START_VECTOR);
}

void k_keyboard_handler(int vec_num) {
    char vc_buffer[] = "[INT:  , ]";
    static int g_keyboard_interrupt_count = 0;

    vc_buffer[5] = '0' + vec_num / 10;
    vc_buffer[6] = '0' + vec_num % 10;
    vc_buffer[8] = '0' + g_keyboard_interrupt_count;
    g_keyboard_interrupt_count = (g_keyboard_interrupt_count + 1) % 10;
    k_print(70,2,vc_buffer);

    k_send_EOI_to_PIC(vec_num - PIC_IRQ_START_VECTOR);
}

