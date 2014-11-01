#define MAX_PKT_NO 7038
#define NO_OF_PACKETS 7038
short int send_array1[NO_OF_PACKETS];
short int send_array2[NO_OF_PACKETS];

char* array2bit(short int array[])
{
  char *new = malloc(MAX_PKT_NO/8 + (MAX_PKT_NO % 8 != 0));
  int n;

  //  printf("%d\n",(MAX_PKT_NO/8 + (MAX_PKT_NO % 8 != 0)));
  for(n=0 ; n < (MAX_PKT_NO/8 + (MAX_PKT_NO % 8 != 0)) ; n++) {
    //printf("af\n");
    if(array[(n*8)]) new[n] |= 0b10000000;  
    if(array[(n*8)+1]) new[n] |= 0b01000000;  
    if(array[(n*8)+2]) new[n] |= 0b00100000;  
    if(array[(n*8)+3]) new[n] |= 0b00010000;  

    if(array[(n*8)+4]) new[n] |= 0b00001000;  
    if(array[(n*8)+5]) new[n] |= 0b00000100;  
    if(array[(n*8)+6]) new[n] |= 0b00000010;  
    if(array[(n*8)+7]) new[n] |= 0b00000001;  
  }

  //  printf("arr: %x\n", new[0] & 0xff);
  //  printf("arr: %x\n", new[1] & 0xff);
  //  printf("arr: %x\n", new[2] & 0xff);

  return new;
}

void bit2array1(char *recv_data)
{
  int n;

  //  printf("arr: %x\n", recv_data[0] & 0xff);
  //  printf("arr: %x\n", recv_data[1] & 0xff);
  //  printf("arr: %x\n", recv_data[2] & 0xff);

  //  printf("delay: %d\n",(MAX_PKT_NO/8 + (MAX_PKT_NO % 8 != 0)));
  int z = (MAX_PKT_NO/8 + (MAX_PKT_NO % 8 != 0));

  for(n=0 ; n < z ; n++) {
      send_array1[n*8] = 0; 
      send_array1[(n*8)+1] = 0; 
      send_array1[(n*8)+2] = 0; 
      send_array1[(n*8)+3] = 0; 
      send_array1[(n*8)+4] = 0; 
      send_array1[(n*8)+5] = 0; 
      send_array1[(n*8)+6] = 0; 
      send_array1[(n*8)+7] = 0; 


    if ((recv_data[n] >> 7 & 1) == 1)
      send_array1[n*8] = 1; 
    if ((recv_data[n] >> 6 & 1) == 1)
      send_array1[n*8+1] = 1; 
    if ((recv_data[n] >> 5 & 1) == 1)
      send_array1[n*8+2] = 1;
    if ((recv_data[n] >> 4 & 1) == 1)
      send_array1[n*8+3] = 1;
    if ((recv_data[n] >> 3 & 1) == 1)
      send_array1[n*8+4] = 1;
    if ((recv_data[n] >> 2 & 1) == 1)
      send_array1[n*8+5] = 1;
    if ((recv_data[n] >> 1 & 1) == 1)
      send_array1[n*8+6] = 1;
    if ((recv_data[n] >> 0 & 1) == 1)
      send_array1[n*8+7] = 1;


  }

  //  int i = 0;
  //  for (i =0; i<MAX_PKT_NO ; i++) {
    //    printf("%d.",send_array[i]);
    //    if(i==7) printf("\n");
  //  }
  //  printf("\n");
  //  printf("edn\n");

  return;
}


void bit2array2(char *recv_data)
{
  int n;

  //  printf("arr: %x\n", recv_data[0] & 0xff);
  //  printf("arr: %x\n", recv_data[1] & 0xff);
  //  printf("arr: %x\n", recv_data[2] & 0xff);

  //  printf("delay: %d\n",(MAX_PKT_NO/8 + (MAX_PKT_NO % 8 != 0)));
  int z = (MAX_PKT_NO/8 + (MAX_PKT_NO % 8 != 0));

  for(n=0 ; n < z ; n++) {
      send_array2[n*8] = 0; 
      send_array2[(n*8)+1] = 0; 
      send_array2[(n*8)+2] = 0; 
      send_array2[(n*8)+3] = 0; 
      send_array2[(n*8)+4] = 0; 
      send_array2[(n*8)+5] = 0; 
      send_array2[(n*8)+6] = 0; 
      send_array2[(n*8)+7] = 0; 


    if ((recv_data[n] >> 7 & 1) == 1)
      send_array2[n*8] = 1; 
    if ((recv_data[n] >> 6 & 1) == 1)
      send_array2[n*8+1] = 1; 
    if ((recv_data[n] >> 5 & 1) == 1)
      send_array2[n*8+2] = 1;
    if ((recv_data[n] >> 4 & 1) == 1)
      send_array2[n*8+3] = 1;
    if ((recv_data[n] >> 3 & 1) == 1)
      send_array2[n*8+4] = 1;
    if ((recv_data[n] >> 2 & 1) == 1)
      send_array2[n*8+5] = 1;
    if ((recv_data[n] >> 1 & 1) == 1)
      send_array2[n*8+6] = 1;
    if ((recv_data[n] >> 0 & 1) == 1)
      send_array2[n*8+7] = 1;


  }

  //  int i = 0;
  //  for (i =0; i<MAX_PKT_NO ; i++) {
    //    printf("%d.",send_array[i]);
    //    if(i==7) printf("\n");
  //  }
  //  printf("\n");
  //  printf("edn\n");

  return;
}

