#include "mbed.h"
#include "rtos.h"
#include "uLCD_4DGL.h"
#include "DRV2605.h"
//console debugging
Serial pc(USBTX, USBRX);
//manual testing
DigitalIn PISig(p20);
//status lite to confirm it's working
DigitalOut myled(LED1);
//bluetooth com with RPi
RawSerial blue(p13,p14);
//LCD screen
uLCD_4DGL uLCD(p28,p27,p29);
//Vibration motors
PwmOut vib1(p24);
PwmOut vib2(p25);
//LED PINS
PwmOut  redPin(p21);
PwmOut  greenPin(p22);
PwmOut  bluePin(p23);
//haptic motor to simulate
DRV2605 haptics(p9, p10);

volatile char siren;
Mutex lcd_mutex;

void vibrate(){
    while(1){
        while(siren){
            haptics.load_waveform_sequence(123,21,43,18,94,48,112,36);
            haptics.play();
        }
    }
}

void notify(){
    while(1){  
        while(siren){
            lcd_mutex.lock();
            uLCD.color(RED);
            uLCD.locate(0,5);
            uLCD.printf("Siren Detected!");
            uLCD.locate(0,7);
            uLCD.printf("Use Caution!");
            lcd_mutex.unlock();
            Thread::wait(5000);
        }
        lcd_mutex.lock();
        uLCD.cls();
        lcd_mutex.unlock();
        Thread::wait(1000);
    }
}

void led_effect(){
    while(1){
        while(siren){
            //flash three times on LED1
            for(int i=0; i<3; i++) {
                //ramp up brightness level
                for(double x = 0.0; x <= 1.0; x = x+0.2) {
                    redPin = x*x;
                    wait(.02);
                }
            }
            redPin=0.0; //LED1 off
            //flash three times on LED2
            for(int i=0; i<3; i++) {
                //ramp up brightness level
                for(double x = 0.0; x <= 1.0; x = x+0.2) {
                    bluePin = x*x;
                    wait(.02);
                }
            }
            bluePin=0.0; //LED2 off
        }
    }
}

//void bluetooth(){
//    while(1){
//        if(blue.getc()=='1'){
//            siren = 1;
//            Thread::wait(5000);
//        }else{
//            siren = 0;
//        }
//        Thread::wait(1000);
//    }
//}

int main() {
    
    Thread t1(vibrate);
    Thread t2(notify);
    Thread t3(led_effect);
    //Thread t4(bluetooth);
    PISig.mode(PullDown);
    printf("Calibration Result: %X\n",haptics.init(3.3));
    printf("Diagnostics Result: %X\n", haptics.diagnostics());
    while(1) {
        
    
    
        myled = 1;
        wait(0.2);
        myled = 0;
        wait(0.2);
        
        //for simulation purposes, adding a pb to confirm everything works.
        //siren = 0;
        siren = PISig.read();
        pc.printf("siren: %d\n", siren);
        Thread::wait(1000);
        
    }
}
