#include <util/twi.h>
#include <avr/interrupt.h>

#include "I2CSlave.hpp"
#include "vfdprotocol.hpp"

static void (*I2C_recv)(uint8_t);
static void (*I2C_req)();

// Status byte returned to the master on a read. Starts READY.
static volatile uint8_t i2cStatus = STATUS_READY;

void I2C_setCallbacks(void (*recv)(uint8_t), void (*req)())
{
  I2C_recv = recv;
  I2C_req = req;
}

void I2C_setStatus(uint8_t status)
{
  i2cStatus = status;
}

void I2C_init(uint8_t address)
{
  cli();
  // load address into TWI address register (7-bit address in bits 7..1;
  // bit0 = TWGCE general-call enable, which stays 0 because the supplied
  // address has its low bit clear)
  TWAR = address;
  // set the TWCR to enable address matching and enable TWI, clear TWINT, enable TWI interrupt
  TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWINT) | (1<<TWEN);
  sei();
}

void I2C_stop(void)
{
  // clear acknowledge and enable bits
  cli();
  TWCR = 0;
  TWAR = 0;
  sei();
}

ISR(TWI_vect)
{
  switch(TW_STATUS)
  {
    case TW_SR_DATA_ACK:
      // received data from master, call the receive callback
      if (I2C_recv)
      {
        I2C_recv(TWDR);
      }
      TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
      break;
    case TW_ST_SLA_ACK:
    case TW_ST_DATA_ACK:
      // master is reading: notify the callback (bookkeeping) and place the
      // current status byte on the bus.
      if (I2C_req)
      {
        I2C_req();
      }
      TWDR = i2cStatus;
      TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
      break;
    case TW_ST_DATA_NACK:
    case TW_ST_LAST_DATA:
      // master signalled end of read; return to addressable idle state
      TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
      break;
    case TW_BUS_ERROR:
      // some sort of erroneous state, prepare TWI to be readdressed
      TWCR = 0;
      TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN); 
      break;
    default:
      TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
      break;
  }
} 