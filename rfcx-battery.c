/**********************************************************
*	RFCx Microcontroller Software - Battery
*
*	Kalee Stutzman 	(stutzmak@mail.gvsu.edu)
*	Joe Gibson		(gibsjose@mail.gvsu.edu)
*
*	08 July 2015
*
*   www.rfcx.org
**********************************************************/

#include "rfcx-battery.h"

//Declare pins as inputs/outputs
void rfcx_batteries_init(void) {
    //Inputs
    DDRC &= ~_BV(BAT_1_INPUT_PIN);
    DDRC &= ~_BV(BAT_2_INPUT_PIN);

    //Outputs
    DDRC |= _BV(BAT_1_OUTPUT_PIN);
    DDRC |= _BV(BAT_2_OUTPUT_PIN);

    //Initialize outputs low
    PORTC &= ~_BV(BAT_1_OUTPUT_PIN);
    PORTC &= ~_BV(BAT_2_OUTPUT_PIN);
}

void rfcx_batteries_status(batteries_t * batteries) {
    batteries->battery_1.status = rfcx_battery_status(BATTERY_1);
    batteries->battery_2.status = rfcx_battery_status(BATTERY_2);
}

unsigned char rfcx_battery_status(unsigned char id) {
    unsigned char status = 0x00;
    unsigned char first = 0x00;
    unsigned char second = 0x00;

    unsigned char input_pin = 0x00;
    unsigned char output_pin = 0x00;

    //Battery 1
    if(id == BATTERY_1) {
        input_pin = BAT_1_INPUT_PIN;
        output_pin = BAT_1_OUTPUT_PIN;
    }

    //Battery 2
    else if(id == BATTERY_2){
        input_pin = BAT_2_INPUT_PIN;
        output_pin = BAT_2_OUTPUT_PIN;
    }

    //This should never happen...
    else {
        return TEMPERATURE_FAULT;
    }

    //Read input pin initially
    first = (PINC & _BV(input_pin));

    //Toggle output pin
    PORTC ^= _BV(output_pin);

    //Read again
    second = (PINC & _BV(input_pin));

    //Toggle output back
    PORTC ^= _BV(output_pin);

    //If pin status is unchanged it is in High Z mode (sleep mode/temp fault)
    if(first == second) {
        return SLEEP_MODE;
    }

    //Was high (charging)
    else {
        if(first) {
            return CHARGING;
        } else {
            return CHARGE_COMPLETE;
        }
    }
}
