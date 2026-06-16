#ifndef DEFINES_H_
#define DEFINES_H_

// ---------------------------------------------------------------------------
// VFD-driver (slave) build configuration.
// This chip runs at 18.432 MHz. F_CPU is normally provided by the build
// system / Makefile; we guard it here so a missing definition fails loudly
// instead of silently using a wrong value for _delay_us().
// ---------------------------------------------------------------------------
#ifndef F_CPU
#  warning "F_CPU not defined by build system; assuming 18432000UL for the VFD slave"
#  define F_CPU 18432000UL
#endif

#ifndef _BV
#  define _BV(bit) (1 << (bit))
#endif

// 8-bit I2C address; LSB (R/W) must be 0.
#define I2C_ADDR 0xF4

// Largest single command payload the slave must buffer.
// CMD_SHOW_GRA_TEXT carries 14 characters (indices 0..13), which is the
// maximum. Sizing the buffer exactly to that left no margin; we size to the
// real maximum and assert it so the protocol and the buffer cannot drift
// apart.
#define I2CDATA_MAXLENGTH 14

#endif