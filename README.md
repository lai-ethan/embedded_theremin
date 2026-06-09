## AirTone
A digital theremin built with the Mango Pi.

## Team members
Ethan Lai & Jayden Chen

## Project description
We created a theremine using two ultrasonic sensors. In addition to the original theremin experience for continuously adjusting pitch and volume, we added modes to play scales in different keys (discrete pitch), adjust vibrato, and play a song in different keys. For this, we added three additional buttons as well as two slide potentiometers. 

## Member contribution

Ethan worked on code for the ultrasonic sensors, slide potentiometers, and button interrupts as well as running initial tests of the relevant hardware for each sensor. He worked with the I2C module, ADC, and interrupts.

Jayden worked on the code for generating sound, using the speaker, and converting sensor values into frequencies/amplitudes. He worked with the DAC, I2S module, math libraries, and creating the initial main file to bring everything together.

For the final integration, we both worked together extensively on testing the features, debugging various issues, and fine tuning the various capabilities.

## Self-evaluation
We were able to stick to our initial proposal with very minimal adjustments. We were able to divide up the work pretty cleanly and work together well towards the end of to get everything together.

The initial milestone was being able to smoothly control pitch and volume with the ultrasonic sensors. This enabled us to figure out how to integrate polling/interrupts and how frequently we could read from the sensors.

One important breakthrough towards the end was adjusting how we were handling delays / counting clock ticks for the ultrasonic sensor readings. It took several hours of debugging but ended up being a simple fix that solved all the remaining discrepancies with our code. It was definitely a group effort to figure it out and a crucial fix to bring the project over the finish line.

## References
https://projecthub.arduino.cc/tdelatorre/theremino-a-theremin-made-with-arduino-3e661f

## AI Usage
We used AI to learn about the various relevant topics (I2C, I2S, generating sound, etc.) as well as to ask well informed questions for debugging. 

## Photos
