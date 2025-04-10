//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ryan Miner
//
//       LAB NAME:  Lab 10 part 1
//
//      FILE NAME:  lab10p2_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program will allow the user to pick from 4 different options using the UART. 
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------


#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------

#include <ti/devices/msp/msp.h>
#include "LaunchPad.h"
#include "clock.h"
#include "uart.h"
#include "lcd1602.h"
#include "adc.h"
//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab10_part2();
void printr(const char *string);
char UART_SELECT();
void display_temp();
void inc_seg7( bool enable);
void flash_leds();
//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define ADC_CHANNEL5 (5)
#define CIRCLE  (0xdf)
#define ALL_LEDS (0xFFFF)
//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
const uint8_t seg7_number_code[] =
{
    0x3F, 0x06, 0x5B, 0x4F, 
    0x66, 0x6D, 0x7D,
    0x07, 0x7F, 0x6F
};


// Define a structure to hold different data types

int main(void)
{
    clock_init_40mhz();
    launchpad_gpio_init();

    I2C_init();
    lcd1602_init();
    lcd_clear();
    UART_init(115200);
    
    led_init();
    seg7_init();
  
    ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);
 
    run_lab10_part2();

    // Endless loop to prevent program from ending
    while (1);

} /* main */
// This function creates a menu for the user using the printr function.
// The user can choose from 4 different options  
void run_lab10_part2()
{
     bool done = false;
     char input; 
    
    inc_seg7(false);
    while(done != true)
    {
        printr("\nMenu options:");
        printr("\n\t1. Increment count on 7-segment display");
        printr("\n\t2. Show current temperature");
        printr("\n\t3. Flash LEDs 3 times");
        printr("\n\t4. End program");
        printr("\nEnter your selection:");
       input = UART_SELECT();
       switch (input) 
       {
            case '1':
                printr("\nIncrement count on 7-segment display\n\r");
                inc_seg7(true);
                break;
            case '2':
                printr("\nShowing current temperature\n\r");
                display_temp();
                break;
            case '3':
                printr("\nFlash LEDs 3 times\n\r");
                led_enable();
                seg7_off();
                flash_leds();
                led_disable();
                inc_seg7(false);
                break;
            case '4':
                done = true;
                break;
            default:
             printr("\nInvalid input please try again");
       }
    }
    printr("\nThank you for using the program");
    lcd_clear();
    lcd_write_string("program stopped");
    seg7_off();
}
// this function gets the user input from the UART.
char UART_SELECT()
{
    bool done = false;
    char character = '0'; 
    char selection = '0';
    uint8_t idx = 0;
    
    while(done != true)
    {
        character = UART_in_char();
        
        if(character == '\r')
        {
            character = '\0';
            done = true;
        }
        else 
            if (character == '\b' && idx > 0) 
                --idx;
            else
            {
                ++idx;
                selection = character;
            }
                
            
            if(idx > 0)
                UART_out_char(character);
    }
    return selection;
    
}
// this function prints a string of characters to the serial console  
void printr(const char *string)
{
    while(*string != '\0')
    {
        UART_out_char(*string++);
    }
}
// This function increment count on 7-segment display
// and rests it to zero when it goes over nine
void inc_seg7(bool enable)
{
    static uint8_t idex = 0;
    
    if(enable)
    {
        if(idex > 9)
            idex = 0;
        idex++;
    }
    seg7_on(seg7_number_code[idex], SEG7_DIG0_ENABLE_IDX);
}
// This function gets the temperature of the room and 
// display it on the lcd in fahrenheit  
void display_temp()
{
    lcd_clear();
    
    uint16_t adc = ADC0_in(ADC_CHANNEL5);
    uint16_t temp = (thermistor_calc_temperature(adc) * (9.0/5)) + 32;
    
    lcd_write_string("Temp = ");
    lcd_write_byte(temp);
    lcd_write_char(CIRCLE);
    lcd_write_char('F');
} 
// this function flashes the LEDs three times
void flash_leds()
{
    for(uint8_t i = 0; i < 3; ++i)
    {
        leds_on(ALL_LEDS);
        msec_delay(500);
        leds_off();
        msec_delay(500);
    }
    
    
}
