# AirTone

## Team members
- Jayden Chen
- Ethan Lai

## Project goals

The inspiration for our project is to create a basic Theremin using speakers and an ultrasound sensor. As a baseline, we hope to be able to control pitch using our hand's distance from the sensor. For extension, we could add other sensors such as sliders, buttons, or potentially an accelerometer / gyroscope to add reverb or simulate different sounds. 

## Resources, budget

### Core:
- I2S to analog: PCM5102 and/or MAX98357A
- Analog to I2C: ADS1115
- Ultrasonic Sensor: WWZMDiB
- Speakers (salvaged from lab64 / home parts)

### Additions:
- Potentiometers
- Buttons
- Touch sensor
- Accelerometer / Gyroscope

## Tasks, member responsibilities
- Tone generation / sound effects using I2S (Jayden)
- Sensor reading / calibration using I2C (Ethan)
- Integration using interrupts (Jayden / Ethan)

## Schedule, midpoint milestones
- Milestone 1 (05/29 EOD): Software using existing modules for I2S and I2C 
- Milestone 2 (06/02 EOD): Initial integration with ultrasonic sensor
- Milestone 3 (06/05 EOD): Addition of potentiometers, switches, other sensors / effects
- Milestone 4 (06/07 EOD): Integration / casing complete
- Milestone 5 (06/09 EOD): Final presentation prep

## Additional resources, issues
- We will be using the provided I2S and I2C modules.