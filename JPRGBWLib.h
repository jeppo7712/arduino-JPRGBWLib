
#ifndef JPRGBWLib_H
#define JPRGBWLib_H

#include <Arduino.h>

//#define RGBWTOOL_DEBUG

enum Mode { COMMON_ANODE, COMMON_CATHODE };

class JPRGBWLib
{
public:
        JPRGBWLib(uint8_t r, uint8_t g, uint8_t b, uint8_t w, void (*pinset)(const int, const uint8_t), uint16_t PWMlevels);
        JPRGBWLib(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t common_cathode, void (*pinset)(const int, const uint8_t), uint16_t PWMlevels);
        JPRGBWLib(uint8_t r, uint8_t g, uint8_t b, uint8_t w, Mode mode, void (*pinset)(const int, const uint8_t), uint16_t PWMlevels);

        void loop();

        void requestColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
        void requestHSI(uint16_t hue, uint8_t sat, uint8_t bri);
        void requestHue(uint16_t hue);
        void requestSat(uint8_t sat);
        void requestBri(uint8_t bri);

        void fadeToColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t steps, unsigned long duration, unsigned long startdelay);
        void fadeToHSI(uint16_t hue, uint8_t sat, uint8_t bri, uint16_t steps, unsigned long duration, unsigned long startdelay);
        void fadeToHue(uint16_t hue, uint16_t steps, unsigned long duration, unsigned long startdelay);
        void fadeToSat(uint8_t sat, uint16_t steps, unsigned long duration, unsigned long startdelay);
        void fadeToBri(uint8_t bri, uint16_t steps, unsigned long duration, unsigned long startdelay);

        void getcurrentColor(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w);
        void getrequestedColor(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w);
        void getHSI(uint16_t *hue, uint8_t *sat, uint8_t *bri); //based on requesed, i.e. final rgbw, not the current while fading
        void getHue(uint16_t *hue); //based on requesed, i.e. final rgbw, not the current while fading
        void getSat(uint8_t *sat); //based on requesed, i.e. final rgbw, not the current while fading
        void getBri(uint8_t *bri); //based on requesed, i.e. final rgbw, not the current while fading

        boolean getactivity();
        boolean getfading();

        void hsi2rgbw(uint16_t hue, uint8_t sat, uint8_t bri, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w);
        void rgbw2hsi(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t *hue, uint8_t *sat, uint8_t *bri);

private:
        void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
        void fadeTo(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t steps, unsigned long duration, unsigned long startdelay);

        void (*PinWrite)(const int, const uint8_t);
        uint16_t pwmlevels;

        // pins for colors
        uint8_t r_pin, g_pin, b_pin, w_pin;

        // saves current state (color)
        uint8_t curr_r, curr_g, curr_b, curr_w;

        // HSI based on RGBW
        uint16_t Hue;
        uint8_t Sat, Bri;
        boolean HSIuptodate;

        // output for common cathode RGBW leds (0 = dark)
        // default: common anode (0 = bright)
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
