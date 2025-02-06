//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ryan Miner
//
//       LAB NAME:  Lab 8
//
//      FILE NAME:  lab8_part2_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a demonstration of ADC on the micro controller
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
#include "adc.h"
#include "lcd1602.h"
//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void config_pb1_interrupts();
void config_pb2_interrupts();
void run_lab8_part2();
//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define ADC_CHANNEL7 (7)
//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool g_pb1_pressed = false;
bool g_pb2_pressed = false;

// Define a structure to hold different data types

int main(void)
{
    clock_init_40mhz();
    launchpad_gpio_init();
    
    dipsw_init();
    I2C_init();
    lcd1602_init();
    
    led_init();
    
    ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);
    
    config_pb1_interrupts();
    config_pb2_interrupts();
    
    run_lab8_part2();
 
 // Endless loop to prevent program from ending
 while (1);

} /* main */
// this function can toggle the leds using the potentiometer 
void run_lab8_part2()
{
    uint32_t adc;
    uint16_t led_adc_seg = 455;
    
    led_enable();
    
    while(g_pb1_pressed == false) 
    {
        lcd_clear();
        adc = ADC0_in(ADC_CHANNEL7);
        
        lcd_write_string("adc =");
        lcd_write_doublebyte(adc);
        
        leds_off();
        for(uint8_t led_idx = 0x0; led_idx < (adc/led_adc_seg);++led_idx)
        {
            led_on(led_idx);
        }
        msec_delay(100);
    }
    g_pb1_pressed = false;
    lcd_clear();
    lcd_write_string("Program terminated ");
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
     NVIC_SetPriority(GPIOA_INT_IRQn,2);
    NVIC_EnableIRQ(GPIOA_INT_IRQn);

}
// this function configures pb2 to be able to do interrupts. 
void config_pb2_interrupts()
{
    GPIOB -> POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;
    GPIOB -> CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
    GPIOB -> CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;
    NVIC_SetPriority(GPIOB_INT_IRQn,2);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);

}