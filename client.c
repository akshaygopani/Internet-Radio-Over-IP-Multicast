/*

sudo apt-get install libgtk2.0-dev
gcc client.c -o c -pthread `pkg-config --cflags --libs gtk+-2.0`
sudo ./c 0.0.0.0 5432 wlp2s0
sudo kill -9 $(sudo lsof -t -i:5464)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include <gtk/gtk.h>
 
struct args
{
        int port;
        char* ip;
        char* name;
};

struct args ptr;
//ptr=(struct args*)(malloc(sizeof(struct args)));

//#define MC_PORT 1723   //  1723 , 5499
#define BUF_SIZE 4096
int MC_PORT = 41054;
int INFO_PORT = 1723;
int true;
int choice;
int dava2 = 0;
int currentStationNumber;
pthread_t thread_id_song,thread_id_song_info;
pthread_t thread_id_vlc;
int flag = 1;
int fffff=0;
int s; /* socket descriptor */
int sInfo;
int change;
int songInfoWindowGTK=1;
GtkWidget *labelGtkSongInfo;
//char buf[BUF_SIZE];
void *myThreadFun(void *vargp);
void *startStream(void *vargp);
void *tcp(void *vargp);
void *infoThread(void *vargp);
struct ip_mreq mcast_req;
struct sockaddr_in mcast_saddr;
socklen_t mcast_saddr_len;
int myflag=1;
GtkWidget *windowStationSelect;

struct station_info{
        uint8_t  station_number;
        uint8_t  station_name_size;
        char     station_name[256];
        uint32_t multicast_address;
        uint16_t data_port;
        uint16_t info_port;
        uint32_t bit_rate;
};

typedef struct {
        uint8_t  type;
        uint8_t  site_name_size;
        char     site_name[256];
        uint8_t  site_desc_size;
        char     site_desc[256];
        uint8_t  station_count;
        struct station_info station_list[10] ;
}site_info;

typedef struct {
        uint8_t type;
        uint8_t song_name_size ;
        char song_name[ 256 ];
        uint16_t remaining_time_in_sec;
        uint8_t next_song_name_size;
        char next_song_name[ 256 ];
}song_info;

song_info current_song_info;

void *stationInfo(void *vargp);

static void oneListener  (){
        choice = 1;
        if(flag==1){
                pthread_create(&thread_id_song, NULL, startStream,NULL);
                sleep(1);
               pthread_create(&thread_id_song_info, NULL, infoThread,NULL);                
        }
        else
                dava2 = 3       ; 
        gtk_widget_destroy(windowStationSelect);     
}

static void twoListener  (){
        choice = 2;
        if(flag==1){
                pthread_create(&thread_id_song, NULL, startStream,NULL);
                sleep(1);
                pthread_create(&thread_id_song_info, NULL, infoThread,NULL);
        }
        else
                dava2 = 3;       
        gtk_widget_destroy(windowStationSelect);     
}

static void threeListener  (){
        choice = 3;
        if(flag==1){
                pthread_create(&thread_id_song, NULL, startStream,NULL);
                sleep(1);
                pthread_create(&thread_id_song_info, NULL, infoThread,NULL);
        }else
                dava2 = 3;       
        gtk_widget_destroy(windowStationSelect);     
}

static void fourListener  (){
        choice = 4;
        if(flag==1){
                pthread_create(&thread_id_song, NULL, startStream,NULL);
                sleep(1);
                pthread_create(&thread_id_song_info, NULL, infoThread,NULL);
        }else
                dava2 = 3;       
        gtk_widget_destroy(windowStationSelect);     
}

static void fiveListener  (){
        choice = 5;
        if(flag==1){
                pthread_create(&thread_id_song, NULL, startStream,NULL);
                sleep(1);
                pthread_create(&thread_id_song_info, NULL, infoThread,NULL);
        }else
                dava2 = 3;  
        gtk_widget_destroy(windowStationSelect);     
}

