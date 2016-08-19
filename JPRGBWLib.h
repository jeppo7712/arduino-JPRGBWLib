
#ifndef JPRGBWLib_H
#define JPRGBWLib_H

#include <Arduino.h>

//#define RGBWTOOL_DEBUG

enum Mode { COMMON_ANODE, COMMON_CATHODE };

class JPRGBWLib
{
public:
        JPRGBWLib(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t common_cathode, void (*pinset)(const int, const uint8_t), uint16_t PWMlevels,
                  uint8_t _default_r,  uint8_t _default_g,  uint8_t _default_b,  uint8_t _default_w);

        void loop();

        void requestColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
        void requestLastColor();
        void requestHSB(uint16_t hue, uint8_t sat, uint8_t bri);
        void requestHue(uint16_t hue);
        void requestSat(uint8_t sat);
        void requestBri(uint8_t bri);

        void fadeToColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t steps, unsigned long duration, unsigned long startdelay);
        void fadeToLastColor(uint16_t steps, unsigned long duration, unsigned long startdelay);
        void fadeToHSB(uint16_t hue, uint8_t sat, uint8_t bri, uint16_t steps, unsigned long duration, unsigned long startdelay);
        void fadeToHue(uint16_t hue, uint16_t steps, unsigned long duration, unsigned long startdelay);
        void fadeToSat(uint8_t sat, uint16_t steps, unsigned long duration, unsigned long startdelay);
        void fadeToBri(uint8_t bri, uint16_t steps, unsigned long duration, unsigned long startdelay);

        void getcurrentColor(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w);
        void getrequestedColor(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w);
        void getHSB(uint16_t *hue, uint8_t *sat, uint8_t *bri); //based on requesed, i.e. final rgbw, not the current while fading
        void getHue(uint16_t *hue); //based on requesed, i.e. final rgbw, not the current while fading
        void getSat(uint8_t *sat); //based on requesed, i.e. final rgbw, not the current while fading
        void getBri(uint8_t *bri); //based on requesed, i.e. final rgbw, not the current while fading

        void GetLastColor(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w); //returns color to use for last color command

        boolean getactivity();
        boolean getfading();

        void hsb2rgbw(uint16_t hue, uint8_t sat, uint8_t bri, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w);
        void rgbw2hsb(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t *hue, uint8_t *sat, uint8_t *bri);

private:
        void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
        void fadeTo(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t steps, unsigned long duration, unsigned long startdelay);

        void (*PinWrite)(const int, const uint8_t);
        uint8_t levels;

        // pins for colors
        uint8_t r_pin, g_pin, b_pin, w_pin;

        // saves current state (color)
        uint8_t curr_r, curr_g, curr_b, curr_w;

        // saves last non-zero state (color)
        uint8_t last_r, last_g, last_b, last_w;
        const uint8_t default_r, default_g, default_b, default_w;

        // HSI based on RGBW
        uint16_t Hue;
        uint8_t Sat, Bri;
        boolean HSBuptodate;

        uint8_t _common_cathode;

        uint32_t fadestarttime;
        uint32_t fadedelay;
        boolean fading;
        uint16_t fadesteps, currentfadestep, lastfadestep;
        uint8_t requested_r, requested_g, requested_b, requested_w;
        uint8_t start_r, start_g, start_b, start_w;
        int32_t step_r, step_g, step_b, step_w;
};
#endif
