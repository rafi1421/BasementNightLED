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
			analogWrite(RelayStair, rampOnValue);
			analogWrite(RelayBath, rampOnValue);

			do {
				#if DEBUG
				Serial.println("inside do while");
				delay(500);
				#endif

				// Sleep 8 seconds 

				//Dont think im able to use watchdog for sleep while running PWM
				//watchdog = true;
				delay(1000);

				//goSleep(WDT_8_SEC);
				//watchdog = false;

			} while (
				// Poll sensors for activity
				//digitalRead(PIRbath) ||
				//digitalRead(PIRtele) ||
				//digitalRead(PIRtv)   ||
				digitalRead(PIRstair) == 1);

			// If no activity detected, user probably arrived at thier destination by now

			delay(1000); // Keep lights on for another second just incase user is still settling down

			// Fade out


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
