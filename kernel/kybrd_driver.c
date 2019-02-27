/* kybrd_driver.c */

#include "kybrd_driver.h"
#include "utils.h"

// Sets keyboard LEDS
void kybrd_set_leds(byte leds) 
{
    kybrd_wait_write();
    
    writeport(KYBRD_ENC_PORT, 0xed); // 0xed is set leds command
    writeport(KYBRD_ENC_PORT, leds);
}

// Blocks until the keyboard controller is ready to be written to
void kybrd_ctrl_wait_write() 
{
    while (TRUE) {
        if (readport(KYBRD_CTRL_PORT) & 0x2 == 0) break;
    }
}

// Blocks until the keyboard can be read from
void kybrd_ctrl_wait_read()
{
    while (TRUE) {
        if (readport(KYBRD_CTRL_PORT) & 0x1 == 1) break;
    }
}

byte kybrd_enc_read()
{
    return readport(KYBRD_ENC_PORT);
}

// Enables the keyboard and enables keyboard interrupts
void kybrd_enable() 
{
    kybrd_ctrl_wait_write();
    writeport(KYBRD_CTRL_PORT, 0xae);
    
    kybrd_ctrl_wait_write();
    writeport(KYBRD_CTRL_PORT, 0x20); // read cmd byte
    
    kybrd_ctrl_wait_read();
    byte ctrl_cmd_byte = readport(KYBRD_ENC_PORT);
    
    kybrd_ctrl_wait_write();
    writeport(KYBRD_CTRL_PORT, 0x60); // write cmd byte
    
    // Set enable interrupts to true (bit 0)
    // Set BAT to completed (bit 2)
    ctrl_cmd_byte |= 0x1;
    ctrl_cmd_byte |= 0x4;
    
    kybrd_ctrl_wait_write();
    writeport(KYBRD_ENC_PORT, ctrl_cmd_byte);
}