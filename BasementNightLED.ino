/* connections:
*  usb:
*  red= blue =  reset = pin 1
*  white =yellow = rx = pin2
*  green = green = tx = pin 2
*  black = gray = gnd = pin gnd
pin 2 : light sensor
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
/// Pin Configureation
///////////////////////////////////////////////
int potPin = A4;		//potentiometer for controlling the brightness of led's which will pwm to the transistor pins
int potValue = 0;		// variable to store the value coming from the potentiometer
int rampOnValue = 0;	//value of pwm to ramp up led's on smoothly
int justOn = 0;			// Which Led was just on? 0 = off, 1 =  tv, 2 = stair, 3 = bath
#define RampSpeedSlow 38  //delay amount to transistionj from off to on
#define RampSpeedMed 16
#define RampSpeedFast 10

int lightSen = A5;  // light sensor
int lightSenValue;  // variable to store the value coming from the sensor

#define PIRbath 4   // Pir for bath 
#define PIRstair 8  // Pir for stairs 
#define PIRtv 9     // TV sensor pin
#define PIRtele 11  // PIR for ruben's room connected via initial telephone port

#define RelayStair 5 //relay pin for stair Led strip
#define RelayBath 6  //relay pin for bathroom Led strip
#define RelaySensor 10 //relay pin to power on the sensors when it is dark. but i think its really a transistor
#define debugled 13
