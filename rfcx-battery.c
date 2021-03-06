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

void rfcx_batteries_data_init(batteries_t * batteries) {
    batteries->battery_1.status = BAT_STATUS_ERROR;
    batteries->battery_2.status = BAT_STATUS_ERROR;
}

void rfcx_batteries_status(batteries_t * batteries) {
    batteries->battery_1.status = rfcx_battery_status(BATTERY_1);
    batteries->battery_2.status = rfcx_battery_status(BATTERY_2);
}

unsigned char rfcx_battery_status(unsigned char id) {
    bool first = 0x00;
    bool second = 0x00;

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
        return BAT_STATUS_ERROR;
    }

    //Clear output pin LOW
    PORTC &= ~_BV(output_pin);

    //Read input pin initially
    first = (bool)(PINC & _BV(input_pin));

    //Set output pin HIGH
    PORTC |= _BV(output_pin);

    //Read again
    second = (bool)(PINC & _BV(input_pin));

    //Clear output again
    PORTC &= ~_BV(output_pin);

    //If input pin followed output (low -> high) it is in High Z mode (sleep mode/temp fault)
    if((!first) && second) {
        return SLEEP_MODE;
    }

    //Remained high both times (charging)
    else if(first && second) {
            return CHARGING;
    }

    //Remained low both times (charged)
    else if(!(first && second)) {
        return CHARGE_COMPLETE;
    }

    //Shouldn't ever get here
    return BAT_STATUS_ERROR;
}

void rfcx_battery_status_string(char * str, unsigned char status) {
	switch(status) {
		case CHARGING:
			sprintf(str, "Charging");
			break;
		case CHARGE_COMPLETE:
			sprintf(str, "Charge Complete");
			break;
		case SLEEP_MODE:
			sprintf(str, "Sleep Mode");
			break;
		//@TODO How to identify between Sleep Mode and Temp Fault?
		// case TEMPERATURE_FAULT:
		// 	sprintf(str, "Temperature Fault");
		// 	break;
		case BAT_STATUS_ERROR:
			sprintf(str, "ERROR");
		default:
			sprintf(str, "UNKNOWN");
			break;
	}
}
