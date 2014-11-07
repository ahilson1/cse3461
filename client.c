#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define address "127.0.0.1"
#define BUFLEN 512  //Max length of buffer



/* Global */
char message[1000][BUFLEN];

void init_message()
{
  int i;    
  for(i=0;i<1000;i++)
  {
    strcpy(message[i],"empty");
  } 
} 

int check_message(int total_num)
{
  int flag = 1;
  int i;
  for(i=0;i<total_num;i++)
  {
    if(strcmp(message[i],"empty"))
    {
      flag = 0; //  False our message is not done sending
    }
  }
  return flag;
}

void die(char *s)
{
    perror(s);
    exit(1);
}

int main(int argc, char *argv[])
{
  //checks that enough arguments were given
  if(argc < 3)
  {
    fprintf(stderr, "Not enough arguments given \n");
    return -1;
  }
  
  init_message();

  //declare and grab all command line arguments
  //  char address[BUFLEN];
  int port;
  char file_name[BUFLEN];
  
//  strcpy(address,argv[1]);
  port = atoi(argv[2]);
  strcpy(file_name, argv[3]);

  struct sockaddr_in si_other;
  int s, i, slen=sizeof(si_other);
  char buf[BUFLEN];
  int total_number_packs, seq_num;  
  char snum[10];  

  if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
      die("socket");
  }
 
  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(port);
     
  if (inet_aton(address , &si_other.sin_addr) == 0)
  {
      fprintf(stderr, "inet_aton() failed\n");
      exit(1);
  }

  memset(buf,0,sizeof(buf));      
  //send the file name to the server
  if (sendto(s, file_name, strlen(file_name) , 0 , (struct sockaddr *) &si_other, slen)==-1)
  {
    die("sendto()");
  }
         
  // Get the total number of packets from the server
  if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
  {
    die("recvfrom()");
  }

  total_number_packs = atoi(buf);
   
  for(i=0;i<total_number_packs;i++)
  {
    // Get the sequence number of the packet from the server
    if (recvfrom(s, snum, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1) 
    {
      printf("Packet Lost\n");
      die("recvfrom()");
    }
    else
    {
      seq_num = atoi(snum);
      printf("Receiving Packet: %d\n",seq_num);
    }
   
    // Get the data from the server
    if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1) 
    {
      printf("Packet Lost\n");
      die("recvfrom()");
    }
    else
    {
      printf("Receiving Data for Packet: %d\n",seq_num);
      strcpy(message[seq_num],buf);
    }
    //send the ack to the server
    if (sendto(s, "ack", strlen("ack") , 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
      die("sendto()");
    }
  
    memset(buf,0,sizeof(buf));  //  Clear buf for the next data
    memset(snum,0,sizeof(snum));  //  Clear snum for the next seq number
  }  
    
  for(i=0;i<total_number_packs;i++)
  {
   // printf("this is the seq num: %d\nthis is the message: %s\n ",i,message[i]);
  }
  printf("\nFile Transfer is complete!\n");
  printf("Received %d out of %d packets. Success!\n",total_number_packs,total_number_packs);
  close(s);
  return 0;
}


