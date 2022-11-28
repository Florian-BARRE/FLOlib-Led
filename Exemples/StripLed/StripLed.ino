#include "FLOlib_StripLed.h"

StripLed_Manager led;
void setup() {
 led.Start_stripLed();
 
 led.Update_strip_led(String("Coul,0,0,0"));
 led.Update_strip_led(String("Bright,100"));
}

void loop() {
  
 led.StripLed_loop();

}
