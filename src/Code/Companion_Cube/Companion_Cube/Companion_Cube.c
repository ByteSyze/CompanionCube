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

#define LED_LEFT		PB2 /////////////////////////////////////2
#define LED_FRONT		PB1 // All of the different LED pins,  //1
#define LED_RIGHT		PB4 // corresponding to the PCB layout.//4
#define LED_BACK		PB3 /////////////////////////////////////3

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>

unsigned char counter = 6;	//Extra counter to increment.

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
	
	TCCR0B	|= (0x01 << CS02) | (0x01 << CS00);	// Set prescaler to 1024; start clock.
	TIMSK0	|= (0x01 << TOIE0);					//Set timer interrupt register to go off when timer overflows.
	sei();										//Enable interrupts
	
    while(1)
    {
		
    }
}

/**Timer overflow interrupt. Called whenever the timer hits its maximum and resets.*/
ISR(TIM0_OVF_vect)
{
	counter++;
	if(counter % 2 == 0)
	{
		PORTB = 0x00; //Turn off LEDs.
	}
	else
	{
		display_number(counter/2);
		if(counter >= 8)
		{
			counter = 2;
		}
	}
}
