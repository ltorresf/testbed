#include <gps.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "subframe2file.pb-c.h"

void dump_gps_and_rssi_to_file(struct gps_data_t *gps_buffer,int n_data){

	GpsInfo gps_info = GPS_INFO__INIT;

	gps_info.n_gps_data = n_data;
	GpsData **gps_data;
	gps_data = malloc(sizeof(GpsData*)*gps_info.n_gps_data);

	for (int h = 0; h < gps_info.n_gps_data; h++) {
		gps_data[h]=malloc (sizeof(GpsData));
		gps_data__init(gps_data[h]);

		gps_data[h]->gps_fix = malloc (sizeof(GpsFix));
		gps_fix__init(gps_data[h]->gps_fix);

		gps_data[h]->gps_fix->time = gps_buffer[h].fix.time;
		gps_data[h]->gps_fix->mode = gps_buffer[h].fix.mode;
		gps_data[h]->gps_fix->ept = gps_buffer[h].fix.ept;
		gps_data[h]->gps_fix->latitude = gps_buffer[h].fix.latitude;
		gps_data[h]->gps_fix->epy = gps_buffer[h].fix.epy;
		gps_data[h]->gps_fix->longitude = gps_buffer[h].fix.longitude;
		gps_data[h]->gps_fix->epx = gps_buffer[h].fix.epx;
		gps_data[h]->gps_fix->altitude = gps_buffer[h].fix.altitude;
		gps_data[h]->gps_fix->epv = gps_buffer[h].fix.epv;
		gps_data[h]->gps_fix->track = gps_buffer[h].fix.track;
		gps_data[h]->gps_fix->epd = gps_buffer[h].fix.epd;
		gps_data[h]->gps_fix->speed = gps_buffer[h].fix.speed;
		gps_data[h]->gps_fix->eps = gps_buffer[h].fix.eps;
		gps_data[h]->gps_fix->climb = gps_buffer[h].fix.climb;
		gps_data[h]->gps_fix->epc = gps_buffer[h].fix.epc;

		gps_data[h]->status=gps_buffer[h].status;
		gps_data[h]->satellites_used=gps_buffer[h].satellites_used;
		gps_data[h]->satellites_visible=gps_buffer[h].satellites_visible;
	}
	gps_info.gps_data=gps_data;

/*	printf("1st. %f vs. %f\n",gps_buffer[0].fix.time,gps_info.gps_data[0]->gps_fix->time);
	printf("2nd. %f vs. %f\n",gps_buffer[1].fix.time,gps_info.gps_data[1]->gps_fix->time);
	printf("3rd. %f vs. %f\n",gps_buffer[2].fix.time,gps_info.gps_data[2]->gps_fix->time);*/



    //String that contains current time to be used in filename
    time_t	rawtime=time(NULL);
    struct tm *tm = localtime(&rawtime);
    char s[20];
    char filename_gps[50];
    strftime(s, sizeof(s), "%Y%m%d.%H%M%S_", tm);	//s contains 16 char

	void *buf;
	unsigned len;

	//len = rx_tx_thread_data__get_packed_size(&rxd);
	len = gps_info__get_packed_size(&gps_info);
	buf = malloc(len);
	//rx_tx_thread_data__pack(&rxd,buf);		//serializes the message.
	gps_info__pack(&gps_info,buf);


    //snprintf(filename,50,"%s%s%d-%d.%d_%d%s",s,"UE_",UE_data->frame_parms.Nid_cell,proc_data->frame_rx,proc_data->subframe_rx,tttime,".dat");
	snprintf(filename_gps,50,"%s%s",s,"GPS.dat");
    //printf("Filename: %s, subframe: %d\n",filename,proc_data->subframe_rx);
    FILE *fd;
    if ((fd = fopen(filename_gps,"w"))!=NULL ) {
    	//fprintf(stderr,"Writing %d serialized bytes\n",len);
    	printf("Writing %d serialized bytes in %s\n",len,filename_gps);
    	fwrite(buf,len,1,fd);
    	fclose(fd);
    }
    free(buf);
    return;

}

