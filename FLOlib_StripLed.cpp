#include <Arduino.h>
#include "FLOlib_StripLed.h"

void StripLed_Manager::Start_stripLed()
{
    strip_led.begin();
    Update_strip_led(String("Coul,100,0,0"));
    Update_strip_led(String("Bright,5"));
}

int *StripLed_Manager::Update_Color(int new_color[3])
{
    // 2 cas possibles: animation en cours | pas d'animation en cours
    // 1) LED => Pas d'animation en cours
    if (animation == String("led") || animation == String("daa"))
    {
        while (color[0] != new_color[0] || color[1] != new_color[1] || color[2] != new_color[2])
        {
            if (color[0] > new_color[0])
                color[0]--;
            else if (color[0] < new_color[0])
                color[0]++;

            if (color[1] > new_color[1])
                color[1]--;
            else if (color[1] < new_color[1])
                color[1]++;

            if (color[2] > new_color[2])
                color[2]--;
            else if (color[2] < new_color[2])
                color[2]++;

            delay(animation_delay / 5);
            Instant_Update_Color(color);
        }
    }
    else
    {
    } // On fait rien, juste actualisation de "color" grâce au return de la fonction

    return new_color;
}

void StripLed_Manager::Instant_Update_Color(int target_color[])
{
    for (int i = 0; i < nb_pixels; i++)
    {
        strip_led.setPixelColor(i, target_color[0], target_color[1], target_color[2]);
    }
    strip_led.show();
}

int StripLed_Manager::Update_Bright(int new_bright)
{
    // 2 cas possibles: animation en cours | pas d'animation en cours
    // 1) LED => Pas d'animation en cours
    if (animation == String("led"))
    {
        while (bright <= new_bright)
        {
            strip_led.setBrightness(bright);
            Instant_Update_Color(color);
            bright++;
            delay(animation_delay);
        }
        while (bright >= new_bright)
        {
            strip_led.setBrightness(bright);
            Instant_Update_Color(color);
            bright--;
            delay(animation_delay);
        }
    }

    // 2) si !LED alors anim en cours
    else
    {
        strip_led.setBrightness(bright);
        strip_led.show();
    }

    return new_bright;
}

void StripLed_Manager::Random_Color()
{
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= intervale_color_changement)
    {
        previousMillis = currentMillis;

        byte new_color_index = random(0, 8);
        while (color[0] == ALL_COLOR[new_color_index][0] &&
               color[1] == ALL_COLOR[new_color_index][1] &&
               color[2] == ALL_COLOR[new_color_index][2])
        {
            new_color_index = random(0, 8);
        }

        for (byte k = 0; k < 3; k++)
        {
            color[k] = ALL_COLOR[new_color_index][k];
        }
    }
}


