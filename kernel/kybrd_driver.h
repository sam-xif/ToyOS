/* kybrd_driver.h */

#ifndef __KYBRD_DRIVER_H
#define __KYBRD_DRIVER_H

#include "inttypes.h"

#define KYBRD_CTRL_PORT 0x64
#define KYBRD_ENC_PORT 0x60

void kybrd_set_leds(byte leds);
void kybrd_ctrl_wait_write();
void kybrd_ctrl_wait_read();
byte kybrd_enc_read();
void kybrd_enable();
int kybrd_get_scanset();

#endif /* __KYBRD_DRIVER_H */