int main() {
int rc,waiting_ms=200;
struct timeval tv;
double time_in_mill;

int a=0,l=0,n=5*60; //300 GPS samples per minute
int i=0,k=0;
double curr_ms=0.0,last_ms=0.0;
struct gps_data_t gps_data,gps_buffer[n];


memset(gps_buffer,0,n*sizeof(struct gps_data_t));

printf("INITIALLY [GPS][%d][%s][%s] %.3lf [+/-%.0f ms]. %d bytes, baudrate = %u. Coord: (%f, %f) [+/-%.3fm,+/-%.3fm], speed: %.3f, ",
		i,(gps_data.status == 0)? "NO_FIX" : "FIX",(gps_data.fix.mode == MODE_2D)? "MODE_2D" : ((gps_data.fix.mode == MODE_3D)? "MODE_3D" :((gps_data.fix.mode == MODE_NO_FIX)? "MODE_NO_FIX" : "MODE_NOT_SEEN")),
		gps_data.fix.time,1000*gps_data.fix.ept,rc,gps_data.devices.list[0].baudrate,gps_data.fix.latitude, gps_data.fix.longitude,gps_data.fix.epy,gps_data.fix.epx, gps_data.fix.speed);
printf("n_sat = %d of %d\n", gps_data.satellites_used,gps_data.satellites_visible);
//Open a session socket to the daemon
if ((rc = gps_open("localhost", "2947", &gps_data)) == -1) {
    printf("code: %d, reason: %s\n", rc, gps_errstr(rc));
    return EXIT_FAILURE;
}



/*gps_send(&gps_data,"?DEVICE={\"class\":\"DEVICE\",\"path\":\"/dev/ttyACM0\",\"cycle\":0.500}\r\n");/* //55 characters (<80), new line indicator CR-LF is ignored
gps_send(&gps_data,"?DEVICE={\"driver\":\"u-blox\",\"parity\":\"N\"}\r\n");
gps_send(&gps_data,"?DEVICE={\"baudrate\":38400,\"stopbits\":1,\"flags\":1}\r\n"); //baudrates: 9600 (default),19200, 38400, 57600, and 115200.
*/

//After the cycle is set, otherwise the DEVICES structure would not be updated
//gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, "/dev/ttyACM0");


//Set watch policy.
gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);

