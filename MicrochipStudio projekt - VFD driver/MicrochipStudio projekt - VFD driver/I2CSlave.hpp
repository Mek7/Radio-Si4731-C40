#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <avr/interrupt.h>
#include <stdint.h>

void I2C_init(uint8_t address);
void I2C_stop(void);
void I2C_setCallbacks(void (*recv)(uint8_t), void (*req)());

// Set the status byte that the slave returns to the master on an I2C read.
// The protocol is one-way for data; this lets the master poll READY / BUSY
// before sending the next command. Safe to call from the I2C ISR or main
// context (single-byte write is atomic on AVR).
void I2C_setStatus(uint8_t status);

inline void __attribute__((always_inline)) I2C_transmitByte(uint8_t data)
{
  TWDR = data;
}

ISR(TWI_vect);

#endif