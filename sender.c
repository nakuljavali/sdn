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
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "myheader.h"
#include <pthread.h>
#include <pcap.h>
#include "array.h"

#define PACK_SIZE 5
#define FILE_SIZE 10485760
#define PAYLOAD_SIZE 1490
#define NO_OF_PACKETS 7038


short int dest1 = 0;
short int dest2 = 0;
short int source = 0;

char* buffer = NULL;
int no_of_packets = 0;
int last_packet_size = 0;
char interface[5];

struct timeval tv;
struct sockaddr_ll saddrll; // store link layer structure globally 
socklen_t sll_len;
int s1,s2;

int pktinterval = 0 ;
int doneinterval = 0;
//decalare file headers here 
const char* myfile; 

int count1=0,count2 =0;
void check_arr() 
{
  int i;
  count1 = 0;
  count2 = 0;
  for (i = 0; i< NO_OF_PACKETS; i++) 
  {
   if(send_array1[i] == 1)
     count1++;
   if(send_array2[i] == 1)
     count2++;
  }
  printf("Check array count1=%d count2=%d \n",count1,count2);
  if((count1==0) && (count2==0))
	{
		printf(" Sending Complete........ Exiting");
		exit(0);
	}
}



void cal_seq_no()
{
	no_of_packets = FILE_SIZE / 1490;
	printf("no of packets = %d\n",no_of_packets);
	last_packet_size = FILE_SIZE - no_of_packets*1490;
	printf("last packet siZe =%d\n",last_packet_size);
}


const char *read_file_to_heap(char *file_name)
{

    const char *memblock;
    int fd;
    struct stat sb;

    fd = open(file_name, O_RDONLY);
    if (fd == -1) 
    {
        printf("can't open file name");
        exit(1);
    }
   
    fstat(fd, &sb);
    memblock = mmap(NULL, sb.st_size, PROT_WRITE, MAP_PRIVATE, fd, 0);

    if (memblock == MAP_FAILED) 
    {
	printf("\n mmap failed\n");
    }
    close(fd);
    return memblock;
}

