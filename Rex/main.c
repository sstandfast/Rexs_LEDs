/**
  
  File Name:
    main.c

  Summary:
    This is the main program file generated for Rex's LED driver board

  Description:
    This file provides implementations for driver APIs for all lights and 
    triggers defined on the Rev - Driver board.
*/

#include "mcc_generated_files/mcc.h"
#include "stop_watch.h"
#include "LED_Patterns.h"

/*
                         Main application
 */

// constant defines
const uint8_t TRIGGER1_PIN = 0;
const uint8_t TRIGGER2_PIN = 1;
const uint8_t STROBE1_PIN = 2;
const uint8_t STROBE2_PIN = 3;

//Main loop
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    
    // Initialize the Stopwatch driver.
    StopWatch_Initialize();
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Start the Stopwatch timer.
    StartStopWatch();

    //Initialize the Gun/Tracer instances.
    GunPort Gun1 = GunPortInit(&LATC,&PORTC,&LATD,&PORTD,5,2);
    GunPort Gun2 = GunPortInit(&LATC,&PORTC,&LATD,&PORTD,4,1);
    GunPort Gun3 = GunPortInit_(&LATD,&PORTD,3,0);
    
    //Initialize the Strobe Instances
    StrobeLED EFStrobe = newStrobe(&LATC,&PORTC,3,EF_Strobe);
    StrobeLED GStrobe = newStrobe(&LATC,&PORTC,2,G_Strobe);
    StrobeLED ACStrobe = newStrobe(&LATC,&PORTC,0,AntiCollision);
    StrobeLED GenStrobe = newStrobe(&LATC,&PORTC,1,General);
    
    //Initialize the Rabbit lights
    RabbitPort Rabbit = RabbitLightsInit(&LATB,&PORTB,0,8);
    
    //Do this forever
    while (1)
    {   
        if ((PORTA & (0x01 << TRIGGER1_PIN)) == 0x00)  //Check trigger 1 switch pressed
        {
            if (!Gun1.GunRUN)  //Check if guns are already firing
            {
                StartGunFirePattern(&Gun1);  //Start firing!
                //_delay(50);
            }
            if (!Gun2.GunRUN)  //Check if the guns are already firing
            {
                StartGunFirePattern(&Gun2);  //Start firing!
            }
        }
        else  //Trigger is released
        {
            StopGunFirePattern(&Gun1);  //Stop firing
            StopGunFirePattern(&Gun2);  //Stop firing
        }

        if ((PORTA & (0x01 << TRIGGER2_PIN)) == 0x00)  //Check trigger 2 switch pressed
        {
            if (!Gun3.GunRUN)  //check if gun is already firing
            {
                StartGunFirePattern(&Gun3);  //Start firing!
            }
        }
        else  //Trigger is released
        {
            StopGunFirePattern(&Gun3);  //Stop firing
        }
        
        if ((PORTA & (1 << STROBE1_PIN))== 0x00)  //Check if strobe enable switch is pressed
        {
            if (!EFStrobe.StrobeRUN)  //Check if strobes are already running
            {
                //Start strobes
                StartSyncStrobes(&GenStrobe, &ACStrobe, AC_STROBE_DELAY_MS);
                StartStrobe(&EFStrobe);
                StartStrobe(&GStrobe);

            }
        }
        else  //Strobes disabled
        {
            //Stop strobes.
            StopStrobe(&EFStrobe);
            StopStrobe(&GStrobe);
            StopSyncStrobes(&GenStrobe, &ACStrobe);
        }

        if ((PORTA & (1 << STROBE2_PIN))== 0x00)  //Check if strobe enable 2 switch is pressed.
        {
            if (!Rabbit.RabbitRUN)  //Check if rabbit lights are running
            {
                StartRabbitLights(&Rabbit);  //start lights
            }
            //_delay(50);
        }
        else  //Lights are disabled
        {
            StopRabbitLights(&Rabbit);  //Stop lights
        }

        // Update All LEDs
        UpdateGunLED(&Gun1);
        UpdateGunLED(&Gun2);
        UpdateGunLED(&Gun3);
        UpdateStrobeLED(&EFStrobe);
        UpdateStrobeLED(&GStrobe);
        UpdateStrobeLED(&GenStrobe);
        UpdateStrobeLED(&ACStrobe);
        UpdateRabbitLEDs(&Rabbit);
        
    }
}
/**
 End of File
*/