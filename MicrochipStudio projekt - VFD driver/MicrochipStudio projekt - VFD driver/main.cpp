#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.hpp"
#include "vfd.hpp"
#include "vfdprotocol.hpp"
#include "I2CSlave.hpp"

static unsigned int activeGrid = 0; // 0-17
static VfdState displayMemory[18] = {'\0'};
static volatile uint8_t i2cdata[I2CDATA_MAXLENGTH] = { UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX };
static volatile bool showUnderlines = false;
static volatile unsigned char modeCmdGraTextCurrentIndex = UINT8_MAX; // UINT8_MAX means we are not in CMD_SHOW_GRA_TEXT mode
static volatile unsigned char modeCmdNumTextCurrentIndex = UINT8_MAX; // UINT8_MAX means we are not in CMD_SHOW_NUM_TEXT mode
static volatile unsigned char modeCmdGraCharCurrentIndex = UINT8_MAX; // etc.
static volatile unsigned char modeCmdNumCharCurrentIndex = UINT8_MAX;
static volatile unsigned char modeCmdShowSpeCurrentIndex = UINT8_MAX;
static volatile unsigned char modeCmdHideSpeCurrentIndex = UINT8_MAX;
static volatile unsigned char modeCmdToggleSpeCurrentIndex = UINT8_MAX;
static volatile unsigned char modeCmdShowVolumeCurrentIndex = UINT8_MAX;
static volatile char modeCmdFinished = UINT8_MAX;
static unsigned int displayCounter = 0;
static volatile unsigned char displayOn = 1; // 0 or 1 or 2 (means "turn off display in the next loop and set this variable to 0")

static void graPuts(char const* text);
static void numPuts(char const* text);

// Called from the I2C ISR on a STOP/repeated-START. Abort any partially
// received command so a new transaction always begins by expecting a
// command byte. Without this, a transaction that ends mid-payload leaves a
// modeCmd* collector "armed", and the NEXT command byte is swallowed as
// payload - which silently drops whole commands (notably CMD_SHOW_GRA_TEXT).
static void I2C_resetReceiveState()
{
	modeCmdGraTextCurrentIndex = UINT8_MAX;
	modeCmdNumTextCurrentIndex = UINT8_MAX;
	modeCmdGraCharCurrentIndex = UINT8_MAX;
	modeCmdNumCharCurrentIndex = UINT8_MAX;
	modeCmdShowSpeCurrentIndex = UINT8_MAX;
	modeCmdHideSpeCurrentIndex = UINT8_MAX;
	modeCmdToggleSpeCurrentIndex = UINT8_MAX;
	modeCmdShowVolumeCurrentIndex = UINT8_MAX;
}

static void clearI2Cdata()
{
	for (int i = 0; i <= I2CDATA_MAXLENGTH - 1; i++)
	{
		i2cdata[i] = UINT8_MAX;
	}
}

static void clearGraDisplay()
{
	graPuts("              ");
}

static void clearNumDisplay()
{
	numPuts("      ");
}

static void shiftOut(uint8_t val)
{
	uint8_t i;

	for (i = 0; i < 8; i++) 
	{
		if ((val & 128) != 0)
		{
			PORTC |= _BV(PC0);
		}
		else
		{
			PORTC &= ~_BV(PC0);
		}
		
		val <<= 1;
		PORTC |= _BV(PC1); // 100 ns between these two (without delay)
		_delay_us(5);
		PORTC &= ~_BV(PC1);
		_delay_us(5);
	}
}

