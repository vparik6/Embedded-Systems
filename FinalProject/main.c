/*
 * main.c for ECE 266 Final Project Group B4
 * Implements temperature, displays on seg7, is button controlled and shows led state
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "seg7digit.h"
#include "launchpad.h"
#include "seg7.h"
#include "temp.h"
#include "pwmbuzzer.h"
#include <math.h>
#include <led.c>
#include <driverlib/sysctl.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_i2c.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/i2c.h>


// Buzzer-related constants
#define BUZZER_CHECK_INTERVAL 10
#define BUZZER_ON_TIME  30
#define BUZZER_OFF_TIME (3000 - BUZZER_ON_TIME)
#define BUZZER_MAX_PERIOD ((uint32_t) (50000000 / 261.63))
#define BUZZER_MAX_PULSE_WIDTH 50000
#define BUZZER_MIN_PERIOD ((uint32_t) (50000000 / 525.25))


static uint8_t seg7Coding[11] = {
        0b00111111,         // digit 0
        0b00000110,         // digit 1
        0b01011011,         // digit 2
        0b01001111,         // digit 3
        0b01100110,         // digit 4
        0b01101101,         // digit 5
        0b01111101,         // digit 6
        0b00000111,         // digit 7
        0b01111111,         // digit 8
        0b01101111,         // digit 9
        0b00111001,         // letter C

};

// Update the display
enum {
    Reset, Run, Pause
}  sysState = Run;

typedef struct{
    enum
    {
        Off, On, SwitchOff, SwitchOn
    } state;            // the running state of the buzzer system
    bool buzzing;       // if the buzzer is buzzing
    int32_t timeLeft;   // the time left for the buzzer to buzz or not buzz
    int pwmPulseWidth;
    int pwmPeriod;
} buzzer_t;

static volatile buzzer_t buzzer = { Off, false, 0, BUZZER_MAX_PERIOD, 0 };

// The buzzer play callback function
void buzzerPlay(uint32_t time)
{
    uint32_t delay = BUZZER_CHECK_INTERVAL;     // the delay for next callback
    switch (buzzer.state) {
    case Off:           // the buzzer system is turned off, do nothing
        break;
    case On:            // the buzzer system is active, turn buzzer on and off
        if (buzzer.buzzing)
        {
            // If the buzzer has been buzzing for enough time, turn it off
            if ((buzzer.timeLeft -= BUZZER_CHECK_INTERVAL) <= 0)
            {
                buzzerPwmSet(0, buzzer.pwmPeriod);
                buzzer.buzzing = false;
                buzzer.timeLeft = BUZZER_OFF_TIME;
            }
        }
        else
        {
            // If the buzzer has been silent for enough time, turn it on
            if ((buzzer.timeLeft -= BUZZER_CHECK_INTERVAL) <= 0)
            {
                buzzerPwmSet(buzzer.pwmPulseWidth, buzzer.pwmPeriod);
                buzzer.buzzing = true;
                buzzer.timeLeft = BUZZER_ON_TIME;
            }
        }
        break;

    case SwitchOff:             // De-activate the buzzer system
        if (buzzer.buzzing)
        buzzerPwmSet(0, buzzer.pwmPeriod);
        buzzer.state = Off;
        buzzer.buzzing = Off;
        break;

    case SwitchOn:              // Activate the buzzer system
        buzzerPwmSet(buzzer.pwmPulseWidth, buzzer.pwmPeriod);
        buzzer.state = On;
        buzzer.buzzing = true;
        buzzer.timeLeft = BUZZER_ON_TIME;
        break;
    }
    // schedule the next callback
    schdCallback(buzzerPlay, time + delay);
}

/*
 * The task for checking push button
 */

// If the user has activated the buzzer system or not
static bool userActivated = false;

// The callback function for checking the pushbuttons
void checkPushButton2(uint32_t time)
{
    uint32_t delay = 10;        // the default delay for the next checking

    int code = pbRead();        // read the pushbutton

    switch (code)
    {
    case 1:                     // SW1: Turn on the buzzer system
        userActivated = true;
        buzzer.state = SwitchOn;
        delay = 250;
        uprintf("%s\n\r", "button is on");
        break;

    case 2:                     // SW2: Turn off the buzzer system
        userActivated = false;
        buzzer.state = SwitchOff;
        delay = 250;
        uprintf("%s\n\r", "button is off");
        break;
    }

    // schedule the next callback
    schdCallback(checkPushButton2, time + delay);
}



