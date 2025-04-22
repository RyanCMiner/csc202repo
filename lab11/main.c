//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  TBD
//
//       LAB NAME:  TBD
//
//      FILE NAME:  main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a ... 
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <cstdint>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "LaunchPad.h"
#include "uart.h"
#include "lcd1602.h"
#include "adc.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab11();
void printr(const char *string);
char UART_SELECT();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------


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
  
 
    // Endless loop to prevent program from ending
    while (1);

} /* main */
// This function creates a menu for the user using the printr function.
// The user can choose from 4 different options  
void run_lab11()
{
    bool done = false;
    char input; 
    
    while(done != true)
    {
        lcd_write_string("Program running");
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        lcd_write_string("See serial port");

        printr("\nMenu options:");
        printr("\n\t1. Set data to send");
        printr("\n\t2. Send Data");
        printr("\n\t3. Update LEDS");
        printr("\n\t4. End program");
        printr("\nEnter your selection:");
       input = UART_SELECT();
       switch (input) 
       {
            case '1':
                printr("\nSet SPI xmit data menu selected\n\r");
                
                break;
            case '2':
                printr("\nSent Data\n\r");
                
                break;
            case '3':
                printr("\nUpdated LEDs\n\r");
            
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
void get_spi_data()
{

}
uint16_t string_to_uint16t(const char *string)
{

}

