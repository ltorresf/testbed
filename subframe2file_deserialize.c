#include <stdio.h>
#include <stdlib.h>
#include "subframe2file.pb-c.h"
#include "variables.h"
#define MAX_MSG_SIZE 2097152

void dump_frame_parms(Frameparms *frame_parms);
void dump_ue_rxtx_proc(UeRxTxProc *proc_data);
//void dump_phy_vars_ue(PhyVarsUe *UE_data);
void dump_phy_vars_ue(PhyVarsUe *UE_data,UeRxTxProc *proc_data);
void dump_ue_proc(UeProc *proc_data);
void dump_phy_measurements(PhyMeasurements *measurements);

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
//RxTxThreadData *msg;
	PhyVarsUe *msg;

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
  msg = phy_vars_ue__unpack(NULL, msg_len, buf);
  if (msg == NULL)
  {
    fprintf(stderr, "error unpacking incoming message\n");
    exit(1);
  }

  // display the message's fields.
  //printf("Received: n_rb_dl=%d,n_rb_ul=%d\n",msg->ue->frame_param->n_rb_dl,msg->ue->frame_param->n_rb_ul);  // required field
  //if (msg->has_b)                   // handle optional field
  //  printf("  n_rb_ul=%d",msg->frame_param->n_rb_ul);
  //printf("\n");
  //printf("subframe_rx = %d,ue_scan = %d\n",msg->proc->subframe_rx,msg->ue->ue_scan);
  //printf("ue_scan = %d\n",msg->ue->ue_scan);

  UeRxTxProc	*procd = msg->proc->proc_rxtx;
  //PhyVarsUe    *ued   = msg->ue;
  PhyVarsUe    *ued   = msg;
  Frameparms *frame_parms = msg->frame_parms;
  PhyMeasurements *measurements = msg->measurements;

  //dump_ue_proc(ued->proc);
  //dump_phy_vars_ue(ued,procd);
  //dump_frame_parms(frame_parms);
  dump_ue_rxtx_proc(ued->proc->proc_rxtx);
  //dump_phy_measurements(measurements);

  // Free the unpacked message
  phy_vars_ue__free_unpacked(msg, NULL);
  return 0;
}

