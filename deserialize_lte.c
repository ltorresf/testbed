#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "subframe2file.pb-c.h"
#include "variables.h"
#include "gps.h"
#include <math.h>
#include <malloc.h>
#include "assertions.h"

//#define MAX_MSG_SIZE 2192121
unsigned int max_msg_size = 40192121;
#define BAND_AVERAGES 0
#define BAND_SCAN 1
#define MATLAB 0


void dump_rxpwrlvl_gps(GpsData **gps_data,RssiData **rssi_container,GpsInfo *msg);
void dump_lte_container(LTEInfo *lte_info);
extern int write_output(const char *fname,const char *vname,void *data,int length,int dec,char format);

//! \brief Allocate \c size bytes of memory on the heap with alignment 16 and zero it afterwards.
//! If no more memory is available, this function will terminate the program with an assertion error.
static inline void* malloc16_clear( size_t size )
{
#ifdef __AVX2__
  void* ptr = memalign(32, size);
#else
  void* ptr = memalign(16, size);
#endif
  //DevAssert(ptr);
  memset( ptr, 0, size );
  return ptr;
}

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

	LTEInfo *lte_info;

  // Read packed message from standard-input.
  uint8_t *buf;
  buf = malloc(max_msg_size*sizeof(uint8_t));

  FILE *fileptr,*list_filenames;
  char *filename = NULL;
  size_t len=0;
  ssize_t read;
  int counter=0;

  //list_filenames = fopen("/home/ltorresf/src/scanner_OAI_HetNet/cmake_targets/lte_build_oai/build/list","r");
  list_filenames = fopen(argv[1],"r");
  if (list_filenames == NULL)
	  exit(-1);

  //printf("timestamp,latitude,longitude,speed,LTE band 20,LTE band 3,LTE band 7\n");

  while ((read = getline(&filename, &len, list_filenames)) != -1) {
      //printf("Processing: %s", filename);

      //eliminating the new line delimiter
      if (filename[read-1] == '\n')
            filename[read-1] = '\0';

      //opening filenames indicated in the listing file
      fileptr = fopen(filename,"rb");
      if (fileptr == NULL){
      	  printf("Error! Could not open file %s\n",filename);
      	  exit(-1); // must include stdlib.h
      	}
      size_t lte_info_len = read_buffer(fileptr,max_msg_size, buf);
      //printf("File size = %lu kB\n",msg_len/1024);
      fclose(fileptr);

      // Unpack the message using protobuf-c.
      lte_info = lteinfo__unpack(NULL, lte_info_len, buf);
      if (lte_info == NULL)
      {
        fprintf(stderr, "error unpacking incoming message\n");
        exit(1);
      }

      //LteFrame **lte_frame_container = lte_info->lte_frame_container;

      dump_lte_container(lte_info);

      //dump_rxpwrlvl_gps(gps_data,rssi_container,msg);


      // Free the unpacked message
      lteinfo__free_unpacked(lte_info, NULL);
      filename = NULL;

      counter++;
  }
  printf("%d files processed\n",counter);

  //closing file containing list of files to process
  fclose(list_filenames);
  if (filename)
      free(filename);

  return 0;
}

void dump_lte_container(LTEInfo *lte_info) {
	int cnt=0;
	char filename[50],variable[50];

	for (int h = 0; h < lte_info->n_lte_frame_container; h++) {

		if (lte_info->lte_frame_container[h]->n_frame_band20 > 0){
			for (int i=0; i<lte_info->lte_frame_container[h]->n_frame_band20; i++) {
				printf("Timestamp: %.6f, drop: %d\n",
						lte_info->lte_frame_container[h]->frame_band20[cnt]->lte_frame_unix_time,
						lte_info->lte_frame_container[h]->frame_band20[cnt]->lte_frame_drop_label);
				snprintf(filename,50,"%s%d%s","rx_signal",cnt+1,"_b20.m");
				snprintf(variable,50,"%s%d","rx_signal",cnt+1);
				write_output(filename,variable,(void*)lte_info->lte_frame_container[h]->frame_band20[0]->lte_frame_samples,
						lte_info->lte_frame_container[h]->frame_band20[0]->n_lte_frame_samples,1,1);
				cnt++;
			}
		}

		if (lte_info->lte_frame_container[h]->n_frame_band3 > 0){
			for (int i=0; i<lte_info->lte_frame_container[h]->n_frame_band3; i++) {
				printf("Timestamp: %.6f, drop: %d\n",
						lte_info->lte_frame_container[h]->frame_band3[cnt]->lte_frame_unix_time,
						lte_info->lte_frame_container[h]->frame_band3[cnt]->lte_frame_drop_label);
				snprintf(filename,50,"%s%d%s","rx_signal",cnt+1,"_b3.m");
				snprintf(variable,50,"%s%d","rx_signal",cnt+1);
				write_output(filename,variable,(void*)lte_info->lte_frame_container[h]->frame_band3[0]->lte_frame_samples,
						lte_info->lte_frame_container[h]->frame_band3[0]->n_lte_frame_samples,1,1);
				cnt++;
			}
		}

		if (lte_info->lte_frame_container[h]->n_frame_band7 > 0){
			for (int i=0; i<lte_info->lte_frame_container[h]->n_frame_band7; i++) {
				printf("Timestamp: %.6f, drop: %d\n",
						lte_info->lte_frame_container[h]->frame_band7[cnt]->lte_frame_unix_time,
						lte_info->lte_frame_container[h]->frame_band7[cnt]->lte_frame_drop_label);
				cnt++;
			}
		}

	}



}

