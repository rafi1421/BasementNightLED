#define LightLimitBright 150 // Put to sleep above this value
#define LightLimitDark 50    // Turn on lights under this value

// Light resistor: Light = Short, Dark = Open
// 150k Ohm Pull Up   : 
// 5V     : Dark [850, 4.3V], Ambient [440, 2.2V], Light [320, 1.6V]
// Battery: Dark [600, 2.9V], Ambient [290, 1.4V], Light [170, 0.8V]
// 150k Ohm Pull Down :
// 5V     : Dark [40, 0.2V], Ambient [500, 2.4V], Light [750, 3.7V]
// Battery: Dark [40, 0.2V], Ambient [270, 1.3V], Light [450, 2.2V]

// sensorActive turns true from PIR interrupt

void CheckAmbientLight() {
	if (!sensorActive) {

		lightSensorValue = analogRead(LightSensorAnalogPin);  // Read the value from the sensor 
		// Make sure leds are off
		digitalWrite(RelayStair, LOW);

		#if DEBUG
		Serial.print("lightsensor value: ");
		Serial.println(lightSensorValue);
		#endif

		/// Bright -> Go to sleep
		if (lightSensorValue >= LightLimitBright) {

			// (Motion Sensor ISR should already be disabled)

			// Enable LS ISR
      attachInterrupt(LightSensorInt, WakeLight, LOW); //detaches interrupt in wakelight()

			#if DEBUG
			Serial.println("going to sleep: bright");
			delay(1000);
			#endif

			// Sleep
			GoToSleep();
			//and it will resume here

		}

		/// Dark
		else {
			#if DEBUG
			Serial.println("going to sleep: dark");
			delay(1000);
			#endif

      // Enable sensor ISR
      EnablePinChangeInt();

      // Sleep
      GoToSleep();
      //and it will resume here

			// Disable Interrupts now that chip is awake
      DisablePinChangeInt();
		}
	}
}


void TurnOnLights() {
	if (sensorActive) {
		lightSensorValue = analogRead(LightSensorAnalogPin);
		if (lightSensorValue < LightLimitDark) {
			#if DEBUG
			Serial.println("light turned on");
			delay(500);
			#endif

			// Fade on the led's
			int fadeLed;
			for (int x = 0; x <73; x++) {
				fadeLed = .015*x*x; // Final value is [80=96]; [73=80], close to old version
				analogWrite4(fadeLed);
				delay(30);
			}

			do {
				#if DEBUG
				Serial.println("inside do while");
				delay(500);
				#endif

				// Sleep 16 seconds
        EnableWatchdog(WDT_8_SEC);
        GoToSleep(SLEEP_MODE_IDLE); 
				EnableWatchdog(WDT_8_SEC);
        GoToSleep(SLEEP_MODE_IDLE); 

			} while (
				// Poll sensors for activity
				digitalRead(PIRstair) == 1);

			do {
				#if DEBUG
				Serial.println("inside do while");
				delay(500);
				#endif

				// Sleep 4 seconds 
        EnableWatchdog(WDT_4_SEC);
        GoToSleep(SLEEP_MODE_IDLE); 

			} while (
				// Poll sensors for activity
				digitalRead(PIRstair) == 1);

			// If no activity detected, user probably arrived at thier destination by now

			// Fade out
			for (int x = 73; x >0; x--) {
				fadeLed = .015*x*x; 
				analogWrite4(fadeLed);
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
