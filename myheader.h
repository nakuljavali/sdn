
struct myudp 
{
  unsigned short dest;
  unsigned short dest1;
  unsigned short dest2;
  short int src;
  short int seq;
};

struct table
{
  short int did;
  short int iface;
};


/*
unsigned short calcsum(unsigned short *buffer, int length)
{
  unsigned long sum; 

  // initialize sum to zero and loop until length (in words) is 0 
  for (sum=0; length>1; length-=2) // sizeof() returns number of bytes, we're interested in number of words 
    sum += *buffer++;// add 1 word of buffer to sum and proceed to the next 

  // we may have an extra byte 
  if (length==1)
    sum += (char)*buffer;

  sum = (sum >> 16) + (sum & 0xFFFF);  // add high 16 to low 16 
  sum += (sum >> 16);     // add carry 
  return ~sum;
}

*/
