#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "stop_watch.h"
#include "LED_Patterns.h"

/*
 * Initializes a pair of LEDs for Gun and Tracer use.  LEDs must belong to the
 * same GPIO port.
 * 
 */

GunPort GunPortInit_(volatile const unsigned char (* OP), volatile const unsigned char (* IP), const uint8_t GLED, const uint8_t TLED)
{
    GunPort gp;
    
    gp.OutputPort = (volatile unsigned char *)OP;
    gp.InputPort = (volatile unsigned char *)IP;
    gp.TracerPort = (volatile unsigned char *)OP;
    gp.TracerInPort = (volatile unsigned char *)IP;
    
    gp.GunLED = GLED;
    gp.TracerLED = TLED;

    gp.GunCnt = 0;
    gp.ShotsCnt = 0;
    
    
    gp.GunRUN = false;  
    gp.A_B = false;
    
    *gp.OutputPort |= ((1 << GLED) | (1 << TLED));
    
    return gp;
}
/*
 * Initializes a pair of LEDs for Gun and Tracer use.  LEDs may belong to 
 * different GPIO ports.
 * 
 */

GunPort GunPortInit(volatile const unsigned char (* OP), volatile const unsigned char (* IP), volatile const unsigned char (* TP), volatile const unsigned char (* TIP), const uint8_t GLED, const uint8_t TLED)
{
    GunPort gp;
    
    gp.OutputPort = (volatile unsigned char *)OP;
    gp.InputPort = (volatile unsigned char *)IP;
    gp.TracerPort = (volatile unsigned char *)TP;
    gp.TracerInPort = (volatile unsigned char *)TIP;
    
    gp.GunLED = GLED;
    gp.TracerLED = TLED;

    gp.GunCnt = 0;
    gp.ShotsCnt = 0;
    
    
    gp.GunRUN = false;  
    gp.A_B = false;
    
    *gp.OutputPort |= (1 << GLED);
    *gp.TracerPort |= (1 << TLED);
    
    return gp;
}

/*
 * Starts an initialized Gun/Tracer pair blink pattern
 * 
 */

void StartGunFirePattern(GunPort *gp)
{
    if (gp->GunRUN)  //Check if pattern is already running
        StopGunFirePattern(gp);  //Stop it if it is.
    
    gp->GunCnt = GUN_ON_TIME_MS + GetCurrentTime();  //Compute next transition time
    gp->GunRUN = true;  //Set the Running Flag ON
    gp->ShotsCnt = 0;  //Reset number of rounds fired
    gp->A_B = true;  //First half of the pulse
    
    *gp->OutputPort &= (unsigned char)~(1 << gp->GunLED);  //Drive Gun pin Low
    *gp->TracerPort |= (unsigned char)(1 << gp->TracerLED); //Drive Tracer pin High

}

/*
 * Checks the amount of time elapsed and toggles Gun/Tracer GPIO pins
 */
void UpdateGunLED(GunPort *gp)
{
    if (gp->GunRUN)  //Only update the GPIO pins if the Gun is firing
    {
        if (gp->GunCnt <= GetCurrentTime())  //Check enough time has passed 
        {
            gp->A_B = (bool)!gp->A_B;  //Move to next half of the pulse
            
            if (!gp->A_B)  //"Off" portion of the pulse
            {
                *gp->OutputPort |= (unsigned char)(1 << gp->GunLED);  //Drive Gun GPIO pin High
                *gp->TracerPort |= (unsigned char)(1 << gp->TracerLED);//Drive Tracer GPIO pin High
                
                if(gp->ShotsCnt == 8)  //Check to see if Tracer is next to fire
                {
                    gp->GunCnt += GUN_SPACE_TIME_MS;  //Wait appropriate time to put tracer in the middle of 100mS pause
                }
                else
                {
                    gp->GunCnt += GUN_OFF_TIME_MS;  //Normal "off" time
                }

                gp->ShotsCnt++;  //increment number of rounds fired
                
                if (gp->ShotsCnt == 10)  //Reset after 10 rounds (0-9)
                {
                    gp->ShotsCnt = 0;                    
                }
            }
            else  //"On" portion of pulse cycle
            {
                if (gp->ShotsCnt < 9)  //First 9 rounds are regular gun
                {
                    *gp->OutputPort &= (unsigned char)~(1 << gp->GunLED);  //Drive GUN GPIO pin Low
                    *gp->TracerPort |= (unsigned char)(1 << gp->TracerLED);//Drive Tracer GPIO pin High
                    gp->GunCnt += GUN_ON_TIME_MS;//Calculate when to turn off the LED
                    
                }
                else if(gp->ShotsCnt == 9) //10th round is a tracer
                {
                    *gp->TracerPort &= (unsigned char)~(1 << gp->TracerLED);//Drive Tracer GPIO pin Low
                    *gp->OutputPort |= (unsigned char)(1 << gp->GunLED);//Drive GUN GPIO pin High
                    gp->GunCnt += GUN_ON_TIME_MS;  //Calculate when to turn off the LED
                
                }
            }
        }
    }
}

