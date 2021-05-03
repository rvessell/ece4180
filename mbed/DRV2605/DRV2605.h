/*
    Bryce Williams 10/16/2015
    
    Library for the TI DRV2605L 2 to 5.2 V Haptic Driver for LRA and ERM
    With Effect Library and Smart-Loop Architecture
     
    References:
    http://www.ti.com/product/DRV2605L/description&lpos=Middle_Container&lid=Alternative_Devices
    http://www.ti.com/lit/ds/symlink/drv2605l.pdf (Datasheet) 
    http://www.ti.com/lit/an/sloa189/sloa189.pdf (Setup Guide; SLOA189)
*/

#ifndef DRV2605_H
#define DRV2605_H

#include "mbed.h"

/******************************************************************************
***** DRV2605 Addresses
******************************************************************************/
#define SLAVE_ADDR_7_BIT 0x5A // 7-bit slave address 

/******************************************************************************
****** DRV2605 REGISTERS ******************************************************
******************************************************************************/
#define STATUS                               0x00
#define MODE                                 0x01
#define REAL_TIME_PLAYBACK                   0x02
#define LIBRARY_SELECTION                    0x03
#define WAVEFORM_SEQUENCER_1                 0x04
#define WAVEFORM_SEQUENCER_2                 0x05
#define WAVEFORM_SEQUENCER_3                 0x06
#define WAVEFORM_SEQUENCER_4                 0x07
#define WAVEFORM_SEQUENCER_5                 0x08
#define WAVEFORM_SEQUENCER_6                 0x09
#define WAVEFORM_SEQUENCER_7                 0x0A
#define WAVEFORM_SEQUENCER_8                 0x0B
#define GO                                   0x0C
#define OVERDRIVE_TIME_OFFSET                0x0D
#define POSITIVE_SUSTAIN_TIME_OFFSET         0x0E
#define NEGATIVE_SUSTAIN_TIME_OFFSET         0x0F
#define BRAKE_TIME_OFFSET                    0x10
#define AUDIO_TO_VIBE_CONTROL                0x11
#define AUDIO_TO_VIBE_MINIMUM_INPUT_LEVEL    0x12
#define AUDIO_TO_VIBE_MAXIMUM_INPUT_LEVEL    0x13
#define AUDIO_TO_VIBE_MINIMUM_OUPUT_DRIVE    0x14
#define AUDIO_TO_VIBE_MAXIMUM_OUTPUT_DRIVE   0x15
#define RATED_VOLTAGE                        0x16
#define OVERDRIVE_CLAMP_VOLTAGE              0x17
#define AUTO_CALIBRATION_COMPENSATION_RESULT 0x18
#define AUTO_CALIBRATION_BACK_EMF_RESULT     0x19
#define FEEDBACK_CONTROL                     0x1A
#define CONTROL                              0x1B
#define CONTROL2                             0x1C
#define CONTROL3                             0x1D
#define CONTROL4                             0x1E
#define CONTROL5                             0x1F
#define LRA_OPEN_LOOP_PERIOD                 0x20
#define VBAT_VOLTAGE_MONITOR                 0x21
#define LRA_RESONANCE_PERIOD                 0x22

class DRV2605{
    
    public:
        //// modes defines the possible modes of the DRV2605L
        enum Mode{
            INTERNAL_TRIG,   // 0x00: Waveforms fired by Setting GO bit in Register 0x0C
            EXTERNAL_EDGE,   // 0x01: Rising Edge on IN/TRIG pin set GO Bit.
            EXTERNAL_LEVEL,  // 0x02: GO bit follows state of edge on IN/TRIG pin. 
            PWM_ANALOG,      // 0x03: PWM or Analog Signal accepted at IN/TRIG pin.
            AUDIO_TO_VIBE,   // 0x04: An AC-coupled audio signal is accepted at the IN/TRIG pin.
            RTP,             // 0x05: Real- Time Playback
            DIAG,            // 0x06: Set to perform actuator diagnostics
            AUTO_CAL,        // 0x07: Set to perform auto calibration of device for actuator 
            STANDBY,  // 0x40: Set Device to Software Standby (Low- Power Mode)
            DRESET    // 0x80: Reset Device (equivalent of power cycling the device)
         };
         
