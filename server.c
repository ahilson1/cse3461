#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
 
#define BUFLEN 512  //Max length of buffer


 
void die(char *s)
{
    perror(s);
    exit(1);
}

// This struct will represent 1 packet
typedef struct
{
  int seq_num;
  int total_num_pack;
  char data[997];
}pack_t;

/* Global  */
pack_t *array_of_packs[1000];

/* Function Prototypes */
void make_packs(FILE *,pack_t**); 


void make_packs(FILE * input,pack_t *array_of_packs[1000])
{
  int seq_number = 0;
  int total_number = 0;
  char the_data[997];
  int i;
  while(fgets(the_data,997,input))
  {  
    pack_t* the_pack = (pack_t*)malloc(sizeof(pack_t));
    strcpy(the_pack -> data,the_data);
    the_pack -> seq_num = seq_number;
    
    array_of_packs[seq_number] = the_pack;
    seq_number++;
    total_number++;
  }
  
  for(i=0;i<seq_number;i++)
  {
    array_of_packs[i] -> total_num_pack = total_number;
  }
}
 
int main(int argc, char *argv[])
{
  int port = atoi(argv[1]);
    
  struct sockaddr_in si_me, si_other;
     
  int s, i,a,slen = sizeof(si_other) , recv_len, number_of_packets;
  int the_seq_num, the_total_num_pack;
  char buf[BUFLEN];
  char snum[10];
  char tnum[10];
  
  //create a UDP socket
  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
      die("socket");
  }
     
  // zero out the structure
  memset((char *) &si_me, 0, sizeof(si_me));
     
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(port);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
  //bind socket to port
  if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
  {
      die("bind");
  }
  // This will point to the file to be sent to the client
  FILE *input; 

  //keep listening for data
  while(1)
  {
    memset(buf,0,sizeof(buf));  
    printf("Waiting for data...\n");
    fflush(stdout);
         
    //try to receive some data, this is a blocking call
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)       
    {
      die("recvfrom()");
    }
    input = fopen(buf,"r");
    if(input)
    {
      make_packs(input,array_of_packs);
      fclose(input);
      memset(buf,0,sizeof(buf));
      number_of_packets = array_of_packs[0] -> total_num_pack;
      snprintf(tnum,sizeof(tnum),"%d",number_of_packets);
      printf("hereart %d\n",number_of_packets);      
      if (sendto(s, tnum, strlen(tnum), 0, (struct sockaddr*) &si_other, slen) == -1)
      { 
        die("sendto()");
      } 
 
    }
    else
    {
      strcpy(buf,"404 That file could not be found.");
    }
             
    for(a=0;a<number_of_packets;a++)
    {
      the_total_num_pack = array_of_packs[a] -> total_num_pack;
      the_seq_num = array_of_packs[a] -> seq_num;

      strcpy(buf,array_of_packs[a] -> data);
      snprintf(snum,sizeof(snum),"%d",the_seq_num);
      
      //  Send the sequence number to the receiver
      if (sendto(s, snum, strlen(snum), 0, (struct sockaddr*) &si_other, slen) == -1)
      {   
        die("sendto()");
      }
      else
      {
        printf("Sending Packet: %d\n",the_seq_num);
      }
      
      //  Send the data to the receiver      
      if (sendto(s, buf, strlen(buf), 0, (struct sockaddr*) &si_other, slen) == -1)
      {   
        die("sendto()");
      }
      else
      {
         printf("Sending Data for Packet: %d\n",the_seq_num); 
      }
     
      memset(buf,0,sizeof(buf));  //  clear buf for more data  
      memset(snum,0,sizeof(snum));  // clear snum for the next seq number
    }

  }
 
  close(s);
  return 0;
}
