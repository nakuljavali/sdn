#include <sys/socket.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <net/if.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include "myheader.h"
#include <sys/time.h>
#include <pthread.h>


#define MAX_PAY_SIZE 1490
#define TEN_MB 10485760
#define LAST_PAY_SIZE 630
#define SIZE_BIT_ARRAY 880

#include "array.h"

short int ack_arr1[MAX_PKT_NO];
short int ack_arr2[MAX_PKT_NO];

int prev1, prev2;  
int s, f; 
int flag = 0;
struct sockaddr_ll saddrll;
char* buffer;
socklen_t sll_len;
char interface[5], output1[6], output2[6]; 
short int source1, source2, destination;
struct timeval start, end;
int rem_count, gg;
int file_flag1 = 0,file_flag2 =0;

void initialize() {
 
  s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (s < 0)
    printf("Socket not created\n");

  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strncpy (ifr.ifr_name, interface, sizeof(ifr.ifr_name) - 1);
  ifr.ifr_name[sizeof(ifr.ifr_name)-1] = '\0';
 
  if (ioctl(s, SIOCGIFINDEX, &ifr) == -1) {
      close(s);
      exit(EXIT_FAILURE);
    }
 
  // is the interface up? 
  ioctl(s, SIOCGIFFLAGS, &ifr);
  if ( (ifr.ifr_flags & IFF_UP) == 0) {
    printf("Interface is down\n");
    exit(EXIT_FAILURE);
  } 
 
  // just write in the structure again 
  ioctl(s, SIOCGIFINDEX, &ifr);

  memset((void*)&saddrll, 0, sizeof(saddrll));
  sll_len = (socklen_t)sizeof(saddrll);
  saddrll.sll_family    = AF_PACKET;
  saddrll.sll_ifindex   = ifr.ifr_ifindex;
  saddrll.sll_protocol  = htons(ETH_P_ALL);
   //Get the current flags that the device might have 
  if (ioctl (s, SIOCGIFFLAGS, &ifr) == -1)
    {
      perror ("Error: Could not retrive the flags from the device.\n");
      exit (1);
    }
  //Set the old flags plus the IFF_PROMISC flag 
  ifr.ifr_flags |= IFF_PROMISC;
  if (ioctl (s, SIOCSIFFLAGS, &ifr) == -1)
    {
      perror ("Error: Could not set flag IFF_PROMISC");
      exit (1);
    }
  // Configure the device 
  if (ioctl (s, SIOCGIFINDEX, &ifr) < 0)
    {
      perror ("Error: Error getting the device index.\n");
      exit (1);
    }
  
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
}

int check_arr(short int arr[]) {
  int i, count = 0;
  for (i = 0; i< MAX_PKT_NO; i++) {
   if(arr[i] == 1) 
     count++;
  }
  return count;
}

void sender(short int source, short int arr[]) {
  struct myudp* udp = (struct myudp*)malloc(sizeof(struct myudp));
  udp->dest = htons(source);
  udp->src = htons(destination);
  udp->seq = htons(9999);
  udp->dest1 = htons(0);
  udp->dest2 = htons(0);

  // copy the udp header info 
  memset(buffer,0,ETH_FRAME_LEN);
  memcpy(buffer,udp,sizeof(struct myudp)); 
  memcpy(buffer+sizeof(struct myudp),array2bit(arr),SIZE_BIT_ARRAY);
  int i=0;
  int check = check_arr(arr);
   
  for(i =0; i<f;i++) {
     if (sendto(s, buffer,sizeof(struct myudp)+SIZE_BIT_ARRAY, 0,(struct sockaddr*)&saddrll, sizeof(saddrll)) < 0)
           printf("Error, could not send\n");
     usleep(200);
  }
  //printf("Sent ACK array\n");
}


