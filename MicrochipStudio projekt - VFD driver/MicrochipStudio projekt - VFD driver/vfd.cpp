#include <avr/pgmspace.h>
#include "vfd.hpp"

// the ones for numbers and characters are only base ones, to make them light up we have to set the correct anode to 1 as well
// the characters that are only once on the VFD (such as semicolon, HDMI sign...) have the correct anode already set to 1 here
// here, 1 = on, 0 = off, but these are negated before use
// Num*a and Num*b is even/odd segment of the numeric VFD part
const struct VfdState PROGMEM Num0a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00100011,
	.chip1 = 0b00110001
};

const struct VfdState PROGMEM Num1a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000001,
	.chip1 = 0b00011000
};

const struct VfdState PROGMEM Num2a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00100010,
	.chip1 = 0b11010001
};

const struct VfdState PROGMEM Num3a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00100001,
	.chip1 = 0b01010001
};

const struct VfdState PROGMEM Num4a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000001,
	.chip1 = 0b11110000
};

const struct VfdState PROGMEM Num5a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00100001,
	.chip1 = 0b11100001
};

const struct VfdState PROGMEM Num6a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00100011,
	.chip1 = 0b11100001
};

const struct VfdState PROGMEM Num7a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00001000,
	.chip1 = 0b00001001
};

const struct VfdState PROGMEM Num8a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00100011,
	.chip1 = 0b11110001
};

const struct VfdState PROGMEM Num9a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00100001,
	.chip1 = 0b11110001
};

const struct VfdState PROGMEM NumAa = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000011,
	.chip1 = 0b11110001
};

const struct VfdState PROGMEM NumFa = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000010,
	.chip1 = 0b11100001
};

const struct VfdState PROGMEM NumMa = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000011,
	.chip1 = 0b00111010
};

const struct VfdState PROGMEM Num0b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00001000,
	.chip3 = 0b11001100,
	.chip2 = 0b01000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Num1b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b01000110,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Num2b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00001000,
	.chip3 = 0b10110100,
	.chip2 = 0b01000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Num3b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00001000,
	.chip3 = 0b01010100,
	.chip2 = 0b01000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Num4b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b01111100,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Num5b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00001000,
	.chip3 = 0b01111000,
	.chip2 = 0b01000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Num6b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00001000,
	.chip3 = 0b11111000,
	.chip2 = 0b01000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Num7b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000010,
	.chip3 = 0b00000010,
	.chip2 = 0b01000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Num8b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00001000,
	.chip3 = 0b11111100,
	.chip2 = 0b01000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Num9b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00001000,
	.chip3 = 0b01111100,
	.chip2 = 0b01000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM NumAb = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b11111100,
	.chip2 = 0b01000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM NumFb = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b10111000,
	.chip2 = 0b01000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM NumMb = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b11001110,
	.chip2 = 0b10000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM NumSpacea = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM NumSpaceb = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraSpace = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraExclamationMark = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00000000,
	.chip3 = 0b00000010,
	.chip2 = 0b00010000,
	.chip1 = 0b10000100
};

const struct VfdState PROGMEM GraQuote = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00101001,
	.chip1 = 0b01001010
};

const struct VfdState PROGMEM GraHash = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000010,
	.chip4 = 0b10010101,
	.chip3 = 0b11110101,
	.chip2 = 0b01111101,
	.chip1 = 0b01001010
};

const struct VfdState PROGMEM GraDollar = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00011111,
	.chip3 = 0b01001111,
	.chip2 = 0b00010111,
	.chip1 = 0b11000100
};

const struct VfdState PROGMEM GraPercent = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000110,
	.chip4 = 0b00110010,
	.chip3 = 0b00100010,
	.chip2 = 0b00100010,
	.chip1 = 0b01100011
};

const struct VfdState PROGMEM GraAmpersand = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000101,
	.chip4 = 0b10010011,
	.chip3 = 0b01011001,
	.chip2 = 0b00010100,
	.chip1 = 0b10100010
};

const struct VfdState PROGMEM GraApostropheLeft = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00001000,
	.chip1 = 0b10000110
};

