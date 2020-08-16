#define LightLimitBright 40 // Put to sleep above this value
#define LightLimitDark 20    // Turn on lights under this value


// Analog read value  :: mV :: Basement light ~3-4pm, 3.01V battery
/*
00 :: 065mV :: pinch right over sensor
10 :: 135mV :: 1 inch hover
20 :: 250mV :: 3 inch hover
30 :: 325mV :: hand shadow, 5 inch away
40 :: 400mV :: ambient in 3pm basement light
50 :: 580mV :: lamp at 36%
60 :: 700mV :: lamp at 55%
70 :: 800mV :: lamp at 67%
80> :: >900mV :: lamp full brightness 
*/
// sensorActive turns true from PIR interrupt

void CheckAmbientLight() {
	if (!sensorActive) {

		lightSensorValue = analogRead(LightSensorAnalogPin);  // Read the value from the sensor 
		// Make sure leds are off
		digitalWrite(RelayStair, LOW);

		#if DEBUG_serial
		Serial.print("lightsensor value: ");
		Serial.println(lightSensorValue);
		#endif

		/// Bright -> Go to sleep
		if (lightSensorValue >= LightLimitBright) {

			// (Motion Sensor ISR should already be disabled)

			// Enable LS ISR
      attachInterrupt(LightSensorInt, WakeLight, LOW); //detaches interrupt in wakelight()

			#if DEBUG_serial
			Serial.println("going to sleep: bright");
			delay(1000);
			#endif

			// Sleep
			GoToSleep();
			//and it will resume here

		}

		/// Dark
		else {
			#if DEBUG_serial
			Serial.println("going to sleep: dark");
			delay(1000);
			#endif

      // Enable sensor ISR
      if (digitalRead(PIRstair) == LOW) {
        //do nothing and wait for pir to get low before becoming an interrupt state
        #if DEBUGled_dark
        digitalWrite(debugled,HIGH);
        #endif
      
      
      EnablePinChangeInt();
      delay(100);
      #if DEBUGled_dark
      digitalWrite(debugled, LOW);
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
	//if (sensorActive) {
  //if (DEBUGled_fade == true) {
		lightSensorValue = analogRead(LightSensorAnalogPin);
		//if (lightSensorValue < LightLimitDark) {
    //if (DEBUGled_fade == true) {
			#if DEBUG_serial
			Serial.println("light turned on");
			delay(500);
			#endif
      #if DEBUGled_fade
      digitalWrite(debugled,HIGH);
      delay(1000);
      digitalWrite(debugled,LOW);
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
				#if DEBUG_serial
				Serial.println("inside do while");
				delay(500);
				#endif

				// Sleep 16 seconds
        EnableWatchdog(WDT_4_SEC);
        GoToSleep(SLEEP_MODE_IDLE); 
				//delay(5000);//EnableWatchdog(WDT_1_SEC);
        //GoToSleep(SLEEP_MODE_IDLE); 

			} while (digitalRead(PIRstair) == 1); // Poll sensors for activity; repeats do() if there is activity
						

			do {
				#if DEBUG_serial
				Serial.println("inside do while");
				delay(500);
				#endif
        #if DEBUGled_fade
        digitalWrite(debugled,HIGH);
        delay(200);
        digitalWrite(debugled,LOW);
        #endif
        
				// Sleep 4 seconds 
        EnableWatchdog(WDT_1_SEC);
        GoToSleep(SLEEP_MODE_IDLE); 

			} while (digitalRead(PIRstair) == 1); // Poll sensors for activity
			 
      #if DEBUGled_fade
      digitalWrite(debugled,HIGH); //light on while fading out
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
      digitalWrite(debugled,LOW);
      #endif
			#if DEBUG_serial
			Serial.println("exit");
			delay(500);
			#endif
		//}
	//}
	sensorActive = false; 
	// Put this here, because if its not here, then when the sensor is triggered after sleeping during the bright,
	// the code still gets tue for sensor active interrupt, and then itll be stuck infinite loop and never go to sleep.
}
