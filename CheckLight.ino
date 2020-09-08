#define LightLimitBright 470 // Put to sleep above this value
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
    delay(1000);
		/// Bright -> Go to sleep
		if (lightSensorValue > LightLimitBright) {

			// (Motion Sensor ISR should already be disabled)

			
      
      
			#if DEBUG_serial
			Serial.println("going to sleep: bright");
			delay(1000);
			#endif
      #if DEBUGled_dark
      digitalWrite(debugled, HIGH);
      delay(100);
      digitalWrite(debugled, LOW);
      delay(200);
      digitalWrite(debugled, HIGH);
      delay(100);
      digitalWrite(debugled, LOW);
      #endif
      //if (digitalRead(2) == HIGH) {
      // Enable LS ISR
      //EnablePin2ChangeInt();
			// Sleep
      attachInterrupt(LightSensorInt, WakeLight, LOW); //detaches interrupt in wakelight()
			GoToSleep();
			//and it will resume here
      //DisablePin2ChangeInt();
      //}
      delay(1000);
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
        EnablePinChangeInt();
        #if DEBUGled_dark
        digitalWrite(debugled, HIGH);
        delay(100);
        digitalWrite(debugled, LOW);
        delay(200);
        digitalWrite(debugled, HIGH);
        delay(100);
        digitalWrite(debugled, LOW);
        delay(200);
        digitalWrite(debugled, HIGH);
        delay(100);
        digitalWrite(debugled, LOW);
        #endif
        
      delay(100);
      // Sleep
      GoToSleep(SLEEP_MODE_IDLE); //should it be idle? nahh
      //and it will resume here
      
			// Disable Interrupts now that chip is awake
      DisablePinChangeInt();
      sensorActive = true;
      }
      // And should go to next section since sensorActive should now be true
		}
	}
}

// 150k Ohm Pull Down : using values generated from trinket, not nano
// USB     : Dark [40, 0.2V], Ambient [500, 2.4V], Light [750, 3.7V]
// Battery: Dark [40, 0.2V], Ambient [270, 1.3V], Light [450, 2.2V]
// testing with usb active during >2, but not 50, even with a flashlight
//  acive at >10
void TurnOnLights() {
	if (sensorActive) {
		lightSensorValue = analogRead(LightSensorAnalogPin);
		//if (lightSensorValue < LightLimitDark) {
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
		}
	//}
	sensorActive = false; 
	// Put this here, because if its not here, then when the sensor is triggered after sleeping during the bright,
	// the code still gets tue for sensor active interrupt, and then itll be stuck infinite loop and never go to sleep.
}
