# Internet-Radio-Over-IP-Multicast
Installation for client side
sudo apt-get install libgtk2.0-dev
gcc client.c -o c -pthread `pkg-config --cflags --libs gtk+-2.0`

Serever side setup your song/video file directory in below mention array which is available in server.c file
char s1[]="/home/ajaybechara1/Desktop/CN_PROJECT/Final/SX";
