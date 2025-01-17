/*
 * TinyTest - Handy Heater simulator
 *
 * Blink the LEDs, respond to the buttons, spin the fan.
 */


/* "Handy Heater" pins for ATtiny1614 */
#define PIN_HEAT_800W PIN_PA4
#define PIN_HEAT_400W PIN_PA5
#define PIN_UNUSED0   PIN_PA6
#define PIN_FAN       PIN_PA7
#define PIN_LED_BLU   PIN_PB3
#define PIN_LED_RED   PIN_PB2
#define PIN_LED_GRN   PIN_PB1
#define PIN_BUT_POWER PIN_PB0
#define PIN_BUT_COLOR PIN_PA0
#define PIN_UNUSED1   PIN_PA1
#define PIN_UNUSED2   PIN_PA2
#define PIN_POT_DNP   PIN_PA3


void setup() {
  // Initialize control pins as outputs.
  pinMode(PIN_HEAT_800W, OUTPUT);
  pinMode(PIN_HEAT_400W, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GRN, OUTPUT);
  pinMode(PIN_LED_BLU, OUTPUT);
  // Button pins are inputs.
  pinMode(PIN_BUT_COLOR, INPUT_PULLUP);
  pinMode(PIN_BUT_POWER, INPUT_PULLUP);

  // Initial pin states
  digitalWrite(PIN_HEAT_800W, false);
  digitalWrite(PIN_HEAT_400W, false);
  analogWrite(PIN_FAN, 0);
  digitalWrite(PIN_LED_RED, false);
  digitalWrite(PIN_LED_GRN, false);
  digitalWrite(PIN_LED_BLU, false);
}

unsigned char heaterstate = 0;  // 0=off, 1=400W, 2=800W, 3=1200W
unsigned char ledsstate = 0;    // bit 0=red, bit 1=green, bit 2=blue
unsigned char fanstate = 0;     // 0-255
bool colorbuttonstate = false;
bool powerbuttonstate = false;
bool direction = true;          // true=forward

void loop() {
  if (direction) {
    if (++heaterstate > 3)
      heaterstate = 0;
  } else {
     if (--heaterstate > 3) // wrapped around
        heaterstate = 3;
  }
  digitalWrite(PIN_HEAT_800W, heaterstate & 2);
  digitalWrite(PIN_HEAT_400W, heaterstate & 1);

  if (direction) {
    ledsstate <<= 1;
    if (!(ledsstate & 7))
      ledsstate = 1;
  } else {
    ledsstate >>= 1;
    if (!(ledsstate & 7))
      ledsstate = 4;
  }
  digitalWrite(PIN_LED_RED, ledsstate & (1<<0));
  digitalWrite(PIN_LED_GRN, ledsstate & (1<<1));
  digitalWrite(PIN_LED_BLU, ledsstate & (1<<2));

  fanstate += 10;
  if (fanstate < 80)
    analogWrite(PIN_FAN, 0);
  else if (fanstate < 16)
    analogWrite(PIN_FAN, 120);
  else
    analogWrite(PIN_FAN, 255);

  bool newstate = digitalRead(PIN_BUT_POWER);
  if (powerbuttonstate != newstate) {
    powerbuttonstate = newstate;
    if (!powerbuttonstate)
      direction = !direction;
  }

  if (digitalRead(PIN_BUT_COLOR))
    delay(1000);                       // wait for a second
  else
    delay(100);
}