const struct VfdState PROGMEM GraParenthesisLeft = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000010,
	.chip4 = 0b00001000,
	.chip3 = 0b00100001,
	.chip2 = 0b00001000,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraParenthesisRight = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b10001000,
	.chip3 = 0b10000100,
	.chip2 = 0b00100000,
	.chip1 = 0b10000010
};

const struct VfdState PROGMEM GraAsterisk = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00101010,
	.chip3 = 0b11100010,
	.chip2 = 0b00111010,
	.chip1 = 0b10100100
};

const struct VfdState PROGMEM GraPlus = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00001000,
	.chip3 = 0b01001111,
	.chip2 = 0b10010000,
	.chip1 = 0b10000000
};

const struct VfdState PROGMEM GraComma = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b10001000,
	.chip3 = 0b01100000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraMinus = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00001111,
	.chip2 = 0b10000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraDot = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b10001100,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraForwardSlash = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000010,
	.chip3 = 0b00100010,
	.chip2 = 0b00100010,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Gra0 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100011,
	.chip3 = 0b00111010,
	.chip2 = 0b11100110,
	.chip1 = 0b00101110
};

const struct VfdState PROGMEM Gra1 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10001000,
	.chip3 = 0b01000010,
	.chip2 = 0b00010000,
	.chip1 = 0b11000100
};

const struct VfdState PROGMEM Gra2 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b11000100,
	.chip3 = 0b01000100,
	.chip2 = 0b01000010,
	.chip1 = 0b00101110
};

const struct VfdState PROGMEM Gra3 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100011,
	.chip3 = 0b00000100,
	.chip2 = 0b00010001,
	.chip1 = 0b00011111
};

const struct VfdState PROGMEM Gra4 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000010,
	.chip4 = 0b00010001,
	.chip3 = 0b11110100,
	.chip2 = 0b10101001,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM Gra5 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100011,
	.chip3 = 0b00001000,
	.chip2 = 0b00111100,
	.chip1 = 0b00111111
};

const struct VfdState PROGMEM Gra6 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100011,
	.chip3 = 0b00010111,
	.chip2 = 0b10000100,
	.chip1 = 0b01001100
};

const struct VfdState PROGMEM Gra7 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00001000,
	.chip3 = 0b01000100,
	.chip2 = 0b00100010,
	.chip1 = 0b00011111
};

const struct VfdState PROGMEM Gra8 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100011,
	.chip3 = 0b00010111,
	.chip2 = 0b01000110,
	.chip1 = 0b00101110
};

const struct VfdState PROGMEM Gra9 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b10010001,
	.chip3 = 0b00001111,
	.chip2 = 0b01000110,
	.chip1 = 0b00101110
};

const struct VfdState PROGMEM GraColon = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00001100,
	.chip3 = 0b01100000,
	.chip2 = 0b00011000,
	.chip1 = 0b11000000
};

const struct VfdState PROGMEM GraSemicolon = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b10001000,
	.chip3 = 0b01100000,
	.chip2 = 0b00011000,
	.chip1 = 0b11000000
};

const struct VfdState PROGMEM GraAngleBracketLeft = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000010,
	.chip4 = 0b00001000,
	.chip3 = 0b00100000,
	.chip2 = 0b10001000,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraEquals = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000001,
	.chip3 = 0b11110000,
	.chip2 = 0b01111100,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraAngleBracketRight = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b10001000,
	.chip3 = 0b10001000,
	.chip2 = 0b00100000,
	.chip1 = 0b10000010
};

const struct VfdState PROGMEM GraQuestionMark = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00000000,
	.chip3 = 0b01000100,
	.chip2 = 0b01000010,
	.chip1 = 0b00101110
};

const struct VfdState PROGMEM GraAt = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10000011,
	.chip3 = 0b11011010,
	.chip2 = 0b11110110,
	.chip1 = 0b00101110
};

const struct VfdState PROGMEM GraUpperA = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01100011,
	.chip3 = 0b11111000,
	.chip2 = 0b11000101,
	.chip1 = 0b01000100
};

