// this file MUST be saved in ANSI encoding, not UTF-8 - because of the diacritic chars which should take up only one byte

#include <Arduino.h>
#include <SI4735.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.hpp"
#include "vfdprotocol.hpp"
#include "irmp.hpp"

static SI4735 si4735;
static unsigned char volume = DEFAULT_VOLUME;
static volatile bool readFromRTC = false;
static volatile char timer2OverflowCounter = 0;
//static unsigned int dtSeconds = UINT16_MAX; // set some nonsense values so that they are overwritten by correct values on startup
static unsigned int dtMinutes = UINT16_MAX;
static unsigned int dtHours = UINT16_MAX;
static unsigned int dtDayInMonth = UINT16_MAX;
static unsigned int dtDayInWeek = UINT16_MAX;
static unsigned int dtMonth = UINT16_MAX;
static unsigned int dtYear = UINT16_MAX; // two-place year
static const char* dayNames[] = { "Pon", "Uto", "Str", "Štv", "Pia", "Sob", "Ned" };
static const char* fmString = "      FM      ";
static char *radioText; // partial only, dynamic length + nul character
static char *stationName; // partial only, exactly 8 characters + nul character
static char completeStationName[STATION_NAME_CHARS]; // 8 characters without nul character
static char completeRadioText[RADIOTEXT_CHARS];
static volatile bool readRDS = false;
static bool rdsSyncedFreshly = false;
static unsigned char radioTextLostCounter = 0;
static volatile bool displayRadioText = false; // if false, displays station name, if true, displays (scrolling) radio text
static volatile char radioTextScrollingStartIndex = 0;
static volatile char radioTextScrollingEndIndex = 0;
static volatile char initialScrollDelayCounter = 0; // seconds for how much the radiotext is not scrolled
static volatile char radioTextDelayCounter = 0; // seconds for how much is station name shown, then radio text is shown
static char graDisplayMemory[GRA_DISPLAY_CHARS]; // buffer to avoid flickering, data is sent to display only when something differs
static IRMP_DATA irmp_data;
static unsigned char specialMode = 0;
static unsigned char irLearningMode = 0;
static bool isMuted = false;
static bool forceMono = false;
static bool isStereoDisplayOn = false;
static bool isTunedDisplayOn = false;
static volatile bool setupFinished = false;
static unsigned int setupRTCCounter = 0;
static volatile unsigned char displayedSpeForStandbyIndex = 0; // blinking spe segments while radio is off
static unsigned char oldDisplayedSpeIndex = 1; // blinking spe segments while radio is off
static unsigned long lastSettingsChange = 0;
static unsigned int lastFreqMhz = DEFAULT_FREQ_MHZ; // these are used when switching bands
static unsigned int lastFreqKhz = DEFAULT_FREQ_KHZ;
static unsigned char trebleBass = 0;
static unsigned int stereoAndTunedUpdateCounter = 0;

static const unsigned char* LAST_MODE_ADDRESS = (unsigned char*)0; // 1 B
static const unsigned char* LAST_FREQ_KHZ_ADDRESS = (unsigned char*)1; // 2 B
static const unsigned char* LAST_FREQ_MHZ_ADDRESS = (unsigned char*)3; // 2 B
static const unsigned char* VOLUME_ADDRESS = (unsigned char*)5; // 1 B (number 0 - 63)
static const unsigned char* TREBLEBASS_ADDRESS = (unsigned char*)6; // 1 B
static IRMP_DATA irmp_program0;
static unsigned int freqProgram0am;
static unsigned int freqProgram0fm;
const unsigned char* PROGRAM0_ADDRESS = (unsigned char*)7; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM0_AM_FREQ_ADDRESS = (unsigned char*)13; // 2 B
const unsigned char* PROGRAM0_FM_FREQ_ADDRESS = (unsigned char*)15; // 2 B
static IRMP_DATA irmp_program1;
static unsigned int freqProgram1am;
static unsigned int freqProgram1fm;
const unsigned char* PROGRAM1_ADDRESS = (unsigned char*)17; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM1_AM_FREQ_ADDRESS = (unsigned char*)23; // 2 B
const unsigned char* PROGRAM1_FM_FREQ_ADDRESS = (unsigned char*)25; // 2 B
static IRMP_DATA irmp_program2;
static unsigned int freqProgram2am;
static unsigned int freqProgram2fm;
const unsigned char* PROGRAM2_ADDRESS = (unsigned char*)27; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM2_AM_FREQ_ADDRESS = (unsigned char*)33; // 2 B
const unsigned char* PROGRAM2_FM_FREQ_ADDRESS = (unsigned char*)35; // 2 B
static IRMP_DATA irmp_program3;
static unsigned int freqProgram3am;
static unsigned int freqProgram3fm;
const unsigned char* PROGRAM3_ADDRESS = (unsigned char*)37; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM3_AM_FREQ_ADDRESS = (unsigned char*)43; // 2 B
const unsigned char* PROGRAM3_FM_FREQ_ADDRESS = (unsigned char*)45; // 2 B
static IRMP_DATA irmp_program4;
static unsigned int freqProgram4am;
static unsigned int freqProgram4fm;
const unsigned char* PROGRAM4_ADDRESS = (unsigned char*)47; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM4_AM_FREQ_ADDRESS = (unsigned char*)53; // 2 B
const unsigned char* PROGRAM4_FM_FREQ_ADDRESS = (unsigned char*)55; // 2 B
static IRMP_DATA irmp_program5;
static unsigned int freqProgram5am;
static unsigned int freqProgram5fm;
const unsigned char* PROGRAM5_ADDRESS = (unsigned char*)57; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM5_AM_FREQ_ADDRESS = (unsigned char*)63; // 2 B
const unsigned char* PROGRAM5_FM_FREQ_ADDRESS = (unsigned char*)65; // 2 B
static IRMP_DATA irmp_program6;
static unsigned int freqProgram6am;
static unsigned int freqProgram6fm;
const unsigned char* PROGRAM6_ADDRESS = (unsigned char*)67; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM6_AM_FREQ_ADDRESS = (unsigned char*)73; // 2 B
const unsigned char* PROGRAM6_FM_FREQ_ADDRESS = (unsigned char*)75; // 2 B
static IRMP_DATA irmp_program7;
static unsigned int freqProgram7am;
static unsigned int freqProgram7fm;
const unsigned char* PROGRAM7_ADDRESS = (unsigned char*)77; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM7_AM_FREQ_ADDRESS = (unsigned char*)83; // 2 B
const unsigned char* PROGRAM7_FM_FREQ_ADDRESS = (unsigned char*)85; // 2 B
static IRMP_DATA irmp_program8;
static unsigned int freqProgram8am;
static unsigned int freqProgram8fm;
const unsigned char* PROGRAM8_ADDRESS = (unsigned char*)87; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM8_AM_FREQ_ADDRESS = (unsigned char*)93; // 2 B
const unsigned char* PROGRAM8_FM_FREQ_ADDRESS = (unsigned char*)95; // 2 B
static IRMP_DATA irmp_program9;
static unsigned int freqProgram9am;
static unsigned int freqProgram9fm;
const unsigned char* PROGRAM9_ADDRESS = (unsigned char*)97; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM9_AM_FREQ_ADDRESS = (unsigned char*)103; // 2 B
const unsigned char* PROGRAM9_FM_FREQ_ADDRESS = (unsigned char*)105; // 2 B

static IRMP_DATA irmp_program10;
static unsigned int freqProgram10am;
static unsigned int freqProgram10fm;
const unsigned char* PROGRAM10_ADDRESS = (unsigned char*)107; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM10_AM_FREQ_ADDRESS = (unsigned char*)113; // 2 B
const unsigned char* PROGRAM10_FM_FREQ_ADDRESS = (unsigned char*)115; // 2 B
static IRMP_DATA irmp_program11;
static unsigned int freqProgram11am;
static unsigned int freqProgram11fm;
const unsigned char* PROGRAM11_ADDRESS = (unsigned char*)117; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM11_AM_FREQ_ADDRESS = (unsigned char*)123; // 2 B
const unsigned char* PROGRAM11_FM_FREQ_ADDRESS = (unsigned char*)125; // 2 B
static IRMP_DATA irmp_program12;
static unsigned int freqProgram12am;
static unsigned int freqProgram12fm;
const unsigned char* PROGRAM12_ADDRESS = (unsigned char*)127; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM12_AM_FREQ_ADDRESS = (unsigned char*)133; // 2 B
const unsigned char* PROGRAM12_FM_FREQ_ADDRESS = (unsigned char*)135; // 2 B
static IRMP_DATA irmp_program13;
static unsigned int freqProgram13am;
static unsigned int freqProgram13fm;
const unsigned char* PROGRAM13_ADDRESS = (unsigned char*)137; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM13_AM_FREQ_ADDRESS = (unsigned char*)143; // 2 B
const unsigned char* PROGRAM13_FM_FREQ_ADDRESS = (unsigned char*)145; // 2 B
static IRMP_DATA irmp_program14;
static unsigned int freqProgram14am;
static unsigned int freqProgram14fm;
const unsigned char* PROGRAM14_ADDRESS = (unsigned char*)147; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM14_AM_FREQ_ADDRESS = (unsigned char*)153; // 2 B
const unsigned char* PROGRAM14_FM_FREQ_ADDRESS = (unsigned char*)155; // 2 B
static IRMP_DATA irmp_program15;
static unsigned int freqProgram15am;
static unsigned int freqProgram15fm;
const unsigned char* PROGRAM15_ADDRESS = (unsigned char*)157; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM15_AM_FREQ_ADDRESS = (unsigned char*)163; // 2 B
const unsigned char* PROGRAM15_FM_FREQ_ADDRESS = (unsigned char*)165; // 2 B
static IRMP_DATA irmp_program16;
static unsigned int freqProgram16am;
static unsigned int freqProgram16fm;
const unsigned char* PROGRAM16_ADDRESS = (unsigned char*)167; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM16_AM_FREQ_ADDRESS = (unsigned char*)173; // 2 B
const unsigned char* PROGRAM16_FM_FREQ_ADDRESS = (unsigned char*)175; // 2 B
static IRMP_DATA irmp_program17;
static unsigned int freqProgram17am;
static unsigned int freqProgram17fm;
const unsigned char* PROGRAM17_ADDRESS = (unsigned char*)177; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM17_AM_FREQ_ADDRESS = (unsigned char*)183; // 2 B
const unsigned char* PROGRAM17_FM_FREQ_ADDRESS = (unsigned char*)185; // 2 B
static IRMP_DATA irmp_program18;
static unsigned int freqProgram18am;
static unsigned int freqProgram18fm;
const unsigned char* PROGRAM18_ADDRESS = (unsigned char*)187; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM18_AM_FREQ_ADDRESS = (unsigned char*)193; // 2 B
const unsigned char* PROGRAM18_FM_FREQ_ADDRESS = (unsigned char*)195; // 2 B
static IRMP_DATA irmp_program19;
static unsigned int freqProgram19am;
static unsigned int freqProgram19fm;
const unsigned char* PROGRAM19_ADDRESS = (unsigned char*)197; // 6 B - sizeof(IRMP_DATA)
const unsigned char* PROGRAM19_AM_FREQ_ADDRESS = (unsigned char*)203; // 2 B
const unsigned char* PROGRAM19_FM_FREQ_ADDRESS = (unsigned char*)205; // 2 B

