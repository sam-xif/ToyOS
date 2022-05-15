/* kybrd_driver.c */

#include "kybrd_driver.h"
#include "utils.h"
#include "kmain.h"

// Sets keyboard LEDS
void kybrd_set_leds(byte leds) 
{
    kybrd_ctrl_wait_write();
    
    writeport(KYBRD_ENC_PORT, 0xed); // 0xed is set leds command
    writeport(KYBRD_ENC_PORT, leds);
}

// Blocks until the keyboard controller is ready to be written to
void kybrd_ctrl_wait_write() 
{
    while (TRUE) {
        if ((readport(KYBRD_CTRL_PORT) & 0x2) == 0) break;
    }
}

// Blocks until the keyboard can be read from
void kybrd_ctrl_wait_read()
{
    while (TRUE) {
        if ((readport(KYBRD_CTRL_PORT) & 0x1) == 1) break;
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

int kybrd_id()
{
    kybrd_ctrl_wait_write();
    writeport(KYBRD_CTRL_PORT, 0xF2);

}

int kybrd_enable_scanning()
{
    kybrd_ctrl_wait_write();
    writeport(KYBRD_ENC_PORT, 0xF4);

    kybrd_ctrl_wait_read();
    byte response = readport(KYBRD_ENC_PORT);
    if (response == 0xFA) {
        return 1;
    } else {
        return -1;
    }
}


int kybrd_issue_command(byte command)
{
    kybrd_ctrl_wait_write();
    writeport(KYBRD_ENC_PORT, command);

    kybrd_ctrl_wait_read();
    byte response = readport(KYBRD_ENC_PORT);
    if (response == 0xFA) {
        return 1;
    } else {
        return -1;
    }
}

int kybrd_disable_scanning()
{
    kybrd_ctrl_wait_write();
    writeport(KYBRD_ENC_PORT, 0xF5);

    kybrd_ctrl_wait_read();
    byte response = readport(KYBRD_ENC_PORT);
    if (response == 0xFA) {
        return 1;
    } else {
        return -1;
    }
}

// Gets the current scan set that the keyboard is using 
int kybrd_get_scanset()
{
    kybrd_ctrl_wait_write();
    writeport(KYBRD_ENC_PORT, 0xF0); // this is the set scanset command??
    //kybrd_ctrl_wait_write();
    //writeport(KYBRD_ENC_PORT, 0x00);
    kybrd_ctrl_wait_write();
    writeport(KYBRD_ENC_PORT, 0x03);

   
    kybrd_ctrl_wait_read();
    byte cmd_response = readport(KYBRD_ENC_PORT);
    if (cmd_response == 0xFA) {
        kprint(&vidptr, "GET SCANSET COMMAND ACKNOWLEDGED", LIGHTGREY);
        knewline(&vidptr);
    }

    kybrd_ctrl_wait_read();
    byte scanset_response = readport(KYBRD_ENC_PORT);

    if (scanset_response == 0xFA) {
        kprint(&vidptr, "SCANSET SET", LIGHTGREY);
        knewline(&vidptr);
        // TODO: Add another function to set scanset
        return 0x01;//readport(KYBRD_ENC_PORT);
    } else {
        char s[2];
        byte_to_hex(scanset_response, s);
        kprintn(&vidptr, s, 2, LIGHTGREY);
        knewline(&vidptr); 
        return -2;
    }
   /* 
    kybrd_ctrl_wait_write();
    // Writing Bit 0 outputs the current scan set to port 0x60 (encoder port)
    writeport(KYBRD_ENC_PORT, 0x00);
    
    byte response1 = readport(KYBRD_ENC_PORT);
    // 0xFA is the acknowledged code
    while (1) {
        switch (response1) {
            case 0xFA: {
                kybrd_ctrl_wait_read();
                response1 = readport(KYBRD_ENC_PORT);
                break;
            }
            case 0xFE: {
                return -1;
            }
            default: return response1;
        }
    }
    */
}
