/*
 * main.c
 *
 *  Created on: Aug 23, 2016
 *      Author: zzhang
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <driverlib/sysctl.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_i2c.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/i2c.h>
#include "launchpad.h"
#include "seg7.h"


//test test

// 7-segment coding table. See https://en.wikipedia.org/wiki/Seven-segment_display. The segments
// are named as A, B, C, D, E, F, G. In this coding table, segments A-G are mapped to bits 0-7.
// Bit 7 is not used in the coding. This display uses active high signal, in which '1' turns ON a
// segment, and '0' turns OFF a segment.
static uint8_t seg7Coding[10] = {
		0b00111111, 		// digit 0
		0b00000110, 		// digit 1
		0b01011011,			// digit 2
		0b01001111,			// digit 3
		0b01100110,         // digit 4
		0b01101101,         // digit 5
		0b01111101,         // digit 6
		0b00000111,         // digit 7
		0b01111111,         // digit 8
		0b01101111,         // digit 9
		// MORE CODINGS
};

// The colon status: if colon == 0b10000000, then the colon is on,
// otherwise it is off.
static uint8_t colon = 0;

int s1 = 0;	//seconds on the right
int s2 = 0;	//seconds on the left
int m1 = 0; //minutes on the right
int m2 = 0; //minutes on the left


// Update the clock display
void clockUpdate(uint32_t time)								// pointer to a 4-byte array
{
	uint8_t code[4];									// The 7-segment code for the four clock digits

	// Calculate the display digits and colon setting for the next update

	// Display 01:23 on the 7-segment displays
	// The colon ':' will flash on and off every 0.5 seconds

	code[0] = seg7Coding[s1] + colon;
	code[1] = seg7Coding[s2] + colon;
	code[2] = seg7Coding[m1] + colon;
	code[3] = seg7Coding[m2] + colon;
	seg7Update(code);

	if (colon == 0b00000000) {
		colon = 0b10000000;
	}
	else{
	    colon = 0b00000000;

//Because this update runs every .5 se . conds, sticking the incrementer in the else statement;
//the code will run every second
	    s1++;
	    if (s1 == 10) {
	        s1 = 0;
	        s2++;
	    }
	    if(s2 == 6) {
	        s2 = 0;
	        m1++;
	    }
	    if(m1 == 10) {
	        m1 = 0;
	        m2++;
	    }
	    if(m2 == 6) {
	        s1=0;
	        s2=0;
	        m1=0;
	        m2=0;
	    }
	}

	// Call back after .5 second
	schdCallback(clockUpdate, time + 500);
}

void checkPushButton(uint32_t time)
{
	// Read the pushbutton state; see pbRead() in launchpad.h
	int code = pbRead();
	uint32_t delay = 10;

	switch (code) {
	case 1:
		uprintf("%s\n\r", "SW1 is pushed");
		// Use an inertia for soft de-bouncing
		delay = 250;

		m1++;
		if (m1 == 10) {
			m1 = 0;
			m2++;
		}
		if (m2 == 6) {
			s1 = 0;
			s2 = 0;
			m1 = 0;
			m2 = 0;
		}
		break;

	case 2:
		// Pring out a message
		// See uprintf() in launchpad.h
		uprintf("%s\n\r", "SW2 is pushed");
		// Use an inertia for soft de-bouncing
		delay = 250;
		
		s1++;
		if (s1 == 10) {
			s1 = 0;
			s2++;
		}
		if (s2 == 6) {
			s2 = 0;
			m1++;
		}
		break;
	}

	schdCallback(checkPushButton, time + delay);
}

int main(void)
{
	lpInit();
	seg7Init();

	uprintf("%s\n\r", "Lab 2: Wall clock");

	// Schedule the first callback events for LED flashing and push button checking.
	// Those trigger callback chains. The time unit is millisecond.
	schdCallback(clockUpdate, 1000);
	checkPushButton(10);
	// Loop forever
	while (true) {
		schdExecute();
	}
}
