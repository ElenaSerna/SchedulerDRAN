/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Marco Miozzo <marco.miozzo@cttc.es>
 */

#ifndef SCHEDULER_CRAN_MAC_H
#define SCHEDULER_CRAN_MAC_H

#include <ns3/lte-common.h>
#include <ns3/ff-mac-csched-sap.h>
#include <ns3/ff-mac-sched-sap.h>
#include <ns3/ff-mac-scheduler.h>
#include <vector>
#include <map>
#include <ns3/nstime.h>
#include <ns3/lte-amc.h>
#include <ns3/lte-ffr-sap.h>
#include <ns3/TiposScheduling.h>
#include <ns3/lte-enb-rrc.h>
#include <ns3/lte-enb-phy.h>
#include <ns3/lte-phy.h>


// value for SINR outside the range defined by FF-API, used to indicate that there
// is no CQI for this element
#define NO_SINR -5000


#define HARQ_PROC_NUM 8
#define HARQ_DL_TIMEOUT 11

namespace ns3 {


typedef std::vector < uint8_t > DlHarqProcessesStatus_t;
typedef std::vector < uint8_t > DlHarqProcessesTimer_t;
typedef std::vector < DlDciListElement_s > DlHarqProcessesDciBuffer_t; // DCI -> dowlink control indicator
typedef std::vector < std::vector <struct RlcPduListElement_s> > RlcPduList_t; // vector of the LCs and layers per UE
typedef std::vector < RlcPduList_t > DlHarqRlcPduListBuffer_t; // vector of the 8 HARQ processes per UE

typedef std::vector < UlDciListElement_s > UlHarqProcessesDciBuffer_t;
typedef std::vector < uint8_t > UlHarqProcessesStatus_t;


struct schedulerCRANFlowPerf_t
{
  Time flowStart;
  unsigned long totalBytesTransmitted;
  unsigned int lastTtiBytesTrasmitted;
  double lastAveragedThroughput;
};


/**
 * \ingroup ff-api
 * \brief Implements the SCHED SAP and CSCHED SAP for a Proportional Fair scheduler
 *
 * This class implements the interface defined by the FfMacScheduler abstract class
 */

class schedulerCRANMac : public FfMacScheduler
{
public:
  /**
   * \brief Constructor
   *
   * Creates the MAC Scheduler interface implementation
   */
  schedulerCRANMac ();

  /**
   * Destructor
   */
  virtual ~schedulerCRANMac ();

  // inherited from Object
  virtual void DoDispose (void);
  static TypeId GetTypeId (void);

  // inherited from FfMacScheduler
  virtual void SetFfMacCschedSapUser (FfMacCschedSapUser* s);
  virtual void SetFfMacSchedSapUser (FfMacSchedSapUser* s);
  virtual FfMacCschedSapProvider* GetFfMacCschedSapProvider ();
  virtual FfMacSchedSapProvider* GetFfMacSchedSapProvider ();

  // FFR SAPs
  virtual void SetLteFfrSapProvider (LteFfrSapProvider* s);
  virtual LteFfrSapUser* GetLteFfrSapUser ();

  friend class schedulerCRANMemberCschedSapProvider; // its private and protected members can be accessed from outside because it is declarated as friend class
  friend class schedulerCRANMemberSchedSapProvider;
  friend class LteEnbRrc;

  void TransmissionModeConfigurationUpdate (uint16_t rnti, uint8_t txMode);

//  static const char *scheduling_path_results;
//  static T_UL_DL ULDL;
//  static int Nsubf;
//  static int Nsubf_promedia;
//  static int Nsubbands;
//  static int Nsubbands_CQI;
//  static int NRB;
//  static int TruncCQI;
//  static int Debug;
//  static unsigned int Semilla;
//  static int Scheduler_type;
//  static double Sinr_gain_min;
//  static double Sinr_gain_max;
//  static int Sinr_gain_points;
//  static int Schedgain_sector_file;
//  static double *SINR_avg;
//  static double **Ganancias;
//  static int NSectores;
//  static int Nusers;

  static int Nsubbands_user; // Numero de subbandas maximo que puede tener un usuario (adyacentes o no)

  static int nextRNTI; // indice UE asignar en scheduler simple en el que hay UEs pero no CQIs
private:
  //
  // Implementation of the CSCHED API primitives
  // (See 4.1 for description of the primitives)
  //
  void DoCschedCellConfigReq (const struct FfMacCschedSapProvider::CschedCellConfigReqParameters& params);

  void DoCschedUeConfigReq (const struct FfMacCschedSapProvider::CschedUeConfigReqParameters& params);

