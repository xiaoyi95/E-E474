Lab 3: LKMs and Shift Registers

File Names:
Makefile

Under directory LCD\
lcd.c
lcd.h
lcd.ko
test_lcd.c
Makefile

Under directory Sensors\
lcd.c
lcd.h
lcd.ko
sensor.c
Makefile

Description:
This project talks to a LCD display using shift register and a character device driver created from the kernel mode. The program lcd.c built a kernel-mode driver which can receive commands from the user space, and send this input to the LCD using a shift register. The program test_lcd.c is used get user input typed from the keyboard to stdin. This input goes from the user space through the device to the LCD screen. The display has two lines with 16 spaces, and each line of the display can hold up to 40 characters. When 16 characters have been written on the screen, the lines of the display will begin to shift to the left until the characters reach 40 (the end of the line length). The display will then shift back to the front and the rest of the characters will be displayed on the second line. After 56 characters have been inputed, the entire display will begin to shift to the left until 80 characters have been inputted. After 80 characters, the entire display will be cleared and the characters will start from the first line again.

Another program, sensor.c, is used to read environment temperature and brightness using a temprature sensor and a photoresistor. This information is then sent to a modified LCD display driver, which displays the information on the LCD screen. The user can choose to display the tempoerature in Kelvin, Celsius and Fahrenheit, and the brightness is displayed as a percentage. When running the program, the data is updated about every half for a assingned REPEAT number of times, which can be changed in the program.

Contributor:
Tiffany Luu
Xiaoyi Ling
Pejman Khorasani

Reference:
Stackflow, Linux man page, C library, C manual page