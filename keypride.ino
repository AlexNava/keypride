// Includes code from:
// Pride2015
// Animated, ever-changing rainbows.
// by Mark Kriegsman

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
#define BRIGHTNESS  255

#define N_SAMPLES 40
#define T_POLLING 2
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

uint16_t lightLevels[N_SAMPLES];
uint8_t  lightSampleIndex;

uint16_t scrollLockLevel;

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
    .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


void loop()
{
  if (effectsTimer->expired())
  {
    pride();
    FastLED.show();

    uint16_t lightLevel = 0;
    for (int i = 0; i < N_SAMPLES; ++i)
      lightLevel += lightLevels[i];

    Serial.println(scrollLockLevel);

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
