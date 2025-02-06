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

#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "LaunchPad.h"
#include "clock.h"
#include "uart.h"
#include "lcd1602.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab10_part1();

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

    UART_init(115200);
    
    run_lab10_part1();

    // Endless loop to prevent program from ending
    while (1);

} /* main */

void run_lab10_part1()
{
    bool done = false;
    char buffer[16];
    char character;
    uint8_t idx = 0;
    while(done != true)
    {
        character = UART_in_char();
        UART_out_char(character);
        if(character == '\r')
        {
            character = '\0';
            buffer[idx++] = character;
            done = true;
        }
        else  if (character == '\b') 
            {
                //UART_out_char(character);
                if(idx > 0)
                    --idx;
            }
        else {
         buffer[idx++] = character;
            }

                
    }
    lcd_write_string("Name:" );
     lcd_write_string(buffer);
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("program stopped");
    
}
