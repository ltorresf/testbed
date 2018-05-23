#include <stdio.h>
#include <stdlib.h>
#include "subframe2file.pb-c.h"
#include "variables.h"
#include "gps.h"
#define MAX_MSG_SIZE 2097152


void dump_gps_data(GpsData **gps_data,int n_data);
void dump_rxpwrlvl_data(RssiData *rssi_data);

static size_t read_buffer (FILE *fileptr,unsigned max_length, uint8_t *out)
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

	GpsInfo *msg;

  // Read packed message from standard-input.
  uint8_t buf[MAX_MSG_SIZE];

  FILE *fileptr;

  printf("file: %s\n",argv[1]);
  fileptr = fopen(argv[1],"rb");
  if (fileptr == NULL){
  	  printf("Error! Could not open file\n");
  	  exit(-1); // must include stdlib.h
  	}
  size_t msg_len = read_buffer(fileptr,MAX_MSG_SIZE, buf);
  printf("msg_len=%lu\n",msg_len);
  fclose(fileptr);

  // Unpack the message using protobuf-c.
  msg = gps_info__unpack(NULL, msg_len, buf);
  if (msg == NULL)
  {
    fprintf(stderr, "error unpacking incoming message\n");
    exit(1);
  }

  GpsData **gps_data = msg->gps_data;
  RssiData *rssi_container = msg->rssi_container;

  dump_gps_data(gps_data,msg->n_gps_data);
  dump_rxpwrlvl_data(rssi_container);

  // Free the unpacked message
  gps_info__free_unpacked(msg, NULL);
  return 0;
}

void dump_gps_data(GpsData **gps_data,int n_data) {
	for(int h=0;h<n_data;h++){
		printf("[GPS][%s][%s] %.3lf [+/-%.0f ms]. Coord: (%f, %f) [+/-%.3fm,+/-%.3fm], speed: %.3f, ",
					(gps_data[h]->status == 0)? "NO_FIX" : "FIX",(gps_data[h]->gps_fix->mode == MODE_2D)? "MODE_2D" : ((gps_data[h]->gps_fix->mode == MODE_3D)? "MODE_3D" :((gps_data[h]->gps_fix->mode == MODE_NO_FIX)? "MODE_NO_FIX" : "MODE_NOT_SEEN")),
					gps_data[h]->gps_fix->time,1000*gps_data[h]->gps_fix->ept,gps_data[h]->gps_fix->latitude, gps_data[h]->gps_fix->longitude,gps_data[h]->gps_fix->epy,gps_data[h]->gps_fix->epx, gps_data[h]->gps_fix->speed);
		printf("n_sat = %d of %d\n", gps_data[h]->satellites_used,gps_data[h]->satellites_visible);
	}

	return;
}

void dump_rxpwrlvl_data(RssiData *rssi_container) {
	for(int j=0;j<rssi_container->n_rssi_data;j++){

		printf("[RxPwrLvl] Sample %d of %d. Rx Power [dB]: ",j+1,(int)rssi_container->n_rssi_data);
		for(int freq_id=0;freq_id<4;freq_id++)
			printf("%d | ",rssi_container->rssi_data[j]->lte_data[freq_id]);
		printf("\n");
	}

}