void dump_phy_measurements(PhyMeasurements *measurements) {
	printf("measurements->rssi=%u\n",measurements->rssi);
	printf("measurements->n_adj_cells=%d\n",measurements->n_adj_cells);
	for (int i = 0; i < measurements->n_adj_cell_id; i++)
			printf("measurements->adj_cell_id[%d] = %u\n",i,measurements->adj_cell_id[i]);
	for (int i = 0; i < measurements->n_rsrq; i++)
			printf("measurements->rsrq[%d] = %u\n",i,measurements->rsrq[i]);
	for (int i = 0; i < measurements->n_rsrp; i++)
			printf("measurements->rsrp[%d] = %u\n",i,measurements->rsrp[i]);
	for (int i = 0; i < measurements->n_rsrp_filtered; i++)
			printf("measurements->rsrp_filtered[%d] = %f\n",i,measurements->rsrp_filtered[i]);
	for (int i = 0; i < measurements->n_rsrq_filtered; i++)
			printf("measurements->rsrq_filtered[%d] = %f\n",i,measurements->rsrq_filtered[i]);
	for (int i = 0; i < measurements->n_n0_power; i++)
			printf("measurements->n0_power[%d] = %u\n",i,measurements->n0_power[i]);
	for (int i = 0; i < measurements->n_n0_power_db; i++)
			printf("measurements->n0_power_db[%d] = %u\n",i,measurements->n0_power_db[i]);
	printf("measurements->n0_power_tot=%u\n",measurements->n0_power_tot);
	printf("measurements->n0_power_tot_db=%u\n",measurements->n0_power_tot_db);
	printf("measurements->n0_power_avg=%u\n",measurements->n0_power_avg);
	printf("measurements->n0_power_avg_db=%u\n",measurements->n0_power_avg_db);
	printf("measurements->n0_power_tot_dbm=%hi\n",measurements->n0_power_tot_dbm);
	//UE measurements

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
				printf("measurements->rx_spatial_power[%d][%d][%d] = %i\n",i,j,k,measurements->rx_spatial_power[i]->value3[j]->value2[k]);
	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					printf("measurements->rx_spatial_power_dB[%d][%d][%d] = %hu\n",i,j,k,measurements->rx_spatial_power_db[i]->value3[j]->value2[k]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			for (int j = 0; j < NB_ANTENNAS_RX; j++)
				printf("measurements->rx_power[%d][%d] = %i\n",i,j,measurements->rx_power[i]->value2[j]);
	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			for (int j = 0; j < NB_ANTENNAS_RX; j++)
				printf("measurements->rx_power_dB[%d][%d] = %hu\n",i,j,measurements->rx_power_db[i]->value2[j]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			printf("measurements->rx_power_tot[%d] = %i\n",i,measurements->rx_power_tot[i]);
	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			printf("measurements->rx_power_tot_dB[%d] = %hu\n",i,measurements->rx_power_tot_db[i]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			printf("measurements->rx_power_avg[%d] = %i\n",i,measurements->rx_power_avg[i]);
	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			printf("measurements->rx_power_avg_dB[%d] = %hu\n",i,measurements->rx_power_avg_db[i]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			printf("measurements->wideband_cqi_tot[%d] = %i\n",i,measurements->wideband_cqi_tot[i]);
	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			printf("measurements->wideband_cqi_avg[%d] = %i\n",i,measurements->wideband_cqi_avg[i]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			printf("measurements->rx_rssi_dBm[%d] = %hi\n",i,measurements->rx_rssi_dbm[i]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			for (int j = 0; j < 2; j++)
				printf("measurements->rx_correlation[%d][%d] = %i\n",i,j,measurements->rx_correlation[i]->value2[j]);
	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			for (int j = 0; j < 2; j++)
				printf("measurements->rx_correlation_dB[%d][%d] = %i\n",i,j,measurements->rx_correlation_db[i]->value2[j]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX+1; i++)
				for (int j = 0; j < 4; j++)
					printf("measurements->precoded_cqi_dB[%d][%d] = %i\n",i,j,measurements->precoded_cqi_db[i]->value2[j]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			for (int j = 0; j < NB_ANTENNAS_RX; j++)
				for (int k = 0; k < NUMBER_OF_SUBBANDS_MAX; k++)
					printf("measurements->subband_cqi[%d][%d][%d] = %i\n",i,j,k,measurements->subband_cqi[i]->value3[j]->value2[k]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			for (int j = 0; j < NUMBER_OF_SUBBANDS_MAX; j++)
				printf("measurements->subband_cqi_tot[%d][%d] = %i\n",i,j,measurements->subband_cqi_tot[i]->value2[j]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			for (int j = 0; j < NB_ANTENNAS_RX; j++)
				for (int k = 0; k < NUMBER_OF_SUBBANDS_MAX; k++)
					printf("measurements->subband_cqi_dB[%d][%d][%d] = %i\n",i,j,k,measurements->subband_cqi_db[i]->value3[j]->value2[k]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			for (int j = 0; j < NUMBER_OF_SUBBANDS_MAX; j++)
				printf("measurements->subband_cqi_tot_dB[%d][%d] = %i\n",i,j,measurements->subband_cqi_tot_db[i]->value2[j]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			for (int j = 0; j < NB_ANTENNAS_RX; j++)
				printf("measurements->wideband_pmi_re[%d][%d] = %i\n",i,j,measurements->wideband_pmi_re[i]->value2[j]);

	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
			for (int j = 0; j < NB_ANTENNAS_RX; j++)
				printf("measurements->wideband_pmi_im[%d][%d] = %i\n",i,j,measurements->wideband_pmi_im[i]->value2[j]);
//abc
	for (int i = 0; i < measurements->n_subband_pmi_re; i++)
			for (int j = 0; j < measurements->subband_pmi_re[i]->n_value3; j++)
				for (int k = 0; k < measurements->subband_pmi_re[i]->value3[j]->n_value2; k++)
					printf("measurements->subband_pmi_re[%d][%d][%d] = %i\n",i,j,k,measurements->subband_pmi_re[i]->value3[j]->value2[k]);

	for (int i = 0; i < measurements->n_subband_pmi_im; i++)
			for (int j = 0; j < measurements->subband_pmi_im[i]->n_value3; j++)
				for (int k = 0; k < measurements->subband_pmi_im[i]->value3[j]->n_value2; k++)
					printf("measurements->subband_pmi_im[%d][%d][%d] = %i\n",i,j,k,measurements->subband_pmi_im[i]->value3[j]->value2[k]);

	for (int i = 0; i < measurements->n_selected_rx_antennas; i++)
		for (int j = 0; j < measurements->selected_rx_antennas[i]->n_value2; j++)
			printf("measurements->selected_rx_antennas[%d][%d] = %u\n",i,j,measurements->selected_rx_antennas[i]->value2[j]);


	for (int i = 0; i < NUMBER_OF_CONNECTED_eNB_MAX; i++)
		printf("measurements->rank[%d] = %u\n",i,measurements->rank[i]);

	//some excluded
	printf("measurements->nb_antennas_rx=%u\n",measurements->nb_antennas_rx);
	return;
}

void dump_frame_parms(Frameparms *frame_parms) {
	printf("frame_parms->N_RB_DL=%u\n",frame_parms->n_rb_dl);
	printf("frame_parms->N_RB_UL=%u\n",frame_parms->n_rb_ul);
	printf("frame_parms->N_RBG=%u\n",frame_parms->n_rbg);
	printf("frame_parms->N_RBGS=%u\n",frame_parms->n_rbgs);
	printf("frame_parms->Nid_cell=%d\n",frame_parms->nid_cell);
	printf("frame_parms->Nid_cell_mbsfn=%d\n",frame_parms->nid_cell_mbsfn);
	printf("frame_parms->Ncp=%d\n",frame_parms->ncp);
	printf("frame_parms->Ncp_UL=%d\n",frame_parms->ncp_ul);
	printf("frame_parms->nushift=%d\n",frame_parms->nushift);
	printf("frame_parms->frame_type=%d\n",frame_parms->frame_type);
	printf("frame_parms->tdd_config=%d\n",frame_parms->tdd_config);
	printf("frame_parms->tdd_config_S=%d\n",frame_parms->tdd_config_s);
	printf("frame_parms->srsX=%u\n",frame_parms->srsx);
	printf("frame_parms->node_id=%u\n",frame_parms->node_id);
	printf("frame_parms->freq_idx=%u\n",frame_parms->freq_idx);
	//some excluded here
	printf("frame_parms->mode1_flag=%d\n",frame_parms->mode1_flag);
	printf("frame_parms->threequarter_fs=%d\n",frame_parms->threequarter_fs);
	printf("frame_parms->ofdm_symbol_size=%d\n",frame_parms->ofdm_symbol_size);
	printf("frame_parms->nb_prefix_samples=%d\n",frame_parms->nb_prefix_samples);
	printf("frame_parms->nb_prefix_samples0=%d\n",frame_parms->nb_prefix_samples0);
	printf("frame_parms->first_carrier_offset=%d\n",frame_parms->first_carrier_offset);
	printf("frame_parms->samples_per_tti=%d\n",frame_parms->samples_per_tti);
	printf("frame_parms->symbols_per_tti=%d\n",frame_parms->symbols_per_tti);
	printf("frame_parms->dl_symbols_in_S_subframe=%u\n",frame_parms->dl_symbols_in_s_subframe);
	printf("frame_parms->ul_symbols_in_s_subframe=%u\n",frame_parms->ul_symbols_in_s_subframe);
	printf("frame_parms->nb_antennas_tx=%d\n",frame_parms->nb_antennas_tx);
	printf("frame_parms->nb_antennas_rx=%d\n",frame_parms->nb_antennas_rx);
	printf("frame_parms->nb_antenna_ports_enb=%d\n",frame_parms->nb_antenna_ports_enb);
	//common configuration excluded here
	printf("frame_parms->num_mbsfn_config=%u\n",frame_parms->num_mbsfn_config);
	for(int i=0; i<MAX_MBSFN_AREA;i++)
		  printf("frame_parms->MBSFN_config[%d]: radioframeAllocationPeriod = %d, radioframeAllocationOffset = %d, fourFrames_flag = %d, mbsfn_SubframeConfig = %d\n",
				  i,frame_parms->mbsfn_config[i]->radioframeallocationperiod,frame_parms->mbsfn_config[i]->radioframeallocationoffset,frame_parms->mbsfn_config[i]->fourframes_flag,frame_parms->mbsfn_config[i]->mbsfn_subframeconfig);
	printf("frame_parms->maxharq_msg3tx=%u\n",frame_parms->maxharq_msg3tx);
	printf("frame_parms->SIwindowsize=%u\n",frame_parms->siwindowsize);
	printf("frame_parms->SIPeriod=%u\n",frame_parms->siperiod);
	printf("frame_parms->pcfich_reg={%u,%u,%u,%u}\n",frame_parms->pcfich_reg[0],frame_parms->pcfich_reg[1],frame_parms->pcfich_reg[2],frame_parms->pcfich_reg[3]);
	printf("frame_parms->pcfich_first_reg_idx=%u\n",frame_parms->pcfich_first_reg_idx);
	printf("frame_parms->phich_reg={[%u,%u,%u][%u,%u,%u][%u,%u,%u][%u,%u,%u]}\n",
			frame_parms->phich_reg_outer[0]->phich_reg_inner[0],frame_parms->phich_reg_outer[0]->phich_reg_inner[1],frame_parms->phich_reg_outer[0]->phich_reg_inner[2],
			frame_parms->phich_reg_outer[1]->phich_reg_inner[0],frame_parms->phich_reg_outer[1]->phich_reg_inner[1],frame_parms->phich_reg_outer[1]->phich_reg_inner[2],
			frame_parms->phich_reg_outer[2]->phich_reg_inner[0],frame_parms->phich_reg_outer[2]->phich_reg_inner[1],frame_parms->phich_reg_outer[2]->phich_reg_inner[2],
			frame_parms->phich_reg_outer[7]->phich_reg_inner[0],frame_parms->phich_reg_outer[7]->phich_reg_inner[1],frame_parms->phich_reg_outer[7]->phich_reg_inner[2]);
	//printf("frame_parms->pdsch_config_common.referenceSignalPower=%i dBm\n",frame_parms->pdsch_config_common.referenceSignalPower);
	// printf("frame_parms->pdsch_config_common.p_b=%u\n",frame_parms->pdsch_config_common.p_b);

	return;
}

void dump_ue_proc(UeProc *proc_data) {
	printf("proc_data->CC_id=%u\n",proc_data->cc_id);
	printf("proc_data->timestamp_tx=%ld\n",proc_data->timestamp_rx);
	printf("proc_data->instance_cnt_synch=%d\n",proc_data->instance_cnt_synch);
	return;
}

void dump_ue_rxtx_proc(UeRxTxProc *proc_data) {
	printf("proc_data->proc_id=%d\n",proc_data->proc_id);
	printf("proc_data->CC_id=%u\n",proc_data->cc_id);
	printf("proc_data->timestamp_tx=%ld\n",proc_data->timestamp_tx);
	printf("proc_data->subframe_tx=%d\n",proc_data->subframe_tx);
	printf("proc_data->subframe_rx=%d\n",proc_data->subframe_rx);
	printf("proc_data->frame_tx=%d\n",proc_data->frame_tx);
	printf("proc_data->frame_rx=%d\n",proc_data->frame_rx);
	printf("proc_data->instance_cnt_rxtx=%d\n",proc_data->instance_cnt_rxtx);
	printf("proc_data->pthread_rxtx=%lu\n",proc_data->pthread_rxtx);	//check the variable type
	//excluded: attr_rxtx
	//excluded: cond_rxtx
	//excluded: mutex_rxtx
	//excluded: sched_param_rxtx
	printf("proc_data->instance_cnt_slot1_dl_processing=%d\n",proc_data->instance_cnt_slot1_dl_processing);
	printf("proc_data->pthread_slot1_dl_processing=%lu\n",proc_data->pthread_slot1_dl_processing);
	//excluded: attr_slot1_dl_processing
	//excluded: cond_slot1_dl_processing
	//excluded: mutex_slot1_dl_processing
	printf("proc_data->chan_est_pilot0_slot1_available=%u\n",proc_data->chan_est_pilot0_slot1_available);
	printf("proc_data->chan_est_slot1_available=%u\n",proc_data->chan_est_slot1_available);
	printf("proc_data->llr_slot1_available=%u\n",proc_data->llr_slot1_available);
	printf("proc_data->dci_slot0_available=%u\n",proc_data->dci_slot0_available);
	printf("proc_data->first_symbol_available=%u\n",proc_data->first_symbol_available);
	//excluded: sched_param_fep_slot1
	printf("proc_data->sub_frame_start=%d\n",proc_data->sub_frame_start);
	printf("proc_data->sub_frame_step=%d\n",proc_data->sub_frame_step);
	printf("proc_data->gotIQs=%lu\n",proc_data->gotiqs);	//check the variable type
	return;
}

void dump_phy_vars_ue(PhyVarsUe *UE_data,UeRxTxProc *proc_data) {
	printf("UE_data->mod_id=%u\n",UE_data->mod_id);
	printf("UE_data->cc_id=%u\n",UE_data->cc_id);
	printf("UE_data->mode=%u\n",UE_data->mode);
	printf("UE_data->ue_scan=%d\n",UE_data->ue_scan);
	printf("UE_data->ue_scan_carrier=%d\n",UE_data->ue_scan_carrier);
	printf("UE_data->is_synchronized=%d\n",UE_data->is_synchronized);
	printf("UE_data->no_timing_correction=%d\n",UE_data->no_timing_correction);
	printf("UE_data->tx_total_gain_dB=%u\n",UE_data->tx_total_gain_db);
	printf("UE_data->rx_total_gain_dB=%u\n",UE_data->rx_total_gain_db);
	printf("UE_data->tx_power_max_dBm=%d\n",UE_data->tx_power_max_dbm);
	printf("UE_data->n_connected_eNB=%u\n",UE_data->n_connected_enb);
	printf("UE_data->ho_initiated=%u\n",UE_data->ho_initiated);
	printf("UE_data->ho_triggered=%u\n",UE_data->ho_triggered);
	printf("proc_data->subframe_rx=%d\n",proc_data->subframe_rx);
/*	for(int i=1; i< 100; i++) {//UE_data->frame_parms.samples_per_tti*10
		if (i==1) printf("UE_data->common_vars.rxdata[0][i] =");
		printf(" %x ",UE_data->common_vars->rxdata[0]->lte_data[i-1]);
		if ((i%20==0 && i!=1) || i==100) printf("\n");
	}
	for(int i=1; i<= 100; i++) {//UE_data->frame_parms.samples_per_tti*10
		if (i==1) printf("\nUE_data->common_vars.rxdata[0][samples_per_tti-100] = \n");
		printf(" %x ",UE_data->common_vars->rxdata[0]->lte_data[UE_data->frame_parms->samples_per_tti-60+i-1]);
		if ((i%20==0 && i!=1) || i==100) printf("\n");
	}*/
	//dump_frame_parms(UE_data->frame_parms);
	for (int i = 0; i < UE_data->n_current_thread_id; i++)
		printf("UE_data->current_thread_id[%d] = %u\n",i,UE_data->current_thread_id[i]);
	printf("UE_data->IMSImod1024=%u\n",UE_data->imsimod1024);
	printf("UE_data->PF=%u\n",UE_data->pf);
	printf("UE_data->PO=%u\n",UE_data->po);
	for (int i = 0; i < UE_data->n_ue_mode; i++)
		printf("UE_data->ue_mode[%d] = %u\n",i,UE_data->ue_mode[i]);
	printf("UE_data->high_speed_flag=%u\n",UE_data->high_speed_flag);
	printf("UE_data->perfect_ce=%u\n",UE_data->perfect_ce);
	printf("UE_data->ch_est_alpha=%d\n",UE_data->ch_est_alpha);
	printf("UE_data->turbo_iterations=%d\n",UE_data->turbo_iterations);
	printf("UE_data->turbo_cntl_iterations=%d\n",UE_data->turbo_cntl_iterations);
	printf("UE_data->generate_prach=%u\n",UE_data->generate_prach);
	printf("UE_data->prach_cnt=%u\n",UE_data->prach_cnt);
	printf("UE_data->prach_PreambleIndex=%u\n",UE_data->prach_preambleindex);
	printf("UE_data->decode_SIB=%u\n",UE_data->decode_sib);
	printf("UE_data->decode_MIB=%u\n",UE_data->decode_mib);
	printf("UE_data->rx_offset=%d\n",UE_data->rx_offset);
	printf("UE_data->rx_offset_diff=%d\n",UE_data->rx_offset_diff);
	printf("UE_data->time_sync_cell=%d\n",UE_data->time_sync_cell);
	printf("UE_data->timing_advance=%d\n",UE_data->timing_advance);
	printf("UE_data->hw_timing_advance=%d\n",UE_data->hw_timing_advance);
	printf("UE_data->N_TA_offset=%d\n",UE_data->n_ta_offset);
	printf("UE_data->is_secondary_ue=%u\n",UE_data->is_secondary_ue);
	printf("UE_data->has_valid_precoder=%u\n",UE_data->has_valid_precoder);
	printf("UE_data->log2_maxp=%d\n",UE_data->log2_maxp);
	printf("UE_data->mac_enabled=%d\n",UE_data->mac_enabled);
	printf("UE_data->init_averaging=%d\n",UE_data->init_averaging);
	printf("UE_data->sinr_eff=%f\n",UE_data->sinr_eff);
	printf("UE_data->N0=%f\n",UE_data->n0);
	for (int i = 0; i < UE_data->n_transmission_mode; i++)
		printf("UE_data->transmission_mode[%d] = %u\n",i,UE_data->transmission_mode[i]);
	return;
}
