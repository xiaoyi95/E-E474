Project name:
music

Description:
This project contains two files: a C source file called music.c and a Makefile. The Makefile is used to make the executable program music from music.c and the program music will run on the Beaglebone black board.

When the program music is running, three LEDs on the board will light up in order of the number counting up from 0 to 7 in three-bit binary number. The green LED represents the lowest bit (1), the blue LED represents the middle bit (2), and the red LED represents the highest bit (4). For example, when the count is 0 the LEDS display 000 so none will light up. When the count is 5 the LEDS should display 101, so the green and red LEDs will light up. As the LEDs light up for each number counted, a PWM pin will also output to a buzzer, producing different sounds simulating music notes. We chose to play the note middle C at count 0, and goes up to D, E, F, G, A, B when the count increments until a higher note C.

The repeat time of the counts can be changed using the value REPEAT. When REPEAT is 3, the program will count from 0 to 7 three times and terminate automatically. The volume of the sounds can also be determined by the duty cycle, it is set to be the period divided by the value VOLUME. The value VOLUME should be smaller than the lowest period 1911128. The program will fail and exit if one of the files cannot be opened properly. 

Contributor:
Tifffany Luu
Xiaoyi Ling
Pejman Khorasani