static void selectStationListener (GtkWidget *widget,gpointer *data)
{
        GtkWidget *vbox;

        GtkWidget *one;
        GtkWidget *two;
        GtkWidget *three;
        GtkWidget *four;
        GtkWidget *five;
        GtkWidget *label;

        void print_s (){};
        void print_a (){};
        void print_l (){};
        void print_c (){};
        void print_d (){};

        windowStationSelect = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_position(GTK_WINDOW(windowStationSelect), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(windowStationSelect), 230, 250);
        gtk_window_set_title(GTK_WINDOW(windowStationSelect), "Select Station");
        gtk_container_set_border_width(GTK_CONTAINER(windowStationSelect), 5);

        vbox = gtk_vbox_new(TRUE, 1);
        gtk_container_add(GTK_CONTAINER(windowStationSelect), vbox);

        one = gtk_button_new_with_label("1");
        two = gtk_button_new_with_label("2");
        three = gtk_button_new_with_label("3");
        four = gtk_button_new_with_label("4");
        five = gtk_button_new_with_label("5");

        gtk_box_pack_start(GTK_BOX(vbox), one, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), two, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), three, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), four, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), five, TRUE, TRUE, 0);

        g_signal_connect (one, "clicked", G_CALLBACK (oneListener), NULL);
        g_signal_connect (two, "clicked", G_CALLBACK (twoListener), NULL);
        g_signal_connect (three, "clicked", G_CALLBACK (threeListener), NULL);
        g_signal_connect (four, "clicked", G_CALLBACK (fourListener), NULL);
        g_signal_connect (five, "clicked", G_CALLBACK (fiveListener), NULL);

        gtk_widget_show_all(windowStationSelect);

}

static void stationInfoListener  (){
        pthread_create(&thread_id_song,NULL,tcp,NULL);
        sleep(2);
}

static void pauseListener (){
        dava2 = 2;
}

static void restartListener (){
        dava2 = 1;
}

static void exit00Listener (){
        system("pidof ffplay | xargs -I{} kill {}");
        exit(0);
}

void *startStream(void *vargp){
////////////////////////////////////////////////////////////////////////////////////////////////////
                        char buf[BUF_SIZE];
        struct ip_mreq mcast_req;
        struct sockaddr_in mcast_saddr;
        socklen_t mcast_saddr_len;
        
        char *if_name;        
        if_name = ptr.name; 

        if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("receiver: socket");
                exit(1);
        }
        
        struct sockaddr_in sin;
        memset((char *)&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = htonl(INADDR_ANY);
        sin.sin_port = htons(MC_PORT);
        
        if ((bind(s, (struct sockaddr *) &sin, sizeof(sin))) < 0) {
                perror("receiver: bind()");
                exit(1);
        }

        struct ifreq ifr;
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name , if_name, sizeof(if_name)-1);
 
        if ((setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr,sizeof(ifr))) < 0){
                perror("receiver: setsockopt() error");
                close(s);
                exit(1);
        }

        int iSetOption = 1;
        setsockopt(s , SOL_SOCKET , SO_REUSEADDR ,(char*)&iSetOption , sizeof(iSetOption)); 

