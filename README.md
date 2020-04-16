# Internet-Radio-Over-IP-Multicast
#Installation for client side
sudo apt-get install libgtk2.0-dev
gcc client.c -o c -pthread `pkg-config --cflags --libs gtk+-2.0`
