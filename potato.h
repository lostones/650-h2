#ifndef _POTATO_H_
#define _POTATO_H_
#define BUFF_LEN   512
#define MAX_POTATO 512
#include <string.h>


typedef struct{
  char msg_type;
  int total_hops;
  int hop_count;
  
  unsigned long hop_trace[MAX_POTATO];
} potato_t;

 char * strcatMP(char * m2p, char * base, unsigned int strleng){
   //printf("%s\n", m2p);
   char * temp = (char *) malloc(strlen(m2p)-1);
   strncpy(temp, m2p, strleng);
   char * result = (char *) malloc(strlen(temp)+strlen(base));
   
   strcpy(result, base);
   strcat(result, temp); 
   return result;
}

 char * strcatPP(char * p2p, char * base){
   //printf("%s\n", p2p);
   char * result = (char *) malloc(strlen(p2p)+strlen(base)+1);
   
   strcpy(result, base);
   strcat(result, p2p); 
   return result;
}

void sendPotato(potato_t potato, int fdr, int fds, int playedID){
  char ackbuf[5];
  write(fds, "potato", strlen("potato")+1);
   
  read(fdr, ackbuf, 5);

  if(strcmp(ackbuf, "ack") == 0){
    char temp[100];
    sprintf(temp, "%d#%d" , potato.total_hops, potato.hop_count);
    write(fds, temp, strlen(temp)+1);
   }

}

potato_t unpackPotato(char message[]){
    potato_t hot_potato;
    char h1[5]; 
    char h2[5];
    char * ret;
    int hopsLeft, hopsTotal, i, j, k;
    ret = strstr(message, "#");
    j = strlen(message);
    k = strlen(ret);
    strncpy(h1, message, j-k);
    for(i = 1; i < k; i++){
        h2[i] = ret[i];
    }
    //recast the strings into the potato strcut
    /*printf("h1%s\n" , h1);
    printf("h2%s\n", h2);*/
    return hot_potato;
}


#endif