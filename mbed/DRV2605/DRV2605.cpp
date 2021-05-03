#include "DRV2605.h"

DRV2605::DRV2605(PinName sda, PinName scl): i2c(sda, scl){
        wait_us(250);   // Device datasheet specified wait time before I2C
                        // comms should be used
}

void DRV2605::i2cWriteByte(char reg, char value){
    char buff[2] = {reg, value};
    i2c.write(SLAVE_ADDR_7_BIT<<1, buff, 2);
}

uint8_t DRV2605::i2cReadByte(char reg){
    char result;                                    // Temp result storage
    i2c.write(SLAVE_ADDR_7_BIT<<1, &reg, 1, true);  
    i2c.read(SLAVE_ADDR_7_BIT<<1, &result, 1);
    
    return result;         
}

void DRV2605::mode(Mode mode){
    i2cWriteByte(MODE, mode);
}

int DRV2605::init(float actuator_peak_voltage, Library lib){
    int result = 
        auto_cal_open_loop(actuator_peak_voltage); // Perform Open-Loop ERM Cal
    i2cWriteByte(LIBRARY_SELECTION, lib);     // Select ROM Library
    mode(STANDBY);                            // Put device into low- power mode
    return result;                            // Cal/Init Result 0: Pass 1: Fail
}

uint8_t DRV2605::diagnostics(){
    mode(DIAG);
    i2cWriteByte(GO, 1);
    while(i2cReadByte(GO));     // Wait for GO bit to clear
    
    return i2cReadByte(STATUS); // Return Status Reg Value
}

void DRV2605::play_waveform(int waveform_effect){
    mode(INTERNAL_TRIG);                                 // Bring device out of standby and set to internal trigger
    i2cWriteByte(WAVEFORM_SEQUENCER_1, waveform_effect); // Load waveform index to play
    i2cWriteByte(WAVEFORM_SEQUENCER_2, 0);               // Insert stop condition so we don't play other registers if filled
    i2cWriteByte(GO, 0x01);                              // Set GO bit to start playback
}

void DRV2605::load_waveform_sequence(int effect1, int effect2, 
                                     int effect3, int effect4,
                                     int effect5, int effect6, 
                                     int effect7, int effect8){
    i2cWriteByte(WAVEFORM_SEQUENCER_1, effect1);
    i2cWriteByte(WAVEFORM_SEQUENCER_2, effect2);
    i2cWriteByte(WAVEFORM_SEQUENCER_3, effect3);
    i2cWriteByte(WAVEFORM_SEQUENCER_4, effect4);
    i2cWriteByte(WAVEFORM_SEQUENCER_5, effect5);
    i2cWriteByte(WAVEFORM_SEQUENCER_6, effect6);
    i2cWriteByte(WAVEFORM_SEQUENCER_7, effect7);
    i2cWriteByte(WAVEFORM_SEQUENCER_8, effect8);                                         
}

void DRV2605::play(){
    i2cWriteByte(MODE, INTERNAL_TRIG);      // Internal Trigger Mode
    i2cWriteByte(GO, 1);
}

uint8_t DRV2605::auto_cal_open_loop(float actuator_peak_voltage){
    // Exit Standby Mode; Enter Auto- Cal Mode
    mode(AUTO_CAL);
    
    /* Set the following registers to the appropriate values:
        • Rated Voltage (0x16)
        • Overdrive Voltage (0x17)
        • Feedback Control (0x1A) – Bits [1:0] can be left blank and will be
          populated by the auto-calibration engine
        • Control 1 (0x1B), Control 2 (0x1C), and Control 3 (0x1D)
        • Mode (0x01) – Set mode to Auto-Calibration
        • Auto-calibration Memory Interface (0x1E) – the auto-calibration time 
          can be increased to improve calibration, but can be left as default 
          for the initial calibration
    */ 
    // Rated Voltage (0x16) not referenced for open-loop operation, skip calc
    
    // Calc and Set Overdrive Voltage Register (0x17)
    int od_clamp = (int)(((actuator_peak_voltage*255)/5.6)+0.5);
    i2cWriteByte(OVERDRIVE_CLAMP_VOLTAGE,
                 i2cReadByte(OVERDRIVE_CLAMP_VOLTAGE) | od_clamp);
    
    // Set Feedback Control Register (0x1A), use default values
    
    
    // Set Control 1 Register (0x1B), use default values
    
    
    // Set Control 2 Register (0x1C), use default values
    
    // Set Control3 Register (0x1D), Set to ERM Open-Loop Operation 
    i2cWriteByte(CONTROL3, 0xA0);   

    // Set Control 4 Register (0x1E)                
    i2cWriteByte(CONTROL4,
                 i2cReadByte(CONTROL4 | 0x30)); // Max Calibration Time
    
    // Device already set to Auto- Cal Mode at top of this code block
    
    // Start auto- calibration
    i2cWriteByte(GO, 0x01);
    
    // Wait for calibration to complete
    while(i2cReadByte(GO));
    
    // Read and return DIAG_RESULT in Status Register (0x00)
    return (i2cReadByte(STATUS)); // Return the Diag_Result Bit Result
}
        