//////////////////////////////////////////////////////////////////////////////////////////////////////
                
        currentStationNumber = choice;             
        char addressnew[]="239.192.xx.11";
        addressnew[8]=(currentStationNumber/10)+'0';
        addressnew[9]=(currentStationNumber%10)+'0';
        
        char *mcast_addr; 
        mcast_addr= (char *)&addressnew;

        mcast_req.imr_multiaddr.s_addr = inet_addr(mcast_addr);
        mcast_req.imr_interface.s_addr = htonl(INADDR_ANY);

        if ((setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP,    (void*) &mcast_req, sizeof(mcast_req))) < 0) {
                printf("%s Not able to join \n",mcast_addr);
                perror("mcast join receive: setsockopt()");
                exit(1);
        }else{
                printf("%s Multicast Group Joined \n",mcast_addr);
        }
        
        
        int temp = choice;
        
        FILE *fp;
        fp=fopen("recv.mp3","wb");
        fclose(fp);   

        int cnt=0;
        int count=1;
        
        while(1) {
                
                if(dava2 > 0){
                        
                        // dava2 == 3 --> station change
                        // dava2 == 1 --> restart
                        // dava2 == 2 --> pause
                        
                        
                        if(dava2 == 3 || dava2 == 2){
                               
                                if ((setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void*) &mcast_req, sizeof(mcast_req))) < 0){
                                        printf("%s Not able to drop \n",mcast_addr);
                                        perror("mcast join receive: setsockopt()");
                                        exit(1);           
                                }else{
                                        printf("%s Droped Successfully \n",mcast_addr);
                                        fp=fopen("recv.mp3","wb");
                                        fclose(fp);   
                                        system("pidof ffplay | xargs -I{} kill {}");
                                }
                                
                                if(dava2 == 2){
                                        while( dava2 != 1 && dava2 != 3);
                                }
                                count = 0;
/*                                if(dava2 == 2)
                                        dava2 = 0;
*/
                        }
                        
                        if(dava2 == 3 || dava2 == 1){
                                currentStationNumber = choice;             
                                char addressnew[]="239.192.xx.11";
                                addressnew[8]=(currentStationNumber/10)+'0';
                                addressnew[9]=(currentStationNumber%10)+'0';
                                
                                char *mcast_addr; 
                                mcast_addr= (char *) &addressnew;

                                mcast_req.imr_multiaddr.s_addr = inet_addr(mcast_addr);
                                mcast_req.imr_interface.s_addr = htonl(INADDR_ANY);

                                if ((setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP,    (void*) &mcast_req, sizeof(mcast_req))) < 0){
                                        printf("%s Not able to join  \n",mcast_addr);
                                        perror("mcast join receive: setsockopt()");
                                        exit(1);
                                }else{
                                        printf("%s Multicast Group Joined \n",mcast_addr);
                                        fp=fopen("recv.mp3","wb");
                                       // count = 0;
                                        fclose(fp);
                                }
                                
                            dava2 = 0;
                        }
                        
                }
        
                memset(&mcast_saddr, 0, sizeof(mcast_saddr));
                mcast_saddr_len = sizeof(mcast_saddr);


                memset(buf, 0, sizeof(buf));
                int len;
                if ((len = recvfrom(s, buf, BUF_SIZE, 0, (struct sockaddr*)&mcast_saddr, &mcast_saddr_len)) < 0) {
                        perror("receiver: recvfrom()");
                        exit(1);
                }

                        fp=fopen("recv.mp3","a+b");
                        fwrite(buf,sizeof(buf),1,fp);
                        fclose(fp);
                                               
                        if(count==3 ){
                                pthread_create(&thread_id_vlc, NULL, myThreadFun, NULL);
                                flag = 0;                                
                        }
                        //myflag=0;
                        cnt++;
                        count++;
                
                memset(buf, 0, sizeof(buf));
        }
        
        
        close(s);
}

void *infoThread(void *vargp){
////////////////////////////////////////////////////////////////////////////////////////////////////
        int s;
        char *if_name;        
        if_name = ptr.name;  //take input from command line

        if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("receiver: socket");
                exit(1);
        }
        
        struct sockaddr_in sin;
        memset((char *)&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = htonl(INADDR_ANY);
        sin.sin_port = htons(INFO_PORT);
        
        if ((bind(s, (struct sockaddr *) &sin, sizeof(sin))) < 0) {
                perror("receiver: bind()");
                exit(1);
        }

        struct ifreq ifr;
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name , if_name, sizeof(if_name)-1);
 
        if ((setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr,sizeof(ifr))) < 0){
                perror("receiver: setsockopt() error");
                close(s);
                exit(1);
        }

        int iSetOption = 1;
        setsockopt(s , SOL_SOCKET , SO_REUSEADDR ,(char*)&iSetOption , sizeof(iSetOption)); 

