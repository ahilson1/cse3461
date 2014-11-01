#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define BUFLEN 512  //Max length of buffer
#define PORT 5000   //The port on which to listen for incoming data

 
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
pack_t* make_packs(FILE *,pack_t**); 
void dump_array(pack_t**);

void dump_array(pack_t *array_of_packs[1000])
{
  int i;
  for(i=0;i<10;i++)
  {
    printf("seq_num = %d",array_of_packs[i] -> seq_num);
    printf("total_num_pack = %d",array_of_packs[i] -> total_num_pack);
    printf("data = %s",array_of_packs[i] -> data);
  }
}
pack_t* make_packs(FILE * input,pack_t *array_of_packs[1000])
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
    printf("seq_num = %d  ",array_of_packs[i] -> seq_num);
    printf("total_num_pack = %d  ",array_of_packs[i] -> total_num_pack);
    printf("data = %s  \n\n",array_of_packs[i] -> data);

  }
}

int main(void)
{
  FILE *input;
  input = fopen("test.txt","r");
  *array_of_packs = make_packs(input,array_of_packs);
//  dump_array(array_of_packs);
}

/* 
int main(void)
{
    struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
    char str[BUFLEN];
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
     
    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);
         
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
        else
        {
            
            
        }
         
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);
         
        //now reply the client with the same data
        if (sendto(s, buf, strlen(buf), 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            die("sendto()");
        }
    }
 
    close(s);
    return 0;
}*/