/*
 * Stops a currently running Gun Pattern
 */
void StopGunFirePattern(GunPort *gp)
{
    if (gp->GunRUN)  //Only stop a running pattern
    {
        gp->GunRUN = false;  //Clear the running flag
        *gp->OutputPort |= (unsigned char)(1 << gp->GunLED);  //Drive GUN GPIO Pin High
        *gp->TracerPort |= (unsigned char)(1 << gp->TracerLED);//Drive Tracer GPIO Pin High
    }
}

/*
 * Starts an initialized strobe pattern on its LED pin.
 */
void StartStrobe(StrobeLED *SLED)
{
    if (SLED->StrobeRUN)  //Check to see if this pattern is already running
        StopStrobe(SLED);  //It is running. Stop it so we can restart it with new interval
    
    SLED->StrobeCnt = STROBE_ON_TIME_MS + GetCurrentTime();  //Calculate when to transition next.
    SLED->StrobeRUN = true;  //Set the run flag true.
    SLED->A_B = true;  //Set "ON" portion of pulse period
    SLED->PulseCnt = 0;  //reset number of flashes to 0
    *SLED->OutPort &= (unsigned char)~(1 << SLED->pin);  //Turn on the LED.
}

/*
 * Starts a pair strobe LEDs with a specified delay between them.
 */
void StartSyncStrobes(StrobeLED *SLED_A, StrobeLED *SLED_B, uint32_t delay)
{

    if (SLED_B->StrobeRUN)  //Check to see if this pattern is already running
        StopStrobe(SLED_B);  //It is running. Stop it so we can restart it with new interval
    
    StartStrobe(SLED_A);  //Start the leading strobe
    

    //Start the lagging strobe
    
    //Lag the leading strobe by delay # milliseconds.  
    SLED_B->StrobeCnt = SLED_A->StrobeCnt + delay;  //Calculate when to transition next based on when the leading strobe light turns off the first time.
    SLED_B->StrobeRUN = true;  //Set the run flag true.
    SLED_B->A_B = false;  //Set "OFF" portion of pulse period
    SLED_B->PulseCnt = 0;  //reset number of flashes to 0
    *SLED_B->OutPort |= (unsigned char)(1 << SLED_B->pin);  //Turn off the LED.    

}

/*
 * Stops a Strobe LED
 */
void StopStrobe(StrobeLED *SLED)
{
    if (SLED->StrobeRUN)  //Only need to stop a running strobe
    {
        SLED->StrobeRUN = false;  //clear running flag
        *SLED->OutPort |= (unsigned char)(1 << SLED->pin);  //turn off the LED
    }
}

/*
 * Stops a pair of sync'd strobes
 */
void StopSyncStrobes(StrobeLED *SLED_A, StrobeLED *SLED_B)
{
    StopStrobe(SLED_A);  //Stop the first strobe
    StopStrobe(SLED_B);  //Stop the second strobe
}

/*
 * Updates the strobe LED based on the chosen pattern
 */
