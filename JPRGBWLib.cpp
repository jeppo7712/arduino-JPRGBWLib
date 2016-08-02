#include "JPRGBWLib.h"
//
// constructor; saves the pins
JPRGBWLib::JPRGBWLib(uint8_t r, uint8_t g, uint8_t b, uint8_t w, void (*pinset)(const int, const uint8_t), uint16_t PWMlevels)
{
        this->r_pin = r;
        this->g_pin = g;
        this->b_pin = b;
        this->w_pin = w;
        fading = false;
        PinWrite = pinset;
        pwmlevels = PWMlevels;
        _common_cathode = 1;
        Hue = 0;
        Sat = 0;
        Bri = 0;
        HSIuptodate = true;
}

/**
   Constructor using the Mode enum in header file.
   Ex: RGBTools(9, 10, 11, COMMON_ANODE);
 */
JPRGBWLib::JPRGBWLib(uint8_t r, uint8_t g, uint8_t b, uint8_t w, Mode mode, void (*pinset)(const int, const uint8_t), uint16_t PWMlevels)
{
        this->r_pin = r;
        this->g_pin = g;
        this->b_pin = b;
        this->w_pin = w;
        if (mode == COMMON_CATHODE)
                this->_common_cathode = 1;
        else
                this->_common_cathode = 0;
        fading = false;
        PinWrite = pinset;
        pwmlevels = PWMlevels;
        Hue = 0;
        Sat = 0;
        Bri = 0;
        HSIuptodate = true;
}

JPRGBWLib::JPRGBWLib(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t common_cathode, void (*pinset)(const int, const uint8_t), uint16_t PWMlevels)
{
        this->r_pin = r;
        this->g_pin = g;
        this->b_pin = b;
        this->w_pin = w;

        this->_common_cathode = common_cathode;
        fading = false;
        PinWrite = pinset;
        pwmlevels = PWMlevels;
        Hue = 0;
        Sat = 0;
        Bri = 0;
        HSIuptodate = true;
}

// Set LED-color to custom color instantely
void JPRGBWLib::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
        // set color of LED
        if (_common_cathode)
        {
                PinWrite(r_pin, (pwmlevels * r) >>8);
                PinWrite(g_pin, (pwmlevels * g) >>8);
                PinWrite(b_pin, (pwmlevels * b) >>8);
                PinWrite(w_pin, (pwmlevels * w) >>8);
        }
        else
        {
                PinWrite(r_pin, pwmlevels - ((pwmlevels * r) >>8));
                PinWrite(g_pin, pwmlevels - ((pwmlevels * g) >>8));
                PinWrite(b_pin, pwmlevels - ((pwmlevels * b) >>8));
                PinWrite(w_pin, pwmlevels - ((pwmlevels * w) >>8));
        }

        // save state
        curr_r = r;
        curr_g = g;
        curr_b = b;
        curr_w = w;
}

// Fade to custom color in specific time in specific steps
void JPRGBWLib::fadeTo(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t steps, unsigned long duration, unsigned long startdelay)
{
        if (r != requested_r && g != requested_g && b != requested_b && w != requested_w)
        {
                fadesteps = steps;

                requested_r = r;
                requested_g = g;
                requested_b = b;
                requested_w = w;

                start_r = curr_r;
                start_g = curr_g;
                start_b = curr_b;
                start_w = curr_w;

                // calculate the width of each step
                step_r = ((int32_t)(r - curr_r) << 8) / steps;
                step_g = ((int32_t)(g - curr_g) << 8) / steps;
                step_b = ((int32_t)(b - curr_b) << 8) / steps;
                step_w = ((int32_t)(w - curr_w) << 8) / steps;

                lastfadestep = 0;
                fading = true;
                fadedelay = duration / steps;
                fadestarttime = millis() + startdelay;
        }
}

void JPRGBWLib::requestColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
        HSIuptodate = false;
        fadeTo(r, g, b, w, 1, 1, 0);
}

// Fade to custom color in specific time in specific steps
void JPRGBWLib::fadeToColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t steps, unsigned long duration, unsigned long startdelay)
{
        HSIuptodate = false;
        fadeTo(r, g, b, w, steps, duration, startdelay);
}

void JPRGBWLib::requestHSI(uint16_t hue, uint8_t sat, uint8_t bri)
{
        fadeToHSI(hue,sat,bri,1,1,0);
}