void initialize_sender1() {

  s1 = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (s1 < 0)
    printf("Socket not created\n");

  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strncpy (ifr.ifr_name, interface, sizeof(ifr.ifr_name) - 1);
  ifr.ifr_name[sizeof(ifr.ifr_name)-1] = '\0';

  if (ioctl(s1, SIOCGIFINDEX, &ifr) == -1) {
      close(s1);
      exit(EXIT_FAILURE);
    }

  // is the interface up?
  ioctl(s1, SIOCGIFFLAGS, &ifr);
  if ( (ifr.ifr_flags & IFF_UP) == 0) {
    printf("Interface is down\n");
    exit(EXIT_FAILURE);
  }

  // just write in the structure again
  ioctl(s1, SIOCGIFINDEX, &ifr);

  memset((void*)&saddrll, 0, sizeof(saddrll));
  sll_len = (socklen_t)sizeof(saddrll);
  saddrll.sll_family    = AF_PACKET;
  saddrll.sll_ifindex   = ifr.ifr_ifindex;
  saddrll.sll_protocol  = htons(ETH_P_ALL);
  printf("Index : %d\n", ifr.ifr_ifindex);
   //Get the current flags that the device might have
  if (ioctl (s1, SIOCGIFFLAGS, &ifr) == -1)
    {
      perror ("Error: Could not retrive the flags from the device.\n");
      exit (1);
    }
  //Set the old flags plus the IFF_PROMISC flag
  ifr.ifr_flags |= IFF_PROMISC;
  if (ioctl (s1, SIOCSIFFLAGS, &ifr) == -1)
    {
 perror ("Error: Could not set flag IFF_PROMISC");
      exit (1);
    }
  // Configure the device
  if (ioctl (s1, SIOCGIFINDEX, &ifr) < 0)
    {
      perror ("Error: Error getting the device index.\n");
      exit (1);
    }

  //int optval = 1;
  tv.tv_sec = 3;
  tv.tv_usec = 0;
  //setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
  setsockopt(s1, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
  //setsockopt(s1, SOL_SOCKET,  SO_REUSEADDR, &optval, sizeof optval);

  /*  if (bind(s, (struct sockaddr *) &saddrll,
           sizeof(struct sockaddr_ll)) == -1)
    printf("bind error\n");
  */
}
void initialize_sender2() {

  s2 = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (s2 < 0)
    printf("Socket not created\n");

  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strncpy (ifr.ifr_name, interface, sizeof(ifr.ifr_name) - 1);
  ifr.ifr_name[sizeof(ifr.ifr_name)-1] = '\0';

  if (ioctl(s2, SIOCGIFINDEX, &ifr) == -1) {
      close(s2);
      exit(EXIT_FAILURE);
    }

  // is the interface up?
  ioctl(s2, SIOCGIFFLAGS, &ifr);
  if ( (ifr.ifr_flags & IFF_UP) == 0) {
    printf("Interface is down\n");
    exit(EXIT_FAILURE);
  }

  // just write in the structure again
  ioctl(s2, SIOCGIFINDEX, &ifr);

  memset((void*)&saddrll, 0, sizeof(saddrll));
  sll_len = (socklen_t)sizeof(saddrll);
  saddrll.sll_family    = AF_PACKET;
  saddrll.sll_ifindex   = ifr.ifr_ifindex;
  saddrll.sll_protocol  = htons(ETH_P_ALL);
  printf("Index : %d\n", ifr.ifr_ifindex);
   //Get the current flags that the device might have
  if (ioctl (s2, SIOCGIFFLAGS, &ifr) == -1)
    {
      perror ("Error: Could not retrive the flags from the device.\n");
      exit (1);
    }
  //Set the old flags plus the IFF_PROMISC flag
  ifr.ifr_flags |= IFF_PROMISC;
  if (ioctl (s2, SIOCSIFFLAGS, &ifr) == -1)
    {
 perror ("Error: Could not set flag IFF_PROMISC");
      exit (1);
    }
  // Configure the device
  if (ioctl (s2, SIOCGIFINDEX, &ifr) < 0)
    {
      perror ("Error: Error getting the device index.\n");
      exit (1);
    }

  int optval = 1;
  tv.tv_sec = 3;
  tv.tv_usec = 0;
  //setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
  fcntl(s2, F_SETFL, O_NONBLOCK);
  //  setsockopt(s2, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
  setsockopt(s2, SOL_SOCKET,  SO_REUSEADDR, &optval, sizeof optval);

  /*  if (bind(s, (struct sockaddr *) &saddrll,
           sizeof(struct sockaddr_ll)) == -1)
    printf("bind error\n");
  */
}


/*
void initialize_sender()
{
 // int optval = 1;	
  sock = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
 // setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  //strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name)-1);
  strncpy (ifr.ifr_name, interface, sizeof(ifr.ifr_name) - 1);
  ifr.ifr_name[sizeof(ifr.ifr_name)-1] = '\0';
  if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) 
  {
    printf("Error: could not get interface index\n");
    close(sock);
    exit(0);
  }
  
 // just write in the structure again
    ioctl(sock, SIOCGIFFLAGS, &ifr);
  if ( (ifr.ifr_flags & IFF_UP) == 0) {
    printf("Interface is down\n");
    exit(EXIT_FAILURE);
  }

  ioctl(sock, SIOCGIFINDEX, &ifr);

  
  memset((void*)&saddrll, 0, sizeof(saddrll));
  sll_len = (socklen_t)sizeof(saddrll);
  saddrll.sll_family = AF_PACKET;   
  saddrll.sll_ifindex = ifr.ifr_ifindex;
  //ddrll.sll_halen = ETH_ALEN;
  saddrll.sll_protocol  = htons(ETH_P_ALL);
  printf("Index : %d\n", ifr.ifr_ifindex);

 //Get the current flags that the device might have
  if (ioctl (sock, SIOCGIFFLAGS, &ifr) == -1)
    {
      perror ("Error: Could not retrive the flags from the device.\n");
      exit (1);
    }
  //Set the old flags plus the IFF_PROMISC flag
  ifr.ifr_flags |= IFF_PROMISC;
  if (ioctl (sock, SIOCSIFFLAGS, &ifr) == -1)
    {
      perror ("Error: Could not set flag IFF_PROMISC");
      exit (1);
    }
  // Configure the device
  if (ioctl (sock, SIOCGIFINDEX, &ifr) < 0)
    {
      perror ("Error: Error getting the device index.\n");
      exit (1);
    }
 struct timeval tv;
  tv.tv_sec = 3;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval));


}
*/
void set_to_one()
{
	int i=0;
	for(i=0;i<=NO_OF_PACKETS;i++)
		{
			send_array1[i]=1;
			send_array2[i]=1;
		}
}


void send_whole_array()
{
  char* data= (char *)malloc(sizeof(struct myudp)+PAYLOAD_SIZE);


           ((struct myudp*)(data))->src = htons(source);
           ((struct myudp*)(data))->dest1 = htons(0);
           ((struct myudp*)(data))->dest2 = htons(0);

  int i=0;
  for(i=0;i<=no_of_packets;i++)
  //for(i=0;i<=10;i++)
  {
        if(send_array1[i] == 1)
        {

           ((struct myudp*)(data))->dest = htons(dest1);
           ((struct myudp*)(data))->seq = htons(i);
           memcpy(data+sizeof(struct myudp),myfile+i*PAYLOAD_SIZE,PAYLOAD_SIZE);
           if (sendto(s1, data,sizeof(struct myudp)+PAYLOAD_SIZE, 0,(struct sockaddr*)&saddrll, sizeof(saddrll)) < 0)
                printf("Error, could not send\n");
	  // printf("Sent dest%d seqno=%d\n",ntohs(((struct myudp*)(data))->dest),ntohs(((struct myudp*)(data))->seq));
        }

	if(send_array2[i] == 1)
        {

           ((struct myudp*)(data))->dest = htons(dest2);
           ((struct myudp*)(data))->seq = htons(i);
           memcpy(data+sizeof(struct myudp),myfile+i*PAYLOAD_SIZE,PAYLOAD_SIZE);
           if (sendto(s1, data,sizeof(struct myudp)+PAYLOAD_SIZE, 0,(struct sockaddr*)&saddrll, sizeof(saddrll)) < 0)
                printf("Error, could not send\n");
	  // printf("Sent dest%d seqno=%d\n",ntohs(((struct myudp*)(data))->dest),ntohs(((struct myudp*)(data))->seq));
        }
     
          usleep(pktinterval);
  }
  printf("Sent Packets ......\n ");
  free(data);
}

void done_sending1()
{
  struct myudp* udp1 = (struct myudp*)malloc(sizeof(struct myudp));
  udp1->src = htons(source);
  udp1->dest = htons(dest1);
  udp1->dest1 = htons(0);
  udp1->dest2 = htons(0);
  udp1->seq = htons(9998);

  int i=0;

  for(i=0;i<30;i++)
  {
  if (sendto(s1,udp1,sizeof(struct myudp),0,(struct sockaddr*)&saddrll, sizeof(saddrll)) < 0)
                          printf("Error, could not send\n");
	usleep(doneinterval);
  }
  printf("Done Sending %d .......\n",dest1);
  free(udp1);
}

void done_sending2()
{
  struct myudp* udp2 = (struct myudp*)malloc(sizeof(struct myudp));
  udp2->src = htons(source);
  udp2->dest = htons(dest2);
  udp2->seq = htons(9998);
  udp2->dest1 = htons(0);//htons(sizeof(struct myudp));
  udp2->dest2 = htons(0);//htons(sizeof(struct myudp));

  int i=0;

  for(i=0;i<30;i++)
  {
  if (sendto(s1,udp2,sizeof(struct myudp),0,(struct sockaddr*)&saddrll, sizeof(saddrll)) < 0)
                          printf("Error, could not send\n");
        usleep(doneinterval);
  }
  printf("Done Sending %d .......\n",dest2);
  free(udp2);
}

void *send_all()
{
  while(1)
	{
		send_whole_array();
	}

}

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){

  u_char *buffer;
  buffer = (u_char*)packet;
   if(ntohs(((struct myudp*)(buffer))->dest) == (source) && ntohs(((struct myudp*)(buffer))->seq) == 9999 )//&&  ntohs(((struct myudp*)(buffer))->len)==1)
        {
                check_arr();
                if(ntohs(((struct myudp*)(buffer))->src) == dest1 ) {
                        bit2array1(buffer+sizeof(struct myudp));
                        send_whole_array();
                        done_sending1();
                }

                if(ntohs(((struct myudp*)(buffer))->src) == dest2 ){
           		bit2array2(buffer+sizeof(struct myudp));
                        send_whole_array();
                        done_sending2();
                }
        }

   return;
}

