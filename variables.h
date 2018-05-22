/*
 * variables.h
 *
 *  Created on: Apr 9, 2018
 *      Author: ltorresf
 */

#include <stdint.h>
#include <sched.h>
//#include "pthreadtypes.h"
//#include <pthreadtypes.h>

#ifndef VARIABLES_H_
#define VARIABLES_H_

#define MAX_NUM_DLSCH_SEGMENTS 16
#define MAX_NUM_ULSCH_SEGMENTS MAX_NUM_DLSCH_SEGMENTS
#define RX_NB_TH_MAX 2
//LA: Number of receiver threads (always 2, since they process even and odd subframes of the 10-ms radio frame)
#define RX_NB_TH 2
#define LTE_SLOTS_PER_SUBFRAME 2
#define NUMBER_OF_CONNECTED_eNB_MAX 3
#define NB_ANTENNAS_RX  2
#define NUMBER_OF_SUBBANDS_MAX 13
#define MAX_NUM_PHICH_GROUPS 56  //110 RBs Ng=2, p.60 36-212, Sec. 6.9

typedef struct __attribute__((packed))
{
  uint64_t padding:3;
  uint64_t pmi:1;
  uint64_t diffcqi2:26;
  uint64_t cqi2:4;
  uint64_t diffcqi1:26;
  uint64_t cqi1:4;
}
HLC_subband_cqi_rank2_2A_20MHz;

#define MAX_CQI_PAYLOAD (sizeof(HLC_subband_cqi_rank2_2A_20MHz)*8*20)
#define MAX_CQI_BITS (sizeof(HLC_subband_cqi_rank2_2A_20MHz)*8)
#define MAX_CQI_BYTES (sizeof(HLC_subband_cqi_rank2_2A_20MHz))
#define MAX_ACK_PAYLOAD 18
#define MAX_RI_PAYLOAD 6
#define MAX_NUM_RE (14*1200)
#define MAX_NUM_CHANNEL_BITS (14*1200*6)  // 14 symbols, 1200 REs, 12 bits/RE
#define NUMBER_OF_eNB_MAX 7
#define NB_BANDS_MAX 8
#define MAX_MBSFN_AREA 8

#endif /* VARIABLES_H_ */

typedef enum {normal_txrx=0,rx_calib_ue=1,rx_calib_ue_med=2,rx_calib_ue_byp=3,debug_prach=4,no_L2_connect=5,calib_prach_tx=6,rx_dump_frame=7,loop_through_memory=8} runmode_t;
typedef enum {EXTENDED=1,NORMAL=0} lte_prefix_type_t;
typedef enum {TDD=1,FDD=0} lte_frame_type_t;
typedef int64_t openair0_timestamp;

typedef enum {LOCALIZED=0,DISTRIBUTED=1} vrb_t;
typedef enum {
  SCH_IDLE,
  ACTIVE,
  CBA_ACTIVE,
  DISABLED
} SCH_status_t;

typedef enum {
  /// TM1
  SISO=0,
  /// TM2
  ALAMOUTI=1,
  /// TM3
  LARGE_CDD=2,
  /// the next 6 entries are for TM5
  UNIFORM_PRECODING11=3,
  UNIFORM_PRECODING1m1=4,
  UNIFORM_PRECODING1j=5,
  UNIFORM_PRECODING1mj=6,
  PUSCH_PRECODING0=7,
  PUSCH_PRECODING1=8,
  /// the next 3 entries are for TM4
  DUALSTREAM_UNIFORM_PRECODING1=9,
  DUALSTREAM_UNIFORM_PRECODINGj=10,
  DUALSTREAM_PUSCH_PRECODING=11,
  TM7=12,
  TM8=13,
  TM9_10=14
} MIMO_mode_t;

typedef enum {
  ue_selected,
  wideband_cqi_rank1_2A, //wideband_cqi_rank1_2A,
  wideband_cqi_rank2_2A, //wideband_cqi_rank2_2A,
  HLC_subband_cqi_nopmi, //HLC_subband_cqi_nopmi,
  HLC_subband_cqi_rank1_2A, //HLC_subband_cqi_rank1_2A,
  HLC_subband_cqi_rank2_2A, //HLC_subband_cqi_rank2_2A,
  HLC_subband_cqi_modes123, //HLC_subband_cqi_modes123
  HLC_subband_cqi_mcs_CBA, // MCS and RNTI, for contention-based acces
  unknown_cqi//
} UCI_format_t;

/// Enumeration for parameter PHICH-Duration \ref PHICH_CONFIG_COMMON::phich_duration.
typedef enum {
  normal=0,
  extended=1
} PHICH_DURATION_t;

typedef enum {
  NOT_SYNCHED=0,
  PRACH=1,
  RA_RESPONSE=2,
  PUSCH=3,
  RESYNCH=4
} UE_MODE_t;

/// Enumeration for parameter Ng \ref PHICH_CONFIG_COMMON::phich_resource.
typedef enum {
  oneSixth=1,
  half=3,
  one=6,
  two=12
} PHICH_RESOURCE_t;

/* this structure is used to pass both UE phy vars and
 * proc to the function UE_thread_rxn_txnp4
 */


/// Context data structure for RX/TX portion of subframe processing
typedef struct {
  /// index of the current UE RX/TX proc
  int                  proc_id;
  /// Component Carrier index
  uint8_t              CC_id;
  /// timestamp transmitted to HW
  openair0_timestamp timestamp_tx;
  /// subframe to act upon for transmission
  int subframe_tx;
  /// subframe to act upon for reception
  int subframe_rx;
  /// frame to act upon for transmission
  int frame_tx;
  /// frame to act upon for reception
  int frame_rx;
  /// \brief Instance count for RXn-TXnp4 processing thread.
  /// \internal This variable is protected by \ref mutex_rxtx.
  int instance_cnt_rxtx;
  /// pthread structure for RXn-TXnp4 processing thread
  pthread_t pthread_rxtx;
  /// pthread attributes for RXn-TXnp4 processing thread
  pthread_attr_t attr_rxtx;
  /// condition variable for tx processing thread
  pthread_cond_t cond_rxtx;
  /// mutex for RXn-TXnp4 processing thread
  pthread_mutex_t mutex_rxtx;
  /// scheduling parameters for RXn-TXnp4 thread
  struct sched_param sched_param_rxtx;

  /// internal This variable is protected by ref mutex_fep_slot1.
  //int instance_cnt_slot0_dl_processing;
  int instance_cnt_slot1_dl_processing;
  /// pthread descriptor fep_slot1 thread
  //pthread_t pthread_slot0_dl_processing;
  pthread_t pthread_slot1_dl_processing;
  /// pthread attributes for fep_slot1 processing thread
 // pthread_attr_t attr_slot0_dl_processing;
  pthread_attr_t attr_slot1_dl_processing;
  /// condition variable for UE fep_slot1 thread;
  //pthread_cond_t cond_slot0_dl_processing;
  pthread_cond_t cond_slot1_dl_processing;
  /// mutex for UE synch thread
  //pthread_mutex_t mutex_slot0_dl_processing;
  pthread_mutex_t mutex_slot1_dl_processing;
  //
  uint8_t chan_est_pilot0_slot1_available;
  uint8_t chan_est_slot1_available;
  uint8_t llr_slot1_available;
  uint8_t dci_slot0_available;
  uint8_t first_symbol_available;
  //uint8_t channel_level;
  /// scheduling parameters for fep_slot1 thread
  struct sched_param sched_param_fep_slot1;

  int sub_frame_start;
  int sub_frame_step;
  unsigned long long gotIQs;
} UE_rxtx_proc_t;

typedef struct {
  /// Status Flag indicating for this DLSCH (idle,active,disabled)
  SCH_status_t status;
  /// Transport block size
  uint32_t TBS;
  /// The payload + CRC size in bits, "B" from 36-212
  uint32_t B;
  /// Pointer to the payload
  uint8_t *b;
  /// Pointers to transport block segments
  uint8_t *c[MAX_NUM_DLSCH_SEGMENTS];
  /// RTC values for each segment (for definition see 36-212 V8.6 2009-03, p.15)
  uint32_t RTC[MAX_NUM_DLSCH_SEGMENTS];
  /// Frame where current HARQ round was sent
  uint32_t frame;
  /// Subframe where current HARQ round was sent
  uint32_t subframe;
  /// Index of current HARQ round for this DLSCH
  uint8_t round;
  /// MCS format for this DLSCH
  uint8_t mcs;
  /// Redundancy-version of the current sub-frame
  uint8_t rvidx;
  /// MIMO mode for this DLSCH
  MIMO_mode_t mimo_mode;
  /// Current RB allocation
  uint32_t rb_alloc[4];
  /// distributed/localized flag
  vrb_t vrb_type;
  /// Current subband PMI allocation
  uint16_t pmi_alloc;
  /// Current subband RI allocation
  uint32_t ri_alloc;
  /// Current subband CQI1 allocation
  uint32_t cqi_alloc1;
  /// Current subband CQI2 allocation
  uint32_t cqi_alloc2;
  /// Current Number of RBs
  uint16_t nb_rb;
  /// downlink power offset field
  uint8_t dl_power_off;
  /// Concatenated "e"-sequences (for definition see 36-212 V8.6 2009-03, p.17-18)
  uint8_t e[MAX_NUM_CHANNEL_BITS] __attribute__((aligned(32)));
  /// Turbo-code outputs (36-212 V8.6 2009-03, p.12
  uint8_t *d[MAX_NUM_DLSCH_SEGMENTS];//[(96+3+(3*6144))];
  /// Sub-block interleaver outputs (36-212 V8.6 2009-03, p.16-17)
  uint8_t w[MAX_NUM_DLSCH_SEGMENTS][3*6144];
  /// Number of code segments (for definition see 36-212 V8.6 2009-03, p.9)
  uint32_t C;
  /// Number of "small" code segments (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t Cminus;
  /// Number of "large" code segments (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t Cplus;
  /// Number of bits in "small" code segments (<6144) (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t Kminus;
  /// Number of bits in "large" code segments (<6144) (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t Kplus;
  /// Number of "Filler" bits (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t F;
  /// Number of MIMO layers (streams) (for definition see 36-212 V8.6 2009-03, p.17, TM3-4)
  uint8_t Nl;
  /// Number of layers for this PDSCH transmission (TM8-10)
  uint8_t Nlayers;
  /// First layer for this PSCH transmission
  uint8_t first_layer;
   /// codeword this transport block is mapped to
  uint8_t codeword;
} LTE_DL_eNB_HARQ_t;

typedef struct {
  /// TX buffers for UE-spec transmission (antenna ports 5 or 7..14, prior to precoding)
  int32_t *txdataF[8];
  /// beamforming weights for UE-spec transmission (antenna ports 5 or 7..14), for each codeword, maximum 4 layers?
  int32_t **ue_spec_bf_weights[4];
  /// dl channel estimates (estimated from ul channel estimates)
  int32_t **calib_dl_ch_estimates;
  /// Allocated RNTI (0 means DLSCH_t is not currently used)
  uint16_t rnti;
  /// Active flag for baseband transmitter processing
  uint8_t active;
  /// Indicator of TX activation per subframe.  Used during PUCCH detection for ACK/NAK.
  uint8_t subframe_tx[10];
  /// First CCE of last PDSCH scheduling per subframe.  Again used during PUCCH detection for ACK/NAK.
  uint8_t nCCE[10];
  /// Current HARQ process id
  uint8_t current_harq_pid;
  /// Process ID's per subframe.  Used to associate received ACKs on PUSCH/PUCCH to DLSCH harq process ids
  uint8_t harq_ids[10];
  /// Window size (in outgoing transport blocks) for fine-grain rate adaptation
  uint8_t ra_window_size;
  /// First-round error threshold for fine-grain rate adaptation
  uint8_t error_threshold;
  /// Pointers to 8 HARQ processes for the DLSCH
  LTE_DL_eNB_HARQ_t *harq_processes[8];
  /// Number of soft channel bits
  uint32_t G;
  /// Codebook index for this dlsch (0,1,2,3)
  uint8_t codebook_index;
  /// Maximum number of HARQ processes (for definition see 36-212 V8.6 2009-03, p.17)
  uint8_t Mdlharq;
  /// Maximum number of HARQ rounds
  uint8_t Mlimit;
  /// MIMO transmission mode indicator for this sub-frame (for definition see 36-212 V8.6 2009-03, p.17)
  uint8_t Kmimo;
  /// Nsoft parameter related to UE Category
  uint32_t Nsoft;
  /// amplitude of PDSCH (compared to RS) in symbols without pilots
  int16_t sqrt_rho_a;
  /// amplitude of PDSCH (compared to RS) in symbols containing pilots
  int16_t sqrt_rho_b;

} LTE_eNB_DLSCH_t;

