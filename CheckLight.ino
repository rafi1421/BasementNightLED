#include <EnableInterrupt.h>
//#include <AHeader.h>
#define lightLD 600
#define lightDL 600
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





void TurnOnLights() {

	// Disable Interrupts so they dont duplicate running this code

	disableInterrupt(PIRbath);
	disableInterrupt(PIRstair);
	//disableInterrupt(PIRtele);
	//disableInterrupt(PIRtv);

#if DEBUG
	Serial.println("light turned on");
	delay(500);
#endif

	// Fade on the led's
	analogWrite(RelayStair, rampOnValue);
	analogWrite(RelayBath, rampOnValue);

	do {
		// Sleep 8 seconds 
#if DEBUG
		Serial.println("inside do while");
		delay(500);
#endif
		watchdog = true;
		delay(5000);

		//goSleep(WDT_8_SEC);
		watchdog = false;

		// Poll sensors for activity

	} while (digitalRead(PIRbath) ||
		digitalRead(PIRstair) == 1);

	// If no activity detected, user probably arrived at thier destination by now


#if DEBUG
	Serial.println("exit");
	delay(500);
#endif

	// Check brightness
	CheckAmbientLight();
}





void CheckAmbientLight() {
	lightSensorValue = analogRead(LightSensorPin);  // read the value from the sensor 
	digitalWrite(RelayBath, LOW);  //make sure leds are off
	digitalWrite(RelayStair, LOW);
#if DEBUG
	Serial.print("lightsensor value: ");
	Serial.println(lightSensorValue);
#endif
	// Bright
	if (lightSensorValue >= lightLD) {
		
		// (Sensor ISR should already be disabled)

		// Enable LS ISR

		digitalWrite(RelaySensor, LOW);  //turns off sensors (save 7-15mA per sensor)

		enableInterrupt(LightSensorIntPin, wakelight, LOW);

#if DEBUG
		Serial.println("going to sleep: bright");
		delay(500);
#endif

		// Sleep
		goSleep(WDT_SLeeP); 

		//and theoretical it will also resume here
	}

	/// Dark
	/// mayber better if only if? and then might not need the medium part, because then itll just wait until someone crosses the sensor, 
	/// light will turn on and then itll check the brightness again, and by that time, sundown should have happened by then

	if (lightSensorValue < lightLD) {

		// Disable LS ISR. But I probably dont need to disasable the LS, cause it should disable when it wakes up. yeaaa

		// Enable sensor ISR
		//enableInterrupt(PIRtv, TurnOnLights, RISING);
		enableInterrupt(PIRstair, TurnOnLights, CHANGE);
		enableInterrupt(PIRbath, TurnOnLights, CHANGE);
		//enableInterrupt(PIRtele, TurnOnLights, RISING);

#if DEBUG
		Serial.println("going to sleep: dark");
		delay(1500);
#endif

		//Sleep
		//goSleep(WDT_SLeeP);
	}


	// Medium
}


