#ifndef DEFINES_H_
#define DEFINES_H_

// ---------------------------------------------------------------------------
// Master (main ATmega) build configuration.
// This chip runs at 20 MHz. The IRMP decoder derives all of its pulse/pause
// tolerance windows from F_CPU via F_INTERRUPTS, so an incorrect F_CPU would
// silently corrupt IR decoding. Fail loudly instead.
// ---------------------------------------------------------------------------
#ifndef F_CPU
#  error "F_CPU must be defined by the build system (expected 20000000UL for the master)."
#endif
#if F_CPU != 20000000UL
#  warning "Master F_CPU is not 20 MHz; verify F_INTERRUPTS/IRMP timing and TIMER2 period."
#endif

// define constants to map raw AVR pins to arduino pin numbers
#define PB0A 8
#define PB1A 9
#define PB2A 10
#define PB3A 11
#define PB4A 12
#define PB5A 13

#define PC0A 14
#define PC1A 15
#define PC2A 16
#define PC3A 17

#define PD0A 0
#define PD1A 1
#define PD2A 2
#define PD3A 3
#define PD4A 4
#define PD5A 5
#define PD6A 6
#define PD7A 7

#define I2C_ADDR_RTC 0xD0
#define I2C_ADDR_VFDDRIVER 0xF4
#define I2C_ADDR_AUDIOPROC 0b10001000

// Pre-shifted 7-bit addresses for the Arduino Wire API (which wants 7-bit).
// Using these avoids repeating ">> 1" at every call site, where a single
// missed shift would silently talk to the wrong device.
#define I2C7_RTC        (I2C_ADDR_RTC >> 1)
#define I2C7_VFDDRIVER  (I2C_ADDR_VFDDRIVER >> 1)
#define I2C7_AUDIOPROC  (I2C_ADDR_AUDIOPROC >> 1)

#ifndef _BV
#  define _BV(bit) (1 << (bit))
#endif

#define STATION_NAME_CHARS 8
#define RADIOTEXT_CHARS 64
#define GRA_DISPLAY_CHARS 14
#define NUM_DISPLAY_CHARS 6

// Timer2 prescaler = 1024 (CS22:CS21:CS20 = 111).
// At F_CPU = 20 MHz this gives an overflow every 256 * 1024 / 20e6 = 13.107 ms.
#define TIMER2_TCCR2B (_BV(CS20) | _BV(CS21) | _BV(CS22))

#define IR_LEARNING_PROGRAM0 1
#define IR_LEARNING_PROGRAM1 2
#define IR_LEARNING_PROGRAM2 3
#define IR_LEARNING_PROGRAM3 4
#define IR_LEARNING_PROGRAM4 5
#define IR_LEARNING_PROGRAM5 6
#define IR_LEARNING_PROGRAM6 7
#define IR_LEARNING_PROGRAM7 8
#define IR_LEARNING_PROGRAM8 9
#define IR_LEARNING_PROGRAM9 10
#define IR_LEARNING_PROGRAM10 11
#define IR_LEARNING_PROGRAM11 12
#define IR_LEARNING_PROGRAM12 13
#define IR_LEARNING_PROGRAM13 14
#define IR_LEARNING_PROGRAM14 15
#define IR_LEARNING_PROGRAM15 16
#define IR_LEARNING_PROGRAM16 17
#define IR_LEARNING_PROGRAM17 18
#define IR_LEARNING_PROGRAM18 19
#define IR_LEARNING_PROGRAM19 20
#define IR_LEARNING_R 111
#define IR_LEARNING_F 112
#define IR_LEARNING_MUTE 113
#define IR_LEARNING_STANDBY 114
#define IR_LEARNING_BAND 115
#define IR_LEARNING_PROGRAM 116
#define IR_LEARNING_MONOSTEREO 117
#define IR_LEARNING_VOLUMEDOWN 118
#define IR_LEARNING_VOLUMEUP 119
#define IR_LEARNING_BASSDOWN 120
#define IR_LEARNING_BASSUP 121
#define IR_LEARNING_TREBLEDOWN 122
#define IR_LEARNING_TREBLEUP 123
#define IR_LEARNING_DISPLAY 124

#define MIN_VOLUME 0
#define MAX_VOLUME 63
#define DEFAULT_VOLUME 20 // 0-63, the lower the louder (for TDA7468)

#define MIN_FREQ_MHZ 8750
#define MAX_FREQ_MHZ 10800
#define MIN_FREQ_KHZ 531
#define MAX_FREQ_KHZ 1701
#define DEFAULT_FREQ_MHZ MIN_FREQ_MHZ
#define DEFAULT_FREQ_KHZ MIN_FREQ_KHZ

#define BUTTON_PRESS_DELAY_MS 180
#define BUTTON_DEBOUNCE_DELAY_MS 10

#define SPECIAL_MODE_IR_LEARNING 1
#define SPECIAL_MODE_PROGRAM 2

#define INPUT_IN1_TUNER 0
#define INPUT_IN2_BT 1
#define OUTPUT_OFF 0
#define OUTPUT_ON 1
#define SI4735_VOLUME 50 // always the same, volume is regulated by TDA7468
#define DEFAULT_TREBLEBASS 0b01110111 // 0 dB treble, 0 dB bass

#define MODE_RADIO_FM 0
#define MODE_RADIO_AM 1
#define MODE_BT 2
#define MODE_STANDBY 3

#endif