const struct VfdState PROGMEM GraUpperB = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b11100011,
	.chip3 = 0b00010111,
	.chip2 = 0b11000110,
	.chip1 = 0b00101111
};

const struct VfdState PROGMEM GraUpperC = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100010,
	.chip3 = 0b00010000,
	.chip2 = 0b10000110,
	.chip1 = 0b00101110
};

const struct VfdState PROGMEM GraUpperD = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b11010011,
	.chip3 = 0b00011000,
	.chip2 = 0b11000101,
	.chip1 = 0b00100111
};

const struct VfdState PROGMEM GraUpperE = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b11000010,
	.chip3 = 0b00010111,
	.chip2 = 0b10000100,
	.chip1 = 0b00111111
};

const struct VfdState PROGMEM GraUpperF = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b01000010,
	.chip3 = 0b00010111,
	.chip2 = 0b10000100,
	.chip1 = 0b00111111
};

const struct VfdState PROGMEM GraUpperG = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b10100011,
	.chip3 = 0b00011110,
	.chip2 = 0b10000110,
	.chip1 = 0b00101110
};

const struct VfdState PROGMEM GraUpperH = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01100011,
	.chip3 = 0b00011111,
	.chip2 = 0b11000110,
	.chip1 = 0b00110001
};

const struct VfdState PROGMEM GraUpperI = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10001000,
	.chip3 = 0b01000010,
	.chip2 = 0b00010000,
	.chip1 = 0b10001110
};

const struct VfdState PROGMEM GraUpperJ = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b10010010,
	.chip3 = 0b10000100,
	.chip2 = 0b00100001,
	.chip1 = 0b00011100
};

const struct VfdState PROGMEM GraUpperK = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01010010,
	.chip3 = 0b01010001,
	.chip2 = 0b10010101,
	.chip1 = 0b00110001
};

const struct VfdState PROGMEM GraUpperL = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b11000010,
	.chip3 = 0b00010000,
	.chip2 = 0b10000100,
	.chip1 = 0b00100001
};

const struct VfdState PROGMEM GraUpperM = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01100011,
	.chip3 = 0b00011010,
	.chip2 = 0b11010111,
	.chip1 = 0b01110001
};

const struct VfdState PROGMEM GraUpperN = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01100011,
	.chip3 = 0b10011010,
	.chip2 = 0b11001110,
	.chip1 = 0b00110001
};

const struct VfdState PROGMEM GraUpperO = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100011,
	.chip3 = 0b00011000,
	.chip2 = 0b11000110,
	.chip1 = 0b00101110
};

const struct VfdState PROGMEM GraUpperP = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b01000010,
	.chip3 = 0b00010111,
	.chip2 = 0b11000110,
	.chip1 = 0b00101111
};

const struct VfdState PROGMEM GraUpperQ = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000101,
	.chip4 = 0b10010011,
	.chip3 = 0b01011000,
	.chip2 = 0b11000110,
	.chip1 = 0b00101110
};

const struct VfdState PROGMEM GraUpperR = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01010010,
	.chip3 = 0b01010111,
	.chip2 = 0b11000110,
	.chip1 = 0b00101111
};

const struct VfdState PROGMEM GraUpperS = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100011,
	.chip3 = 0b00000111,
	.chip2 = 0b00000110,
	.chip1 = 0b00101110
};

const struct VfdState PROGMEM GraUpperT = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00001000,
	.chip3 = 0b01000010,
	.chip2 = 0b00010000,
	.chip1 = 0b10011111
};

const struct VfdState PROGMEM GraUpperU = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100011,
	.chip3 = 0b00011000,
	.chip2 = 0b11000110,
	.chip1 = 0b00110001
};

const struct VfdState PROGMEM GraUpperV = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00010101,
	.chip3 = 0b00011000,
	.chip2 = 0b11000110,
	.chip1 = 0b00110001
};

const struct VfdState PROGMEM GraUpperW = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000010,
	.chip4 = 0b10101011,
	.chip3 = 0b01011010,
	.chip2 = 0b11000110,
	.chip1 = 0b00110001
};

