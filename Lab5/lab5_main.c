//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  RYAN MINER
//
//       LAB NAME:  LAB5
//
//      FILE NAME:  lab5_main.c
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
#include "clock.h"
#include "LaunchPad.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void lab5_part1();
void lab5_part2();
void lab5_part3();
void lab5_part4();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------


// Define a structure to hold different data types
enum state 
{
    GET_LOW, 
    GET_HIGH, 
    DISPLAY
};

int main(void)
{
    clock_init_40mhz();
    launchpad_gpio_init();
    led_init();
    seg7_init();
    keypad_init();
    dipsw_init();
    lpsw_init();
  lab5_part1();
    msec_delay(500);
    lab5_part2();
    msec_delay(500);
    lab5_part3();
    msec_delay(500);
    lab5_part4();
 
 // Endless loop to prevent program from ending
 while (1);

} /* main */

/*When PB1 is pressed for the first time, the number '3' should be displayed on DIG0 of the seven-segment display.
The display of '3' should remain active until PB1 is pressed a second time. On the second press, the display should turn off.
You must press PB1 again to make the '3' reappear.*/
void lab5_part1()
{
    uint8_t loop_count = 0;
    while(loop_count < 3)
    {
        while(is_pb_up(PB1_IDX ));
        while(is_pb_down(PB1_IDX ))
        {
            seg7_on(0x4F, SEG7_DIG0_ENABLE_IDX);
            msec_delay(10);
        }
        while(is_pb_up(PB1_IDX ));
        while(is_pb_down(PB1_IDX ))
        {
            seg7_off();
            msec_delay(10);
        }
        msec_delay(10);
        ++loop_count;
    }
    
}
/*use a DIP switch to control the segments of a seven-segment display. On the first read controls segments D through A. 
On the second read controls segments G through E*/
void lab5_part2()
{
    uint8_t loop_count = 0;
    uint8_t bytes = 0;
    enum state stateChange = GET_LOW;
    
    while(loop_count < 3)
    {
        switch(stateChange)
        {
            case GET_LOW:
                if(is_lpsw_down(LP_SW2_IDX))
                {
                    bytes = (dipsw_read()) & 0xF;
                    stateChange = GET_HIGH;
                    while (is_lpsw_down(LP_SW2_IDX));
                    msec_delay(10);
                }     
                
                break;

            case GET_HIGH:
                if(is_lpsw_down(LP_SW2_IDX))
                {
                    bytes |= dipsw_read() << 4;
                    stateChange = DISPLAY;
                    while (is_lpsw_down(LP_SW2_IDX));
                   msec_delay(10);
                }
               break;

            case DISPLAY:
                if(is_pb_down(PB1_IDX))
                {
                    seg7_on(bytes, SEG7_DIG2_ENABLE_IDX);
                    while(is_pb_down(PB1_IDX));
                    msec_delay(10);
                }
                else 
                {
                    seg7_on(bytes, SEG7_DIG0_ENABLE_IDX);
                }
                 if(is_lpsw_down(LP_SW2_IDX))
                {
                    ++loop_count;
                    seg7_off();
                    stateChange = GET_LOW;
                    while (is_lpsw_down(LP_SW2_IDX));
                   msec_delay(10);

                }
                break;
        }
        msec_delay(10);
        

    }
    leds_off();
    led_disable();
}
/* A function to detect a key pressed on the Keypad and display the corresponding light pattern on the LEDs in binary.
The LED pattern should stay on after the key is released. At the same time, ensure that the segments in the seven-dement displays are all off. */
void lab5_part3()
{
    uint8_t loop_count = 0;
    uint8_t keypress;
    led_enable();
    while(loop_count < 8)
    {
        wait_no_key_pressed();
        keypress = getkey_pressed();
        leds_on(keypress);
        wait_no_key_pressed();
        
        ++loop_count;
        msec_delay(10);
    }
    leds_off();
    led_disable();
}
/* The functions uses the Keypad where the number (0 to F) determines how many times to flash LD[7:0]. The LEDs flash
on for 0.5 seconds and off for 0.5 seconds. The program ignores additional input from the Keypad while the LEDs are
flashing. At the same time, ensure that the segments in the seven-dement displays are all off.*/
void lab5_part4()
{
    uint8_t loop_count = 0;
    led_enable();
    uint8_t keypress;
    while(loop_count < 4)
    {
        keypress = keypad_scan();
     
        if(keypress != 0x10)
        {
              
            for(uint8_t i = 0; i < keypress; ++i)
            {
                leds_on(0xffff);
                msec_delay(500);
                leds_off();
                msec_delay(500);
            }
            ++loop_count;
            wait_no_key_pressed();
            
        }
    }
    led_disable();
}


