/*
 * main.c
 *
 *  Created on: Apr 9, 2018
 *      Author: ltorresf
 */

#include "stdlib.h"
#include "stdio.h"
#include "variables.h"
//#include "/home/ltorresf/src/openairinterface5g/openair1/PHY/defs.h"
//#include "defs.h"

void dump_frame_parms(LTE_DL_FRAME_PARMS *frame_parms);

int main () {
	FILE *fileptr;
	char *buffer;
	long filelen;

	//fileptr = fopen("UE_0.dat","rb");
	//fileptr = fopen("20180409_UE_5336.dat","rb");
	//fileptr = fopen("20180409_UE_500.dat","rb");
	//fileptr = fopen("20180410_UE_2356.dat","rb");
	fileptr = fopen("20180410_UE_9408.dat","rb");
	if (fileptr == NULL){
	  printf("Error! Could not open file\n");
	  exit(-1); // must include stdlib.h
	}

	fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
	filelen = ftell(fileptr);             // Get the current byte offset in the file
	rewind(fileptr);                      // Jump back to the beginning of the file

	buffer = (char *)malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
	fread(buffer, filelen, 1, fileptr); // Read in the entire file
	fclose(fileptr);

	//printf("%c%c%c\n",buffer[0],buffer[1],buffer[2]);
	//printf("%s\n",buffer);

	printf("File length: %ld bytes vs. Data type length: %ld\n",filelen,sizeof(PHY_VARS_UE));
	printf("%x-%x-%x-%x\n",buffer[0],buffer[1],buffer[2],buffer[3]);

	//rx_tx_thread_data *rtd = &buffer;	//this array pointer &buffer points to the whole array
	//rx_tx_thread_data *rtd = buffer;	//this is a pointer that points to the first element of the array buffer

	//rx_tx_thread_data *rtd = buffer;

	// printf("%x-%x-%x\n",rtd[0],rtd[1],rtd[2]);

	//UE_rxtx_proc_t *proc = rtd->proc;
	//PHY_VARS_UE    *ue   = rtd->ue;

	//PHY_VARS_UE    *ue = (PHY_VARS_UE*) buffer;

	LTE_DL_FRAME_PARMS    *frame_parms = (LTE_DL_FRAME_PARMS*) buffer;
	  //LTE_DL_FRAME_PARMS *frame_parms = &ue->frame_parms;
	  //LTE_UE_COMMON *common_vars   = &ue->common_vars;

	  //printf("Symbol size: %d,N_RB_DL = %d eNb_id = %d\n",frame_parms->ofdm_symbol_size,frame_parms->N_RB_DL,common_vars->eNb_id);
	printf("Symbol size: %d,N_RB_DL = %d\n",frame_parms->ofdm_symbol_size,frame_parms->N_RB_DL);

	//printf("Frame: %d, sub-frame: %d\n",proc->frame_rx,proc->subframe_rx);

	dump_frame_parms(frame_parms);
}

void dump_frame_parms(LTE_DL_FRAME_PARMS *frame_parms) {
printf("frame_parms->N_RB_DL=%u\n",frame_parms->N_RB_DL);
  printf("frame_parms->N_RB_UL=%u\n",frame_parms->N_RB_UL);
  printf("frame_parms->N_RBG=%u\n",frame_parms->N_RBG);
  printf("frame_parms->N_RBGS=%u\n",frame_parms->N_RBGS);
  printf("frame_parms->Nid_cell=%d\n",frame_parms->Nid_cell);
  printf("frame_parms->Ncp=%d\n",frame_parms->Ncp);
  printf("frame_parms->threequarter_fs=%u\n",frame_parms->threequarter_fs);
  printf("frame_parms->node_id=%u\n",frame_parms->node_id);
  printf("frame_parms->Ncp_UL=%d\n",frame_parms->Ncp_UL);
  printf("frame_parms->nushift=%d\n",frame_parms->nushift);
  printf("frame_parms->frame_type=%d\n",frame_parms->frame_type);
  printf("frame_parms->tdd_config=%d\n",frame_parms->tdd_config);
  printf("frame_parms->tdd_config_S=%d\n",frame_parms->tdd_config_S);
  printf("frame_parms->mode1_flag=%d\n",frame_parms->mode1_flag);
  printf("frame_parms->nb_antenna_ports_eNB=%d\n",frame_parms->nb_antenna_ports_eNB);
  printf("frame_parms->nb_antennas_tx=%d\n",frame_parms->nb_antennas_tx);
  printf("frame_parms->nb_antennas_rx=%d\n",frame_parms->nb_antennas_rx);
  printf("frame_parms->ofdm_symbol_size=%d\n",frame_parms->ofdm_symbol_size);
  printf("frame_parms->nb_prefix_samples=%d\n",frame_parms->nb_prefix_samples);
  printf("frame_parms->nb_prefix_samples0=%d\n",frame_parms->nb_prefix_samples0);
  printf("frame_parms->first_carrier_offset=%d\n",frame_parms->first_carrier_offset);
  printf("frame_parms->samples_per_tti=%d\n",frame_parms->samples_per_tti);
  printf("frame_parms->symbols_per_tti=%d\n",frame_parms->symbols_per_tti);
  printf("frame_parms->dl_symbols_in_S_subframe=%u\n",frame_parms->dl_symbols_in_S_subframe);
  printf("frame_parms->pcfich_reg={%u,%u,%u,%u}\n",frame_parms->pcfich_reg[0],frame_parms->pcfich_reg[1],frame_parms->pcfich_reg[2],frame_parms->pcfich_reg[3]);
  printf("frame_parms->pcfich_first_reg_idx=%u\n",frame_parms->pcfich_first_reg_idx);
  printf("frame_parms->SIwindowsize=%u\n",frame_parms->SIwindowsize);
  printf("frame_parms->SIPeriod=%u\n",frame_parms->SIPeriod);
  printf("frame_parms->pdsch_config_common.referenceSignalPower=%i dBm\n",frame_parms->pdsch_config_common.referenceSignalPower);
  printf("frame_parms->pdsch_config_common.p_b=%u\n",frame_parms->pdsch_config_common.p_b);
}
