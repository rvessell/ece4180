The purpose of this project is to create a proof of concept for a Machine Learning based system which identifies sirens and alerts drivers with hearing impairments.

## Hardware Used

Raspberry Pi 4: This application was tested on a Raspberry Pi 4 Model BCM2711, Revision c03111, running RaspianOS. This code will likely work with other version of Pi but has not been tested. This device is primarily used as the inference engine. It collects audio samples froma a USB microphone, runs the TensorFlow Lite model inference on the sample, and produces a percentage of classification for each label supplied to the model. In the first itteration, this is only traffic noise and sirens. Once a siren is detected, a GPIO pin is used to send a signal to a GPIO pin on the LPC1768.

USB Microphone: I used SunFounder USB 2.0 Mini Microphone, however, any ALSA compatible microphone will work. This device is used to capture 1/4 of a second of sound to feed to the model.

LPC1768: The main purpose of this device is to drive the output devices. I originally intended to use this device as the inference engine as well as the controller for the output devices. What I learned is that my model needed to be held in memory and was too large for the 32 KB that this device has.

ULCD-144-G2 Serial Miniture LCD Module: This module is used to display a text alert when a siren is detected. This is meant to simulate the LCD screens commonly found in newer model vehicles.

DRV2605 Haptics Driver: This module was used to drive a small haptic vibration motor when a siren is detected. This is meant to simulate the seat rumble/vibration feature commonly found in newer vehicles. This was used in combination with DZS Elec Mini Vibration Motors.

RGB LED: Used to flash blue and red when a siren is detected. This is meant to simulate the LED indicators commonly found in newer vehicles.

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