//////////////////////////////////////////////////////////////////////////////////////////////////////


                GtkWidget *window_p;
                GtkWidget *label;
                window_p = gtk_window_new(GTK_WINDOW_TOPLEVEL);
                gtk_window_set_position(GTK_WINDOW(window_p), GTK_WIN_POS_CENTER);
                gtk_window_set_title(GTK_WINDOW(window_p), "Song Information");
                gtk_container_set_border_width(GTK_CONTAINER(window_p), 15);

                label = gtk_label_new("BHAI BHAI BHAI BHAI BHAI BHAI BHAI BHAI BHAI BHAI");

                gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
                gtk_container_add(GTK_CONTAINER(window_p), label);
                int firstTime = 0;
        
        while(1) {

                char buf[BUF_SIZE];
                memset(buf, 0, sizeof(buf));
                int len;
                
                if ((len = recvfrom(s, &current_song_info, sizeof(current_song_info) , 0, (struct sockaddr*)&mcast_saddr, &mcast_saddr_len)) < 0) {
                        perror("receiver: recvfrom()");
                        exit(1);
                }else{
                        printf("Current Song = %s\n",current_song_info.song_name);
                        printf("Next Song = %s\n",current_song_info.next_song_name);   
                        printf("Left Time = %d\n",current_song_info.remaining_time_in_sec);   

                        if(firstTime){
                                gtk_widget_show_all(window_p);
                                printf("Oh Oh\n");
                        }
                        firstTime = 0;
                        
/*                        char stationInfoDD[1024];
                        
                        sprintf(stationInfoDD, "-----------------------------------------------------------------------------------\n\nCurrent Song : %s\n\n-----------------------------------------------------------------------------------\n\nNext Song : %s\n\n-----------------------------------------------------------------------------------",current_song_info.song_name,current_song_info.next_song_name);
                        labelGtkSongInfo = gtk_label_new(stationInfoDD);
*/
                           
                }
                
                memset(buf, 0, sizeof(buf));
        }
        
        
        close(s);
}


void *tcp(void *vargp){
        FILE *fp;
        struct hostent *hp;
        struct sockaddr_in sin;
        char *host;
        char buf[256];
        int s;
        int len;

        host = ptr.ip;
        hp = gethostbyname(host);
        if (!hp){
                fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
                exit(1);
        }
        else{
                //printf("Client's remote host:");
        }

        bzero((char *)&sin, sizeof(sin));
        sin.sin_family = AF_INET;
        bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
        sin.sin_port = htons(ptr.port);

        if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
                perror("simplex-talk: socket");
                exit(1);
        }
        else{
                //printf("Client created socket.\n");
        }

        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
                perror("simplex-talk: connect");
                close(s);
                exit(1);
        }
        else{
                //printf("Client connected.\n");
        }

        while(1){

                site_info mysite;
                recv(s,&mysite,sizeof(mysite),0);
                
                int stationCount = mysite.station_count;
                
                char stationInfoDD[1024];
                int i=0,j=1,k=2,l=3,m=4;
                
                sprintf(stationInfoDD, "-----------------------------------------------------------------------------------\n\nStation Number : %d\nStation Name : %s\nMulticast Address : %d\nData Port : %d\nInfo Port : %d\nBitrate : %d\n\n-----------------------------------------------------------------------------------\n\nStation Number : %d\nStation Name : %s\nMulticast Address : %d\nData Port : %d\nInfo Port : %d\nBitrate : %d\n\n-----------------------------------------------------------------------------------\n\nStation Number : %d\nStation Name : %s\nMulticast Address : %d\nData Port : %d\nInfo Port : %d\nBitrate : %d\n\n-----------------------------------------------------------------------------------\n\nStation Number : %d\nStation Name : %s\nMulticast Address : %d\nData Port : %d\nInfo Port : %d\nBitrate : %d\n\n-----------------------------------------------------------------------------------",
                mysite.station_list[i].station_number,mysite.station_list[i].station_name,mysite.station_list[i].multicast_address,mysite.station_list[i].data_port,mysite.station_list[i].info_port,mysite.station_list[i].bit_rate,
                mysite.station_list[j].station_number,mysite.station_list[j].station_name,mysite.station_list[j].multicast_address,mysite.station_list[j].data_port,mysite.station_list[j].info_port,mysite.station_list[j].bit_rate,
                mysite.station_list[k].station_number,mysite.station_list[k].station_name,mysite.station_list[k].multicast_address,mysite.station_list[k].data_port,mysite.station_list[k].info_port,mysite.station_list[k].bit_rate,

mysite.station_list[l].station_number,mysite.station_list[l].station_name,mysite.station_list[l].multicast_address,mysite.station_list[l].data_port,mysite.station_list[l].info_port,mysite.station_list[l].bit_rate

);
   
 
                GtkWidget *window_p;
                GtkWidget *label;
                window_p = gtk_window_new(GTK_WINDOW_TOPLEVEL);
                gtk_window_set_position(GTK_WINDOW(window_p), GTK_WIN_POS_CENTER);
                gtk_window_set_title(GTK_WINDOW(window_p), "Station Information");
                gtk_container_set_border_width(GTK_CONTAINER(window_p), 15);

                label = gtk_label_new(stationInfoDD);

                gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
                gtk_container_add(GTK_CONTAINER(window_p), label);

                //g_signal_connect(window_p, "destroy",
                //  G_CALLBACK(gtk_main_quit), NULL);

                gtk_widget_show_all(window_p);


                              
                break;

        }

        close(s);

}


