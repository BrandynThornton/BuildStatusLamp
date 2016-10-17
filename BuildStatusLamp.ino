#include <Adafruit_NeoPixel.h>
#include <math.h>
#include "pitches.h"

#define PIN 6  // the digital pin the led is connected to
#define SPEAKER 9
#define STEPS 100
#define DELAY 30
#define AUTO_OFF_MIL 300000

//DEATH SOUND
// notes in the melody:
int deathMelody[] = {
  NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_REST,
  NOTE_B4, NOTE_F5, NOTE_REST, NOTE_F5,
  NOTE_F5, NOTE_E5, NOTE_D5,
  NOTE_C5, NOTE_E4, NOTE_G3, NOTE_E4,
  NOTE_C4, NOTE_REST, NOTE_REST };

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int deathNoteDurations[] = {
  16, 16, 16, 1,
  4, 4, 4, 4,
  3, 3, 3,
  4, 4, 4, 4,
  4, 1, 2 
};

//RESCUE
// notes in the melody:
int rescueMelody[] = {
  NOTE_C5, NOTE_G4, NOTE_E4, NOTE_C5, NOTE_G4, NOTE_E4, NOTE_C5, NOTE_REST,
  NOTE_DS5, NOTE_AS4, NOTE_F4, NOTE_F5, NOTE_A4, NOTE_F4, NOTE_DS5, NOTE_REST,
  NOTE_DS5, NOTE_AS4, NOTE_G4, NOTE_E5, NOTE_B4, NOTE_G4, NOTE_DS5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_G5 };

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int rescueNoteDurations[] = {
  4, 4, 4, 4, 4, 4, 1, 4,
  4, 4, 4, 4, 4, 4, 1, 4,
  4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 1 
};

// Modifed NeoPixel sample for the holiday craft project

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);
String oldColor = "#000000";
String lastBuildColor = "#000000";
String newColor;
char inByte = 0;
long lastRead = 0;
bool active = true;

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
  strip.setBrightness(255);
}

void loop() {
  if(Serial.available() > 0) {
    inByte = Serial.peek();
    
    if(inByte == '#') {
      newColor = Serial.readString();
      Serial.println("newColor :");
      Serial.println(newColor);
      
      setColor();
      
      lastRead = millis();
      active = true;
    }
  }
  if(active && ((millis() - lastRead) > AUTO_OFF_MIL))
  {
    newColor = "#000000";
    setColor();
    active = false;
  }
} // loop


// fade_up - fade up to the given color
void fade_from_to(int oR, int oB, int oG, int R, int B, int G) {
   uint16_t i, j;
   int rStep = getStep(oR, R);
   int bStep = getStep(oB, B);
   int gStep = getStep(oG, G);
   
   for (i=0; i<STEPS; i++) {
     oR = getNextValue(oR, R, rStep);
     oB = getNextValue(oB, B, bStep);
     oG = getNextValue(oG, G, gStep);

      for(j=0; j<strip.numPixels(); j++) {
         strip.setPixelColor(j, strip.Color(oR, oB, oG));
      }
      String rMessage = "oR :";
      Serial.println(rMessage);
      Serial.println(oR);
      String bMessage = "oB :";
      Serial.println(bMessage);
      Serial.println(oB);
      String gMessage = "oG :";
      Serial.println(gMessage);
      Serial.println(oG);
   strip.show();
   delay(DELAY);
   }  
} // fade_from_to

void fade(String oldColor, String newColor) {
  // Get rid of '#' and convert it to integer
  long oldNumber = (long) strtol( &oldColor[1], NULL, 16);
  long newNumber = (long) strtol( &newColor[1], NULL, 16);

  // Split them up into r, g, b values
  int oR = oldNumber >> 16;
  int oG = oldNumber >> 8 & 0xFF;
  int oB = oldNumber & 0xFF;
  
  // Split them up into r, g, b values
  int r = newNumber >> 16;
  int g = newNumber >> 8 & 0xFF;
  int b = newNumber & 0xFF;

  fade_from_to(oR, oG, oB, r, g, b);
}

int getNextValue(int old, int desired, int increment) {
  old += increment;
  
  if(old > 255) {
    return 255;
  } else if (old < 0) {
    return 0;
  }
  
  if(increment < 0 && old < desired)
  {
    return desired;
  } else if (increment > 0 && old > desired) {
    return desired;
  }
  
  return old;
}

int getStep(int old, int desired) {
  int stepVal = 0;
  float range = desired - old;
  Serial.println("range :");
  Serial.println(range);
  if(range > 0) {
      Serial.println("ceil input :");
      Serial.println((float)(range / STEPS));
     stepVal = ceil((float)(range / STEPS));
   } else if (range < 0) {
     Serial.println("floor input :");
     Serial.println(range / STEPS);
     stepVal = floor(range / STEPS);
   }
   Serial.println("stepVal :");
   Serial.println(stepVal);
   return stepVal;
}

void setColor() {
  fade(oldColor, newColor);
  
  //check for song case before oldColor = newColor
  if(newColor == "#FF6600" && oldColor != "#FF6600") {
    lastBuildColor = oldColor;
  }
  //TODO: Fix sound issues
  if(lastBuildColor == "#FF0000" && newColor == "#00FF00") {
    //rescue
    //playMario(rescueMelody, rescueNoteDurations, .55, (int)(sizeof(rescueMelody)/sizeof(rescueMelody[0])));
  } else if (lastBuildColor == "#00FF00" && newColor == "#FF0000") {
    //death
    //playMario(deathMelody, deathNoteDurations, .6, (int)(sizeof(deathMelody)/sizeof(deathMelody[0])));
  }
  
  oldColor = newColor;

  newColor = "";
}

void playMario(int melody[], int duration[], double pause, int arrLength) {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < arrLength; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / duration[thisNote];
    tone(SPEAKER, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    int pauseBetweenNotes = noteDuration * pause;
    
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(SPEAKER);
  }
}