/*! \brief RF mapping */
typedef struct {
  //! card id
  int card;
  //! rf chain id
  int chain;
} openair0_rf_map;

/// Context data structure for eNB subframe processing
typedef struct {
  /// Component Carrier index
  uint8_t              CC_id;
  /// Last RX timestamp
  openair0_timestamp timestamp_rx;
  /// pthread attributes for main UE thread
  pthread_attr_t attr_ue;
  /// scheduling parameters for main UE thread
  struct sched_param sched_param_ue;
  /// pthread descriptor main UE thread
  pthread_t pthread_ue;
  /// \brief Instance count for synch thread.
  /// \internal This variable is protected by \ref mutex_synch.
  int instance_cnt_synch;
  /// pthread attributes for synch processing thread
  pthread_attr_t attr_synch;
  /// scheduling parameters for synch thread
  struct sched_param sched_param_synch;
  /// pthread descriptor synch thread
  pthread_t pthread_synch;
  /// condition variable for UE synch thread;
  pthread_cond_t cond_synch;
  /// mutex for UE synch thread
  pthread_mutex_t mutex_synch;
  /// set of scheduling variables RXn-TXnp4 threads
  UE_rxtx_proc_t proc_rxtx[RX_NB_TH];
} UE_proc_t;

typedef struct {
  //unsigned int   rx_power[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];     //! estimated received signal power (linear)
  //unsigned short rx_power_dB[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];  //! estimated received signal power (dB)
  //unsigned short rx_avg_power_dB[NUMBER_OF_CONNECTED_eNB_MAX];              //! estimated avg received signal power (dB)

  // RRC measurements
  uint32_t rssi;	//This value is in mWatts, should be converted to dbm using 10.log10(rssi)
  int n_adj_cells;
  unsigned int adj_cell_id[6];
  uint32_t rsrq[7];
  uint32_t rsrp[7];
  float rsrp_filtered[7]; // after layer 3 filtering
  float rsrq_filtered[7];
  // common measurements
  //! estimated noise power (linear)
  unsigned int   n0_power[NB_ANTENNAS_RX];
  //! estimated noise power (dB)
  unsigned short n0_power_dB[NB_ANTENNAS_RX];
  //! total estimated noise power (linear)
  unsigned int   n0_power_tot;
  //! total estimated noise power (dB)
  unsigned short n0_power_tot_dB;
  //! average estimated noise power (linear)
  unsigned int   n0_power_avg;
  //! average estimated noise power (dB)
  unsigned short n0_power_avg_dB;
  //! total estimated noise power (dBm)
  short n0_power_tot_dBm;

  // UE measurements
  //! estimated received spatial signal power (linear)
  int            rx_spatial_power[NUMBER_OF_CONNECTED_eNB_MAX][2][2];
  //! estimated received spatial signal power (dB)
  unsigned short rx_spatial_power_dB[NUMBER_OF_CONNECTED_eNB_MAX][2][2];

  /// estimated received signal power (sum over all TX antennas)
  //int            wideband_cqi[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];
  int            rx_power[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];
  /// estimated received signal power (sum over all TX antennas)
  //int            wideband_cqi_dB[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];
  unsigned short rx_power_dB[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];

  /// estimated received signal power (sum over all TX/RX antennas)
  int            rx_power_tot[NUMBER_OF_CONNECTED_eNB_MAX]; //NEW
  /// estimated received signal power (sum over all TX/RX antennas)
  unsigned short rx_power_tot_dB[NUMBER_OF_CONNECTED_eNB_MAX]; //NEW

  //! estimated received signal power (sum of all TX/RX antennas, time average)
  int            rx_power_avg[NUMBER_OF_CONNECTED_eNB_MAX];
  //! estimated received signal power (sum of all TX/RX antennas, time average, in dB)
  unsigned short rx_power_avg_dB[NUMBER_OF_CONNECTED_eNB_MAX];

  /// SINR (sum of all TX/RX antennas, in dB)
  int            wideband_cqi_tot[NUMBER_OF_CONNECTED_eNB_MAX];
  /// SINR (sum of all TX/RX antennas, time average, in dB)
  int            wideband_cqi_avg[NUMBER_OF_CONNECTED_eNB_MAX];

  //! estimated rssi (dBm)
  short          rx_rssi_dBm[NUMBER_OF_CONNECTED_eNB_MAX];
  //! estimated correlation (wideband linear) between spatial channels (computed in dlsch_demodulation)
  int            rx_correlation[NUMBER_OF_CONNECTED_eNB_MAX][2];
  //! estimated correlation (wideband dB) between spatial channels (computed in dlsch_demodulation)
  int            rx_correlation_dB[NUMBER_OF_CONNECTED_eNB_MAX][2];

  /// Wideband CQI (sum of all RX antennas, in dB, for precoded transmission modes (3,4,5,6), up to 4 spatial streams)
  int            precoded_cqi_dB[NUMBER_OF_CONNECTED_eNB_MAX+1][4];
  /// Subband CQI per RX antenna (= SINR)
  int            subband_cqi[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX][NUMBER_OF_SUBBANDS_MAX];
  /// Total Subband CQI  (= SINR)
  int            subband_cqi_tot[NUMBER_OF_CONNECTED_eNB_MAX][NUMBER_OF_SUBBANDS_MAX];
  /// Subband CQI in dB (= SINR dB)
  int            subband_cqi_dB[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX][NUMBER_OF_SUBBANDS_MAX];
  /// Total Subband CQI
  int            subband_cqi_tot_dB[NUMBER_OF_CONNECTED_eNB_MAX][NUMBER_OF_SUBBANDS_MAX];
  /// Wideband PMI for each RX antenna
  int            wideband_pmi_re[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];
  /// Wideband PMI for each RX antenna
  int            wideband_pmi_im[NUMBER_OF_CONNECTED_eNB_MAX][NB_ANTENNAS_RX];
  ///Subband PMI for each RX antenna
  int            subband_pmi_re[NUMBER_OF_CONNECTED_eNB_MAX][NUMBER_OF_SUBBANDS_MAX][NB_ANTENNAS_RX];
  ///Subband PMI for each RX antenna
  int            subband_pmi_im[NUMBER_OF_CONNECTED_eNB_MAX][NUMBER_OF_SUBBANDS_MAX][NB_ANTENNAS_RX];
  /// chosen RX antennas (1=Rx antenna 1, 2=Rx antenna 2, 3=both Rx antennas)
  unsigned char           selected_rx_antennas[NUMBER_OF_CONNECTED_eNB_MAX][NUMBER_OF_SUBBANDS_MAX];
  /// Wideband Rank indication
  unsigned char  rank[NUMBER_OF_CONNECTED_eNB_MAX];
  /// Number of RX Antennas
  unsigned char  nb_antennas_rx;
  /// DLSCH error counter
  // short          dlsch_errors;

} PHY_MEASUREMENTS;

/// PRACH-ConfigInfo from 36.331 RRC spec
typedef struct {
  /// Parameter: prach-ConfigurationIndex, see TS 36.211 (5.7.1). \vr{[0..63]}
  uint8_t prach_ConfigIndex;
  /// Parameter: High-speed-flag, see TS 36.211 (5.7.2). \vr{[0..1]} 1 corresponds to Restricted set and 0 to Unrestricted set.
  uint8_t highSpeedFlag;
  /// Parameter: \f$N_\text{CS}\f$, see TS 36.211 (5.7.2). \vr{[0..15]}\n Refer to table 5.7.2-2 for preamble format 0..3 and to table 5.7.2-3 for preamble format 4.
  uint8_t zeroCorrelationZoneConfig;
  /// Parameter: prach-FrequencyOffset, see TS 36.211 (5.7.1). \vr{[0..94]}\n For TDD the value range is dependent on the value of \ref prach_ConfigIndex.
  uint8_t prach_FreqOffset;
} PRACH_CONFIG_INFO;

/// PRACH-ConfigSIB or PRACH-Config from 36.331 RRC spec
typedef struct {
  /// Parameter: RACH_ROOT_SEQUENCE, see TS 36.211 (5.7.1). \vr{[0..837]}
  uint16_t rootSequenceIndex;
  /// prach_Config_enabled=1 means enabled. \vr{[0..1]}
  uint8_t prach_Config_enabled;
  /// PRACH Configuration Information
  PRACH_CONFIG_INFO prach_ConfigInfo;
} PRACH_CONFIG_COMMON;

/// PUCCH-ConfigCommon from 36.331 RRC spec
typedef struct {
  /// Parameter: \f$\Delta^\text{PUCCH}_\text{shift}\f$, see TS 36.211 (5.4.1). \vr{[1..3]} \note the specification sais it is an enumerated value.
  uint8_t deltaPUCCH_Shift;
  /// Parameter: \f$N^{(2)}_\text{RB}\f$, see TS 36.211 (5.4). \vr{[0..98]}
  uint8_t nRB_CQI;
  /// Parameter: \f$N^{(1)}_\text{CS}\f$, see TS 36.211 (5.4). \vr{[0..7]}
  uint8_t nCS_AN;
  /// Parameter: \f$N^{(1)}_\text{PUCCH}\f$ see TS 36.213 (10.1). \vr{[0..2047]}
  uint16_t n1PUCCH_AN;

  /// group hopping sequence for DRS \note not part of offical UL-PUCCH_CONFIG_COMMON ASN1 specification.
  uint8_t grouphop[20];
  /// sequence hopping sequence for DRS \note not part of offical UL-PUCCH_CONFIG_COMMON ASN1 specification.
  uint8_t seqhop[20];
} PUCCH_CONFIG_COMMON;

/// PDSCH-ConfigCommon from 36.331 RRC spec
typedef struct {
  /// Parameter: Reference-signal power, see TS 36.213 (5.2). \vr{[-60..50]}\n Provides the downlink reference-signal EPRE. The actual value in dBm.
  int8_t referenceSignalPower;
  /// Parameter: \f$P_B\f$, see TS 36.213 (Table 5.2-1). \vr{[0..3]}
  uint8_t p_b;
} PDSCH_CONFIG_COMMON;

typedef enum {
  interSubFrame=0,
  intraAndInterSubFrame=1
} PUSCH_HOPPING_t;

/// UL-ReferenceSignalsPUSCH from 36.331 RRC spec
typedef struct {
  /// Parameter: Group-hopping-enabled, see TS 36.211 (5.5.1.3). \vr{[0..1]}
  uint8_t groupHoppingEnabled;
  /// Parameter: \f$\Delta SS\f$, see TS 36.211 (5.5.1.3). \vr{[0..29]}
  uint8_t groupAssignmentPUSCH;
  /// Parameter: Sequence-hopping-enabled, see TS 36.211 (5.5.1.4). \vr{[0..1]}
  uint8_t sequenceHoppingEnabled;
  /// Parameter: cyclicShift, see TS 36.211 (Table 5.5.2.1.1-2). \vr{[0..7]}
  uint8_t cyclicShift;
  /// nPRS for cyclic shift of DRS \note not part of offical UL-ReferenceSignalsPUSCH ASN1 specification.
  uint8_t nPRS[20];
  /// group hopping sequence for DRS \note not part of offical UL-ReferenceSignalsPUSCH ASN1 specification.
  uint8_t grouphop[20];
  /// sequence hopping sequence for DRS \note not part of offical UL-ReferenceSignalsPUSCH ASN1 specification.
  uint8_t seqhop[20];
} UL_REFERENCE_SIGNALS_PUSCH_t;

