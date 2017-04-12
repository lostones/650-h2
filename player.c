#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#include "potato.h"


#define MAX_BUF 512


void main(int argc , char *argv[]){

  if( argc == 2 ) {
      //printf("\nPlayer %s is ready to play\n", argv[1]);
   }
   else if( argc > 2 ) {
      printf("Too many arguments supplied.\n");
   }
   else {
      printf("1 arguments expected.\n");
   }

   int ID = atoi(argv[1]);
   //printf("my id is: %d\n", ID);


/*Job of the player process
1 receive potato
2 decrement number of hops
3 append ID to the hops array
4 either return it to ringmaster
or pass it to a random player*/

   char *basepath = "/tmp/mab99/";
   char buf[MAX_BUF];
   int numbuf[1], i;
   char * pathname; 
   unsigned int strl;
   int fd, fd2, p2pfd;
   int  fdw[6] = {0};
   int  fdr[6] = {0};
   srand( (unsigned int) time(NULL) +ID);
    // needs to happen after the numebr of players in total has been communicated
//Create player to master fifos
   char MPbuf[15];
   char PMbuf[15];
   fd_set readfds;
   fd_set writefds;
   FD_ZERO(&readfds);
   FD_ZERO(&writefds);

   //create master to player // player to master fifos 
   strl = sprintf(MPbuf, "master_p%d", ID);
   pathname = strcatMP(MPbuf, basepath, strl);
   //printf("path name is: %s\n", pathname);
   mkfifo(pathname, 0666);

   strl = sprintf(PMbuf, "p%d_master", ID);
   pathname = strcatMP(PMbuf, basepath, strl);
  // printf("%s\n", pathname);
   mkfifo(pathname, 0666);

   //open a fifo to speak to master, tell it you are ready
   //printf("here\n");
   char  msg[5]; 
   sprintf(msg, "%d", ID);
   fd = open(pathname, O_WRONLY);
   //FD_SET(fd, &writefds);
   fdw[0] = fd; //<---------------------p_master is in fdw0
   write(fd, msg, strlen(msg) +1);
  
   //printf("I sent a message: %s \n", msg);
   pathname = strcatMP(MPbuf, basepath, strl);
   //printf("%s\n", pathname);
   fd2 = open(pathname, O_RDONLY);
   fdr[0] = fd2; //<---------------------master_p is in fdr0
   read(fd2, buf, MAX_BUF);
   //FD_SET(fd2, &readfds);
   
   
   char numPlayers[5];
   strcpy(numPlayers, buf);
   int numP = atoi(numPlayers);
   printf("Connected as player %d out of %d total players\n", ID, numP); 

   //Now that the player knows how many players in all there are, they can spawn their fifos for the neighbors
   // Going to use a multi-pass algorithim needing 4 passes. 
   // first pass, even pplayer numbers establish pipe to next odd, and odd looks for that
   // second pass, even look for odd to establish, and odd establish
   // passese 3 and 4, same as 1 and 2, but with previous node instead of next

   char cur_player[5];
   char next_player[5];
   char prev_player[5];
   char c2n[30]; // current to next fifo name
   char c2p[30]; //
   char n2c[30]; //
   char p2c[30]; // 
   char cfn[30]; // current to next fifo name
   char cfp[30]; //
   char nfc[30]; 
   char pfc[30];


   sprintf(cur_player, "p%d", ID);

      if(ID == numP-1){
         sprintf(next_player, "p%d", 0);
      } else {
         sprintf(next_player, "p%d", ID+1);
      }
      if(ID == 0){
         sprintf(prev_player, "p%d", numP-1);
      } else{
         sprintf(prev_player, "p%d", ID-1);
      }
  
   //First pass
   sprintf(c2n, "%s%s_%s", basepath, cur_player, next_player);
   sprintf(cfp, "%s%s_%s", basepath, prev_player, cur_player);

   if(ID%2 == 0){
      mkfifo(c2n, 0666);
      p2pfd = open(c2n, O_WRONLY);
      fdw[1] = p2pfd;
   } else{
      mkfifo(cfp, 0666);
      p2pfd = open(cfp, O_RDONLY); 
      fdr[1] = p2pfd;
   }

   // second pass 

   sprintf(cfn, "%s%s_%s", basepath,  next_player, cur_player);
   sprintf(c2p, "%s%s_%s", basepath,  cur_player, prev_player);

   if(ID%2 == 0){
      mkfifo(cfn, 0666);
      p2pfd = open(cfn, O_RDONLY);
      fdr[2] = p2pfd;
   } else{
      mkfifo(c2p, 0666);
      p2pfd = open(c2p, O_WRONLY);
      fdw[2] = p2pfd; 

   }

   
   //printf("ID %d made it\n" , ID );

   //third pass

   sprintf(p2c, "%s%s_%s", basepath, prev_player, cur_player);
   sprintf(n2c, "%s%s_%s", basepath, cur_player, next_player);

   if(ID%2 == 0){
      mkfifo(p2c, 0666);
      p2pfd = open(p2c, O_RDONLY);
      fdr[3] = p2pfd;

   } else{
      mkfifo(n2c, 0666);

      p2pfd = open(n2c, O_WRONLY); 
      fdw[3] = p2pfd;
      //FD_SET(p2pfd, &writefds);
   }

   // fourth pass 

   sprintf(pfc, "%s%s_%s", basepath,  cur_player, prev_player);
   sprintf(nfc, "%s%s_%s", basepath,  next_player, cur_player);

   if(ID%2 == 0){
      mkfifo(pfc, 0666);
      p2pfd = open(pfc, O_WRONLY);
      fdw[4] = p2pfd;
   } else{
      mkfifo(nfc, 0666);
      p2pfd = open(nfc, O_RDONLY); 
      fdr[4] = p2pfd;

   }

   /*for(i = 0; i < 5; i++){
      printf("PLAYER %d fdr: %d fdw: %d\n", ID, fdr[i], fdw[i]);
   }*/

   //Add fdr to the fdset for reads
for(i = 0; i < 5; i++){
   if(fdr[i] != 0){
      FD_SET(fdr[i], &readfds);
     ///printf("%dfdr[%d]: %d", ID, i, fdr[i]); 
   }
   //printf("\n");
 }


//receive potato process
char potatobuf[50];
char messagebuf[2048];
int selRes;



   FD_ZERO(&readfds);
   for(i = 0; i < 5; i++){
      if(fdr[i] != 0){
         FD_SET(fdr[i], &readfds);
     // printf("%dfdr[%d]: %d", ID, i, fdr[i]); 
      }
   }

potato_t hot_potato;

 
while(1){
   selRes = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
   for(i = 0; i < 5; i++){
   
   if(FD_ISSET(fdr[i], &readfds)){
      read(fdr[i], potatobuf, 50);
      if(strcmp(potatobuf, "potato") == 0){
         write(fdw[0], "ack", strlen("ack")+1);
         strcpy(potatobuf, "");
         read(fdr[i], messagebuf, 100);
         if((strcmp(messagebuf, "") > 0)){
            break;

               hot_potato = unpackPotato(messagebuf);
              // int random = rand() % 2;

               if(random == 1){
                  //send potato in one direction
                  //sendPotato(hot_potato, p#_p#+1, p#-1_p#, rand_first);
               }else{
                  //send potato in the other direction 
                  //sendPotato(hot_potato, p#-1_p#, p#_p#+1, rand_first);
               }
         }     
    
      }
   }
}
  

 }

//printf("got it ID: %d\n", ID);

write(fdw[0], "done", strlen("done"));

// decrement hops
//append ID

//send potato process
int random = rand() % 2; // gives a 1 or a 0, 0 goes to left, 1 goes to right

// pass to ring master if hops left = 0
// pass to next random potato if hops > 0 
/*int num_hops;
if(num_hops == 0){
   printf("I'm it!");
}
*/

//TODO: need to add sprintf to alter the name of the fifo pipe that I will be using for each pipe



   for(i = 0; i < 5; i++){
      if(fdr[i] != 0 || fdw[i] != 0){
      close(fdr[i]);
      close(fdw[i]);
      }
   }
   

}