static IRMP_DATA irmp_r;
const unsigned char* R_ADDRESS = (unsigned char*)207; // 6 B - sizeof(IRMP_DATA)
static IRMP_DATA irmp_f;
const unsigned char* F_ADDRESS = (unsigned char*)213; // 6 B - sizeof(IRMP_DATA)
static IRMP_DATA irmp_mute;
const unsigned char* MUTE_ADDRESS = (unsigned char*)220; // 6 B - sizeof(IRMP_DATA)
static IRMP_DATA irmp_standby;
const unsigned char* STANDBY_ADDRESS = (unsigned char*)226; // 6 B - sizeof(IRMP_DATA)
static IRMP_DATA irmp_band;
const unsigned char* BAND_ADDRESS = (unsigned char*)232; // 6 B - sizeof(IRMP_DATA)
static IRMP_DATA irmp_program;
const unsigned char* PROGRAM_ADDRESS = (unsigned char*)238; // 6 B - sizeof(IRMP_DATA)
static IRMP_DATA irmp_monostereo;
const unsigned char* MONOSTEREO_ADDRESS = (unsigned char*)244; // 6 B - sizeof(IRMP_DATA)
const unsigned char* FORCEMONO_ADDRESS = (unsigned char*)250; // 1 B
static IRMP_DATA irmp_volumedown;
const unsigned char* VOLUMEDOWN_ADDRESS = (unsigned char*)251; // 6 B - sizeof(IRMP_DATA)
static IRMP_DATA irmp_volumeup;
const unsigned int* VOLUMEUP_ADDRESS = (unsigned int*)257; // 6 B - sizeof(IRMP_DATA)
static IRMP_DATA irmp_bassdown;
const unsigned int* BASSDOWN_ADDRESS = (unsigned int*)263; // 6 B - sizeof(IRMP_DATA)
static IRMP_DATA irmp_bassup;
const unsigned int* BASSUP_ADDRESS = (unsigned int*)269; // 6 B - sizeof(IRMP_DATA)
static IRMP_DATA irmp_trebledown;
const unsigned int* TREBLEDOWN_ADDRESS = (unsigned int*)275; // 6 B - sizeof(IRMP_DATA)
static IRMP_DATA irmp_trebleup;
const unsigned int* TREBLEUP_ADDRESS = (unsigned int*)281; // 6 B - sizeof(IRMP_DATA)
static IRMP_DATA irmp_display;
const unsigned int* DISPLAY_ADDRESS = (unsigned int*)287; // 6 B - sizeof(IRMP_DATA)

static const char speForStandby[] = {
	SPE_ARROW_DOWN,
	SPE_ARROW_UP,
	SPE_ARROW_LEFT,
	SPE_OUT,
	SPE_ONE_SQUARED,
	SPE_TWO_SQUARED,
	SPE_HDMI,
	SPE_SIRIUS,
	SPE_HD,
	SPE_TAG,
	SPE_ENHANCER,
	SPE_ADAPTIVE_DRC,
	SPE_CINEMA_DSP,
	SPE_THREE_SQUARED,
	SPE_STEREO,
	SPE_TUNED,
	SPE_IPOD_CHARGE,
	SPE_PARTY,
	SPE_SPA,
	SPE_RECTANGLE1,
	SPE_SPB,
	SPE_SLEEP,
	SPE_ZONE2,
	SPE_ZONE3,
	SPE_MUTE,
	SPE_RECTANGLE2,
	SPE_ARROW_RIGHT,
	SPE_PL,
	SPE_SW,
	SPE_PR,
	SPE_L,
	SPE_C,
	SPE_R,
	SPE_SL,
	SPE_SR,
	SPE_SBL,
	SPE_SB,
	SPE_SBR
}; 