const struct VfdState PROGMEM GraUpperX = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01100010,
	.chip3 = 0b10100010,
	.chip2 = 0b00101010,
	.chip1 = 0b00110001
};

const struct VfdState PROGMEM GraUpperY = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00001000,
	.chip3 = 0b01000010,
	.chip2 = 0b00101010,
	.chip1 = 0b00110001
};

const struct VfdState PROGMEM GraUpperZ = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b11000010,
	.chip3 = 0b00100010,
	.chip2 = 0b00100010,
	.chip1 = 0b00011111
};

const struct VfdState PROGMEM GraSquareBracketLeft = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10000100,
	.chip3 = 0b00100001,
	.chip2 = 0b00001000,
	.chip1 = 0b01001110
};

const struct VfdState PROGMEM GraBackSlash = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00100000,
	.chip3 = 0b10000010,
	.chip2 = 0b00001000,
	.chip1 = 0b00100000
};

const struct VfdState PROGMEM GraSquareBracketRight = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10010000,
	.chip3 = 0b10000100,
	.chip2 = 0b00100001,
	.chip1 = 0b00001110
};

const struct VfdState PROGMEM GraCaret = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b01000101,
	.chip1 = 0b01000100
};

const struct VfdState PROGMEM GraUnderscore = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b11000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraApostropheRight = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00100000,
	.chip1 = 0b10001100
};

const struct VfdState PROGMEM GraLowerA = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b10100011,
	.chip3 = 0b11101000,
	.chip2 = 0b00111000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerB = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b11100011,
	.chip3 = 0b00011000,
	.chip2 = 0b10111100,
	.chip1 = 0b00100001
};

const struct VfdState PROGMEM GraLowerC = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b10000010,
	.chip3 = 0b00010000,
	.chip2 = 0b11111000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerD = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b10100011,
	.chip3 = 0b00011000,
	.chip2 = 0b11111010,
	.chip1 = 0b00010000
};

const struct VfdState PROGMEM GraLowerE = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10000011,
	.chip3 = 0b11111000,
	.chip2 = 0b10111000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerF = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00001000,
	.chip3 = 0b01000111,
	.chip2 = 0b00010010,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraLowerG = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100001,
	.chip3 = 0b11101000,
	.chip2 = 0b11111000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerH = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01100011,
	.chip3 = 0b00011001,
	.chip2 = 0b10110100,
	.chip1 = 0b00100001
};

const struct VfdState PROGMEM GraLowerI = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10001000,
	.chip3 = 0b01000010,
	.chip2 = 0b00011000,
	.chip1 = 0b00000100
};

const struct VfdState PROGMEM GraLowerJ = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b10010010,
	.chip3 = 0b10000100,
	.chip2 = 0b00110000,
	.chip1 = 0b00001000
};

const struct VfdState PROGMEM GraLowerK = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b10010100,
	.chip3 = 0b01100101,
	.chip2 = 0b01001000,
	.chip1 = 0b01000010
};

const struct VfdState PROGMEM GraLowerL = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10001000,
	.chip3 = 0b01000010,
	.chip2 = 0b00010000,
	.chip1 = 0b10000110
};

const struct VfdState PROGMEM GraLowerM = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000101,
	.chip4 = 0b01101011,
	.chip3 = 0b01011010,
	.chip2 = 0b10101100,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerN = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01100011,
	.chip3 = 0b00011001,
	.chip2 = 0b10110100,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerO = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100011,
	.chip3 = 0b00011000,
	.chip2 = 0b10111000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerP = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b01000010,
	.chip3 = 0b11111000,
	.chip2 = 0b10111100,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerQ = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b00100001,
	.chip3 = 0b11101000,
	.chip2 = 0b11111000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerR = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b10000100,
	.chip3 = 0b00100011,
	.chip2 = 0b01101000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerS = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b11100000,
	.chip3 = 0b11100000,
	.chip2 = 0b10111000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerT = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000010,
	.chip4 = 0b00101000,
	.chip3 = 0b01000010,
	.chip2 = 0b00111000,
	.chip1 = 0b10000100
};

