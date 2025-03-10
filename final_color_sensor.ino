#include <MeMCore.h>
#include <Arduino.h>

// number of color sample measurements to take in each channel
#define NO_SAMPLES 50

#define BUTTON A7

// response time for taking measurement in ms
#define RESPONSE_TIME 200

// set up threshold values the colors
 #define R_YELLOW_LOWER 75
 #define R_RED_LOWER 55
 #define B_BLUE_LOWER 65
 #define R_PURPLE_LOWER 35
 #define R_BLACK_UPPER 10

// values to control RGB LED
#define RED_LED 255,000,000
#define GREEN_LED 000,255,000
#define BLUE_LED 000,000,255
#define OFF_LED 000,000,000

// contains details for red, green and blue channels
struct Color {
  int red;
  int green;
  int blue;
};

// used for calibration. ENSURE THESE HAVE BEEN UPDATED!
Color black = {149, 98, 107};
Color white = {227, 151, 165};

// initialise peripheral objects
MeLightSensor light_sensor(PORT_6);
MeRGBLed led(PORT_7);  // the internal connection is handled by port 7, with default slot number 2

/* COLOR CODE FOR REFERENCE DURING CALIBRATION
 * RED -> return 0
   GREEN -> return 1
   YELLOW -> return 2
   PURPLE -> return 3
   BLUE -> return 4 
   WHITE -> return 5
   BLACK -> return 6
   OTHERS -> return -1 */

// colors for calibration

void setup() {
  // initialize serial communication at 9600 bps
  Serial.begin(9600);

  // initialize start button
  // press then release button to begin the run
  pinMode(BUTTON, INPUT);

  while (analogRead(BUTTON) > 500) {}
  while (analogRead(BUTTON) < 500) {}
  
  // wait for button press to continue with the for loop
  Serial.println("Waiting for button press");
}

void loop() {
  // print out color detected for debugging only
  Serial.println(use_light_sensor());
}

int use_light_sensor() {
  /* returns the color code based on measurement from the color sensor */
  
  // take color measurement
  Color new_color = get_colors();

  // do color balancing. first constrain the new_color measurements so they don't
  // go out of bounds, then scale them
  new_color.red = constrain(new_color.red, black.red, white.red);
  new_color.green = constrain(new_color.green, black.green, white.green);
  new_color.blue = constrain(new_color.blue, black.blue, white.blue);
  
  new_color.red = map(new_color.red, black.red, white.red, 0, 100);
  new_color.green = map(new_color.green, black.green, white.green, 0, 100);
  new_color.blue = map(new_color.blue, black.blue, white.blue, 0, 100);
  
  // return the corresponding code for that color
  return get_color_code(new_color);
}


 int get_color_code(Color color) {
   // returns the corresponding color code

   int red = color.red;
   int green = color.green;
   int blue = color.blue;
   Serial.print(red);
   Serial.print("\t");
   Serial.print(green);
   Serial.print("\t");
   Serial.println(blue);

   if (red > R_YELLOW_LOWER)
   {
     // yellow
     Serial.println("yellow");
     return 2;
   } else if (red > R_RED_LOWER)
   {
     // red
     Serial.println("red");
     return 0;
   } else if (blue > B_BLUE_LOWER)
   {
     // blue
     Serial.println("blue");
     return 4;
   } else if (red > R_PURPLE_LOWER)
   {
     // purple
     Serial.println("purple");
     return 3;
   } else if (red < R_BLACK_UPPER)
   {
     // black
     Serial.println("black");
     return 6;
   } else {
     // green
     Serial.println("green");
     return 1;
   }
 }


Color get_colors() {
  // takes NO_SAMPLES samples and returns the average for each channel
  Color color;

  // set RGB LED to RED
  led.setColor(RED_LED);
  led.show();
  color.red = get_single_color();
 
  // set RGB LED to GREEN
  led.setColor(GREEN_LED);
  led.show();
  color.green = get_single_color();

  // set RGB LED to BLUE
  led.setColor(BLUE_LED);
  led.show();
  color.blue = get_single_color();
  
  // turn off RGB LED
  led.setColor(OFF_LED);
  led.show();

  return color;
}

int get_single_color() {
  // returns the average color value for single channel
  int sum = 0;

  delay(RESPONSE_TIME);
  
  for (int j = 0; j < NO_SAMPLES; j++){
    sum += light_sensor.read();
  }
  
  return sum / NO_SAMPLES;
}