/// PUSCH-ConfigCommon from 36.331 RRC spec.
typedef struct {
  /// Parameter: \f$N_{sb}\f$, see TS 36.211 (5.3.4). \vr{[1..4]}
  uint8_t n_SB;
  /// Parameter: Hopping-mode, see TS 36.211 (5.3.4).
  PUSCH_HOPPING_t hoppingMode;
  /// Parameter: \f$N^{HO}_{RB}\f$, see TS 36.211 (5.3.4). \vr{[0..98]}
  uint8_t pusch_HoppingOffset;
  /// See TS 36.213 (8.6.1). \vr{[0..1]} 1 indicates 64QAM is allowed, 0 not allowed.
  uint8_t enable64QAM;
  /// Ref signals configuration
  UL_REFERENCE_SIGNALS_PUSCH_t ul_ReferenceSignalsPUSCH;
} PUSCH_CONFIG_COMMON;

typedef struct {
  /// Parameter: PHICH-Duration, see TS 36.211 (Table 6.9.3-1).
  PHICH_DURATION_t phich_duration;
  /// Parameter: Ng, see TS 36.211 (6.9). \details Value oneSixth corresponds to 1/6, half corresponds to 1/2 and so on.
  PHICH_RESOURCE_t phich_resource;
} PHICH_CONFIG_COMMON;
/// SoundingRS-UL-ConfigCommon Information Element from 36.331 RRC spec
typedef struct {
  /// enabled flag=1 means SRS is enabled. \vr{[0..1]}
  uint8_t enabled_flag;
  /// Parameter: SRS Bandwidth Configuration, see TS 36.211 (table 5.5.3.2-1, 5.5.3.2-2, 5.5.3.2-3 and 5.5.3.2-4). \vr{[0..7]}\n Actual configuration depends on UL bandwidth. \note the specification sais it is an enumerated value.
  uint8_t srs_BandwidthConfig;
  /// Parameter: SRS SubframeConfiguration, see TS 36.211 (table 5.5.3.3-1 for FDD, table 5.5.3.3-2 for TDD). \vr{[0..15]} \note the specification sais it is an enumerated value.
  uint8_t srs_SubframeConfig;
  /// Parameter: Simultaneous-AN-and-SRS, see TS 36.213 (8.2). \vr{[0..1]}
  uint8_t ackNackSRS_SimultaneousTransmission;
  /// Parameter: srsMaxUpPts, see TS 36.211 (5.5.3.2). \details If this field is present, reconfiguration of \f$m^\text{max}_\text{SRS,0}\f$ applies for UpPts, otherwise reconfiguration does not apply.
  uint8_t srs_MaxUpPts;
} SOUNDINGRS_UL_CONFIG_COMMON;

/// Enumeration for parameter \f$\alpha\f$ \ref UL_POWER_CONTROL_CONFIG_COMMON::alpha.
typedef enum {
  al0=0,
  al04=1,
  al05=2,
  al06=3,
  al07=4,
  al08=5,
  al09=6,
  al1=7
} PUSCH_alpha_t;

/// UplinkPowerControlCommon Information Element from 36.331 RRC spec \note this structure does not currently make use of \ref deltaFList_PUCCH_t.
typedef struct {
  /// Parameter: \f$P_\text{0\_NOMINAL\_PUSCH}(1)\f$, see TS 36.213 (5.1.1.1), unit dBm. \vr{[-126..24]}\n This field is applicable for non-persistent scheduling, only.
  int8_t p0_NominalPUSCH;
  /// Parameter: \f$\alpha\f$, see TS 36.213 (5.1.1.1) \warning the enumeration values do not correspond to the given values in the specification (al04 should be 0.4, ...)!
  PUSCH_alpha_t alpha;
  /// Parameter: \f$P_\text{0\_NOMINAL\_PUCCH}\f$ See TS 36.213 (5.1.2.1), unit dBm. \vr{[-127..-96]}
  int8_t p0_NominalPUCCH;
  /// Parameter: \f$\Delta_\text{PREAMBLE\_Msg3}\f$ see TS 36.213 (5.1.1.1). \vr{[-1..6]}\n Actual value = IE value * 2 [dB].
  int8_t deltaPreambleMsg3;
  /// Parameter: \f$\Delta_\text{F\_PUCCH}(F)\f$ for the PUCCH format 1, see TS 36.213 (5.1.2). \vr{[0..2]} \warning check value range, why is this a long? \note the specification sais it is an enumerated value.
  long deltaF_PUCCH_Format1;
  /// Parameter: \f$\Delta_\text{F\_PUCCH}(F)\f$ for the PUCCH format 1a, see TS 36.213 (5.1.2). \vr{[0..2]} \warning check value range, why is this a long? \note the specification sais it is an enumerated value.
  long deltaF_PUCCH_Format1a;
  /// Parameter: \f$\Delta_\text{F\_PUCCH}(F)\f$ for the PUCCH format 1b, see TS 36.213 (5.1.2). \vr{[0..2]} \warning check value range, why is this a long? \note the specification sais it is an enumerated value.
  long deltaF_PUCCH_Format1b;
  /// Parameter: \f$\Delta_\text{F\_PUCCH}(F)\f$ for the PUCCH format 2, see TS 36.213 (5.1.2). \vr{[0..3]} \warning check value range, why is this a long? \note the specification sais it is an enumerated value.
  long deltaF_PUCCH_Format2;
  /// Parameter: \f$\Delta_\text{F\_PUCCH}(F)\f$ for the PUCCH format 2a, see TS 36.213 (5.1.2). \vr{[0..2]} \warning check value range, why is this a long? \note the specification sais it is an enumerated value.
  long deltaF_PUCCH_Format2a;
  /// Parameter: \f$\Delta_\text{F\_PUCCH}(F)\f$ for the PUCCH format 2b, see TS 36.213 (5.1.2). \vr{[0..2]} \warning check value range, why is this a long? \note the specification sais it is an enumerated value.
  long deltaF_PUCCH_Format2b;
} UL_POWER_CONTROL_CONFIG_COMMON;

/// MBSFN-SubframeConfig Information Element from 36.331 RRC spec \note deviates from specification.
typedef struct {
  /// MBSFN subframe occurance. \details Radio-frames that contain MBSFN subframes occur when equation SFN mod radioFrameAllocationPeriod = radioFrameAllocationOffset is satisfied. When fourFrames is used for subframeAllocation, the equation defines the first radio frame referred to in the description below. Values n1 and n2 are not applicable when fourFrames is used. \note the specification sais it is an enumerated value {n1, n2, n4, n8, n16, n32}.
  int radioframeAllocationPeriod;
  /// MBSFN subframe occurance. \vr{[0..7]}\n Radio-frames that contain MBSFN subframes occur when equation SFN mod radioFrameAllocationPeriod = radioFrameAllocationOffset is satisfied. When fourFrames is used for subframeAllocation, the equation defines the first radio frame referred to in the description below. Values n1 and n2 are not applicable when fourFrames is used.
  int radioframeAllocationOffset;
  /// oneFrame or fourFrames. \vr{[0..1]}
  int fourFrames_flag;
  /// Subframe configuration. \vr{[0..63]} (\ref fourFrames_flag == 0) or \vr{[0..16777215]} (\ref fourFrames_flag == 1)
  /// \par fourFrames_flag == 0
  /// "1" denotes that the corresponding subframe is allocated for MBSFN. The following mapping applies:\n FDD: The first/leftmost bit defines the MBSFN allocation for subframe #1, the second bit for #2, third bit for #3 , fourth bit for #6, fifth bit for #7, sixth bit for #8.\n TDD: The first/leftmost bit defines the allocation for subframe #3, the second bit for #4, third bit for #7, fourth bit for #8, fifth bit for #9. Uplink subframes are not allocated. The last bit is not used.
  /// \par fourFrames_flag == 1
  /// A bit-map indicating MBSFN subframe allocation in four consecutive radio frames, "1" denotes that the corresponding subframe is allocated for MBSFN. The bitmap is interpreted as follows:\n FDD: Starting from the first radioframe and from the first/leftmost bit in the bitmap, the allocation applies to subframes #1, #2, #3 , #6, #7, and #8 in the sequence of the four radio-frames.\n TDD: Starting from the first radioframe and from the first/leftmost bit in the bitmap, the allocation applies to subframes #3, #4, #7, #8, and #9 in the sequence of the four radio-frames. The last four bits are not used. Uplink subframes are not allocated.
  int mbsfn_SubframeConfig;
} MBSFN_config_t;

typedef struct {
  /// Number of resource blocks (RB) in DL
  uint8_t N_RB_DL;
  /// Number of resource blocks (RB) in UL
  uint8_t N_RB_UL;
  ///  total Number of Resource Block Groups: this is ceil(N_PRB/P)
  uint8_t N_RBG;
  /// Total Number of Resource Block Groups SubSets: this is P
  uint8_t N_RBGS;
  /// Cell ID
  uint16_t Nid_cell;
  /// MBSFN Area ID
  uint16_t Nid_cell_mbsfn;
  /// Cyclic Prefix for DL (0=Normal CP, 1=Extended CP)
  lte_prefix_type_t Ncp;
  /// Cyclic Prefix for UL (0=Normal CP, 1=Extended CP)
  lte_prefix_type_t Ncp_UL;
  /// shift of pilot position in one RB
  uint8_t nushift;
  /// Frame type (0 FDD, 1 TDD)
  lte_frame_type_t frame_type;
  /// TDD subframe assignment (0-7) (default = 3) (254=RX only, 255=TX only)
  uint8_t tdd_config;
  /// TDD S-subframe configuration (0-9)
  uint8_t tdd_config_S;
  /// srs extra symbol flag for TDD
  uint8_t srsX;
  /// indicates if node is a UE (NODE=2) or eNB (PRIMARY_CH=0).
  uint8_t node_id;
  /// Frequency index of CBMIMO1 card
  uint8_t freq_idx;
  /// RX Frequency for ExpressMIMO/LIME
  uint32_t carrier_freq[4];
  /// TX Frequency for ExpressMIMO/LIME
  uint32_t carrier_freqtx[4];
  /// RX gain for ExpressMIMO/LIME
  uint32_t rxgain[4];
  /// TX gain for ExpressMIMO/LIME
  uint32_t txgain[4];
  /// RF mode for ExpressMIMO/LIME
  uint32_t rfmode[4];
  /// RF RX DC Calibration for ExpressMIMO/LIME
  uint32_t rxdc[4];
  /// RF TX DC Calibration for ExpressMIMO/LIME
  uint32_t rflocal[4];
  /// RF VCO calibration for ExpressMIMO/LIME
  uint32_t rfvcolocal[4];
  /// Turns on second TX of CBMIMO1 card
  uint8_t dual_tx;
  /// flag to indicate SISO transmission
  uint8_t mode1_flag;
  /// Indicator that 20 MHz channel uses 3/4 sampling frequency
  uint8_t threequarter_fs;
  /// Size of FFT
  uint16_t ofdm_symbol_size;
  /// Number of prefix samples in all but first symbol of slot
  uint16_t nb_prefix_samples;
  /// Number of prefix samples in first symbol of slot
  uint16_t nb_prefix_samples0;
  /// Carrier offset in FFT buffer for first RE in PRB0
  uint16_t first_carrier_offset;
  /// Number of samples in a subframe
  uint32_t samples_per_tti;
  /// Number of OFDM/SC-FDMA symbols in one subframe (to be modified to account for potential different in UL/DL)
  uint16_t symbols_per_tti;
  /// Number of OFDM symbols in DL portion of S-subframe
  uint16_t dl_symbols_in_S_subframe;
  /// Number of SC-FDMA symbols in UL portion of S-subframe
  uint16_t ul_symbols_in_S_subframe;
  /// Number of Physical transmit antennas in node
  uint8_t nb_antennas_tx;
  /// Number of Receive antennas in node
  uint8_t nb_antennas_rx;
  /// Number of common transmit antenna ports in eNodeB (1 or 2)
  uint8_t nb_antenna_ports_eNB;
  /// PRACH_CONFIG
  PRACH_CONFIG_COMMON prach_config_common;
  /// PUCCH Config Common (from 36-331 RRC spec)
  PUCCH_CONFIG_COMMON pucch_config_common;
  /// PDSCH Config Common (from 36-331 RRC spec)
  PDSCH_CONFIG_COMMON pdsch_config_common;
  /// PUSCH Config Common (from 36-331 RRC spec)
  PUSCH_CONFIG_COMMON pusch_config_common;
  /// PHICH Config (from 36-331 RRC spec)
  PHICH_CONFIG_COMMON phich_config_common;
  /// SRS Config (from 36-331 RRC spec)
  SOUNDINGRS_UL_CONFIG_COMMON soundingrs_ul_config_common;
  /// UL Power Control (from 36-331 RRC spec)
  UL_POWER_CONTROL_CONFIG_COMMON ul_power_control_config_common;
  /// Number of MBSFN Configurations
  int num_MBSFN_config;
  /// Array of MBSFN Configurations (max 8 (maxMBSFN-Allocations) elements as per 36.331)
  MBSFN_config_t MBSFN_config[8];
  /// Maximum Number of Retransmissions of RRCConnectionRequest (from 36-331 RRC Spec)
  uint8_t maxHARQ_Msg3Tx;
  /// Size of SI windows used for repetition of one SI message (in frames)
  uint8_t SIwindowsize;
  /// Period of SI windows used for repetition of one SI message (in frames)
  uint16_t SIPeriod;
  /// REGs assigned to PCFICH
  uint16_t pcfich_reg[4];
  /// Index of first REG assigned to PCFICH
  uint8_t pcfich_first_reg_idx;
  /// REGs assigned to PHICH
  uint16_t phich_reg[MAX_NUM_PHICH_GROUPS][3];

  struct MBSFN_SubframeConfig *mbsfn_SubframeConfig[MAX_MBSFN_AREA];

} LTE_DL_FRAME_PARMS;

