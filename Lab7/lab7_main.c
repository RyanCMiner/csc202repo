//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ryan Miner
//
//       LAB NAME:  lab7
//
//      FILE NAME:  lab7_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as  more ways that the lcd can be used and SysTick interupts 
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
void run_lab7_part1();
void run_lab7_part2();
void lcd_string_parser(const char *string, uint8_t start_lcd_addr,uint8_t max_lcd_addr);
//---------------------------------------------------------------------
// Define symbolic constants used by the program
//---------------------------------------------------------------------
#define NUM_STATES                   18

//---------------------------------------------------------------------
// Define global variables and structures here.
//---------------------------------------------------------------------

// Defines the seven segment display for SOS
const uint8_t seg7_letter_code[] =
{
    0x6D, 0x00, 0x6D, 0x00, 0x6D, 0x00, // S
    0x3F, 0x00, 0x3F, 0x00, 0x3F, 0x00, // O
    0x6D, 0x00, 0x6D, 0x00, 0x6D, 0x00, // S
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
    
    sys_tick_init(SYST_TICK_PERIOD_COUNT);
    lcd_clear();

    run_lab7_part1();
    
    while(is_pb_up(PB2_IDX ));
    msec_delay(10);
    
    lcd_clear();
    lcd_write_string("Running part 2");
    msec_delay(1000);
    
    while(is_pb_down(PB2_IDX))
    msec_delay(5);
    lcd_clear();
    
    run_lab7_part2();
    seg7_disable();
    lcd_clear();
    // Endless loop to prevent program from ending
    while (1);

} /* main */
// this function display a measage by scolling from right to left until the user presses pb1 
void run_lab7_part1()
{
    
    uint8_t address;
    uint8_t idx = 0;
    char msg[] = "Microcontrollers are fun.";
    bool done = false;
    while(is_pb_up(PB1_IDX) && done == false)
    {
        for(address = 0x4F; address > 0x40 && done == false; --address)
        {
            if(is_pb_down(PB1_IDX))
            {
                done = true;
                msec_delay(20);
            }
            lcd_set_ddram_addr(address);
            lcd_write_string(msg);
            msec_delay(100);
            lcd_clear();
        }
        idx = 0;
        while(msg[idx] != '\0' && done == false)
        {
             if(is_pb_down(PB1_IDX))
            {
                done = true;
                msec_delay(20);
            }
            lcd_set_ddram_addr(address);
            lcd_write_string(msg + idx);
            msec_delay(100);
            lcd_clear();
            
            idx++;
        }
        idx = 0;
    }
    lcd_clear();
    lcd_write_string("part 1 done");
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("Press pb2");
    msec_delay(1000);
    
    
   
}
// this function display a measage by scolling from right to left using a differant function that breaks the measage down so can fit on the botton lcd
//And does not stop until the user presses pb1 
void run_lab7_part2()
{
    
    uint8_t address;
    uint8_t idx = 0;
    char msg[] = "Microcontrollers are fun. I love programming in MSPM0+ assembly code!!!";
    bool done = false;
    while(is_pb_up(PB1_IDX) && done == false)
    {
        for(address = 0x4F; address > 0x40 && done == false; --address)
        {
            if(is_pb_down(PB1_IDX))
            {
                done = true;
                msec_delay(20);
            }
            lcd_set_ddram_addr(address);
           lcd_string_parser(msg,address,0x4f);
            msec_delay(100);
            lcd_clear();
        }
        idx = 0;
        while(msg[idx] != '\0' && done == false)
        {
             if(is_pb_down(PB1_IDX))
            {
                done = true;
                msec_delay(20);
            }
            lcd_set_ddram_addr(address);
             lcd_string_parser(msg,0x40,0x4f);
            msec_delay(100);
            lcd_clear();
            
            idx++;
        }
        idx = 0;
    }
    lcd_clear();
    lcd_write_string("part 2 done");
    lcd_set_ddram_addr(LCD_LINE2_ADDR);

    msec_delay(1000);
}
// this function breaks down the message so can fit on the lcd without wrapping around the lcd.
void lcd_string_parser(const char *string, uint8_t start_lcd_addr,uint8_t max_lcd_addr)
{
    uint8_t max = max_lcd_addr -start_lcd_addr;
    uint8_t count = 0;
      // for each character in string, write it to the LCD module
    while ((*string != '\0') && (count <= max) )
    {
        lcd_write_char(*string++);
        ++count;
    } /* while */
}
//------------------------------------------------------------------------------
// DESCRIPTION:
// This function represents the ISR (Interrupt Service Routine) for
// the SysTick timer. It is called at regular intervals based on the
// configured SysTick period. This ISR is responsible for managing the
// timing and display of the Morse code for "SOS" on the seven-segment
// display. It controls the blink timing for the letters, with quick
// blinks for "S" (dot-dot-dot) and slower blinks for "O" (dash-dash-dash).
// The sequence of letters is repeated indefinitely.
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

    delay_time--;
    if (delay_time == 0)
    {
        // delay time has expired so now move on to next letter to display
        seg7_on(seg7_letter_code[code_index], SEG7_DIG0_ENABLE_IDX);

        // get next delay time
        delay_time = delay_count[code_index];
        code_index++;

        if (code_index == NUM_STATES)
        {
            // we went through SOS message so restart index
            code_index = 0;
        } /* if */
    } /* if */
} /* SysTick_Handler */