const struct VfdState PROGMEM GraLowerU = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000101,
	.chip4 = 0b10110011,
	.chip3 = 0b00011000,
	.chip2 = 0b11000100,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerV = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00010101,
	.chip3 = 0b00011000,
	.chip2 = 0b11000100,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerW = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000010,
	.chip4 = 0b10101011,
	.chip3 = 0b01011000,
	.chip2 = 0b11000100,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerX = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01010100,
	.chip3 = 0b01000101,
	.chip2 = 0b01000100,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerY = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100001,
	.chip3 = 0b11101000,
	.chip2 = 0b11000100,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraLowerZ = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b11000100,
	.chip3 = 0b01000100,
	.chip2 = 0b01111100,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM GraCurlyBracketLeft = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000110,
	.chip4 = 0b00001000,
	.chip3 = 0b01000001,
	.chip2 = 0b00010000,
	.chip1 = 0b10011000
};

const struct VfdState PROGMEM GraPipe = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00001000,
	.chip3 = 0b01000010,
	.chip2 = 0b00010000,
	.chip1 = 0b10000100
};

const struct VfdState PROGMEM GraCurlyBracketRight = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b11001000,
	.chip3 = 0b01000001,
	.chip2 = 0b00010000,
	.chip1 = 0b10000011
};

const struct VfdState PROGMEM GraTilde = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00100010,
	.chip1 = 0b10100010
};

const struct VfdState PROGMEM GraCaronS = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b11100000,
	.chip3 = 0b11100000,
	.chip2 = 0b10111000,
	.chip1 = 0b10001010
};

const struct VfdState PROGMEM GraCaronLowerL = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10001000,
	.chip3 = 0b01000010,
	.chip2 = 0b00010010,
	.chip1 = 0b10010110
};

const struct VfdState PROGMEM GraCaronUpperL = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b11000010,
	.chip3 = 0b00010000,
	.chip2 = 0b10000101,
	.chip1 = 0b00110101
};

const struct VfdState PROGMEM GraCaronC = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b10000010,
	.chip3 = 0b00010000,
	.chip2 = 0b11111000,
	.chip1 = 0b10001010
};

const struct VfdState PROGMEM GraCaronUpperT = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00001000,
	.chip3 = 0b01000010,
	.chip2 = 0b01111100,
	.chip1 = 0b10001010
};

const struct VfdState PROGMEM GraCaronLowerT = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000010,
	.chip4 = 0b00101000,
	.chip3 = 0b01000010,
	.chip2 = 0b00111010,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraCaronZ = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b11000100,
	.chip3 = 0b01000100,
	.chip2 = 0b01111100,
	.chip1 = 0b10001010
};

const struct VfdState PROGMEM GraAcuteLowerY = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100001,
	.chip3 = 0b11101000,
	.chip2 = 0b11000100,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraAcuteUpperY = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000001,
	.chip4 = 0b00001000,
	.chip3 = 0b01000101,
	.chip2 = 0b01000110,
	.chip1 = 0b10101000
};

const struct VfdState PROGMEM GraAcuteLowerA = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b10100011,
	.chip3 = 0b11101000,
	.chip2 = 0b00111000,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraAcuteUpperA = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01100011,
	.chip3 = 0b11111000,
	.chip2 = 0b10101000,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraAcuteLowerI = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10001000,
	.chip3 = 0b01000011,
	.chip2 = 0b00000000,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraAcuteUpperI = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10001000,
	.chip3 = 0b01000010,
	.chip2 = 0b00111000,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraAcuteLowerE = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b10000011,
	.chip3 = 0b11111000,
	.chip2 = 0b10111000,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraAcuteUpperE = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b11000010,
	.chip3 = 0b11110000,
	.chip2 = 0b11111100,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraAcuteLowerO = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100011,
	.chip3 = 0b00010111,
	.chip2 = 0b00000000,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraAcuteUpperO = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000011,
	.chip4 = 0b10100011,
	.chip3 = 0b00011000,
	.chip2 = 0b10111000,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraAcuteLowerU = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000101,
	.chip4 = 0b10110011,
	.chip3 = 0b00011000,
	.chip2 = 0b10000000,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraAcuteUpperU = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000101,
	.chip4 = 0b10110011,
	.chip3 = 0b00011000,
	.chip2 = 0b11000100,
	.chip1 = 0b10001000
};

