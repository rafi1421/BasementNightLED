#define LightLimitBright 550 // Put to sleep above this value
#define LightLimitDark 500   // Turn on lights under this value



/*
 * // 150k Ohm Pull Down : using values generated from trinket
       if (voltage > 500) analogWrite4(200);  Lamp @ 32%  1475 mV
  else if (voltage > 460) analogWrite4(100);  Lamp @ 31%  1390 mV
  else if (voltage > 430) analogWrite4(50);   Lamp @ 25%  1250 mV
  else if (voltage > 400) analogWrite4(20);   Lamp @ 25%  1250 mV
  else if (voltage > 0) analogWrite4(5);      Lamp @ %   mV

       if (voltage > 650) analogWrite4(200);  Lamp @ 46%  1800 mV
  else if (voltage > 600) analogWrite4(100);  Lamp @ 40%  1670 mV
  else if (voltage > 550) analogWrite4(50);   Lamp @ 37%  1570 mV
  else if (voltage > 530) analogWrite4(20);   Lamp @ 35%  1500 mV
  else if (voltage > 515) analogWrite4(20);   Lamp @ 34%  1450 mV
  else if (voltage > 500) analogWrite4(20);   Lamp @ 33%  1425 mV
  else if (voltage > 0) analogWrite4(5);      Lamp @ %   mV

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
      
      // Enable LS ISR and Sleep
      attachInterrupt(LightSensorInt, WakeLight, LOW); //detaches interrupt in wakelight()
			GoToSleep();
			//and it will resume here
      //delay(1000);
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
        
      //delay(100);
      // Sleep
      GoToSleep(); //should it be idle? nahh
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
        #if DEBUGled_fade
        digitalWrite(debugled,HIGH);
        delay(200);
        digitalWrite(debugled,LOW);
        #endif
        
				// Sleep 8 seconds 
        EnableWatchdog(WDT_8_SEC);
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
	}
	sensorActive = false; 
	// Put this here, because if its not here, then when the sensor is triggered after sleeping during the bright,
	// the code still gets tue for sensor active interrupt, and then itll be stuck infinite loop and never go to sleep.
}