typedef struct {

	  /// \brief Holds the received data in the frequency domain.
	  /// - first index: rx antenna [0..nb_antennas_rx[
	  /// - second index: symbol [0..28*ofdm_symbol_size[
	  int32_t **rxdataF;

	  /// \brief Hold the channel estimates in frequency domain.
	  /// - first index: eNB id [0..6] (hard coded)
	  /// - second index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
	  /// - third index: samples? [0..symbols_per_tti*(ofdm_symbol_size+LTE_CE_FILTER_LENGTH)[
	  int32_t **dl_ch_estimates[7];

	  /// \brief Hold the channel estimates in time domain (used for tracking).
	  /// - first index: eNB id [0..6] (hard coded)
	  /// - second index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
	  /// - third index: samples? [0..2*ofdm_symbol_size[
	  int32_t **dl_ch_estimates_time[7];
}LTE_UE_COMMON_PER_THREAD;

typedef struct {
  /// \brief Holds the transmit data in time domain.
  /// For IFFT_FPGA this points to the same memory as PHY_vars->tx_vars[a].TX_DMA_BUFFER.
  /// - first index: tx antenna [0..nb_antennas_tx[
  /// - second index: sample [0..FRAME_LENGTH_COMPLEX_SAMPLES[
  int32_t **txdata;
  /// \brief Holds the transmit data in the frequency domain.
  /// For IFFT_FPGA this points to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER.
  /// - first index: tx antenna [0..nb_antennas_tx[
  /// - second index: sample [0..FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX[
  int32_t **txdataF;

  /// \brief Holds the received data in time domain.
  /// Should point to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER.
  /// - first index: rx antenna [0..nb_antennas_rx[
  /// - second index: sample [0..FRAME_LENGTH_COMPLEX_SAMPLES+2048[
  int32_t **rxdata;

  LTE_UE_COMMON_PER_THREAD common_vars_rx_data_per_thread[RX_NB_TH_MAX];

  /// holds output of the sync correlator
  int32_t *sync_corr;
  /// estimated frequency offset (in radians) for all subcarriers
  int32_t freq_offset;
  /// eNb_id user is synched to
  int32_t eNb_id;
} LTE_UE_COMMON;

typedef struct {
  /// \brief Received frequency-domain signal after extraction.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **rxdataF_ext;
  /// \brief Received frequency-domain ue specific pilots.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..12*N_RB_DL[
  int32_t **rxdataF_uespec_pilots;
  /// \brief Received frequency-domain signal after extraction and channel compensation.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **rxdataF_comp0;
  /// \brief Received frequency-domain signal after extraction and channel compensation for the second stream. For the SIC receiver we need to store the history of this for each harq process and round
  /// - first index: ? [0..7] (hard coded) accessed via \c harq_pid
  /// - second index: ? [0..7] (hard coded) accessed via \c round
  /// - third index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - fourth index: ? [0..168*N_RB_DL[
  int32_t **rxdataF_comp1[8][8];
  /// \brief Downlink channel estimates extracted in PRBS.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_estimates_ext;
  /// \brief Downlink cross-correlation of MIMO channel estimates (unquantized PMI) extracted in PRBS. For the SIC receiver we need to store the history of this for each harq process and round
  /// - first index: ? [0..7] (hard coded) accessed via \c harq_pid
  /// - second index: ? [0..7] (hard coded) accessed via \c round
  /// - third index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - fourth index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_rho_ext[8][8];
  /// \brief Downlink beamforming channel estimates in frequency domain.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: samples? [0..symbols_per_tti*(ofdm_symbol_size+LTE_CE_FILTER_LENGTH)[
  int32_t **dl_bf_ch_estimates;
  /// \brief Downlink beamforming channel estimates.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_bf_ch_estimates_ext;
  /// \brief Downlink cross-correlation of MIMO channel estimates (unquantized PMI) extracted in PRBS.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_rho2_ext;
  /// \brief Downlink PMIs extracted in PRBS and grouped in subbands.
  /// - first index: ressource block [0..N_RB_DL[
  uint8_t *pmi_ext;
  /// \brief Magnitude of Downlink Channel first layer (16QAM level/First 64QAM level).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_mag0;
  /// \brief Magnitude of Downlink Channel second layer (16QAM level/First 64QAM level).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_mag1[8][8];
  /// \brief Magnitude of Downlink Channel, first layer (2nd 64QAM level).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_magb0;
  /// \brief Magnitude of Downlink Channel second layer (2nd 64QAM level).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_magb1[8][8];
  /// \brief Cross-correlation of two eNB signals.
  /// - first index: rx antenna [0..nb_antennas_rx[
  /// - second index: symbol [0..]
  int32_t **rho;
  /// never used... always send dl_ch_rho_ext instead...
  int32_t **rho_i;
  /// \brief Pointers to llr vectors (2 TBs).
  /// - first index: ? [0..1] (hard coded)
  /// - second index: ? [0..1179743] (hard coded)
  int16_t *llr[2];
  /// \f$\log_2(\max|H_i|^2)\f$
  int16_t log2_maxh;
    /// \f$\log_2(\max|H_i|^2)\f$ //this is for TM3-4 layer1 channel compensation
  int16_t log2_maxh0;
    /// \f$\log_2(\max|H_i|^2)\f$ //this is for TM3-4 layer2 channel commpensation
  int16_t log2_maxh1;
  /// \brief LLR shifts for subband scaling.
  /// - first index: ? [0..168*N_RB_DL[
  uint8_t *llr_shifts;
  /// \brief Pointer to LLR shifts.
  /// - first index: ? [0..168*N_RB_DL[
  uint8_t *llr_shifts_p;
  /// \brief Pointers to llr vectors (128-bit alignment).
  /// - first index: ? [0..0] (hard coded)
  /// - second index: ? [0..]
  int16_t **llr128;
  /// \brief Pointers to llr vectors (128-bit alignment).
  /// - first index: ? [0..0] (hard coded)
  /// - second index: ? [0..]
  int16_t **llr128_2ndstream;
  //uint32_t *rb_alloc;
  //uint8_t Qm[2];
  //MIMO_mode_t mimo_mode;
  // llr offset per ofdm symbol
  uint32_t llr_offset[14];
  // llr length per ofdm symbol
  uint32_t llr_length[14];
} LTE_UE_PDSCH;

typedef struct {
  /// \brief Received frequency-domain signal after extraction.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..]
  int32_t **rxdataF_ext;
  /// \brief Received frequency-domain signal after extraction and channel compensation.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..]
  double **rxdataF_comp;
  /// \brief Downlink channel estimates extracted in PRBS.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..]
  int32_t **dl_ch_estimates_ext;
  ///  \brief Downlink cross-correlation of MIMO channel estimates (unquantized PMI) extracted in PRBS.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..]
  double **dl_ch_rho_ext;
  /// \brief Downlink PMIs extracted in PRBS and grouped in subbands.
  /// - first index: ressource block [0..N_RB_DL[
  uint8_t *pmi_ext;
  /// \brief Magnitude of Downlink Channel (16QAM level/First 64QAM level).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..]
  double **dl_ch_mag;
  /// \brief Magnitude of Downlink Channel (2nd 64QAM level).
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..]
  double **dl_ch_magb;
  /// \brief Cross-correlation of two eNB signals.
  /// - first index: rx antenna [0..nb_antennas_rx[
  /// - second index: ? [0..]
  double **rho;
  /// never used... always send dl_ch_rho_ext instead...
  double **rho_i;
  /// \brief Pointers to llr vectors (2 TBs).
  /// - first index: ? [0..1] (hard coded)
  /// - second index: ? [0..1179743] (hard coded)
  int16_t *llr[2];
  /// \f$\log_2(\max|H_i|^2)\f$
  uint8_t log2_maxh;
  /// \brief Pointers to llr vectors (128-bit alignment).
  /// - first index: ? [0..0] (hard coded)
  /// - second index: ? [0..]
  int16_t **llr128;
  //uint32_t *rb_alloc;
  //uint8_t Qm[2];
  //MIMO_mode_t mimo_mode;
} LTE_UE_PDSCH_FLP;

typedef struct {
  /// \brief Pointers to extracted PBCH symbols in frequency-domain.
  /// - first index: rx antenna [0..nb_antennas_rx[
  /// - second index: ? [0..287] (hard coded)
  int32_t **rxdataF_ext;
  /// \brief Pointers to extracted and compensated PBCH symbols in frequency-domain.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..287] (hard coded)
  int32_t **rxdataF_comp;
  /// \brief Pointers to downlink channel estimates in frequency-domain extracted in PRBS.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..287] (hard coded)
  int32_t **dl_ch_estimates_ext;
  /// \brief Pointer to PBCH llrs.
  /// - first index: ? [0..1919] (hard coded)
  int8_t *llr;
  /// \brief Pointer to PBCH decoded output.
  /// - first index: ? [0..63] (hard coded)
  uint8_t *decoded_output;
  /// \brief Total number of PDU errors.
  uint32_t pdu_errors;
  /// \brief Total number of PDU errors 128 frames ago.
  uint32_t pdu_errors_last;
  /// \brief Total number of consecutive PDU errors.
  uint32_t pdu_errors_conseq;
  /// \brief FER (in percent) .
  uint32_t pdu_fer;
} LTE_UE_PBCH;

typedef struct {
  /// \brief Pointers to extracted PDCCH symbols in frequency-domain.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **rxdataF_ext;
  /// \brief Pointers to extracted and compensated PDCCH symbols in frequency-domain.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **rxdataF_comp;
  /// \brief Pointers to extracted channel estimates of PDCCH symbols.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_estimates_ext;
  /// \brief Pointers to channel cross-correlation vectors for multi-eNB detection.
  /// - first index: ? [0..7] (hard coded) FIXME! accessed via \c nb_antennas_rx
  /// - second index: ? [0..168*N_RB_DL[
  int32_t **dl_ch_rho_ext;
  /// \brief Pointers to channel cross-correlation vectors for multi-eNB detection.
  /// - first index: rx antenna [0..nb_antennas_rx[
  /// - second index: ? [0..]
  int32_t **rho;
  /// \brief Pointer to llrs, 4-bit resolution.
  /// - first index: ? [0..48*N_RB_DL[
  uint16_t *llr;
  /// \brief Pointer to llrs, 16-bit resolution.
  /// - first index: ? [0..96*N_RB_DL[
  uint16_t *llr16;
  /// \brief \f$\overline{w}\f$ from 36-211.
  /// - first index: ? [0..48*N_RB_DL[
  uint16_t *wbar;
  /// \brief PDCCH/DCI e-sequence (input to rate matching).
  /// - first index: ? [0..96*N_RB_DL[
  int8_t *e_rx;
  /// number of PDCCH symbols in current subframe
  uint8_t num_pdcch_symbols;
  /// Allocated CRNTI for UE
  uint16_t crnti;
  /// 1: the allocated crnti is Temporary C-RNTI / 0: otherwise
  uint8_t crnti_is_temporary;
  /// Total number of PDU errors (diagnostic mode)
  uint32_t dci_errors;
  /// Total number of PDU received
  uint32_t dci_received;
  /// Total number of DCI False detection (diagnostic mode)
  uint32_t dci_false;
  /// Total number of DCI missed (diagnostic mode)
  uint32_t dci_missed;
  /// nCCE for PUCCH per subframe
  uint8_t nCCE[10];
  //Check for specific DCIFormat and AgregationLevel
  uint8_t dciFormat;
  uint8_t agregationLevel;
} LTE_UE_PDCCH;