void dump_rxpwrlvl_gps(GpsData **gps_data,RssiData **rssi_container,GpsInfo *msg) {

	RxSignal **rx_signal = msg->rx_signal;
//	RssiVal **rssi_data = rssi_container->rssi_data;
	char filename[50],variable[50];

	for(int j=0;j<msg->n_gps_data;j++){
		//printf("j = %d, rssi_container->n_rssi_data = %zd, msg->n_gps_data = %zd",j,rssi_container->n_rssi_data,msg->n_gps_data);
		printf("[%03d|%03d] [GPS] %.2lf [%s][%s] %.2lf Coord: (%.4f,%.4f), Speed: %.0f, [%d/%d]. ",j+1,(int)rssi_container[0]->n_rssi_data,gps_data[j]->gps_unix_time,
					(gps_data[j]->status == 0)? "NO_FIX" : "FIX",(gps_data[j]->gps_fix->mode == MODE_2D)? "MODE_2D" : ((gps_data[j]->gps_fix->mode == MODE_3D)? "MODE_3D" :((gps_data[j]->gps_fix->mode == MODE_NO_FIX)? "MODE_NO_FIX" : "MODE_NOT_SEEN")),
					gps_data[j]->gps_fix->time,gps_data[j]->gps_fix->latitude, gps_data[j]->gps_fix->longitude, gps_data[j]->gps_fix->speed,
					gps_data[j]->satellites_used,gps_data[j]->satellites_visible);
#if BAND_AVERAGES
		//Sequence used to store the RSSI measurements: B20,B7,B3
		printf("[RxPwrLvl] %.3f. Rx Power [dB]: ",rssi_container->rssi_data[j]->rssi_unix_time);
		for(int freq_id=0;freq_id<2;freq_id++)
			printf("%.1f | ",rssi_container->rssi_data[j]->rssi_val[freq_id]);
		for(int freq_id=6;freq_id<12;freq_id++)
			printf("%.1f | ",rssi_container->rssi_data[j]->rssi_val[freq_id]);
		for(int freq_id=2;freq_id<6;freq_id++)
			printf("%.1f | ",rssi_container->rssi_data[j]->rssi_val[freq_id]);
		printf("\n");
#endif
#if BAND_SCAN

		if(j < rssi_container[0]->n_rssi_data) {
			printf("[RxPwrLvl] %.2f. Rx Power [dB]: ",rssi_container[0]->rssi_data[j]->rssi_unix_time);
			for(int freq_id=0;freq_id<3;freq_id++)
				printf("%.1f|",rssi_container[0]->rssi_data[j]->rssi_val[freq_id]);
			printf(" [Gain][dB]: ");
			for(int freq_id=0;freq_id<3;freq_id++)
				printf("%.1f|",rssi_container[0]->rssi_data[j]->rx_gain_level[freq_id]);
		} else {
			printf("[RxPwrLvl] not available. [Gain] not available.");
		}
		printf("\n");
#endif
	}
	//creating MATLAB files that contain time-domain signals
	for (int s=0; s<msg->n_rx_signal; s++) {
		for(int b=0;b<rx_signal[s]->n_band;b++){
			snprintf(filename,50,"%s%d%s%d%s","rx_signal",s+1,"_band_",b+1,".m");
			snprintf(variable,50,"%s%d%s%d","rx_signal",s+1,"_b_",b+1);
			/*int32_t ***rx_scanner;
			rx_scanner[s][b]  = (int32_t*)malloc16_clear( (rx_signal[s]->band[b]->n_signal_samples)*sizeof(int32_t) );
			memcpy((void*)&rx_scanner[s][b][0],(void*)&rx_signal[s]->band[b]->signal_samples,rx_signal[s]->band[b]->n_signal_samples*sizeof(int32_t));
			write_output(filename,variable,(void*)&rx_scanner[s][b][0],rx_signal[s]->band[b]->n_signal_samples,1,1);*/
			write_output(filename,variable,(void*)rx_signal[s]->band[b]->signal_samples,rx_signal[s]->band[b]->n_signal_samples,1,1);
		}
	}
}
