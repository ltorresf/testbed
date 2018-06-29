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

void dump_gps_data(GpsData **gps_data,int n_data);
void dump_rxpwrlvl_data(RssiData *rssi_data);
void dump_rxpwrlvl_gps(GpsData **gps_data,RssiData **rssi_container,GpsInfo *msg);
void arcgis_input_generator(GpsInfo *msg);
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

	GpsInfo *msg;

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

  printf("timestamp,latitude,longitude,speed,LTE band 20,LTE band 3,LTE band 7\n");

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
      size_t msg_len = read_buffer(fileptr,max_msg_size, buf);
      //printf("File size = %lu kB\n",msg_len/1024);
      fclose(fileptr);

      // Unpack the message using protobuf-c.
      msg = gps_info__unpack(NULL, msg_len, buf);
      if (msg == NULL)
      {
        fprintf(stderr, "error unpacking incoming message\n");
        exit(1);
      }

      GpsData **gps_data = msg->gps_data;
      RssiData **rssi_container = msg->rssi_container;

      //dump_gps_data(gps_data,msg->n_gps_data);
      //dump_rxpwrlvl_data(rssi_container);
      dump_rxpwrlvl_gps(gps_data,rssi_container,msg);

      //arcgis_input_generator(msg); //FIXME: incongruent values with last update

      // Free the unpacked message
      gps_info__free_unpacked(msg, NULL);
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