typedef struct {
  int16_t amp;
  int16_t *prachF;
  int16_t *prach;
} LTE_UE_PRACH;

typedef struct {
  /// HARQ process id
  uint8_t harq_id;
  /// ACK bits (after decoding) 0:NACK / 1:ACK / 2:DTX
  uint8_t ack;
  /// send status (for PUCCH)
  uint8_t send_harq_status;
  /// nCCE (for PUCCH)
  uint8_t nCCE;
  /// DAI value detected from DCI1/1a/1b/1d/2/2a/2b/2c. 0xff indicates not touched
  uint8_t vDAI_DL;
  /// DAI value detected from DCI0/4. 0xff indicates not touched
  uint8_t vDAI_UL;
} harq_status_t;

typedef struct {
  /// Indicator of first transmission
  uint8_t first_tx;
  /// Last Ndi received for this process on DCI (used for C-RNTI only)
  uint8_t DCINdi;
  /// DLSCH status flag indicating
  SCH_status_t status;
  /// Transport block size
  uint32_t TBS;
  /// The payload + CRC size in bits
  uint32_t B;
  /// Pointer to the payload
  uint8_t *b;
  /// Pointers to transport block segments
  uint8_t *c[MAX_NUM_DLSCH_SEGMENTS];
  /// RTC values for each segment (for definition see 36-212 V8.6 2009-03, p.15)
  uint32_t RTC[MAX_NUM_DLSCH_SEGMENTS];
  /// Index of current HARQ round for this DLSCH
  uint8_t round;
  /// MCS format for this DLSCH
  uint8_t mcs;
  /// Qm (modulation order) for this DLSCH
  uint8_t Qm;
  /// Redundancy-version of the current sub-frame
  uint8_t rvidx;
  /// MIMO mode for this DLSCH
  MIMO_mode_t mimo_mode;
  /// soft bits for each received segment ("w"-sequence)(for definition see 36-212 V8.6 2009-03, p.15)
  int16_t w[MAX_NUM_DLSCH_SEGMENTS][3*(6144+64)];
  /// for abstraction soft bits for each received segment ("w"-sequence)(for definition see 36-212 V8.6 2009-03, p.15)
  double w_abs[MAX_NUM_DLSCH_SEGMENTS][3*(6144+64)];
  /// soft bits for each received segment ("d"-sequence)(for definition see 36-212 V8.6 2009-03, p.15)
  int16_t *d[MAX_NUM_DLSCH_SEGMENTS];
  /// Number of code segments (for definition see 36-212 V8.6 2009-03, p.9)
  uint32_t C;
  /// Number of "small" code segments (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t Cminus;
  /// Number of "large" code segments (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t Cplus;
  /// Number of bits in "small" code segments (<6144) (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t Kminus;
  /// Number of bits in "large" code segments (<6144) (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t Kplus;
  /// Number of "Filler" bits (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t F;
  /// Number of MIMO layers (streams) (for definition see 36-212 V8.6 2009-03, p.17)
  uint8_t Nl;
  /// current delta_pucch
  int8_t delta_PUCCH;
  /// Number of soft channel bits
  uint32_t G;
  /// Current Number of RBs
  uint16_t nb_rb;
  /// Current subband PMI allocation
  uint16_t pmi_alloc;
  /// Current RB allocation (even slots)
  uint32_t rb_alloc_even[4];
  /// Current RB allocation (odd slots)
  uint32_t rb_alloc_odd[4];
  /// distributed/localized flag
  vrb_t vrb_type;
  /// downlink power offset field
  uint8_t dl_power_off;
  /// trials per round statistics
  uint32_t trials[8];
  /// error statistics per round
  uint32_t errors[8];
  /// codeword this transport block is mapped to
  uint8_t codeword;
} LTE_DL_UE_HARQ_t;

typedef struct {
  /// RNTI
  uint16_t rnti;
  /// Active flag for DLSCH demodulation
  uint8_t active;
  /// Transmission mode
  uint8_t mode1_flag;
  /// amplitude of PDSCH (compared to RS) in symbols without pilots
  int16_t sqrt_rho_a;
  /// amplitude of PDSCH (compared to RS) in symbols containing pilots
  int16_t sqrt_rho_b;
  /// Current HARQ process id threadRx Odd and threadRx Even
  uint8_t current_harq_pid;
  /// Current subband antenna selection
  uint32_t antenna_alloc;
  /// Current subband RI allocation
  uint32_t ri_alloc;
  /// Current subband CQI1 allocation
  uint32_t cqi_alloc1;
  /// Current subband CQI2 allocation
  uint32_t cqi_alloc2;
  /// saved subband PMI allocation from last PUSCH/PUCCH report
  uint16_t pmi_alloc;
  /// HARQ-ACKs
  harq_status_t harq_ack[10];
  /// Pointers to up to 8 HARQ processes
  LTE_DL_UE_HARQ_t *harq_processes[8];
  /// Maximum number of HARQ processes(for definition see 36-212 V8.6 2009-03, p.17
  uint8_t Mdlharq;
  /// MIMO transmission mode indicator for this sub-frame (for definition see 36-212 V8.6 2009-03, p.17)
  uint8_t Kmimo;
  /// Nsoft parameter related to UE Category
  uint32_t Nsoft;
  /// Maximum number of Turbo iterations
  uint8_t max_turbo_iterations;
  /// number of iterations used in last turbo decoding
  uint8_t last_iteration_cnt;
  /// accumulated tx power adjustment for PUCCH
  int8_t               g_pucch;
} LTE_UE_DLSCH_t;

typedef struct {
  /// Indicator of first transmission
  uint8_t first_tx;
  /// Last Ndi received for this process on DCI (used for C-RNTI only)
  uint8_t DCINdi;
  /// Flag indicating that this ULSCH has a new packet (start of new round)
  //  uint8_t Ndi;
  /// Status Flag indicating for this ULSCH (idle,active,disabled)
  SCH_status_t status;
  /// Subframe scheduling indicator (i.e. Transmission opportunity indicator)
  uint8_t subframe_scheduling_flag;
  /// Subframe cba scheduling indicator (i.e. Transmission opportunity indicator)
  uint8_t subframe_cba_scheduling_flag;
  /// First Allocated RB
  uint16_t first_rb;
  /// Current Number of RBs
  uint16_t nb_rb;
  /// Last TPC command
  uint8_t TPC;
  /// Transport block size
  uint32_t TBS;
  /// The payload + CRC size in bits, "B" from 36-212
  uint32_t B;
  /// Length of ACK information (bits)
  uint8_t O_ACK;
  /// Pointer to the payload
  uint8_t *b;
  /// Pointers to transport block segments
  uint8_t *c[MAX_NUM_ULSCH_SEGMENTS];
  /// RTC values for each segment (for definition see 36-212 V8.6 2009-03, p.15)
  uint32_t RTC[MAX_NUM_ULSCH_SEGMENTS];
  /// Index of current HARQ round for this ULSCH
  uint8_t round;
  /// MCS format of this ULSCH
  uint8_t mcs;
  /// Redundancy-version of the current sub-frame
  uint8_t rvidx;
  /// Turbo-code outputs (36-212 V8.6 2009-03, p.12
  uint8_t d[MAX_NUM_ULSCH_SEGMENTS][(96+3+(3*6144))];
  /// Sub-block interleaver outputs (36-212 V8.6 2009-03, p.16-17)
  uint8_t w[MAX_NUM_ULSCH_SEGMENTS][3*6144];
  /// Number of code segments (for definition see 36-212 V8.6 2009-03, p.9)
  uint32_t C;
  /// Number of "small" code segments (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t Cminus;
  /// Number of "large" code segments (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t Cplus;
  /// Number of bits in "small" code segments (<6144) (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t Kminus;
  /// Number of bits in "large" code segments (<6144) (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t Kplus;
  /// Total number of bits across all segments
  uint32_t sumKr;
  /// Number of "Filler" bits (for definition see 36-212 V8.6 2009-03, p.10)
  uint32_t F;
  /// Msc_initial, Initial number of subcarriers for ULSCH (36-212, v8.6 2009-03, p.26-27)
  uint16_t Msc_initial;
  /// Nsymb_initial, Initial number of symbols for ULSCH (36-212, v8.6 2009-03, p.26-27)
  uint8_t Nsymb_initial;
  /// n_DMRS  for cyclic shift of DMRS (36.213 Table 9.1.2-2)
  uint8_t n_DMRS;
  /// n_DMRS2 for cyclic shift of DMRS (36.211 Table 5.5.1.1.-1)
  uint8_t n_DMRS2;
  /// Flag to indicate that this is a control only ULSCH (i.e. no MAC SDU)
  uint8_t control_only;
  /// Flag to indicate that this is a calibration ULSCH (i.e. no MAC SDU and filled with TDD calibration information)
  //  int calibration_flag;
  /// Number of soft channel bits
  uint32_t G;

  // decode phich
  uint8_t decode_phich;
} LTE_UL_UE_HARQ_t;

typedef struct {
  /// Current Number of Symbols
  uint8_t Nsymb_pusch;
  /// SRS active flag
  uint8_t srs_active;
  /// Pointers to 8 HARQ processes for the ULSCH
  LTE_UL_UE_HARQ_t *harq_processes[8];
  /// Pointer to CQI data
  uint8_t o[MAX_CQI_BYTES];
  /// Length of CQI data (bits)
  uint8_t O;
  /// Format of CQI data
  UCI_format_t uci_format;
  /// Rank information
  uint8_t o_RI[2];
  /// Length of rank information (bits)
  uint8_t O_RI;
  /// Pointer to ACK
  uint8_t o_ACK[4];
  /// Minimum number of CQI bits for PUSCH (36-212 r8.6, Sec 5.2.4.1 p. 37)
  uint8_t O_CQI_MIN;
  /// ACK/NAK Bundling flag
  uint8_t bundling;
  /// Concatenated "e"-sequences (for definition see 36-212 V8.6 2009-03, p.17-18)
  uint8_t e[MAX_NUM_CHANNEL_BITS];
  /// Interleaved "h"-sequences (for definition see 36-212 V8.6 2009-03, p.17-18)
  uint8_t h[MAX_NUM_CHANNEL_BITS];
  /// Scrambled "b"-sequences (for definition see 36-211 V8.6 2009-03, p.14)
  uint8_t b_tilde[MAX_NUM_CHANNEL_BITS];
  /// Modulated "d"-sequences (for definition see 36-211 V8.6 2009-03, p.14)
  int32_t d[MAX_NUM_RE];
  /// Transform-coded "z"-sequences (for definition see 36-211 V8.6 2009-03, p.14-15)
  int32_t z[MAX_NUM_RE];
  /// "q" sequences for CQI/PMI (for definition see 36-212 V8.6 2009-03, p.27)
  uint8_t q[MAX_CQI_PAYLOAD];
  /// coded and interleaved CQI bits
  uint8_t o_w[(MAX_CQI_BITS+8)*3];
  /// coded CQI bits
  uint8_t o_d[96+((MAX_CQI_BITS+8)*3)];
  /// coded ACK bits
  uint8_t q_ACK[MAX_ACK_PAYLOAD];
  /// coded RI bits
  uint8_t q_RI[MAX_RI_PAYLOAD];
  /// beta_offset_cqi times 8
  uint16_t beta_offset_cqi_times8;
  /// beta_offset_ri times 8
  uint16_t beta_offset_ri_times8;
  /// beta_offset_harqack times 8
  uint16_t beta_offset_harqack_times8;
  /// power_offset
  uint8_t power_offset;
  // for cooperative communication
  uint8_t cooperation_flag;
  /// RNTI attributed to this ULSCH
  uint16_t rnti;
  /// f_PUSCH parameter for PUSCH power control
  int16_t f_pusch;
  /// Po_PUSCH - target output power for PUSCH
  int16_t Po_PUSCH;
  /// PHR - current power headroom (based on last PUSCH transmission)
  int16_t PHR;
  /// Po_SRS - target output power for SRS
  int16_t Po_SRS;
  /// num active cba group
  uint8_t num_active_cba_groups;
  /// num dci found for cba
  uint8_t num_cba_dci[10];
  /// allocated CBA RNTI
  uint16_t cba_rnti[4];//NUM_MAX_CBA_GROUP];
  /// UL max-harq-retransmission
  uint8_t Mlimit;
} LTE_UE_ULSCH_t;

