/*

gcc server.c -o s -pthread
./s
sudo kill -9 $(sudo lsof -t -i:5464)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>

//#define MC_PORT 5499
#define BUF_SIZE 4096
#define MAX_PENDING 5
#define MAX_LINE 256
#define SONG_COUNT 3

int MC_PORT = 41054;
int INFO_PORT = 1723;
void filelist();
char s1[]="/home/ajaybechara1/Desktop/CN_PROJECT/Final/SX";
int b=128000,t=3;
char *filesList[SONG_COUNT];
int choose=0;
void *songStreamThread(void *);

void filelist();
void doIt(int PORT,int choose_temp);

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

song_info current_song_info[5];

int getPacketRate(int bitrate){
        int byterate = (bitrate*1000)/8;
        int packetRate = byterate / BUF_SIZE;

        return packetRate+1;
}

void *tcpData(){

        struct sockaddr_in sin;
        char buf[MAX_LINE];
        int len;
        int s, new_s;
        char str[INET_ADDRSTRLEN];

        bzero((char *)&sin, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons(5432);

        if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
                perror("simplex-talk: socket");
                exit(1);
        }

        inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);

        if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0){
                perror("simplex-talk: bind");
                exit(1);
        }
        else{
                //printf("Server bind done.\n");
                }

        listen(s, MAX_PENDING);

        while(1) {

                if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
                        perror("simplex-talk: accept\n");
                        exit(1);
                }

                site_info mysite;
               
                memset(&mysite,0,sizeof(mysite));
                mysite.type=1;
                mysite.site_name_size=9;
                strcpy(mysite.site_name,"gaana.com");
                mysite.site_desc_size=4;
                strcpy(mysite.site_desc,"1234");
                
                
                int ast[10] = {0};
                
                for(int i=0 ; i<10 ; i++)
                        ast[i] = 1;
                
                int i=0;

                for(int j=1;j<=5;j++){
                        if(ast[j]){
                                mysite.station_list[i].station_number=j;
                                mysite.station_list[i].station_name_size=1;
                                char stationName[50] = "Station";
                                stationName[7] = (j-1) + 'A';
                                strcpy(mysite.station_list[i].station_name,stationName);
                                mysite.station_list[i].multicast_address=j;
                                mysite.station_list[i].data_port=MC_PORT;
                                mysite.station_list[i].info_port=INFO_PORT;
                                mysite.station_list[i].bit_rate=(j+7)*20;  
                                i++;
                                printf("i = %d\n",i);
                        }
                }
                
                mysite.station_count=i;
               
                send(new_s,&mysite,sizeof(mysite),0);

        }
        
        close(new_s);

}

void *songStreamThread(void *arg){
        
        printf("Ha ela\n");
        int s;
        struct sockaddr_in sin;
        char buf[BUF_SIZE];
        int len;
        int choose_temp=choose;
 
        char *mcast_addr;
 
        if (1) {
                char multicast_address[] = "239.192.xx.11";
                multicast_address[9] = ((choose_temp%10) + '0');
                multicast_address[8] = ((int)(choose_temp/10)) + '0';                
                mcast_addr = (char *)&multicast_address;
        }
        else{
                fprintf(stderr, "usage: sender multicast_address\n");
                exit(1);
        }
   
        if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("server: socket");
                exit(1);
        }
 
        memset((char *)&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = inet_addr(mcast_addr);
        sin.sin_port = htons(MC_PORT);

        memset(buf, 0, sizeof(buf));
 
        FILE *fp;
        struct timespec tim;
        tim.tv_sec=0;
        tim.tv_nsec=1000000;
        
//        int temp = MC_PORT;
        char* filesListtemp[SONG_COUNT];
        
        for(int j=0;j<SONG_COUNT; j++){
                filesListtemp[j] = (char*) malloc (strlen(filesList[j])+1);
                strncpy(filesListtemp[j],filesList[j],strlen(filesList[j]));   
        }
        
        char command[1024];
        int packetRate;
        int bitrate;
        
                        
        char fileName[] = "songNameX.txt";
        fileName[8] = ( choose_temp-1 ) + 'A';           

        sprintf(command, "ffmpeg -i %s 2>&1 | grep bitrate | cut -d ',' -f 3 | cut -d ':' -f 2 | cut -d ' ' -f 2 |sed s/,// > %s", filesListtemp[0],fileName);
        system(command);

        FILE *fpSongName = fopen(fileName,"r");
        fpSongName = fopen(fileName,"r");
        char temp[50];
        memset(temp,0,50);
        fread(temp , sizeof( temp ) , 1 , fpSongName);
        bitrate = atoi(temp);
        fclose(fpSongName);
        
        printf("bitrate = %d\n",bitrate);
        packetRate = getPacketRate(bitrate);

        int dataRate = packetRate * BUF_SIZE;
        int packetSent = 0;
        printf("Packer %d\n",packetRate);
                                      
        while(1){
                int i=0;
                for(i=0; i<SONG_COUNT; i++){
                        printf( "Station %d - %s\n",choose_temp,filesListtemp[i]);

                        sprintf(command, "ffmpeg -i %s 2>&1 | grep service_name | cut -d ':' -f 2 | sed s/,// > %s", filesListtemp[i],fileName);
                        system(command);

                        fread(current_song_info[choose_temp].song_name , sizeof( current_song_info[choose_temp].song_name ) , 1 , fpSongName);
                        fclose(fpSongName);



                        
                        sprintf(command, "ffmpeg -i %s 2>&1 | grep service_name | cut -d ':' -f 2 | sed s/,// > %s", filesListtemp[(i+1)%5],fileName);
                        system(command);

                        fpSongName = fopen(fileName,"r");
                        fread(current_song_info[choose_temp].next_song_name , sizeof( current_song_info[choose_temp].next_song_name ) , 1 , fpSongName);
                        fclose(fpSongName);




                        
                        sprintf(command, "ffmpeg -i %s 2>&1 | grep Duration | cut -d ' ' -f 4 | cut -d ':' -f 2 | sed s/,// > %s", filesListtemp[i],fileName);
                        system(command);

                        fpSongName = fopen(fileName,"r");
                        memset(temp,0,50);
                        fread(temp , sizeof( temp ) , 1 , fpSongName);
                        int sec = atoi(temp) * 60;
                        fclose(fpSongName);



                        sprintf(command, "ffmpeg -i %s 2>&1 | grep Duration | cut -d ' ' -f 4 | cut -d ':' -f 3 | cut -d '.' -f 1 | sed s/,// > %s ", filesListtemp[i],fileName);
                        system(command);
                        

                        memset(temp,0,50);
                        fread(temp , sizeof( temp ) , 1 , fpSongName);
                        sec = sec + atoi(temp);
                        current_song_info[choose_temp].remaining_time_in_sec = sec;                       
                        
                        fclose(fpSongName);
                        


                        fp = fopen(filesListtemp[i],"r");
                        while(!feof(fp)){
                                fread(buf,sizeof(buf),1,fp);

                                sendto(s, buf, sizeof(buf), 0,(struct sockaddr *)&sin, sizeof(sin));

                                if( packetSent % packetRate == 0){
                                      // tim.tv_nsec = 1000000000 - ( packetRate *25* 1000000);
                                      // printf("aayo\n");
                                           tim.tv_sec = 1;
                                           tim.tv_nsec = 0;
                                           sleep(1);
                                      //     nanosleep(&tim,NULL);
                                        // printf("%ld\n",tim.tv_nsec);          
                                      // sleep(1);
                                }else{
                                           tim.tv_sec = 0;
                                           tim.tv_nsec = 500000;
                                           nanosleep(&tim,NULL);
                                      //  tim.tv_nsec=101000000;
                                      //  nanosleep(&tim,NULL);
                                }
                                
                                memset(buf,0,sizeof(buf));  
                                packetSent++; 
                        }
                        fclose(fp);
                        printf("Song Changed\n");
                }
        }
 
  close(s);
}

void *infoThread(void *arg){
        
        int s;
        struct sockaddr_in sin;
        char buf[BUF_SIZE];
        int len;
        int choose_temp=choose;
 
        char *mcast_addr;
 
        if (1) {
                char multicast_address[] = "239.192.xx.11";
                multicast_address[9] = ((choose_temp%10) + '0');
                multicast_address[8] = ((int)(choose_temp/10)) + '0';                
                mcast_addr = (char *)&multicast_address;
        }
        else{
                fprintf(stderr, "usage: sender multicast_address\n");
                exit(1);
        }
   
        if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("server: socket");
                exit(1);
        }
 
        memset((char *)&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = inet_addr(mcast_addr);
        sin.sin_port = htons(INFO_PORT);

        memset(buf, 0, sizeof(buf));
 
        struct timespec tim;
        tim.tv_sec=0;
        tim.tv_nsec=1000000;
                   
        while(1){
                sendto(s, &current_song_info[choose_temp], sizeof(current_song_info[choose_temp]), 0,(struct sockaddr *)&sin, sizeof(sin));
                sleep(1);
        }
 
        close(s);
}


int main(int argc, char * argv[]){

       system("sudo kill -9 $(sudo lsof -t -i:5432)");
       system("sudo kill -9 $(sudo lsof -t -i:1723)");
       system("sudo kill -9 $(sudo lsof -t -i:41054)");
       system("sudo kill -9 $(sudo lsof -t -i:5499)");

         pthread_t thread_id;
         pthread_create(&thread_id, NULL, tcpData, NULL);
         sleep(1);

        for(int i=1 ; i<=4 ; i++){
                choose = i;
                doIt(5499,i);
                sleep(2);
        }

        while(1);

        return 0;
}

void filelist(){
        DIR *d;
        struct dirent *dir;

        d = opendir(s1);

        rewinddir(d);
        int i=0;

        while((dir = readdir(d)) != NULL) {
                if ( !strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..") )  {}
                else {

                        filesList[i] = (char*) malloc (strlen(dir->d_name)+1+1+strlen(s1));

                        strcpy(filesList[i] , s1); /* copy name into the new var */
                        strcat(filesList[i] , "/");
                        strcat(filesList[i] , dir->d_name); /* add the extension */

                        i++;
                }
        }
        rewinddir(d); 
   
}

void doIt(int PORT,int choose_temp){
         s1 [ strlen(s1)-1 ] = choose_temp + '0';
                      printf("%s\n",s1) ;

         filelist();
         
         sleep(1);
         
         pthread_t thread_id;
         pthread_create(&thread_id, NULL, songStreamThread, NULL);
         sleep(1);
         pthread_t thread_id2;
         pthread_create(&thread_id2, NULL, infoThread, NULL);
         
         sleep(1);     
}
