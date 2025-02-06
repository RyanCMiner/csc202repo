//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ryan Miner
//
//       LAB NAME:  Lab 4
//
//      FILE NAME:  lab4_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a way to control the led lights and the seven segment 
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
#include "clock.h"
#include "LaunchPad.h"



//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab4_part2();
void run_lab4_part3();
void run_lab4_part4();
void run_lab4_part5();
void run_lab4_part6();
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
    led_init();

    led_enable();
    leds_on(0x3C);
    
    msec_delay(0x3E8);
    run_lab4_part2();
    msec_delay(0x1F4);
   run_lab4_part3();
    seg7_init();

    led_disable();
    run_lab4_part4();
    msec_delay(0x1F4);
  
    run_lab4_part5();
     msec_delay(0x1F4);
    run_lab4_part6();
    seg7_off();
    // Endless loop to prevent program from ending
    while (1);

} /* main */
// This function goes up down the led in a sequence
void run_lab4_part2()
{
    leds_off();
    int count = 0;
  
    while(count < 5)
    {
        for(uint8_t led_idx = LED_BAR_LD0_IDX; led_idx <= LED_BAR_LD7_IDX; ++led_idx )
        {
            led_on(led_idx);
            msec_delay(200);
            led_off(led_idx);
            
        }
        for (uint8_t led_idx = LED_BAR_LD6_IDX; led_idx > 1; --led_idx) 
        {
            led_on(led_idx);
            msec_delay(200);
            led_off(led_idx);
           
        }
     
        ++count;
    }
    led_on(LED_BAR_LD0_IDX);
     msec_delay(200);
    led_off(LED_BAR_LD0_IDX);
    
}
// this functions displays the LEDS in sequece order by counter through each light though binnary. 
void run_lab4_part3()
{
    int counter = 0;
    uint8_t led_idx = 0x0;
    while(counter < 2)
    {
        if(led_idx == 0xff)
            ++counter;
        leds_on(led_idx++);
        msec_delay(100);

    }
}
// this function displays L on DIG0
void run_lab4_part4()
{
    seg7_on(0x38,SEG7_DIG0_ENABLE_IDX);
}
// This function display the number 4 on DIG2 for 3 seconds and off for 2 seconds. And does it four times 
void run_lab4_part5()
{
    int count = 0;
    while(count < 4)
    {
        seg7_on(0x66,SEG7_DIG2_ENABLE_IDX);
        msec_delay(3000);
        seg7_off();
        msec_delay(2000);
        count++;
    }
    
}
// This function display cafe until the counter reaches 200
void run_lab4_part6()
{
    int counter = 0;
    while (counter < 200) 
    {
        seg7_on(0x39, SEG7_DIG0_ENABLE_IDX);
         msec_delay(2);
        seg7_on(0x77, SEG7_DIG1_ENABLE_IDX);
         msec_delay(2);
        seg7_on(0x71, SEG7_DIG2_ENABLE_IDX);
         msec_delay(2);
        seg7_on(0x79, SEG7_DIG3_ENABLE_IDX);
         msec_delay(2);
         ++counter;
    }
   
}