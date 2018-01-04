#include <EnableInterrupt.h>
#define lightLD 400
#define lightDL 600

void CheckAmbientLight() {
	lightSensorValue = analogRead(LightSensorPin);  // read the value from the sensor 

	// Bright
	if (lightSensorValue >= lightLD) {
		
		// (Sensor ISR should already be disabled)

		// Enable LS ISR
		digitalWrite(RelayBath, LOW);  //make sure leds are off
		digitalWrite(RelayStair, LOW);
		digitalWrite(RelaySensor, LOW);  //turns off sensors (save 7-15mA per sensor)

		attachInterrupt(LightSensorIntPin, wakelight, LOW);  // Enable hardware external interrupt pin

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
		enableInterrupt(PIRtv, TurnOnLights, RISING);
		enableInterrupt(PIRstair, TurnOnLights, RISING);
		enableInterrupt(PIRbath, TurnOnLights, RISING);
		enableInterrupt(PIRtele, TurnOnLights, RISING);

		//Sleep
		goSleep(WDT_SLeeP);
	}


	// Medium
}


void TurnOnLights() {

	// Disable Interrupts so they dont duplicate running this code

	disableInterrupt(PIRbath);
	disableInterrupt(PIRstair);
	disableInterrupt(PIRtele);
	disableInterrupt(PIRtv);

	// Fade on the led's
	analogWrite(RelayStair, rampOnValue);
	analogWrite(RelayBath, rampOnValue);

	do {
		// Sleep 8 seconds 
		watchdog = true;
		goSleep(WDT_8_SEC);
		watchdog = false;
	} while (
		// Poll sensors for activity
		analogRead(PIRtele) ||
		analogRead(PIRtv) ||
		analogRead(PIRbath) ||
		analogRead(PIRstair) == 1);
	// If no activity detected, user probably arrived at thier destination by now


	// Check brightness
	CheckAmbientLight();
}