void *rec_all()
{
  char errbuf[PCAP_ERRBUF_SIZE];/* error buffer */
  pcap_t *handle;/* packet capture handle */

  handle = pcap_open_live(interface, 1500, 1, 1500, errbuf);
  if (handle == NULL) {
    fprintf(stderr, "Couldn't open device %s: %s\n", interface, errbuf);
    exit(EXIT_FAILURE);
  }

  pcap_loop(handle, -1, got_packet, NULL);

  return;
}
int main(int argc, char* argv[])
{

  pthread_t send_thread;
  pthread_t rec_thread;

  dest1 = atoi(argv[1]);
  dest2 = atoi(argv[2]);
  source = atoi(argv[3]);
  strcpy(interface, argv[4]);
  pktinterval = atoi(argv[5]); 
  doneinterval = atoi(argv[6]);
  
  printf("===============Parameters============================\n");
  printf("dest1 =%d\ndest2 =%d\nsource=%d\ninterface=%s\n",dest1,dest2,source,interface);
  printf("pkt_interval = %d\ndone_interval = %d\n",pktinterval,doneinterval);
  printf("=====================================================\n");
  

  if(argc != 7)
	{	printf("Error less arguments\n");exit(0);}

  cal_seq_no();
  initialize_sender1();
  initialize_sender2();
  set_to_one();
  sleep(1);
  myfile =  read_file_to_heap("data.bin");
  
  //============================================================
  //===============Sending starts from her =====================
  //============================================================
  
  pthread_create(&send_thread,NULL,send_all,NULL);
  pthread_create(&rec_thread,NULL,rec_all,NULL);

  // send_whole_array();
  //done_sending1();
 // done_sending2();
  pthread_join(send_thread, NULL);
  pthread_join(rec_thread, NULL);

return 0;

}
