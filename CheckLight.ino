#include <EnableInterrupt.h>
//#include <AHeader.h>
#define LightLimitBright 600
#define LightLimitDark 500
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




void CheckAmbientLight() {
	if (!sensorActive) {

		lightSensorValue = analogRead(LightSensorPin);  // read the value from the sensor 
		digitalWrite(RelayBath, LOW);  //make sure leds are off
		digitalWrite(RelayStair, LOW);

		#if DEBUG
		Serial.print("lightsensor value: ");
		Serial.println(lightSensorValue);
		#endif

		/// Bright
		if (lightSensorValue >= LightLimitBright) {

			// (Sensor ISR should already be disabled)

			// Enable LS ISR
			digitalWrite(RelaySensor, LOW);  //turns off sensors (save 7-15mA per sensor)

			enableInterrupt(LightSensorIntPin, wakelight, LOW);

			#if DEBUG
			Serial.println("going to sleep: bright");
			delay(1000);
			#endif

			// Sleep
			goSleep(WDT_SLeeP);

			//and theoretical it will also resume here

		}

		/// Dark
		else {

			//Enable Sensor power
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

			//Sleep
			goSleep(WDT_SLeeP);
			
			// Disable Interrupts so they dont duplicate running this code

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
				// Sleep 8 seconds 
				#if DEBUG
				Serial.println("inside do while");
				delay(500);
				#endif
				//dont think im ablew to use watchdog for sleep while running PWM
				watchdog = true;
				delay(1000);

				//goSleep(WDT_8_SEC);
				watchdog = false;

			} while (
				// Poll sensors for activity
				//digitalRead(PIRbath) ||
				//digitalRead(PIRtele) ||
				//digitalRead(PIRtv)   ||
				digitalRead(PIRstair) == 1);

			// If no activity detected, user probably arrived at thier destination by now

			#if DEBUG
			Serial.println("exit");
			delay(500);
			#endif
		}
	}
	sensorActive = false; 
	//Put this here, because if its not here, then when the sensor is triggered after sleeping during the bright,
	// the code still gets tue for sensor active interrupt, and then itll be stuck infinite loop and never go to sleep.
}
