/* 
 * File:   LED_Patterns.h
 * Author: Shawn
 *
 * Created on December 9, 2017, 9:34 PM
 */

#ifndef LED_PATTERNS_H
#define	LED_PATTERNS_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum uint8_t
{
    EF_Strobe,
    G_Strobe,
    General,
    AntiCollision
}strobeType;

    // Constants and Typedefs
    const uint32_t GUN_ON_TIME_MS = 10; //10 ms ON
    const uint32_t GUN_OFF_TIME_MS = 40;  //40 ms OFF (50ms period)
    const uint32_t GUN_SPACE_TIME_MS = 50;  //Wait 50ms before firing tracer
    const uint32_t STROBE_ON_TIME_MS = 100;  //100 mS on
    const uint32_t STROBE_OFF_TIME_MS = 900;  //1 second period
    const uint32_t GEN_STROBE_ON_TIME_MS = 100;  //100 ms ON
    const uint32_t GEN_STROBE_OFF_TIME_MS = 1900;  //2 second period
    const uint32_t AC_STROBE_DELAY_MS = 500;  //500 ms delay between two strobes
    const uint32_t RABBIT_ON_TIME_MS = 10;  //10 ms ON
    const uint32_t RABBIT_OFF_TIME_MS = 40; //40 ms OFF (50ms period)
    
    
    typedef struct StrobeLED StrobeLED;
    struct StrobeLED
    {
        volatile unsigned char *OutPort;  //Pointer to GPIO Port Output Register
        volatile unsigned char *InPort;  //Pointer to GPIO Port Input Register
        uint8_t pin;  //Holds pin number of the GPIO Port (0-7)
        strobeType S_Type;  //Assigns this LED's Strobe Pattern Type
        uint32_t StrobeCnt;  //Used to set the timing for this LED
        bool StrobeRUN;  //On/Off control for this LED
        uint8_t PulseCnt;  //Used for counting pulses in pattern
        bool A_B;  //Identifies which half of the pulse is currently active
        
    };
    
    typedef struct GunPort GunPort;
    struct GunPort
    {
        volatile unsigned char *OutputPort;//Pointer to GPIO Output register
        volatile unsigned char *InputPort;//Pointer to GPIO Input register
        
        volatile unsigned char *TracerPort;  //Pointer to GPIO Output register
        volatile unsigned char *TracerInPort;//Pointer to GPIO Input register
        
        uint8_t GunLED;  //Holds pin # of the GPIO port (0-7) for the GUN LED
        uint8_t TracerLED;//Holds pin # of the GPIO port (0-7)for the Tracer LED

        uint32_t GunCnt;  //Used to set the timing of the Gun LED
        uint8_t ShotsCnt;  //Used to count number of pulses completed
    
    
        bool GunRUN; //On/Off control for the LED
        bool A_B;  //Identifies which half of the pulse is currently active
    
    };
    
    typedef struct RabbitPort RabbitPort;
    struct RabbitPort
    {
        volatile unsigned char *OutputPort;  //Pointer to GPIO Output Port
        volatile unsigned char *InputPort;  //Pointer to GPIO Input Port
        uint8_t StartingLED;  //Pin number for the first Rabbit LED
        uint8_t NumLights;  //Number of Rabbit Lights used MAX = 8;
        
        uint32_t msCnt;  //Used to set timing of the Rabbit Lights
        uint8_t ShiftCNT;  //Used to count number of shifts completed so far
        
        bool RabbitRUN;  //On/Off control for Rabbit Lights
        bool A_B;  //Identifies which half of the current pulse is active
    };
    
    // Prototype Function Definitions
    RabbitPort RabbitLightsInit(volatile const unsigned char (* OP), volatile const unsigned char (* IP), const uint8_t SLED, const uint8_t num);
    void StartRabbitLights(RabbitPort *rp);
    void UpdateRabbitLEDs(RabbitPort *rp);
    void StopRabbitLights(RabbitPort *rp);
    
    GunPort GunPortInit_(volatile const unsigned char (* OP), volatile const unsigned char (* IP), const uint8_t GLED, const uint8_t TLED);
    GunPort GunPortInit(volatile const unsigned char (* OP), volatile const unsigned char (* IP), volatile const unsigned char (* TP), volatile const unsigned char (* TIP), const uint8_t GLED, const uint8_t TLED);
    void StartGunFirePattern(GunPort *gp);
    void UpdateGunLED(GunPort *gp);
    void StopGunFirePattern(GunPort *gp);
    
    void StartStrobe(StrobeLED *SLED);
    void StartSyncStrobes(StrobeLED *SLED_A, StrobeLED *SLED_B, uint32_t delay);
    void UpdateStrobeLED(StrobeLED *SLED);
    StrobeLED newStrobe( volatile const unsigned char (* OP), volatile const unsigned char (* IP), const uint8_t sLED, const strobeType type);
    void StopStrobe(StrobeLED *SLED);
    void StopSyncStrobes(StrobeLED *SLED_A, StrobeLED *SLED_B);
    
    


#ifdef	__cplusplus
}
#endif

#endif	/* LED_PATTERNS_H */