static unsigned int weekday(unsigned int y, unsigned int m, unsigned int d) {
	// function must return one-based day index (1 = Monday, 7 = Sunday), but by default 0 = Sunday
	unsigned int result = (d += m < 3 ? y-- : y - 2 , 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7;
	
	if (result == 0)
	{
		result = 7;
	}
	
	return result;
}

static void learnIrCode(const void* eepromAddress, IRMP_DATA* irmpDataToSet, IRMP_DATA* irmpDataVariable)
{
	eeprom_write_block(irmpDataToSet, (unsigned char *)eepromAddress, sizeof(*irmpDataToSet));
	memcpy(irmpDataVariable, irmpDataToSet, sizeof(*irmpDataToSet));
}

static unsigned char getMode()
{
	return digitalRead(PB0A) == LOW // display is dimmed
		? MODE_STANDBY
		: digitalRead(PC3A) == LOW // BT is off
			? si4735.isCurrentTuneFM() ? MODE_RADIO_FM : MODE_RADIO_AM
			: MODE_BT;
}

static void saveSettings()
{
	if (getMode() == MODE_RADIO_FM)
	{
		unsigned int lastFreqMhz;
		eeprom_read_block(&lastFreqMhz, (unsigned char *)LAST_FREQ_MHZ_ADDRESS, sizeof(lastFreqMhz));
		unsigned int freqMhz = si4735.getCurrentFrequency();
		if (freqMhz != lastFreqMhz)
		{
			eeprom_update_byte((unsigned char*)LAST_FREQ_MHZ_ADDRESS, freqMhz & 0xFF);
			eeprom_update_byte((unsigned char*)LAST_FREQ_MHZ_ADDRESS + 1, freqMhz >> 8);
		}
	}
	
	if (getMode() == MODE_RADIO_AM)
	{
		unsigned int lastFreqKhz;
		eeprom_read_block(&lastFreqKhz, (unsigned char *)LAST_FREQ_KHZ_ADDRESS, sizeof(lastFreqKhz));
		unsigned int freqKhz = si4735.getCurrentFrequency();
		if (freqKhz != lastFreqKhz)
		{
			eeprom_update_byte((unsigned char*)LAST_FREQ_KHZ_ADDRESS, freqKhz & 0xFF);
			eeprom_update_byte((unsigned char*)LAST_FREQ_KHZ_ADDRESS + 1, freqKhz >> 8);
		}
	}
	
	unsigned char lastMode;
	eeprom_read_block(&lastMode, (unsigned char *)LAST_MODE_ADDRESS, sizeof(lastMode));
	if (getMode() != lastMode)
	{
		eeprom_update_byte((unsigned char*)LAST_MODE_ADDRESS, getMode());
	}
	
	unsigned char lastVolume;
	eeprom_read_block(&lastVolume, (unsigned char *)VOLUME_ADDRESS, sizeof(lastVolume));
	if (volume != lastVolume)
	{
		eeprom_update_byte((unsigned char*)VOLUME_ADDRESS, volume);
	}
	
	unsigned char lastForceMono;
	eeprom_read_block(&lastForceMono, (unsigned char *)FORCEMONO_ADDRESS, sizeof(lastForceMono));
	if (forceMono != lastForceMono)
	{
		eeprom_update_byte((unsigned char*)FORCEMONO_ADDRESS, forceMono);
	}
	
	unsigned char lastTrebleBass;
	eeprom_read_block(&lastTrebleBass, (unsigned char *)TREBLEBASS_ADDRESS, sizeof(lastTrebleBass));
	if (trebleBass != lastTrebleBass)
	{
		eeprom_update_byte((unsigned char*)TREBLEBASS_ADDRESS, trebleBass);
	}
}

static bool compareGraMemory(char const* text)
{
	bool result = false;
	for (unsigned char i = 0; i < GRA_DISPLAY_CHARS; i++)
	{
		if (graDisplayMemory[i] != text[i])
		{
			result = true;
			break;
		}
	}
	
	return result;
}

static void graPuts(char const* text)
{
	if (compareGraMemory(text))
	{
		Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
		Wire.write(CMD_SHOW_GRA_TEXT);
		for (unsigned char i = 0; i <= 13; i++)
		{
			graDisplayMemory[i] = text[i];
			Wire.write(text[i]);
		}
		Wire.endTransmission();
		delay(1);
	}
}

static void numPuts(char const* text)
{
	Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
	Wire.write(CMD_SHOW_NUM_TEXT);
	for (unsigned char i = 0; i <= 5; i++)
	{
		Wire.write(text[i]);
	}
	Wire.endTransmission();
	delay(1);
}

static void graPutc(char chr, unsigned char index)
{
	if (graDisplayMemory[index] != chr)
	{
		graDisplayMemory[index] = chr;
		Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
		Wire.write(CMD_SHOW_GRA_CHAR);
		Wire.write(chr);
		Wire.write(index);
		Wire.endTransmission();
		delay(1);
	}
}

static void numPutc(char chr, unsigned char index)
{
	Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
	Wire.write(CMD_SHOW_NUM_CHAR);
	Wire.write(chr);
	Wire.write(index);
	Wire.endTransmission();
	delay(1);
}

static void clearGra()
{
	for (unsigned char i = 0; i < GRA_DISPLAY_CHARS; i++)
	{
		graDisplayMemory[i] = ' ';
	}
	
	Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
	Wire.write(CMD_CLEAR_GRA);
	Wire.endTransmission();
	delay(1);
}

static void clearNum()
{
	Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
	Wire.write(CMD_CLEAR_NUM);
	Wire.endTransmission();
	delay(1);
}

static void clearAll()
{
	for (unsigned char i = 0; i < GRA_DISPLAY_CHARS; i++)
	{
		graDisplayMemory[i] = ' ';
	}

	Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
	Wire.write(CMD_CLEAR_ALL);
	Wire.endTransmission();
	delay(1);
}

static void speOn(char spe)
{
	// if STEREO or TUNED is requested to be off, do not invoke I2C calls, they would be unnecessary
	if ((getMode() == MODE_RADIO_FM || getMode() == MODE_RADIO_AM)
		&& ((spe == SPE_STEREO && isStereoDisplayOn) || (spe == SPE_TUNED && isTunedDisplayOn)))
	{
		return;
	}
	
	Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
	Wire.write(CMD_SHOW_SPE);
	Wire.write(spe);
	Wire.endTransmission();
	delay(1);
}

static void speOff(char spe)
{
	// if STEREO or TUNED is requested to be off, do not invoke I2C calls, they would be unnecessary
	if ((getMode() == MODE_RADIO_FM || getMode() == MODE_RADIO_AM)
		&& ((spe == SPE_STEREO && !isStereoDisplayOn) || (spe == SPE_TUNED && !isTunedDisplayOn)))
	{
		return;
	}

	Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
	Wire.write(CMD_HIDE_SPE);
	Wire.write(spe);
	Wire.endTransmission();
	delay(1);
}

static void speToggle(char spe)
{
	Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
	Wire.write(CMD_TOGGLE_SPE);
	Wire.write(spe);
	Wire.endTransmission();
	delay(1);
}

inline static void showAM()
{
	graPuts("      AM      ");
}

inline static void showBT()
{
	graPuts("  Bluetooth   ");
}

static void volDisplayUpdate()
{
	Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
	Wire.write(CMD_SHOW_VOLUME);
	Wire.write(MAX_VOLUME - volume);
	Wire.endTransmission();
	delay(1);
}

static void volumeUp()
{
	if (volume > MIN_VOLUME)
	{
		volume--;
		//si4735.setVolume(volume); // Si4735 has 63 as maximum volume, but TDA7468 has it as minimum
		
		// volume left
		Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
		Wire.write(0b00000011);
		Wire.write(volume);
		Wire.endTransmission();
			
		// volume right
		Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
		Wire.write(0b00000100);
		Wire.write(volume);
		Wire.endTransmission();
		
		volDisplayUpdate();
		lastSettingsChange = millis();
	}
}

static void volumeDown()
{
	if (volume < MAX_VOLUME)
	{
		volume++;
		//si4735.setVolume(volume);
		
		// volume left
		Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
		Wire.write(0b00000011);
		Wire.write(volume);
		Wire.endTransmission();
		
		// volume right
		Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
		Wire.write(0b00000100);
		Wire.write(volume);
		Wire.endTransmission();
		
		volDisplayUpdate();
		lastSettingsChange = millis();
	}
}

static void trebleUp()
{
	// take lower nibble of trebleBass (set upper nibble to 0)
	unsigned char treble = trebleBass & 0x0f;
	if (treble != 8)
	{
		treble = treble < 7
			? (treble + 1)
			: treble == 7
				? 14
				: (treble - 1);
		trebleBass = (trebleBass & 0xF0) | (treble & 0x0F); // update lower nibble only, upper nibble stays as it was
		
		// treble/bass
		Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
		Wire.write(0b00000101);
		Wire.write(trebleBass);
		Wire.endTransmission();

		lastSettingsChange = millis();
	}
}

static void trebleDown()
{
	// take lower nibble of trebleBass (set upper nibble to 0)
	unsigned char treble = trebleBass & 0x0f;
	if (treble != 0)
	{
		treble = treble <= 7
			? (treble - 1)
			: treble == 14
				? 7
				: (treble + 1);
		trebleBass = (trebleBass & 0xF0) | (treble & 0x0F); // update lower nibble only, upper nibble stays as it was
		
		// treble/bass
		Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
		Wire.write(0b00000101);
		Wire.write(trebleBass);
		Wire.endTransmission();

		lastSettingsChange = millis();
	}
}

static void bassUp()
{
	// take upper nibble of trebleBass (set lower nibble to 0)
	unsigned char bass = trebleBass >> 4;
	if (bass != 8)
	{
		bass = bass < 7
			? (bass + 1)
			: bass == 7
			? 14
			: (bass - 1);
		trebleBass = (trebleBass & 0x0F) | (bass << 4); // update upper nibble only, lower nibble stays as it was
		
		// treble/bass
		Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
		Wire.write(0b00000101);
		Wire.write(trebleBass);
		Wire.endTransmission();

		lastSettingsChange = millis();
	}
}

static void bassDown()
{
	// take upper nibble of trebleBass (set lower nibble to 0)
	unsigned char bass = trebleBass >> 4;
	if (bass != 0)
	{
		bass = bass <= 7
			? (bass - 1)
			: bass == 14
				? 7
				: (bass + 1);
		trebleBass = (trebleBass & 0x0F) | (bass << 4); // update upper nibble only, lower nibble stays as it was
		
		// treble/bass
		Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
		Wire.write(0b00000101);
		Wire.write(trebleBass);
		Wire.endTransmission();

		lastSettingsChange = millis();
	}
}

static void forceDateTimeReadAndDisplay()
{
	readFromRTC = true;
	//dtSeconds = UINT16_MAX;
	dtMinutes = UINT16_MAX;
	dtHours = UINT16_MAX;
	dtDayInWeek = UINT16_MAX;
	dtDayInMonth = UINT16_MAX;
	dtMonth = UINT16_MAX;
	dtYear = UINT16_MAX;
	
	if (getMode() == MODE_STANDBY)
	{
		clearGra();
		graPutc('.', 6);
		graPutc('.', 9);
		graPutc('2', 10);
		graPutc('0', 11);
	}
}

static void showFM()
{
	graPuts(fmString);
}

static void clearRadioText()
{
	char maxIndex = radioText == NULL ? UINT8_MAX : (strlen(radioText) - 1);
	for (unsigned char i = 0; i < RADIOTEXT_CHARS; i++)
	{
		completeRadioText[i] = ' ';
		
		if (maxIndex != UINT8_MAX && i <= maxIndex)
		{
			radioText[i] = ' ';
		}
	}
	
	radioTextScrollingStartIndex = 0;
	radioTextScrollingEndIndex = 0;
	initialScrollDelayCounter = 0;
	radioTextDelayCounter = 0;
}

static void setMuteOnly(bool on)
{
	// set only mute or unmute (usable also when powering up/down, so don't indicate it or do anything other)
	
	if (on)
	{
		//si4735.setAudioMute(true); // mute
		
		// output
		Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
		Wire.write(0b00000110);
		Wire.write(OUTPUT_OFF);
		Wire.endTransmission();
		
		// unmute power amplifier
		digitalWrite(PB1A, LOW);
	}
	else
	{
		//si4735.setAudioMute(false); // unmute
		
		// output
		Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
		Wire.write(0b00000110);
		Wire.write(OUTPUT_ON);
		Wire.endTransmission();
		
		// mute power amplifier
		digitalWrite(PB1A, HIGH);
	}
}

static void toggleMute()
{
	if (isMuted) // cannot query Si4735 for mute indicator, it was always false
	{
		speOff(SPE_MUTE);
		speOn(SPE_L);
		speOn(SPE_R);
		
		setMuteOnly(false);
		
		isMuted = false;
	}
	else
	{
		speOn(SPE_MUTE);
		speOff(SPE_L);
		speOff(SPE_R);
		
		setMuteOnly(true);
		
		isMuted = true;
	}
}

static void setMono()
{
	if (getMode() == MODE_RADIO_FM)
	{
		si4735.setFmBlendStereoThreshold(127); // stereo = 49 (default value), mono = 127
		speOff(SPE_STEREO);
		forceMono = true;
		lastSettingsChange = millis();
	}
}

static void setStereo()
{
	si4735.setFmBlendStereoThreshold(49);
	speOn(SPE_STEREO);
	forceMono = false;
	lastSettingsChange = millis();
}

static void toggleForceMono()
{
	if (forceMono)
	{
		setStereo();
	}
	else
	{
		setMono();
	}
}

static void clearRdsInfo()
{
	char maxIndex = stationName == NULL ? UINT8_MAX : (strlen(stationName) - 1);
	for (unsigned char i = 0; i < STATION_NAME_CHARS; i++)
	{
		completeStationName[i] = ' ';
		
		if (maxIndex != UINT8_MAX && i <= maxIndex)
		{
			stationName[i] = ' ';
		}
	}
	
	displayRadioText = false;
	clearRadioText();
	speOff(SPE_ARROW_LEFT);
	speOff(SPE_ARROW_RIGHT);
}

static void exitSpecialMode()
{
	irLearningMode = 0;
	specialMode = 0;
	clearGra();
	forceDateTimeReadAndDisplay();
	clearRdsInfo();
	
	if (getMode() != MODE_STANDBY)
	{
		if (getMode() == MODE_RADIO_FM)
		{
			showFM();
		}
		else if (getMode() == MODE_RADIO_AM)
		{
			showAM();
		}
		else
		{
			showBT();
		}
	}
}

static void setRTCValues(unsigned char bytes[])
{
	// 7 bytes expected...
	Wire.beginTransmission(I2C_ADDR_RTC >> 1);
	Wire.write(0); // start from register 0, other registers are accessed incrementally
	
	for (unsigned char i = 0; i <= 6; i++)
	{
		Wire.write(bytes[i]);
	}
	
	Wire.endTransmission();
}

void setup() {
	wdt_reset(); // reset immediately in case of a previous system reset
	wdt_enable(WDTO_2S); // enable watchdog with 2 second timeout

	Wire.setClock(50000);
	pinMode(PB0A, OUTPUT); // VFD dimming
	pinMode(PB1A, OUTPUT); // hardware mute
	pinMode(PB3A, INPUT_PULLUP); // button
	pinMode(PB4A, INPUT); // IR sensor
	pinMode(PC3A, OUTPUT); // BT on/off
	
	// IRMP setup and TIMER1 configuration
	irmp_init();
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 0;
	OCR1B = 0;
	TIMSK1 = 0;
	TIFR1 = 0;
    OCR1A = (F_CPU / F_INTERRUPTS) - 1; // compare value: 1/15000 of CPU frequency
    TCCR1B = _BV(WGM12) | _BV(CS10); // switch CTC Mode on, set prescaler to 1
    TIMSK1 = _BV(OCIE1A); // OCIE1A: interrupt by timer compare
	
	// TIMER2 configuration
	// we need to clear everything to override arduino defaults
	TCCR2A = 0;
	TCCR2B = 0;
	TCNT2 = 0;
	OCR2A = 0;
	OCR2B = 0;
	TIMSK2 = 0;
	TIFR2 = 0;
	ASSR = 0;
	GTCCR = 0;
	TIMSK2 |= _BV(TOIE2); // enable interrupt on overflow
	TCCR2B |= TIMER2_TCCR2B; // configure prescaler to 1024 and start timer
	
	sei(); // enable global interrupts

	wdt_reset();
	
	// tuner setup
	// set some delays otherwise the tuner does not start properly sometimes
	si4735.setup(PB2A, 0);
	si4735.setMaxDelayPowerUp(700); // must be at least 500 ms
	si4735.setMaxDelaySetFrequency(100);
	
	unsigned int r;
	eeprom_read_block(&r, (unsigned char *)LAST_FREQ_MHZ_ADDRESS, sizeof(r));
	lastFreqMhz = (r >= MIN_FREQ_MHZ && r <= MAX_FREQ_MHZ) ? r : DEFAULT_FREQ_MHZ;
	
	unsigned int s;
	eeprom_read_block(&s, (unsigned char *)LAST_FREQ_KHZ_ADDRESS, sizeof(s));
	lastFreqKhz = (s >= MIN_FREQ_KHZ && s <= MAX_FREQ_KHZ) ? s : DEFAULT_FREQ_KHZ;
	
	unsigned char volume0;
	eeprom_read_block(&volume0, (unsigned char *)VOLUME_ADDRESS, sizeof(volume0));
	volume = (volume0 >= MIN_VOLUME && volume0 <= MAX_VOLUME) ? volume0 : DEFAULT_VOLUME;
	
	unsigned char forceMono0;
	eeprom_read_block(&forceMono0, (unsigned char *)FORCEMONO_ADDRESS, sizeof(forceMono0));
	forceMono = (forceMono0 == 1) ? true : false;
	
	unsigned char trebleBass0;
	eeprom_read_block(&trebleBass0, (unsigned char *)TREBLEBASS_ADDRESS, sizeof(trebleBass0));
	trebleBass = (trebleBass0 != 0xff) ? trebleBass0 : DEFAULT_TREBLEBASS;
	
	// input
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000000); 
	Wire.write(INPUT_IN1_TUNER);
	Wire.endTransmission();
		
	// input gain
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000001);
	Wire.write(0b00000000);
	Wire.endTransmission();
		
	// surround
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000010);
	Wire.write(0b01011000); // bits 3-5 must be set like this otherwise the chip is in MUTE mode!
	Wire.endTransmission();
	
	// volume left
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000011);
	Wire.write(volume);
	Wire.endTransmission();
	
	// volume right
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000100);
	Wire.write(volume);
	Wire.endTransmission();
	
	// treble/bass
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000101);
	// Wire.write(0b10000000); // max bass, min treble
	// Wire.write(0b11111111); // 0 db bass, 0 db treble
	Wire.write(trebleBass);
	Wire.endTransmission();

	// output
	setMuteOnly(true);
		
	// bass alc
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000111);
	Wire.write(0b10000000);
	Wire.endTransmission();
	
	eeprom_read_block(&irmp_program0, (unsigned char *)PROGRAM0_ADDRESS, sizeof(irmp_program0));
	eeprom_read_block(&freqProgram0am, (unsigned char *)PROGRAM0_AM_FREQ_ADDRESS, sizeof(freqProgram0am));
	eeprom_read_block(&freqProgram0fm, (unsigned char *)PROGRAM0_FM_FREQ_ADDRESS, sizeof(freqProgram0fm));
	eeprom_read_block(&irmp_program1, (unsigned char *)PROGRAM1_ADDRESS, sizeof(irmp_program1));
	eeprom_read_block(&freqProgram1am, (unsigned char *)PROGRAM1_AM_FREQ_ADDRESS, sizeof(freqProgram1am));
	eeprom_read_block(&freqProgram1fm, (unsigned char *)PROGRAM1_FM_FREQ_ADDRESS, sizeof(freqProgram1fm));
	eeprom_read_block(&irmp_program2, (unsigned char *)PROGRAM2_ADDRESS, sizeof(irmp_program2));
	eeprom_read_block(&freqProgram2am, (unsigned char *)PROGRAM2_AM_FREQ_ADDRESS, sizeof(freqProgram2am));
	eeprom_read_block(&freqProgram2fm, (unsigned char *)PROGRAM2_FM_FREQ_ADDRESS, sizeof(freqProgram2fm));
	eeprom_read_block(&irmp_program3, (unsigned char *)PROGRAM3_ADDRESS, sizeof(irmp_program3));
	eeprom_read_block(&freqProgram3am, (unsigned char *)PROGRAM3_AM_FREQ_ADDRESS, sizeof(freqProgram3am));
	eeprom_read_block(&freqProgram3fm, (unsigned char *)PROGRAM3_FM_FREQ_ADDRESS, sizeof(freqProgram3fm));
	eeprom_read_block(&irmp_program4, (unsigned char *)PROGRAM4_ADDRESS, sizeof(irmp_program4));
	eeprom_read_block(&freqProgram4am, (unsigned char *)PROGRAM4_AM_FREQ_ADDRESS, sizeof(freqProgram4am));
	eeprom_read_block(&freqProgram4fm, (unsigned char *)PROGRAM4_FM_FREQ_ADDRESS, sizeof(freqProgram4fm));
	eeprom_read_block(&irmp_program5, (unsigned char *)PROGRAM5_ADDRESS, sizeof(irmp_program5));
	eeprom_read_block(&freqProgram5am, (unsigned char *)PROGRAM5_AM_FREQ_ADDRESS, sizeof(freqProgram5am));
	eeprom_read_block(&freqProgram5fm, (unsigned char *)PROGRAM5_FM_FREQ_ADDRESS, sizeof(freqProgram5fm));
	eeprom_read_block(&irmp_program6, (unsigned char *)PROGRAM6_ADDRESS, sizeof(irmp_program6));
	eeprom_read_block(&freqProgram6am, (unsigned char *)PROGRAM6_AM_FREQ_ADDRESS, sizeof(freqProgram6am));
	eeprom_read_block(&freqProgram6fm, (unsigned char *)PROGRAM6_FM_FREQ_ADDRESS, sizeof(freqProgram6fm));
	eeprom_read_block(&irmp_program7, (unsigned char *)PROGRAM7_ADDRESS, sizeof(irmp_program7));
	eeprom_read_block(&freqProgram7am, (unsigned char *)PROGRAM7_AM_FREQ_ADDRESS, sizeof(freqProgram7am));
	eeprom_read_block(&freqProgram7fm, (unsigned char *)PROGRAM7_FM_FREQ_ADDRESS, sizeof(freqProgram7fm));
	eeprom_read_block(&irmp_program8, (unsigned char *)PROGRAM8_ADDRESS, sizeof(irmp_program8));
	eeprom_read_block(&freqProgram8am, (unsigned char *)PROGRAM8_AM_FREQ_ADDRESS, sizeof(freqProgram8am));
	eeprom_read_block(&freqProgram8fm, (unsigned char *)PROGRAM8_FM_FREQ_ADDRESS, sizeof(freqProgram8fm));
	eeprom_read_block(&irmp_program9, (unsigned char *)PROGRAM9_ADDRESS, sizeof(irmp_program9));
	eeprom_read_block(&freqProgram9am, (unsigned char *)PROGRAM9_AM_FREQ_ADDRESS, sizeof(freqProgram9am));
	eeprom_read_block(&freqProgram9fm, (unsigned char *)PROGRAM9_FM_FREQ_ADDRESS, sizeof(freqProgram9fm));
	
	eeprom_read_block(&irmp_program10, (unsigned char *)PROGRAM10_ADDRESS, sizeof(irmp_program10));
	eeprom_read_block(&freqProgram10am, (unsigned char *)PROGRAM10_AM_FREQ_ADDRESS, sizeof(freqProgram10am));
	eeprom_read_block(&freqProgram10fm, (unsigned char *)PROGRAM10_FM_FREQ_ADDRESS, sizeof(freqProgram10fm));
	eeprom_read_block(&irmp_program11, (unsigned char *)PROGRAM11_ADDRESS, sizeof(irmp_program11));
	eeprom_read_block(&freqProgram11am, (unsigned char *)PROGRAM11_AM_FREQ_ADDRESS, sizeof(freqProgram11am));
	eeprom_read_block(&freqProgram11fm, (unsigned char *)PROGRAM11_FM_FREQ_ADDRESS, sizeof(freqProgram11fm));
	eeprom_read_block(&irmp_program12, (unsigned char *)PROGRAM12_ADDRESS, sizeof(irmp_program12));
	eeprom_read_block(&freqProgram12am, (unsigned char *)PROGRAM12_AM_FREQ_ADDRESS, sizeof(freqProgram12am));
	eeprom_read_block(&freqProgram12fm, (unsigned char *)PROGRAM12_FM_FREQ_ADDRESS, sizeof(freqProgram12fm));
	eeprom_read_block(&irmp_program13, (unsigned char *)PROGRAM13_ADDRESS, sizeof(irmp_program13));
	eeprom_read_block(&freqProgram13am, (unsigned char *)PROGRAM13_AM_FREQ_ADDRESS, sizeof(freqProgram13am));
	eeprom_read_block(&freqProgram13fm, (unsigned char *)PROGRAM13_FM_FREQ_ADDRESS, sizeof(freqProgram13fm));
	eeprom_read_block(&irmp_program14, (unsigned char *)PROGRAM14_ADDRESS, sizeof(irmp_program14));
	eeprom_read_block(&freqProgram14am, (unsigned char *)PROGRAM14_AM_FREQ_ADDRESS, sizeof(freqProgram14am));
	eeprom_read_block(&freqProgram14fm, (unsigned char *)PROGRAM14_FM_FREQ_ADDRESS, sizeof(freqProgram14fm));
	eeprom_read_block(&irmp_program15, (unsigned char *)PROGRAM15_ADDRESS, sizeof(irmp_program15));
	eeprom_read_block(&freqProgram15am, (unsigned char *)PROGRAM15_AM_FREQ_ADDRESS, sizeof(freqProgram15am));
	eeprom_read_block(&freqProgram15fm, (unsigned char *)PROGRAM15_FM_FREQ_ADDRESS, sizeof(freqProgram15fm));
	eeprom_read_block(&irmp_program16, (unsigned char *)PROGRAM16_ADDRESS, sizeof(irmp_program16));
	eeprom_read_block(&freqProgram16am, (unsigned char *)PROGRAM16_AM_FREQ_ADDRESS, sizeof(freqProgram16am));
	eeprom_read_block(&freqProgram16fm, (unsigned char *)PROGRAM16_FM_FREQ_ADDRESS, sizeof(freqProgram16fm));
	eeprom_read_block(&irmp_program17, (unsigned char *)PROGRAM17_ADDRESS, sizeof(irmp_program17));
	eeprom_read_block(&freqProgram17am, (unsigned char *)PROGRAM17_AM_FREQ_ADDRESS, sizeof(freqProgram17am));
	eeprom_read_block(&freqProgram17fm, (unsigned char *)PROGRAM17_FM_FREQ_ADDRESS, sizeof(freqProgram17fm));
	eeprom_read_block(&irmp_program18, (unsigned char *)PROGRAM18_ADDRESS, sizeof(irmp_program18));
	eeprom_read_block(&freqProgram18am, (unsigned char *)PROGRAM18_AM_FREQ_ADDRESS, sizeof(freqProgram18am));
	eeprom_read_block(&freqProgram18fm, (unsigned char *)PROGRAM18_FM_FREQ_ADDRESS, sizeof(freqProgram18fm));
	eeprom_read_block(&irmp_program19, (unsigned char *)PROGRAM19_ADDRESS, sizeof(irmp_program19));
	eeprom_read_block(&freqProgram19am, (unsigned char *)PROGRAM19_AM_FREQ_ADDRESS, sizeof(freqProgram19am));
	eeprom_read_block(&freqProgram19fm, (unsigned char *)PROGRAM19_FM_FREQ_ADDRESS, sizeof(freqProgram19fm));
	
	eeprom_read_block(&irmp_r, (unsigned char *)R_ADDRESS, sizeof(irmp_r));
	eeprom_read_block(&irmp_f, (unsigned char *)F_ADDRESS, sizeof(irmp_f));
	eeprom_read_block(&irmp_mute, (unsigned char *)MUTE_ADDRESS, sizeof(irmp_mute));
	eeprom_read_block(&irmp_standby, (unsigned char *)STANDBY_ADDRESS, sizeof(irmp_standby));
	eeprom_read_block(&irmp_band, (unsigned char *)BAND_ADDRESS, sizeof(irmp_band));
	eeprom_read_block(&irmp_program, (unsigned char *)PROGRAM_ADDRESS, sizeof(irmp_program));
	eeprom_read_block(&irmp_monostereo, (unsigned char *)MONOSTEREO_ADDRESS, sizeof(irmp_monostereo));
	eeprom_read_block(&irmp_volumedown, (unsigned char *)VOLUMEDOWN_ADDRESS, sizeof(irmp_volumedown));
	eeprom_read_block(&irmp_volumeup, (unsigned int *)VOLUMEUP_ADDRESS, sizeof(irmp_volumeup));
	eeprom_read_block(&irmp_bassdown, (unsigned char *)BASSDOWN_ADDRESS, sizeof(irmp_bassdown));
	eeprom_read_block(&irmp_bassup, (unsigned int *)BASSUP_ADDRESS, sizeof(irmp_bassup));
	eeprom_read_block(&irmp_trebledown, (unsigned char *)TREBLEDOWN_ADDRESS, sizeof(irmp_trebledown));
	eeprom_read_block(&irmp_trebleup, (unsigned int *)TREBLEUP_ADDRESS, sizeof(irmp_trebleup));
	eeprom_read_block(&irmp_display, (unsigned int *)DISPLAY_ADDRESS, sizeof(irmp_display));
	
	wdt_reset();
	
	// set Saturday 1.1.2000 to RTC
	unsigned char bytes[] = {
		0b00000000, // seconds
		0b00000000, // minutes
		0b00000000, // hours
		0b00000110, // Saturday (6)
		0b00000001, // day of month (1)
		0b00000001, // month (1)
		0b00000000 // 2000
	};
	
	setRTCValues(bytes);
}