void JPRGBWLib::requestHue(uint16_t hue)
{
        if (!HSIuptodate)
        {
                rgbw2hsi(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
        }
        requestHSI(hue, Sat, Bri);
}

void JPRGBWLib::requestSat(uint8_t sat)
{
        if (!HSIuptodate)
        {
                rgbw2hsi(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
        }
        requestHSI(Hue, sat, Bri);
}

void JPRGBWLib::requestBri(uint8_t bri)
{
        if (!HSIuptodate)
        {
                rgbw2hsi(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
        }
        requestHSI(Hue, Sat, bri);
}

void JPRGBWLib::fadeToHSI(uint16_t hue, uint8_t sat, uint8_t bri, uint16_t steps, unsigned long duration, unsigned long startdelay)
{
        uint8_t r,g,b,w;
        if (hue != Hue || Sat != sat || Bri != bri || !HSIuptodate)
        {
                Hue = hue;
                Sat = sat;
                Bri = bri;
                HSIuptodate = true;
                hsi2rgbw(Hue, Sat, Bri, &r, &g, &b, &w);
                fadeTo(r,g,b,w,steps,duration,startdelay);
        }
}

void JPRGBWLib::fadeToHue(uint16_t hue, uint16_t steps, unsigned long duration, unsigned long startdelay)
{
        if (!HSIuptodate)
        {
                rgbw2hsi(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
                HSIuptodate = true;
        }
        fadeToHSI(hue, Sat, Bri, steps,duration,startdelay);
}

void JPRGBWLib::fadeToSat(uint8_t sat, uint16_t steps, unsigned long duration, unsigned long startdelay)
{
        if (!HSIuptodate)
        {
                rgbw2hsi(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
                HSIuptodate = true;
        }
        fadeToHSI(Hue, sat, Bri, steps,duration,startdelay);
}

void JPRGBWLib::fadeToBri(uint8_t bri, uint16_t steps, unsigned long duration, unsigned long startdelay)
{
        if (!HSIuptodate)
        {
                rgbw2hsi(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
                HSIuptodate = true;
        }
        fadeToHSI(Hue, Sat, bri, steps,duration,startdelay);
}

void JPRGBWLib::getcurrentColor(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w)
{
        *r = curr_r;
        *g = curr_g;
        *b = curr_b;
        *w = curr_w;
}

void JPRGBWLib::getrequestedColor(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w)
{
        *r = requested_r;
        *g = requested_g;
        *b = requested_b;
        *w = requested_w;
}

void JPRGBWLib::getHSI(uint16_t *hue, uint8_t *sat, uint8_t *bri) //based on requesed, i.e. final rgbw, not the current while fading
{
        if (!HSIuptodate)
        {
                rgbw2hsi(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
        }
        HSIuptodate = true;
        *hue = Hue;
        *sat = Sat;
        *bri = Bri;
}

void JPRGBWLib::getHue(uint16_t *hue) //based on requesed, i.e. final rgbw, not the current while fading
{
        getHSI( hue,  &Sat,  &Bri);
}

void JPRGBWLib::getSat(uint8_t *sat) //based on requesed, i.e. final rgbw, not the current while fading
{
        getHSI( &Hue,  sat,  &Bri);
}

void JPRGBWLib::getBri(uint8_t *bri) //based on requesed, i.e. final rgbw, not the current while fading
{
        getHSI( &Hue,  &Sat,  bri);
}

boolean JPRGBWLib::getactivity()
{
        return (curr_r || curr_g || curr_b || curr_w || fading);
}

boolean JPRGBWLib::getfading()
{
        return fading;
}
void JPRGBWLib::loop()
{
        if (fading && (signed long)(millis() - fadestarttime) > 0)
        {
                currentfadestep = ((unsigned long)(millis() - fadestarttime)) / fadedelay;
                if (currentfadestep >= fadesteps)
                {
                        fading = false;
                        setColor(requested_r, requested_g, requested_b, requested_w);
                }
                else if (currentfadestep != lastfadestep)
                {
                        //set color of current step
                        setColor(
                                ((((int32_t)(start_r)) << 8) + (currentfadestep * step_r)) >> 8, // red target minus distance to target steps
                                ((((int32_t)(start_g)) << 8) + (currentfadestep * step_g)) >> 8, // green
                                ((((int32_t)(start_b)) << 8) + (currentfadestep * step_b)) >> 8, // blue
                                ((((int32_t)(start_w)) << 8) + (currentfadestep * step_w)) >> 8 // white
                                );
                        lastfadestep = currentfadestep;
                }
        }
}

void JPRGBWLib::hsi2rgbw(uint16_t hue, uint8_t sat, uint8_t bri, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w)
{
        int red, green, blue, white;
        float cos_h, cos_1047_h;
        float H, S, I;
        H = (float)fmod(hue,360); // cycle H around to 0-360 degrees
        H = 3.14159*H/(float)180; // Convert to radians.
        S = sat>100 ? 1 : sat/(float)100; // clamp S and I to interval [0,1]
        I = bri>100 ? 1 : bri/(float)100;

        if(H < 2.09439) {
                cos_h = cos(H);
                cos_1047_h = cos(1.047196667-H);
                red = S*255*I/3*(1+cos_h/cos_1047_h);
                green = S*255*I/3*(1+(1-cos_h/cos_1047_h));
                blue = 0;
                white = 255*(1-S)*I;
        } else if(H < 4.188787) {
                H = H - 2.09439;
                cos_h = cos(H);
                cos_1047_h = cos(1.047196667-H);
                green = S*255*I/3*(1+cos_h/cos_1047_h);
                blue = S*255*I/3*(1+(1-cos_h/cos_1047_h));
                red = 0;
                white = 255*(1-S)*I;
        } else {
                H = H - 4.188787;
                cos_h = cos(H);
                cos_1047_h = cos(1.047196667-H);
                blue = S*255*I/3*(1+cos_h/cos_1047_h);
                red = S*255*I/3*(1+(1-cos_h/cos_1047_h));
                green = 0;
                white = 255*(1-S)*I;
        }
        *r=red;
        *g=green;
        *b=blue;
        *w=white;
}

void JPRGBWLib::rgbw2hsi(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t *hue, uint8_t *sat, uint8_t *bri)
{
        float H, S, I;

        I = ((float)r+g+b+w)/(float)7.65;
        S = (1-w/((float)r+g+b+w))*100;

        H = acosf((r-g/(float)2-b/(float)2)/sqrt((float)r*r+(float)g*g+(float)b*b-(float)r*g-(float)r*b-(float)g*b));
        H = 180*H/(float)3.14159;
        if (b > g)
                H = 360 - H;

        *hue = (uint16_t)H;
        *sat = (uint8_t)S;
        *bri = (uint8_t)I;
}
