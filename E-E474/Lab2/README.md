Lab 2: Inputs, Outputs, and Time
Writing to an LCD Display

File names:
commands.c
commands.h
fileinput.c
filemusic.c
input.c
lcd.c
osc.c

Description:
This project contains two sub-projects. The first one is to perform a scheduler jitter test by makeing the voltage of a GPIO pin go low and high every 1 us. This output was displayed on the oscilloscope and on a blue LED. This program is stored in osc.c
The main sub-project is to talk to a LCD display. The program lcd.c talks to the LCD display. The LCD will display whatever the user inputs from the keyboard. This input goes from the user through a pipe to the LCD output. The display has two lines with 16 spaces, and each line of the display can hold up to 40 characters. When 16 characters have been inputted, the lines of the display will begin to shift to the left until the characters reach 40. Then the display will shift back to the front and the rest of the characters will be displayed on the second line. After 56 characters have been inputed, the entire display will begin to shift to the left until 80 characters have been inputted. After 80 characters, the entire display will be cleared and the characters will start from the first line again.

Another program, filemusic.c, is used to play music using formatted files inputted from fileinput.c. 26 upper-case characters represent 26 music notes from A2 to E5. The program can read from a music file and output the music coresponding to each letter. The content in the file will be displayed on the screen. Even we used 26 letters to simulate notes, the LCD will only display the corresponding A B C D E F G note according to its musical pitch. Three text files called mary, twinkle, and row are included, and they perform "Mary had a little lamb", "Twinkle Twinkle little star", and "Row your boat" respectively. Note that the program can also handle multiple file inputs from the teminal. 

Contributor:
Tifffany Luu
Xiaoyi Ling
Pejman Khorasani

Reference:
Stackflow, Linux man page, C library, C manual page