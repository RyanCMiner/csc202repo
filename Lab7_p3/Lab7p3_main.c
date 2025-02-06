//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ryan Miner
//
//       LAB NAME:  lab7 part 3
//
//      FILE NAME:  lab7p3_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a way to demonstrate SysTick interrupts using the dip switch
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------



#include <stdbool.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "LaunchPad.h"
#include "clock.h"
#include "lcd1602.h"


//---------------------------------------------------------------------
// Define function prototypes used by the program
//---------------------------------------------------------------------
#define MSPMO_CLOCK_FREQUENCY        (40E6)
#define SYST_TICK_PERIOD             (10.25E-3)
#define SYST_TICK_PERIOD_COUNT       (SYST_TICK_PERIOD * MSPMO_CLOCK_FREQUENCY)
void run_lab7_part3();

//---------------------------------------------------------------------
// Define symbolic constants used by the program
//---------------------------------------------------------------------
#define NUM_STATES                   18

//---------------------------------------------------------------------
// Define global variables and structures here.
//---------------------------------------------------------------------

// Defines the seven segment display for all dips combination
const uint8_t seg7_number_code[] =
{
    0x00,0x06, // 1
    0x06, 0x5B,// 2
    0x06, 0x5B, 0x5B, 0x4F, // 3
    0x06, 0x5B,0x5B, 0x4F, 0x5B, 0x4F,0x4F,0x66// 4
};

// These timings for morse code dots and dashes can vary slightly based on
// different implementations, but the 1:3 ratio for dots:dashes is a
// typical ratio for Morse code. The delay times in terms of number of
// Systick interrupts:
const uint8_t delay_count[] =
{
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x18, // dots
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24, // dash
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, // dots
};

int main(void)
{
    clock_init_40mhz();
    launchpad_gpio_init();
    
    I2C_init();
    lcd1602_init();
    lcd_set_display_on();
    
    led_init();
    led_disable();
    
    seg7_init();
    
    dipsw_init();
    
    lpsw_init();
    
    sys_tick_init(SYST_TICK_PERIOD_COUNT);
    
    lcd_clear();

    run_lab7_part3();
 // Endless loop to prevent program from ending
 while (1);

} /* main */
// A function that count from 0 to 99 and display it on the lcd.
void run_lab7_part3()
{
    bool done = false; 
    while(is_lpsw_up(LP_SW1_IDX) || done == true)
    {
        uint8_t count = 0;
        lcd_set_ddram_addr(6);
        while (count != 99) 
        {
            if(is_lpsw_down(LP_SW1_IDX))
            {
                done = true;
            }
            lcd_write_byte(count++);
            if(is_lpsw_down(LP_SW1_IDX))
            {
                count = 0;
                msec_delay(10);
            }
            msec_delay(200);
            lcd_clear();
            lcd_set_ddram_addr(6);
        }
    }
}
//------------------------------------------------------------------------------
// DESCRIPTION:
// This function represents the ISR (Interrupt Service Routine) for
// the SysTick timer. It is called at regular intervals based on the
// configured SysTick period. This ISR is responsible for managing the
// timing and display the dip swich on how many swiches are up on the lcd
//
// INPUT PARAMETERS:
// none
//
// OUTPUT PARAMETERS:
// none
//
// RETURN:
// none
//------------------------------------------------------------------------------
void SysTick_Handler(void)
{
    static uint16_t delay_time = 1;
    static uint16_t code_index = 0;
    uint8_t bytes = 0;
    delay_time--;
    if (delay_time == 0)
    {
        // delay time has expired so now move on to next number to display
          bytes = (dipsw_read());
        seg7_on(seg7_number_code[bytes], SEG7_DIG0_ENABLE_IDX);

        // get next delay time
        delay_time = delay_count[code_index];
        code_index++;

        if (code_index == NUM_STATES)
        {
            code_index = 0;
        } /* if */
    } /* if */
} /* SysTick_Handler */




