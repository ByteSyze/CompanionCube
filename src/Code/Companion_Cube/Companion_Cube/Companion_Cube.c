/**
 * Companion_Cube.c
 *
 * Created: 12/11/2014 5:42:15 AM
 * Author: Tyler Hackett
 *
 * This project is designed for the purpose of the "Blinky Companion Cube" project, which can be found here: 
 * https://github.com/Insomniac10102/3D-LED-Companion-Cube
 *
 */ 

#define PIN_OUT			0x01
#define PIN_IN			0x00

#define LED_LEFT		PB2 /////////////////////////////////////
#define LED_FRONT		PB1 // All of the different LED pins,  //
#define LED_RIGHT		PB4 // corresponding to the PCB layout.//
#define LED_BACK		PB3 /////////////////////////////////////

#define STATE_STARTUP	1	//Initialization state.
#define STATE_FLASH		2	//LED flashing state.
#define STATE_SLEEP		3	//Sleeping state.

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>

unsigned char counter	= 0;	//Extra counter to increment.
unsigned char number	= 3;	//Number to display.

unsigned char button_val = 1;
unsigned char last_button_val = 1;
unsigned char button_hold_time = 0;

unsigned char state		= STATE_SLEEP; //Current state.

char get_pin(char pin)
{
	_NOP(); // First execute a no-operation, as recommended in the datasheet.
	return PINB >> pin & 0x01;
}

void set_pin(char pin, char val)
{
	if(val & 0x01)
		PORTB |= (val & 0x01) << pin;
	else
		PORTB = ~PORTB & (val & 0x01) << pin;
}

void toggle_pin(char pin)
{
	PORTB ^= 1 << PB1;
}

/**
* Sets the I/O direction of the specified pin. 1 for output, 0 for input.
*/
void set_pin_dir(char pin, char dir)
{
	DDRB |= (dir & 0x01) << pin;
}

void display_number(unsigned char num)
{
	if(num == 1)
		PORTB = 1 << LED_FRONT;
	else if(num == 2)
		PORTB = (0x1 << LED_RIGHT) | (0x1 << LED_LEFT);
	else if(num == 3)
		PORTB = (0x1 << LED_RIGHT) | (0x1 << LED_LEFT) | (0x1 << LED_BACK);
	else
		PORTB = 0x1E; //All 4 LEDs.
}

int main(void)
{
	set_pin_dir(0, PIN_IN);						// PB0 is connected to button.
	set_pin_dir(1, PIN_OUT);					// PB1-4 are LEDs.
	set_pin_dir(2, PIN_OUT);
	set_pin_dir(3, PIN_OUT);
	set_pin_dir(4, PIN_OUT);
	
	TCCR0B	|= (0x01 << CS02); //| (0x01 << CS00);	// Set prescaler to 1024; start clock.
	TIMSK0	|= (0x01 << TOIE0);					//Set timer interrupt register to go off when timer overflows.
	sei();										//Enable interrupts
	
	while(1);
}

/**Timer overflow interrupt. Called whenever the timer hits its maximum and resets.*/
ISR(TIM0_OVF_vect)
{
	last_button_val = button_val;
	button_val = (PINB >> PB0) & 0x01;
	
	if(button_val == 0)
	{
		if(last_button_val == button_val)
			button_hold_time++;
		else
			button_hold_time=0;
	}
	if(button_hold_time == 5)
	{
		if(state == STATE_SLEEP)
		{
			TCCR0B	&= ~(1 << CS00); 
			state = STATE_STARTUP;
		}
		else if(state == STATE_FLASH)
			state = STATE_SLEEP;
	}
	
	if(state == STATE_STARTUP)
	{
		if(counter % 2)
		{
			if(number % 4 == 0)
				PORTB = 0x01 << LED_RIGHT;
			else if(number % 3 == 0)
				PORTB = 0x01 << LED_BACK;
			else if(number % 2 == 0)
				PORTB = 0x01 << LED_LEFT;
			else if(number % 1 == 0)
				PORTB = 0x01 << LED_FRONT;
			
			if(++number > 16)
			{
				TCCR0B	|= (0x01 << CS02) | (0x01 << CS00);
				state = STATE_FLASH;
				number = 3;
				counter = 0;
				return;
			}
		}
		counter++;
	}	
	else if(state == STATE_FLASH)
	{
		if(counter % 2 == 0)
		{
			PORTB = 0x00; //Turn off LEDs.
		}
		else
		{
			if(number == 3 && counter < 8)
			{
				counter++;
				return; //Add some delay after 2.
			}
		
			display_number(number);
		
			if(++number > 4)
			{
				counter = 0;
				number	= 1;
				return;
			}
		}
		counter++;
	}
	else //Assume sleep
	{
		PORTB = 0x00;
	}
}