  void DoCschedLcConfigReq (const struct FfMacCschedSapProvider::CschedLcConfigReqParameters& params);

  void DoCschedLcReleaseReq (const struct FfMacCschedSapProvider::CschedLcReleaseReqParameters& params);

  void DoCschedUeReleaseReq (const struct FfMacCschedSapProvider::CschedUeReleaseReqParameters& params);

  //
  // Implementation of the SCHED API primitives
  // (See 4.2 for description of the primitives)
  //
  
  void DoSchedDlTriggerReq (const struct FfMacSchedSapProvider::SchedDlTriggerReqParameters& params); // starts the DL MAC scheduler for this subframe

  void DoSchedDlRlcBufferReq (const struct FfMacSchedSapProvider::SchedDlRlcBufferReqParameters& params);

  void DoSchedDlPagingBufferReq (const struct FfMacSchedSapProvider::SchedDlPagingBufferReqParameters& params);

  void DoSchedDlMacBufferReq (const struct FfMacSchedSapProvider::SchedDlMacBufferReqParameters& params);

  void DoSchedDlRachInfoReq (const struct FfMacSchedSapProvider::SchedDlRachInfoReqParameters& params);

  void DoSchedDlCqiInfoReq (const struct FfMacSchedSapProvider::SchedDlCqiInfoReqParameters& params);

  void DoSchedUlTriggerReq (const struct FfMacSchedSapProvider::SchedUlTriggerReqParameters& params);

  void DoSchedUlNoiseInterferenceReq (const struct FfMacSchedSapProvider::SchedUlNoiseInterferenceReqParameters& params);

  void DoSchedUlSrInfoReq (const struct FfMacSchedSapProvider::SchedUlSrInfoReqParameters& params);

  void DoSchedUlMacCtrlInfoReq (const struct FfMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters& params);

  void DoSchedUlCqiInfoReq (const struct FfMacSchedSapProvider::SchedUlCqiInfoReqParameters& params);

  // Identificación de cómo va a ser la asignación de recursos -> Basado en High-Layer configured CQI y resource allocation type 0
  // Vamos a considerar relación 1 a 1 entre RBG y el tamaño de las subbandas para tener más resolución.
  // Suponemos que para 5G se definirá esta relación para obtener más precisión en la estimación del canal.
  // Ahora mismo LTE considera una asignación 1 RBG a 2 tamaño de las subbandas --> BW de estimación de 1.4 MHz para un ancho de banda de 20 MHz, es muy grande los valores de SNR pueden variar mucho
  int GetRbgSize (int dlbandwidth); 
  // Consideraremos un total de 25 subbandas -> 25 RBGs

  int LcActivePerFlow (uint16_t rnti); // Obtener el número de LC activos por flujo (para tx/ re-tx)

  double EstimateUlSinr (uint16_t rnti, uint16_t rb); // Obtenemos la SINR media para cada RB

  void RefreshDlCqiMaps (void);
  void RefreshUlCqiMaps (void);

  void UpdateDlRlcBufferInfo (uint16_t rnti, uint8_t lcid, uint16_t size);
  void UpdateUlRlcBufferInfo (uint16_t rnti, uint16_t size);

  /**
  * \brief Update and return a new process Id for the RNTI specified
  *
  * \param rnti the RNTI of the UE to be updated
  * \return the process id  value
  */
  uint8_t UpdateHarqProcessId (uint16_t rnti);

  /**
  * \brief Return the availability of free process for the RNTI specified
  *
  * \param rnti the RNTI of the UE to be updated
  * \return the process id  value
  */
  uint8_t HarqProcessAvailability (uint16_t rnti);

  /**
  * \brief Refresh HARQ processes according to the timers
  *
  */
  void RefreshHarqProcesses ();
//
  Ptr<LteAmc> m_amc;

  /*
   * Vectors of UE's LC info
  */
  // std::map <First, Last> name --> Crea un map container con tantos elementos como se indica en el rango [First, last)
  // incluyendo el elemento señalado por first pero no elemento señalado por last
  // con cada elemento construido a partir de su elemento correspondiente en ese intervalo
  std::map <LteFlowId_t, FfMacSchedSapProvider::SchedDlRlcBufferReqParameters> m_rlcBufferReq; // Objetos de tipo SchedDlRlc... con índices LteFlowId_t ??
  // Contenedor asociativo, asocian el valor Last con una determinada palabra clave, first

  /*
  * Map of UE statistics (per RNTI basis) in downlink
  */
  std::map <uint16_t, schedulerCRANFlowPerf_t> m_flowStatsDl;

