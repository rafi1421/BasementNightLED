
#define DEBUG false		// serial print messages
#define DEBUGled true	// indicate when chip is awake/sleep
//#include <AHeader.h>

/* connections:
*  usb:
*  red= blue =  reset = pin 1
*  white =yellow = rx = pin2
*  green = green = tx = pin 2
*  black = gray = gnd = pin gnd
pin 2 : light sensor ( i think its also connected to pin A4)
pin 3: manual button
pin 4: bathroom PIR sentton inturrupt - normally lsor to purple connector pin 4 (replace ground as is currently.
see how the sensor reacts for the first minute on the sensor pin)
pin 5: bath relay/led / now a tip120 transistor
pin 6: stair relay/led  / now a tip120 transistor
pin 7: bath sesnor [not anymore]
pin 8: stair sensor (replace with PIR)
pin 9: tv sensor
pin 10: NPN 3904 / PNP 3906 transistor for sensor switch
pin 11: piezo/led
pin 12: optional program debug switch to upload //althoughi never needed it?
pin 13: debug led
*/


///////////////////////////////////////////////
/// Pin Configuration
///////////////////////////////////////////////

int PotPin = A4;		//potentiometer for controlling the brightness of led's which will pwm to the transistor pins
int potValue = 0;		// variable to store the value coming from the potentiometer
int rampOnValue = 40;	//value of pwm to ramp up led's on smoothly
int justOn = 0;			// Which Led was just on? 0 = off, 1 =  tv, 2 = stair, 3 = bath
#define RampSpeedSlow 38  //delay amount to transistion from off to on
#define RampSpeedMed 16
#define RampSpeedFast 10

byte LightSensorIntPin = 2; //light interrupt pin 2 
int LightSensorPin = A5;  // light sensor
int lightSensorValue;  // variable to store the value coming from the sensor

#define PIRbath 4   // Pir for bath 
#define PIRstair 8  // Pir for stairs 
#define PIRtv 9     // TV sensor pin
#define PIRtele 11  // PIR for ruben's room connected via initial telephone port

#define RelayStair 5 //relay pin for stair Led strip
#define RelayBath 6  //relay pin for bathroom Led strip
#define RelaySensor 10 //relay pin to power on the sensors when it is dark. but i think its really a transistor
#define debugled 13
boolean watchdog = false;  //boolean for watchdog(true) or sleep forever (false)
volatile boolean sensorActive = false;




///////////////////////////////////////////////
/// Setup
///////////////////////////////////////////////


void setup() {
	delay(1000);
#if DEBUG
	Serial.begin(115200);
#endif // DEBUG
#if DEBUGled
	pinMode(debugled, OUTPUT);
	digitalWrite(debugled, LOW);
#endif

	// Setup input sensors
	pinMode(PIRbath, INPUT_PULLUP);
	pinMode(PIRstair, INPUT_PULLUP);
	pinMode(PIRtv, INPUT_PULLUP);
	pinMode(PIRtele, INPUT_PULLUP);
	// Setup relay pins LOW for off at start of program.
	pinMode(RelayBath, OUTPUT);
	pinMode(RelayStair, OUTPUT);
	pinMode(RelaySensor, OUTPUT);
	digitalWrite(RelayBath, LOW);
	digitalWrite(RelayStair, LOW);
	digitalWrite(RelaySensor, LOW);

	//CheckAmbientLight();

}


void loop() {
	#if DEBUGled
	digitalWrite(debugled, 1);
	#endif

	CheckAmbientLight();
	TurnOnLights();
}
