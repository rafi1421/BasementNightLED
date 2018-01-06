
///////////////////////////////////////////////
/// WatchDog / Sleep Mode
///////////////////////////////////////////////

#include <avr/sleep.h>
#include <avr/wdt.h>
//#include <AHeader.h>




// i dont think i need this anymore: //boolean sleepType = false; //false is pwr down. true is idle(keeps clock on)  



// watchdog intervals
// sleep bit patterns for WDTCSR


	/*
	// sleep for a total of 64 seconds (8 x 8)
	for (int i = 0; i < 8; i++)
	if (issleep == false) break;
	else {
	myWatchdogEnable (WDT_8_SEC);
	}
	*/



// watchdog interrupt / is executed when  watchdog timed out
ISR(WDT_vect)
{
	wdt_disable();  // disable watchdog
}


//to use in script use: goSleep(WDT_time_enum); 

void goSleep(const byte interval) {
	#if DEBUGled
	digitalWrite(debugled, 0);
	#endif	

	if (watchdog == true) { //watchdog option here
		// clear various "reset" flags
		MCUSR = 0;
		// allow changes, disable reset
		WDTCSR = _BV(WDCE) | _BV(WDE);
		// set interrupt mode and an interval 
		WDTCSR = _BV(WDIE) | interval;    // set WDIE, and requested delay
		wdt_reset();  // pat the dog


	}
	// disable ADC
	byte old_ADCSRA = ADCSRA;
	ADCSRA = 0;

	// turn off various modules
	byte old_PRR = PRR;
	PRR = 0xFF;

	// timed sequence coming up

	noInterrupts();
	
	// ready to sleep

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//if (!watchdog) set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//else set_sleep_mode(SLEEP_MODE_IDLE);

	sleep_enable();

	// turn off brown-out enable in software
	MCUCR = _BV(BODS) | _BV(BODSE);
	MCUCR = _BV(BODS);
	interrupts();
	sleep_cpu(); 

	// cancel sleep as a precaution
	sleep_disable();
	PRR = old_PRR;
	ADCSRA = old_ADCSRA;  //return ACD enabled
	
} // end of myWatchdogEnable


void wakelight() {
	disableInterrupt(LightSensorIntPin);
}

void SensorDetect() {
	// Flag to indicate that the sensors have been triggered, 
	// so that it will run the cod. Because I am using interrupts, i had to
	// structure the code so that the function is available but will only run when triggered.
	// If i tried to turn on the leds via the interrupt function, the chip would go back to 
	// sleep because its running the previous code from where it left off, and sleep before the led function finishes.
	sensorActive = true;
}
