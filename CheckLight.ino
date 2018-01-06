#include <EnableInterrupt.h>
//#include "AHeader.h"
#define LightLimitBright 600
#define LightLimitDark 500


void CheckAmbientLight() {
	if (!sensorActive) {

		lightSensorValue = analogRead(LightSensorPin);  // Read the value from the sensor 
		digitalWrite(RelayBath, LOW);  // Make sure leds are off
		digitalWrite(RelayStair, LOW);

		#if DEBUG
		Serial.print("lightsensor value: ");
		Serial.println(lightSensorValue);
		#endif

		/// Bright
		if (lightSensorValue >= LightLimitBright) {

			// (Motion Sensor ISR should already be disabled)

			digitalWrite(RelaySensor, LOW);  // Turns off sensors power (save 7-15mA per sensor) during daytime

			// Enable LS ISR
			enableInterrupt(LightSensorIntPin, wakelight, LOW);

			#if DEBUG
			Serial.println("going to sleep: bright");
			delay(1000);
			#endif

			// Sleep
			goSleep();

			//and theoretical it will also resume here

		}

		/// Dark
		else {

			// Enable Sensor power
			//digitalWrite(RelaySensor, 1);

			// Enable sensor ISR
			enableInterrupt(PIRtv, SensorDetect, RISING);
			enableInterrupt(PIRstair, SensorDetect, RISING);
			enableInterrupt(PIRbath, SensorDetect, RISING);
			enableInterrupt(PIRtele, SensorDetect, RISING);

			#if DEBUG
			Serial.println("going to sleep: dark");
			delay(1000);
			#endif

			// Sleep
			goSleep();
			
			// Disable Interrupts now that chip is awake

			disableInterrupt(PIRbath);
			disableInterrupt(PIRstair);
			disableInterrupt(PIRtele);
			disableInterrupt(PIRtv);
			
		}
	}
	
}


void TurnOnLights() {

	if (sensorActive) {
		lightSensorValue = analogRead(LightSensorPin);
		if (lightSensorValue < LightLimitDark) {

			#if DEBUG
			Serial.println("light turned on");
			delay(500);
			#endif

			// Fade on the led's
			int fadeLed;
			for (int x = 0; x <80; x++) {
				fadeLed = .015*x*x; // Final value is 96, close to old version
				analogWrite(RelayBath, fadeLed);
				analogWrite(RelayStair, fadeLed);
				delay(30);
			}

			do {
				#if DEBUG
				Serial.println("inside do while");
				delay(500);
				#endif

				// Sleep 8 seconds 
				delay(8000);

				//Dont think im able to use watchdog for sleep while running PWM. Actualy maybe yes, will read later
				//watchdog = true;
				//goSleep(WDT_8_SEC);
				//watchdog = false;

			} while (
				// Poll sensors for activity
				digitalRead(PIRbath) ||
				digitalRead(PIRtele) ||
				digitalRead(PIRtv)   ||
				digitalRead(PIRstair) == 1);

			// If no activity detected, user probably arrived at thier destination by now

			delay(1000); // Keep lights on for another second just incase user is still settling down

			// Fade out
			for (int x = 80; x >0; x--) {
				fadeLed = .015*x*x; 
				analogWrite(RelayBath, fadeLed);
				analogWrite(RelayStair, fadeLed);
				delay(10);
			}

			#if DEBUG
			Serial.println("exit");
			delay(500);
			#endif
		}
	}
	sensorActive = false; 
	// Put this here, because if its not here, then when the sensor is triggered after sleeping during the bright,
	// the code still gets tue for sensor active interrupt, and then itll be stuck infinite loop and never go to sleep.
}