const struct VfdState PROGMEM GraDiaeresisUpperA = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01111111,
	.chip3 = 0b00010101,
	.chip2 = 0b00010000,
	.chip1 = 0b00001010
};

const struct VfdState PROGMEM GraDiaeresisLowerA = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000111,
	.chip4 = 0b10100011,
	.chip3 = 0b11101000,
	.chip2 = 0b00111000,
	.chip1 = 0b00001010
};

const struct VfdState PROGMEM GraCaronLowerN = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00000100,
	.chip4 = 0b01100011,
	.chip3 = 0b00110110,
	.chip2 = 0b10000000,
	.chip1 = 0b10001010
};

const struct VfdState PROGMEM SpeSirius = { 
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00010000,
	.chip4 = 0b00010000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};
	
const struct VfdState PROGMEM SpeOut = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00010000,
	.chip4 = 0b00100000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};
	
const struct VfdState PROGMEM SpeHdmi = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00010000,
	.chip4 = 0b01000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};
	
const struct VfdState PROGMEM SpeOneSquared = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00010000,
	.chip4 = 0b10000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};
	
const struct VfdState PROGMEM SpeTwoSquared = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00010001,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};
	
const struct VfdState PROGMEM SpeArrowLeft = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00010010,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};
	
const struct VfdState PROGMEM SpeArrowUp = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00010100,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};
	
const struct VfdState PROGMEM SpeArrowDown = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00011000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeNumColonUpper = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00100000,
	.chip4 = 0b00010000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeNumColonLower = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00100000,
	.chip4 = 0b00100000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeIpodCharge = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00100000,
	.chip4 = 0b01000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeSpb = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00100000,
	.chip4 = 0b10000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeRectangle1 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00100001,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeSpa = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00100010,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeParty = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b00100100,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeHd = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b01000000,
	.chip4 = 0b00010000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeTag = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b01000000,
	.chip4 = 0b00100000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeCinemaDsp = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b01000000,
	.chip4 = 0b01000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeThreeSquared = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b01000000,
	.chip4 = 0b10000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeStereo = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b01000001,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeTuned = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b01000010,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeEnhancer = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b01000100,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeAdaptiveDrc = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b01001000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpePL = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00001000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeSW = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00010000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpePR = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00100000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeL = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b01000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeC = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b10000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeR = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000001,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeSL = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000010,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeSR = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000100,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeSBL = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00001000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeSB = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00010000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeSBR = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00100000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeArrowRight = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b01000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeRectangle2 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b10000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeMute = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000001,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeZone2 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000010,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeZone3 = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000100,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeSleep = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10001000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM SpeVolDb = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b00000001
};

const struct VfdState PROGMEM Vol0a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000010,
	.chip2 = 0b10100001,
	.chip1 = 0b00100100
};

const struct VfdState PROGMEM Vol1a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b10000001,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Vol2a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000010,
	.chip2 = 0b10001000,
	.chip1 = 0b00100100
};

const struct VfdState PROGMEM Vol3a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000010,
	.chip2 = 0b10001001,
	.chip1 = 0b00000100
};

const struct VfdState PROGMEM Vol4a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b10101001,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Vol5a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000010,
	.chip2 = 0b00101001,
	.chip1 = 0b00000100
};

const struct VfdState PROGMEM Vol6a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000010,
	.chip2 = 0b00101001,
	.chip1 = 0b00100100
};

const struct VfdState PROGMEM Vol7a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000010,
	.chip2 = 0b10000001,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Vol8a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000010,
	.chip2 = 0b10101001,
	.chip1 = 0b00100100
};

