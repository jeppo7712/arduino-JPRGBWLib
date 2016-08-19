#include "JPRGBWLib.h"

JPRGBWLib::JPRGBWLib(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t common_cathode, void (*pinset)(const int, const uint8_t), uint16_t PWMlevels,
                     uint8_t _default_r,  uint8_t _default_g,  uint8_t _default_b,  uint8_t _default_w) :

        default_r(_default_r),
        default_g(_default_g),
        default_b(_default_b),
        default_w(_default_w)

{
        this->r_pin = r;
        this->g_pin = g;
        this->b_pin = b;
        this->w_pin = w;

        this->_common_cathode = common_cathode;
        fading = false;
        PinWrite = pinset;
        levels = PWMlevels-1;
        Hue = 0;
        Sat = 0;
        Bri = 0;
        HSBuptodate = true;
        requested_r = 0;
        requested_g = 0;
        requested_b = 0;
        requested_w = 0;
        curr_r = 0;
        curr_g = 0;
        curr_b = 0;
        curr_w = 0;
        last_r = (uint8_t)((uint16_t)default_r * levels / 255);
        last_g = (uint8_t)((uint16_t)default_g * levels / 255);
        last_b = (uint8_t)((uint16_t)default_b * levels / 255);
        last_w = (uint8_t)((uint16_t)default_w * levels / 255);
}

void JPRGBWLib::GetLastColor(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w)
{
        *r = (uint8_t)((uint16_t)last_r * 255 / levels);
        *g = (uint8_t)((uint16_t)last_g * 255 / levels);
        *b = (uint8_t)((uint16_t)last_b * 255 / levels);
        *w = (uint8_t)((uint16_t)last_w * 255 / levels);
}

// Set LED-color to custom color instantely
void JPRGBWLib::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
        // set color of LED
        if (_common_cathode)
        {
                PinWrite(r_pin, r);
                PinWrite(g_pin, g);
                PinWrite(b_pin, b);
                PinWrite(w_pin, w);
        }
        else
        {
                PinWrite(r_pin, levels - r);
                PinWrite(g_pin, levels - g);
                PinWrite(b_pin, levels - b);
                PinWrite(w_pin, levels - w);
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
        if (r != requested_r || g != requested_g || b != requested_b || w != requested_w)
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
        HSBuptodate = false;
        if (!r && !g && !b && !w)
        {
                last_r = requested_r;
                last_g = requested_g;
                last_b = requested_b;
                last_w = requested_w;
        }
        fadeTo((uint8_t)((uint16_t)r * levels / 255), (uint8_t)((uint16_t)g * levels / 255),
               (uint8_t)((uint16_t)b * levels / 255), (uint8_t)((uint16_t)w * levels / 255), 1, 1, 0);
}

void JPRGBWLib::requestLastColor()
{
        HSBuptodate = false;
        if (!getactivity())
                fadeTo(last_r, last_g, last_b, last_w, 1, 1, 0);
}

// Fade to custom color in specific time in specific steps
void JPRGBWLib::fadeToColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t steps, unsigned long duration, unsigned long startdelay)
{
        HSBuptodate = false;
        if (!r && !g && !b && !w)
        {
                last_r = requested_r;
                last_g = requested_g;
                last_b = requested_b;
                last_w = requested_w;
        }
        fadeTo((uint8_t)((uint16_t)r * levels / 255), (uint8_t)((uint16_t)g * levels / 255),
               (uint8_t)((uint16_t)b * levels / 255), (uint8_t)((uint16_t)w * levels / 255), steps, duration, startdelay);
}

void JPRGBWLib::fadeToLastColor(uint16_t steps, unsigned long duration, unsigned long startdelay)
{
        HSBuptodate = false;
        if (!getactivity())
                fadeTo(last_r, last_g, last_b, last_w, steps, duration, startdelay);
}

void JPRGBWLib::requestHSB(uint16_t hue, uint8_t sat, uint8_t bri)
{
        fadeToHSB(hue,sat,bri,1,1,0);
}