String StripLed_Manager::Split_string_from_separator(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void StripLed_Manager::Update_strip_led(String cmd)
{
    // Decode cmd
    String decoded_cmd[8];
    for (byte k = 0; k < 8; k++)
    {
        decoded_cmd[k] = Split_string_from_separator(cmd, ',', k);
    }
    String cmd_type = decoded_cmd[0];

    // 6 messages possibles: Bright | Delay | Anim | Coul | Mode_Auto_Coul | All
    // 1) Bright
    if (cmd_type == String("Bright") || cmd_type == String("bright"))
        bright = Update_Bright(decoded_cmd[1].toInt());
    // 2) Delay
    else if (cmd_type == String("Delay") || cmd_type == String("delay"))
        animation_delay = decoded_cmd[1].toInt();
    // 3) Anim
    else if (cmd_type == String("Anim") || cmd_type == String("anim"))
        animation = decoded_cmd[1];
    // 4) Coul
    else if (cmd_type == String("Coul") || cmd_type == String("coul"))
    {
        int new_color[3];
        for (byte k = 0; k < 3; k++)
        {
            if (decoded_cmd[k + 1] == String("*"))
            {
                new_color[k] = color[k];
            }
            else
            {
                new_color[k] = decoded_cmd[k + 1].toInt();
            }
        }
        int *tmp_color = Update_Color(new_color);
        for (byte k = 0; k < 3; k++)
        {
            color[k] = tmp_color[k];
        }
    }
    // 5) Mode_Auto_Coul
    else if (cmd_type == String("Mode_Auto_Coul") || cmd_type == String("mode_auto_coul"))
        ramdom_color = decoded_cmd[1].toInt();

    // 6) All
    else if (cmd_type == String("All") || cmd_type == String("all"))
    {
        int new_color[3] = {color[0], color[1], color[2]};

        // 1) Anim
        if (String(decoded_cmd[1]) != String("*"))
            animation = decoded_cmd[1];
        // 2) Anim_Delay
        if (String(decoded_cmd[2]) != String("*"))
            animation_delay = decoded_cmd[2].toInt();
        // 3) Bright
        if (String(decoded_cmd[3]) != String("*"))
            bright = Update_Bright(decoded_cmd[3].toInt());
        // 4) Color[0]
        if (String(decoded_cmd[4]) != String("*"))
            new_color[0] = decoded_cmd[4].toInt();
        // 5) Color[1]
        if (String(decoded_cmd[5]) != String("*"))
            new_color[1] = decoded_cmd[5].toInt();
        // 6) Color[2]
        if (String(decoded_cmd[6]) != String("*"))
            new_color[2] = decoded_cmd[6].toInt();
        // 7) Mode_Auto_Coul
        if (String(decoded_cmd[7]) != String("*"))
            ramdom_color = decoded_cmd[7].toInt();

        int *tmp_color = Update_Color(new_color);
        for (byte k = 0; k < 3; k++)
        {
            color[k] = tmp_color[k];
        }
    }
}

void StripLed_Manager::_chnge_coul(int COLOR[])
{
    for (int i = 0; i < nb_pixels; i++)
    {
        strip_led.setPixelColor(i, COLOR[0], COLOR[1], COLOR[2]);
    }
    strip_led.show();
}

// Feu
void StripLed_Manager::_feu()
{
    int r = ORANGE[0];
    int g = ORANGE[1] + 5;
    int b = ORANGE[2];
    int variation = 70;
    for (int x = 0; x < nb_pixels; x++)
    {
        int flicker = random(0, variation);
        int r1 = r - flicker;
        int g1 = g - flicker;
        int b1 = b - flicker;
        if (g1 < 0)
            g1 = 0;
        if (r1 < 0)
            r1 = 0;
        if (b1 < 0)
            b1 = 0;
        strip_led.setPixelColor(x, r1, g1, b1);
    }
    strip_led.setBrightness(bright);
    strip_led.show();
    delay(random(70, 135));
}
// Arc
byte *Wheel(byte WheelPos)
{
    static byte c[3];
    if (WheelPos < 85)
    {
        c[0] = WheelPos * 3;
        c[1] = 255 - WheelPos * 3;
        c[2] = 0;
    }
    else if (WheelPos < 170)
    {
        WheelPos -= 85;
        c[0] = 255 - WheelPos * 3;
        c[1] = 0;
        c[2] = WheelPos * 3;
    }
    else
    {
        WheelPos -= 170;
        c[0] = 0;
        c[1] = WheelPos * 3;
        c[2] = 255 - WheelPos * 3;
    }
    return c;
}

void StripLed_Manager::_arc(int SpeedDelay)
{
    byte *c;
    uint16_t i, j;
    for (j = 0; j < 256; j++)
    { // 5 cycles of all colors on wheel
        for (i = 0; i < nb_pixels; i++)
        {
            c = Wheel(((i * 256 / nb_pixels) + j) & 255);
            strip_led.setPixelColor(i, *c, *(c + 1), *(c + 2));
        }
        strip_led.show();
        strip_led.setBrightness(bright);
        strip_led.show();
        delay(SpeedDelay);
    }
}
// Chase
void StripLed_Manager::_chase(int C[])
{
    for (int q = 0; q < 5; q++)
    {
        for (int i = 0; i < nb_pixels; i = i + 5)
        {
            strip_led.setPixelColor(i + q, C[0], C[1], C[2]);
        }
        strip_led.show();
        delay(animation_delay * 10);
        for (int i = 0; i < nb_pixels; i = i + 3)
        {
            strip_led.setPixelColor(i + q, 0, 0, 0);
        }
    }
}

// Tire
void StripLed_Manager::_cto(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for (int i = ((nb_pixels - EyeSize) / 2); i >= 0; i--)
    {
        _chnge_coul(ETEINT);
        strip_led.setPixelColor(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            strip_led.setPixelColor(i + j, red, green, blue);
        }
        strip_led.setPixelColor(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        strip_led.setPixelColor(nb_pixels - i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            strip_led.setPixelColor(nb_pixels - i - j, red, green, blue);
        }
        strip_led.setPixelColor(nb_pixels - i - EyeSize - 1, red / 10, green / 10, blue / 10);
        strip_led.show();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}
void StripLed_Manager::_oct(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for (int i = 0; i <= ((nb_pixels - EyeSize) / 2); i++)
    {
        _chnge_coul(ETEINT);
        strip_led.setPixelColor(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            strip_led.setPixelColor(i + j, red, green, blue);
        }
        strip_led.setPixelColor(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        strip_led.setPixelColor(nb_pixels - i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            strip_led.setPixelColor(nb_pixels - i - j, red, green, blue);
        }
        strip_led.setPixelColor(nb_pixels - i - EyeSize - 1, red / 10, green / 10, blue / 10);
        strip_led.show();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}
void StripLed_Manager::_ltr(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for (int i = 0; i < nb_pixels - EyeSize - 2; i++)
    {
        _chnge_coul(ETEINT);
        strip_led.setPixelColor(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            strip_led.setPixelColor(i + j, red, green, blue);
        }
        strip_led.setPixelColor(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        strip_led.show();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}
void StripLed_Manager::_rtl(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{ // RightToLeft
    for (int i = nb_pixels - EyeSize - 2; i > 0; i--)
    {
        _chnge_coul(ETEINT);
        strip_led.setPixelColor(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            strip_led.setPixelColor(i + j, red, green, blue);
        }
        strip_led.setPixelColor(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        strip_led.show();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}
void StripLed_Manager::_tire(int SpeedD, int C[], int ES, int RD)
{
    _rtl(C[0], C[1], C[2], ES, SpeedD, RD);
    _ltr(C[0], C[1], C[2], ES, SpeedD, RD);
    _oct(C[0], C[1], C[2], ES, SpeedD, RD);
    _cto(C[0], C[1], C[2], ES, SpeedD, RD);
    _ltr(C[0], C[1], C[2], ES, SpeedD, RD);
    _rtl(C[0], C[1], C[2], ES, SpeedD, RD);
    _oct(C[0], C[1], C[2], ES, SpeedD, RD);
    _cto(C[0], C[1], C[2], ES, SpeedD, RD);
}

// Mete
void StripLed_Manager::_mete_g(int C[], byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay)
{
    _chnge_coul(ETEINT);
    for (int i = nb_pixels; i >= -30; i--)
    {
        for (int j = 0; j < nb_pixels; j++)
        {
            if ((!meteorRandomDecay) || (random(10) > 5))
            {
                _fadeToBlack(j, meteorTrailDecay);
            }
        }
        for (int j = 0; j < meteorSize; j++)
        {
            if ((i - j < nb_pixels) && (i - j >= 0))
            {
                strip_led.setPixelColor(nb_pixels - i - j, C[0], C[1], C[2]);
            }
        }
        strip_led.show();
        delay(SpeedDelay);
    }
}
void StripLed_Manager::_mete_d(int C[], byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay)
{
    _chnge_coul(ETEINT);
    for (int i = nb_pixels; i >= -30; i--)
    {
        for (int j = 0; j < nb_pixels; j++)
        {
            if ((!meteorRandomDecay) || (random(10) > 5))
            {
                _fadeToBlack(j, meteorTrailDecay);
            }
        }
        for (int j = 0; j < meteorSize; j++)
        {
            if ((i - j < nb_pixels) && (i - j >= 0))
            {
                strip_led.setPixelColor(i - j, C[0], C[1], C[2]);
            }
        }
        strip_led.show();
        delay(SpeedDelay);
    }
}
void StripLed_Manager::_fadeToBlack(int ledNo, byte fadeValue)
{
#ifdef ADAFRUIT_NEOPIXEL_H
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
    oldColor = strip_led.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);
    r = (r <= 10) ? 0 : (int)r - (r * fadeValue / 256);
    g = (g <= 10) ? 0 : (int)g - (g * fadeValue / 256);
    b = (b <= 10) ? 0 : (int)b - (b * fadeValue / 256);
    strip_led.setPixelColor(ledNo, r, g, b);
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
    leds[ledNo].fadeToBlackBy(fadeValue);
#endif
}
void StripLed_Manager::_mete(int SpeedDelay, int C[], byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay)
{
    _mete_d(C, meteorSize, meteorTrailDecay, meteorRandomDecay, SpeedDelay);
    /* Pas de sens inverse car bandeau = circulaire
    _mete_g(C, meteorSize, meteorTrailDecay, meteorRandomDecay, SpeedDelay);
    */
}

// Da
void StripLed_Manager::_da(int Delay)
{
    for (int i = 0; i < nb_pixels; i++)
    {
        strip_led.setPixelColor(i, color[0], color[1], color[2]);
        strip_led.show();
        delay(Delay);
    }

    byte Rang_New_Color = random(0, 8);
    while (color[0] == ALL_COLOR[Rang_New_Color][0] && color[1] == ALL_COLOR[Rang_New_Color][1] && color[2] == ALL_COLOR[Rang_New_Color][2])
    {
        Rang_New_Color = random(0, 8);
    }
    for (byte k = 0; k < 3; k++)
    {
        color[k] = ALL_COLOR[Rang_New_Color][k];
    }

    /* Pas de sens inverse car bandeau = circulaire
    for(int i=nb_pixels; i>=0; i--){ strip_led.setPixelColor(i, color[0], color[1], color[2]); strip_led.show();delay(Delay);}

    Rang_New_Color = random(0, 8);
    while( color[0]==ALL_COLOR[Rang_New_Color][0]&&color[1]==ALL_COLOR[Rang_New_Color][1]&&color[2]==ALL_COLOR[Rang_New_Color][2]){Rang_New_Color=random(0, 8);}
    for(byte k=0; k<3; k++){ color[k] = ALL_COLOR[Rang_New_Color][k] ; }
    */
}

// Daa
void StripLed_Manager::_daa()
{
    int New_Coul[3];
    byte Rang_New_Color = random(0, 8);
    while (color[0] == ALL_COLOR[Rang_New_Color][0] && color[1] == ALL_COLOR[Rang_New_Color][1] && color[2] == ALL_COLOR[Rang_New_Color][2])
    {
        Rang_New_Color = random(0, 8);
    }
    for (byte k = 0; k < 3; k++)
    {
        New_Coul[k] = ALL_COLOR[Rang_New_Color][k];
    }
    int *Tampon_Coul = Update_Color(New_Coul);
    for (byte k = 0; k < 3; k++)
    {
        color[k] = Tampon_Coul[k];
    }
}
// Spar
void StripLed_Manager::_spar(int SpeedDelay, int C[])
{
    strip_led.setBrightness(0);
    int Pixel = random(nb_pixels);
    strip_led.setBrightness(255);
    strip_led.setPixelColor(Pixel, C[0], C[1], C[2]);
    strip_led.show();
    delay(SpeedDelay);
    strip_led.setPixelColor(Pixel, 0, 0, 0);
}

// Sspar
void StripLed_Manager::_sspar(int SpeedDelay, int C[], int SparkleDelay)
{
    C[0] = C[0] / 20;
    C[1] = C[1] / 20;
    C[2] = C[2] / 20;
    for (int i = 0; i < nb_pixels; i++)
    {
        strip_led.setPixelColor(i, C[0], C[1], C[2]);
    }
    strip_led.show();
    int Pixel = random(nb_pixels);
    strip_led.setPixelColor(Pixel, C[0] * 20, C[1] * 20, C[2] * 20);
    strip_led.show();
    delay(SparkleDelay);
    strip_led.setPixelColor(Pixel, C[0], C[1], C[2]);
    strip_led.show();
    delay(SpeedDelay);
    C[0] = C[0] * 20;
    C[1] = C[1] * 20;
    C[2] = C[2] * 20;
}
// Illu
void StripLed_Manager::_illu(int SpeedDelay, int Count, boolean OnlyOne)
{
    for (int i = 0; i < nb_pixels; i++)
    {
        strip_led.setPixelColor(i, ETEINT[0], ETEINT[1], ETEINT[2]);
    }
    strip_led.show();
    strip_led.setBrightness(bright);
    strip_led.show();
    int Fade = bright;
    for (int i = 0; i < Count; i++)
    {
        strip_led.setPixelColor(random(nb_pixels), color[0], color[1], color[2]);
        strip_led.show();
        delay(SpeedDelay);
    }
    while (Fade >= 0)
    {
        strip_led.setBrightness(Fade);
        strip_led.show();
        Fade--;
    }
    strip_led.setBrightness(bright);
    strip_led.show();
}
// Unknow_anim
void StripLed_Manager::_unknow_anim()
{
    strip_led.setBrightness(255);
    for (int i = 0; i < nb_pixels; i += 2)
    {
        strip_led.setPixelColor(i, 255, 0, 0);
    }
    strip_led.show();
    delay(1000);
    for (int i = 0; i < nb_pixels; i++)
    {
        strip_led.setPixelColor(i, 0, 0, 0);
    }
    strip_led.show();
    for (int i = 1; i < nb_pixels; i += 2)
    {
        strip_led.setPixelColor(i, 255, 0, 0);
    }
    strip_led.show();
    delay(1000);
    for (int i = 0; i < nb_pixels; i++)
    {
        strip_led.setPixelColor(i, 0, 0, 0);
    }
    strip_led.show();
}

//
void StripLed_Manager::StripLed_loop()
{
    if (animation == String("feu"))
        _feu();
    else if (animation == String("arc"))
        _arc(animation_delay * 3);
    else if (animation == String("chase"))
        _chase(color);
    else if (animation == String("tire"))
        _tire(animation_delay * 4, color, 8, 50);
    else if (animation == String("mete"))
        _mete(animation_delay * 16, color, 5, 64, true);
    else if (animation == String("da"))
        _da(animation_delay * 4);
    else if (animation == String("daa"))
        _daa();
    else if (animation == String("spar"))
        _spar(animation_delay * 2, color);
    else if (animation == String("sspar"))
        _sspar(random(animation_delay * 20, animation_delay * 140), color, 10);
    else if (animation == String("illu"))
        _illu(animation_delay * 5, 300, false);
    else if (animation == String("led"))
        Instant_Update_Color(color); // Force les leds à la couleur (évite que certaines leds changent de couleur sans raison)
    else
        _unknow_anim();

    if ((animation == String("chase") || animation == String("tire") || animation == String("mete") || animation == String("illu")) && ramdom_color)
    {
        Random_Color();
    }
}