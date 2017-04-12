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


int main(int argc , char *argv[]){

   //Check initial argument passing
  if( argc == 3 ) {
      printf("Potato Ringmaster\nPlayers = %s\nHops = %s\n", argv[1], argv[2]);
   }
   else if( argc > 3 ) {
      printf("Too many arguments supplied.\n");
      return 0;
   }
   else if(  !(0<= argv[2] <= 512)){
      printf("Number of hops is not within permissible range");
      return 0;
   } else {
      printf("2 arguments expected.\n");
      return 0;
   }
   //End Argument check



   //Start Program - initialize variables
   char *basepath = "/tmp/mab99/";
   char * pathname; 
   int fdr[500], fdw[500], fdarrS[500], fdarrR[500], fd, fdt;
   int i, j, k;
   int N, H;
   char Nbuf[5];
   N = atoi(argv[1]);
   H = atoi(argv[2]); 
   unsigned int strl;
   char buf[MAX_BUF];



/*Job of the Ringmaster:
1. start the game
2. report the results
3. end the game
*/


   //Make Potato
   srand( (unsigned int) time(NULL) );
   int rand_first = rand() % N;

   potato_t hot_potato;// = (potato_t *) malloc(sizeof(potato_t));
   hot_potato.total_hops = H; 
   hot_potato.hop_count = 0;


   
   //Spawn fifo network 
   char  MPbuf[25];
   char  PMbuf[25];
   char  PPbufNF[25]; //player to player buffer for next forward
   char  PPbufNR[25]; // player to player buffer for next return
   fd_set readfds;
   fd_set writefds;
   FD_ZERO(&readfds);
   FD_ZERO(&writefds);
   int fds; 
   int player[500] = {0};




   //one for loop to create each of the master_player fifos < -- done by each player
  for(i = 0; i < N; i++){
      // wait for player to establish connection
   
      strl = sprintf(PMbuf, "p%d_master", i);
      pathname = strcatMP(PMbuf, basepath, strl);
      fd = open(pathname, O_RDONLY);
      while(fd == -1){
         fd = open(pathname, O_RDONLY);
      }
      fdr[i] = fd;

      //printf("ringmaster Oh dear, something went wrong with read()! %s\n", strerror(errno));
      read(fd, buf, MAX_BUF);
      //printf("id: %s fdr: %d\n", buf , fdr[i]);
      fdarrR[atoi(buf)] = fd;
      //printf("Player %s is ready to play\n", buf);
      FD_SET(fd, &readfds);


      //send ack message backt to player
      player[atoi(buf)] = 1; // mark their location in an arrray
      strl = sprintf(MPbuf, "master_p%d", i);
      pathname = strcatMP(MPbuf, basepath, strl);
      fdt = open(pathname, O_WRONLY);
      while(fdt == -1){
         fdt = open(pathname, O_WRONLY);
      }
      fdw[i] = fdt;
      //printf("id: %s fdw: %d\n", buf , fdw[i]);
      //printf("ringmaster Oh dear, something went wrong with write()! %s\n", strerror(errno));
      sprintf(Nbuf, "%d", N);
      write(fdt, Nbuf, strlen(Nbuf)+1);
      fdarrS[atoi(buf)] = fdt;
     // printf(" FD: %d ID: %s\n" , fdt, buf);
      FD_SET(fdt, &writefds);

   }  



   //Announce that all players are ready

   printf("All players present, sending potato to player %d\n" , rand_first);
   if(H == 0){

      for(i = 0; i < N; i++){
         close(fdr[i]);
         close(fdw[i]);
      }
      return 0; 
   }

   //Begin game play
   char first_player[30];
 
   sprintf(first_player, "%smaster_p%d", basepath, rand_first);
   sendPotato(hot_potato, fdarrR[rand_first], fdarrS[rand_first], rand_first);

   //write(fdarrS[rand_first], "potato", strlen("potato")+1);    


   //printf("should've sent the message to %s\n", fdarrS[rand_first]);


    // remove the FIFO 
      //unlink(pathname);

   //blocking select for waiting to get end message from last player 
   select(FD_SETSIZE, &readfds, NULL, NULL, NULL);

   for(i = 0; i < N; i++){
      close(fdr[i]);
      close(fdw[i]);
   }

   //Report the Results
  /*int trace[500];
   // next, print out game trace
   printf("Trace of potato:\n");
   for(i = 0; i < N; i++){
      printf("%d, " , trace[i]);
   }*/
   //iterate through each fifo and unlink it

   //End the game


   //free the return value from strModMast
   //free(pathname);

   return 0;
}


