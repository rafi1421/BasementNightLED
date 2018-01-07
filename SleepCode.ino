//
/////////////////////////////////////////////////
///// WatchDog / Sleep Mode
/////////////////////////////////////////////////
//
//#include <avr/sleep.h>
//#include <avr/wdt.h>
//
//
//
//// Watchdog intervals
//// sleep bit patterns for WDTCSR/
////enum {
////	WDT_16_MS = 0b000000,
////	WDT_32_MS = 0b000001,
////	WDT_64_MS = 0b000010,
////	WDT_128_MS = 0b000011,
////	WDT_256_MS = 0b000100,
////	WDT_512_MS = 0b000101,
////	WDT_1_SEC = 0b000110,
////	WDT_2_SEC = 0b000111,
////	WDT_4_SEC = 0b100000,
////	WDT_8_SEC = 0b100001,
////	WDT_SLeeP = 0b100001 // dummy name since i need to pass in variable.
////};  // end of WDT intervals enum
//	/*
//	// sleep for a total of 64 seconds (8 x 8)
//	for (int i = 0; i < 8; i++)
//	if (issleep == false) break;
//	else {
//	myWatchdogEnable (WDT_8_SEC);
//	}
//	*/
//
//
//
//// watchdog interrupt / is executed when  watchdog timed out
//ISR(WDT_vect)
//{
//	wdt_disable();  // disable watchdog
//}
//
//// USAGE: in script use: goSleep(WDT_time_enum); 
//// to sleep for a certain length.
//// Use only goSleep() for infinite sleep.
//
//void goSleep(const byte interval) {
//	if (watchdog == true) { //watchdog option here
//		// clear various "reset" flags
//		MCUSR = 0;
//		// allow changes, disable reset
//		WDTCSR = _BV(WDCE) | _BV(WDE);
//		// set interrupt mode and an interval 
//		WDTCSR = _BV(WDIE) | interval;    // set WDIE, and requested delay
//		wdt_reset();  // pat the dog
//	}
//	goSleep();
//}
//
//void goSleep() {
//	#if DEBUGled
//	digitalWrite(debugled, 0);
//	#endif	
//
//	// disable ADC
//	byte old_ADCSRA = ADCSRA;
//	ADCSRA = 0;
//
//	// turn off various modules
//	byte old_PRR = PRR;
//	PRR = 0xFF;
//
//	// timed sequence coming up
//
//	noInterrupts();
//	
//	// ready to sleep
//
//	//set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//	if (!watchdog) set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//	else set_sleep_mode(SLEEP_MODE_IDLE);
//
//	sleep_enable();
//
//	// turn off brown-out enable in software
//	MCUCR = _BV(BODS) | _BV(BODSE);
//	MCUCR = _BV(BODS);
//	interrupts();
//	sleep_cpu(); 
//
//	// cancel sleep as a precaution
//	sleep_disable();
//	PRR = old_PRR;
//	ADCSRA = old_ADCSRA;  //return ACD enabled
//	
//} // end of myWatchdogEnable
//
//