void *myThreadFun(void *vargp){
        //  system(ffmpeg -i input -f mpegts udp://0.0.0.0:port?pkt_size=188&buffer_size=65535)
        char command[] = "";

        // system("cat recv.mp3 | ffplay pipe:");
       // system("cat recv.mp3 | vlc-wrapper -");
      //   system("ffplay -nodisp -i recv.mp3");
        system("pidof ffplay | xargs -I{} kill {}");
       // system("ffplay -nodisp -i recv.mp3 -hide_banner -loglevel panic");
        system("ffplay -i recv.mp3 -hide_banner -loglevel panic");
        return NULL;
}

int main(int argc, char * argv[]){
        
 //      system("sudo kill -9 $(sudo lsof -t -i:5432)");
       system("sudo kill -9 $(sudo lsof -t -i:1723)");
       system("sudo kill -9 $(sudo lsof -t -i:41054)");
       system("sudo kill -9 $(sudo lsof -t -i:5499)");

        if(argc==4)
        {
                ptr.ip=argv[1];
                ptr.port=atoi(argv[2]);
                printf("%d\n",ptr.port);
                ptr.name=argv[3];

        GtkWidget *window;
        GtkWidget *vbox;

        GtkWidget *stationInfo;
        GtkWidget *selectStation;
        GtkWidget *pause;
        GtkWidget *restart;
        GtkWidget *exit00;
        GtkWidget *label;

        int temp = 1;
        char **temp2;
        gtk_init(&temp, &temp2);
        
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(window), 230, 250);
        gtk_window_set_title(GTK_WINDOW(window), "Menu");
        gtk_container_set_border_width(GTK_CONTAINER(window), 5);

        vbox = gtk_vbox_new(TRUE, 1);
        gtk_container_add(GTK_CONTAINER(window), vbox);

        stationInfo = gtk_button_new_with_label("Station Info");
        selectStation = gtk_button_new_with_label("Select Station");
        pause = gtk_button_new_with_label("Pause");
        restart = gtk_button_new_with_label("Restart");
        exit00 = gtk_button_new_with_label("Exit");
        
        gtk_box_pack_start(GTK_BOX(vbox), stationInfo, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), selectStation, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), pause, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), restart, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), exit00, TRUE, TRUE, 0);

        g_signal_connect (stationInfo, "clicked", G_CALLBACK (stationInfoListener), NULL);
        g_signal_connect (selectStation, "clicked", G_CALLBACK (selectStationListener), NULL);
        g_signal_connect (pause, "clicked", G_CALLBACK (pauseListener), NULL);
        g_signal_connect (restart, "clicked", G_CALLBACK (restartListener), NULL);
        g_signal_connect (exit00, "clicked", G_CALLBACK (exit00Listener), NULL);

        gtk_widget_show_all(window);

        gtk_main();

                while(1); 
        }
        else
                printf("command line argument is missing\n"); 

        return 0;
}
