#define DEBUG false    // serial print messages
#define DEBUGled false  // indicate when chip is awake/sleep
#define DEBUGled_loop false  // indicate when chip is awake/sleep
#define DEBUGled_dark false  // indicate when chip is awake/sleep
#define DEBUGled_fade false  // indicate when chip is awake/sleep
#define LightLimitBright 500 // Put to sleep above this value
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
      if (digitalRead(PIRstair) == LOW) {
        //do nothing and wait for pir to get low before becoming an interrupt state
        #if DEBUGled_dark
        digitalWrite(1,1);
        #endif
      
      
      EnablePinChangeInt();
      delay(100);
      #if DEBUGled_dark
      digitalWrite(1,0);
      delay(100);
      #endif

      // Sleep
      GoToSleep(SLEEP_MODE_IDLE);
      //and it will resume here
      
			// Disable Interrupts now that chip is awake
      DisablePinChangeInt();
      sensorActive = true;
      }
      // And should go to next section since sensorActive should now be true
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
      PWM4_init(); //Some reason I needed this here too. maybe configuration gets conflicted
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
        //EnableWatchdog(WDT_8_SEC);
        //GoToSleep(SLEEP_MODE_IDLE); 
				EnableWatchdog(WDT_1_SEC);
        GoToSleep(SLEEP_MODE_IDLE); 

			} while (digitalRead(PIRstair) == 1); // Poll sensors for activity
						
      #if DEBUGled_fade
      digitalWrite(1,1);
      #endif
			do {
				#if DEBUG
				Serial.println("inside do while");
				delay(500);
				#endif
        #if DEBUGled_fade
        digitalWrite(1,0);
        #endif
        
				// Sleep 4 seconds 
        EnableWatchdog(WDT_1_SEC);
        GoToSleep(SLEEP_MODE_IDLE); 

			} while (digitalRead(PIRstair) == 1); // Poll sensors for activity
			 
      #if DEBUGled_fade
      digitalWrite(1,1);
      #endif
      
			// If no activity detected, user probably arrived at their destination by now

			// Fade out
     PWM4_init();
			for (int x = 73; x >0; x--) {
				fadeLed = .015*x*x; 
				analogWrite4(fadeLed);
				delay(10);
			}
      #if DEBUGled_fade
      digitalWrite(1,0);
      #endif
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