static void frequencyDisplayUpdate()
{
	if (getMode() == MODE_RADIO_FM || getMode() == MODE_RADIO_AM)
	{
		int freq = si4735.getCurrentFrequency();
		char str[2];
		char numString[NUM_DISPLAY_CHARS];
		
		
		if (getMode() == MODE_RADIO_FM)
		{
			numString[0] = freq >= 10000 ? itoa(freq / 10000, str, 10)[0] : ' ';
			numString[1] = itoa((freq % 10000) / 1000, str, 10)[0];
			numString[2] = itoa((freq % 1000) / 100, str, 10)[0];
			numString[3] = itoa((freq % 100) / 10, str, 10)[0];
			
			numString[4] = 'F';
			speOn(SPE_COLON_LOWER);
		}
		else
		{
			numString[0] = freq >= 1000 ? itoa(freq / 1000, str, 10)[0] : ' ';
			numString[1] = itoa((freq % 1000) / 100, str, 10)[0];
			numString[2] = itoa((freq % 100) / 10, str, 10)[0];
			numString[3] = itoa(freq % 10, str, 10)[0];

			numString[4] = 'A';
			speOff(SPE_COLON_LOWER);
		}
		
		numString[5] = 'M';
		
		numPuts((const char *)&numString);
	}
}