  /*
  * Map of UE statistics (per RNTI basis) in uplink
  */
  std::map <uint16_t, schedulerCRANFlowPerf_t> m_flowStatsUl;


  /*
  * Map of UE's DL CQI P10 received --> Periodic CQI: reportado cada cierto tiempo
  */
  std::map <uint16_t,uint8_t> m_p10CqiRxed;
  /*
  * Map of UE's timers on DL CQI P10 received
  */
  std::map <uint16_t,uint32_t> m_p10CqiTimers;

  /*
  * Map of UE's DL CQI A30 received --> Aperiodic CQI: reportado bajo demanda del eNB
  */
  std::map <uint16_t,SbMeasResult_s> m_a30CqiRxed;
  /*
  * Map of UE's timers on DL CQI A30 received
  */
  std::map <uint16_t,uint32_t> m_a30CqiTimers;

  /*
  * Map of UE's metrics
  */
  std::map <uint16_t, std::vector <double> > m_metricasUEs; // Mapa que contiene las metricas (por subbanda) para cada usuario

  /*
  * Map of previous allocated UE per RBG --> sfnSf-usuarios(RNTI)
  * (used to retrieve info from UL-CQI)
  */
  std::map <uint16_t, std::vector <uint16_t> > m_allocationMaps;


  /*
  * Map of UEs' UL-CQI per RBG --> CQI INSTANTÁNEO?? O MEDIO??
  */
  std::map <uint16_t, std::vector <double> > m_ueCqi;
  /*
  * Map of UEs' timers on UL-CQI per RBG
  */
  std::map <uint16_t, uint32_t> m_ueCqiTimers;

  /*
  * Map of UE's buffer status reports received
  */
  std::map <uint16_t,uint32_t> m_ceBsrRxed;

  // MAC SAPs
  FfMacCschedSapUser* m_cschedSapUser;
  FfMacSchedSapUser* m_schedSapUser;
  FfMacCschedSapProvider* m_cschedSapProvider;
  FfMacSchedSapProvider* m_schedSapProvider;

  // FFR SAPs --> para reutilización de frecuencias
  LteFfrSapUser* m_ffrSapUser;
  LteFfrSapProvider* m_ffrSapProvider;

  // Internal parameters
  FfMacCschedSapProvider::CschedCellConfigReqParameters m_cschedCellConfig;

  double m_timeWindow;

  uint16_t m_nextRntiUl; // RNTI of the next user to be served next scheduling in UL

  int m_cellId_sched;

  uint32_t m_cqiTimersThreshold; // # of TTIs for which a CQI canbe considered valid

  std::map <uint16_t,uint8_t> m_uesTxMode; // txMode of the UEs

  // HARQ attributes
  /**
  * m_harqOn when false inhibit te HARQ mechanisms (by default active)
  */
  bool m_harqOn;
  std::map <uint16_t, uint8_t> m_dlHarqCurrentProcessId;
  //HARQ status
  // 0: process Id available
  // x>0: process Id equal to `x` trasmission count
  std::map <uint16_t, DlHarqProcessesStatus_t> m_dlHarqProcessesStatus;
  std::map <uint16_t, DlHarqProcessesTimer_t> m_dlHarqProcessesTimer;
  std::map <uint16_t, DlHarqProcessesDciBuffer_t> m_dlHarqProcessesDciBuffer;
  std::map <uint16_t, DlHarqRlcPduListBuffer_t> m_dlHarqProcessesRlcPduListBuffer;
  std::vector <DlInfoListElement_s> m_dlInfoListBuffered; // HARQ retx buffered

  std::map <uint16_t, uint8_t> m_ulHarqCurrentProcessId; // RNTI - HARQ process ID
  //HARQ status
  // 0: process Id available
  // x>0: process Id equal to `x` trasmission count
  std::map <uint16_t, UlHarqProcessesStatus_t> m_ulHarqProcessesStatus;
  std::map <uint16_t, UlHarqProcessesDciBuffer_t> m_ulHarqProcessesDciBuffer;


  // RACH attributes (RACH -> Random Access Channel)
  std::vector <struct RachListElement_s> m_rachList;
  std::vector <uint16_t> m_rachAllocationMap;
  uint8_t m_ulGrantMcs; // MCS for UL grant (default 0)
  bool m_rachAllocationDone; // Variable que permite identificar si ya se ha realizado la asignación RACH en cuyo caso podemos saltar a realizar la asignación de subbandas para datos

};

} // namespace ns3

#endif /* PF_FF_MAC_SCHEDULER_H */
