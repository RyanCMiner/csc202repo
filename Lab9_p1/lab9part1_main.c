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

#include <stdbool.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "LaunchPad.h"
#include "lcd1602.h"
#include "adc.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab9_part1();
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
enum motor_state
{
    MOTOR_OFF1,
    MOTOR_CW,
    MOTOR_OFF2,
    MOTOR_CCW
};

int main(void)
{
    clock_init_40mhz();
    launchpad_gpio_init();
    
    led_init();
    led_enable();
    dipsw_init();
    
    I2C_init();
    lcd1602_init();
    
    keypad_init();
    
    motor0_init();
    motor0_pwm_init(4000,0);
    motor0_pwm_enable();
    
    config_pb1_interrupts();
    config_pb2_interrupts();
    
    run_lab9_part1();
    
    NVIC_DisableIRQ(GPIOA_INT_IRQn);
    NVIC_DisableIRQ(GPIOB_INT_IRQn);
    // Endless loop to prevent program from ending
    while (1);

} /* main */
// This function allows the user to controls the dc motor speed via the keypad.
// Any time the user press the keypad it take the keypress and divide by 15
// in order to get the percent for the speed of the motor. When pb2 is pressed 
// it triggers the state machine to go the next stage. The state machine changes the 
// direction of the motor. When pb1 is pressed the function ends.    
void run_lab9_part1()
{
    uint8_t count = 0;
    uint8_t keypress;
    uint8_t duty_cycle = 0;
    bool done = false;
    enum motor_state motorSAT = MOTOR_OFF1;

    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("Motor Speed");

    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_byte(duty_cycle);
    lcd_write_char('%');
    
    while (g_pb1_pressed != true) 
    {
        switch (motorSAT) //state machine 
        {
            case  MOTOR_OFF1:
                led_off(LED_BAR_LD1_IDX);
                led_off(LED_BAR_LD2_IDX);
                if(g_pb2_pressed)
                {
                    motorSAT = MOTOR_CW;
                    g_pb2_pressed = false;
                }
                break;
            case MOTOR_CW:
                led_off(LED_BAR_LD1_IDX);
                led_on(LED_BAR_LD2_IDX);
                if(g_pb2_pressed)
                {
                    motorSAT = MOTOR_OFF2;
                    g_pb2_pressed = false;
                }
                break;
            case MOTOR_OFF2:
                led_off(LED_BAR_LD1_IDX);
                led_off(LED_BAR_LD2_IDX);
                if(g_pb2_pressed)
                {
                    motorSAT = MOTOR_CCW;
                    g_pb2_pressed = false;
                }
                break;
            case MOTOR_CCW:
                led_on(LED_BAR_LD1_IDX);
                led_off(LED_BAR_LD2_IDX);
                if(g_pb2_pressed)
                {
                    motorSAT = MOTOR_OFF1;
                    g_pb2_pressed = false;
                }
                break;
            default:
                break;                
                
        }
        
        keypress = keypad_scan();
        if( keypress != 0x10)
        {
            duty_cycle = (keypress * 100)/15;
            motor0_set_pwm_dc(duty_cycle);

            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            lcd_write_string("Motor Speed");

            lcd_set_ddram_addr(LCD_LINE2_ADDR);
            lcd_write_byte(duty_cycle);
            lcd_write_char('%');

            wait_no_key_pressed();
        }
        msec_delay(100);
    }
    
    g_pb1_pressed = false;
    lcd_clear();
    lcd_write_string("Program done");
    leds_off();
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