void receiver() {
  // Initializations
  char *heap_mem1 = (char *)malloc(MAX_PAY_SIZE*MAX_PKT_NO);
  char *heap_mem2 = (char *)malloc(MAX_PAY_SIZE*MAX_PKT_NO);
  FILE *fp1 = fopen(output1,"w");
  FILE *fp2 = fopen(output2,"w");
  assert(heap_mem1 != NULL);
  assert(fp1 != NULL);
  assert(heap_mem2 != NULL);
  assert(fp2 != NULL);
  struct myudp* head;
  int length = 0;
 
  // start Listening
  printf("Listening\n");
  while(1){
    length = recvfrom(s, buffer, ETH_FRAME_LEN,0,(struct sockaddr *)&saddrll, &sll_len);
    f = 15;

    if (file_flag1 && file_flag2) {
      
       gettimeofday(&end, NULL);
       printf("END time = %f \n", (double)(end.tv_sec + end.tv_usec / 1000000));
       exit(1);
    }
    if (length <= 0){ 
      f = 5;
      sender(source1, ack_arr1);
      sender(source2, ack_arr2);
      //usleep(500);
      continue;
    }
    if(buffer == NULL) {
      continue;
    }
    head = (struct myudp *) buffer;
    if (ntohs(head->dest) == destination) {


       if(ntohs(head->seq) != 9998 && ntohs(head->seq) != 9999) {
	 u_char *payload = (u_char*)(buffer + sizeof(struct myudp));
	 if(payload == NULL) {
	   printf("Payload NULL\n");
	   exit(0);
	 }
	 if(ntohs(head->src) == source1) {
	   // printf("SEQ : %d : SCR : %d\n", ntohs(head->seq), source1);

	  memcpy(heap_mem1+(htons(head->seq))*MAX_PAY_SIZE, payload, MAX_PAY_SIZE);
	  ack_arr1[htons(head->seq)] = 0;
	  if (gg == 0) {
	    gettimeofday(&start, NULL);
	    printf("START time = %f\n", (double)(start.tv_sec + start.tv_usec / 1000000));

	    gg = 1;
	  }

  
	  rem_count = check_arr(ack_arr1);
	  if(rem_count == 0 && file_flag1 == 0) {	  
	    sender(source1, ack_arr1);
	    printf("Writing to file for %d\n", source1);
	    fwrite(heap_mem1,1,TEN_MB,fp1);
	    fclose(fp1);
	    file_flag1 = 1;
	  }
	 }

        if(ntohs(head->src) == source2) {
	  //printf("SEQ : %d : SCR : %d\n", ntohs(head->seq),source2);

	 memcpy(heap_mem2+(htons(head->seq))*MAX_PAY_SIZE, payload, MAX_PAY_SIZE);
         ack_arr2[htons(head->seq)] = 0;
        
	 rem_count = check_arr(ack_arr2);
	 if(rem_count == 0 && file_flag2 == 0) {
	   sender(source2, ack_arr2);
	   printf("Writing to file for %d\n", source2);
	   fwrite(heap_mem2,1,TEN_MB,fp2);
	   fclose(fp2);
	   file_flag2 = 1;
	   }
         }
       }
       else if(ntohs(head->seq) == 9998 && ntohs(head->seq) != 9999) {
	 if(ntohs(head->src) == source1) {
	  int cnt = check_arr(ack_arr1);
	    if(cnt  < prev1){
	      sender(source1, ack_arr1);
	      prev1 = cnt;
	    }
	 }
	 if(ntohs(head->src) == source2) {
	  int cnt = check_arr(ack_arr2);
	    if(cnt  < prev2){
	      sender(source2, ack_arr2);
	      prev2 = cnt;
	   }
	 }
       }
    }
  }
}
    

int main(int argc, char* argv[]) {
  
  prev1 = MAX_PKT_NO;
  prev2 = MAX_PKT_NO;
  destination = (short int)atoi(argv[1]);
  source1 = (short int)atoi(argv[2]);
  source2 = (short int)atoi(argv[3]);
  strcpy(interface, argv[4]);
  strcpy(output1, argv[5]);
  strcpy(output2, argv[6]);
  buffer = (void*)malloc(ETH_FRAME_LEN);
  
  gg = 0;
  int i;
  for(i = 0;i < MAX_PKT_NO; i++) {
    ack_arr1[i] = 1;
    ack_arr2[i] = 1;
  }  
  initialize();
  receiver();
  return 0;
}

     
 





     