void JPRGBWLib::requestHue(uint16_t hue)
{
        if (!HSBuptodate)
        {
                rgbw2hsb(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
        }
        requestHSB(hue, Sat, Bri);
}

void JPRGBWLib::requestSat(uint8_t sat)
{
        if (!HSBuptodate)
        {
                rgbw2hsb(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
        }
        requestHSB(Hue, sat, Bri);
}

void JPRGBWLib::requestBri(uint8_t bri)
{
        if (!HSBuptodate)
        {
                rgbw2hsb(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
        }
        requestHSB(Hue, Sat, bri);
}

void JPRGBWLib::fadeToHSB(uint16_t hue, uint8_t sat, uint8_t bri, uint16_t steps, unsigned long duration, unsigned long startdelay)
{
        uint8_t r,g,b,w;
        if (hue != Hue || Sat != sat || Bri != bri || !HSBuptodate)
        {
                Hue = hue;
                Sat = sat;
                Bri = bri;
                HSBuptodate = true;
                hsb2rgbw(Hue, Sat, Bri, &r, &g, &b, &w);
                fadeTo(r,g,b,w,steps,duration,startdelay);
        }
}

void JPRGBWLib::fadeToHue(uint16_t hue, uint16_t steps, unsigned long duration, unsigned long startdelay)
{
        if (!HSBuptodate)
        {
                rgbw2hsb(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
                HSBuptodate = true;
        }
        fadeToHSB(hue, Sat, Bri, steps,duration,startdelay);
}

void JPRGBWLib::fadeToSat(uint8_t sat, uint16_t steps, unsigned long duration, unsigned long startdelay)
{
        if (!HSBuptodate)
        {
                rgbw2hsb(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
                HSBuptodate = true;
        }
        fadeToHSB(Hue, sat, Bri, steps,duration,startdelay);
}

void JPRGBWLib::fadeToBri(uint8_t bri, uint16_t steps, unsigned long duration, unsigned long startdelay)
{
        if (!HSBuptodate)
        {
                rgbw2hsb(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
                HSBuptodate = true;
        }
        fadeToHSB(Hue, Sat, bri, steps,duration,startdelay);
}

void JPRGBWLib::getcurrentColor(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w)
{
        *r = (uint8_t)((uint16_t)curr_r * 255 / levels);
        *g = (uint8_t)((uint16_t)curr_g * 255 / levels);
        *b = (uint8_t)((uint16_t)curr_b * 255 / levels);
        *w = (uint8_t)((uint16_t)curr_w * 255 / levels);
}

void JPRGBWLib::getrequestedColor(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w)
{
        *r = (uint8_t)((uint16_t)requested_r * 255 / levels);
        *g = (uint8_t)((uint16_t)requested_g * 255 / levels);
        *b = (uint8_t)((uint16_t)requested_b * 255 / levels);
        *w = (uint8_t)((uint16_t)requested_w * 255 / levels);
}

void JPRGBWLib::getHSB(uint16_t *hue, uint8_t *sat, uint8_t *bri) //based on requesed, i.e. final rgbw, not the current while fading
{
        if (!HSBuptodate)
        {
                rgbw2hsb(requested_r, requested_g, requested_b, requested_w, &Hue, &Sat, &Bri);
        }
        HSBuptodate = true;
        *hue = Hue;
        *sat = Sat;
        *bri = Bri;
}

void JPRGBWLib::getHue(uint16_t *hue) //based on requesed, i.e. final rgbw, not the current while fading
{
        getHSB( hue,  &Sat,  &Bri);
}

void JPRGBWLib::getSat(uint8_t *sat) //based on requesed, i.e. final rgbw, not the current while fading
{
        getHSB( &Hue,  sat,  &Bri);
}

void JPRGBWLib::getBri(uint8_t *bri) //based on requesed, i.e. final rgbw, not the current while fading
{
        getHSB( &Hue,  &Sat,  bri);
}

boolean JPRGBWLib::getactivity()
{
        return (requested_r || requested_g || requested_b || requested_w || fading);
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

void JPRGBWLib::hsb2rgbw(uint16_t hue, uint8_t sat, uint8_t bri, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w)
{
        uint32_t X, C = (uint32_t)sat * (uint32_t)bri;
        uint32_t *red, *green, *blue, white = (((uint32_t)bri * 100 - C) * (uint32_t)levels) / 10000;
        uint32_t Z = 0;

        if (hue < 60)
        {
                red = &C;
                green = &X;
                blue = &Z;
        }
        else if (hue < 120)
        {
                hue = 120 - hue;
                red = &X;
                green = &C;
                blue = &Z;
        }
        else if (hue < 180)
        {
                hue -= 120;
                red = &Z;
                green = &C;
                blue = &X;
        }
        else if (hue < 240)
        {
                hue = 240 - hue;
                red = &Z;
                green = &X;
                blue = &C;
        }
        else if (hue < 300)
        {
                hue -= 240;
                red = &X;
                green = &Z;
                blue = &C;
        }
        else
        {
                hue = 360 - hue;
                red = &C;
                green = &Z;
                blue = &X;
        }

        X = (C * hue * levels) / 600000;
        C = (C * levels) / 10000;

        *r = (uint8_t)(*red);
        *g = (uint8_t)(*green);
        *b = (uint8_t)(*blue);
        *w = (uint8_t)white;
}

void JPRGBWLib::rgbw2hsb(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t *hue, uint8_t *sat, uint8_t *bri)
{
        uint8_t max = 0;

        if (!r && !g && !b)
        {
                *hue = 0;
                *sat = 0;
        }
        else
        {
                if (!b)
                {
                        if (r>g)
                        {
                                *hue = (g * 60) / r;
                                max = r;
                        }
                        else
                        {
                                *hue = 120 - (r * 60) / g;
                                max = g;
                        }
                }
                else if (!g)
                {
                        if (r>b)
                        {

                                *hue = 360 - (b * 60) / r;
                                max = r;
                        }
                        else
                        {
                                *hue = 240 + (r * 60) / b;
                                max = b;
                        }
                }
                else if (!r)
                {
                        if (g>b)
                        {
                                *hue = 120 + (b * 60) / g;
                                max = g;
                        }
                        else
                        {
                                *hue = 240 - (g * 60) / b;
                                max = b;
                        }
                }
                else
                        *hue = 0;

                *sat = 100 - (w * 100) / (max + w);

        }

        *bri = ((max + w) * 100) / levels;
}