// called in interrupt of I2C receive, keep as fast as possible
static void I2C_received(uint8_t data)
{
	// the slave is collecting payload or about to accept a new command:
	// report BUSY to the master so it polls before sending the next command.
	// (cleared back to READY in the main loop once the finished command is
	// applied and we are idle again.)
	I2C_setStatus(STATUS_BUSY);

	if (modeCmdGraTextCurrentIndex != UINT8_MAX)
	{
		// accepting data for gra display
		// exactly 14 characters are accepted
		i2cdata[modeCmdGraTextCurrentIndex] = data;
		
		if (modeCmdGraTextCurrentIndex != 13)
		{
			modeCmdGraTextCurrentIndex++;
		}
		else
		{
			modeCmdGraTextCurrentIndex = UINT8_MAX;
			modeCmdFinished = CMD_SHOW_GRA_TEXT;
		}
	}
	else if (modeCmdNumTextCurrentIndex != UINT8_MAX)
	{
		// accepting data for num display
		// exactly 6 characters are accepted
		i2cdata[modeCmdNumTextCurrentIndex] = data;
		
		if (modeCmdNumTextCurrentIndex != 5)
		{
			modeCmdNumTextCurrentIndex++;
		}
		else
		{
			modeCmdNumTextCurrentIndex = UINT8_MAX;
			modeCmdFinished = CMD_SHOW_NUM_TEXT;
		}
	}
	else if (modeCmdGraCharCurrentIndex != UINT8_MAX)
	{
		// data contains the code of gra char to display
		// or the index to display the char on
		i2cdata[modeCmdGraCharCurrentIndex] = data;
	
		if (modeCmdGraCharCurrentIndex != 1)
		{
			modeCmdGraCharCurrentIndex++;
		}
		else
		{
			modeCmdGraCharCurrentIndex = UINT8_MAX;
			modeCmdFinished = CMD_SHOW_GRA_CHAR;
		}
	}
	else if (modeCmdNumCharCurrentIndex != UINT8_MAX)
	{
		// data contains the code of num char to display
		// or the index to display the char on
		i2cdata[modeCmdNumCharCurrentIndex] = data;
		
		if (modeCmdNumCharCurrentIndex != 1)
		{
			modeCmdNumCharCurrentIndex++;
		}
		else
		{
			modeCmdNumCharCurrentIndex = UINT8_MAX;
			modeCmdFinished = CMD_SHOW_NUM_CHAR;
		}
	}
	else if (modeCmdShowSpeCurrentIndex != UINT8_MAX)
	{
		// data contains the code of special char to display
		i2cdata[0] = data;
		modeCmdShowSpeCurrentIndex = UINT8_MAX;
		modeCmdFinished = CMD_SHOW_SPE;
	}
	else if (modeCmdHideSpeCurrentIndex != UINT8_MAX)
	{
		// data contains the code of special char to hide
		i2cdata[0] = data;
		modeCmdHideSpeCurrentIndex = UINT8_MAX;
		modeCmdFinished = CMD_HIDE_SPE;
	}
	else if (modeCmdToggleSpeCurrentIndex != UINT8_MAX)
	{
		// data contains the code of special char to toggle
		i2cdata[0] = data;
		modeCmdToggleSpeCurrentIndex = UINT8_MAX;
		modeCmdFinished = CMD_TOGGLE_SPE;
	}
	else if (modeCmdShowVolumeCurrentIndex != UINT8_MAX)
	{
		// data contains the value of volume 0-63
		i2cdata[0] = data;
		modeCmdShowVolumeCurrentIndex = UINT8_MAX;
		modeCmdFinished = CMD_SHOW_VOLUME;
	}
	else
	{
		if (data == CMD_CLEAR_GRA)
		{
			modeCmdFinished = CMD_CLEAR_GRA;
		}
		else if (data == CMD_CLEAR_NUM)
		{
			modeCmdFinished = CMD_CLEAR_NUM;
		}
		else if (data == CMD_CLEAR_ALL)
		{
			modeCmdFinished = CMD_CLEAR_ALL;
		}
		else if (data == CMD_SHOW_UNDERLINES)
		{
			showUnderlines = true;
			// single-byte command with no payload and no main-loop step:
			// we are immediately idle again.
			I2C_setStatus(STATUS_READY);
		}
		else if (data == CMD_HIDE_UNDERLINES)
		{
			showUnderlines = false;
			I2C_setStatus(STATUS_READY);
		}
		else if (data == CMD_SHOW_GRA_TEXT)
		{
			modeCmdGraTextCurrentIndex = 0;
		}
		else if (data == CMD_SHOW_NUM_TEXT)
		{
			modeCmdNumTextCurrentIndex = 0;
		}
		else if (data == CMD_SHOW_GRA_CHAR)
		{
			modeCmdGraCharCurrentIndex = 0;
		}
		else if (data == CMD_SHOW_NUM_CHAR)
		{
			modeCmdNumCharCurrentIndex = 0;
		}
		else if (data == CMD_SHOW_SPE)
		{
			modeCmdShowSpeCurrentIndex = 0;
		}
		else if (data == CMD_HIDE_SPE)
		{
			modeCmdHideSpeCurrentIndex = 0;
		}
		else if (data == CMD_TOGGLE_SPE)
		{
			modeCmdToggleSpeCurrentIndex = 0;
		}
		else if (data == CMD_SHOW_VOLUME)
		{
			modeCmdShowVolumeCurrentIndex = 0;
		}
		else if (data == CMD_TOGGLE_DISPLAY)
		{
			displayOn = displayOn == 1 ? 2 : 1;
			I2C_setStatus(STATUS_READY);
		}
	}
}

