// Includes code from:
//
// Pride2015
// Animated, ever-changing rainbows.
// by Mark Kriegsman
//
// "Pacifica"
// Gentle, blue-green ocean waves.
// December 2019, Mark Kriegsman and Mary Corey March.
// For Dan.

#include "FastLED.h"
#include "ClickRotary.h"
#include "Timer.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    5
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    15

// Samples to average to read the pwm backlight level
#define N_SAMPLES 40

// Period of the polling timer for the volume encoder
#define T_POLLING 5

// Period of the light effects refresh
#define T_EFFECT  20

// Thresholds
// Typical reading for backlight (sum of 40 samples):
// low:  0
// mid:  36900
// high: 40730
#define BACKLIGHT_LOW  18450
#define BACKLIGHT_MID  38815
// Typical reading for scroll lock (1 reading is enough, no pwm here)
// off:  1023
// on:   619
#define SCROLLLOCK_OFF 821

CRGB leds[NUM_LEDS];
ClickRotary *volumeEncoder;
Timer *effectsTimer;
Timer *pollingTimer;

enum Brightness
{
	LIGHT_OFF  = 0,
	LIGHT_MID  = 128,
	LIGHT_HIGH = 255
};

uint16_t lightLevels[N_SAMPLES];
uint8_t  lightSampleIndex;
Brightness brightness;

uint16_t scrollLockLevel;
bool scrollLockOn;

enum LedAnimation
{
	UNUSED = 0,
	PACIFICA,
	PRIDE,
	STATIC_RED,
	STATIC_ORANGE,
	STATIC_YELLOW,
	STATIC_GREEN,
	STATIC_CYAN,
	STATIC_BLUE,
	STATIC_PURPLE,
	STATIC_WHITE,
	N_EFFECTS
};

LedAnimation animation;

void setup() {
	delay(3000); // 3 second delay for recovery

	Serial.begin(115200);

	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(A6, INPUT); // scroll lock cathode
	pinMode(A7, INPUT); // backlight pwm -.-

	effectsTimer = new Timer(T_EFFECT);
	pollingTimer = new Timer(T_POLLING);

	volumeEncoder = new ClickRotary(3, 4, 10, true);

	memset(lightLevels, 0, sizeof(lightLevels));
	lightSampleIndex = 0;

	scrollLockLevel = 0;

	// tell FastLED about the LED strip configuration
	FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
		.setCorrection(TypicalLEDStrip)
		.setDither(true);

	// set master brightness control
	FastLED.setBrightness(0);

	animation = PACIFICA;
}

void loop()
{
	if (effectsTimer->expired())
	{
		uint16_t lightLevel = 0;
		for (int i = 0; i < N_SAMPLES; ++i)
			lightLevel += lightLevels[i];

		brightness = LIGHT_MID;
		if (lightLevel < BACKLIGHT_LOW)
			brightness = LIGHT_OFF;
		else if (lightLevel > BACKLIGHT_MID)
			brightness = LIGHT_HIGH;

		scrollLockOn = (scrollLockLevel < SCROLLLOCK_OFF); // LED is common anode, so the adc will read max value when it is off

		// set master brightness control
		FastLED.setBrightness(brightness);

		// Cycle animations
		if (scrollLockOn)
		{
			animation = (LedAnimation)(animation + volumeEncoder->getDelta());
			if (animation <= UNUSED)
				animation = N_EFFECTS - 1;
			if (animation >= N_EFFECTS)
				animation = UNUSED + 1;
		}
		else
		{
			// ignore the value but avoid switching effect while scroll lock is off
			volumeEncoder->getDelta();
		}
		// Show effect
		switch (animation)
		{
		case PACIFICA:
			pacifica_loop();
			break;
		case PRIDE:
			pride();
			break;
		case STATIC_RED:
		case STATIC_ORANGE:
		case STATIC_YELLOW:
		case STATIC_GREEN:
		case STATIC_CYAN:
		case STATIC_BLUE:
		case STATIC_PURPLE:
		case STATIC_WHITE:
			staticColor(animation);
			break;
		}
		FastLED.show();
	}


	//Serial.println(scrLock);
	if (pollingTimer->expired())
	{
		scrollLockLevel =  analogRead(A6);

		lightLevels[lightSampleIndex] = analogRead(A7);
		++lightSampleIndex;
		if (lightSampleIndex >= N_SAMPLES)
			lightSampleIndex = 0;

		volumeEncoder->checkPins();
		int volume = volumeEncoder->getValue();

	}
}

