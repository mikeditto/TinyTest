/*
 * TinyTest - Handy Heater simulator
 *
 * Blink the LEDs, respond to the buttons, spin the fan.
 */


#define nel(array) (sizeof (array) / sizeof *(array))


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


void setup()
{
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
    digitalWrite(PIN_FAN, false);
    digitalWrite(PIN_LED_RED, false);
    digitalWrite(PIN_LED_GRN, false);
    digitalWrite(PIN_LED_BLU, false);
}

unsigned char heaterstate = 0;  // 0=off, 1=400W, 2=800W, 3=1200W
unsigned short ledstates[3] = { 0, 5000, 10000 };
const unsigned short ledincrements[3] = { 5, 11, 17 };
bool powerbuttonstate = false;
bool direction = true;          // true=forward
unsigned short ticks = 0;
unsigned char pwmcount;
static const unsigned char pwmpins[] = {
    PIN_FAN, PIN_LED_RED, PIN_LED_GRN, PIN_LED_BLU
};
static unsigned char pwmvalues[nel(pwmpins)];


void loop()
{
    /* Run this loop about 1000 times per second. */
    delay(1);

    /*
     * Poor man's PWM - 16 cycles, about 60 Hz
     * Values can be 0 (fully off), 1-15 (partial), 16 or higher (fully on).
     */
    if (++pwmcount >= 16)
	pwmcount = 0;
    for (int i=0; i<nel(pwmpins); ++i)
	digitalWrite(pwmpins[i], pwmcount < pwmvalues[i]);

    /*
     * Throb the LED colors at different rates.
     * Each state is a 16-bit value, using bits [11:14] as the cycling
     * intensity and bit [15] as a sign bit so that it ramps up 0..15 and then
     * down 15..0.
     */
    for (int i=0; i<nel(ledstates); ++i) {
	ledstates[i] += ledincrements[i];
	unsigned char val = ledstates[i] >> 11;
	if (val & 16)
	    val = 16 - (val & 15);
	else
	    val &= 15;
	pwmvalues[i+1] = val;
    }

    bool newstate = digitalRead(PIN_BUT_POWER);  // active low
    if (powerbuttonstate != newstate) {
	powerbuttonstate = newstate;
	/* When POWER is pressed, toggle the direction. */
	if (!powerbuttonstate)
	    direction = !direction;
    }

    unsigned short period;
    if (digitalRead(PIN_BUT_COLOR))	// active low
	period = 3000;			// not pressed
    else
	period = 100;			// pressed
    if (++ticks < period)
	return;
    ticks = 0;

    /*
     * Above portion gets run every tick (1 ms);
     * below portion runs every period (3 seconds or 100 ms).
     */

    if (direction) {
	if (++heaterstate > 3)
	    heaterstate = 0;
    } else {
	if (--heaterstate > 3) // wrapped around
	    heaterstate = 3;
    }

    /* Set heater power - off, 400 W, 800 W, or 1200 W. */
    digitalWrite(PIN_HEAT_800W, heaterstate & 2);
    digitalWrite(PIN_HEAT_400W, heaterstate & 1);

    /* Set fan speed - off, 6/16, 12/16, or max. */
    pwmvalues[0] = heaterstate * 6;
}
