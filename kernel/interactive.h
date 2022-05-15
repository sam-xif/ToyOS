/**
 * Provides an interface for executing hand-entered machine code
 */

#ifndef __INTERACTIVE_H
#define __INTERACTIVE_H

/**
 * Executes the machine code given by the given hexadecimal string.
 * Returns a negative value if there is an error in the string:
 * i.e. it is not valid hex digits
 */
int execute(char *str);

#endif /* __INTERACTIVE_H */