typedef struct UE_SCAN_INFO_s {
  /// 10 best amplitudes (linear) for each pss signals
  int32_t amp[3][10];
  /// 10 frequency offsets (kHz) corresponding to best amplitudes, with respect do minimum DL frequency in the band
  int32_t freq_offset_Hz[3][10];
} UE_SCAN_INFO_t;

/// Enumeration for Parameter \f$P_A\f$ \ref PDSCH_CONFIG_DEDICATED::p_a.
typedef enum {
  dBm6=0, ///< (dB-6) corresponds to -6 dB
  dBm477, ///< (dB-4dot77) corresponds to -4.77 dB
  dBm3,   ///< (dB-3) corresponds to -3 dB
  dBm177, ///< (dB-1dot77) corresponds to -1.77 dB
  dB0,    ///< corresponds to 0 dB
  dB1,    ///< corresponds to 1 dB
  dB2,    ///< corresponds to 2 dB
  dB3     ///< corresponds to 3 dB
} PA_t;

/// PDSCH-ConfigDedicated from 36.331 RRC spec
typedef struct {
  /// Parameter: \f$P_A\f$, see TS 36.213 (5.2).
  PA_t p_a;
} PDSCH_CONFIG_DEDICATED;

typedef struct {
  /// Preamble index for PRACH (0-63)
  uint8_t ra_PreambleIndex;
  /// RACH MaskIndex
  uint8_t ra_RACH_MaskIndex;
  /// Target received power at eNB (-120 ... -82 dBm)
  int8_t ra_PREAMBLE_RECEIVED_TARGET_POWER;
  /// PRACH index for TDD (0 ... 6) depending on TDD configuration and prachConfigIndex
  uint8_t ra_TDD_map_index;
  /// Corresponding RA-RNTI for UL-grant
  uint16_t ra_RNTI;
  /// Pointer to Msg3 payload for UL-grant
  uint8_t *Msg3;
} PRACH_RESOURCES_t;

/// UE specific PUSCH configuration.
typedef struct {
  /// Parameter: \f$I^\text{HARQ-ACK}_\text{offset}\f$, see TS 36.213 (Table 8.6.3-1). \vr{[0..15]}
  uint16_t betaOffset_ACK_Index;
  /// Parameter: \f$I^{RI}_\text{offset}\f$, see TS 36.213 (Table 8.6.3-2). \vr{[0..15]}
  uint16_t betaOffset_RI_Index;
  /// Parameter: \f$I^{CQI}_\text{offset}\f$, see TS 36.213 (Table 8.6.3-3). \vr{[0..15]}
  uint16_t betaOffset_CQI_Index;
} PUSCH_CONFIG_DEDICATED;

/// lola CBA information
typedef struct {
  ///
  uint16_t betaOffset_CA_Index;
  ///
  uint16_t cShift;
} PUSCH_CA_CONFIG_DEDICATED;

/// Enumeration for parameter \f$N_\text{ANRep}\f$ \ref PUCCH_CONFIG_DEDICATED::repetitionFactor.
typedef enum {
  n2=0,
  n4,
  n6
} ACKNAKREP_t;

/// Enumeration for \ref PUCCH_CONFIG_DEDICATED::tdd_AckNackFeedbackMode.
typedef enum {
  bundling=0,
  multiplexing
} ANFBmode_t;

/// PUCCH-ConfigDedicated from 36.331 RRC spec
typedef struct {
  /// Flag to indicate ACK NAK repetition activation, see TS 36.213 (10.1). \vr{[0..1]}
  uint8_t ackNackRepetition;
  /// Parameter: \f$N_\text{ANRep}\f$, see TS 36.213 (10.1).
  ACKNAKREP_t repetitionFactor;
  /// Parameter: \f$n^{(1)}_\text{PUCCH,ANRep}\f$, see TS 36.213 (10.1). \vr{[0..2047]}
  uint16_t n1PUCCH_AN_Rep;
  /// Feedback mode, see TS 36.213 (7.3). \details Applied to both PUCCH and PUSCH feedback. For TDD, should always be set to bundling.
  ANFBmode_t tdd_AckNackFeedbackMode;
} PUCCH_CONFIG_DEDICATED;

/// UplinkPowerControlDedicated Information Element from 36.331 RRC spec
typedef struct {
  /// Parameter: \f$P_\text{0\_UE\_PUSCH}(1)\f$, see TS 36.213 (5.1.1.1), unit dB. \vr{[-8..7]}\n This field is applicable for non-persistent scheduling, only.
  int8_t p0_UE_PUSCH;
  /// Parameter: Ks, see TS 36.213 (5.1.1.1). \vr{[0..1]}\n en0 corresponds to value 0 corresponding to state “disabled”. en1 corresponds to value 1.25 corresponding to “enabled”. \note the specification sais it is an enumerated value. \warning the enumeration values do not correspond to the given values in the specification (en1 should be 1.25).
  uint8_t deltaMCS_Enabled;
  /// Parameter: Accumulation-enabled, see TS 36.213 (5.1.1.1). \vr{[0..1]} 1 corresponds to "enabled" whereas 0 corresponds to "disabled".
  uint8_t accumulationEnabled;
  /// Parameter: \f$P_\text{0\_UE\_PUCCH}(1)\f$, see TS 36.213 (5.1.2.1), unit dB. \vr{[-8..7]}
  int8_t p0_UE_PUCCH;
  /// Parameter: \f$P_\text{SRS\_OFFSET}\f$, see TS 36.213 (5.1.3.1). \vr{[0..15]}\n For Ks=1.25 (\ref deltaMCS_Enabled), the actual parameter value is pSRS_Offset value - 3. For Ks=0, the actual parameter value is -10.5 + 1.5*pSRS_Offset value.
  int8_t pSRS_Offset;
  /// Specifies the filtering coefficient for RSRP measurements used to calculate path loss, as specified in TS 36.213 (5.1.1.1).\details The same filtering mechanism applies as for quantityConfig described in 5.5.3.2. \note the specification sais it is an enumerated value.
  uint8_t filterCoefficient;
} UL_POWER_CONTROL_DEDICATED;

/// Union for \ref TPC_PDCCH_CONFIG::tpc_Index.
typedef union {
  /// Index of N when DCI format 3 is used. See TS 36.212 (5.3.3.1.6). \vr{[1..15]}
  uint8_t indexOfFormat3;
  /// Index of M when DCI format 3A is used. See TS 36.212 (5.3.3.1.7). \vr{[1..31]}
  uint8_t indexOfFormat3A;
} TPC_INDEX_t;

/// TPC-PDCCH-Config Information Element from 36.331 RRC spec
typedef struct {
  /// RNTI for power control using DCI format 3/3A, see TS 36.212. \vr{[0..65535]}
  uint16_t rnti;
  /// Index of N or M, see TS 36.212 (5.3.3.1.6 and 5.3.3.1.7), where N or M is dependent on the used DCI format (i.e. format 3 or 3a).
  TPC_INDEX_t tpc_Index;
} TPC_PDCCH_CONFIG;

/// Enumeration for parameter reporting mode \ref CQI_REPORT_CONFIG::cqi_ReportModeAperiodic.
typedef enum {
  rm12=0,
  rm20=1,
  rm22=2,
  rm30=3,
  rm31=4
} CQI_REPORTMODEAPERIODIC;

/// CQI-ReportPeriodic
typedef struct {
  /// Parameter: \f$n^{(2)}_\text{PUCCH}\f$, see TS 36.213 (7.2). \vr{[0..1185]}, -1 indicates inactivity
  int16_t cqi_PUCCH_ResourceIndex;
  /// Parameter: CQI/PMI Periodicity and Offset Configuration Index \f$I_\text{CQI/PMI}\f$, see TS 36.213 (tables 7.2.2-1A and 7.2.2-1C). \vr{[0..1023]}
  int16_t cqi_PMI_ConfigIndex;
  /// Parameter: K, see 36.213 (4.2.2). \vr{[1..4]}
  uint8_t K;
  /// Parameter: RI Config Index \f$I_\text{RI}\f$, see TS 36.213 (7.2.2-1B). \vr{[0..1023]}, -1 indicates inactivity
  int16_t ri_ConfigIndex;
  /// Parameter: Simultaneous-AN-and-CQI, see TS 36.213 (10.1). \vr{[0..1]} 1 indicates that simultaneous transmission of ACK/NACK and CQI is allowed.
  uint8_t simultaneousAckNackAndCQI;
  /// parameter computed from Tables 7.2.2-1A and 7.2.2-1C
  uint16_t Npd;
  /// parameter computed from Tables 7.2.2-1A and 7.2.2-1C
  uint16_t N_OFFSET_CQI;
} CQI_REPORTPERIODIC;

/// CQI-ReportConfig Information Element from 36.331 RRC spec
typedef struct {
  /// Parameter: reporting mode. Value rm12 corresponds to Mode 1-2, rm20 corresponds to Mode 2-0, rm22 corresponds to Mode 2-2 etc. PUSCH reporting modes are described in TS 36.213 [23, 7.2.1].
  CQI_REPORTMODEAPERIODIC cqi_ReportModeAperiodic;
  /// Parameter: \f$\Delta_\text{offset}\f$, see TS 36.213 (7.2.3). \vr{[-1..6]}\n Actual value = IE value * 2 [dB].
  int8_t nomPDSCH_RS_EPRE_Offset;
  CQI_REPORTPERIODIC CQI_ReportPeriodic;
} CQI_REPORT_CONFIG;

/// SoundingRS-UL-ConfigDedicated Information Element from 36.331 RRC spec
typedef struct {
  /// Parameter: \f$B_\text{SRS}\f$, see TS 36.211 (table 5.5.3.2-1, 5.5.3.2-2, 5.5.3.2-3 and 5.5.3.2-4). \vr{[0..3]} \note the specification sais it is an enumerated value.
  uint8_t srs_Bandwidth;
  /// Parameter: SRS hopping bandwidth \f$b_\text{hop}\in\{0,1,2,3\}\f$, see TS 36.211 (5.5.3.2) \vr{[0..3]} \note the specification sais it is an enumerated value.
  uint8_t srs_HoppingBandwidth;
  /// Parameter: \f$n_\text{RRC}\f$, see TS 36.211 (5.5.3.2). \vr{[0..23]}
  uint8_t freqDomainPosition;
  /// Parameter: Duration, see TS 36.213 (8.2). \vr{[0..1]} 0 corresponds to "single" and 1 to "indefinite".
  uint8_t duration;
  /// Parameter: \f$k_\text{TC}\in\{0,1\}\f$, see TS 36.211 (5.5.3.2). \vr{[0..1]}
  uint8_t transmissionComb;
  /// Parameter: \f$I_\text{SRS}\f$, see TS 36.213 (table 8.2-1). \vr{[0..1023]}
  uint16_t srs_ConfigIndex;
  /// Parameter: \f$n^\text{CS}_\text{SRS}\f$. See TS 36.211 (5.5.3.1). \vr{[0..7]} \note the specification sais it is an enumerated value.
  uint8_t cyclicShift;
  // Parameter: internal implementation: UE SRS configured
  uint8_t srsConfigDedicatedSetup;
  // Parameter: cell srs subframe for internal implementation
  uint8_t srsCellSubframe;
  // Parameter: ue srs subframe for internal implementation
  uint8_t srsUeSubframe;
} SOUNDINGRS_UL_CONFIG_DEDICATED;

/// Enumeration for parameter SR transmission \ref SCHEDULING_REQUEST_CONFIG::dsr_TransMax.
typedef enum {
  sr_n4=0,
  sr_n8=1,
  sr_n16=2,
  sr_n32=3,
  sr_n64=4
} DSR_TRANSMAX_t;

