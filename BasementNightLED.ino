// Git Branch for Trinket board to use this lights project code on the 1st to 2nd floor staircase handle
// Board: Adafruit Trinket 8MHz
// Programmer: USBtinyISP

#define DEBUG_serial false		// serial print messages
#define DEBUGled false	// indicate when chip is awake/sleep
#define DEBUGled_loop false  // indicate when chip is awake/sleep
#define DEBUGled_dark false  // indicate when chip is awake/sleep
#define DEBUGled_fade false  // indicate when chip is awake/sleep

/* Uno usb connections:
*  red= blue =  reset = pin 1
*  white =yellow = rx = pin 2
*  green = green = tx = pin 3
*  black = gray = gnd = pin gnd

trinket pins:
pin 0: PIR stair sensor 
pin 1: board led_builtin
pin 2: light sensor 
pin 3: 
pin 4: stair led tip120 transistor

// ATMEL ATTINY 25/45/85 / ARDUINO
//
//                  +-\/-+
// Ain0 (D 5) PB5  1|    |8  Vcc
// Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1
// Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1
//            GND  4|    |5  PB0 (D 0) pwm0
//                  +----+

*/


///////////////////////////////////////////////
/// Configuration
///////////////////////////////////////////////

byte LightSensorInt = 0; //light interrupt physical pin 2 is actually "interupt pin" 0
int LightSensorAnalogPin = 1;	// forgot why 1 is needed instead of 2, probably the internal pin # on trinket
int lightSensorValue;		// variable to store the value coming from the sensor

#define PIRstair 0  // Pir for stairs 
#define RelayStair 4		// pin for stair Led strip
#define debugled LED_BUILTIN

boolean watchdog = false;  //boolean for watchdog(true) or sleep forever (false)
volatile boolean sensorActive = false;

//// Watchdog intervals
//// sleep bit patterns for WDTCSR/
enum {
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
};  // end of WDT intervals enum


///////////////////////////////////////////////
/// Setup
///////////////////////////////////////////////


void setup() {
	//delay(5000);
  #if DEBUG_serial
	Serial.begin(115200);
  #endif // DEBUG
  //#if DEBUGled
	pinMode(debugled, OUTPUT);
	//digitalWrite(debugled, HIGH);
  delay(1000);
  //#endif

  
	// Setup input sensors
	// The PIR sensor's output signal is an open-collector, 
  // so a pull-up resistor is required:
	pinMode(PIRstair, INPUT_PULLUP);
	// Setup relay pins LOW for off at start of program.
	pinMode(RelayStair, OUTPUT);
	digitalWrite(RelayStair, LOW);
  PWM4_init(); // Needs to be run after pinMode assigned.
}



void loop() {
	#if DEBUGled_loop
	digitalWrite(debugled, 1);

  //PWM4_init();
  //analogWrite4(100);
  delay(100);
  digitalWrite(debugled, 0);
  delay(100);
  #endif
  //sensorActive = true;
	CheckAmbientLight();
	TurnOnLights();
}

void PWM4_init() {
// Set up PWM on Trinket GPIO #4 (PB4, pin 3) using Timer 1
TCCR1 = _BV (CS10); // no prescaler
GTCCR = _BV (COM1B1) | _BV (PWM1B); // clear OC1B on compare
OCR1B = 0; //OCR1B = 127; // duty cycle initialize to 50%
OCR1C = 255; // frequency
}
 
// Function to allow analogWrite on Trinket GPIO #4
void analogWrite4(uint8_t duty_value) {
OCR1B = duty_value; // duty may be 0 to 255 (0 to 100%)
}