static void I2C_requested()
{
	// The protocol is one-way for data; on a read we report a single status
	// byte (READY / BUSY) so the master can poll before sending the next
	// command. The actual byte placed on the bus is set inside I2CSlave from
	// the value provided via I2C_setStatus(); nothing extra is needed here.
}

static VfdState getGraDisplayChar(char chr)
{
	// convert supplied character to graphic display character
	VfdState result;
	
	if (chr >= 32 && chr <= 126)
	{
		result = GraAsciiTable[chr - 32];
	}
	else
	{
		const VfdState* vfdStatePointer;
		
		switch ((unsigned char)chr)
		{
			case 0x9A:  /* š */
			case 0x8A:  /* Š */
			vfdStatePointer = &GraCaronS;
			break;

			case 0xBE:  /* ľ */
			vfdStatePointer = &GraCaronLowerL;
			break;

			case 0xBC:  /* Ľ */
			vfdStatePointer = &GraCaronUpperL;
			break;

			case 0xE8:  /* č */
			case 0xC8:  /* Č */
			vfdStatePointer = &GraCaronC;
			break;

			case 0x8D:  /* Ť */
			vfdStatePointer = &GraCaronUpperT;
			break;

			case 0x9D:  /* ť */
			vfdStatePointer = &GraCaronLowerT;
			break;

			case 0x9E:  /* ž */
			case 0x8E:  /* Ž */
			vfdStatePointer = &GraCaronZ;
			break;

			case 0xFD:  /* ý */
			vfdStatePointer = &GraAcuteLowerY;
			break;

			case 0xDD:  /* Ý */
			vfdStatePointer = &GraAcuteUpperY;
			break;

			case 0xE1:  /* á */
			vfdStatePointer = &GraAcuteLowerA;
			break;

			case 0xC1:  /* Á */
			vfdStatePointer = &GraAcuteUpperA;
			break;

			case 0xED:  /* í */
			vfdStatePointer = &GraAcuteLowerI;
			break;

			case 0xCD:  /* Í */
			vfdStatePointer = &GraAcuteUpperI;
			break;

			case 0xE9:  /* é */
			vfdStatePointer = &GraAcuteLowerE;
			break;

			case 0xC9:  /* É */
			vfdStatePointer = &GraAcuteUpperE;
			break;

			case 0xF3:  /* ó */
			vfdStatePointer = &GraAcuteLowerO;
			break;

			case 0xD3:  /* Ó */
			vfdStatePointer = &GraAcuteUpperO;
			break;

			case 0xFA:  /* ú */
			vfdStatePointer = &GraAcuteLowerU;
			break;

			case 0xDA:  /* Ú */
			vfdStatePointer = &GraAcuteUpperU;
			break;

			case 0xC4:  /* Ä */
			vfdStatePointer = &GraDiaeresisUpperA;
			break;

			case 0xE4:  /* ä */
			vfdStatePointer = &GraDiaeresisLowerA;
			break;

			case 0xF2:  /* ň */
			vfdStatePointer = &GraCaronLowerN;
			break;

			default:
			vfdStatePointer = &GraSpace;
			break;
		}
		
		memcpy_P(&result, vfdStatePointer, sizeof(VfdState));
	}
	
	return result;
}

static void graPuts(char const* text)
{
	for (unsigned char i = 0; i <= 13; i++)
	{
		displayMemory[4 + i] = getGraDisplayChar(text[i]);
	}
}

static void graPutc(char chr, char index)
{
	displayMemory[4 + index] = getGraDisplayChar(chr);
}

