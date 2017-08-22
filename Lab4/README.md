Lab 4: Interrupts and Realtime

File Names:
start.sh
startup.sh
drive.c
sense.c

Under directory compononts_tests\
motor_test.c
ds.c
Makefile

Under directory signal_test\
receiver.c
sender.c
send.sh
receive.sh


Description:
This project contains several programs. Files outside the dirctories are used to autodrive a tank using four distance sensors as input to AIN pins after the sensor output has been voltage divided. When there are no obstacles, the tank will move forward. If there are obstacles in the front of the tank, the tank will reverse; if there are obstacles on the back of the tank, the tank will move forward. (Commented out: if there are obstacles on the left, the tank will turn to right; if there are obstacles on the right, the tank will turn to left.) There are LEDs to indicate when something has been sensed on the front, back, right, or left of the tank.

The programs under signal_test are used to test sending signals. The sender file will read input from four AIN pins every 1ms and compute the average values of 100 samples for each. When the average value of one AIN pin is greater than 1000, the sender send a signal chosen from 20, 30, 40 and 50 according to the pin number to the receiver file and print on the screen which signal it sent. In our case, we used 20 for front, 30 for back, 40 for left and 50 for right. In the other terminal, the receiver will wait until it receives signal. Then it will print to the screen which signal it receives among front, back, left and right.

Under component_tests has two seperate programs, motor_test.c is used to test the h-bridge driver for the motors of the tank. It will make the tank to move forward for 3 seconds, move backward for 3 seconds, only move left wheels for 3 seconds, only move right wheels for 3 seconds, and then brake. ds.c is used to test AIN pins, it will display the value read from four AIN pins given by four sensors and display them on the terminal screen in the order of front, back, right and left. 


Contributor:
Tiffany Luu
Xiaoyi Ling
Pezhman Khorasani

Reference:
Stackflow, Linux man page, C library, C manual page