/// SchedulingRequestConfig Information Element from 36.331 RRC spec
typedef struct {
  /// Parameter: \f$n^{(1)}_\text{PUCCH,SRI}\f$, see TS 36.213 (10.1). \vr{[0..2047]}
  uint16_t sr_PUCCH_ResourceIndex;
  /// Parameter: \f$I_\text{SR}\f$, see TS 36.213 (10.1). \vr{[0..155]}
  uint8_t sr_ConfigIndex;
  /// Parameter for SR transmission in TS 36.321 (5.4.4). \details The value n4 corresponds to 4 transmissions, n8 corresponds to 8 transmissions and so on.
  DSR_TRANSMAX_t dsr_TransMax;
} SCHEDULING_REQUEST_CONFIG;

typedef struct {

  long long in;
  long long diff;
  long long diff_now;
  long long p_time; /*!< \brief absolute process duration */
  long long diff_square; /*!< \brief process duration square */
  long long max;
  int trials;
  int meas_flag;
} time_stats_t;

typedef enum {
  MIN_RF_DEV_TYPE = 0,
  /*!\brief device is ExpressMIMO */
  EXMIMO_DEV,
  /*!\brief device is USRP B200/B210*/
  USRP_B200_DEV,
  /*!\brief device is USRP X300/X310*/
  USRP_X300_DEV,
  /*!\brief device is BLADE RF*/
  BLADERF_DEV,
  /*!\brief device is LMSSDR (SoDeRa)*/
  LMSSDR_DEV,
  /*!\brief device is NONE*/
  NONE_DEV,
  MAX_RF_DEV_TYPE

} dev_type_t;

typedef enum {
  MIN_TRANSP_TYPE = 0,
  /*!\brief transport protocol ETHERNET */
  ETHERNET_TP,
  /*!\brief no transport protocol*/
  NONE_TP,
  MAX_TRANSP_TYPE

} transport_type_t;

/*!\brief  openair0 device host type */
typedef enum {
  MIN_HOST_TYPE = 0,
 /*!\brief device functions within a BBU */ //LA: Baseband Unit -> this is the digital function unit. I reckon this is used since the processing is done in the baseband.
  BBU_HOST,
 /*!\brief device functions within a RRH */ //LA: Remote Radio Head -> this is the radio function unit
  RRH_HOST,
  MAX_HOST_TYPE

}host_type_t;

typedef enum {
  duplex_mode_TDD=1,duplex_mode_FDD=0
} duplex_mode_t;

/*! \brief RF Gain clibration */
typedef struct {
  //! Frequency for which RX chain was calibrated
  double freq;
  //! Offset to be applied to RX gain
  double offset;
} rx_gain_calib_table_t;

//#define USRP_GAIN_OFFSET (56.0)  // 86 calibrated for USRP B210 @ 2.6 GHz to get equivalent RS EPRE in OAI to SMBV100 output
typedef enum {
  max_gain=0,med_gain,byp_gain
} rx_gain_t;

/*! \brief Clock source types */
typedef enum {
  //! This tells the underlying hardware to use the internal reference
  internal=0,
  //! This tells the underlying hardware to use the external reference
  external=1,
  //! This tells the underlying hardware to use the gpsdo reference
  gpsdo=2
} clock_source_t;

/*! \brief RF frontend parameters set by application */
typedef struct {
  //! Module ID for this configuration
  int Mod_id;
  //! device log level
  int log_level;
  //! duplexing mode
  duplex_mode_t duplex_mode;
  //! number of downlink resource blocks
  int num_rb_dl;
  //! number of samples per frame
  unsigned int  samples_per_frame;
  //! the sample rate for both transmit and receive.
  double sample_rate;
  //! flag to indicate that the device is doing mmapped DMA transfers
  int mmapped_dma;
  //! offset in samples between TX and RX paths
  int tx_sample_advance;
  //! samples per packet on the fronthaul interface
  int samples_per_packet;
  //! number of RX channels (=RX antennas)
  int rx_num_channels;
  //! number of TX channels (=TX antennas)
  int tx_num_channels;
  //! \brief RX base addresses for mmapped_dma
  int32_t* rxbase[4];
  //! \brief TX base addresses for mmapped_dma
  int32_t* txbase[4];
  //! \brief Center frequency in Hz for RX.
  //! index: [0..rx_num_channels[
  double rx_freq[4];
  //! \brief Center frequency in Hz for TX.
  //! index: [0..rx_num_channels[ !!! see lte-ue.c:427 FIXME iterates over rx_num_channels
  double tx_freq[4];
  //! \brief memory
  //! \brief Pointer to Calibration table for RX gains
  rx_gain_calib_table_t *rx_gain_calib_table;

  //! mode for rxgain (ExpressMIMO2)
  rx_gain_t rxg_mode[4];
  //! \brief Gain for RX in dB.
  //! index: [0..rx_num_channels]
  double rx_gain[4];
  //! \brief Gain offset (for calibration) in dB
  //! index: [0..rx_num_channels]
  double rx_gain_offset[4];
  //! gain for TX in dB
  double tx_gain[4];
  //! RX bandwidth in Hz
  double rx_bw;
  //! TX bandwidth in Hz
  double tx_bw;
  //! clock source
  clock_source_t clock_source;
  //! Auto calibration flag
  int autocal[4];
  //! rf devices work with x bits iqs when oai have its own iq format
  //! the two following parameters are used to convert iqs
  int iq_txshift;
  int iq_rxrescale;
  //! remote IP/MAC addr for Ethernet interface
  char *remote_addr;
  //! remote port number for Ethernet interface
  unsigned int remote_port;
  //! local IP/MAC addr for Ethernet interface (eNB/BBU, UE)
  char *my_addr;
  //! local port number for Ethernet interface (eNB/BBU, UE)
  unsigned int my_port;
  //! Configuration file for LMS7002M
  char *configFilename;
} openair0_config_t;

/*!\brief structrue holds the parameters to configure USRP devices*/
typedef struct openair0_device_t openair0_device;

/*!\brief structure holds the parameters to configure USRP devices */
struct openair0_device_t {
  /*!brief Module ID of this device */
  int Mod_id;

  /*!brief Component Carrier ID of this device */
  int CC_id;

  /*!brief Type of this device */
  dev_type_t type;

  /*!brief Transport protocol type that the device suppports (in case I/Q samples need to be transported) */
  transport_type_t transp_type;

   /*!brief Type of the device's host (BBU/RRH) */
  host_type_t host_type;

  /* !brief RF frontend parameters set by application */
  openair0_config_t *openair0_cfg;

  /*!brief Can be used by driver to hold internal structure*/
  void *priv;

  /* Functions API, which are called by the application*/

  /*! \brief Called to start the transceiver. Return 0 if OK, < 0 if error
      @param device pointer to the device structure specific to the RF hardware target
  */
  int (*trx_start_func)(openair0_device *device);

  /*! \brief Called to send a request message between BBU-RRH
      @param device pointer to the device structure specific to the RF hardware target
      @param msg pointer to the message structure passed between BBU-RRH
      @param msg_len length of the message
  */
  int (*trx_request_func)(openair0_device *device, void *msg, ssize_t msg_len);

  /*! \brief Called to send a reply  message between BBU-RRH
      @param device pointer to the device structure specific to the RF hardware target
      @param msg pointer to the message structure passed between BBU-RRH
      @param msg_len length of the message
  */
  int (*trx_reply_func)(openair0_device *device, void *msg, ssize_t msg_len);

  /*! \brief Called to send samples to the RF target
      @param device pointer to the device structure specific to the RF hardware target
      @param timestamp The timestamp at whicch the first sample MUST be sent
      @param buff Buffer which holds the samples
      @param nsamps number of samples to be sent
      @param antenna_id index of the antenna if the device has multiple anteannas
      @param flags flags must be set to TRUE if timestamp parameter needs to be applied
  */
  int (*trx_write_func)(openair0_device *device, openair0_timestamp timestamp, void **buff, int nsamps,int antenna_id, int flags);

  /*! \brief Receive samples from hardware.
   * Read \ref nsamps samples from each channel to buffers. buff[0] is the array for
   * the first channel. *ptimestamp is the time at which the first sample
   * was received.
   * \param device the hardware to use
   * \param[out] ptimestamp the time at which the first sample was received.
   * \param[out] buff An array of pointers to buffers for received samples. The buffers must be large enough to hold the number of samples \ref nsamps.
   * \param nsamps Number of samples. One sample is 2 byte I + 2 byte Q => 4 byte.
   * \param antenna_id Index of antenna for which to receive samples
   * \returns the number of sample read
   */
  int (*trx_read_func)(openair0_device *device, openair0_timestamp *ptimestamp, void **buff, int nsamps,int antenna_id);

  /*! \brief print the device statistics
   * \param device the hardware to use
   * \returns  0 on success
   */
  int (*trx_get_stats_func)(openair0_device *device);

  /*! \brief Reset device statistics
   * \param device the hardware to use
   * \returns 0 in success
   */
  int (*trx_reset_stats_func)(openair0_device *device);

  /*! \brief Terminate operation of the transceiver -- free all associated resources
   * \param device the hardware to use
   */
  void (*trx_end_func)(openair0_device *device);

  /*! \brief Stop operation of the transceiver
   */
  int (*trx_stop_func)(openair0_device *device);

  /* Functions API related to UE*/

  /*! \brief Set RX feaquencies
   * \param device the hardware to use
   * \param openair0_cfg RF frontend parameters set by application
   * \param exmimo_dump_config  dump EXMIMO configuration
   * \returns 0 in success
   */
  int (*trx_set_freq_func)(openair0_device* device, openair0_config_t *openair0_cfg,int exmimo_dump_config);

  /*! \brief Set gains
   * \param device the hardware to use
   * \param openair0_cfg RF frontend parameters set by application
   * \returns 0 in success
   */
  int (*trx_set_gains_func)(openair0_device* device, openair0_config_t *openair0_cfg);

};