static VfdState getNumDisplayChar(char chr, const bool isEven)
{
	// convert supplied character to numeric display character
	const VfdState* vfdStatePointer;
	
	switch (chr)
	{
		case '0':
		vfdStatePointer = isEven ? &Num0a : &Num0b;
		break;
		case '1':
		vfdStatePointer = isEven ? &Num1a : &Num1b;
		break;
		case '2':
		vfdStatePointer = isEven ? &Num2a : &Num2b;
		break;
		case '3':
		vfdStatePointer = isEven ? &Num3a : &Num3b;
		break;
		case '4':
		vfdStatePointer = isEven ? &Num4a : &Num4b;
		break;
		case '5':
		vfdStatePointer = isEven ? &Num5a : &Num5b;
		break;
		case '6':
		vfdStatePointer = isEven ? &Num6a : &Num6b;
		break;
		case '7':
		vfdStatePointer = isEven ? &Num7a : &Num7b;
		break;
		case '8':
		vfdStatePointer = isEven ? &Num8a : &Num8b;
		break;
		case '9':
		vfdStatePointer = isEven ? &Num9a : &Num9b;
		break;
		case 'A':
		vfdStatePointer = isEven ? &NumAa : &NumAb;
		break;
		case 'F':
		vfdStatePointer = isEven ? &NumFa : &NumFb;
		break;
		case 'M':
		vfdStatePointer = isEven ? &NumMa : &NumMb;
		break;
		default:
		vfdStatePointer = isEven ? &NumSpacea : &NumSpaceb;
		break;
	}
	
	VfdState result;
	memcpy_P(&result, vfdStatePointer, sizeof(VfdState));
	
	return result;
}

static void numPutc(char chr, char index)
{
	const bool isEven = index % 2 == 0;
	const int displayMemoryIndex = index / 2;
	VfdState oldVfdState = displayMemory[displayMemoryIndex];
	VfdState newVfdState = getNumDisplayChar(chr, isEven);
	VfdState merged = oldVfdState;
	
	if (isEven)
	{
		// take segments A-N only, leave others as they were
		merged.chip1 = newVfdState.chip1;
		merged.chip2 = (merged.chip2 & ~0b00111111) | (newVfdState.chip2 & 0b00111111);
	}
	else
	{
		// take segments O-AB only, leave others as they were
		merged.chip2 = (merged.chip2 & ~0b11000000) | (newVfdState.chip2 & 0b11000000);
		merged.chip3 = newVfdState.chip3;
		merged.chip4 = (merged.chip4 & ~0b00001111) | (newVfdState.chip4 & 0b00001111);
	}
	
	displayMemory[displayMemoryIndex] = merged;
}

static void numPuts(char const* text)
{
	for (int i = 0; i <= 5; i++)
	{
		numPutc(text[i], i);
	}
}

static VfdState getSpeDisplayChar(char spe)
{
	// convert supplied character to special display character
	const VfdState* vfdStatePointer;
	
	switch (spe)
	{
		case SPE_SIRIUS:
		vfdStatePointer = &SpeSirius;
		break;
		case SPE_OUT:
		vfdStatePointer = &SpeOut;
		break;
		case SPE_HDMI:
		vfdStatePointer = &SpeHdmi;
		break;
		case SPE_ONE_SQUARED:
		vfdStatePointer = &SpeOneSquared;
		break;
		case SPE_TWO_SQUARED:
		vfdStatePointer = &SpeTwoSquared;
		break;
		case SPE_ARROW_LEFT:
		vfdStatePointer = &SpeArrowLeft;
		break;
		case SPE_ARROW_UP:
		vfdStatePointer = &SpeArrowUp;
		break;
		case SPE_ARROW_DOWN:
		vfdStatePointer = &SpeArrowDown;
		break;
		case SPE_COLON_UPPER:
		vfdStatePointer = &SpeNumColonUpper;
		break;
		case SPE_COLON_LOWER:
		vfdStatePointer = &SpeNumColonLower;
		break;
		case SPE_IPOD_CHARGE:
		vfdStatePointer = &SpeIpodCharge;
		break;
		case SPE_SPB:
		vfdStatePointer = &SpeSpb;
		break;
		case SPE_RECTANGLE1:
		vfdStatePointer = &SpeRectangle1;
		break;
		case SPE_SPA:
		vfdStatePointer = &SpeSpa;
		break;
		case SPE_PARTY:
		vfdStatePointer = &SpeParty;
		break;
		case SPE_HD:
		vfdStatePointer = &SpeHd;
		break;
		case SPE_TAG:
		vfdStatePointer = &SpeTag;
		break;
		case SPE_CINEMA_DSP:
		vfdStatePointer = &SpeCinemaDsp;
		break;
		case SPE_THREE_SQUARED:
		vfdStatePointer = &SpeThreeSquared;
		break;
		case SPE_STEREO:
		vfdStatePointer = &SpeStereo;
		break;
		case SPE_TUNED:
		vfdStatePointer = &SpeTuned;
		break;
		case SPE_ENHANCER:
		vfdStatePointer = &SpeEnhancer;
		break;
		case SPE_ADAPTIVE_DRC:
		vfdStatePointer = &SpeAdaptiveDrc;
		break;
		case SPE_PL:
		vfdStatePointer = &SpePL;
		break;
		case SPE_SW:
		vfdStatePointer = &SpeSW;
		break;
		case SPE_PR:
		vfdStatePointer = &SpePR;
		break;
		case SPE_L:
		vfdStatePointer = &SpeL;
		break;
		case SPE_C:
		vfdStatePointer = &SpeC;
		break;
		case SPE_R:
		vfdStatePointer = &SpeR;
		break;
		case SPE_SL:
		vfdStatePointer = &SpeSL;
		break;
		case SPE_SR:
		vfdStatePointer = &SpeSR;
		break;
		case SPE_SBL:
		vfdStatePointer = &SpeSBL;
		break;
		case SPE_SB:
		vfdStatePointer = &SpeSB;
		break;
		case SPE_SBR:
		vfdStatePointer = &SpeSBR;
		break;
		case SPE_ARROW_RIGHT:
		vfdStatePointer = &SpeArrowRight;
		break;
		case SPE_RECTANGLE2:
		vfdStatePointer = &SpeRectangle2;
		break;
		case SPE_MUTE:
		vfdStatePointer = &SpeMute;
		break;
		case SPE_ZONE2:
		vfdStatePointer = &SpeZone2;
		break;
		case SPE_ZONE3:
		vfdStatePointer = &SpeZone3;
		break;
		case SPE_SLEEP:
		vfdStatePointer = &SpeSleep;
		break;
		case SPE_VOLDB:
		vfdStatePointer = &SpeVolDb;
		break;
		default:
		vfdStatePointer = &SpeRectangle1;
		break;
	}
	
	VfdState result;
	memcpy_P(&result, vfdStatePointer, sizeof(VfdState));
	
	return result;
}

