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

#define PIN_OUT		0x01
#define PIN_IN		0x00

#define LED_LEFT		0x02 /////////////////////////////////////
#define LED_FRONT		0x01 // All of the different LED pins,	//
#define LED_RIGHT		0x04 // corresponding to the PCB layout.//
#define LED_BACK		0x03 /////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>

unsigned char counter;	//Extra counter to increment.
unsigned char num;		//Number to flash on LEDs.

char get_pin(char pin)
{
	_NOP(); // First execute a no-operation, as recommended in the datasheet.
	return PINB >> pin & 0x01;
}

void set_pin(char pin, char val)
{
	PORTB |= (val & 0x01) << pin;
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
		PORTB = LED_FRONT;
	else if(num == 2)
		PORTB = (0x1 << LED_RIGHT) | (0x1 << LED_LEFT);
	else if(num == 3)
		PORTB = (0x1 << LED_RIGHT) | (0x1 << LED_LEFT) | (0x1 << LED_BACK);
	else
		PORTB = 0x0F; //All 4 LEDs.
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

ISR(TIM0_OVF_vect)
{
	counter++;
	
	if(counter > 5)
	{
		PORTB ^= 1 << PB1;
		counter = 0;
	}
}