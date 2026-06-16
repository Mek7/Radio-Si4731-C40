#ifndef VFDPROTOCOL_H_
#define VFDPROTOCOL_H_

// ---------------------------------------------------------------------------
// Shared I2C protocol contract between the master (ATmega @ 20 MHz) and the
// VFD-driver slave (ATmega @ 18.432 MHz).
//
// IMPORTANT: this file MUST be byte-for-byte identical in both projects.
// If you change a value here, copy the whole file to the other project.
// ---------------------------------------------------------------------------

// ---- Commands: master -> slave (written as the first I2C data byte) --------
#define CMD_SHOW_GRA_CHAR     0
#define CMD_SHOW_NUM_CHAR     1
#define CMD_SHOW_SPE          2
#define CMD_HIDE_SPE          3
#define CMD_TOGGLE_SPE        4
#define CMD_CLEAR_GRA         5
#define CMD_CLEAR_NUM         6
#define CMD_CLEAR_ALL         7
#define CMD_SHOW_UNDERLINES   8
#define CMD_HIDE_UNDERLINES   9
#define CMD_SHOW_GRA_TEXT     10
#define CMD_SHOW_NUM_TEXT     11
#define CMD_SHOW_VOLUME       12
#define CMD_TOGGLE_DISPLAY    13

// Number of payload bytes each multi-byte command expects AFTER the command
// byte. Single-byte commands (clears, underlines, toggle display) are 0.
// The master uses these to know how many bytes to send; the slave uses them
// to know when a command is complete. Keeping them here guarantees both
// sides agree.
#define CMD_PAYLOAD_GRA_CHAR  2   // char code + index
#define CMD_PAYLOAD_NUM_CHAR  2   // char code + index
#define CMD_PAYLOAD_SPE       1   // spe code
#define CMD_PAYLOAD_GRA_TEXT  14  // exactly 14 chars
#define CMD_PAYLOAD_NUM_TEXT  6   // exactly 6 chars
#define CMD_PAYLOAD_VOLUME    1   // 0-63

// ---- Status: slave -> master (returned on an I2C read) ---------------------
// The protocol is one-way for *data*, but the master may read a single status
// byte to find out whether the slave is ready for the next command. This
// closes the race where the slave is busy in memcpy_P()/switch and silently
// drops incoming bytes.
#define STATUS_READY          0xA5  // slave idle, safe to send a new command
#define STATUS_BUSY           0x5A  // slave is mid-command, do not send yet

// ---- Special segment identifiers ------------------------------------------
#define SPE_SIRIUS            0
#define SPE_OUT               1
#define SPE_HDMI              2
#define SPE_ONE_SQUARED       3
#define SPE_TWO_SQUARED       4
#define SPE_ARROW_LEFT        5
#define SPE_ARROW_UP          6
#define SPE_ARROW_DOWN        7
#define SPE_COLON_UPPER       8
#define SPE_COLON_LOWER       9
#define SPE_IPOD_CHARGE       10
#define SPE_SPB               11
#define SPE_RECTANGLE1        12
#define SPE_SPA               13
#define SPE_PARTY             14
#define SPE_HD                15
#define SPE_TAG               16
#define SPE_CINEMA_DSP        17
#define SPE_THREE_SQUARED     18
#define SPE_STEREO            19
#define SPE_TUNED             20
#define SPE_ENHANCER          21
#define SPE_ADAPTIVE_DRC      22
#define SPE_PL                23
#define SPE_SW                24
#define SPE_PR                25
#define SPE_L                 26
#define SPE_C                 27
#define SPE_R                 28
#define SPE_SL                29
#define SPE_SR                30
#define SPE_SBL               31
#define SPE_SB                32
#define SPE_SBR               33
#define SPE_ARROW_RIGHT       34
#define SPE_RECTANGLE2        35
#define SPE_MUTE              36
#define SPE_ZONE2             37
#define SPE_ZONE3             38
#define SPE_SLEEP             39
#define SPE_VOLDB             40

#endif