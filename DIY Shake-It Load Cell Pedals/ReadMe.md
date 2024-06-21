This project consists in a pair of Diy Pedals for Sim Racing designed by me.

Main Component:
- 2x 100kg Load Cell (you can set the force threeshold in the arduino code): https://amzn.eu/d/0fegADWT
- 2x Arduino Pro Micro which act as Joysticks for the pedal position 
- 1x Arduino Mega for Shake-It Vibration Motors
- 2x 775 vibration motors: https://amzn.eu/d/031aityS
- 2x 100mm 14kg steel spring 
- 2x hx711 for load cell signal conversion
- 2x BTS7960 Dc drivers
- 12V 5A (at least) PSU
- USB splitter for connecting the three Arduinos 

In the files you will find the arduino code for the two Arduino Pro Micro
The Arduino Mega needs to be flashed using Simhub. (enable one L298N driver in the Arduino Setup Tool and assign EnA and EnB pin to two PWM capable pin on the Mega, assigned the others to unused pins)

![IMG_1222](https://github.com/ValeriOS03/3D-Printing-/assets/121107647/80305c93-4f55-420b-95a2-82506fdc96c0)
![IMG_1221](https://github.com/ValeriOS03/3D-Printing-/assets/121107647/854bde1b-e5e1-4846-98fd-d5c6acd3a9ac)
![IMG_1225](https://github.com/ValeriOS03/3D-Printing-/assets/121107647/f818fb56-bb6b-4c75-867b-1e1b50dac09e)
![IMG_1223](https://github.com/ValeriOS03/3D-Printing-/assets/121107647/c154c66d-47bb-4947-9433-9b14d345a521)