void staticColor(LedAnimation animation)
{
	switch (animation) {
	case STATIC_RED:
		fill_solid(leds, NUM_LEDS, CRGB(255, 0, 0));
		break;
	case STATIC_ORANGE:
		fill_solid(leds, NUM_LEDS, CRGB(255, 128, 0));
		break;
	case STATIC_YELLOW:
		fill_solid(leds, NUM_LEDS, CRGB(255, 255, 0));
		break;
	case STATIC_GREEN:
		fill_solid(leds, NUM_LEDS, CRGB(0, 255, 0));
		break;
	case STATIC_CYAN:
		fill_solid(leds, NUM_LEDS, CRGB(0, 255, 255));
		break;
	case STATIC_BLUE:
		fill_solid(leds, NUM_LEDS, CRGB(0, 0, 255));
		break;
	case STATIC_PURPLE:
		fill_solid(leds, NUM_LEDS, CRGB(255, 0, 255));
		break;
	case STATIC_WHITE:
		fill_solid(leds, NUM_LEDS, CRGB(255, 255, 255));
		break;
	}
}

// ---- Pride code starts here ----

// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride() 
{
	static uint16_t sPseudotime = 0;
	static uint16_t sLastMillis = 0;
	static uint16_t sHue16 = 0;

	uint8_t sat8 = beatsin88( 87, 220, 250);
	uint8_t brightdepth = beatsin88( 341, 96, 224);
	uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
	uint8_t msmultiplier = beatsin88(147, 23, 60);

	uint16_t hue16 = sHue16;//gHue * 256;
	uint16_t hueinc16 = beatsin88(113, 1, 3000);

	uint16_t ms = millis();
	uint16_t deltams = ms - sLastMillis ;
	sLastMillis  = ms;
	sPseudotime += deltams * msmultiplier;
	sHue16 += deltams * beatsin88( 400, 5,9);
	uint16_t brightnesstheta16 = sPseudotime;

	for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
		hue16 += hueinc16;
		uint8_t hue8 = hue16 / 256;

		brightnesstheta16  += brightnessthetainc16;
		uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

		uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
		uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
		bri8 += (255 - brightdepth);

		CRGB newcolor = CHSV( hue8, sat8, bri8);

		uint16_t pixelnumber = i;
		pixelnumber = (NUM_LEDS-1) - pixelnumber;

		nblend( leds[pixelnumber], newcolor, 64);
	}
}

// ---- Pacifica code starts here ---

CRGBPalette16 pacifica_palette_1 =
	{ 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
	  0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 =
	{ 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
	  0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 =
	{ 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33,
	  0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };


void pacifica_loop()
{
	// Increment the four "color index start" counters, one for each wave layer.
	// Each is incremented at a different speed, and the speeds vary over time.
	static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
	static uint32_t sLastms = 0;
	uint32_t ms = GET_MILLIS();
	uint32_t deltams = ms - sLastms;
	sLastms = ms;
	uint16_t speedfactor1 = beatsin16(3, 179, 269);
	uint16_t speedfactor2 = beatsin16(4, 179, 269);
	uint32_t deltams1 = (deltams * speedfactor1) / 256;
	uint32_t deltams2 = (deltams * speedfactor2) / 256;
	uint32_t deltams21 = (deltams1 + deltams2) / 2;
	sCIStart1 += (deltams1 * beatsin88(1011,10,13));
	sCIStart2 -= (deltams21 * beatsin88(777,8,11));
	sCIStart3 -= (deltams1 * beatsin88(501,5,7));
	sCIStart4 -= (deltams2 * beatsin88(257,4,6));

	// Clear out the LED array to a dim background blue-green
	fill_solid( leds, NUM_LEDS, CRGB( 2, 6, 10));

	// Render each of four layers, with different scales and speeds, that vary over time
	pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
	pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
	pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
	pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));

	// Add brighter 'whitecaps' where the waves lines up more
	pacifica_add_whitecaps();

	// Deepen the blues and greens a bit
	pacifica_deepen_colors();
}

// Add one layer of waves into the led array
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
	uint16_t ci = cistart;
	uint16_t waveangle = ioff;
	uint16_t wavescale_half = (wavescale / 2) + 20;
	for( uint16_t i = 0; i < NUM_LEDS; i++) {
		waveangle += 250;
		uint16_t s16 = sin16( waveangle ) + 32768;
		uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
		ci += cs;
		uint16_t sindex16 = sin16( ci) + 32768;
		uint8_t sindex8 = scale16( sindex16, 240);
		CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
		leds[i] += c;
	}
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
	uint8_t basethreshold = beatsin8( 9, 55, 65);
	uint8_t wave = beat8( 7 );

	for( uint16_t i = 0; i < NUM_LEDS; i++) {
		uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
		wave += 7;
		uint8_t l = leds[i].getAverageLight();
		if( l > threshold) {
			uint8_t overage = l - threshold;
			uint8_t overage2 = qadd8( overage, overage);
			leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
		}
	}
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
	for( uint16_t i = 0; i < NUM_LEDS; i++) {
		leds[i].blue = scale8( leds[i].blue,  145);
		leds[i].green= scale8( leds[i].green, 200);
		leds[i] |= CRGB( 2, 5, 7);
	}
}
