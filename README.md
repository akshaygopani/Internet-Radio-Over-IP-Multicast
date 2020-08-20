# Internet-Radio-Over-IP-Multicast

Supportable OS  -Ubuntu.

Installation for client side

sudo apt-get install libgtk2.0-dev


Serever side setup your song/video file directory in below mention array which is available in server.c file

char s1[]="/home/ajaybechara1/Desktop/CN_PROJECT/Final/SX";

command to run client file

gcc client.c -o c -pthread `pkg-config --cflags --libs gtk+-2.0`

sudo ./c serverIP portnumber wlp2s0

command to run server file

gcc server.c -o s -pthread

./s
