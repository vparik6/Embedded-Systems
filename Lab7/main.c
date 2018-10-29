/*
 * mail.c: Starter code for Lab 7, Fall 2018
 *
 * Created by Zhao Zhang
 */

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "launchpad.h"
#include "rotary.h"
#include "pwmled.h"
#include "pwmbuzzer.h"

// LED-related constant
#define LED_PWM_PERIOD          5000
#define LED_MAX_PULSE_WIDTH     40

/*
 * Task 1: Play a sine pattern on LED
 */

typedef struct
{
    int pwmPeriod;              // PWM period for LED
    int maxPulseWidth;          // maximum pulse width
} LedState_t;

static LedState_t led =
        { LED_PWM_PERIOD /* PWM Period */, LED_MAX_PULSE_WIDTH /* Multiply factor */};

// a sine function that uses degree as input
static inline double sine(unsigned int degree)
{
    double radian = 2 * M_PI * ((double) (degree % 360) / 360);
    return sin(radian);
}

// LED playing callback function
void ledPlay(uint32_t time)
{
    static unsigned int angle = 0; // the degree of angle, used for calculating sine value
    int delay = 5;                    // the callback delay
   int degrees = 0;
       degrees = adcVal();

    // Calculate PWM parameters for red, blue, and green sub-LEDs using sine function.
    // Use phase shift of 60, 30, and 0 degrees for red, blue, and green
    pwm_t red, blue, green;

     if(degrees >= 0 && degrees < 800){
            led.maxPulseWidth = 40;
            red.pulseWidth = sine(angle + 60) * led.maxPulseWidth;
            blue.pulseWidth = sine(angle + 30) * led.maxPulseWidth;
            green.pulseWidth = sine(angle) * led.maxPulseWidth;
            red.period = green.period = blue.period = led.pwmPeriod;
       }

     if(degrees > 800 && degrees <= 1600){
             led.maxPulseWidth = 40;
             red.pulseWidth = sine(angle + 60) * led.maxPulseWidth + 100;
             blue.pulseWidth = sine(angle + 30) * led.maxPulseWidth + 100;
             green.pulseWidth = sine(angle) * led.maxPulseWidth + 100;
             red.period = green.period = blue.period = led.pwmPeriod;
       }

     if(degrees > 1600 && degrees <= 2400){
             led.maxPulseWidth = 40;
             red.pulseWidth = sine(angle + 60) * led.maxPulseWidth + 200;
             blue.pulseWidth = sine(angle + 30) * led.maxPulseWidth + 200;
             green.pulseWidth = sine(angle) * led.maxPulseWidth + 200;
             red.period = green.period = blue.period = led.pwmPeriod;
       }

     if(degrees > 2400 && degrees <= 3200){
              led.maxPulseWidth = 40;
              red.pulseWidth = sine(angle + 60) * led.maxPulseWidth + 300;
              blue.pulseWidth = sine(angle + 30) * led.maxPulseWidth + 300;
              green.pulseWidth = sine(angle) * led.maxPulseWidth + 300;
              red.period = green.period = blue.period = led.pwmPeriod;
        }
     if(degrees > 3200 && degrees <= 4096){
               led.maxPulseWidth = 40;
               red.pulseWidth = sine(angle + 60) * led.maxPulseWidth + 400;
               blue.pulseWidth = sine(angle + 30) * led.maxPulseWidth + 400;
               green.pulseWidth = sine(angle) * led.maxPulseWidth + 400;
               red.period = green.period = blue.period = led.pwmPeriod;
        }

    // Set the PWM parameters for LED
    ledPwmSet(red, blue, green);

    // Advance the angle by one degree, so a play period is 360 * 5 = 1800 ms
    angle++;

    // Schedule the next callback
    schdCallback(ledPlay, time + delay);
}

void main(void)

{
    lpInit();
    ledPwmInit();
    adcInit();
    //adcVal();

    // Schedule the first callback events for LED flashing and push button checking.
    // Those trigger callback chains. The time unit is millisecond.
    schdCallback(ledPlay, 1002);

    // Loop forever
    while (true)
    {
        schdExecute();
    }
}