static void speOn(const VfdState newVfdState)
{
	// take AC-AJ from grids 0-2
	// take A and T-AJ from grid 3
	// leave the rest as they were
	//
	// NOTE: the original code used "x |= (x & ~mask) | (new & mask)", whose
	// leading |= made the "& ~mask" clear dead - segments could only ever be
	// turned ON, never replaced. This is a plain masked merge so the selected
	// bits take the new value and the rest are preserved.
	if (newVfdState.chip5 & 0b00010000)
	{
		// grid 0
		displayMemory[0].chip4 = (displayMemory[0].chip4 & ~0b11110000) | (newVfdState.chip4 & 0b11110000);
		displayMemory[0].chip5 = (displayMemory[0].chip5 & ~0b00001111) | (newVfdState.chip5 & 0b00001111);
	}
	else if (newVfdState.chip5 & 0b00100000)
	{
		// grid 1
		displayMemory[1].chip4 = (displayMemory[1].chip4 & ~0b11110000) | (newVfdState.chip4 & 0b11110000);
		displayMemory[1].chip5 = (displayMemory[1].chip5 & ~0b00001111) | (newVfdState.chip5 & 0b00001111);
	}
	else if (newVfdState.chip5 & 0b01000000)
	{
		// grid 2
		displayMemory[2].chip4 = (displayMemory[2].chip4 & ~0b11110000) | (newVfdState.chip4 & 0b11110000);
		displayMemory[2].chip5 = (displayMemory[2].chip5 & ~0b00001111) | (newVfdState.chip5 & 0b00001111);
	}
	else if (newVfdState.chip5 & 0b10000000)
	{
		// grid 3
		displayMemory[3].chip1 = (displayMemory[3].chip1 & ~0b00000001) | (newVfdState.chip1 & 0b00000001);
		displayMemory[3].chip3 = (displayMemory[3].chip3 & ~0b11111000) | (newVfdState.chip3 & 0b11111000);
		displayMemory[3].chip4 = newVfdState.chip4;
		displayMemory[3].chip5 = (displayMemory[3].chip5 & ~0b00001111) | (newVfdState.chip5 & 0b00001111);
	}
}

