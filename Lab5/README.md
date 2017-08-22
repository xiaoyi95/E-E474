Lab 5: Geophysical Detecting Tank & Webcam Streaming
(Tank with Bluetooth Control and Environment Sensors)

File Names:
startup.sh
basicreq.c
btreceive.c
distsense.c
drive.c
sensor.c
Makefile

Under directory Camera\
Scan
Video.sdp
Folder mjpg-streamer
Record
output.avi

Description:
The idea of this project is to control a tank via Bluetooth that had hardware that could detect its surroundings, and send the information about its surroundings back to the user. This project contains files on the upper level that are used to control the system. The system is a tank powered by an H-bridge driver, that is controlled via character inputs through bluetooth. The user uses a terminal on a laptop to send input to the tank's bluetooth device. A USB controller (Logitech F310) was connected to the laptop, and the D-pad or joystick could be used to control the direction of the tank's movement. The "X" button was used to halt the tank's movement. These buttons/controls were programmed into characters to send through the bluetooth. The tank also features 4 distance sensors on each side. If the distance sensor sensed that an object was too close to the tank, the tank would halt movement in that direction. For example, if the tank is moving forward and it senses an object, it will stop. If the user want the tank wants to move forward but it senses an object, it will not allow the user to move forward.

The tank also utilized the bluetooth to send data back to the user. Using a temperature sensor and a photoresistor, the temperature of the tank's surroundings (default in Fahrenheit) and a brightness level (in percentage) was returned to the user every 3 seconds (time is adjustable). This way the tank could be moved around to pick up different readings on its surroundings. The Makefile is used to compile all of these files together, and the startup script was run on boot, running the programs in a particular order so the programs can send signals to each other. drive.c was used to control the driving function; distsense.c was used for the distance sensor inputs, and sent information to drive.c; btreceive was used for the bluetooth input and sent information to drive.c; and sensor.c was used to send temperature and light input to the user.

Under dirctory Camera are three different methods to capture videos. The initial idea was to stream video back to the user. The methods are described below:
1) Scan and Video.sdp are used to perform live video streaming on the computer using VCL. Video.sdp should be sent to the computer which performs the video. Change the IP address to the address of the receive output video in bash script Scan. Then run the script Scan and until it begins to encode data. Terminate the program and copy everything on the terminal up to “Press ctrl-C to stop encoding” and paste them into Video.sdp. Send Video.sdp file to the computer that displays video. Then run the script Scan again and open Video.sdp file using VLC. The live streams captured by webcam will display in the VLC video player.
2) Folder mjpg-streamer is adapted from a precoded program called mjpg-streamer at https://github.com/shrkey/mjpg-streamer. It gets the mjpeg data from V4L2 and sends it through an HTTP session. Make the file in the folder and run it with bash script Run. Then open the webpage at 192.168.7.2:8080.Then the live video will be displayed on the webpage. 
3) Record is used to record video capture by webcam and store it in a file called output.avi. Run the bash script record and it should begin to record the video. Use ctrl-C to terminate the program and a new file output.avi will be generated in the Beaglebone. Send file from Beaglebone to the computer and then the video can be played. We also include a sample video in the folder.

Contributors:
Tiffany Luu
Xiaoyi Ling
Pezhman Khorasani

Reference:
Stackflow, Linux man page, C library, C manual page, https://github.com/shrkey/mjpg-streamer, http://derekmolloy.ie/
