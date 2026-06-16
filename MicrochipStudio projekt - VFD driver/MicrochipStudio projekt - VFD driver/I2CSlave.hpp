#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <avr/interrupt.h>
#include <stdint.h>

void I2C_init(uint8_t address);
void I2C_stop(void);
// recv: called for each received data byte.
// req:  called when the master reads (status handshake).
// stop: called on an I2C STOP/repeated-START so the slave can reset any
//       partially-collected command state. May be NULL.
void I2C_setCallbacks(void (*recv)(uint8_t), void (*req)(), void (*stop)());

void I2C_setStatus(uint8_t status);

inline void __attribute__((always_inline)) I2C_transmitByte(uint8_t data)
{
	TWDR = data;
}

ISR(TWI_vect);

#endif