#include <util/twi.h>
#include <avr/interrupt.h>

#include "I2CSlave.hpp"
#include "vfdprotocol.hpp"

static void (*I2C_recv)(uint8_t);
static void (*I2C_req)();
static void (*I2C_stopCb)();

static volatile uint8_t i2cStatus = STATUS_READY;

void I2C_setCallbacks(void (*recv)(uint8_t), void (*req)(), void (*stop)())
{
	I2C_recv = recv;
	I2C_req = req;
	I2C_stopCb = stop;
}

void I2C_setStatus(uint8_t status)
{
	i2cStatus = status;
}

void I2C_init(uint8_t address)
{
	cli();
	TWAR = address;
	TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWINT) | (1<<TWEN);
	sei();
}

void I2C_stop(void)
{
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
		if (I2C_recv)
		{
			I2C_recv(TWDR);
		}
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		break;
		case TW_SR_STOP:
		// STOP or repeated START ended the write transaction. Reset any
		// half-collected command so the next transaction starts clean.
		if (I2C_stopCb)
		{
			I2C_stopCb();
		}
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		break;
		case TW_ST_SLA_ACK:
		case TW_ST_DATA_ACK:
		if (I2C_req)
		{
			I2C_req();
		}
		TWDR = i2cStatus;
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		break;
		case TW_ST_DATA_NACK:
		case TW_ST_LAST_DATA:
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		break;
		case TW_BUS_ERROR:
		TWCR = 0;
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		break;
		default:
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		break;
	}
}