void UpdateStrobeLED(StrobeLED *SLED)
{
    if (SLED->StrobeRUN)  //Only need to evaluate strobes that are running (flashing)
    {
        if (SLED->StrobeCnt <= GetCurrentTime())  //Check enough time has passed
        {
            switch (SLED->S_Type)  //Determine what type of strobe this is.
            {
                case EF_Strobe:  //E/F Strobe pattern defined here
                {
                    /*
                     * E/F Strobe is 3 Consecutive strobes with a 10% duty cycle
                     * in a 1 second pulse width.  After 3 consecutive strobes 
                     * there will be a 2 second wait and the strobe pattern will
                     *  repeat.
                     */
                    SLED->A_B = (bool)!SLED->A_B;  //Switch to next half of pulse
                    
                    if (!SLED->A_B)  //"Off" portion of the pulse
                    {
                        *SLED->OutPort |= (unsigned char)(1 << SLED->pin);  //turn off the LED
                        SLED->StrobeCnt += STROBE_OFF_TIME_MS;  //calculate next transition time
        
                        SLED->PulseCnt++;  //increase the number of pulses completed
                        if (SLED->PulseCnt == 5)  //reset after 5 complete cycles
                            SLED->PulseCnt = 0;
                        
                    }
                    else  //"ON" portion of the pulse
                    {
                        if (SLED->PulseCnt < 3)  //Only the first three seconds have a light pulse
                        {
                            *SLED->OutPort &= (unsigned char)~(1<<SLED->pin);  //turn on the LED
                        }
                        SLED->StrobeCnt += STROBE_ON_TIME_MS;  //calculate the next transition time
                    }   
                    
                    break;
                }
                case G_Strobe:
                {
                    /*
                     * G Strobe is an active 100 msec active low signal in a 1 
                     * second pulse width.  However with this strobe there will 
                     * be 2 pulses (as described previously) followed by a 2 
                     * second wait then 1 more 100 msec negative strobe followed
                     * by an additional 2 second wait and the sequence will then
                     * repeat.
                     */
                    SLED->A_B = (bool)!SLED->A_B;  //toggle period halves
                    
                    if (!SLED->A_B)  //"OFF" portion of the pulse period
                    {
                        *SLED->OutPort |= (unsigned char)(1<< SLED->pin);  //Turn off the LED
                        
                        if ((SLED->PulseCnt < 2) || (SLED->PulseCnt == 3))  //1 second periods for pulses 0,1,& 3.
                            SLED->StrobeCnt += STROBE_OFF_TIME_MS;
                        else
                            SLED->StrobeCnt += (2000 - STROBE_ON_TIME_MS);  //2 second periods for pulses 2 and 4
                        
                        SLED->PulseCnt++;  //increment number of pulses done
                        
                        if (SLED->PulseCnt == 5)  //reset after 5 pulses
                            SLED->PulseCnt = 0;
                        
                    }
                    else  //"ON" portion of the period
                    {
                        if ((SLED->PulseCnt < 2) || (SLED->PulseCnt == 3))  //Only light the LED for pulses 0,1,& 3.
                        {
                            *SLED->OutPort &= (unsigned char)~(1 << SLED->pin);  //Turn on the LED.
                        }
                        
                        SLED->StrobeCnt += STROBE_ON_TIME_MS;  //Calculate next transition time
                    }   
                    
                    break;                
                }
                case General:
                case AntiCollision:
                {
                    /*
                     * General Strobe is a 100 msec active low signal in a 2 
                     * second pulse width that repeats.  General Anti Collision 
                     * Strobe is a 100 msec active low signal in a 2 second 
                     * pulse width that repeats but that is offset of the 
                     * General Strobe by .5 seconds.  Offset is taken care of by
                     * a synched strobe start of the General and A/C strobes.
                     */
                    SLED->A_B = (bool)!SLED->A_B;  //toggle period half.
                    
                    if (!SLED->A_B)  //"OFF" portion of the period
                    {
                        *SLED->OutPort |= (unsigned char)(1 << SLED->pin);  //turn LED off
                        SLED->StrobeCnt += GEN_STROBE_OFF_TIME_MS;  //calculate next transition time.
                    }
                    else //"ON" portion of the pulse
                    {
                        *SLED->OutPort &= (unsigned char)~(1 << SLED->pin);  //Turn on the LED.
                        SLED->StrobeCnt += GEN_STROBE_ON_TIME_MS;  //calculate next transition time.
                    }   
                    
                    
                    break;
                    
                }
            }
            
        }
    }
}

/*
 * Initializes a new Strobe "Object" on the specified pin with the specified 
 * flash pattern.
 */

StrobeLED newStrobe(volatile const unsigned char (* OP),volatile const unsigned char (* IP), const uint8_t sLED, const strobeType t)
{
    
    StrobeLED Strobe;
    Strobe.OutPort = (volatile unsigned char *)OP;
    Strobe.InPort = (volatile unsigned char *)IP;
    Strobe.pin = sLED;
    Strobe.S_Type = t;
    Strobe.StrobeCnt = 0;
    Strobe.StrobeRUN = false;
    Strobe.PulseCnt = 0;
    Strobe.A_B = false;
    
    *Strobe.OutPort |= (1 << sLED);
    
    return Strobe;
}

