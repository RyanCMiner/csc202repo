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
#include "clock.h"
#include "clock.h"
#include "LaunchPad.h"
#include "lcd1602.h"
#include "adc.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void run_lab9_part3();
void config_pb1_interrupts();
void config_pb2_interrupts();
//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
bool g_pb1_pressed = false;
bool g_pb2_pressed = false;

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
    dipsw_init();
    
    I2C_init();
    lcd1602_init();
    
    ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);
    
    motor0_init();
    motor0_pwm_init(4000,0);
    motor0_pwm_enable();

    config_pb1_interrupts();
    config_pb2_interrupts();
    
    run_lab9_part3();

    NVIC_DisableIRQ(GPIOA_INT_IRQn);
    NVIC_DisableIRQ(GPIOB_INT_IRQn);
 // Endless loop to prevent program from ending
 while (1);

} /* main */
// This function determins the rotation of the servo motor
// via the use of the potentiometer. When pb1 is pressed the function ends.
void run_lab9_part3()
{
    uint8_t duty_cycle = 0;
    uint8_t temp = 0;
    uint32_t adc;
    uint16_t servo_count;
    
    led_off(LED_BAR_LD1_IDX);
    led_on(LED_BAR_LD2_IDX);
    
    while(g_pb1_pressed == false) 
    {
        adc = (ADC0_in(7) ) >> 2;
        servo_count = ((adc * (500 - 100)) / 1023) + 100;
        
        motor0_set_pwm_count(servo_count);

        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        lcd_write_string("adc value =");
        lcd_write_doublebyte(adc);

        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        lcd_write_string("servo cnt =");
        lcd_write_doublebyte(servo_count);

        msec_delay(250);
    }
    
    leds_off();
    g_pb1_pressed = false;
    lcd_clear();
    lcd_write_string("Program done");

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
                    g_pb2_pressed = true;
                    GPIOA -> CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
                }
                break;
            
            case (CPUSS_INT_GROUP_IIDX_STAT_INT1):
                gpio_mis = GPIOB -> CPU_INT.MIS;
                
                if((gpio_mis & GPIO_CPU_INT_MIS_DIO18_MASK) == GPIO_CPU_INT_MIS_DIO18_SET)
                {
                    g_pb1_pressed = true;
                    GPIOB -> CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
                }
                break;
            default:
                break;
        }
    } while (group_iidx_status != 0);
}
// this function configures pb2 to be able to do interrupts.  
void config_pb2_interrupts()
{
    GPIOA -> POLARITY15_0 = GPIO_POLARITY15_0_DIO15_RISE;
    GPIOA -> CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
    GPIOA -> CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO15_SET;
     NVIC_SetPriority(GPIOA_INT_IRQn,2);
    NVIC_EnableIRQ(GPIOA_INT_IRQn);

}
// this function configures pb1 to be able to do interrupts. 
void config_pb1_interrupts()
{
    GPIOB -> POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;
    GPIOB -> CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
    GPIOB -> CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;
    NVIC_SetPriority(GPIOB_INT_IRQn,2);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);
}
