#include "types.h"
#include "keyboard.h"
#include "descriptor.h"
#include "set_register.h"
#include "PIC.h"
#include "interrupt_stack.h"
#include "console.h"
#include "shell.h"
#include "util.h"

void k_print_num(int x, int y, unsigned char num);
extern int cli_stack;

void main(void) {

    int cursor_x, cursor_y;
    k_init_console(0,10);
    k_printf("IA-32e mode starting...\n");


    // GDT init
    k_printf("GDT init");
    k_init_GDT_TSS();
    k_load_GDTR( GDTR_ADDR );
    k_printf(" pasn");

    k_printf("TSS load");
    k_load_TR( GDT_TSS_SEGMENT_OFFSET );
    k_printf(" pass\n");
    
    k_printf("IDT init");
    k_init_IDT_table();
    k_load_IDTR(IDTR_ADDR);
    k_printf(" pass\n");

    k_printf("Total ram size check");
    k_check_total_RAM();
    k_printf(" pass, size = %d MB\n", k_get_total_RAM_size());


    k_printf("Keyboard and keyqueue activation");
    if (k_init_keyboard()) {
        k_printf(" pass\n");
    } else {
        k_printf("Keyboard is fked up.");
        for(;;);
    }


    k_printf("PIC init");
    k_init_PIC();
    k_mask_PIC_interrupt(0);
    pop_cli();
    k_printf(" pass\n");

    k_start_shell();





    // for(;;) {
    //     if(k_is_output_buffer_full()) {
    //         uint8_t temp_SC = k_get_scan_code();

    //         if (k_convert_SC_to_ASCII(temp_SC, &( vc_temp[0]), &flags)) {
    //             if((flags & KEY_FLAGS_DOWN)) {
    //                 if ((unsigned) vc_temp[0] < (unsigned) 0x80) k_print(i++, 20, vc_temp);
    //                 k_print_num(50, 0, vc_temp[0]);
    //                 if (vc_temp[0] == '0') {
    //                     // test interrupt
    //                     int temp = 1 / 0;
    //                 }
    //             }
    //         }
    //     }
    // }
    // k_key_data key_data;
    // for(;;) {
    //     if (k_get_key_from_key_queue(&key_data)) {
    //         if (key_data.flags & KEY_FLAGS_DOWN) {
    //             vc_temp[0] = key_data.ascii;
    //             k_print(i++, 20, vc_temp);

    //             if (vc_temp[0] == '0') {
    //                 // test interrupt
    //                 1 / 0;
    //             }
    //         }
    //     }
    // }

}
// void k_print(int x, int y, const char* str) {
//     k_char* screen = (void*) 0xB8000;
//     screen += (80 * y) + x;

//     for(int i=0; str[i] != 0; i++) screen[i].b_char = str[i];
// }

void k_print_num(int x, int y, unsigned char num) {
    k_char* screen = (void*) 0xB8000;

    screen += (y*80) + x;
    // screen[0].b_char = num;
    screen[0].b_char = (char) (num % 10 + '0');
    screen[-1].b_char = (char) ((num / 10) % 10 + '0');
    screen[-2].b_char = (char) ((num / 100) % 10 + '0');
    screen[-3].b_char = (char) ((num / 1000) % 10 + '0');
}