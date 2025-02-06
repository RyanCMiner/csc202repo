//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ryan Miner
//
//       LAB NAME:  Lab6
//
//      FILE NAME:  main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a way to demonstrate the LCD 
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
#include "lcd1602.h"
#include "LaunchPad.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab6_part1();
void run_lab6_part2();
void run_lab6_part3();
void run_lab6_part4();

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
    dipsw_init();
    keypad_init();
    lcd_set_display_on();
    run_lab6_part1();
    while(is_pb_up(PB2_IDX ));
    msec_delay(10);
    lcd_clear();
    lcd_write_string("Running part 2");
    msec_delay(1000);
    while(is_pb_down(PB2_IDX));
    msec_delay(5);
    lcd_clear();
    run_lab6_part2();
   
    while(is_pb_up(PB2_IDX ));
    msec_delay(10);
    lcd_clear();
    lcd_write_string("Running part 3");
    msec_delay(1000);
    while(is_pb_down(PB2_IDX));
    msec_delay(5);
    lcd_clear();
    run_lab6_part3();
    
    while(is_pb_up(PB2_IDX ));
    lcd_clear();
    lcd_write_string("Running part 4");
    msec_delay(1000);
    while(is_pb_down(PB2_IDX));
    msec_delay(5);  
    lcd_clear();
    run_lab6_part4();
    lcd_clear();
    lcd_write_string("Program terminated");
    msec_delay(1000);
    lcd_set_display_off();
    lcd_clear();

 
    // Endless loop to prevent program from ending
    while (1);

} /* main */
// This function writes the A - Z to the lcd using the lcd_write_char().
void run_lab6_part1()
{
    
   for(uint8_t letter = 'A'; letter <= 'Z';++letter)
   {
        if(letter == ('A' + CHARACTERS_PER_LCD_LINE - 1))
        {
            lcd_set_ddram_addr(LCD_LINE2_ADDR);
        }
        lcd_write_char(letter);
        msec_delay(50);
   } 
   
}
// this function display a 32,16 and 8 bit value to the lcd. when you press PB1_IDX the first time it would display the 16 bit value.
// when you press PB1_IDX the second time it would clear the display and then display the 8 bit value. 
// and the final PB1_IDX press would display part 2 done and press pb2 
void run_lab6_part2()
{
    uint32_t bitVaule_32 = 1234567890;
    uint16_t bitVaule_16 = 1234;
    uint8_t bitVaule_8 = 123;
    lcd_set_ddram_addr(3);
    lcd_write_quadbyte(bitVaule_32);
    while(is_pb_up(PB1_IDX ));
    while(is_pb_down(PB1_IDX))
    {
        lcd_set_ddram_addr(4 + LCD_LINE2_ADDR);
        lcd_write_doublebyte(bitVaule_16);
        msec_delay(10);
    }
    while(is_pb_up(PB1_IDX ));
    while(is_pb_down(PB1_IDX))
    {
        lcd_clear();
        lcd_set_ddram_addr(5);
        lcd_write_byte(bitVaule_8);
        msec_delay(10);
    }
    while(is_pb_up(PB1_IDX ));
    while(is_pb_down(PB1_IDX))
    {
        lcd_clear();
        lcd_write_string("Part 2 Done");
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        lcd_write_string("Press PB2");
        msec_delay(1000);
    }
    
    
}
// this function counts from 100 to zero and write to the lcd and will continue to loop until pb2 is pressed.
// if the user press pb1 it would rest the count.
void run_lab6_part3()
{
    
    bool done = false; 
    while(is_pb_up(PB2_IDX) || done == true)
    {
        uint8_t count = 100;
        lcd_set_ddram_addr(6);
        while (count != 0) 
        {
            if(is_pb_down(PB2_IDX))
            {
                done = true;
                break;
            }
            lcd_write_byte(count--);
            if(is_pb_down(PB1_IDX))
            {
                count = 100;
                msec_delay(10);
            }
            msec_delay(200);
            lcd_clear();
            lcd_set_ddram_addr(6);
        }
    }
    lcd_clear();
    lcd_write_string("Part 3 Done");
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("Press PB2");
    msec_delay(1000);
    
    
}
// this function writes what was pressed on the keypad and when the screen gets full. it would reset the screen 
// if you press pb1 it would reset the screen as well and if pb2 is pressed then the function would end. 
void run_lab6_part4()
{
    uint8_t count = 0;
    uint8_t keypress;
    bool done = false;
    
    while (is_pb_up(PB2_IDX) || done == true) 
    {
        if(is_pb_down(PB1_IDX))
        {
            lcd_clear();
            count = 0;
            msec_delay(10);
        }
        keypress = keypad_scan();
        if( keypress != 0x10)
        {
            ++count;
            hex_to_lcd(keypress);
            
            if(count == CHARACTERS_PER_LCD_LINE)
            {
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
            }
            if(is_pb_down(PB1_IDX) || count == TOTAL_CHARACTERS_PER_LCD + 1)
            {
                lcd_clear();
                count = 0;
                hex_to_lcd(keypress);
                msec_delay(10);

            }
            if(is_pb_down(PB2_IDX))
            {
                done = true;
            }
            wait_no_key_pressed();
        }
        
       
    
    }
     lcd_clear();
    lcd_write_string("Part 4 Done");
    msec_delay(1000);
   
}