static void speOff(const VfdState newVfdState)
{
	// take AC-AJ from grids 0-2
	// take A and T-AJ from grid 3
	// leave the rest as they were
	if (newVfdState.chip5 & 0b00010000)
	{
		// grid 0
		displayMemory[0].chip4 &= ~(newVfdState.chip4 & 0b11110000);
		displayMemory[0].chip5 &= ~(newVfdState.chip5 & 0b00001111);
	}
	else if (newVfdState.chip5 & 0b00100000)
	{
		// grid 1
		displayMemory[1].chip4 &= ~(newVfdState.chip4 & 0b11110000);
		displayMemory[1].chip5 &= ~(newVfdState.chip5 & 0b00001111);
	}
	else if (newVfdState.chip5 & 0b01000000)
	{
		// grid 2
		displayMemory[2].chip4 &= ~(newVfdState.chip4 & 0b11110000);
		displayMemory[2].chip5 &= ~(newVfdState.chip5 & 0b00001111);
	}
	else if (newVfdState.chip5 & 0b10000000)
	{
		// grid 3
		displayMemory[3].chip1 &= ~(newVfdState.chip1 & 0b00000001);
		displayMemory[3].chip3 &= ~(newVfdState.chip3 & 0b11111000);
		displayMemory[3].chip4 &= ~newVfdState.chip4;
		displayMemory[3].chip5 &= ~(newVfdState.chip5 & 0b00001111);
	}
}

static void speToggle(const VfdState newVfdState)
{
	// take AC-AJ from grids 0-2
	// take A and T-AJ from grid 3
	// leave the rest as they were
	if (newVfdState.chip5 & 0b00010000)
	{
		// grid 0
		displayMemory[0].chip4 ^= newVfdState.chip4 & 0b11110000;
		displayMemory[0].chip5 ^= newVfdState.chip5 & 0b00001111;
	}
	else if (newVfdState.chip5 & 0b00100000)
	{
		// grid 1
		displayMemory[1].chip4 ^= newVfdState.chip4 & 0b11110000;
		displayMemory[1].chip5 ^= newVfdState.chip5 & 0b00001111;
	}
	else if (newVfdState.chip5 & 0b01000000)
	{
		// grid 2
		displayMemory[2].chip4 ^= newVfdState.chip4 & 0b11110000;
		displayMemory[2].chip5 ^= newVfdState.chip5 & 0b00001111;
	}
	else if (newVfdState.chip5 & 0b10000000)
	{
		// grid 3
		displayMemory[3].chip1 ^= newVfdState.chip1 & 0b00000001;
		displayMemory[3].chip3 ^= newVfdState.chip3 & 0b11111000;
		displayMemory[3].chip4 ^= newVfdState.chip4;
		displayMemory[3].chip5 ^= newVfdState.chip5 & 0b00001111;
	}
}

static void volDisplay(const char volume)
{
	// A sentinel volume of UINT8_MAX (0xFF) means "do not display volume".
	// NOTE: the original guard checked volDb == UINT16_MAX, but volDb is
	// 945 - volume*15 which can never reach UINT16_MAX, so that branch was
	// dead. Test the sentinel on the input instead.
	if ((unsigned char)volume == UINT8_MAX)
	{
		// do not display anything
		// set grid 3, segments C-S to 0, leave others as they were
		// also do not display minus sign (segment A)
		displayMemory[3].chip1 = (displayMemory[3].chip1 & ~0b11111110) | (0b00000000 & 0b11111110);
		displayMemory[3].chip2 = 0b00000000;
		displayMemory[3].chip3 = (displayMemory[3].chip3 & ~0b00000111) | (0b00000000 & 0b00000111);
	}
	else
	{
		unsigned int volDb = 945 - volume * 15; // -94.5 dB is maximum silence, 0 is maximum loudness

		const unsigned int segmentA = volDb / 100;
		const unsigned int segmentB = (volDb % 100) / 10;
		const unsigned int segmentC = volDb % 10;
		const VfdState* segmentAPointer = 0;
		const VfdState* segmentBPointer = 0;
		const VfdState* segmentCPointer = 0;
		
		switch (segmentA)
		{
			case 0:
			segmentAPointer = &Vol0a;
			break;
			case 1:
			segmentAPointer = &Vol1a;
			break;
			case 2:
			segmentAPointer = &Vol2a;
			break;
			case 3:
			segmentAPointer = &Vol3a;
			break;
			case 4:
			segmentAPointer = &Vol4a;
			break;
			case 5:
			segmentAPointer = &Vol5a;
			break;
			case 6:
			segmentAPointer = &Vol6a;
			break;
			case 7:
			segmentAPointer = &Vol7a;
			break;
			case 8:
			segmentAPointer = &Vol8a;
			break;
			case 9:
			segmentAPointer = &Vol9a;
			break;
		}
		
		switch (segmentB)
		{
			case 0:
			segmentBPointer = &Vol0b;
			break;
			case 1:
			segmentBPointer = &Vol1b;
			break;
			case 2:
			segmentBPointer = &Vol2b;
			break;
			case 3:
			segmentBPointer = &Vol3b;
			break;
			case 4:
			segmentBPointer = &Vol4b;
			break;
			case 5:
			segmentBPointer = &Vol5b;
			break;
			case 6:
			segmentBPointer = &Vol6b;
			break;
			case 7:
			segmentBPointer = &Vol7b;
			break;
			case 8:
			segmentBPointer = &Vol8b;
			break;
			case 9:
			segmentBPointer = &Vol9b;
			break;
		}
		
		switch (segmentC)
		{
			case 0:
			segmentCPointer = &Vol0c;
			break;
			case 5:
			segmentCPointer = &Vol5c;
			break;
		}
		
		VfdState segmentAVfdState;
		memcpy_P(&segmentAVfdState, segmentAPointer, sizeof(VfdState));
		VfdState segmentBVfdState;
		memcpy_P(&segmentBVfdState, segmentBPointer, sizeof(VfdState));
		VfdState segmentCVfdState;
		memcpy_P(&segmentCVfdState, segmentCPointer, sizeof(VfdState));
		
		// combine all segments into C-S for grid 3
		// leave others as they were
		
		displayMemory[3].chip1 = (displayMemory[3].chip1 & ~0b11111100) | ((segmentAVfdState.chip1 | segmentBVfdState.chip1 | segmentCVfdState.chip1) & 0b11111100);
		displayMemory[3].chip2 = segmentAVfdState.chip2 | segmentBVfdState.chip2 | segmentCVfdState.chip2;
		displayMemory[3].chip3 = (displayMemory[3].chip3 & ~0b00000111) | ((segmentAVfdState.chip3 | segmentBVfdState.chip3 | segmentCVfdState.chip3) & 0b00000111);
		
		if (volDb == 0)
		{
			// do not display minus sign
			displayMemory[3].chip1 &= ~0b00000010;
		}
		else
		{
			// display minus sign
			displayMemory[3].chip1 |= 0b00000010;
		}
	}
}

