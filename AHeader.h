#pragma once
enum
{
	WDT_16_MS = 0b000000,
	WDT_32_MS = 0b000001,
	WDT_64_MS = 0b000010,
	WDT_128_MS = 0b000011,
	WDT_256_MS = 0b000100,
	WDT_512_MS = 0b000101,
	WDT_1_SEC = 0b000110,
	WDT_2_SEC = 0b000111,
	WDT_4_SEC = 0b100000,
	WDT_8_SEC = 0b100001,
	WDT_SLeeP = 0b100001 // dummy name since i need to pass in variable.
};  // end of WDT intervals enum