while (1) {
	a=0;
	while(a < n) {
		gettimeofday(&tv, NULL);
		time_in_mill = (tv.tv_sec) * 1000LL + (tv.tv_usec) / 1000 ;

		//it checks whether there is new data from the daemon waiting for our client application. If there is no immediate answer, it waits for $waiting_ms milliseconds before returning.
		if (gps_waiting (&gps_data, waiting_ms*1000)) {

			/* read data */
			if ((rc = gps_read(&gps_data)) == -1) {
				printf("error occured reading gps data. code: %d, reason: %s\n", rc, gps_errstr(rc));
			} else {
				/* Display data from the GPS receiver. */
				if ((gps_data.status == STATUS_FIX) && !isnan(gps_data.fix.time) && PACKET_SET) {
					curr_ms = gps_data.fix.time;
					if (k==0) {
						last_ms = curr_ms - 0.2;//waiting_ms/1000;
						k=1;
					}
					printf("Timing details: [i=%d] curr_ms = %f, last_ms = %f\n",i,curr_ms,last_ms);

					if ((gps_data.fix.mode == MODE_2D || gps_data.fix.mode == MODE_3D) && (curr_ms > last_ms) && !isnan(gps_data.fix.latitude) && !isnan(gps_data.fix.longitude)) {
						printf("[CPU] %.3f ms - ",time_in_mill/1000);
						printf("[GPS][%d][%s][%s] %.3lf [+/-%.0f ms]. %d bytes, baudrate = %u. Coord: (%f, %f) [+/-%.3fm,+/-%.3fm], speed: %.3f, ",
								i,(gps_data.status == 0)? "NO_FIX" : "FIX",(gps_data.fix.mode == MODE_2D)? "MODE_2D" : ((gps_data.fix.mode == MODE_3D)? "MODE_3D" :((gps_data.fix.mode == MODE_NO_FIX)? "MODE_NO_FIX" : "MODE_NOT_SEEN")),
								gps_data.fix.time,1000*gps_data.fix.ept,rc,gps_data.devices.list[0].baudrate,gps_data.fix.latitude, gps_data.fix.longitude,gps_data.fix.epy,gps_data.fix.epx, gps_data.fix.speed);
						printf("n_sat = %d of %d.\n", gps_data.satellites_used,gps_data.satellites_visible);
						memcpy(&gps_buffer[a],&gps_data,sizeof(struct gps_data_t));
						last_ms = gps_data.fix.time;
						gps_clear_fix(&gps_data.fix);
						//usleep(400000);
						//PACKET_SET = 0;
						a++;
						//j=1;
					} else if (curr_ms > last_ms)  {
/*							//memset(&gps_buffer[i%5],0,sizeof(struct gps_data_t));
						memcpy(&gps_buffer[a],&gps_data,sizeof(struct gps_data_t));
						//gps_buffer[i%5].fix.time=gps_data.fix.time;
						LOG_I(PHY,"[CPU] %.3f ms - ",time_in_mill/1000);
						printf("[GPS][%d][%s][%s] %.3lf [+/-%.0f ms]. %d bytes, baudrate = %u. Coord: (%f, %f) [+/-%.3fm,+/-%.3fm], speed: %.3f, ",
								i,(gps_data.status == 0)? "NO_FIX" : "FIX",(gps_data.fix.mode == MODE_2D)? "MODE_2D" : ((gps_data.fix.mode == MODE_3D)? "MODE_3D" :((gps_data.fix.mode == MODE_NO_FIX)? "MODE_NO_FIX" : "MODE_NOT_SEEN")),
								gps_data.fix.time,1000*gps_data.fix.ept,rc,gps_data.devices.list[0].baudrate,gps_data.fix.latitude, gps_data.fix.longitude,gps_data.fix.epy,gps_data.fix.epx, gps_data.fix.speed);
						printf("n_sat = %d of %d\n", gps_data.satellites_used,gps_data.satellites_visible);*/
						/*if(j==1) {
							j=0;
						} else {*/
							printf("No GPS lock. GPS data received successfully, however no lock has been achieved.\n");
							memcpy(&gps_buffer[a],&gps_data,sizeof(struct gps_data_t));
							last_ms = gps_data.fix.time;
							a++;
						//}
					}

				} else {
					//Some GPS information has been retrived, but it is still incomplete and another iteration is needed to complete it
					//memset(&gps_buffer[i%5],0,sizeof(struct gps_data_t));
					//LOG_I(PHY,"No fix obtained in a %d-ms time interval\n",waiting_ms);
				}
			}
		} else {
			//Once this condition is met, $waiting_ms milliseconds have already passed. This happens after clearing the buffer.
			//memset(&gps_buffer[i%5],0,sizeof(struct gps_data_t));
			//printf("Timeout in receiving data after %d ms\n",waiting_ms);
		}
		i++;
	}

	printf("------------------------------------------------------------------------------------------------\n");
	for(l=0; l < n; l++) {
		//printf("[CPU] %.3f ms - ",time_in_mill/1000);

		printf("[GPS][%d][%s][%s] %.3lf [+/-%.0f ms]. %d bytes, baudrate = %u. Coord: (%f, %f) [+/-%.3fm,+/-%.3fm], speed: %.3f, ",
				i,(gps_buffer[l].status == 0)? "NO_FIX" : "FIX",(gps_buffer[l].fix.mode == MODE_2D)? "MODE_2D" : ((gps_buffer[l].fix.mode == MODE_3D)? "MODE_3D" :((gps_buffer[l].fix.mode == MODE_NO_FIX)? "MODE_NO_FIX" : "MODE_NOT_SEEN")),
				gps_buffer[l].fix.time,1000*gps_buffer[l].fix.ept,rc,gps_buffer[l].devices.list[0].baudrate,gps_buffer[l].fix.latitude, gps_buffer[l].fix.longitude,gps_buffer[l].fix.epy,gps_buffer[l].fix.epx, gps_buffer[l].fix.speed);
		printf("n_sat = %d of %d\n", gps_buffer[l].satellites_used,gps_buffer[l].satellites_visible);

	}
	dump_gps_and_rssi_to_file(gps_buffer,n);
	printf("------------------------------------------------------------------------------------------------\n");
}

/* When you are done... */
gps_stream(&gps_data, WATCH_DISABLE, NULL);
gps_close (&gps_data);

return EXIT_SUCCESS;
}