int main(void)
{
	// set pins as output:
	// PC0 - shift reg. DATA
	// PC1 - shift reg. CLK
	// PC2 - shift reg. LATCH
	// PC3 - shift reg. MR/
	DDRC = 0b00001111;
	
	// initialize I2C slave
	I2C_setCallbacks(I2C_received, I2C_requested, I2C_resetReceiveState);
	I2C_init(I2C_ADDR);
	I2C_setStatus(STATUS_READY);
		
	PORTC |= _BV(PC3); // MR/, 0 clears whole register
	
	// clear all
	PORTC &= ~_BV(PC2); // LATCH, 1 active
	for (char i = 1; i <= 7; i++)
	{
		shiftOut(~0b00000000);
	}
	PORTC |= _BV(PC2); // LATCH, 1 active
	
	sei(); // enable global interrupts
	
	graPuts("-R\xE1""dio by Mek-");
	
    while (1) 
    {	
		if (modeCmdFinished != UINT8_MAX)
		{
			// snapshot the finished-command id with interrupts briefly off so
			// it cannot change between the test above and the switch below.
			char finished;
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				finished = modeCmdFinished;
				modeCmdFinished = UINT8_MAX;
			}

			switch (finished)
			{
				case CMD_SHOW_GRA_TEXT:
				// TEMP DIAG: prove this case runs at all
				displayMemory[10] = getGraDisplayChar('Y');
				graPuts((const char *)i2cdata);
				clearI2Cdata();
				break;
				/*				case CMD_SHOW_GRA_TEXT:
				graPuts((const char *)i2cdata);
				clearI2Cdata();
				break;*/
				case CMD_SHOW_NUM_TEXT:
				numPuts((const char *)i2cdata);
				clearI2Cdata();
				break;
				case CMD_CLEAR_GRA:
				clearGraDisplay();
				break;
				case CMD_CLEAR_NUM:
				clearNumDisplay();
				break;
				case CMD_CLEAR_ALL:
				{
					VfdState graChar;
					graChar = getGraDisplayChar(' '); // happens to be all zeroes, turns off everything including Spe segments
				
					for (unsigned char i = 0; i <= 17; i++)
					{
						displayMemory[i] = graChar;
					}
				}
				break;
				case CMD_SHOW_GRA_CHAR:
				graPutc(i2cdata[0], i2cdata[1]);
				clearI2Cdata();
				break;
				case CMD_SHOW_NUM_CHAR:
				numPutc(i2cdata[0], i2cdata[1]);
				clearI2Cdata();
				break;
				case CMD_SHOW_SPE:
				speOn(getSpeDisplayChar(i2cdata[0]));
				clearI2Cdata();
				break;
				case CMD_HIDE_SPE:
				speOff(getSpeDisplayChar(i2cdata[0]));
				clearI2Cdata();
				break;
				case CMD_TOGGLE_SPE:
				speToggle(getSpeDisplayChar(i2cdata[0]));
				clearI2Cdata();
				break;
				case CMD_SHOW_VOLUME:
				volDisplay(i2cdata[0]);
				clearI2Cdata();
				break;
			}

			// command applied; if no new frame has started in the meantime,
			// report READY again so the master may send the next command.
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				if (modeCmdFinished == UINT8_MAX
					&& modeCmdGraTextCurrentIndex == UINT8_MAX
					&& modeCmdNumTextCurrentIndex == UINT8_MAX
					&& modeCmdGraCharCurrentIndex == UINT8_MAX
					&& modeCmdNumCharCurrentIndex == UINT8_MAX
					&& modeCmdShowSpeCurrentIndex == UINT8_MAX
					&& modeCmdHideSpeCurrentIndex == UINT8_MAX
					&& modeCmdToggleSpeCurrentIndex == UINT8_MAX
					&& modeCmdShowVolumeCurrentIndex == UINT8_MAX)
				{
					I2C_setStatus(STATUS_READY);
				}
			}
		}
		
		if (displayOn == 1)
		{
			if (displayCounter >= 250)
			{
				displayCounter = 0;
		
				if (activeGrid == 17)
				{
					activeGrid = 0;
				}
				else
				{
					activeGrid++;
				}

				unsigned char chip7 = 0b00000000;
				unsigned char chip6 = 0b00000000;
				unsigned char chip5 = 0b00000000;

				switch (activeGrid)
				{
					case 0:
					chip5 = 0b00010000;
					break;
					case 1:
					chip5 = 0b00100000;
					break;
					case 2:
					chip5 = 0b01000000;
					break;
					case 3:
					chip5 = 0b10000000;
					break;
					// beware, order of grids is changed below
					// because on the VFD display positions of graphical characters (and respective grids) are reversed
					case 4:
					chip7 = 0b00100000; // highest 2 bits are unused
					break;
					case 5:
					chip7 = 0b00010000;
					break;
					case 6:
					chip7 = 0b00001000;
					break;
					case 7:
					chip7 = 0b00000100;
					break;
					case 8:
					chip7 = 0b00000010;
					break;
					case 9:
					chip7 = 0b00000001;
					break;
					case 10:
					chip6 = 0b10000000;
					break;
					case 11:
					chip6 = 0b01000000;
					break;
					case 12:
					chip6 = 0b00100000;
					break;
					case 13:
					chip6 = 0b00010000;
					break;
					case 14:
					chip6 = 0b00001000;
					break;
					case 15:
					chip6 = 0b00000100;
					break;
					case 16:
					chip6 = 0b00000010;
					break;
					case 17:
					chip6 = 0b00000001;
					break;
				}
		
				// add underlines
				if (activeGrid >= 4)
				{
					if (showUnderlines)
					{
						displayMemory[activeGrid].chip5 |= 0b00001000;
					}
					else
					{
						displayMemory[activeGrid].chip5 &= ~0b00001000;
					}
				}
		
				PORTC &= ~_BV(PC2); // LATCH, 1 active
				shiftOut(~displayMemory[activeGrid].chip1);
				shiftOut(~displayMemory[activeGrid].chip2);
				shiftOut(~displayMemory[activeGrid].chip3);
				shiftOut(~displayMemory[activeGrid].chip4);
				shiftOut(~(chip5 | displayMemory[activeGrid].chip5)); // only lowest 4 bits belong to grids, so the rest is always set to 0000 (1111 inverted)
				shiftOut(~chip6);
				shiftOut(~chip7);
				PORTC |= _BV(PC2); // LATCH, 1 active
			}
			else
			{
				displayCounter++;
			}
		}
		else
		{
			if (displayOn == 2)
			{
				displayOn = 0;
				
				PORTC &= ~_BV(PC2); // LATCH, 1 active
				
				for (unsigned char k = 1; k <= 7; k++)
				{
					shiftOut(UINT8_MAX);
				}
				
				PORTC |= _BV(PC2); // LATCH, 1 active
			}
		}
   }
}