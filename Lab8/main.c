/*
 * mail.c: Starter code for Lab 7, Fall 2018
 *
 * Created by Zhao Zhang
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "launchpad.h"
#include "pwmbuzzer.h"
#include "ranger.h"


// Buzzer-related constants
#define LED_PWM_PERIOD  30
#define LED_MAX_PULSE_WIDTH 5000

#define BUZZER_CHECK_INTERVAL 10
#define BUZZER_ON_TIME  30
#define BUZZER_OFF_TIME (3000 - BUZZER_ON_TIME)
#define BUZZER_MAX_PERIOD ((uint32_t) (50000000 / 261.63))
#define BUZZER_MAX_PULSE_WIDTH 50000
#define BUZZER_MIN_PERIOD ((uint32_t) (50000000 / 525.25))

/*
 * The task for playing the buzzer.
 *
 * The buzzer state and callback function. The buzzer system can be in four states:
 *   Off: The system is turned off. The buzzer is silent.
 *   On: The buzzer system is turned on. The buzzer will buzz periodically.
 *   SwitchOn: The buzzer system is to be turned on.
 *   SwitchOff: The buzzer system is to be turned off.
 */

// The buzzer object
typedef struct{
    enum
    {
        Off, On, SwitchOff, SwitchOn
    } state;            // the running state of the buzzer system
    bool buzzing;       // if the buzzer is buzzing
    uint32_t timeLeft;   // the time left for the buzzer to buzz or not buzz
    int pwmPulseWidth;
    int pwmPeriod;
} buzzer_t;

static volatile buzzer_t buzzer = { On, false, 0, BUZZER_MAX_PERIOD, 0 };

void buzzerPlay(uint32_t time){
    uint32_t delay = BUZZER_CHECK_INTERVAL;     // the delay for next callback

    switch (buzzer.state)
    {
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

void checkRange(uint32_t xDistance){

    xDistance = rangerDetect();

    uprintf("%d\n", xDistance);

    schdCallback(checkRange, 100);
}

void main(void){

    uprintf("uegasyfg");
    lpInit();
    buzzerInit();
    rangerInit();

    schdCallback(buzzerPlay, 1000);
    schdCallback(checkRange, 1005);

    while (true)
    {
        schdExecute();
    }
}