static void frequencyUp()
{
	si4735.frequencyUp();
	frequencyDisplayUpdate();
	lastSettingsChange = millis();
}

static void frequencyDown()
{
	si4735.frequencyDown();
	frequencyDisplayUpdate();
	lastSettingsChange = millis();
}

static void setFM()
{
	showFM();
	clearRdsInfo();
	rdsSyncedFreshly = false;
	
	digitalWrite(PC3A, LOW); // BT off
	
	si4735.setFM(MIN_FREQ_MHZ, MAX_FREQ_MHZ, lastFreqMhz, 10); // 10 = 100 kHz spacing
	si4735.setRdsConfig(1, 2, 2, 2, 2); // do not allow any uncorrected errors in RDS, first parameter is "enable RDS"
	si4735.setTuneFrequencyAntennaCapacitor(0);
	si4735.setSeekFmLimits(MIN_FREQ_MHZ, MAX_FREQ_MHZ);
	si4735.setSeekFmSpacing(10); // 100 kHz
	si4735.setVolume(SI4735_VOLUME);

	// input
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000000);
	Wire.write(INPUT_IN1_TUNER);
	Wire.endTransmission();
	
	// input gain
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000001);
	Wire.write(0b00000000);
	Wire.endTransmission();

	if (forceMono)
	{
		setMono();
	}
	
	frequencyDisplayUpdate();
}

static void setAM()
{
	digitalWrite(PC3A, LOW); // BT off
	clearRdsInfo();
	showAM();
	speOff(SPE_STEREO);
	speOff(SPE_TUNED);

	si4735.setAM(MIN_FREQ_KHZ, MAX_FREQ_KHZ, lastFreqKhz, 9); // 9 = 9 kHz spacing
	si4735.setSeekAmLimits(MIN_FREQ_KHZ, MAX_FREQ_KHZ);
	si4735.setSeekAmSpacing(9); // 9 kHz
	si4735.setVolume(SI4735_VOLUME);
	
	// input
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000000);
	Wire.write(INPUT_IN1_TUNER);
	Wire.endTransmission();
	
	// input gain
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000001);
	Wire.write(0b00000000);
	Wire.endTransmission();
	
	frequencyDisplayUpdate();
}

static void setBT()
{
	clearRdsInfo();
	showBT();
	speOff(SPE_STEREO);
	speOff(SPE_TUNED);
	si4735.powerDown();
	digitalWrite(PC3A, HIGH); // BT on
	
	// input
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000000);
	Wire.write(INPUT_IN2_BT);
	Wire.endTransmission();
	
	// input gain
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000001);
	Wire.write(0b00000111);
	Wire.endTransmission();
	
	clearNum(); // clear frequency display
}

static void on()
{
	unsigned char lastOnMode0;
	eeprom_read_block(&lastOnMode0, (unsigned char *)LAST_MODE_ADDRESS, sizeof(lastOnMode0));
	unsigned char lastOnMode = (lastOnMode0 == MODE_RADIO_AM || lastOnMode0 == MODE_RADIO_FM || lastOnMode0 == MODE_BT) ? lastOnMode0 : MODE_RADIO_FM;

	// light up the display
	digitalWrite(PB0A, HIGH);
	
	// clear displays
	clearAll();
	
	clearRdsInfo();
	
	if (lastOnMode == MODE_RADIO_FM || lastOnMode == MODE_RADIO_AM)
	{		
		if (lastOnMode == MODE_RADIO_FM)
		{
			setFM();
		}
		else if (lastOnMode == MODE_RADIO_AM)
		{
			setAM();
		}
		
		frequencyDisplayUpdate();
	}
	else
	{
		setBT();
	}
	
	// input
	Wire.beginTransmission(I2C_ADDR_AUDIOPROC >> 1);
	Wire.write(0b00000000);
	Wire.write(getMode() == MODE_RADIO_FM || getMode() == MODE_RADIO_AM ? INPUT_IN1_TUNER : INPUT_IN2_BT);
	Wire.endTransmission();
	
	// output
	// turn on after radio power up to avoid pop noise
	setMuteOnly(false);
	
	// show volume
	volDisplayUpdate();
	speOn(SPE_VOLDB);
	speOn(SPE_L);
	speOn(SPE_R);
	
	// show underlines
	Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
	Wire.write(CMD_SHOW_UNDERLINES);
	Wire.endTransmission();
	delay(1);
}

static void off()
{
	isMuted = false;
	
	// must be called before anything other to save correct last mode
	saveSettings();
	
	speOff(SPE_STEREO);
	speOff(SPE_L);
	speOff(SPE_R);
	
	// output
	// turn off before radio power down to avoid pop noise
	setMuteOnly(true);
	
	if (getMode() == MODE_RADIO_FM || getMode() == MODE_RADIO_AM)
	{
		// stop radio
		si4735.powerDown();
		clearRdsInfo();
	}
	else
	{
		// BT off
		digitalWrite(PC3A, LOW); // BT off
	}
	
	// dim the display
	digitalWrite(PB0A, LOW);
	
	// clear displays
	clearAll();
	isStereoDisplayOn = false;
	isTunedDisplayOn = false;
	
	// hide underlines
	Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
	Wire.write(CMD_HIDE_UNDERLINES);
	Wire.endTransmission();
	delay(1);
			
	// show colon for time display and show time, force time read and display
	speOn(SPE_COLON_UPPER);
	speOn(SPE_COLON_LOWER);
	forceDateTimeReadAndDisplay();
}

static void setFrequency(unsigned int freq)
{
	if ((getMode() == MODE_RADIO_FM || getMode() == MODE_RADIO_AM) && si4735.getCurrentFrequency() != freq)
	{
		if (getMode() == MODE_RADIO_FM)
		{
			si4735.setFrequency(freq);
			lastFreqMhz = freq;
		}
		else if (getMode() == MODE_RADIO_AM)
		{
			si4735.setFrequency(freq);
			lastFreqKhz = freq;
		}
	
		frequencyDisplayUpdate();
		lastSettingsChange = millis();
	}
}

static void switchMode()
{
	// to avoid pop noises when switching inputs, mute and then unmute
	setMuteOnly(true);
	
	if (getMode() == MODE_RADIO_FM)
	{
		// switching from FM to AM, shelve last FM frequency
		lastFreqMhz = si4735.getCurrentFrequency();
		setAM(); // this call also sets frequency to lastFreqKhz
	}
	else if (getMode() == MODE_RADIO_AM)
	{
		// switching from AM to BT, shelve last AM frequency
		lastFreqKhz = si4735.getCurrentFrequency();
		setBT();
	}
	else if (getMode() == MODE_BT)
	{
		// switching from BT to FM
		setFM(); // this call also sets frequency to lastFreqMhz
	}
	
	setMuteOnly(false);
	
	lastSettingsChange = millis();
}

//unsigned int err = 100;

static void switchToSavedFrequency(unsigned int fmFrequency, unsigned int amFrequency)
{
	if (getMode() == MODE_STANDBY)
	{
		on();
					
		if (getMode() != MODE_RADIO_FM)
		{
			setFM();
		}
	}
				
	// switch to saved frequency
	if (getMode() == MODE_RADIO_FM)
	{
		setFrequency(fmFrequency);
	}
	else if (getMode() == MODE_RADIO_AM)
	{
		setFrequency(amFrequency);
	}	
}

