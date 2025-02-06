//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ryan Miner
//
//       LAB NAME:   lab 7 part 4
//
//      FILE NAME:  lab7p4_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a way to demonstrate gpio interrupts 
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
#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "ti/devices/msp/peripherals/hw_gpio.h"
#include "ti/devices/msp/peripherals/m0p/hw_cpuss.h"


//---------------------------------------------------------------------
// Define function prototypes used by the program
//---------------------------------------------------------------------
void config_pb1_interrupts();
void config_pb2_interrupts();
void run_lab7_part4();

//---------------------------------------------------------------------
// Define symbolic constants used by the program
//---------------------------------------------------------------------
#define NUM_STATES                   18

//---------------------------------------------------------------------
// Define global variables and structures here.
//---------------------------------------------------------------------
bool g_pb1_pressed = false;
bool g_pb2_pressed = false;


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
    
    config_pb1_interrupts();
    config_pb2_interrupts();
    lcd_clear();

    run_lab7_part4();
 // Endless loop to prevent program from ending
 while (1);

} /* main */
void run_lab7_part4()
{
    bool done = false; 
    while(done != true)
    {
        uint8_t count = 0;
        uint8_t pb2count = 0;
        lcd_set_ddram_addr(6);
        while (count != 99) 
        {
            if(g_pb1_pressed)
            {
                done = true;
                g_pb1_pressed = false;
                lcd_clear();
                lcd_write_string("program stopped");

            }
            
            if(g_pb2_pressed)
            {
                if(++pb2count != 2)
                {
                    lcd_set_ddram_addr(LCD_LINE2_ADDR);
                    lcd_write_string("press pb2");

                }
                else 
                {
                    lcd_clear();
                }
                g_pb2_pressed = false;
            }
            lcd_set_ddram_addr(6);
            lcd_write_byte(count++);
            msec_delay(200);
            lcd_clear();
           
        }
    }
}
//------------------------------------------------------------------------------
// DESCRIPTION:
//This function represents the ISR (Interrupt Service Routine) for
//  GPIOA and GPIOB. It has flag variable to determine iof the pb1 and pb2 are pressed 
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
void GROUP1_IRQHandler(void)
{
    uint32_t group_iidx_status;
    uint32_t gpio_mis;
    do 
    {
        group_iidx_status = CPUSS -> INT_GROUP[1].IIDX;

        switch (group_iidx_status) 
        {
            case (CPUSS_INT_GROUP_IIDX_STAT_INT0):
                gpio_mis = GPIOA -> CPU_INT.MIS;
                
                if((gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) == GPIO_CPU_INT_MIS_DIO15_SET)
                {
                    g_pb1_pressed = true;
                    GPIOA -> CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
                }
                break;
            
            case (CPUSS_INT_GROUP_IIDX_STAT_INT1):
                gpio_mis = GPIOB -> CPU_INT.MIS;
                
                if((gpio_mis & GPIO_CPU_INT_MIS_DIO18_MASK) == GPIO_CPU_INT_MIS_DIO18_SET)
                {
                    g_pb2_pressed = true;
                    GPIOB -> CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
                }
                break;
            default:
                break;
        }
    } while (group_iidx_status != 0);
} 
// this function configures pb1 to be able to do interrupts. 
void config_pb1_interrupts()
{
    GPIOA -> POLARITY15_0 = GPIO_POLARITY15_0_DIO15_RISE;
    GPIOA -> CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
    GPIOA -> CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO15_SET;
     NVIC_SetPriority(GPIOA_INT_IRQn,1);
    NVIC_EnableIRQ(GPIOA_INT_IRQn);

}
// this function configures pb1 to be able to do interrupts. 
void config_pb2_interrupts()
{
    GPIOB -> POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;
    GPIOB -> CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
    GPIOB -> CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;
    NVIC_SetPriority(GPIOB_INT_IRQn,2);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);

}




