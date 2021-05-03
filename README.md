## Background

The purpose of this project is to create a proof of concept for a Machine Learning based system which identifies sirens and alerts drivers with hearing impairments.

The inspiration for this device came from personal experience. I was riding with my grandfather who is deaf, an ambulance came up behind him and he could not hear the siren and had not seen the lights yet. By the time I alerted him to the ambulance, we had already slowed them down. If there was is a way that we could detect sirens for the hearing impared, we can hopefully save them valuable time when responding to an emergency.

## Hardware Used

Raspberry Pi 4: This application was tested on a Raspberry Pi 4 Model BCM2711, Revision c03111, running RaspianOS. This code will likely work with other version of Pi but has not been tested. This device is primarily used as the inference engine. It collects audio samples froma a USB microphone, runs the TensorFlow Lite model inference on the sample, and produces a percentage of classification for each label supplied to the model. In the first itteration, this is only traffic noise and sirens. Once a siren is detected, a GPIO pin is used to send a signal to a GPIO pin on the LPC1768.

USB Microphone: I used SunFounder USB 2.0 Mini Microphone, however, any ALSA compatible microphone will work. This device is used to capture 1/4 of a second of sound to feed to the model.

LPC1768: The main purpose of this device is to drive the output devices. I originally intended to use this device as the inference engine as well as the controller for the output devices. What I learned is that my model needed to be held in memory and was too large for the 32 KB that this device has.

ULCD-144-G2 Serial Miniture LCD Module: This module is used to display a text alert when a siren is detected. This is meant to simulate the LCD screens commonly found in newer model vehicles.

DRV2605 Haptics Driver: This module was used to drive a small haptic vibration motor when a siren is detected. This is meant to simulate the seat rumble/vibration feature commonly found in newer vehicles. This was used in combination with DZS Elec Mini Vibration Motors.

RGB LED: Used to flash blue and red when a siren is detected. This is meant to simulate the LED indicators commonly found in newer vehicles.


# PI Instructions

Once you have downloaded the project, you can build the project by using

```
$ APP_AUDIO=1 make -j
```

To run this project, you will need to use SUDO for the ALSA access. You will also need to know which hardware device the USB device is using. The command below uses plughw:0,0, which means device 0, subdevice 0.

```
$ cd /build
$ sudo ./audio plughw:0,0
```

# MBED Instructions

The MBED code can either be retrieved from the mbed folder in this Git or downloaded from https://os.mbed.com/users/rvessell/code/4180FinalProject/

MBED OS2 is all that is needed for this project. Once built and loaded, the mbed will look for a signal on P20 every second and respond accordingly.

## Demo Video

You can find a quick demo video [here](https://youtu.be/7Q74M9Q2uXY).

# Images

## Schematic

![](https://github.com/rvessell/ece4180/blob/master/imgs/Schematic.PNG)

## Images

Overhead View:
![](https://github.com/rvessell/ece4180/blob/master/imgs/Overhead.jpg)

DRV2605L
![](https://github.com/rvessell/ece4180/blob/master/imgs/DRV2605L.jpg)

ULCD-144G2
![](https://github.com/rvessell/ece4180/blob/master/imgs/ULCD144G2.jpg)

LPC1768
![](https://github.com/rvessell/ece4180/blob/master/imgs/LPC1768.jpg)
