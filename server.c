#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>

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
  char data[2000];
}pack_t;

/* Global  */
pack_t *array_of_packs[1000];
char array_of_ack[1000][5];

/* Function Prototypes */
void make_packs(FILE *,pack_t**); 


void make_packs(FILE * input,pack_t *array_of_packs[1000])
{
  int seq_number = 0;
  int total_number = 0;
  char the_data[2000];
  int i;
  while(fgets(the_data,2000,input))
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
  float loss;
  int RDT = 0;
  int end;
  if(argc >= 3)
  {
    sscanf(argv[2], "%f",&loss);
  }
  if(argc == 4) 
  {
    RDT = 1;
  }    
  struct sockaddr_in si_me, si_other;
     
  int s, temp, i, a, slen = sizeof(si_other) , recv_len, number_of_packets;
  int the_seq_num, the_total_num_pack;
  char buf[BUFLEN];
  char snum[10];
  char tnum[10];
  char the_ack[5];

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
    printf("Waiting for a file name...\n");
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
          
      if (sendto(s, tnum, strlen(tnum), 0, (struct sockaddr*) &si_other, slen) == -1)
      { 
        die("sendto()");
      } 
 
    }
    else
    {
      strcpy(buf,"404 That file could not be found.");
    }
    if(argc >=3)
    {
      end =(int)(number_of_packets*loss);
      end = number_of_packets-end; 
    }
    else
    {
      end = number_of_packets;
    }
    for(a=0;a<end;a++)
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

      //  Wait for an ack from receiver
      if ((recv_len = recvfrom(s, the_ack, strlen(the_ack), 0, (struct sockaddr *) &si_other, &slen)) == -1)
      {
        die("recvfrom()");
      }
      else
      {
        temp = atoi(snum);
        strcpy(array_of_ack[temp],"ack");
      }        
     
      memset(buf,0,sizeof(buf));  //  clear buf for more data  
      memset(snum,0,sizeof(snum));  // clear snum for the next seq number
    }
    
    /* CHECK THE ARRAY OF ACKS TO SEE IF WE NEED TO RESEND A PACKET */
    if(RDT == 1)
    {
      for(i=0;i<the_total_num_pack;i++)
      {
        if(strcmp(array_of_ack[i],"ack"))
        {
          strcpy(buf,array_of_packs[i] -> data);
          snprintf(snum,sizeof(snum),"%d",i);

          printf("\nPacket: %d was lost...\n",i);
          //  Resend the sequence number to the receiver
          if (sendto(s, snum, strlen(snum), 0, (struct sockaddr*) &si_other, slen) == -1) 
          {   
            die("sendto()");
          }   
          else
          {   
            printf("Resending Packet: %d\n",i);
          }   
    
          //  Resend the data to the receiver      
          if (sendto(s, buf, strlen(buf), 0, (struct sockaddr*) &si_other, slen) == -1) 
          {   
            die("sendto()");
          }   
          else
          {   
            printf("Sending Data for Packet: %d\n",i); 
          }   

          //  Wait for an ack from receiver
          if ((recv_len = recvfrom(s, the_ack, strlen(the_ack), 0, (struct sockaddr *) &si_other, &slen)) == -1)
          {
            die("recvfrom()");
          }   
          else
          {   
            temp = atoi(snum);
            strcpy(array_of_ack[temp],"ack");
          }
          memset(buf,0,sizeof(buf));  //  clear buf for more data  
          memset(snum,0,sizeof(snum));  // clear snum for the next seq number    
        }
      }
    }
    printf("\nDone Transmiting Data!\n");
  }
 
  close(s);
  return 0;
}
