#include <stdio.h>
#include <stdlib.h>
#include "amessage.pb-c.h"
#define MAX_MSG_SIZE 1024

static size_t
read_buffer (FILE *fileptr,unsigned max_length, uint8_t *out)
{
  size_t cur_len = 0;
  size_t nread;

  while ((nread=fread(out + cur_len, 1, max_length - cur_len, fileptr)) != 0)  {
    cur_len += nread;
    if (cur_len == max_length)
    {
      fprintf(stderr, "max message length exceeded\n");
      exit(1);
    }
  }
  return cur_len;
}


int main (int argc, const char * argv[])
{
  AMessage *msg;

  // Read packed message from standard-input.
  uint8_t buf[MAX_MSG_SIZE];

  FILE *fileptr;

  fileptr = fopen(argv[1],"rb");
  if (fileptr == NULL){
  	  printf("Error! Could not open file\n");
  	  exit(-1); // must include stdlib.h
  	}
  size_t msg_len = read_buffer (fileptr,MAX_MSG_SIZE, buf);
  fclose(fileptr);

  // Unpack the message using protobuf-c.
  msg = amessage__unpack(NULL, msg_len, buf);
  if (msg == NULL)
  {
    fprintf(stderr, "error unpacking incoming message\n");
    exit(1);
  }

  // display the message's fields.
  printf("Received: a=%d",msg->a);  // required field
  if (msg->has_b)                   // handle optional field
    printf("  b=%d",msg->b);
  printf("\n");

  // Free the unpacked message
  amessage__free_unpacked(msg, NULL);
  return 0;
}