/// Top-level PHY Data Structure for UE
typedef struct {
  /// \brief Module ID indicator for this instance
  uint8_t Mod_id;
  /// \brief Component carrier ID for this PHY instance
  uint8_t CC_id;
  /// \brief Mapping of CC_id antennas to cards
  openair0_rf_map      rf_map;
  //uint8_t local_flag;
  /// \brief Indicator of current run mode of UE (normal_txrx, rx_calib_ue, no_L2_connect, debug_prach)
  runmode_t mode;
  /// \brief Indicator that UE should perform band scanning
  int UE_scan;
  /// \brief Indicator that UE should perform coarse scanning around carrier
  int UE_scan_carrier;
  /// \brief Indicator that UE is synchronized to an eNB
  int is_synchronized;
  /// Data structure for UE process scheduling
  UE_proc_t proc;
  /// Flag to indicate the UE shouldn't do timing correction at all
  int no_timing_correction;
  /// \brief Total gain of the TX chain (16-bit baseband I/Q to antenna)
  uint32_t tx_total_gain_dB;
  /// \brief Total gain of the RX chain (antenna to baseband I/Q) This is a function of rx_gain_mode (and the corresponding gain) and the rx_gain of the card.
  uint32_t rx_total_gain_dB;
  /// \brief Total gains with maximum RF gain stage (ExpressMIMO2/Lime)
  uint32_t rx_gain_max[4];
  /// \brief Total gains with medium RF gain stage (ExpressMIMO2/Lime)
  uint32_t rx_gain_med[4];
  /// \brief Total gains with bypassed RF gain stage (ExpressMIMO2/Lime)
  uint32_t rx_gain_byp[4];
  /// \brief Current transmit power
  int16_t tx_power_dBm[10];
  /// \brief Total number of REs in current transmission
  int tx_total_RE[10];
  /// \brief Maximum transmit power
  int8_t tx_power_max_dBm;
  /// \brief Number of eNB seen by UE
  uint8_t n_connected_eNB;
  /// \brief indicator that Handover procedure has been initiated
  uint8_t ho_initiated;
  /// \brief indicator that Handover procedure has been triggered
  uint8_t ho_triggered;
  /// \brief Measurement variables.
  PHY_MEASUREMENTS measurements;
  LTE_DL_FRAME_PARMS  frame_parms;
  /// \brief Frame parame before ho used to recover if ho fails.
  LTE_DL_FRAME_PARMS  frame_parms_before_ho;
  LTE_UE_COMMON    common_vars;

  // point to the current rxTx thread index
  uint8_t current_thread_id[10];

  LTE_UE_PDSCH     *pdsch_vars[RX_NB_TH_MAX][NUMBER_OF_CONNECTED_eNB_MAX+1]; // two RxTx Threads
  LTE_UE_PDSCH_FLP *pdsch_vars_flp[NUMBER_OF_CONNECTED_eNB_MAX+1];
  LTE_UE_PDSCH     *pdsch_vars_SI[NUMBER_OF_CONNECTED_eNB_MAX+1];
  LTE_UE_PDSCH     *pdsch_vars_ra[NUMBER_OF_CONNECTED_eNB_MAX+1];
  LTE_UE_PDSCH     *pdsch_vars_p[NUMBER_OF_CONNECTED_eNB_MAX+1];
  LTE_UE_PDSCH     *pdsch_vars_MCH[NUMBER_OF_CONNECTED_eNB_MAX];
  LTE_UE_PBCH      *pbch_vars[NUMBER_OF_CONNECTED_eNB_MAX];
  LTE_UE_PDCCH     *pdcch_vars[RX_NB_TH_MAX][NUMBER_OF_CONNECTED_eNB_MAX];
  LTE_UE_PRACH     *prach_vars[NUMBER_OF_CONNECTED_eNB_MAX];
  LTE_UE_DLSCH_t   *dlsch[RX_NB_TH_MAX][NUMBER_OF_CONNECTED_eNB_MAX][2]; // two RxTx Threads
  LTE_UE_ULSCH_t   *ulsch[NUMBER_OF_CONNECTED_eNB_MAX];
  LTE_UE_DLSCH_t   *dlsch_SI[NUMBER_OF_CONNECTED_eNB_MAX];
  LTE_UE_DLSCH_t   *dlsch_ra[NUMBER_OF_CONNECTED_eNB_MAX];
  LTE_UE_DLSCH_t   *dlsch_p[NUMBER_OF_CONNECTED_eNB_MAX];
  LTE_UE_DLSCH_t   *dlsch_MCH[NUMBER_OF_CONNECTED_eNB_MAX];
  // This is for SIC in the UE, to store the reencoded data
  LTE_eNB_DLSCH_t  *dlsch_eNB[NUMBER_OF_CONNECTED_eNB_MAX];

  //Paging parameters
  uint32_t              IMSImod1024;
  uint32_t              PF;
  uint32_t              PO;

  // For abstraction-purposes only
  uint8_t               sr[10];
  uint8_t               pucch_sel[10];
  uint8_t               pucch_payload[22];

  UE_MODE_t        UE_mode[NUMBER_OF_CONNECTED_eNB_MAX];
  /// cell-specific reference symbols
  uint32_t lte_gold_table[7][20][2][14];

  /// UE-specific reference symbols (p=5), TM 7
  uint32_t lte_gold_uespec_port5_table[20][38];

  /// ue-specific reference symbols
  uint32_t lte_gold_uespec_table[2][20][2][21];

  /// mbsfn reference symbols
  uint32_t lte_gold_mbsfn_table[10][3][42];

  uint32_t X_u[64][839];

  uint32_t high_speed_flag;
  uint32_t perfect_ce;
  int16_t ch_est_alpha;
  int generate_ul_signal[NUMBER_OF_CONNECTED_eNB_MAX];

  UE_SCAN_INFO_t scan_info[NB_BANDS_MAX];

  char ulsch_no_allocation_counter[NUMBER_OF_CONNECTED_eNB_MAX];



  unsigned char ulsch_Msg3_active[NUMBER_OF_CONNECTED_eNB_MAX];
  uint32_t  ulsch_Msg3_frame[NUMBER_OF_CONNECTED_eNB_MAX];
  unsigned char ulsch_Msg3_subframe[NUMBER_OF_CONNECTED_eNB_MAX];
  PRACH_RESOURCES_t *prach_resources[NUMBER_OF_CONNECTED_eNB_MAX];
  int turbo_iterations, turbo_cntl_iterations;
  /// \brief ?.
  /// - first index: eNB [0..NUMBER_OF_CONNECTED_eNB_MAX[ (hard coded)
  uint32_t total_TBS[NUMBER_OF_CONNECTED_eNB_MAX];
  /// \brief ?.
  /// - first index: eNB [0..NUMBER_OF_CONNECTED_eNB_MAX[ (hard coded)
  uint32_t total_TBS_last[NUMBER_OF_CONNECTED_eNB_MAX];
  /// \brief ?.
  /// - first index: eNB [0..NUMBER_OF_CONNECTED_eNB_MAX[ (hard coded)
  uint32_t bitrate[NUMBER_OF_CONNECTED_eNB_MAX];
  /// \brief ?.
  /// - first index: eNB [0..NUMBER_OF_CONNECTED_eNB_MAX[ (hard coded)
  uint32_t total_received_bits[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_errors[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_errors_last[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_received[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_received_last[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_fer[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_SI_received[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_SI_errors[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_ra_received[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_ra_errors[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_p_received[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_p_errors[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mch_received_sf[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mch_received[NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mcch_received[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mtch_received[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mcch_errors[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mtch_errors[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mcch_trials[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int dlsch_mtch_trials[MAX_MBSFN_AREA][NUMBER_OF_CONNECTED_eNB_MAX];
  int current_dlsch_cqi[NUMBER_OF_CONNECTED_eNB_MAX];
  unsigned char first_run_timing_advance[NUMBER_OF_CONNECTED_eNB_MAX];
  uint8_t               generate_prach;
  uint8_t               prach_cnt;
  uint8_t               prach_PreambleIndex;
  //  uint8_t               prach_timer;
  uint8_t               decode_SIB;
  uint8_t               decode_MIB;
  int              rx_offset; /// Timing offset
  int              rx_offset_diff; /// Timing adjustment for ofdm symbol0 on HW USRP
  int              time_sync_cell;
  int              timing_advance; ///timing advance signalled from eNB
  int              hw_timing_advance;
  int              N_TA_offset; ///timing offset used in TDD
  /// Flag to tell if UE is secondary user (cognitive mode)
  unsigned char    is_secondary_ue;
  /// Flag to tell if secondary eNB has channel estimates to create NULL-beams from.
  unsigned char    has_valid_precoder;
  /// hold the precoder for NULL beam to the primary eNB
  int              **ul_precoder_S_UE;
  /// holds the maximum channel/precoder coefficient
  char             log2_maxp;

  /// if ==0 enables phy only test mode
  int mac_enabled;

  /// Flag to initialize averaging of PHY measurements
  int init_averaging;

  /// \brief sinr for all subcarriers of the current link (used only for abstraction).
  /// - first index: ? [0..12*N_RB_DL[
  double *sinr_dB;

  /// \brief sinr for all subcarriers of first symbol for the CQI Calculation.
  /// - first index: ? [0..12*N_RB_DL[
  double *sinr_CQI_dB;

  /// sinr_effective used for CQI calulcation
  double sinr_eff;

  /// N0 (used for abstraction)
  double N0;

  /// PDSCH Varaibles
  PDSCH_CONFIG_DEDICATED pdsch_config_dedicated[NUMBER_OF_CONNECTED_eNB_MAX];

  /// PUSCH Varaibles
  PUSCH_CONFIG_DEDICATED pusch_config_dedicated[NUMBER_OF_CONNECTED_eNB_MAX];

  /// PUSCH contention-based access vars
  PUSCH_CA_CONFIG_DEDICATED  pusch_ca_config_dedicated[NUMBER_OF_eNB_MAX]; // lola

  /// PUCCH variables

  PUCCH_CONFIG_DEDICATED pucch_config_dedicated[NUMBER_OF_CONNECTED_eNB_MAX];

  uint8_t ncs_cell[20][7];

  /// UL-POWER-Control
  UL_POWER_CONTROL_DEDICATED ul_power_control_dedicated[NUMBER_OF_CONNECTED_eNB_MAX];

  /// TPC
  TPC_PDCCH_CONFIG tpc_pdcch_config_pucch[NUMBER_OF_CONNECTED_eNB_MAX];
  TPC_PDCCH_CONFIG tpc_pdcch_config_pusch[NUMBER_OF_CONNECTED_eNB_MAX];

  /// CQI reporting
  CQI_REPORT_CONFIG cqi_report_config[NUMBER_OF_CONNECTED_eNB_MAX];

  /// SRS Variables
  SOUNDINGRS_UL_CONFIG_DEDICATED soundingrs_ul_config_dedicated[NUMBER_OF_CONNECTED_eNB_MAX];

  /// Scheduling Request Config
  SCHEDULING_REQUEST_CONFIG scheduling_request_config[NUMBER_OF_CONNECTED_eNB_MAX];

  /// Transmission mode per eNB
  uint8_t transmission_mode[NUMBER_OF_CONNECTED_eNB_MAX];

  time_stats_t phy_proc[RX_NB_TH];
  time_stats_t phy_proc_tx;
  time_stats_t phy_proc_rx[RX_NB_TH];

  uint32_t use_ia_receiver;

  time_stats_t ofdm_mod_stats;
  time_stats_t ulsch_encoding_stats;
  time_stats_t ulsch_modulation_stats;
  time_stats_t ulsch_segmentation_stats;
  time_stats_t ulsch_rate_matching_stats;
  time_stats_t ulsch_turbo_encoding_stats;
  time_stats_t ulsch_interleaving_stats;
  time_stats_t ulsch_multiplexing_stats;

  time_stats_t generic_stat;
  time_stats_t generic_stat_bis[RX_NB_TH][LTE_SLOTS_PER_SUBFRAME];
  time_stats_t ue_front_end_stat[RX_NB_TH];
  time_stats_t ue_front_end_per_slot_stat[RX_NB_TH][LTE_SLOTS_PER_SUBFRAME];
  time_stats_t pdcch_procedures_stat[RX_NB_TH];
  time_stats_t pdsch_procedures_stat[RX_NB_TH];
  time_stats_t pdsch_procedures_per_slot_stat[RX_NB_TH][LTE_SLOTS_PER_SUBFRAME];
  time_stats_t dlsch_procedures_stat[RX_NB_TH];

  time_stats_t ofdm_demod_stats;
  time_stats_t dlsch_rx_pdcch_stats;
  time_stats_t rx_dft_stats;
  time_stats_t dlsch_channel_estimation_stats;
  time_stats_t dlsch_freq_offset_estimation_stats;
  time_stats_t dlsch_decoding_stats[2];
  time_stats_t dlsch_demodulation_stats;
  time_stats_t dlsch_rate_unmatching_stats;
  time_stats_t dlsch_turbo_decoding_stats;
  time_stats_t dlsch_deinterleaving_stats;
  time_stats_t dlsch_llr_stats;
  time_stats_t dlsch_llr_stats_parallelization[RX_NB_TH][LTE_SLOTS_PER_SUBFRAME];
  time_stats_t dlsch_unscrambling_stats;
  time_stats_t dlsch_rate_matching_stats;
  time_stats_t dlsch_turbo_encoding_stats;
  time_stats_t dlsch_interleaving_stats;
  time_stats_t dlsch_tc_init_stats;
  time_stats_t dlsch_tc_alpha_stats;
  time_stats_t dlsch_tc_beta_stats;
  time_stats_t dlsch_tc_gamma_stats;
  time_stats_t dlsch_tc_ext_stats;
  time_stats_t dlsch_tc_intl1_stats;
  time_stats_t dlsch_tc_intl2_stats;
  time_stats_t tx_prach;

  /// RF and Interface devices per CC
  openair0_device rfdevice;
  time_stats_t dlsch_encoding_SIC_stats;
  time_stats_t dlsch_scrambling_SIC_stats;
  time_stats_t dlsch_modulation_SIC_stats;
  time_stats_t dlsch_llr_stripping_unit_SIC_stats;
  time_stats_t dlsch_unscrambling_SIC_stats;

#if ENABLE_RAL
  hash_table_t    *ral_thresholds_timed;
  SLIST_HEAD(ral_thresholds_gen_poll_s, ral_threshold_phy_t) ral_thresholds_gen_polled[RAL_LINK_PARAM_GEN_MAX];
  SLIST_HEAD(ral_thresholds_lte_poll_s, ral_threshold_phy_t) ral_thresholds_lte_polled[RAL_LINK_PARAM_LTE_MAX];
#endif

} PHY_VARS_UE;

typedef struct {
  PHY_VARS_UE    *ue;
  UE_rxtx_proc_t *proc;
} rx_tx_thread_data;



