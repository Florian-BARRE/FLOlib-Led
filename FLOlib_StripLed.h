#ifndef FLOlib_StripLed
#define FLOlib_StripLed
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#define def_nb_pixels 289
#define def_pin 5
#define def_stripLed_type NEO_BRG
#define def_stripLed_freq NEO_KHZ800

// FLOlib Strip LED
// I) StripLed_Manager
// ###--- Summary ---###
// --- Fonctions: void Start_stripLed()
// -------------- void Update_strip_led(String cmd)
// -------------- void StripLed_loop()
// --- Attributs: int color[3]
// -------------- int bright
// -------------- String animation
// -------------- bool ramdom_color
// -------------- short pin 
// -------------- short nb_pixels
// ###---------------###

// I) StripLed_Manager
class StripLed_Manager
{
private:
    // Colors
    int RED[3]          = {255, 0, 0};       
    int GREEN[3]        = {0, 255, 0};     
    int CYAN[3]         = {0, 255, 255};    
    int YELLOW[3]       = {255, 125, 0};  
    int ORANGE[3]       = {255, 40, 0};   
    int PURPLE[3]       = {255, 0, 255};  
    int PINK[3]         = {255, 0, 100};    
    int BLUE[3]         = {0, 0, 255};      
    int WHITE[3]        = {255, 255, 255}; 
    int ETEINT[3]       = {0, 0, 0};
    int *ALL_COLOR[8]   = {RED, GREEN, CYAN, YELLOW, ORANGE, PURPLE, PINK, BLUE};

    // Manipulation des leds
    int *Update_Color(int new_color[3]);
    void Instant_Update_Color(int target_color[]);
    int Update_Bright(int new_bright);
    void Random_Color();

    // Animations
    void _chnge_coul(int COLOR[]);
    // Feu
    void _feu();
    // Arc
    void _arc(int SpeedDelay);
    // Chase
    void _chase(int C[]);
    // Tire
    void _cto(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void _oct(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void _ltr(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void _rtl(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void _tire(int SpeedD, int C[], int ES, int RD);
    // Mete
    void _mete_g(int C[], byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
    void _mete_d(int C[], byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
    void _fadeToBlack(int ledNo, byte fadeValue);
    void _mete(int SpeedDelay, int C[], byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay);
    // Da
    void _da(int Delay);
    // Daa
    void _daa();
    // Spar
    void _spar(int SpeedDelay, int C[]);
    // Sspar
    void _sspar(int SpeedDelay, int C[], int SparkleDelay);
    // Illu
    void _illu(int SpeedDelay, int Count, boolean OnlyOne);
    // Unknow_anim
    void _unknow_anim();

    // Utilitaire(s)
    String Split_string_from_separator(String data, char separator, int index);

public:
    int color[3] = {0, 0, 0};
    int bright = 100;
    String animation = "led";
    bool ramdom_color = 1;
    short animation_delay = 5;
    unsigned long previousMillis = 0;
    int intervale_color_changement = 5000;
    short pin = def_pin;
    short nb_pixels = def_nb_pixels;

    Adafruit_NeoPixel strip_led = Adafruit_NeoPixel(nb_pixels, pin, def_stripLed_type + def_stripLed_freq);

    void Start_stripLed();

    void Update_strip_led(String cmd);

    void StripLed_loop();
};

#endif