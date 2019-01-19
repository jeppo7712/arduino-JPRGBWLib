
#ifndef JPRGBWLib_H
#define JPRGBWLib_H

#include <Arduino.h>

//#define RGBWTOOL_DEBUG

enum Mode { COMMON_ANODE, COMMON_CATHODE };

class JPRGBWLib
{
public:
        JPRGBWLib(uint8_t r, uint8_t g, uint8_t b, uint8_t w, void (*pinset)(const int, const uint8_t), uint16_t PWMlevels,
                  uint8_t _default_r,  uint8_t _default_g,  uint8_t _default_b,  uint8_t _default_w, uint8_t *_converttable);

        void loop(bool enable);

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
        void hsb2rgbw_v2(uint16_t hue, uint8_t sat, uint8_t bri, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w);
        void rgbw2hsb(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t *hue, uint8_t *sat, uint8_t *bri);

private:
        void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
        void fadeTo(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t steps, unsigned long duration, unsigned long startdelay);

        void (*PinWrite)(const int, const uint8_t);
        uint8_t levels;

        // pins for colors
        uint8_t r_pin;
        uint8_t g_pin;
        uint8_t b_pin;
        uint8_t w_pin;

        // saves current state (color)
        uint8_t curr_r;
        uint8_t curr_g;
        uint8_t curr_b;
        uint8_t curr_w;

        // saves last non-zero state (color)
        uint8_t last_r;
        uint8_t last_g;
        uint8_t last_b;
        uint8_t last_w;

        const uint8_t default_r;
        const uint8_t default_g;
        const uint8_t default_b;
        const uint8_t default_w;

        // HSI based on RGBW
        uint16_t Hue;
        uint8_t Sat;
        uint8_t Bri;
        boolean HSBuptodate;

        uint32_t fadestarttime;
        uint32_t fadedelay;
        boolean fading;
        uint16_t fadesteps, currentfadestep, lastfadestep;
        uint8_t requested_r;
        uint8_t requested_g;
        uint8_t requested_b;
        uint8_t requested_w;
        uint8_t start_r;
        uint8_t start_g;
        uint8_t start_b;
        uint8_t start_w;
        int32_t step_r;
        int32_t step_g;
        int32_t step_b;
        int32_t step_w;

        uint32_t fadelastchecktime;

        uint8_t *converttable;
};
#endif