void arcgis_input_generator(GpsInfo *gps_info) {
	time_t int_time;
	int rssi_cnt=0,gps_cnt=0,gps_ix=0,freq_id=0;
	double tmp,tmp2,b3,b7,b20,frac_time;
	double tmp_rssi_time_diff[gps_info->n_gps_data];
	struct tm  ts;
	char timestamp_buf[80],timestamp_buf2[80],frac_buf[80];
	GpsData **gps_data = gps_info->gps_data;
	  RssiData **rssi_container = gps_info->rssi_container;

	  //header of the log file
	  //printf("timestamp,latitude,longitude,speed,b20-1,b20-2,b3-1,b3-2,b3-3,b3-4,b3-5,b3-6,b7-1,b7-2,b7-3,b7-4\n");

	  //printf("timestamp,latitude,longitude,speed,LTEband20,LTEband3,LTEband7\n");

	//Only the RSSI samples that contain data are processed
	while (rssi_container[0]->rssi_data[rssi_cnt]->rssi_unix_time > 1) {

		//printf("|| RSSI Sample No %d, RSSI Unix Timestamp: %.3f ",rssi_cnt,rssi_container->rssi_data[rssi_cnt]->rssi_unix_time);

		//no time difference should be higher than 1000 seconds (arbitrary number)
		tmp=1000;

		for(gps_cnt=0;gps_cnt<gps_info->n_gps_data;gps_cnt++) {
			tmp_rssi_time_diff[gps_cnt] = fabs(rssi_container[0]->rssi_data[rssi_cnt]->rssi_unix_time - gps_data[gps_cnt]->gps_unix_time);
			if(tmp_rssi_time_diff[gps_cnt] < tmp) {
				tmp = tmp_rssi_time_diff[gps_cnt];
				gps_ix = gps_cnt;
			}
		}

		//printf("|| GPS Sample No %d, GPS Unix Timestamp = %.3f >> Time diff = %.3f || GPS Timestamp = %.3lf ||\n",gps_ix,gps_data[gps_ix]->gps_unix_time,tmp,gps_data[gps_ix]->gps_fix->time);

#ifdef BAND_SCAN		//ArcGIS: latest. the Rx signal level for the band is calculated in C

		//printing out GPS data
		frac_time = modf(gps_data[gps_ix]->gps_fix->time,&tmp2);
		snprintf(frac_buf, 10, "%f\n", frac_time);
		int_time = tmp2;

		ts = *localtime(&int_time);
		strftime(timestamp_buf, sizeof(timestamp_buf), "%a %Y-%m-%d %H:%M:%S", &ts);
		strftime(timestamp_buf2, sizeof(timestamp_buf2), "%Z", &ts);
		printf("%s.%c %s,%.6f,%.6f,%.1f km/h,",timestamp_buf,frac_buf[2],timestamp_buf2,gps_data[gps_ix]->gps_fix->latitude,gps_data[gps_ix]->gps_fix->longitude,18*(gps_data[gps_ix]->gps_fix->speed)/5);

		//printing out RSSI data (in dB) -> B20, B3, B7
		printf("%.1f,%.1f,%.1f\n",rssi_container[0]->rssi_data[rssi_cnt]->rssi_val[0],rssi_container[0]->rssi_data[rssi_cnt]->rssi_val[1],rssi_container[0]->rssi_data[rssi_cnt]->rssi_val[2]);
#endif


#ifdef BAND_AVERAGES		//ArcGIS: 5-MHz channels are used for calculating the Rx Signal level

		//printing out GPS data
		frac_time = modf(gps_data[gps_ix]->gps_fix->time,&tmp2);
		snprintf(frac_buf, 10, "%f\n", frac_time);
		int_time = tmp2;

		ts = *localtime(&int_time);
		strftime(timestamp_buf, sizeof(timestamp_buf), "%a %Y-%m-%d %H:%M:%S", &ts);
		strftime(timestamp_buf2, sizeof(timestamp_buf2), "%Z", &ts);
		printf("%s.%c %s,%.6f,%.6f,%.1f km/h,",timestamp_buf,frac_buf[2],timestamp_buf2,gps_data[gps_ix]->gps_fix->latitude,gps_data[gps_ix]->gps_fix->longitude,18*(gps_data[gps_ix]->gps_fix->speed)/5);

		//printing out RSSI data (in dB)
		b20=0,b3=0,b7=0;
		//Band 20, considering 2 samples: 811 - 821 MHz (10-MHz BW)
		for(freq_id=0;freq_id<2;freq_id++) {
			b20+=rssi_container[0]->rssi_data[rssi_cnt]->rssi_val[freq_id];
		}
		//Band 3, considering 4 samples: 1805 - 1825 MHz (20-MHz BW)
		for(freq_id=6;freq_id<10;freq_id++)
			b3+=rssi_container[0]->rssi_data[rssi_cnt]->rssi_val[freq_id];
		//Band 7, considering 4 samples: 2640 - 2660 MHz (10-MHz BW)
		for(freq_id=2;freq_id<6;freq_id++)
			b7+=rssi_container[0]->rssi_data[rssi_cnt]->rssi_val[freq_id];

		printf("%.1f,%.1f,%.1f\n",b20/2,b3/4,b7/4);
#endif

#ifdef MATLAB	//MATLAB

		//printing out GPS data
		printf("%.1f,%.6f,%.6f,%.1f,",gps_data[gps_ix]->gps_fix->time,gps_data[gps_ix]->gps_fix->latitude,gps_data[gps_ix]->gps_fix->longitude,gps_data[gps_ix]->gps_fix->speed);

		//printing out the signal energy levels (dB) per each 5-MHz bandwidth
		for(freq_id=0;freq_id<2;freq_id++)
			printf("%.1f,",rssi_container[0]->rssi_data[rssi_cnt]->rssi_val[freq_id]);
		for(freq_id=6;freq_id<12;freq_id++)
			printf("%.1f,",rssi_container[0]->rssi_data[rssi_cnt]->rssi_val[freq_id]);
		for(freq_id=2;freq_id<5;freq_id++)
			printf("%.1f,",rssi_container[0]->rssi_data[rssi_cnt]->rssi_val[freq_id]);
		printf("%.1f\n",rssi_container[0]->rssi_data[rssi_cnt]->rssi_val[5]);

#endif

		rssi_cnt++;
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

void dump_gps_data(GpsData **gps_data,int n_data) {
	for(int h=0;h<n_data;h++){
		printf("%.3lf [GPS][%s][%s] %.3lf [+/-%.0f ms]. Coord: (%f, %f) [+/-%.3fm,+/-%.3fm], speed: %.3f, ",gps_data[h]->gps_unix_time,
					(gps_data[h]->status == 0)? "NO_FIX" : "FIX",(gps_data[h]->gps_fix->mode == MODE_2D)? "MODE_2D" : ((gps_data[h]->gps_fix->mode == MODE_3D)? "MODE_3D" :((gps_data[h]->gps_fix->mode == MODE_NO_FIX)? "MODE_NO_FIX" : "MODE_NOT_SEEN")),
					gps_data[h]->gps_fix->time,1000*gps_data[h]->gps_fix->ept,gps_data[h]->gps_fix->latitude, gps_data[h]->gps_fix->longitude,gps_data[h]->gps_fix->epy,gps_data[h]->gps_fix->epx, gps_data[h]->gps_fix->speed);
		printf("n_sat = %d of %d\n", gps_data[h]->satellites_used,gps_data[h]->satellites_visible);
	}

	return;
}

void dump_rxpwrlvl_data(RssiData *rssi_container) {
	for(int j=0;j<rssi_container->n_rssi_data;j++){

		printf("[RxPwrLvl] Tunix = %.3f. Sample %d of %d. Rx Power [dB]: ",rssi_container->rssi_data[j]->rssi_unix_time,j+1,(int)rssi_container->n_rssi_data);
		for(int freq_id=0;freq_id<12;freq_id++)
			printf("%.1f | ",rssi_container->rssi_data[j]->rssi_val[freq_id]);
		printf("\n");
	}

}
