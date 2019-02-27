/* utils.h
 * Defines useful assembly language functions
 */

#ifndef __UTILS_H
#define __UTILS_H

#include "inttypes.h"

// Reads from the given I/O port
extern byte readport(byte port);

// Write the given byte to the given I/O port
extern void writeport(byte port, byte cmd);

#endif /* __UTILS_H */