static uint8_t colon = 0;
int s1 = 0; //seconds on the right
int s2 = 0; //seconds on the left
int m1 = 0; //minutes on the right
int m2 = 0; //minutes on the left


void checkTemp(uint32_t time) {

    unsigned int reading = tempDetect();
    unsigned int humidity = reading >> 16;
    unsigned int temperature = reading & 0xFFFF;

    if (temperature >= 250 && temperature <= 280) {
        buzzer.timeLeft = 200;
        buzzer.pwmPulseWidth = BUZZER_MAX_PULSE_WIDTH * .8 ;
        buzzer.pwmPeriod = BUZZER_MIN_PERIOD;
    }
    if (temperature > 280 && temperature <= 300) {
        buzzer.timeLeft = 100;
        buzzer.pwmPulseWidth = BUZZER_MAX_PULSE_WIDTH * .4;
        buzzer.pwmPeriod = BUZZER_MIN_PERIOD * .5 + BUZZER_MIN_PERIOD;
    }
    if (temperature > 300) {
        buzzer.timeLeft = 25;
        buzzer.pwmPulseWidth = BUZZER_MAX_PULSE_WIDTH;
        buzzer.pwmPeriod = BUZZER_MAX_PERIOD;

    }

    uprintf("%s\n\r", "Hello World");
    uprintf("humidity is %f\n\r", (float)humidity / 10.0);
    uprintf("temp is %f\n\r", (float)temperature / 10.0);

    uprintf("%s\n\r", "Hello World 2nd");

    if (temperature > 0) {
        s2 = temperature % 10;
        temperature /= 10;
        m1 = temperature % 10;
        temperature /= 10;
        m2 = temperature % 10;
        temperature /= 10;
    }

    uprintf("temperature broken down %d %d %d \n\r", m2,m1,s2);
    schdCallback(checkTemp, time + 500);
}


void clockUpdate(uint32_t time)
{
    uint8_t code[4];

    if(sysState == Run){

         code[0] = seg7Coding[10] + colon;
         code[1] = seg7Coding[s2] + colon;
         code[2] = seg7Coding[m1] + colon;
         code[3] = seg7Coding[m2] + colon;
         colon = 0b10000000;
         seg7Update(code);
         buzzer.state = SwitchOn;
    }
    // Call back after .5 second
    schdCallback(clockUpdate, time + 500);
}

void checkPushButton(uint32_t time){

    uint32_t delay = 500;
    uint8_t code[4];                                   // The 7-segment code for the four clock digit
    int read = pbRead();

    switch (read) {

    case 1:                         // SW1 is the Reset button, only when the stopwatch is paused
        code[0] = seg7Coding[0] + colon;
        code[1] = seg7Coding[0] + colon;
        code[2] = seg7Coding[0] + colon;
        code[3] = seg7Coding[0] + colon;
        colon = 0b10000000;
        seg7Update(code);

        ledTurnOnOff(1,0,0);
        sysState = Pause;
        buzzer.state = SwitchOff;
        break;

    case 2:
        if(sysState == Pause) {
              sysState = Run;
              ledTurnOnOff(0,0,1);
              buzzer.state = SwitchOn;
        }
        else if(sysState == Run){
              sysState = Pause;
              ledTurnOnOff(0,1,0);
              buzzer.state = SwitchOff;
        }
        break;

        default:
        delay = 10;
     }

    schdCallback(checkPushButton, time + delay);
}

int main(void) {
    lpInit();
    seg7Init();
    tempInit();
    buzzerInit();
    ledInit();
    seg7Init();

    schdCallback(checkTemp, 1000);
    schdCallback(clockUpdate, 1000);
    schdCallback(checkPushButton, 1005);
    schdCallback(checkPushButton2, 1005);
    schdCallback(buzzerPlay, 1005);

    // Run the event scheduler to process callback events
    while (true) {
        schdExecute();
    }
}