void loop() {
	wdt_reset();
	
	if (digitalRead(PB3A) == 0)
	{
		wdt_reset();
		_delay_ms(BUTTON_DEBOUNCE_DELAY_MS);
		wdt_reset();
		if (digitalRead(PB3A) == 0)
		{
			if (specialMode == 0)
			{
				specialMode = SPECIAL_MODE_IR_LEARNING;
				irLearningMode = IR_LEARNING_PROGRAM0;
				graPuts("Program 1 = ? ");
				speOff(SPE_ARROW_LEFT); // might be displayed as part of RDS so hide them
				speOff(SPE_ARROW_RIGHT);
			}
			else if (specialMode == SPECIAL_MODE_IR_LEARNING)
			{
				exitSpecialMode();
				speOff(SPE_ARROW_DOWN); // might be displayed as part of IR setup so hide them
				speOff(SPE_ARROW_UP);
			}
			
			/*set_sleep_mode(SLEEP_MODE_ADC);
			sleep_enable();*/
		}
		
		_delay_ms(BUTTON_PRESS_DELAY_MS);
		wdt_reset();
	}
	
	if (!setupFinished)
	{
		if (setupRTCCounter >= 65000)
		{
			clearAll();
			setupFinished = true;
			forceDateTimeReadAndDisplay();
		}
		else
		{
			setupRTCCounter++;
		}
	}
	
	if (getMode() == MODE_STANDBY && readFromRTC && setupFinished)
	{
		readFromRTC = false;
		
		speToggle(SPE_COLON_UPPER);
		speToggle(SPE_COLON_LOWER);
			
		// read from RTC if indicated by interrupt from TIMER2
		// start from register 0
		Wire.beginTransmission(I2C_ADDR_RTC >> 1);
		Wire.write(0);
		Wire.endTransmission();
	
		unsigned char registers[7];
		Wire.requestFrom(I2C_ADDR_RTC >> 1, 7, false);
		Wire.readBytes((unsigned char *)&registers, 7);
	
		//char seconds = (registers[0] & 0b00001111) + ((registers[0] & 0b01110000) >> 4) * 10;
		char minutes = (registers[1] & 0b00001111) + ((registers[1] & 0b01110000) >> 4) * 10;
		char hours = (registers[2] & 0b00001111) + ((registers[2] & 0b00110000) >> 4) * 10;
		char dayInWeek = registers[3] & 0b00000111;
		char dayInMonth = (registers[4] & 0b00001111) + ((registers[4] & 0b00110000) >> 4) * 10;
		char month = (registers[5] & 0b00001111) + ((registers[5] & 0b00010000) >> 4) * 10;
		char year = (registers[6] & 0b00001111) + ((registers[6] & 0b11110000) >> 4) * 10;
		char str[2]; // to include \0
	
		/*if (seconds != dtSeconds)
		{
			dtSeconds = seconds;
		}*/
		
		if (minutes != dtMinutes)
		{
			dtMinutes = minutes;
			numPutc(itoa(minutes / 10, str, 10)[0], 3);
			numPutc(itoa(minutes % 10, str, 10)[0], 4);
		}
	
		if (hours != dtHours)
		{
			dtHours = hours;
			numPutc(itoa(hours / 10, str, 10)[0], 1);
			numPutc(itoa(hours % 10, str, 10)[0], 2);
		}
		
		if (dayInWeek != dtDayInWeek)
		{
			dtDayInWeek = dayInWeek;
			
			if (specialMode == 0)
			{
				graPutc(dayNames[dayInWeek - 1][0], 0);
				graPutc(dayNames[dayInWeek - 1][1], 1);
				graPutc(dayNames[dayInWeek - 1][2], 2);
			}
		}
		
		if (dayInMonth != dtDayInMonth)
		{
			dtDayInMonth = dayInMonth;
			
			if (specialMode == 0)
			{
				graPutc(itoa(dayInMonth / 10, str, 10)[0], 4);
				graPutc(itoa(dayInMonth % 10, str, 10)[0], 5);
			}
		}
		
		if (month != dtMonth)
		{
			dtMonth = month;
			if (specialMode == 0)
			{
				graPutc(itoa(month / 10, str, 10)[0], 7);
				graPutc(itoa(month % 10, str, 10)[0], 8);
			}
		}
		
		if (year != dtYear)
		{
			dtYear = year;
			if (specialMode == 0)
			{
				graPutc(itoa(year / 10, str, 10)[0], 12);
				graPutc(itoa(year % 10, str, 10)[0], 13);
			}
		}
	}
	
	if (getMode() == MODE_STANDBY && oldDisplayedSpeIndex != displayedSpeForStandbyIndex && setupFinished)
	{
		speOff(speForStandby[displayedSpeForStandbyIndex == 0 ? (sizeof(speForStandby) - 1) : (displayedSpeForStandbyIndex - 1)]);
		speOn(speForStandby[displayedSpeForStandbyIndex]);
		oldDisplayedSpeIndex = displayedSpeForStandbyIndex;
	}
	
	if (getMode() != MODE_STANDBY)
	{
		if (lastSettingsChange != 0 && (millis() - lastSettingsChange > 5000))
		{
			lastSettingsChange = 0;
			saveSettings();
		}

		if (getMode() == MODE_RADIO_FM)
		{
			// do not ask for signal quality too often, it causes unremovable noise in audio coming from the AVR
			if (stereoAndTunedUpdateCounter == 4000)
			{
				stereoAndTunedUpdateCounter = 0;
				
				si4735.getCurrentReceivedSignalQuality(); // must be called before getCurrentPilot()
		
				if (!forceMono && si4735.getCurrentPilot())
				{
					speOn(SPE_STEREO);
					isStereoDisplayOn = true;
				}
				else
				{
					speOff(SPE_STEREO);
					isStereoDisplayOn = false;
				}
		
				//unsigned int snr = si4735.getCurrentSNR();
				//char str[2];
				//str[0] = itoa(snr / 10, str, 10)[0];
				//str[1] = itoa(snr % 10, str, 10)[0];
				//numPutc(str[0], 4);
				//numPutc(str[1], 5);
		
				if (si4735.getCurrentSNR() > 27)
				{
					speOn(SPE_TUNED);
					isTunedDisplayOn = true;
				}
				else
				{
					speOff(SPE_TUNED);
					isTunedDisplayOn = false;
				}
			}
			else
			{
				stereoAndTunedUpdateCounter++;
			}
		}
	}
	
	if (getMode() == MODE_RADIO_FM && readRDS && specialMode == 0)
	{
		readRDS = false;
		si4735.getRdsStatus(0, 0, 0);
		
		if (si4735.getRdsReceived())
		{
			if (si4735.getRdsSync() && si4735.getRdsSyncFound())
			{
				if (!rdsSyncedFreshly)
				{
					rdsSyncedFreshly = true;
					clearRdsInfo();
					clearGra();
				}
				
				radioText = si4735.getRdsText2A();
				stationName = si4735.getRdsText0A();
			
				if (radioText != NULL)
				{
					radioTextLostCounter = 0;
					
					// because RDS data arrive 4 characters at a time, the rest is spaces
					// merge non-space characters into completeRadioText only
					// it has been initialized to all-spaces above so any new spaces will be preserved
					bool nonSpaceCharChanged = false;
					for (unsigned char i = 0; i < RADIOTEXT_CHARS; i++)
					{
						if (radioText[i] != ' ' && completeRadioText[i] != ' ' && radioText[i] != completeRadioText[i] && radioText[i] != 0)
						{
							nonSpaceCharChanged = true;
							break;
						}
					}
					
					if (displayRadioText)
					{
						if (nonSpaceCharChanged)
						{
							clearRadioText();
							clearGra();
						}
						
						for (unsigned char i = 0; i < RADIOTEXT_CHARS; i++)
						{
							if (radioText[i] > 33 && radioText[i] < 127 && radioText[i] != completeRadioText[i] && radioText[i] != 0) // do not merge nul character
							{
								completeRadioText[i] = radioText[i];
							}
						}
						
						// determine radioTextScrollingEndIndex - last non-space character
						for (int i = RADIOTEXT_CHARS - 1; i >= 0; i--)
						{
							if (completeRadioText[i] != ' ')
							{
								radioTextScrollingEndIndex = i;
								break;
							}
						}
					
						if (radioTextScrollingEndIndex < GRA_DISPLAY_CHARS)
						{
							// no scrolling, radiotext is short
							radioTextScrollingStartIndex = 0;
						}
					
						char graString[GRA_DISPLAY_CHARS]; // not nul-terminated
						for (unsigned char i = 0; i < GRA_DISPLAY_CHARS; i++)
						{
							graString[i] = completeRadioText[i + radioTextScrollingStartIndex];
						}
					
						graPuts((const char *)&graString);
						
						if (radioTextScrollingStartIndex > 0)
						{
							speOn(SPE_ARROW_LEFT);
						}
						else
						{
							speOff(SPE_ARROW_LEFT);
						}
						
						delay(1);
						
						if ((radioTextScrollingStartIndex + GRA_DISPLAY_CHARS < radioTextScrollingEndIndex + 1))
						{
							speOn(SPE_ARROW_RIGHT);
						}
						else
						{
							speOff(SPE_ARROW_RIGHT);
						}
						
						delay(1);
					}
				}
				else
				{
					// radioText is not transmitted anymore, but station name may still be, let it display station name
					if (radioTextLostCounter <= 8)
					{
						radioTextLostCounter++;
					}
					else if (radioTextLostCounter == 8)
					{
						displayRadioText = false;
						clearRdsInfo();
						clearGra();
					}
				}

				if (stationName != NULL && si4735.getRdsNewBlockA())
				{
					// station name arrives 2 characters at a time, the rest are spaces
					// merge non-space characters into completeStationName only
					// it has been initialized to all-spaces above so any new spaces will be preserved
					
					// stationName is terminated with nul character, leave it out
					// oldStationName is not terminated
					for (unsigned char i = 0; i < STATION_NAME_CHARS; i++)
					{
						if (stationName[i] != ' ')
						{
							completeStationName[i] = stationName[i];
						}
					}
					
					if (!displayRadioText)
					{
						// find real length of station name - it may be padded with spaces and we want to center it
						char stationNameMaxIndex = 0;
						for (unsigned char i = STATION_NAME_CHARS - 1; i >= 0; i--)
						{
							if (completeStationName[i] != ' ')
							{
								stationNameMaxIndex = i;
								break;
							}
						}
					
						char halfStationNameLength = (stationNameMaxIndex + 1) / 2;
						char oddPlacement = stationNameMaxIndex % 2 == 0 ? 1 : 0; // 5-character station name has max index = 4, align it one position left, it looks better
					
						for (unsigned char i = 0; i < STATION_NAME_CHARS; i++)
						{
							graPutc(completeStationName[i], 7 - halfStationNameLength - oddPlacement + i); // center
						}
					}
				}

				// set time from RDS if it has not been set yet
				if (dtYear == 0 && si4735.getRdsDateTime(&dtYear, &dtMonth, &dtDayInMonth, &dtHours, &dtMinutes))
				{					
					dtDayInWeek = weekday(dtYear, dtMonth, dtDayInMonth);
					dtYear = dtYear - 2000; // store only two-place year, even if the variable is uint16
					
					// cannot use array initializer, it throws a warning about narrowing conversion
					unsigned char bytes[7];
					bytes[0] = 0b00000000; // seconds
					bytes[1] = (dtMinutes % 10) | ((dtMinutes / 10) << 4);
					bytes[2] = (dtHours % 10) | ((dtHours / 10) << 4);
					bytes[3] = dtDayInWeek;
					bytes[4] = (dtDayInMonth % 10) | ((dtDayInMonth / 10) << 4);
					bytes[5] = (dtMonth % 10) | ((dtMonth / 10) << 4);
					bytes[6] = (dtYear % 10) | ((dtYear / 10) << 4);
	
					setRTCValues(bytes);
				}
			}
			else
			{
				// RDS sync lost
				clearRdsInfo();
				rdsSyncedFreshly = false;
				
				if (compareGraMemory(fmString))
				{
					clearGra();
					showFM();
				}
			}
		}
		else
		{
			// no RDS
			clearRdsInfo();
			rdsSyncedFreshly = false;
			
			if (compareGraMemory(fmString))
			{
				clearGra();
				showFM();
			}
		}
	}
	
	if (irmp_get_data(&irmp_data))
	{
		char wasRepetition = irmp_data.flags & IRMP_FLAG_REPETITION;
		
		if (specialMode == SPECIAL_MODE_IR_LEARNING)
		{
			if (!wasRepetition)
			{
				switch (irLearningMode)
				{
					case IR_LEARNING_PROGRAM0:
					learnIrCode(PROGRAM0_ADDRESS, &irmp_data, &irmp_program0);
					graPuts("Program 2 = ? ");
					irLearningMode = IR_LEARNING_PROGRAM1;
					break;
					case IR_LEARNING_PROGRAM1:
					learnIrCode(PROGRAM1_ADDRESS, &irmp_data, &irmp_program1);
					graPuts("Program 3 = ? ");
					irLearningMode = IR_LEARNING_PROGRAM2;
					break;
					case IR_LEARNING_PROGRAM2:
					learnIrCode(PROGRAM2_ADDRESS, &irmp_data, &irmp_program2);
					graPuts("Program 4 = ? ");
					irLearningMode = IR_LEARNING_PROGRAM3;
					break;
					case IR_LEARNING_PROGRAM3:
					learnIrCode(PROGRAM3_ADDRESS, &irmp_data, &irmp_program3);
					graPuts("Program 5 = ? ");
					irLearningMode = IR_LEARNING_PROGRAM4;
					break;
					case IR_LEARNING_PROGRAM4:
					learnIrCode(PROGRAM4_ADDRESS, &irmp_data, &irmp_program4);
					graPuts("Program 6 = ? ");
					irLearningMode = IR_LEARNING_PROGRAM5;
					break;
					case IR_LEARNING_PROGRAM5:
					learnIrCode(PROGRAM5_ADDRESS, &irmp_data, &irmp_program5);
					graPuts("Program 7 = ? ");
					irLearningMode = IR_LEARNING_PROGRAM6;
					break;
					case IR_LEARNING_PROGRAM6:
					learnIrCode(PROGRAM6_ADDRESS, &irmp_data, &irmp_program6);
					graPuts("Program 8 = ? ");
					irLearningMode = IR_LEARNING_PROGRAM7;
					break;
					case IR_LEARNING_PROGRAM7:
					learnIrCode(PROGRAM7_ADDRESS, &irmp_data, &irmp_program7);
					graPuts("Program 9 = ? ");
					irLearningMode = IR_LEARNING_PROGRAM8;
					break;
					case IR_LEARNING_PROGRAM8:
					learnIrCode(PROGRAM8_ADDRESS, &irmp_data, &irmp_program8);
					graPuts("Program 10 = ?");
					irLearningMode = IR_LEARNING_PROGRAM9;
					break;
					case IR_LEARNING_PROGRAM9:
					learnIrCode(PROGRAM9_ADDRESS, &irmp_data, &irmp_program9);
					graPuts("Program 11 = ? ");
					irLearningMode = IR_LEARNING_PROGRAM10;
					break;
					case IR_LEARNING_PROGRAM10:
					learnIrCode(PROGRAM10_ADDRESS, &irmp_data, &irmp_program10);
					graPuts("Program 12 = ?");
					irLearningMode = IR_LEARNING_PROGRAM11;
					break;
					case IR_LEARNING_PROGRAM11:
					learnIrCode(PROGRAM11_ADDRESS, &irmp_data, &irmp_program11);
					graPuts("Program 13 = ?");
					irLearningMode = IR_LEARNING_PROGRAM12;
					break;
					case IR_LEARNING_PROGRAM12:
					learnIrCode(PROGRAM12_ADDRESS, &irmp_data, &irmp_program12);
					graPuts("Program 14 = ?");
					irLearningMode = IR_LEARNING_PROGRAM13;
					break;
					case IR_LEARNING_PROGRAM13:
					learnIrCode(PROGRAM13_ADDRESS, &irmp_data, &irmp_program13);
					graPuts("Program 15 = ?");
					irLearningMode = IR_LEARNING_PROGRAM14;
					break;
					case IR_LEARNING_PROGRAM14:
					learnIrCode(PROGRAM14_ADDRESS, &irmp_data, &irmp_program14);
					graPuts("Program 16 = ?");
					irLearningMode = IR_LEARNING_PROGRAM15;
					break;
					case IR_LEARNING_PROGRAM15:
					learnIrCode(PROGRAM15_ADDRESS, &irmp_data, &irmp_program15);
					graPuts("Program 17 = ?");
					irLearningMode = IR_LEARNING_PROGRAM16;
					break;
					case IR_LEARNING_PROGRAM16:
					learnIrCode(PROGRAM16_ADDRESS, &irmp_data, &irmp_program16);
					graPuts("Program 18 = ?");
					irLearningMode = IR_LEARNING_PROGRAM17;
					break;
					case IR_LEARNING_PROGRAM17:
					learnIrCode(PROGRAM17_ADDRESS, &irmp_data, &irmp_program17);
					graPuts("Program 19 = ?");
					irLearningMode = IR_LEARNING_PROGRAM18;
					break;
					case IR_LEARNING_PROGRAM18:
					learnIrCode(PROGRAM18_ADDRESS, &irmp_data, &irmp_program18);
					graPuts("Program 20 = ?");
					irLearningMode = IR_LEARNING_PROGRAM19;
					break;
					case IR_LEARNING_PROGRAM19:
					learnIrCode(PROGRAM19_ADDRESS, &irmp_data, &irmp_program19);
					speOn(SPE_ARROW_DOWN);
					graPuts("Tuning = ?    ");
					irLearningMode = IR_LEARNING_R;
					break;
					case IR_LEARNING_R:
					learnIrCode(R_ADDRESS, &irmp_data, &irmp_r);
					speOff(SPE_ARROW_DOWN);
					speOn(SPE_ARROW_UP);
					graPuts("Tuning = ?    ");
					irLearningMode = IR_LEARNING_F;
					break;
					case IR_LEARNING_F:
					learnIrCode(F_ADDRESS, &irmp_data, &irmp_f);
					speOff(SPE_ARROW_UP);
					graPuts("Mute = ?      ");
					irLearningMode = IR_LEARNING_MUTE;
					break;
					case IR_LEARNING_MUTE:
					learnIrCode(MUTE_ADDRESS, &irmp_data, &irmp_mute);
					graPuts("Standby = ?   ");
					irLearningMode = IR_LEARNING_STANDBY;
					break;
					case IR_LEARNING_STANDBY:
					learnIrCode(STANDBY_ADDRESS, &irmp_data, &irmp_standby);
					graPuts("Band/input = ?");
					irLearningMode = IR_LEARNING_BAND;
					break;
					case IR_LEARNING_BAND:
					learnIrCode(BAND_ADDRESS, &irmp_data, &irmp_band);
					speOn(SPE_ARROW_DOWN);
					graPuts("Volume = ?    ");
					irLearningMode = IR_LEARNING_VOLUMEDOWN;
					break;
					case IR_LEARNING_VOLUMEDOWN:
					learnIrCode(VOLUMEDOWN_ADDRESS, &irmp_data, &irmp_volumedown);
					speOff(SPE_ARROW_DOWN);
					speOn(SPE_ARROW_UP);
					graPuts("Volume = ?    ");
					irLearningMode = IR_LEARNING_VOLUMEUP;
					break;
					case IR_LEARNING_VOLUMEUP:
					learnIrCode(VOLUMEUP_ADDRESS, &irmp_data, &irmp_volumeup);
					speOff(SPE_ARROW_UP);
					speOn(SPE_ARROW_DOWN);
					graPuts("Bass = ?      ");
					irLearningMode = IR_LEARNING_BASSDOWN;
					break;
					case IR_LEARNING_BASSDOWN:
					learnIrCode(BASSDOWN_ADDRESS, &irmp_data, &irmp_bassdown);
					speOff(SPE_ARROW_DOWN);
					speOn(SPE_ARROW_UP);
					graPuts("Bass = ?      ");
					irLearningMode = IR_LEARNING_BASSUP;
					break;
					case IR_LEARNING_BASSUP:
					learnIrCode(BASSUP_ADDRESS, &irmp_data, &irmp_bassup);
					speOn(SPE_ARROW_DOWN);
					speOff(SPE_ARROW_UP);
					graPuts("Treble = ?    ");
					irLearningMode = IR_LEARNING_TREBLEDOWN;
					break;
					case IR_LEARNING_TREBLEDOWN:
					learnIrCode(TREBLEDOWN_ADDRESS, &irmp_data, &irmp_trebledown);
					speOn(SPE_ARROW_UP);
					speOff(SPE_ARROW_DOWN);
					graPuts("Treble = ?    ");
					irLearningMode = IR_LEARNING_TREBLEUP;
					break;
					case IR_LEARNING_TREBLEUP:
					learnIrCode(TREBLEUP_ADDRESS, &irmp_data, &irmp_trebleup);
					speOff(SPE_ARROW_UP);
					graPuts("Mono/st. = ?  ");
					irLearningMode = IR_LEARNING_MONOSTEREO;
					break;
					case IR_LEARNING_MONOSTEREO:
					learnIrCode(MONOSTEREO_ADDRESS, &irmp_data, &irmp_monostereo);
					graPuts("Display = ?   ");
					irLearningMode = IR_LEARNING_DISPLAY;
					break;
					case IR_LEARNING_DISPLAY:
					learnIrCode(DISPLAY_ADDRESS, &irmp_data, &irmp_display);
					graPuts("Program = ?   ");
					irLearningMode = IR_LEARNING_PROGRAM;
					break;
					case IR_LEARNING_PROGRAM:
					learnIrCode(PROGRAM_ADDRESS, &irmp_data, &irmp_program);
					exitSpecialMode();
					break;
				}
			}
		}
		else if (specialMode == SPECIAL_MODE_PROGRAM)
		{
			if (!wasRepetition && (getMode() == MODE_RADIO_FM || getMode() == MODE_RADIO_AM) && getMode() != MODE_STANDBY)
			{
				bool wasRecognized = false;
					
				// user pressed button to save a program into, let's check if it is valid Program button, or the Program button was pressed again to cancel
				if (memcmp(&irmp_data, &irmp_program0, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM0_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM0_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram0fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM0_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM0_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram0am = freq;
					}
						
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program1, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM1_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM1_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram1fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM1_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM1_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram1am = freq;
					}
						
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program2, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM2_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM2_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram2fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM2_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM2_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram2am = freq;
					}
						
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program3, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM3_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM3_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram3fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM3_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM3_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram3am = freq;
					}
						
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program4, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM4_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM4_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram4fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM4_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM4_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram4am = freq;
					}
						
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program5, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM5_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM5_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram5fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM5_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM5_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram5am = freq;
					}
						
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program6, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM6_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM6_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram6fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM6_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM6_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram6am = freq;
					}
						
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program7, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM7_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM7_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram7fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM7_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM7_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram7am = freq;
					}
						
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program8, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM8_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM8_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram8fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM8_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM8_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram8am = freq;
					}
						
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program9, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM9_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM9_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram9fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM9_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM9_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram9am = freq;
					}
						
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program10, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM10_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM10_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram10fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM10_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM10_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram10am = freq;
					}
				
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program11, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM11_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM11_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram11fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM11_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM11_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram11am = freq;
					}
				
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program12, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM12_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM12_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram12fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM12_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM12_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram12am = freq;
					}
				
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program13, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM13_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM13_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram13fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM13_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM13_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram13am = freq;
					}
				
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program14, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM14_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM14_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram14fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM14_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM14_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram14am = freq;
					}
				
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program15, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM15_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM15_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram15fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM15_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM15_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram15am = freq;
					}
				
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program16, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM16_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM16_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram16fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM16_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM16_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram16am = freq;
					}
				
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program17, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM17_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM17_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram17fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM17_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM17_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram17am = freq;
					}
				
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program18, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM18_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM18_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram18fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM18_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM18_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram18am = freq;
					}
				
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program19, sizeof(irmp_data) - 1) == 0)
				{
					if (getMode() == MODE_RADIO_FM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM19_FM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM19_FM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram19fm = freq;
					}
					else if (getMode() == MODE_RADIO_AM)
					{
						unsigned int freq = si4735.getCurrentFrequency();
						eeprom_update_byte((unsigned char*)PROGRAM19_AM_FREQ_ADDRESS, freq & 0xFF);
						eeprom_update_byte((unsigned char*)PROGRAM19_AM_FREQ_ADDRESS + 1, freq >> 8);
						freqProgram19am = freq;
					}
				
					wasRecognized = true;
				}
				else if (memcmp(&irmp_data, &irmp_program, sizeof(irmp_data) - 1) == 0)
				{
					wasRecognized = true;
				}
				
				if (wasRecognized == 1)
				{
					exitSpecialMode();
				}
			}
		}
		else
		{
			// compare trained code with received code
			// below we compare sizeof - 1 because we want to exclude 'flags' property (it indicates repetition)
			if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram0fm >= MIN_FREQ_MHZ && freqProgram0fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram0am >= MIN_FREQ_KHZ && freqProgram0am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program0, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram0fm, freqProgram0am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram1fm >= MIN_FREQ_MHZ && freqProgram1fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram1am >= MIN_FREQ_KHZ && freqProgram1am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program1, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram1fm, freqProgram1am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram2fm >= MIN_FREQ_MHZ && freqProgram2fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram2am >= MIN_FREQ_KHZ && freqProgram2am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program2, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram2fm, freqProgram2am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram3fm >= MIN_FREQ_MHZ && freqProgram3fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram3am >= MIN_FREQ_KHZ && freqProgram3am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program3, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram3fm, freqProgram3am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram4fm >= MIN_FREQ_MHZ && freqProgram4fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram4am >= MIN_FREQ_KHZ && freqProgram4am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program4, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram4fm, freqProgram4am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram5fm >= MIN_FREQ_MHZ && freqProgram5fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram5am >= MIN_FREQ_KHZ && freqProgram5am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program5, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram5fm, freqProgram5am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram6fm >= MIN_FREQ_MHZ && freqProgram6fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram6am >= MIN_FREQ_KHZ && freqProgram6am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program6, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram6fm, freqProgram6am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram7fm >= MIN_FREQ_MHZ && freqProgram7fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram7am >= MIN_FREQ_KHZ && freqProgram7am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program7, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram7fm, freqProgram7am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram8fm >= MIN_FREQ_MHZ && freqProgram8fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram8am >= MIN_FREQ_KHZ && freqProgram8am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program8, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram8fm, freqProgram8am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram9fm >= MIN_FREQ_MHZ && freqProgram9fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram9am >= MIN_FREQ_KHZ && freqProgram9am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program9, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram9fm, freqProgram9am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram10fm >= MIN_FREQ_MHZ && freqProgram10fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram10am >= MIN_FREQ_KHZ && freqProgram10am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program10, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram10fm, freqProgram10am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram11fm >= MIN_FREQ_MHZ && freqProgram11fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram11am >= MIN_FREQ_KHZ && freqProgram11am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program11, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram11fm, freqProgram11am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram12fm >= MIN_FREQ_MHZ && freqProgram12fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram12am >= MIN_FREQ_KHZ && freqProgram12am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program12, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram12fm, freqProgram12am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram13fm >= MIN_FREQ_MHZ && freqProgram13fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram13am >= MIN_FREQ_KHZ && freqProgram13am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program13, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram13fm, freqProgram13am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram14fm >= MIN_FREQ_MHZ && freqProgram14fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram14am >= MIN_FREQ_KHZ && freqProgram14am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program14, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram14fm, freqProgram14am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram15fm >= MIN_FREQ_MHZ && freqProgram15fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram15am >= MIN_FREQ_KHZ && freqProgram15am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program15, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram15fm, freqProgram15am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram16fm >= MIN_FREQ_MHZ && freqProgram16fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram16am >= MIN_FREQ_KHZ && freqProgram16am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program16, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram16fm, freqProgram16am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram17fm >= MIN_FREQ_MHZ && freqProgram17fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram17am >= MIN_FREQ_KHZ && freqProgram17am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program17, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram17fm, freqProgram17am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram18fm >= MIN_FREQ_MHZ && freqProgram18fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram18am >= MIN_FREQ_KHZ && freqProgram18am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program18, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram18fm, freqProgram18am);
			}
			else if ((((getMode() == MODE_STANDBY || getMode() == MODE_RADIO_FM) && freqProgram19fm >= MIN_FREQ_MHZ && freqProgram19fm <= MAX_FREQ_MHZ)
					|| (getMode() == MODE_RADIO_AM && freqProgram19am >= MIN_FREQ_KHZ && freqProgram19am <= MAX_FREQ_KHZ))
				&& memcmp(&irmp_data, &irmp_program19, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchToSavedFrequency(freqProgram19fm, freqProgram19am);
			}
			else if ((getMode() == MODE_RADIO_FM || getMode() == MODE_RADIO_AM)
				&& memcmp(&irmp_data, &irmp_f, sizeof(irmp_data) - 1) == 0
				&& specialMode == 0)
			{
				frequencyUp();
			}
			else if ((getMode() == MODE_RADIO_FM || getMode() == MODE_RADIO_AM)
				&& memcmp(&irmp_data, &irmp_r, sizeof(irmp_data) - 1) == 0
				&& specialMode == 0)
			{
				frequencyDown();
			}
			else if (getMode() != MODE_STANDBY
				&& memcmp(&irmp_data, &irmp_mute, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				toggleMute();
			}
			else if ((getMode() == MODE_RADIO_FM || getMode() == MODE_RADIO_AM)
				&& memcmp(&irmp_data, &irmp_program, sizeof(irmp_program) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				graPuts("Program # = ? ");
				speOff(SPE_ARROW_LEFT);
				speOff(SPE_ARROW_RIGHT);
				specialMode = SPECIAL_MODE_PROGRAM;
			}
			else if (memcmp(&irmp_data, &irmp_standby, sizeof(irmp_standby) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				if (getMode() == MODE_STANDBY)
				{
					on();
				}
				else
				{
					off();
				}
			}
			else if (getMode() != MODE_STANDBY
				&& memcmp(&irmp_data, &irmp_band, sizeof(irmp_band) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				switchMode();
			}
			else if (getMode() == MODE_RADIO_FM
				&& memcmp(&irmp_data, &irmp_monostereo, sizeof(irmp_monostereo) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				toggleForceMono();
			}
			else if (getMode() != MODE_STANDBY && memcmp(&irmp_data, &irmp_volumedown, sizeof(irmp_data) - 1) == 0)
			{
				volumeDown();
			}
			else if (getMode() != MODE_STANDBY && memcmp(&irmp_data, &irmp_volumeup, sizeof(irmp_data) - 1) == 0)
			{
				volumeUp();
			}
			else if (getMode() != MODE_STANDBY && memcmp(&irmp_data, &irmp_trebledown, sizeof(irmp_data) - 1) == 0)
			{
				trebleDown();
			}
			else if (getMode() != MODE_STANDBY && memcmp(&irmp_data, &irmp_trebleup, sizeof(irmp_data) - 1) == 0)
			{
				trebleUp();
			}
			else if (getMode() != MODE_STANDBY && memcmp(&irmp_data, &irmp_bassdown, sizeof(irmp_data) - 1) == 0)
			{
				bassDown();
			}
			else if (getMode() != MODE_STANDBY && memcmp(&irmp_data, &irmp_bassup, sizeof(irmp_data) - 1) == 0)
			{
				bassUp();
			}
			else if (memcmp(&irmp_data, &irmp_display, sizeof(irmp_data) - 1) == 0
				&& !wasRepetition
				&& specialMode == 0)
			{
				Wire.beginTransmission(I2C_ADDR_VFDDRIVER >> 1);
				Wire.write(CMD_TOGGLE_DISPLAY);
				Wire.endTransmission();
				delay(1);
			}
		}
	}
}

ISR(TIMER2_OVF_vect)
{
	unsigned char mode = getMode();
	if (mode == MODE_RADIO_FM && timer2OverflowCounter % 8 == 0) // do not read RDS too often, spare some MCU cycles
	{
		readRDS = true;
	}
	
	// set flag to read from RTC every second, doesn't have to be perfectly second-aligned, because we don't display seconds, only minutes
	// this is like a second prescaler of TIMER2
	if (timer2OverflowCounter < 96)
	{
		timer2OverflowCounter++;
	}
	else
	{
		timer2OverflowCounter = 0;
		
		if (mode == MODE_STANDBY)
		{
			if (setupFinished)
			{
				readFromRTC = true;
				
				if (displayedSpeForStandbyIndex == sizeof(speForStandby) - 1)
				{
					displayedSpeForStandbyIndex = 0;
				}
				else
				{
					displayedSpeForStandbyIndex++;
				}				
			}
		}
		else
		{
			if (!displayRadioText)
			{
				if (radioTextDelayCounter == 3)
				{
					radioTextDelayCounter = 0;
					displayRadioText = true;
				}
				else
				{
					radioTextDelayCounter++;
				}
			}
			else
			{
				// if there is new radio text that is shorter than the previous one and we were past the length of the new one,
				// reset position to 0 immediately
				if (radioTextScrollingStartIndex + GRA_DISPLAY_CHARS > radioTextScrollingEndIndex + 1)
				{
					radioTextScrollingStartIndex = 0;
				}
				
				bool continueScrolling = true; // indicates if enough time has passed so that we can scroll or wait a bit
		
				// wait a bit if showing radiotext from the start or it is at the end of scrolling
				if (radioTextScrollingStartIndex == 0 || (radioTextScrollingStartIndex + GRA_DISPLAY_CHARS == radioTextScrollingEndIndex + 1))
				{
					if (initialScrollDelayCounter == 3)
					{
						initialScrollDelayCounter = 0;
					}
					else
					{
						initialScrollDelayCounter++;
						continueScrolling = false;
					}
				}
			
				if (continueScrolling)
				{
					if (radioTextScrollingStartIndex + GRA_DISPLAY_CHARS <= radioTextScrollingEndIndex)
					{
						radioTextScrollingStartIndex++;
					}
					else
					{
						radioTextScrollingStartIndex = 0;
					}
				}
			}
		}
	}
}

ISR(TIMER1_COMPA_vect)
{
	(void)irmp_ISR();
}