/*
 * Initializes a new Rabbit Light String on the port specified with the number
 * of lights specified.  (max lights = 8)
 */
RabbitPort RabbitLightsInit(volatile const unsigned char (* OP), volatile const unsigned char (* IP), const uint8_t SLED, const uint8_t num)
{
    RabbitPort rp;
    
    rp.OutputPort = (volatile unsigned char *)OP;
    rp.InputPort = (volatile unsigned char *)IP;
    rp.StartingLED = SLED;
    rp.NumLights = num;
    
    rp.RabbitRUN = false;
    rp.ShiftCNT = 0;
    rp.msCnt = 0;
    
    return rp;
    
}

/*
 * Starts a set of rabbit lights flashing on their specified port.
 */

void StartRabbitLights(RabbitPort *rp)
{
    if (rp->RabbitRUN)  // check if lights are already running and stop them if they are.
        StopRabbitLights(rp);
    
    rp->msCnt = RABBIT_ON_TIME_MS + GetCurrentTime();  //calculate next transition time
    rp->RabbitRUN = true;  //show lights are running
    rp->A_B = true;  //set "ON" portion of pulse.
    
    unsigned char Mask = 0;
    
    for (uint8_t i = 0; i < rp->NumLights; i++)
    {
        /*
         * Builds a pin mask for the port that covers only the rabbit lights
         */
        Mask = (unsigned char)((Mask << 1) | 1);
    }
    
    Mask = (unsigned char)(Mask << rp->StartingLED);  //Shifts the mask to the starting pin of the lights
    
    *rp->OutputPort |= Mask; //turn off all the rabbit lights
    *rp->OutputPort &= (unsigned char)~(1 << rp->StartingLED);  //turn on the first rabbit LED.
}
    
/*
 * Checks for right transition time and updates the rabbit LEDs.
 */
void UpdateRabbitLEDs(RabbitPort *rp)
{
    if (rp->RabbitRUN)  //Only need to update if the lights are running.
    {
        if (rp->msCnt <= GetCurrentTime())  //Check if time to switch.
        {
            /*
             * Rabbit lights are an active low 10mS pulse with a pulse width of 
             * 50mS.  The string only has one light on at any given time, with 
             * the light appearing to run down the line.
             */            
            rp->A_B = (bool)!rp->A_B;  //toggle which half of the period are we on.
            
            unsigned char Mask = 0;  //build a pin mask for the rabbit LEDs
            for (uint8_t i = 0; i < rp->NumLights; i++)
            {
                Mask = (unsigned char)((Mask << 1) | 1);
            }

            Mask = (unsigned char)(Mask << rp->StartingLED);  //shift the mask in place

            unsigned char buffer;
            
            if (!rp->A_B)  //"OFF" portion of the pulse.
            {
                rp->ShiftCNT++;  //Increment number of shifts completed
                
                if (rp->ShiftCNT == rp->NumLights)  //reset if last light has been reached
                {
                    rp->ShiftCNT = 0;
                }
                rp->msCnt += RABBIT_OFF_TIME_MS;  //calculate next transition time
                
                *rp->OutputPort |= Mask;  //turn off rabbit LEDs
            }
            else
            {

                //builds proper pin mask to turn on only the next LED in the string.
                buffer = (unsigned char)(1 << (rp->StartingLED + rp->ShiftCNT));
            
                *rp->OutputPort &= (unsigned char)~buffer;  //assert only the new state without disrupting the rest of the port
            
                
                rp->msCnt += RABBIT_ON_TIME_MS;  //calculate next transition time.
            }
        }
    }
}

/*
 * Stops a currently running Rabbit Light String.
 */
    
void StopRabbitLights(RabbitPort *rp)
{
    if (rp->RabbitRUN)  //only stop a running string
    {
        rp->RabbitRUN = false;  //clear the running flag
        unsigned char Mask = 0;  //build a pin mask for the rabbit LEDs
        for (uint8_t i = 0; i < rp->NumLights; i++)
        {
            Mask = (unsigned char)((Mask << 1) | 1);
        }

        Mask = (unsigned char)(Mask << rp->StartingLED);  //shift the mask in place
        
        *rp->OutputPort |= Mask;  //turn off all the LEDs
    }
}

    
