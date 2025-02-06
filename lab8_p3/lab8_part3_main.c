//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Ryan Miner
//
//       LAB NAME:  Lab 8
//
//      FILE NAME:  lab8_part3_main.c
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
#include "ti/devices/msp/m0p/mspm0g350x.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void config_pb1_interrupts();
void config_pb2_interrupts();
void run_lab8_part3();
//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define CIRCLE  (0xdf)
#define ADC_CHANNEL7 (7)
#define ADC_CHANNEL5 (5)
//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool g_pb1_pressed = false;
bool g_pb2_pressed = false;
uint16_t temp = 0;

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
    run_lab8_part3();
    NVIC_DisableIRQ(GPIOA_INT_IRQn);
    NVIC_DisableIRQ(GPIOB_INT_IRQn);
 
 // Endless loop to prevent program from ending
 while (1);

} /* main */
// this function allows the user to get the current temperature by pressing pb2 and can toggle the leds using the potentiometer 
void run_lab8_part3()
{
    uint16_t adc = 0;
    uint16_t led_adc_seg = 455;
    
    led_enable();
    
    while(g_pb1_pressed == false) 
    {
        lcd_clear();
        
        NVIC_DisableIRQ(GPIOB_INT_IRQn);
        adc = ADC0_in(ADC_CHANNEL7);
        NVIC_EnableIRQ(GPIOA_INT_IRQn);
        
        lcd_write_string("adc =");
        lcd_write_doublebyte(adc);
        
        if(g_pb2_pressed)
        {
            g_pb2_pressed = false;
        }
        
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        lcd_write_string("Temp = ");
        lcd_write_byte(temp);
        lcd_write_char(CIRCLE);
        lcd_write_char('F');
        msec_delay(100);
        
        leds_off();
        for(uint8_t led_idx = 0x0; led_idx < (adc/led_adc_seg);++led_idx)
        {
            led_on(led_idx);
        }
        msec_delay(100);
     
    }
    g_pb1_pressed = false;
    lcd_clear();
    lcd_write_string("Program terminated");
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
    uint16_t adc = ADC0_in(ADC_CHANNEL5);
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
                    temp = (thermistor_calc_temperature(adc) * (9.0/5)) +32;// Gets new temp when pb2 is pressed
                    
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