         //// FeedBack_Controls Fields Bitmasks (Register Addr: 0x1A)
         enum Actuator_Type{ERM = 0, LRA = 0x80}; // bit-7
         enum Brake_Factor{x1 = 0x00, x2 = 0x10, x3 = 0x20, 
                           x4 = 0x40, x6 = 0x80, x8 = 0x50, 
                           x16 = 0x60, DDISABLE = 0x70}; // bit-6..4
         enum Loop_Gain{LOW = 0x00, 
                        MED = 0x04, 
                        HIGH = 0x08, 
                        VERY_HIGH = 0x0C};  // bit-3..2
                    
         enum Library{EMPTY, A, B, C, D, E,
                      LRA_LIB, F};              // ROM Waveform Library Selections
        
        /**
            Constructor for DRV2605 Objects
        */
        DRV2605(PinName sda, PinName scl);
        
        /**
            Write value to specified register of device
            @param reg      The device register to write 
            @param value    The value to write to the register
        */
        void i2cWriteByte(char reg, char value);
        
        /**
            Read value from register of device
            @param reg  The device register to read
            @return     The result
        */
        uint8_t i2cReadByte(char reg);
        
        /**
            Place device into specified mode
            @param mode     The mode to place device into
        */
        void mode(Mode mode);
        
        /**
            TODO: Expand to allow initialization for LRAs and Closed Loop operation 
            Initialize the device for Open- Loop ERM mode using specified ROM 
            Waveform Library as specified in Section 9.3 of Device Datasheet.
            See also Device Setup Guide 1.6.1 ERM Initialization Example
            @param actuator_peak_voltage    The Peak Voltage Rating of Actuator
            @param lib  The ROM Waveform Library to use
        */
        int init(float actuator_peak_voltage, Library lib = B);
        
        /**
            Runs diagnostics on the Actuator and Device and returns the results. 
            The results indicate if an actuator is detected, over- current events, 
            etc. Refer to STATUS Register (0x00) in device datasheet for more 
            description register values.
            
            Note: This should be run if the user is having trouble getting the actuator 
            to work.
            
            @return The results of the diagnostics (i.e. Status Reg (0x00))
        */
        uint8_t diagnostics();
        
        /**
            Play single waveform from ROM Library as outlined in Section 9.3.2.1
            of Device Datasheet.
            The library used is the one that is currently written 
            to the Library_Selection Register (0x03). This library
            is set in the init(Library lib) method, but can be 
            changed manually.
            @param waveform_effect  The Waveform Effect Library Index value to play
                                    (valid values are 1 to 123)
        */
        void play_waveform(int waveform_effect);
        
        /**
            Load Wave Sequence into DRV2605 Sequence Registers
            @param effect1... effect8   The effect to play. Valid inputs are 
                                        0 to 123; 0: Stop Condition, 
                                                  1- 123: Waveform Index
        */
        void load_waveform_sequence(int effect1 = 0, int effect2 = 0, 
                                    int effect3 = 0, int effect4 = 0,
                                    int effect5 = 0, int effect6 = 0, 
                                    int effect7 = 0, int effect8 = 0);
        
        /**
            Plays the currently loaded waveform or waveform sequence. 
            Call this after calling play_waveform() or after calling 
            load_waveform_sequence()
            Preconditions: User must have already loaded waveform(s) 
                           using play_waveform() or load_waveform_sequence()
        */
        void play();
        
        /**
            TODO: Add Closed Loop Calibration
            
            Run basic DRV2605L Auto- Calibration as detailed in Section 2 of 
            the Device Setup Guide for OPEN- LOOP ONLY.
            This must be done before using the device in closed- loop mode 
            (unless cal has been done before with values stored in non-volatile
             mem; see datasheet for more info). 
                
            NOTE: It is NOT recommended to store cal values into device 
            non-volatile memory as this can be done only once. Thus do not 
            use this feature unless the device is being used in a final 
            project AND values have been confirmed to result in satisfactory 
            performance).
                
            This uses many of the default device register values such as
            the default DRIVE_TIME .

            @param actuator_peak_voltage The maximum/peak voltage rating of the actuator
        */
        uint8_t auto_cal_open_loop(float actuator_peak_voltage);        
          
    private:
    I2C i2c;
    
};

#endif