const struct VfdState PROGMEM Vol9a = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000010,
	.chip2 = 0b10101001,
	.chip1 = 0b00000100
};

const struct VfdState PROGMEM Vol0b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000101,
	.chip2 = 0b01000010,
	.chip1 = 0b01001000
};

const struct VfdState PROGMEM Vol1b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000001,
	.chip2 = 0b00000010,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Vol2b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000101,
	.chip2 = 0b00010000,
	.chip1 = 0b01001000
};

const struct VfdState PROGMEM Vol3b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000101,
	.chip2 = 0b00010010,
	.chip1 = 0b00001000
};

const struct VfdState PROGMEM Vol4b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000001,
	.chip2 = 0b01010010,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Vol5b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000100,
	.chip2 = 0b01010010,
	.chip1 = 0b00001000
};

const struct VfdState PROGMEM Vol6b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000100,
	.chip2 = 0b01010010,
	.chip1 = 0b01001000
};

const struct VfdState PROGMEM Vol7b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000101,
	.chip2 = 0b00000010,
	.chip1 = 0b00000000
};

const struct VfdState PROGMEM Vol8b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000101,
	.chip2 = 0b01010010,
	.chip1 = 0b01001000
};

const struct VfdState PROGMEM Vol9b = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000101,
	.chip2 = 0b01010010,
	.chip1 = 0b00001000
};

const struct VfdState PROGMEM Vol0c = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000000,
	.chip1 = 0b10010000
};

const struct VfdState PROGMEM Vol5c = {
	.chip7 = 0b00000000,
	.chip6 = 0b00000000,
	.chip5 = 0b10000000,
	.chip4 = 0b00000000,
	.chip3 = 0b00000000,
	.chip2 = 0b00000100,
	.chip1 = 0b00010000
};

const VfdState GraAsciiTable[] = {
	GraSpace,
	GraExclamationMark,
	GraQuote,
	GraHash,
	GraDollar,
	GraPercent,
	GraAmpersand,
	GraApostropheLeft,
	GraParenthesisLeft,
	GraParenthesisRight,
	GraAsterisk,
	GraPlus,
	GraComma,
	GraMinus,
	GraDot,
	GraForwardSlash,
	Gra0,
	Gra1,
	Gra2,
	Gra3,
	Gra4,
	Gra5,
	Gra6,
	Gra7,
	Gra8,
	Gra9,
	GraColon,
	GraSemicolon,
	GraAngleBracketLeft,
	GraEquals,
	GraAngleBracketRight,
	GraQuestionMark,
	GraAt,
	GraUpperA,
	GraUpperB,
	GraUpperC,
	GraUpperD,
	GraUpperE,
	GraUpperF,
	GraUpperG,
	GraUpperH,
	GraUpperI,
	GraUpperJ,
	GraUpperK,
	GraUpperL,
	GraUpperM,
	GraUpperN,
	GraUpperO,
	GraUpperP,
	GraUpperQ,
	GraUpperR,
	GraUpperS,
	GraUpperT,
	GraUpperU,
	GraUpperV,
	GraUpperW,
	GraUpperX,
	GraUpperY,
	GraUpperZ,
	GraSquareBracketLeft,
	GraBackSlash,
	GraSquareBracketRight,
	GraCaret,
	GraUnderscore,
	GraApostropheRight,
	GraLowerA,
	GraLowerB,
	GraLowerC,
	GraLowerD,
	GraLowerE,
	GraLowerF,
	GraLowerG,
	GraLowerH,
	GraLowerI,
	GraLowerJ,
	GraLowerK,
	GraLowerL,
	GraLowerM,
	GraLowerN,
	GraLowerO,
	GraLowerP,
	GraLowerQ,
	GraLowerR,
	GraLowerS,
	GraLowerT,
	GraLowerU,
	GraLowerV,
	GraLowerW,
	GraLowerX,
	GraLowerY,
	GraLowerZ,
	GraCurlyBracketLeft,
	GraPipe,
	GraCurlyBracketRight,
	GraTilde
};
