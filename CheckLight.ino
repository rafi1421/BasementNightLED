#include <EnableInterrupt.h>
#include <LowPower.h>
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
			//goSleep();
			LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
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
			
			LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

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
				LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_ON, TIMER1_OFF, TIMER0_OFF,
					SPI_OFF, USART0_OFF, TWI_OFF);

			} while (
				// Poll sensors for activity
				//digitalRead(PIRbath) ||
				//digitalRead(PIRtele) ||
				//digitalRead(PIRtv)   ||
				digitalRead(PIRstair) == 1);

			// If no activity detected, user probably arrived at thier destination by now

			// Keep lights on for another second just incase user is still settling down

			LowPower.idle(SLEEP_1S, ADC_OFF, TIMER2_ON, TIMER1_OFF, TIMER0_OFF,
				SPI_OFF, USART0_OFF, TWI_OFF);

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


void wakelight() {
	disableInterrupt(LightSensorIntPin);
}

void SensorDetect() {
	// Flag to indicate that the sensors have been triggered, 
	// so that it will run the cod. Because I am using interrupts, i had to
	// structure the code so that the function is available but will only run when triggered.
	// If i tried to turn on the leds via the interrupt function, the chip would go back to 
	// sleep because its running the previous code from where it left off, and sleep before the led function finishes.
	sensorActive = true;
}

