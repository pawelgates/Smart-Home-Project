# Smart Home
### Nuvoton NUC140
![alt text](https://github.com/pawelgates/Smart-Home-Project/blob/main/pics/Arm-cortex-m-nu-lb-nuc140.jpg) 
#### Institute Embedded Systems course project. Imitation of Smart Home that is controlled by Bluetooth application. 
##### System modules: GPIO, Buzzer, Keypad, RGB Leds, LCD, Interrupts, Timers, UART
##### Connected modules: HC05 Bluetooth module, Motor driver L298N, 5V Fan, NPN Transistor, LEDs and Buttons

- 2 LEDs imitating lights
- 5V Fan imitating regular fan
- Door with alarm system and user pincode interface

##### System description:
User has Bluetooth Application with 4 buttons: "Light 1", "Light 2", "FAN", "Door".
"Light 1" and "Light 2" buttons will activate 2 LEDs that are connected through transistors to the GPIO.
"FAN" button will activate 5V fan that is connected to L298N Motor Driver and controlled with GPIO.
"Door" button will activate the LCD and user will be asked to enter the password. If the password is correct, user can open the door without the alarm (Buzzer) for 20 seconds. Indicator will be a Green LED that will turn on during this time. If the door will be opened after 20 sec, alarm system will turn on. When the door is closed, Button is in pressed position. Each time the button is unpressed, system gets an interrupt and checking the door status.

#### System schematic: 

![alt text](https://github.com/pawelgates/Smart-Home-Project/blob/main/pics/tCad1.png)

#### System flowchart: 

![alt text](https://github.com/pawelgates/Smart-Home-Project/blob/main/pics/flowchart.png) 

#### Video:
[![Watch the video](https://i.imgur.com/vKb2F1B.png)](https://youtu.be/CbeWg3qcG9w)

#### Pictures:
![alt text](https://github.com/pawelgates/Smart-Home-Project/blob/main/pics/pic1.png) 
![alt text](https://github.com/pawelgates/Smart-Home-Project/blob/main/pics/pic2.png) 
![alt text](https://github.com/pawelgates/Smart-Home-Project/blob/main/pics/pic3.png) 

 

