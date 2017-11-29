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

#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/math.h>

#include <ns3/simulator.h>
#include <ns3/lte-amc.h>
#include <ns3/lte-enb-rrc.h>
#include <ns3/lte-ue-rrc.h>

#include <ns3/lte-enb-phy.h>
#include <ns3/lte-enb-mac.h>

#include <ns3/lte-phy.h>
#include <ns3/lte-spectrum-phy.h>


#include <ns3/scheduler-DRAN-Mac.h>
#include <ns3/lte-vendor-specific-parameters.h>
#include <ns3/boolean.h>
#include <ns3/object-vector.h>
#include <ns3/CCalculaScheduling.h>
#include <cfloat>
#include <set>
#include <ns3/lte-common.h>
#include <ns3/lte-spectrum-phy.h>

#include <ns3/Objetos.h>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("schedulerDRANMac");

static const int PfType0AllocationRbg[4] = {
  10,       // RGB size 1
  26,       // RGB size 2
  63,       // RGB size 3
  110       // RGB size 4
};  // see table 7.1.6.1-1 of 36.213


NS_OBJECT_ENSURE_REGISTERED (schedulerDRANMac);



class schedulerDRANMemberCschedSapProvider : public FfMacCschedSapProvider
{
public:
  schedulerDRANMemberCschedSapProvider (schedulerDRANMac* scheduler);

  // inherited from FfMacCschedSapProvider
  virtual void CschedCellConfigReq (const struct CschedCellConfigReqParameters& params);
  virtual void CschedUeConfigReq (const struct CschedUeConfigReqParameters& params);
  virtual void CschedLcConfigReq (const struct CschedLcConfigReqParameters& params);
  virtual void CschedLcReleaseReq (const struct CschedLcReleaseReqParameters& params);
  virtual void CschedUeReleaseReq (const struct CschedUeReleaseReqParameters& params);

private:
  schedulerDRANMemberCschedSapProvider ();
  schedulerDRANMac* m_scheduler;
};

schedulerDRANMemberCschedSapProvider::schedulerDRANMemberCschedSapProvider ()
{
}

schedulerDRANMemberCschedSapProvider::schedulerDRANMemberCschedSapProvider (schedulerDRANMac* scheduler) : m_scheduler (scheduler)
{
}


void
schedulerDRANMemberCschedSapProvider::CschedCellConfigReq (const struct CschedCellConfigReqParameters& params)
{
  m_scheduler->DoCschedCellConfigReq (params);
}

void
schedulerDRANMemberCschedSapProvider::CschedUeConfigReq (const struct CschedUeConfigReqParameters& params)
{
  m_scheduler->DoCschedUeConfigReq (params);
}


void
schedulerDRANMemberCschedSapProvider::CschedLcConfigReq (const struct CschedLcConfigReqParameters& params)
{
  m_scheduler->DoCschedLcConfigReq (params);
}

void
schedulerDRANMemberCschedSapProvider::CschedLcReleaseReq (const struct CschedLcReleaseReqParameters& params)
{
  m_scheduler->DoCschedLcReleaseReq (params);
}

void
schedulerDRANMemberCschedSapProvider::CschedUeReleaseReq (const struct CschedUeReleaseReqParameters& params)
{
  m_scheduler->DoCschedUeReleaseReq (params);
}




class schedulerDRANMemberSchedSapProvider : public FfMacSchedSapProvider
{
public:
  schedulerDRANMemberSchedSapProvider (schedulerDRANMac* scheduler);

  // inherited from FfMacSchedSapProvider
  virtual void SchedDlRlcBufferReq (const struct SchedDlRlcBufferReqParameters& params);
  virtual void SchedDlPagingBufferReq (const struct SchedDlPagingBufferReqParameters& params);
  virtual void SchedDlMacBufferReq (const struct SchedDlMacBufferReqParameters& params);
  virtual void SchedDlTriggerReq (const struct SchedDlTriggerReqParameters& params);
  virtual void SchedDlRachInfoReq (const struct SchedDlRachInfoReqParameters& params);
  virtual void SchedDlCqiInfoReq (const struct SchedDlCqiInfoReqParameters& params);
  virtual void SchedUlTriggerReq (const struct SchedUlTriggerReqParameters& params);
  virtual void SchedUlNoiseInterferenceReq (const struct SchedUlNoiseInterferenceReqParameters& params);
  virtual void SchedUlSrInfoReq (const struct SchedUlSrInfoReqParameters& params);
  virtual void SchedUlMacCtrlInfoReq (const struct SchedUlMacCtrlInfoReqParameters& params);
  virtual void SchedUlCqiInfoReq (const struct SchedUlCqiInfoReqParameters& params);


private:
  schedulerDRANMemberSchedSapProvider ();
  schedulerDRANMac* m_scheduler;
};



schedulerDRANMemberSchedSapProvider::schedulerDRANMemberSchedSapProvider ()
{
}


schedulerDRANMemberSchedSapProvider::schedulerDRANMemberSchedSapProvider (schedulerDRANMac* scheduler)
  : m_scheduler (scheduler)
{
}

void
schedulerDRANMemberSchedSapProvider::SchedDlRlcBufferReq (const struct SchedDlRlcBufferReqParameters& params)
{
  m_scheduler->DoSchedDlRlcBufferReq (params);
}

void
schedulerDRANMemberSchedSapProvider::SchedDlPagingBufferReq (const struct SchedDlPagingBufferReqParameters& params)
{
  m_scheduler->DoSchedDlPagingBufferReq (params);
}

void
schedulerDRANMemberSchedSapProvider::SchedDlMacBufferReq (const struct SchedDlMacBufferReqParameters& params)
{
  m_scheduler->DoSchedDlMacBufferReq (params);
}

void
schedulerDRANMemberSchedSapProvider::SchedDlTriggerReq (const struct SchedDlTriggerReqParameters& params)
{
  m_scheduler->DoSchedDlTriggerReq (params);
}

void
schedulerDRANMemberSchedSapProvider::SchedDlRachInfoReq (const struct SchedDlRachInfoReqParameters& params)
{
  m_scheduler->DoSchedDlRachInfoReq (params);
}

void
schedulerDRANMemberSchedSapProvider::SchedDlCqiInfoReq (const struct SchedDlCqiInfoReqParameters& params)
{
  m_scheduler->DoSchedDlCqiInfoReq (params);
}

void
schedulerDRANMemberSchedSapProvider::SchedUlTriggerReq (const struct SchedUlTriggerReqParameters& params)
{
  m_scheduler->DoSchedUlTriggerReq (params);
}

void
schedulerDRANMemberSchedSapProvider::SchedUlNoiseInterferenceReq (const struct SchedUlNoiseInterferenceReqParameters& params)
{
  m_scheduler->DoSchedUlNoiseInterferenceReq (params);
}

void
schedulerDRANMemberSchedSapProvider::SchedUlSrInfoReq (const struct SchedUlSrInfoReqParameters& params)
{
  m_scheduler->DoSchedUlSrInfoReq (params);
}

void
schedulerDRANMemberSchedSapProvider::SchedUlMacCtrlInfoReq (const struct SchedUlMacCtrlInfoReqParameters& params)
{
  m_scheduler->DoSchedUlMacCtrlInfoReq (params);
}

void
schedulerDRANMemberSchedSapProvider::SchedUlCqiInfoReq (const struct SchedUlCqiInfoReqParameters& params)
{
  m_scheduler->DoSchedUlCqiInfoReq (params);
}





schedulerDRANMac::schedulerDRANMac ()
  :   m_cschedSapUser (0),
    m_schedSapUser (0),
    m_timeWindow (99.0),
    m_nextRntiUl (0)
{
  m_amc = CreateObject <LteAmc> (); // Creamos el objeto AMC para implementar el esquema adaptativo de codificación y modulación
  m_cschedSapProvider = new schedulerDRANMemberCschedSapProvider (this); // Le pasamos el objeto m_Scheduler
  m_schedSapProvider = new schedulerDRANMemberSchedSapProvider (this);
  m_ffrSapProvider = 0;
  m_ffrSapUser = new MemberLteFfrSapUser<schedulerDRANMac> (this);
}

//int schedulerDRANMac::Nsubbands_user; // Variable global: numero de subbandas maximo por usuario

schedulerDRANMac::~schedulerDRANMac ()
{
  NS_LOG_FUNCTION (this);
}

void
schedulerDRANMac::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_dlHarqProcessesDciBuffer.clear ();
  m_dlHarqProcessesTimer.clear ();
  m_dlHarqProcessesRlcPduListBuffer.clear ();
  m_dlInfoListBuffered.clear ();
  m_ulHarqCurrentProcessId.clear ();
  m_ulHarqProcessesStatus.clear ();
  m_ulHarqProcessesDciBuffer.clear ();
  delete m_cschedSapProvider;
  delete m_schedSapProvider;
  delete m_ffrSapUser;
}

TypeId
schedulerDRANMac::GetTypeId (void) // Permite registrar este tipo (tid: schedulerDRANMac)
{
  static TypeId tid = TypeId ("ns3::schedulerDRANMac")
    .SetParent<FfMacScheduler> ()
    .SetGroupName("Lte")
    .AddConstructor<schedulerDRANMac> ()
    .AddAttribute ("CqiTimerThreshold",
                   "The number of TTIs a CQI is valid (default 1000 - 1 sec.)",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&schedulerDRANMac::m_cqiTimersThreshold),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("HarqEnabled",
                   "Activate/Deactivate the HARQ [by default is active].",
                   BooleanValue (false), // a false para desactivarlo
                   MakeBooleanAccessor (&schedulerDRANMac::m_harqOn),
                   MakeBooleanChecker ())
    .AddAttribute ("UlGrantMcs", // DCI 0 for this information (UL grant)
                   "The MCS of the UL grant, must be [0..15] (default 0)",
                   UintegerValue (0), // --------------> ¿¿ VALOR ??
                   MakeUintegerAccessor (&schedulerDRANMac::m_ulGrantMcs),
                   MakeUintegerChecker<uint8_t> ())
	.AddAttribute ("FilesEnabled",
				   "Indicate if files have to be created",
				   BooleanValue (false), // a false para desactivarlo
				   MakeBooleanAccessor (&schedulerDRANMac::m_generateFiles),
				   MakeBooleanChecker ())
	.AddAttribute ("MaxSubbandsUE",
				   "Maximum number of subbands available per UE",
				   UintegerValue (25),
				   MakeUintegerAccessor (&schedulerDRANMac::m_Nsubbands_user),
				   MakeUintegerChecker<uint8_t> ())
	// Se pueden añadir aquí más atributos de los disponibles en la cabecera para que puedan ser configurables desde fuera
  ;
  return tid;
}



void
schedulerDRANMac::SetFfMacCschedSapUser (FfMacCschedSapUser* s)
{
  m_cschedSapUser = s;
}

void
schedulerDRANMac::SetFfMacSchedSapUser (FfMacSchedSapUser* s)
{
  m_schedSapUser = s;
}

FfMacCschedSapProvider*
schedulerDRANMac::GetFfMacCschedSapProvider ()
{
  return m_cschedSapProvider;
}

FfMacSchedSapProvider*
schedulerDRANMac::GetFfMacSchedSapProvider ()
{
  return m_schedSapProvider;
}

void
schedulerDRANMac::SetLteFfrSapProvider (LteFfrSapProvider* s)
{
  m_ffrSapProvider = s;
}

LteFfrSapUser*
schedulerDRANMac::GetLteFfrSapUser ()
{
  return m_ffrSapUser;
}

void
schedulerDRANMac::DoCschedCellConfigReq (const struct FfMacCschedSapProvider::CschedCellConfigReqParameters& params)
{
  std::cout << "------> Cell Config Req" << std::endl;

  NS_LOG_FUNCTION (this);
  // Read the subset of parameters used
  m_cschedCellConfig = params;
  m_rachAllocationMap.resize (m_cschedCellConfig.m_ulBandwidth, 0); // Resize the vector m_rach... para que tenga los mismos elementos que m_ulBandwidth e inicializamos esos elementos a 0
  FfMacCschedSapUser::CschedUeConfigCnfParameters cnf;
  cnf.m_result = SUCCESS;
  m_cschedSapUser->CschedUeConfigCnf (cnf);
  return;
} // Asigna los parámetros de entrada al objeto interno "m_cschedCellConfi", resize de rachm_dlAllocationMap e "informa" a través de la variable result al UE para que pueda realizar su configuración.

void
schedulerDRANMac::DoCschedUeConfigReq (const struct FfMacCschedSapProvider::CschedUeConfigReqParameters& params)
{
  std::cout << "------> Ue Config Req" << std::endl;
  NS_LOG_FUNCTION (this << " RNTI " << params.m_rnti << " txMode " << (uint16_t)params.m_transmissionMode);
  std::map <uint16_t,uint8_t>::iterator it = m_uesTxMode.find (params.m_rnti);
  if (it == m_uesTxMode.end ())
    {
      m_uesTxMode.insert (std::pair <uint16_t, double> (params.m_rnti, params.m_transmissionMode));
      // generate HARQ buffers
      m_dlHarqCurrentProcessId.insert (std::pair <uint16_t,uint8_t > (params.m_rnti, 0));
      DlHarqProcessesStatus_t dlHarqPrcStatus;
      dlHarqPrcStatus.resize (8,0);
      m_dlHarqProcessesStatus.insert (std::pair <uint16_t, DlHarqProcessesStatus_t> (params.m_rnti, dlHarqPrcStatus));
      DlHarqProcessesTimer_t dlHarqProcessesTimer;
      dlHarqProcessesTimer.resize (8,0);
      m_dlHarqProcessesTimer.insert (std::pair <uint16_t, DlHarqProcessesTimer_t> (params.m_rnti, dlHarqProcessesTimer));
      DlHarqProcessesDciBuffer_t dlHarqdci;
      dlHarqdci.resize (8);
      m_dlHarqProcessesDciBuffer.insert (std::pair <uint16_t, DlHarqProcessesDciBuffer_t> (params.m_rnti, dlHarqdci));
      DlHarqRlcPduListBuffer_t dlHarqRlcPdu;
      dlHarqRlcPdu.resize (2);
      dlHarqRlcPdu.at (0).resize (8);
      dlHarqRlcPdu.at (1).resize (8);
      m_dlHarqProcessesRlcPduListBuffer.insert (std::pair <uint16_t, DlHarqRlcPduListBuffer_t> (params.m_rnti, dlHarqRlcPdu));
      m_ulHarqCurrentProcessId.insert (std::pair <uint16_t,uint8_t > (params.m_rnti, 0));
      UlHarqProcessesStatus_t ulHarqPrcStatus;
      ulHarqPrcStatus.resize (8,0);
      m_ulHarqProcessesStatus.insert (std::pair <uint16_t, UlHarqProcessesStatus_t> (params.m_rnti, ulHarqPrcStatus));
      UlHarqProcessesDciBuffer_t ulHarqdci;
      ulHarqdci.resize (8);
      m_ulHarqProcessesDciBuffer.insert (std::pair <uint16_t, UlHarqProcessesDciBuffer_t> (params.m_rnti, ulHarqdci));
    }
  else
    {
      (*it).second = params.m_transmissionMode;
    }
  return;
} // De los parámetros de entraba obtiene el RNTI del usuario y el modo de transmisión
// Si ese RNTI está incluido en el mapa key-valu m_uesTxMode, asigna al value txMode el parámetro TXMode recogido de entrada
// Si no está incluido, configura los mapas ul y dl de Harq --> ENTENDER PORQUÉ !!

void
schedulerDRANMac::DoCschedLcConfigReq (const struct FfMacCschedSapProvider::CschedLcConfigReqParameters& params)
{
  std::cout << "------> Lc Config Req" << std::endl;
  NS_LOG_FUNCTION (this << " New LC, rnti: "  << params.m_rnti);

  std::map <uint16_t, schedulerDRANFlowPerf_t>::iterator it;
  for (uint16_t i = 0; i < params.m_logicalChannelConfigList.size (); i++)
    {
      it = m_flowStatsDl.find (params.m_rnti);

      if (it == m_flowStatsDl.end ())
        {
          schedulerDRANFlowPerf_t flowStatsDl;
          flowStatsDl.flowStart = Simulator::Now ();
          flowStatsDl.totalBytesTransmitted = 0;
          flowStatsDl.lastTtiBytesTrasmitted = 0;
          flowStatsDl.lastAveragedThroughput = 1;
          m_flowStatsDl.insert (std::pair<uint16_t, schedulerDRANFlowPerf_t> (params.m_rnti, flowStatsDl));
          schedulerDRANFlowPerf_t flowStatsUl;
          flowStatsUl.flowStart = Simulator::Now ();
          flowStatsUl.totalBytesTransmitted = 0;
          flowStatsUl.lastTtiBytesTrasmitted = 0;
          flowStatsUl.lastAveragedThroughput = 1;
          m_flowStatsUl.insert (std::pair<uint16_t, schedulerDRANFlowPerf_t> (params.m_rnti, flowStatsUl));
        }
    }

  return;
} // Configuración del flujo (comienzo, bytes tx, lastAvgTp) --> Un flujo por cada LC

void
schedulerDRANMac::DoCschedLcReleaseReq (const struct FfMacCschedSapProvider::CschedLcReleaseReqParameters& params)
{
  std::cout << "------> Ue Config Req" << std::endl;
  NS_LOG_FUNCTION (this);
  for (uint16_t i = 0; i < params.m_logicalChannelIdentity.size (); i++)
    {
      std::map<LteFlowId_t, FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it = m_rlcBufferReq.begin ();
      std::map<LteFlowId_t, FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator temp;
      while (it!=m_rlcBufferReq.end ())
        {
          if (((*it).first.m_rnti == params.m_rnti) && ((*it).first.m_lcId == params.m_logicalChannelIdentity.at (i)))
            {
              temp = it;
              it++;
              m_rlcBufferReq.erase (temp);
            }
          else
            {
              it++;
            }
        }
    }
  return;
} // Actualización del mapa con la información de los LC de los UE's --> Libera los LC
// Se eliminan del mapa m_rlcBufferReq aquellos LC que el usuario RNTI (parámetro de entrada) tenga asociados

void
schedulerDRANMac::DoCschedUeReleaseReq (const struct FfMacCschedSapProvider::CschedUeReleaseReqParameters& params)
{
	  std::cout << "------> Ue Release Req" << std::endl;
  NS_LOG_FUNCTION (this);

  m_uesTxMode.erase (params.m_rnti);
  m_dlHarqCurrentProcessId.erase (params.m_rnti);
  m_dlHarqProcessesStatus.erase  (params.m_rnti);
  m_dlHarqProcessesTimer.erase (params.m_rnti);
  m_dlHarqProcessesDciBuffer.erase  (params.m_rnti);
  m_dlHarqProcessesRlcPduListBuffer.erase  (params.m_rnti);
  m_ulHarqCurrentProcessId.erase  (params.m_rnti);
  m_ulHarqProcessesStatus.erase  (params.m_rnti);
  m_ulHarqProcessesDciBuffer.erase  (params.m_rnti);
  m_flowStatsDl.erase  (params.m_rnti);
  m_flowStatsUl.erase  (params.m_rnti);
  m_ceBsrRxed.erase (params.m_rnti);
  std::map<LteFlowId_t, FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it = m_rlcBufferReq.begin ();
  std::map<LteFlowId_t, FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator temp;
  while (it!=m_rlcBufferReq.end ())
    {
      if ((*it).first.m_rnti == params.m_rnti)
        {
          temp = it;
          it++;
          m_rlcBufferReq.erase (temp);
        }
      else
        {
          it++;
        }
    }
  if (m_nextRntiUl == params.m_rnti)
    {
      m_nextRntiUl = 0;
    }

  return;
} // Libera la configuración que se ha realizado del UE



void
schedulerDRANMac::DoSchedDlRlcBufferReq (const struct FfMacSchedSapProvider::SchedDlRlcBufferReqParameters& params)
{
	  std::cout << "------> RLC Buffer Req" << std::endl;

  NS_LOG_FUNCTION (this << params.m_rnti << (uint32_t) params.m_logicalChannelIdentity);
  // API generated by RLC for updating RLC parameters on a LC (tx and retx queues)

  std::map <LteFlowId_t, FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it;

  LteFlowId_t flow (params.m_rnti, params.m_logicalChannelIdentity);

  it =  m_rlcBufferReq.find (flow); // Obtenemos la key-value única determinada por los parámetros de entrada

  if (it == m_rlcBufferReq.end ())
    {
      m_rlcBufferReq.insert (std::pair <LteFlowId_t, FfMacSchedSapProvider::SchedDlRlcBufferReqParameters> (flow, params));
    }
  else
    {
      (*it).second = params;
    }

  return;
} // Actualización del buffer m_rlcBufferReq con los parámetros de entrada para un LC concreto

void
schedulerDRANMac::DoSchedDlPagingBufferReq (const struct FfMacSchedSapProvider::SchedDlPagingBufferReqParameters& params)
{
  NS_LOG_FUNCTION (this);
  NS_FATAL_ERROR ("method not implemented");
  return;
}

void
schedulerDRANMac::DoSchedDlMacBufferReq (const struct FfMacSchedSapProvider::SchedDlMacBufferReqParameters& params)
{
  NS_LOG_FUNCTION (this);
  NS_FATAL_ERROR ("method not implemented");
  return;
}

int
schedulerDRANMac::GetRbgSize (int dlbandwidth)
{
	  std::cout << "------> Get RBG Size" << std::endl;

  for (int i = 0; i < 4; i++)
    {
      if (dlbandwidth < PfType0AllocationRbg[i])
        {
          return (i + 1);
        }
    }

  return (-1);
}


int
schedulerDRANMac::LcActivePerFlow (uint16_t rnti)
{
//	  std::cout << "------> LC Active Per Flow" << std::endl;

	// El iterador sirve para recorrer los elementos del mapa. Es un puntero, por ello para acceder a los elementos accedemos con flecha
	// Para saber los keys o values a los que acceder, "first" indica el key y "second" el value
  std::map <LteFlowId_t, FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it;
  int lcActive = 0;
  for (it = m_rlcBufferReq.begin (); it != m_rlcBufferReq.end (); it++)
	  // Con it obtenemos un puntero a la pareja key-value, para obtener la pareja hacemos *it
    {
	  // dereferencing a std::map iterator gives us a std::pair, we can then access its elements with first and second.
	  // For example, (*it).first will give us the key and (*it).second will give us the value.
	  // These are equivalent to it->first and it->second
      if (((*it).first.m_rnti == rnti) && (((*it).second.m_rlcTransmissionQueueSize > 0)
                                           || ((*it).second.m_rlcRetransmissionQueueSize > 0)
                                           || ((*it).second.m_rlcStatusPduSize > 0) )) // Hay datos para transmitir
        {
          lcActive++;
        }
      if ((*it).first.m_rnti > rnti)
        {
          break;
        }
    }
  return (lcActive);

} // Identifica el número de LCs activos para el usuari dado (rnti)


uint8_t
schedulerDRANMac::HarqProcessAvailability (uint16_t rnti)
{
	  std::cout << "------> HARQ Process Availibility" << std::endl;

  NS_LOG_FUNCTION (this << rnti);

  std::map <uint16_t, uint8_t>::iterator it = m_dlHarqCurrentProcessId.find (rnti);
  if (it == m_dlHarqCurrentProcessId.end ())
    {
      NS_FATAL_ERROR ("No Process Id found for this RNTI " << rnti);
    }
  std::map <uint16_t, DlHarqProcessesStatus_t>::iterator itStat = m_dlHarqProcessesStatus.find (rnti);
  if (itStat == m_dlHarqProcessesStatus.end ())
    {
      NS_FATAL_ERROR ("No Process Id Statusfound for this RNTI " << rnti);
    }
  uint8_t i = (*it).second; // HARQ Process Id
  do
    {
      i = (i + 1) % HARQ_PROC_NUM; // Resto de la división
    }
  while ( ((*itStat).second.at (i) != 0)&&(i != (*it).second));
  if ((*itStat).second.at (i) == 0)
    {
      return (true);
    }
  else
    {
      return (false); // return a not valid harq proc id
    }
} // Obtiene true o false dependiendo de si harq proc id es 0 o no --> valido o no



uint8_t
schedulerDRANMac::UpdateHarqProcessId (uint16_t rnti)
{
	  std::cout << "------> Update HARQ Process Id" << std::endl;

  NS_LOG_FUNCTION (this << rnti);

  if (m_harqOn == false)
    {
      return (0);
    }


  std::map <uint16_t, uint8_t>::iterator it = m_dlHarqCurrentProcessId.find (rnti);
  if (it == m_dlHarqCurrentProcessId.end ())
    {
      NS_FATAL_ERROR ("No Process Id found for this RNTI " << rnti);
    }
  std::map <uint16_t, DlHarqProcessesStatus_t>::iterator itStat = m_dlHarqProcessesStatus.find (rnti);
  if (itStat == m_dlHarqProcessesStatus.end ())
    {
      NS_FATAL_ERROR ("No Process Id Statusfound for this RNTI " << rnti);
    }
  uint8_t i = (*it).second;
  do
    {
      i = (i + 1) % HARQ_PROC_NUM;
    }
  while ( ((*itStat).second.at (i) != 0)&&(i != (*it).second));
  if ((*itStat).second.at (i) == 0)
    {
      (*it).second = i;
      (*itStat).second.at (i) = 1;
    }
  else
    {
      NS_FATAL_ERROR ("No HARQ process available for RNTI " << rnti << " check before update with HarqProcessAvailability");
    }

  return ((*it).second);
} // Actualiza el HARQ Process Id actual si HarqProcessAvailability is true


void
schedulerDRANMac::RefreshHarqProcesses ()
{
	  std::cout << "------> Refresh HARQ Processes" << std::endl;

  NS_LOG_FUNCTION (this);

  std::map <uint16_t, DlHarqProcessesTimer_t>::iterator itTimers;
  for (itTimers = m_dlHarqProcessesTimer.begin (); itTimers != m_dlHarqProcessesTimer.end (); itTimers++)
    {
      for (uint16_t i = 0; i < HARQ_PROC_NUM; i++)
        {
          if ((*itTimers).second.at (i) == HARQ_DL_TIMEOUT)
            {
              // reset HARQ process

              NS_LOG_DEBUG (this << " Reset HARQ proc " << i << " for RNTI " << (*itTimers).first);
              std::map <uint16_t, DlHarqProcessesStatus_t>::iterator itStat = m_dlHarqProcessesStatus.find ((*itTimers).first);
              if (itStat == m_dlHarqProcessesStatus.end ())
                {
                  NS_FATAL_ERROR ("No Process Id Status found for this RNTI " << (*itTimers).first);
                }
              (*itStat).second.at (i) = 0;
              (*itTimers).second.at (i) = 0;
            }
          else
            {
              (*itTimers).second.at (i)++;
            }
        }
    }

}

// FUNCIÓN IMPORTANTE --> LANZA EL SCHEDULER DL PARA ESTA SUBTRAMA

void
schedulerDRANMac::SetMetricasRij(int subbandas, int subbandaSize)
{

	// Calculamos la matriz de metricas con los valores de CQI reportados por los UEs attachados a la celda considerada
//	std::map <uint16_t,SbMeasResult_s>::iterator it;
	 std::map <uint16_t, schedulerDRANFlowPerf_t>::iterator it; // Mapa para recoger los UEs
	 std::map <uint16_t, metricas_Rij>::iterator itMetricasRBGs;
//	 std::map <uint16_t, std::vector <double> >::iterator itmcs;

	 // Eliminamos el mapa de metricas obsoleto de la trama-subtrama anterior
	 m_metricasUEs.erase(m_metricasUEs.begin(), m_metricasUEs.end());

	for(int Ksubb = 0; Ksubb < subbandas; Ksubb++)
	{
//		for (it=m_a30CqiRxed.begin(); it != m_a30CqiRxed.end(); it++)
		for (it=m_flowStatsDl.begin(); it != m_flowStatsDl.end(); it++)
		{
			  // Obtenemos CQI usuario en esa subbanda y TX Mode para calcular CQI por capas segun el modo de TX
			  std::map <uint16_t,uint8_t>::iterator itTxMode;
			  itTxMode = m_uesTxMode.find ((*it).first);
			  if (itTxMode == m_uesTxMode.end ())
				{
				  NS_FATAL_ERROR ("No Transmission Mode info on user " << (*it).first);
				  std::cout << "No Transmission Mode info on user " << (*it).first << std::endl;
				}
			  int nLayer = TransmissionModesLayers::TxMode2LayerNum ((*itTxMode).second);
			  std::vector <uint8_t> sbCqi; // CQI por subbanda/RB
              std::map <uint16_t,SbMeasResult_s>::iterator itCqi;
              itCqi = m_a30CqiRxed.find ((*it).first);
              if (itCqi == m_a30CqiRxed.end ())
                {
                  for (uint8_t k = 0; k < nLayer; k++)
                    {
                      sbCqi.push_back (1);  // start with lowest value
                    }
                }
              else
                {
                  sbCqi = (*itCqi).second.m_higherLayerSelected.at (Ksubb).m_sbCqi;
                }
			  uint8_t cqi1 = sbCqi.at (0);
			  uint8_t cqi2 = 1; // CQI de la capa 2, ponemos inicialmente el mas bajo, si el modo de TX indica que hay 2 capas, obtenemos CQI real capa 2
			  if (sbCqi.size () > 1) // si hay 2 capas --> 2 CQIs
				{
				  cqi2 = sbCqi.at (1); // Coge el CQI de las 2 primeras prosiciones, los de las 2 primeras layers (solo hay disponibles 2 como maximo si es MIMO). En caso SISO solo hay una layer.
				}

			  std::map <uint16_t, schedulerDRANFlowPerf_t>::iterator itStats;
			  itStats=m_flowStatsDl.find((*it).first);
			  if ((cqi1 > 0)||(cqi2 > 0)) // CQI == 0 means "out of range" (see table 7.2.3-1 of 36.213)
				{
				  if (LcActivePerFlow ((*it).first) > 0)
					{
					  // this UE has data to transmit
					  double achievableRate = 0.0;
					  uint8_t mcs = 0;
					  for (uint8_t k = 0; k < nLayer; k++)
						{
						  if (sbCqi.size () > k)
							{
							  mcs = m_amc->GetMcsFromCqi (sbCqi.at (k));
							}
						  else
							{
							  // no info on this subband -> worst MCS
							  mcs = 0;
							}
						  achievableRate += ((m_amc->GetTbSizeFromMcs (mcs, subbandaSize) / 8) / 0.001);   // = TB size / TTI
						}

					  double rcqi = achievableRate / (*itStats).second.lastAveragedThroughput; // Nuestra métrica Rij

					  // Añadimos la metrica al mapa de metricas
					  itMetricasRBGs=m_metricasUEs.find((*it).first);
					  if(itMetricasRBGs==m_metricasUEs.end())
					  {
						  // insert new element
						  metricas_Rij tempMap;
						  tempMap.mcs.push_back(mcs);
						  tempMap.Rij.push_back(rcqi);
						  tempMap.achievableRate.push_back(achievableRate);
						  tempMap.lastAveragedThroughput = (*itStats).second.lastAveragedThroughput;
						  m_metricasUEs.insert (std::pair <uint16_t, metricas_Rij > ((*it).first, tempMap)); // Asignamos UE-metrica
					  }
					  else
					  {
						  (*itMetricasRBGs).second.mcs.push_back(mcs);
						  (*itMetricasRBGs).second.Rij.push_back(rcqi);
						  (*itMetricasRBGs).second.achievableRate.push_back(achievableRate);
						  (*itMetricasRBGs).second.lastAveragedThroughput = (*itStats).second.lastAveragedThroughput;


					  }


					} // end if datos a transmitir (lcactive>0)
				}   // end if cqi>0 (valores validos)
			} // end bucle usuarios
	}

}



void
schedulerDRANMac::DoSchedDlTriggerReq (const struct FfMacSchedSapProvider::SchedDlTriggerReqParameters& params)
{
	  std::cout << "------> LANZAMOS SCHEDULER DL" << std::endl;
  NS_LOG_FUNCTION (this << " Frame no. " << (params.m_sfnSf >> 4) << " subframe no. " << (0xF & params.m_sfnSf));
  std::cout << " Frame no. " << (params.m_sfnSf >> 4) << " subframe no. " << (0xF & params.m_sfnSf) << " Celda " << params.m_cellId << std::endl;
  // evaluate the relative channel quality indicator for each UE per each RBG
  // (since we are using allocation type 0 the small unit of allocation is RBG)
  // Resource allocation type 0 (see sec 7.1.6.1 of 36.213)


  // NOTA:: RBG = Subbanda

  RefreshDlCqiMaps (); // Decrementa el mapa de timers para que cuando llegue a 0 vuelva a solicitarlo

  int rbgSize = GetRbgSize (m_cschedCellConfig.m_dlBandwidth);
  int rbgNum = m_cschedCellConfig.m_dlBandwidth / rbgSize; // Número de subbandas
  uint16_t rbgAllocatedNum = 0; // Número de rbgs/subbandas ya asignados (bien por retx o por tx nueva)
  std::set <uint16_t> rntiAllocated; // usuarios ya asignados
  m_rbgMap.resize (m_cschedCellConfig.m_dlBandwidth / rbgSize, false); // Hacemos que el mapa de subbandas asignadas tenga la longitud del nº de subbandas y puestos los valores a false

  m_rbgMap = m_ffrSapProvider->GetAvailableDlRbg (); // El mapa de rbg disponibles viene determinado por el algoritmo de frequency reuse que se utilice
//   De entre los 6 disponibles, en lteHelper está definido por defecto "LteFrNoOpAlgorithm" este algoritmo permite que todas las frecuencias
//   estén disponibles para todas las celdas y así puedan asignar cualquier RB del BW total a cualquier UE
//   Este método llama a su vez a otro DoGetAvailableDlRbg en el que si el mapa de rbg no está creado
//   lo crea e inicializa a false, si está creado devuelve este mapa con los valores que tenga

  for (std::vector<bool>::iterator it = m_rbgMap.begin (); it != m_rbgMap.end (); it++)
    {
      if ((*it) == true )
        {
          rbgAllocatedNum++; // Obtenemos el número de rbg/subbandas que han sido ya asignados
        }
    }


  FfMacSchedSapUser::SchedDlConfigIndParameters ret; // Objeto que almacenará los parámetros de la decisión del Scheduler DL
  // La función SchedDlConfigInd(SchedDlConfigIndParam param) pasa la decisión de scheduling DL a la capa MAC,
  // y desencadena la creación de PDUs de DL MAC y la configuración de subtrama.



  //   update UL HARQ proc id
  std::map <uint16_t, uint8_t>::iterator itProcId;
  for (itProcId = m_ulHarqCurrentProcessId.begin (); itProcId != m_ulHarqCurrentProcessId.end (); itProcId++)
    {
      (*itProcId).second = ((*itProcId).second + 1) % HARQ_PROC_NUM;
    }


  // RACH Allocation

// El RACH se situa en el medio de la subtrama del UL
// Calculamos el máximo ancho de banda contiguo que tiene disponible en el UL
  uint16_t rbAllocatedNum = 0; // Número de RB asignados
  std::vector <bool> ulRbMap;
  ulRbMap.resize (m_cschedCellConfig.m_ulBandwidth, false);
  ulRbMap = m_ffrSapProvider->GetAvailableUlRbg ();
  uint8_t maxContinuousUlBandwidth = 0;
  uint8_t tmpMinBandwidth = 0;
  uint16_t ffrRbStartOffset = 0;
  uint16_t tmpFfrRbStartOffset = 0;
  uint16_t index = 0;

  for (std::vector<bool>::iterator it = ulRbMap.begin (); it != ulRbMap.end (); it++)
    {
      if ((*it) == true )
        {
          rbAllocatedNum++;
          if (tmpMinBandwidth > maxContinuousUlBandwidth)
            {
              maxContinuousUlBandwidth = tmpMinBandwidth;
              ffrRbStartOffset = tmpFfrRbStartOffset;
            }
          tmpMinBandwidth = 0;
        }
      else
        {
          if (tmpMinBandwidth == 0)
            {
              tmpFfrRbStartOffset = index;
            }
          tmpMinBandwidth++;
        }
      index++;
    }

  if (tmpMinBandwidth > maxContinuousUlBandwidth)
    {
      maxContinuousUlBandwidth = tmpMinBandwidth;
      ffrRbStartOffset = tmpFfrRbStartOffset; // Indica la posición del 1er RB libre a partir del cual hay maxContinuosUlBW
    }

  // Asignación RACH
  m_rachAllocationMap.resize (m_cschedCellConfig.m_ulBandwidth, 0); // Mapa con tantos valores como RBs
  uint16_t rbStart = 0;
  rbStart = ffrRbStartOffset; // La asignación empezará en el RB calculado anteriormente a partir del cual hay nRBs libres
  std::vector <struct RachListElement_s>::iterator itRach; // Vector con los RNTI y el tamaña estimado de cada uno de los usuarios que han enviado el preamble al eNB

  for (itRach = m_rachList.begin (); itRach != m_rachList.end (); itRach++)
    {
      NS_ASSERT_MSG (m_amc->GetTbSizeFromMcs (m_ulGrantMcs, m_cschedCellConfig.m_ulBandwidth) > (*itRach).m_estimatedSize, " Default UL Grant MCS does not allow to send RACH messages");
      if (m_amc->GetTbSizeFromMcs (m_ulGrantMcs, m_cschedCellConfig.m_ulBandwidth) > (*itRach).m_estimatedSize)
    	  std::cout << " Default UL Grant MCS does not allow to send RACH messages" << std::endl;
      BuildRarListElement_s newRar; // RAR -> a través de este mensaje enviado en el DL-SCH el eNB informa al UE del RNTI que tiene asignado, un "UL scheduling grant" con los recursos asignados a transmitir en la próxima fase.
      newRar.m_rnti = (*itRach).m_rnti;
      // DL-RACH Allocation
      // Ideal: no needs of configuring m_dci
      // UL-RACH Allocation
      newRar.m_grant.m_rnti = newRar.m_rnti;
      newRar.m_grant.m_mcs = m_ulGrantMcs;
      uint16_t rbLen = 1;
      uint16_t tbSizeBits = 0;
      // find lowest TB size that fits UL grant estimated size
      while ((tbSizeBits < (*itRach).m_estimatedSize) && (rbStart + rbLen < (ffrRbStartOffset + maxContinuousUlBandwidth)))
        {
          rbLen++;
          tbSizeBits = m_amc->GetTbSizeFromMcs (m_ulGrantMcs, rbLen);
        }
      if (tbSizeBits < (*itRach).m_estimatedSize)
        {
          // no more allocation space: finish allocation
          break;
        }
      newRar.m_grant.m_rbStart = rbStart;
      newRar.m_grant.m_rbLen = rbLen;
      newRar.m_grant.m_tbSize = tbSizeBits / 8;
      newRar.m_grant.m_hopping = false;
      newRar.m_grant.m_tpc = 0;
      newRar.m_grant.m_cqiRequest = false; //False indica sólo CQI periódico
      newRar.m_grant.m_ulDelay = false;
      NS_LOG_INFO (this << " UL grant allocated to RNTI " << (*itRach).m_rnti << " rbStart " << rbStart << " rbLen " << rbLen << " MCS " << m_ulGrantMcs << " tbSize " << newRar.m_grant.m_tbSize);
      std::cout<< "UL Grant allocated to RNTI "<< (*itRach).m_rnti << " rbStart " << rbStart << " rbLen " << rbLen <<  " tbSize in bytes " << newRar.m_grant.m_tbSize << " MCS " << m_ulGrantMcs << std::endl;
      for (uint16_t i = rbStart; i < rbStart + rbLen; i++)
        {
          m_rachAllocationMap.at (i) = (*itRach).m_rnti; // En el vector RachAllocation vamos poniendo para cada RB el RNTI del usuario del que se ha realizado la asignación RACH
        }

      if (m_harqOn == true)
        {
          // generate UL-DCI for HARQ retransmissions
          UlDciListElement_s uldci;
          uldci.m_rnti = newRar.m_rnti;
          uldci.m_rbLen = rbLen;
          uldci.m_rbStart = rbStart;
          uldci.m_mcs = m_ulGrantMcs;
          uldci.m_tbSize = tbSizeBits / 8;
          uldci.m_ndi = 1;
          uldci.m_cceIndex = 0;
          uldci.m_aggrLevel = 1;
          uldci.m_ueTxAntennaSelection = 3; // antenna selection OFF
          uldci.m_hopping = false;
          uldci.m_n2Dmrs = 0;
          uldci.m_tpc = 0; // no power control
          uldci.m_cqiRequest = false; // only period CQI at this stage
          uldci.m_ulIndex = 0; // TDD parameter
          uldci.m_dai = 1; // TDD parameter
          uldci.m_freqHopping = 0;
          uldci.m_pdcchPowerOffset = 0; // not used

          uint8_t harqId = 0;
          std::map <uint16_t, uint8_t>::iterator itProcId;
          itProcId = m_ulHarqCurrentProcessId.find (uldci.m_rnti);
          if (itProcId == m_ulHarqCurrentProcessId.end ())
            {
              NS_FATAL_ERROR ("No info find in HARQ buffer for UE " << uldci.m_rnti);
            }
          harqId = (*itProcId).second;
          std::map <uint16_t, UlHarqProcessesDciBuffer_t>::iterator itDci = m_ulHarqProcessesDciBuffer.find (uldci.m_rnti);
          if (itDci == m_ulHarqProcessesDciBuffer.end ())
            {
              NS_FATAL_ERROR ("Unable to find RNTI entry in UL DCI HARQ buffer for RNTI " << uldci.m_rnti);
            }
          (*itDci).second.at (harqId) = uldci;
        }

      rbStart = rbStart + rbLen;
      ret.m_buildRarList.push_back (newRar); // Introducimos en el objeto que se enviará a la capa MAC la decisión de RBs contiguos (pos y leng), mcs, tbsize, para la situación del RACH en el UL
    }
  m_rachList.clear ();



  // Process DL HARQ feedback
  RefreshHarqProcesses ();
  // retrieve past HARQ retx buffered
  if (m_dlInfoListBuffered.size () > 0)
    {
      if (params.m_dlInfoList.size () > 0)
        {
          NS_LOG_INFO (this << " Received DL-HARQ feedback");
          m_dlInfoListBuffered.insert (m_dlInfoListBuffered.end (), params.m_dlInfoList.begin (), params.m_dlInfoList.end ());
        }
    }
  else
    {
      if (params.m_dlInfoList.size () > 0)
        {
          m_dlInfoListBuffered = params.m_dlInfoList;
        }
    }
  if (m_harqOn == false)
    {
      // Ignore HARQ feedback
      m_dlInfoListBuffered.clear ();
    }
  std::vector <struct DlInfoListElement_s> dlInfoListUntxed;
  for (uint16_t i = 0; i < m_dlInfoListBuffered.size (); i++)
    {
      std::set <uint16_t>::iterator itRnti = rntiAllocated.find (m_dlInfoListBuffered.at (i).m_rnti);
      if (itRnti != rntiAllocated.end ())
        {
          // RNTI already allocated for retx
          continue;
        }
      uint8_t nLayers = m_dlInfoListBuffered.at (i).m_harqStatus.size ();
      std::vector <bool> retx;
      NS_LOG_INFO (this << " Processing DLHARQ feedback");
      if (nLayers == 1)
        {
          retx.push_back (m_dlInfoListBuffered.at (i).m_harqStatus.at (0) == DlInfoListElement_s::NACK);
          retx.push_back (false);
        }
      else
        {
          retx.push_back (m_dlInfoListBuffered.at (i).m_harqStatus.at (0) == DlInfoListElement_s::NACK);
          retx.push_back (m_dlInfoListBuffered.at (i).m_harqStatus.at (1) == DlInfoListElement_s::NACK);
        }
      if (retx.at (0) || retx.at (1))
        {
          // retrieve HARQ process information
          uint16_t rnti = m_dlInfoListBuffered.at (i).m_rnti;
          uint8_t harqId = m_dlInfoListBuffered.at (i).m_harqProcessId;
          NS_LOG_INFO (this << " HARQ retx RNTI " << rnti << " harqId " << (uint16_t)harqId);
          std::map <uint16_t, DlHarqProcessesDciBuffer_t>::iterator itHarq = m_dlHarqProcessesDciBuffer.find (rnti);
          if (itHarq == m_dlHarqProcessesDciBuffer.end ())
            {
              NS_FATAL_ERROR ("No info find in HARQ buffer for UE " << rnti);
            }

          DlDciListElement_s dci = (*itHarq).second.at (harqId);
          int rv = 0;
          if (dci.m_rv.size () == 1)
            {
              rv = dci.m_rv.at (0);
            }
          else
            {
              rv = (dci.m_rv.at (0) > dci.m_rv.at (1) ? dci.m_rv.at (0) : dci.m_rv.at (1));
            }

          if (rv == 3)
            {
              // maximum number of retx reached -> drop process
              NS_LOG_INFO ("Maximum number of retransmissions reached -> drop process");
              std::map <uint16_t, DlHarqProcessesStatus_t>::iterator it = m_dlHarqProcessesStatus.find (rnti);
              if (it == m_dlHarqProcessesStatus.end ())
                {
                  NS_LOG_ERROR ("No info find in HARQ buffer for UE (might change eNB) " << m_dlInfoListBuffered.at (i).m_rnti);
                }
              (*it).second.at (harqId) = 0;
              std::map <uint16_t, DlHarqRlcPduListBuffer_t>::iterator itRlcPdu =  m_dlHarqProcessesRlcPduListBuffer.find (rnti);
              if (itRlcPdu == m_dlHarqProcessesRlcPduListBuffer.end ())
                {
                  NS_FATAL_ERROR ("Unable to find RlcPdcList in HARQ buffer for RNTI " << m_dlInfoListBuffered.at (i).m_rnti);
                }
              for (uint16_t k = 0; k < (*itRlcPdu).second.size (); k++)
                {
                  (*itRlcPdu).second.at (k).at (harqId).clear ();
                }
              continue;
            }
          // check the feasibility of retransmitting on the same RBGs
          // translate the DCI to Spectrum framework
          std::vector <int> dciRbg;
          uint32_t mask = 0x1;
          NS_LOG_INFO ("Original RBGs " << dci.m_rbBitmap << " rnti " << dci.m_rnti);
          for (int j = 0; j < 32; j++) // m_rbBitmap es un entero de 32
            {
              if (((dci.m_rbBitmap & mask) >> j) == 1)
                {
                  dciRbg.push_back (j);
                  NS_LOG_INFO ("\t" << j);
                }
              mask = (mask << 1);
            }
          bool free = true;
          for (uint8_t j = 0; j < dciRbg.size (); j++)
            {
              if (m_rbgMap.at (dciRbg.at (j)) == true)
                {
                  free = false;
                  break;
                }
            }
          if (free)
            {
              // use the same RBGs for the retx
              // reserve RBGs
              for (uint8_t j = 0; j < dciRbg.size (); j++)
                {
                  m_rbgMap.at (dciRbg.at (j)) = true;
                  NS_LOG_INFO ("RBG " << dciRbg.at (j) << " assigned");
                  rbgAllocatedNum++;
                }

              NS_LOG_INFO (this << " Send retx in the same RBGs");
            }
          else
            {
              // find RBGs for sending HARQ retx
              uint8_t j = 0;
              uint8_t rbgId = (dciRbg.at (dciRbg.size () - 1) + 1) % rbgNum;
              uint8_t startRbg = dciRbg.at (dciRbg.size () - 1);
              std::vector <bool> m_rbgMapCopy = m_rbgMap;
              while ((j < dciRbg.size ())&&(startRbg != rbgId))
                {
                  if (m_rbgMapCopy.at (rbgId) == false)
                    {
                      m_rbgMapCopy.at (rbgId) = true;
                      dciRbg.at (j) = rbgId;
                      j++;
                    }
                  rbgId = (rbgId + 1) % rbgNum;
                }
              if (j == dciRbg.size ())
                {
                  // find new RBGs -> update DCI map
                  uint32_t rbgMask = 0;
                  for (uint16_t k = 0; k < dciRbg.size (); k++)
                    {
                      rbgMask = rbgMask + (0x1 << dciRbg.at (k));
                      rbgAllocatedNum++;
                    }
                  dci.m_rbBitmap = rbgMask;
                  m_rbgMap = m_rbgMapCopy;
                  NS_LOG_INFO (this << " Move retx in RBGs " << dciRbg.size ());
                }
              else
                {
                  // HARQ retx cannot be performed on this TTI -> store it
                  dlInfoListUntxed.push_back (m_dlInfoListBuffered.at (i));
                  NS_LOG_INFO (this << " No resource for this retx -> buffer it");
                }
            }
          // retrieve RLC PDU list for retx TBsize and update DCI
          BuildDataListElement_s newEl;
          std::map <uint16_t, DlHarqRlcPduListBuffer_t>::iterator itRlcPdu =  m_dlHarqProcessesRlcPduListBuffer.find (rnti);
          if (itRlcPdu == m_dlHarqProcessesRlcPduListBuffer.end ())
            {
              NS_FATAL_ERROR ("Unable to find RlcPdcList in HARQ buffer for RNTI " << rnti);
            }
          for (uint8_t j = 0; j < nLayers; j++)
            {
              if (retx.at (j))
                {
                  if (j >= dci.m_ndi.size ()) // ndi --> new data indicator
                    {
                      // for avoiding errors in MIMO transient phases
                      dci.m_ndi.push_back (0);
                      dci.m_rv.push_back (0);
                      dci.m_mcs.push_back (0);
                      dci.m_tbsSize.push_back (0);
                      NS_LOG_INFO (this << " layer " << (uint16_t)j << " no txed (MIMO transition)");
                    }
                  else
                    {
                      dci.m_ndi.at (j) = 0;
                      dci.m_rv.at (j)++;
                      (*itHarq).second.at (harqId).m_rv.at (j)++;
                      NS_LOG_INFO (this << " layer " << (uint16_t)j << " RV " << (uint16_t)dci.m_rv.at (j));
                    }
                }
              else
                {
                  // empty TB of layer j
                  dci.m_ndi.at (j) = 0;
                  dci.m_rv.at (j) = 0;
                  dci.m_mcs.at (j) = 0;
                  dci.m_tbsSize.at (j) = 0;
                  NS_LOG_INFO (this << " layer " << (uint16_t)j << " no retx");
                }
            }
          for (uint16_t k = 0; k < (*itRlcPdu).second.at (0).at (dci.m_harqProcess).size (); k++)
            {
              std::vector <struct RlcPduListElement_s> rlcPduListPerLc;
              for (uint8_t j = 0; j < nLayers; j++)
                {
                  if (retx.at (j))
                    {
                      if (j < dci.m_ndi.size ())
                        {
                          rlcPduListPerLc.push_back ((*itRlcPdu).second.at (j).at (dci.m_harqProcess).at (k));
                        }
                    }
                }

              if (rlcPduListPerLc.size () > 0)
                {
                  newEl.m_rlcPduList.push_back (rlcPduListPerLc);
                }
            }
          newEl.m_rnti = rnti;
          newEl.m_dci = dci;
          (*itHarq).second.at (harqId).m_rv = dci.m_rv;
          // refresh timer
          std::map <uint16_t, DlHarqProcessesTimer_t>::iterator itHarqTimer = m_dlHarqProcessesTimer.find (rnti);
          if (itHarqTimer== m_dlHarqProcessesTimer.end ())
            {
              NS_FATAL_ERROR ("Unable to find HARQ timer for RNTI " << (uint16_t)rnti);
            }
          (*itHarqTimer).second.at (harqId) = 0;
          ret.m_buildDataList.push_back (newEl);
          rntiAllocated.insert (rnti);
        }
      else
        {
          // update HARQ process status
          NS_LOG_INFO (this << " HARQ received ACK for UE " << m_dlInfoListBuffered.at (i).m_rnti);
          std::map <uint16_t, DlHarqProcessesStatus_t>::iterator it = m_dlHarqProcessesStatus.find (m_dlInfoListBuffered.at (i).m_rnti);
          if (it == m_dlHarqProcessesStatus.end ())
            {
              NS_FATAL_ERROR ("No info find in HARQ buffer for UE " << m_dlInfoListBuffered.at (i).m_rnti);
            }
          (*it).second.at (m_dlInfoListBuffered.at (i).m_harqProcessId) = 0;
          std::map <uint16_t, DlHarqRlcPduListBuffer_t>::iterator itRlcPdu =  m_dlHarqProcessesRlcPduListBuffer.find (m_dlInfoListBuffered.at (i).m_rnti);
          if (itRlcPdu == m_dlHarqProcessesRlcPduListBuffer.end ())
            {
              NS_FATAL_ERROR ("Unable to find RlcPdcList in HARQ buffer for RNTI " << m_dlInfoListBuffered.at (i).m_rnti);
            }
          for (uint16_t k = 0; k < (*itRlcPdu).second.size (); k++)
            {
              (*itRlcPdu).second.at (k).at (m_dlInfoListBuffered.at (i).m_harqProcessId).clear ();
            }
        }
    }
  m_dlInfoListBuffered.clear ();
  m_dlInfoListBuffered = dlInfoListUntxed;

  //////////////// EMPIEZA SCHEDULER DE DATOS ////////////////
  // Finalizado el proceso de retx HARQ. En caso de haber alcanzado el numero de subbandas maximo, pasamos la decision del scheduler a la capa MAC y terminamos.
  if (rbgAllocatedNum == rbgNum)
    {
      // all the RBGs are already allocated -> exit
      if ((ret.m_buildDataList.size () > 0) || (ret.m_buildRarList.size () > 0))
        {
          m_schedSapUser->SchedDlConfigInd (ret); // Pasa la decisión de scheduling DL a la capa MAC
        }
      return;
    }

  // Abrimos fichero para escribir resultados de scheduler
  if (m_generateFiles)
  {
	  m_pSchedFile = fopen ("Distributed_Scheduler_Results_DL.txt","a");
	  m_pMatrizCQIFile=fopen("DistSched_Matriz_CQI_DL.txt","a");
	  m_pMatrizMetricasFile=fopen("DistSched_Matriz_Metricas_DL.txt","a");
	  m_pSchedFileDet = fopen ("DistScheduler_Info_Results_DL.txt","a");
  }


// Asignación de subbandas a usuarios: Vamos a distinguir entre 2 scheduler diferentes que se van a llevar a cabo

  // Comprobamos si la celda considerada tiene usuarios attachados
  bool uesON = false;
  if (m_flowStatsDl.size() > 0)
	  uesON = true;

  if (uesON) //&& params.m_sfnSf <= 57)
  {

	  // 1. Scheduler simple de usuarios cuando no hay informacion de CQI: A partir de la trama 3 subtrama 4 hay usuarios (RACH realizado, RNTI asignado)
	  // pero no hay informacion de CQI. Realizamos un scheduler simple en el que asignamos a cada usuario una subtrama. Contabilizamos esto en el tp medio.

	  if (m_a30CqiRxed.size()==0)
	  {
	  	  // Imprimimos Trama y subtrama considerada
		  if (m_generateFiles)
		  {
		      fprintf(m_pSchedFile,"--------- Trama %d Subtrama %d , Celda % d ---------\n", params.m_sfnSf >> 4, 0xF & params.m_sfnSf , params.m_cellId);
		      fprintf(m_pSchedFileDet,"--------- Trama %d Subtrama %d , Celda % d ---------\n", params.m_sfnSf >> 4, 0xF & params.m_sfnSf , params.m_cellId);
			  fprintf(m_pMatrizCQIFile,"--------- Trama %d Subtrama %d, Celda %d ---------\n", params.m_sfnSf >> 4, 0xF & params.m_sfnSf,params.m_cellId);
			  fprintf(m_pMatrizMetricasFile,"--------- Trama %d Subtrama %d , Celda %d ---------\n", params.m_sfnSf >> 4, 0xF & params.m_sfnSf, params.m_cellId);

		  }

        std::cout<< "No info CQI" << std::endl;


//          std::map <uint16_t, std::vector<int> > uesNext;
          std::vector <int> uesNext;
		  // Identificamos los UEs attachados y que tienen datos para tx
		  std::map <uint16_t, schedulerDRANFlowPerf_t>::iterator itUEs; // Mapa para recoger los UEs
		  int cont=0;
		  for (itUEs = m_flowStatsDl.begin(); itUEs!=m_flowStatsDl.end(); itUEs++)
		  {
			  if (LcActivePerFlow((*itUEs).first)!=0)
			  {
				  cont++;
//				  uesNext.insert(std::pair <uint16_t, std::vector <int> > ((*itUEs).first, 0));
				  uesNext.push_back((*itUEs).first);
			  }

		  }

		  // Reparto de RBs entre los UEs activos y con datos a tx
		  int rbgPerTb = (cont > 0) ? ((rbgNum - rbgAllocatedNum) / cont) : 0x7FFFFFFF; // Obtenemos las subbandas por UE de entre las subbandas disponibles
		  if (rbgPerTb == 0)
		    {
		      rbgPerTb = 1;    // al menos 1 rb por TB (till available resource)
		    }

		  // Asignacion de UE
		  std::vector <uint16_t> temp;
	      std::map <uint16_t, std::vector <uint16_t> >::iterator itAlloc;
		  int rbs = 0;
		  int rbgAllocated=0;
		  for (int i=0; i<cont; i++)
		  {
			  int nextRNTI = uesNext.at(i);
		      while (rbs < rbgPerTb)
		        {

		          if (m_rbgMap.at (rbgAllocated) == false)
		            {
		              rbs++; // Contador de subbandas asignadas al UE
		              m_rbgMap.at (rbgAllocated) = true;
		              rbgAllocatedNum++;
		            }
		          temp.push_back(rbgAllocated);
		          rbgAllocated++;
		        }
		      itAlloc = m_dlAllocationMap.find(nextRNTI);
		      if (itAlloc == m_dlAllocationMap.end())
		      {
		          m_dlAllocationMap.insert (std::pair <uint16_t, std::vector <uint16_t> > (nextRNTI, temp));
		      }
		      else
		      {
		    	  (*itAlloc).second = temp;

		      }
			  // Imprimimos usuario asignado y metrica por pantalla y fichero
			  std::cout << "UE assigned " << nextRNTI << "in subbands:  " << temp.front() << " to " << temp.back() << std::endl;
			  if (m_generateFiles)
			  {
				  fprintf(m_pSchedFile, "Usuario asignado: %d Metrica: %s en subbandas: %d a %d\n", nextRNTI, " NO CQI", temp.front(), temp.back());
	        	  fprintf(m_pSchedFileDet, "Usuario asignado: %d Metrica: %s en subbandas: %d a %d\n", nextRNTI, " NO CQI", temp.front(), temp.back());
				  fprintf(m_pMatrizCQIFile, "NO CQI -> Asignacion RR \n");
				  fprintf(m_pMatrizMetricasFile, "NO CQI -> Asignacion RR \n");
			  }


			  // Reseteamos temp y cuenta de rbs
			  temp.erase(temp.begin(),temp.end());
			  rbs=0;

		  }
		  if (cont == 0) // No UEs con datos a tx
		  {

			  std::cout << " -> UEs sin datos para tx " << std::endl;
			  if (m_generateFiles)
			  {
				  fprintf(m_pSchedFile, "UEs sin datos para tx \n");
				  fprintf(m_pSchedFileDet, "UEs sin datos para tx \n");
				  fprintf(m_pMatrizCQIFile, "NO CQI + UEs sin datos para tx \n");
				  fprintf(m_pMatrizMetricasFile, "NO CQI + UEs sin datos para tx \n");
			  }


		  }
		  // Cerramos ficheros de resultados
		  if (m_generateFiles)
		  {
			  fclose(m_pSchedFile);
			  fclose(m_pSchedFileDet);
			  fclose(m_pMatrizCQIFile);
			  fclose(m_pMatrizMetricasFile);
		  }

	  }



  // 2. Scheduler distribuido basado en metricas Rij: A partir de la trama 3 subtrama 9 ya hay informacion de CQI, en este caso empleamos el scheduler
  // distribuido realizado en el que asignaremos las subbandas al usuario que tenga una mejor metrica.
//  if (uesON && m_a30CqiRxed.size()>0)
	  if (m_a30CqiRxed.size()>0)
	  {
		  // Calculo metricas
		  SetMetricasRij(rbgNum, rbgSize);

		  // Obtenemos el instante de tiempo
		  int now = Simulator::Now().GetMilliSeconds();
		  if (m_generateFiles)
		  {
			  // Imprimimos Trama y subtrama considerada
			  fprintf(m_pSchedFile,"--------- Trama %d Subtrama %d , Celda %d ---------", params.m_sfnSf >> 4, 0xF & params.m_sfnSf, params.m_cellId);
			  fprintf(m_pSchedFileDet,"--------- Trama %d Subtrama %d , Celda %d ---------\n", params.m_sfnSf >> 4, 0xF & params.m_sfnSf, params.m_cellId);
			  fprintf(m_pMatrizCQIFile,"--------- Trama %d Subtrama %d, Celda %d , Instante de tiempo: %d ---------\n", params.m_sfnSf >> 4, 0xF & params.m_sfnSf,params.m_cellId,now);
			  fprintf(m_pMatrizMetricasFile,"--------- Trama %d Subtrama %d , Celda %d ---------\n", params.m_sfnSf >> 4, 0xF & params.m_sfnSf, params.m_cellId);

			  // Fichero Metricas y CQI imprimimos cabecera
			  fprintf(m_pMatrizCQIFile,"   Sub\t");
		      fprintf(m_pMatrizMetricasFile,"   Sub\t");

			  for (int i=0; i<rbgNum; i++)
			  {
				 fprintf(m_pMatrizCQIFile,"  %d\t",i);
				 fprintf(m_pMatrizMetricasFile,"  %d\t",i);


			  }
			  fprintf(m_pMatrizCQIFile,"\n");
			  fprintf(m_pMatrizMetricasFile,"\n");

			  // Imprimimos en el fichero CQI los valores de CQI UE-Subbanda
			  std::map <uint16_t, SbMeasResult_s>::iterator it;
			  for(it=m_a30CqiRxed.begin(); it!=m_a30CqiRxed.end(); it++)
			  {

				  fprintf(m_pMatrizCQIFile,"   UE %d\t",(*it).first);
				  for (int j=0; j<rbgNum;j++)
				  {
					  fprintf(m_pMatrizCQIFile, "  %d\t",(*it).second.m_higherLayerSelected.at(j).m_sbCqi.at(0)); // Solo un cqi porque solo hay una layer
				  }

				  fprintf(m_pMatrizCQIFile,"\n");
			  }
			  fprintf(m_pMatrizCQIFile,"---------------------------------------------------------------------------------\n");


			  // Imprimimos metricas Rij UE-subbanda en fichero
			  std::map <uint16_t, metricas_Rij >::iterator itRij;
			  for(itRij=m_metricasUEs.begin(); itRij!=m_metricasUEs.end(); itRij++)
			  {

				fprintf(m_pMatrizMetricasFile,"   UE%d\t",(*itRij).first);


				for (int j=0; j<rbgNum;j++)
				{
					fprintf(m_pMatrizMetricasFile, " %g\t",(*itRij).second.Rij.at(j)); // Solo un cqi porque solo hay una layer

				}
				fprintf(m_pMatrizMetricasFile,"\n");

			  }
			  fprintf(m_pMatrizMetricasFile,"---------------------------------------------------------------------------------\n");

			  // Cerramos los ficheros CQI y metricas
			  fclose(m_pMatrizCQIFile);
			  fclose(m_pMatrizMetricasFile);
		  }


		  // Seleccionamos aleatoriamente la subbanda de comienzo
		  int Ksubb=rand()%rbgNum;
//		  int Ksubb;
		  // Comenzamos asignacion UEs-Subbandas
		  for (int i = 0; i < rbgNum; i++)
			{
//			  Ksubb=i;
			  // Comprobamos si la subbanda seleccionada es mayor que el numero de maximo de subbandas en ese caso reseteamos
			  if (Ksubb>rbgNum-1)
			  {
				  Ksubb=0;
			  }
			  NS_LOG_INFO (this << " ALLOCATION for RBG " << Ksubb << " of " << rbgNum);
			  std::cout << " ALLOCATION for subband " << Ksubb << " of " << rbgNum << std::endl;
			  if (m_generateFiles)
			     fprintf(m_pSchedFileDet, "** Subbanda %d **\n", Ksubb);
			  if (m_rbgMap.at (Ksubb) == false) // Hay RBG's libres, subbanda disponible
				{
//				  fprintf(m_pSchedFile, "Disponible");
		          std::map <uint16_t, schedulerDRANFlowPerf_t>::iterator it;
//				  std::map <uint16_t,SbMeasResult_s>::iterator it;
				  std::map <uint16_t, schedulerDRANFlowPerf_t>::iterator itMax = m_flowStatsDl.end ();
				  std::map <uint16_t, metricas_Rij >::iterator itMetricas; // Iterator para recorrer el mapa que contiene las metricas (por subbanda) para cada usuario
				  std::map <uint16_t, uint16_t>::iterator itNSubbAsig; // Iterator para recorrer los usuarios y obtener el numero de subbandas que tiene asignadas
				  double rcqiMaxSubbK = 0.0;
				  double rcqi_k = 0.0;
				  // Recorremos todos los UEs, si los UEs tienen datos a transmitir hacemos asignacion buscando el ue con mayor tp
				  for (it = m_flowStatsDl.begin (); it != m_flowStatsDl.end (); it++) // Todos los UEs que existen (estan atachados a la celda)
//				  for (it = m_a30CqiRxed.begin(); it != m_a30CqiRxed.end(); it++)
					{
					  if ((m_ffrSapProvider->IsDlRbgAvailableForUe (Ksubb, (*it).first)) == false) // true si un UE puede ser servido en la i-th RBG/Subband, false otherwise (based on frequency reuse policy)
						continue;

					  // Comprobamos si el usuario tiene asignadas el maximo de subbandas
					  itNSubbAsig=m_subbandsAsigUEs.find((*it).first);
					  if (itNSubbAsig==m_subbandsAsigUEs.end())
					  {
						  // Añadimos UE al mapa y ponemos contador de subbandas a cero
						  m_subbandsAsigUEs.insert (std::pair <uint16_t, uint16_t > ((*it).first, 0));
					  }
					  else
					  {
						  if((*itNSubbAsig).second == m_Nsubbands_user)
							  continue;
					  }

					  // Buscamos si el usuario ya esta asignado (retx HARQ o sin proceso HARQ disponible)
					  std::set <uint16_t>::iterator itRnti = rntiAllocated.find ((*it).first);
					  if ((itRnti != rntiAllocated.end ())||(!HarqProcessAvailability ((*it).first)))
						{
						  // UE already allocated for HARQ or without HARQ process available -> drop it
						  if (itRnti != rntiAllocated.end ())
							{
							  NS_LOG_DEBUG (this << " RNTI discared for HARQ tx" << (uint16_t)(*it).first);
							  std::cout << "Usuario " << (*it).first << "asignado" << std::endl;
							}
						  if (!HarqProcessAvailability ((*it).first))
							{
							  NS_LOG_DEBUG (this << " RNTI discared for HARQ id" << (uint16_t)(*it).first);
							}
						  continue;
						}
					  std::map <uint16_t, schedulerDRANFlowPerf_t>::iterator itStats; // Mapa para recoger las estadisticas de los UEs
		              std::map <uint16_t,SbMeasResult_s>::iterator itCqi;
		              std::map <uint16_t,uint8_t>::iterator itTxMode;
		              itTxMode = m_uesTxMode.find ((*it).first);
		              if (itTxMode == m_uesTxMode.end ())
		                {
		                  NS_FATAL_ERROR ("No Transmission Mode info on user " << (*it).first);
		                }
		              int nLayer = TransmissionModesLayers::TxMode2LayerNum ((*itTxMode).second);
		              double rcqi=0.0;
		              std::vector <uint8_t> sbCqi;
					  itStats=m_flowStatsDl.find((*it).first);
		              itCqi = m_a30CqiRxed.find ((*it).first);
                      if (itCqi == m_a30CqiRxed.end ())
                         {
                           for (uint8_t k = 0; k < nLayer; k++)
                             {
                               sbCqi.push_back (1);  // start with lowest value
                               // this UE has data to transmit
                             }
                         }

                      else
                      {
                    	  sbCqi = (*itCqi).second.m_higherLayerSelected.at (Ksubb).m_sbCqi;

                      }

                      uint8_t cqi1 = sbCqi.at (0);
                      uint8_t cqi2 = 1;
                      if (sbCqi.size () > 1)
                        {
                          cqi2 = sbCqi.at (1);
                        }

                      if ((cqi1 > 0)||(cqi2 > 0)) // CQI == 0 means "out of range" (see table 7.2.3-1 of 36.213)
                      {
						  if (LcActivePerFlow ((*it).first) > 0)
						  {
							  itMetricas=m_metricasUEs.find((*it).first);
		                	  rcqi=(*itMetricas).second.Rij.at(Ksubb);
							  std::cout << " RNTI " << (*it).first << " MCS " << (uint32_t)(*itMetricas).second.mcs.at(Ksubb) << " achievableRate " << (*itMetricas).second.achievableRate.at(Ksubb)<< " avgThr " << (*itStats).second.lastAveragedThroughput << " Métrica " << (*itMetricas).second.Rij.at(Ksubb) << std::endl;
							  if (m_generateFiles)
							      fprintf(m_pSchedFileDet, "RNTI %d\t MCS %d\t Achievable Rate %f\t Avg Throughput %f\t Metrica Rij %f\n", (*it).first, (*itMetricas).second.mcs.at(Ksubb),  (*itMetricas).second.achievableRate.at(Ksubb), (*itStats).second.lastAveragedThroughput, (*itMetricas).second.Rij.at(Ksubb));

							  if (rcqi > rcqiMaxSubbK) // Busca usuario con métrica más alta en esa subbanda
								{
								  // Recogemos ese nuevo maximo
								  rcqiMaxSubbK = rcqi;
								  itMax = itStats;
								  rcqi_k = rcqiMaxSubbK;
								}
							  else if (rcqi == rcqiMaxSubbK) // si los usuarios tienen igual metrica, sortear aleatoriamente
							  {
								  int r=rand()%2; // valor aleatorio entre 0, 1 para seleccionar que usuario escoger
								  switch(r)
								  {
									 case 0:
										 itMax=itStats;
										 break;
									 case 1:
										 break;
										 // itMax= itMax;
								  }

							  }
						  }
                      }
	               } // end bucle usuarios

				  if (itMax == m_flowStatsDl.end ())
					{
					  // no UE available for this RB
					  NS_LOG_INFO (this << " any UE found");
					  std::cout << "No UE available for this RB " << std::endl;
					  if (m_generateFiles)
					  {
						  fprintf(m_pSchedFile, "\n Subband %d -> No UE disponibles para esta subbanda (maximo de subbandas asignadas) ", Ksubb);
						  fprintf(m_pSchedFileDet, "Subband %d -> No UE disponibles para esta subbanda (maximo de subbandas asignadas)\n", Ksubb);
					  }
					}
				  else if(m_Nsubbands_user < rbgNum)
					{
					  // Comprobamos si existe otra subbanda "l" distinta de "k" en la que se cumpla lo siguiente:
					  // 1- No tenga ningun usuario asignado
					  // 2- Tjl > Tjk o Rjl > Rjk
					  // 3- Usuario j sea el usuario con maximo Througput en la subbanda "l" / o metrica mas alta en la subbanda "l"

					  std::map <uint16_t, std::vector <uint16_t> >::iterator itSubbBusc; // Mapa que almacena subbandas - ues
					  int Lsubb = 0; // posible subbanda en la que se puede obtener una mejor metrica para el UE j
					  double rcqi_l = -1.0;
					  int ueSubbL = 0;
					  bool subbandaMejor = false;

					  double rcqi2MaxSubbL = -1.0;
					  double rcqi2MaxSubbK = -1.0;
					  int ue2MaxSubbL = 0;
					  int ue2MaxSubbK = 0;
					  // Recorremos todas las subbandas
					  for (int Subb=0; Subb < rbgNum; Subb++)
					  {
						  if (Subb == Ksubb)
							  continue;
						  else
						  {
							  // Comprobamos si la subbanda L ha sido ya asignada
							  itSubbBusc=m_subbandasUeMap.find(Subb);
							  if (itSubbBusc == m_subbandasUeMap.end())
							  {
								  //  Si no, Obtenemos la subbanda en la que se encuentre el 2 mayor maximo para el UE j
								  itMetricas = m_metricasUEs.find((*itMax).first);
								  if ((*itMetricas).second.Rij.at(Subb) > rcqi_l)
								  {
									  rcqi_l=(*itMetricas).second.Rij.at(Subb);
									  ueSubbL=(*itMax).first;
									  Lsubb=Subb;
								  }
								  else if ((*itMetricas).second.Rij.at(Subb) == rcqi_l && rcqi_l!=-1)
								  {
									  // Sorteamos aleatoriamente la subbanda a escoger
									  int r=rand()%2; // valor aleatorio entre 0, 1 para seleccionar que subbanda escoger
									  switch(r)
									  {
										 case 0:
											 Lsubb=Subb;
		  								     break;
										 case 1:
											 break;
											 // Lsubb=Lsubb;
									  }
								  }

							  }
							  else // Si ya ha sido asignada saltamos
								  continue;
						  }
					  }

					  // Comprobamos que hay una subbanda con una metrica mas alta que subbanda k
					  if (rcqi_l > rcqi_k)
						  subbandaMejor = true;

					  if (subbandaMejor) // Existe una subbanda L mejor que K y no esta asignada
					  {
						  // Comprobamos si para la subbanda L obtenida el UE j es tambien el UE con mayor metrica
						  for (itMetricas = m_metricasUEs.begin(); itMetricas != m_metricasUEs.end(); itMetricas++)
						  {
							  if ((*itMetricas).second.Rij.at(Lsubb) > rcqi_l)
							  {
								  rcqi_l = (*itMetricas).second.Rij.at(Lsubb);
								  ueSubbL = (*itMetricas).first;
							  }

						  }
						  if (ueSubbL == (*itMax).first) // UE j tambien es el UE con max metrica en subbanda L
						  {
							  // Estudiamos los segundos maximos
							  for (itMetricas = m_metricasUEs.begin(); itMetricas!=m_metricasUEs.end(); itMetricas++)
							  {
								  itNSubbAsig=m_subbandsAsigUEs.find((*itMetricas).first);
								  if ((*itNSubbAsig).second == m_Nsubbands_user)
									  continue;
								  if ((*itMetricas).first != (*itMax).first && (*itMetricas).second.Rij.at(Ksubb) > rcqi2MaxSubbK)
								  {
									  rcqi2MaxSubbK = (*itMetricas).second.Rij.at(Ksubb);
									  ue2MaxSubbK = (*itMetricas).first;
								  }
								  if ((*itMetricas).first != (*itMax).first && (*itMetricas).second.Rij.at(Lsubb) > rcqi2MaxSubbL)
								  {
									  rcqi2MaxSubbL = (*itMetricas).second.Rij.at(Lsubb);
									  ue2MaxSubbL = (*itMetricas).first;
								  }

							  }

							  // Comprobamos que combinacion de metricas produce un mayor tp
							  if (rcqi2MaxSubbK + rcqi_l > rcqi2MaxSubbL + rcqi_k && (ue2MaxSubbK > 0 && ue2MaxSubbL > 0)) // UEs 2maximos tienen que tener un RNTI valido, al inicio estan inicializados a 0
							  {
								  // Asignamos UE j a subbanda L y UE j1 a subbanda K
								  // Usuario j:
								  AssignUe((*itMax).first,Lsubb);

								  // Asignamos usuario j1
								  AssignUe(ue2MaxSubbK,Ksubb);

								  // Actualizamos numero de subbandas asignadas
								  rbgAllocatedNum+=2;

								  // Imprimimos usuarios asignados en las subbandas y metricas por pantalla y fichero
								  std::cout << "UE assigned " << ue2MaxSubbK << "in subband " << Ksubb << "Metrica " << rcqi2MaxSubbK << std::endl;
								  std::cout << "UE assigned " << (*itMax).first << "in subband " << Lsubb << "Metrica " << rcqi_l << std::endl;
								  if (m_generateFiles)
								  {
									  fprintf(m_pSchedFile,"\n Subbanda %d -> Usuario asignado: %d; Metrica: %f", Ksubb, ue2MaxSubbK, rcqi2MaxSubbK);
									  fprintf(m_pSchedFile,"\n Subbanda %d -> Usuario asignado: %d; Metrica: %f", Lsubb, (*itMax).first, rcqi_l);
								  }

							  }
							  else if(ue2MaxSubbK > 0 && ue2MaxSubbL > 0) // UEs 2maximos tienen que tener un RNTI valido, al inicio estan inicializados a 0
							  {
								  // Asignamos UE j a subbanda K y UE j2 a subbanda L
								  // Usuario j:
								  AssignUe((*itMax).first,Ksubb);

								   //Asignamos usuario j2
								  AssignUe(ue2MaxSubbL,Lsubb);

								  // Actualizamos numero de subbandas asignadas
								  rbgAllocatedNum+=2;

								  // Imprimimos usuarios asignados en las subbandas y metricas por pantalla y fichero
								  std::cout << "UE assigned " << (*itMax).first << "in subband " << Ksubb << "Metrica " << rcqi_k << std::endl;
								  std::cout << "UE assigned " << ue2MaxSubbL << "in subband " << Lsubb << "Metrica " << rcqi2MaxSubbL << std::endl;
								  if (m_generateFiles)
								  {
									  fprintf(m_pSchedFile,"\n Subbanda %d -> Usuario asignado: %d; Metrica: %f", Ksubb, (*itMax).first, rcqi_k);
									  fprintf(m_pSchedFile,"\n Subbanda %d -> Usuario asignado: %d; Metrica: %f", Lsubb, ue2MaxSubbL, rcqi2MaxSubbL);
								  }

							  }// End if Rjl + Rj1K > Rj2l + Rjk
							  else // Si no, algunos de los UEs 2os max son 0 -> indica que no hay usuarios con 2os maximos, entonces asignamos UE j a subbanda K
							  {

								  // Asignamos UE j -> subband k
									// 1º Asignamos usuario j
								  AssignUe((*itMax).first,Ksubb);

								  // Actualizamos numero de subbandas asignadas
								  rbgAllocatedNum++;

								  // Imprimimos usuarios asignados en las subbandas y metricas por pantalla y fichero
								  std::cout << "UE assigned " << (*itMax).first << "in subband " << Ksubb << "Metrica " << rcqi_k << std::endl;
								  if (m_generateFiles)
								     fprintf(m_pSchedFile,"\n Subbanda %d -> Usuario asignado: %d; Metrica: %f", Ksubb, (*itMax).first, rcqi_k);

							  }

						  }
						  else // Si UE j no es el maximo en la subbanda L
						  {
							  // Asignamos UE j -> subband k
								// 1º Asignamos usuario j
							  AssignUe((*itMax).first,Ksubb);

							  // Actualizamos numero de subbandas asignadas
							  rbgAllocatedNum++;

							  // Imprimimos usuarios asignados en las subbandas y metricas por pantalla y fichero
							  std::cout << "UE assigned " << (*itMax).first << "in subband " << Ksubb << "Metrica " << rcqi_k << std::endl;
							  if (m_generateFiles)
							     fprintf(m_pSchedFile,"\n Subbanda %d -> Usuario asignado: %d; Metrica: %f", Ksubb, (*itMax).first, rcqi_k);

						  }// End if UE max subbanda L = j
					  }

					  else
					  {
						  // No hay una subbanda mejor -> Asignamos UE j a subbanda K
							// 1º Asignamos usuario j
						  AssignUe((*itMax).first,Ksubb);

						  // Actualizamos numero de subbandas asignadas
						  rbgAllocatedNum++;

						  // Imprimimos usuarios asignados en las subbandas y metricas por pantalla y fichero
						  std::cout << "UE assigned " << (*itMax).first << "in subband " << Ksubb << "Metrica " << rcqi_k << std::endl;
						  if (m_generateFiles)
						     fprintf(m_pSchedFile,"\n Subbanda %d -> Usuario asignado: %d; Metrica: %f", Ksubb, (*itMax).first, rcqi_k);

					  }// End if subbanda mejor



					} // end if exist UE available in this RB -> Distribuited Sched
				  else
				  {
					  // Aplicamos PF
					  // Asignamos UE j a subbanda K
					  AssignUe((*itMax).first, Ksubb);

					  // Actualizamos numero de subbandas asignadas
					  rbgAllocatedNum++;

					  // Imprimimos usuarios asignados en las subbandas y metricas por pantalla y fichero
					  std::cout << "UE assigned " << (*itMax).first << "in subband " << Ksubb << "Metrica " << rcqi_k << std::endl;
					  if (m_generateFiles)
						 fprintf(m_pSchedFile,"\n Subbanda %d -> Usuario asignado: %d; Metrica: %f", Ksubb, (*itMax).first, rcqi_k);
				  }

				} // end for if RBG free

			  else // Subbanda no disponible
			  {
				  std::cout << "Subbanda no disponible " << std::endl;
				  if (m_generateFiles)
				     fprintf(m_pSchedFile, "\n Subbanda no disponible, asignada a otro usuario");
			  }

			  // Siguiente subbanda
			  Ksubb++;

			} // end bucle subbandas/RBGs


		  // Imprimimos el numero de subbandas que han sido asignadas
		  std::cout << "Subbandas asignadas: " << rbgAllocatedNum << std::endl;
		  if (m_generateFiles)
		  {
			  fprintf(m_pSchedFile, "\nSubbandas asignadas: %d \n",rbgAllocatedNum);
	//		  fprintf(m_pSchedFileDet, "------------------------------------------------------------------------\n");

			  // Cerramos ficheros de resultados
			  fclose(m_pSchedFile);
			  fclose(m_pSchedFileDet);
		  }


	  } // End if existen CQIs (Scheduler distribuido)

  }// End if uesON, 2 schedulers

  // Del bucle anterior obtenemos: usuario (itmax) con mayor métrica (rcqiMax) en subbanda (0)...(N-1)

  // Esta parte del codigo se añade para identificar las celdas que no tienen usuarios pero que ejecutan el scheduler de datos
  // A partir de la trama 3 y subtrama 4 de los UEs acampados en la celda algunos/todos tienen datos para transmitir, si a partir de esta trama no se ha realizado el RACH allocation es porque no hay usuarios en esta celda
//  if (params.m_sfnSf >= 52 && uesON==false)
  if (uesON==false)
  {

	  if (m_generateFiles)
	  {
		  // Imprimimos Trama y subtrama considerada
	      fprintf(m_pSchedFile,"--------- Trama %d Subtrama %d , Celda %d ---------\n", params.m_sfnSf >> 4, 0xF & params.m_sfnSf, params.m_cellId);
	      fprintf(m_pSchedFileDet,"--------- Trama %d Subtrama %d , Celda %d ---------\n", params.m_sfnSf >> 4, 0xF & params.m_sfnSf, params.m_cellId);
		  fprintf(m_pMatrizCQIFile,"--------- Trama %d Subtrama %d , Celda %d ---------\n", params.m_sfnSf >> 4, 0xF & params.m_sfnSf, params.m_cellId);
		  fprintf(m_pMatrizMetricasFile,"--------- Trama %d Subtrama %d , Celda %d ---------\n", params.m_sfnSf >> 4, 0xF & params.m_sfnSf, params.m_cellId);

		  // Indicacion de que no hay usuarios
	      fprintf(m_pSchedFile,"Celda sin usuarios\n");
	      fprintf(m_pSchedFileDet,"Celda sin usuarios\n");
		  fprintf(m_pMatrizCQIFile,"Celda sin usuarios\n");
		  fprintf(m_pMatrizMetricasFile,"Celda sin usuarios\n");

		  // Cerramos ficheros
		  fclose(m_pSchedFile);
		  fclose(m_pSchedFileDet);
		  fclose(m_pMatrizCQIFile);
		  fclose(m_pMatrizMetricasFile);

	  }

	  std::cout << " Frame no. " << (params.m_sfnSf >> 4) << " subframe no. " << (0xF & params.m_sfnSf) << std::endl;
	  std::cout << "Celda sin usuarios" << std::endl;

  }


  // Una vez asignados todos los usuarios --> reset TTI stats of users
  std::map <uint16_t, schedulerDRANFlowPerf_t>::iterator itStats;
  for (itStats = m_flowStatsDl.begin (); itStats != m_flowStatsDl.end (); itStats++)
    {
      (*itStats).second.lastTtiBytesTrasmitted = 0;
    }


  //ASIGNACIÓN DE USUARIOS-SUBBANDAS PARA TRAMA-SUBTRAMA (X) REALIZADA

  // generate the transmission opportunities by grouping the RBGs/SUBBANDS of the same RNTI and
  // creating the correspondent DCIs

  std::map <uint16_t, std::vector <uint16_t> >::iterator itMap = m_dlAllocationMap.begin ();
  while (itMap != m_dlAllocationMap.end ())
    {

      // create new BuildDataListElement_s for this LC
      BuildDataListElement_s newEl;
      newEl.m_rnti = (*itMap).first;
      // create the DlDciListElement_s
      DlDciListElement_s newDci;
      newDci.m_rnti = (*itMap).first;
      newDci.m_harqProcess = UpdateHarqProcessId ((*itMap).first);

      // Numero de canales logicos activos tiene ese RNTI
      uint16_t lcActives = LcActivePerFlow ((*itMap).first);
      NS_LOG_INFO (this << "Allocate user " << newEl.m_rnti << " rbg " << lcActives);
      if (lcActives == 0)
        {
          // Set to max value, to avoid divide by 0 below
          lcActives = (uint16_t)65535; // UINT16_MAX;
        }
      // Obtenemos el numero de subbandas(RBGs) asignadas al UE
      uint16_t RgbPerRnti = (*itMap).second.size (); // podriamos obtenerlo tambien del mapa "m_subbandsAsigUEs"
      // Obtenemos CQI y TX Mode del UE
      std::map <uint16_t,SbMeasResult_s>::iterator itCqi;
      itCqi = m_a30CqiRxed.find ((*itMap).first);
      std::map <uint16_t,uint8_t>::iterator itTxMode;
      itTxMode = m_uesTxMode.find ((*itMap).first);
      if (itTxMode == m_uesTxMode.end ())
        {
          NS_FATAL_ERROR ("No Transmission Mode info on user " << (*itMap).first);
        }
      int nLayer = TransmissionModesLayers::TxMode2LayerNum ((*itTxMode).second); // Numero de capas asociadas al modo de TX --> SISO = 1 (siempre SISO)
      // Para obtener el MCS de todas las subbandas que han sido asignadas al usuario, obtenemos el peor CQI en estas subbandas. Ej: CQI_1=15; CQI_2=3; --> CQI = 3.
      // De acuerdo a este CQI se genera el canal que proporcionara un BLER de X (considerando la tecnica LSM MMIB), en RX si la BLER es < 10% --> paquete se descartara y se aceptara en caso contrario
      std::vector <uint8_t> worstCqi1 ;
      std::vector <uint8_t> worstCqi (2, 15); // Vector de longitud 2 y valor inicial 15, aqui almacenaremos el peor CQI
      if (itCqi != m_a30CqiRxed.end ()) //UE tiene informacion de CQI
        {
          for (uint16_t k = 0; k < (*itMap).second.size (); k++) // Recorremos el número de subbandas que han sido asignadas al usuario *itMap.first
            {
        	  // Comprobamos que haya tantos CQI-subbanda elementos como número de subbandas tenga asignadas ese usuario
              if ((*itCqi).second.m_higherLayerSelected.size () > (*itMap).second.at (k))
                // Comprobamos si el tamaño de la estructura m_higherLayerSelected que contiene los CQI para todas las subbandas es mayor que la subbanda que se está considerando--> eso indica que esa subbanda tiene información de CQI
               {
                  NS_LOG_INFO (this << " RBG " << (*itMap).second.at (k) << " CQI " << (uint16_t)((*itCqi).second.m_higherLayerSelected.at ((*itMap).second.at (k)).m_sbCqi.at (0)) );
                  for (uint8_t j = 0; j < nLayer; j++) // Para todas las capas
                    {
                      if ((*itCqi).second.m_higherLayerSelected.at ((*itMap).second.at (k)).m_sbCqi.size () > j) // Comprobamos si hay CQIs por layer
                        {
                    	  // Comprobamos si el CQI en esta layer es peor que el CQI almacenado anteriormente, en ese caso almacenamso el nuevo peor CQI
                          if (((*itCqi).second.m_higherLayerSelected.at ((*itMap).second.at (k)).m_sbCqi.at (j)) < worstCqi.at (j))
                            {
                              worstCqi.at (j) = ((*itCqi).second.m_higherLayerSelected.at ((*itMap).second.at (k)).m_sbCqi.at (j));
                            }
                        }
                      else
                        {
                          // no CQI for this layer of this suband -> worst one
                          worstCqi.at (j) = 1;
                        }
                    }
                }
              else // No hay información de CQI para esa subbanda
                {
                  for (uint8_t j = 0; j < nLayer; j++)
                    {
                      worstCqi.at (j) = 1; // try with lowest MCS in RBG with no info on channel
                    }
                }
            }
        }
      else
        {
          for (uint8_t j = 0; j < nLayer; j++)
            {
              worstCqi.at (j) = 1; // try with lowest MCS in RBG with no info on channel
            }
        }
      for (uint8_t j = 0; j < nLayer; j++)
        {
          NS_LOG_INFO (this << " Layer " << (uint16_t)j << " CQI selected " << (uint16_t)worstCqi.at (j));
        }
      uint32_t bytesTxed = 0;
      // Para cada una de las layers, obtenemos el MCS asociado al peor CQI obtenido y se lo pasamos a la estructura de DCI para el DL
      // El tamaño del bloque de transporte asociado a ese MCS y calculamos los bytes txed para ese usuario por layer.
      for (uint8_t j = 0; j < nLayer; j++)
        {
          newDci.m_mcs.push_back (m_amc->GetMcsFromCqi (worstCqi.at (j)));
          int tbSize = (m_amc->GetTbSizeFromMcs (newDci.m_mcs.at (j), RgbPerRnti * rbgSize) / 8); // (size of TB in bytes according to table 7.1.7.2.1-1 of 36.213)
          newDci.m_tbsSize.push_back (tbSize);
          NS_LOG_INFO (this << " Layer " << (uint16_t)j << " MCS selected" << m_amc->GetMcsFromCqi (worstCqi.at (j)));
//          bytesTxed += tbSize/2; // ¿Porque? Deberia ser el tamaño del bloque de transporte
          bytesTxed += tbSize;
        }

      newDci.m_resAlloc = 0;  // only allocation type 0 at this stage --> ver estandar
      newDci.m_rbBitmap = 0; // Indica los RB a ser tx TBD (32 bit bitmap see 7.1.6 of 36.213)
      uint32_t rbgMask = 0;
      // Obtenemos la mascara de RBG a traves de la cual se indica los RBG que son asignados
      for (uint16_t k = 0; k < (*itMap).second.size (); k++)
        {
          rbgMask = rbgMask + (0x1 << (*itMap).second.at (k));
          NS_LOG_INFO (this << " Allocated RBG " << (*itMap).second.at (k));
        }
      newDci.m_rbBitmap = rbgMask; // (32 bit bitmap see 7.1.6 of 36.213)

      // Create the rlc PDUs -> equally divide resources among actives LCs
      std::map <LteFlowId_t, FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator itBufReq;
      for (itBufReq = m_rlcBufferReq.begin (); itBufReq != m_rlcBufferReq.end (); itBufReq++)
        {
          if (((*itBufReq).first.m_rnti == (*itMap).first)
              && (((*itBufReq).second.m_rlcTransmissionQueueSize > 0)
                  || ((*itBufReq).second.m_rlcRetransmissionQueueSize > 0)
                  || ((*itBufReq).second.m_rlcStatusPduSize > 0) ))
            {
              std::vector <struct RlcPduListElement_s> newRlcPduLe;
              for (uint8_t j = 0; j < nLayer; j++)
                {
                  RlcPduListElement_s newRlcEl;
                  newRlcEl.m_logicalChannelIdentity = (*itBufReq).first.m_lcId;
                  newRlcEl.m_size = newDci.m_tbsSize.at (j) / lcActives; // El tamaño de la PDU RLC lo obtenemos dividiendo igualmente lo recursos entre los LCs activos
                  NS_LOG_INFO (this << " LCID " << (uint32_t) newRlcEl.m_logicalChannelIdentity << " size " << newRlcEl.m_size << " layer " << (uint16_t)j);
                  newRlcPduLe.push_back (newRlcEl);
                  UpdateDlRlcBufferInfo (newDci.m_rnti, newRlcEl.m_logicalChannelIdentity, newRlcEl.m_size); // Actualizamos la cola de tx, simulando que la capa RLC va a tx los flujos asociados a los lcActivos que tiene
                  if (m_harqOn == true)
                    {
                      // store RLC PDU list for HARQ
                      std::map <uint16_t, DlHarqRlcPduListBuffer_t>::iterator itRlcPdu =  m_dlHarqProcessesRlcPduListBuffer.find ((*itMap).first);
                      if (itRlcPdu == m_dlHarqProcessesRlcPduListBuffer.end ())
                        {
                          NS_FATAL_ERROR ("Unable to find RlcPdcList in HARQ buffer for RNTI " << (*itMap).first);
                        }
                      (*itRlcPdu).second.at (j).at (newDci.m_harqProcess).push_back (newRlcEl);
                    }
                }
              newEl.m_rlcPduList.push_back (newRlcPduLe);
            }
          if ((*itBufReq).first.m_rnti > (*itMap).first)
            {
              break;
            }
        }
      for (uint8_t j = 0; j < nLayer; j++)
        {
          newDci.m_ndi.push_back (1);
          newDci.m_rv.push_back (0);
        }

      newDci.m_tpc = m_ffrSapProvider->GetTpc ((*itMap).first);

      newEl.m_dci = newDci;

      if (m_harqOn == true)
        {
          // store DCI for HARQ
          std::map <uint16_t, DlHarqProcessesDciBuffer_t>::iterator itDci = m_dlHarqProcessesDciBuffer.find (newEl.m_rnti);
          if (itDci == m_dlHarqProcessesDciBuffer.end ())
            {
              NS_FATAL_ERROR ("Unable to find RNTI entry in DCI HARQ buffer for RNTI " << newEl.m_rnti);
            }
          (*itDci).second.at (newDci.m_harqProcess) = newDci;
          // refresh timer
          std::map <uint16_t, DlHarqProcessesTimer_t>::iterator itHarqTimer =  m_dlHarqProcessesTimer.find (newEl.m_rnti);
          if (itHarqTimer== m_dlHarqProcessesTimer.end ())
            {
              NS_FATAL_ERROR ("Unable to find HARQ timer for RNTI " << (uint16_t)newEl.m_rnti);
            }
          (*itHarqTimer).second.at (newDci.m_harqProcess) = 0;
        }

      // ...more parameters -> ingored in this version

      // Introducimos los valores del scheduler (RBG bitmap, MCS, TBSIZE, RLC PDU, tipo de asignacion de recursos (=0), NDI, RV, TPC)
      // en el objeto que se pasara a la capa MAC para que ejecute estas decisiones.
      ret.m_buildDataList.push_back (newEl);

      // Una vez se han comunicado las decisiones del scheduler, acutalizamos las estadisiticas del UE
      std::map <uint16_t, schedulerDRANFlowPerf_t>::iterator it;
      it = m_flowStatsDl.find ((*itMap).first);
      if (it != m_flowStatsDl.end ())
        {
          (*it).second.lastTtiBytesTrasmitted = bytesTxed;
          NS_LOG_INFO (this << " UE total bytes txed " << (*it).second.lastTtiBytesTrasmitted);
        }
      else
        {
          NS_FATAL_ERROR (this << " No Stats for this allocated UE");
          std::cout << " No stats for this allocated UE " << std::endl;
        }

      itMap++;
    } // end while allocation

  ret.m_nrOfPdcchOfdmSymbols = 1;   /// \todo check correct value according the DCIs txed


  // update UEs stats
  if (m_dlAllocationMap.size ()>0) // Si hay usuarios asignados, actualizamos las estadísticas de tp y bytes txed
  {
	  NS_LOG_INFO (this << " Update UEs statistics");
	  for (itStats = m_flowStatsDl.begin (); itStats != m_flowStatsDl.end (); itStats++)
		{
		  (*itStats).second.totalBytesTransmitted += (*itStats).second.lastTtiBytesTrasmitted;
		  // update average throughput (see eq. 12.3 of Sec 12.3.1.2 of LTE – The UMTS Long Term Evolution, Ed Wiley)
		  (*itStats).second.lastAveragedThroughput = ((1.0 - (1.0 / m_timeWindow)) * (*itStats).second.lastAveragedThroughput) + ((1.0 / m_timeWindow) * (double)((*itStats).second.lastTtiBytesTrasmitted / 0.001));
		  NS_LOG_INFO (this << " UE total bytes " << (*itStats).second.totalBytesTransmitted);
		  NS_LOG_INFO (this << " UE average throughput " << (*itStats).second.lastAveragedThroughput);
		  (*itStats).second.lastTtiBytesTrasmitted = 0;
		}
  }

  // Reseteamos variables miembro relacionadas con la asignacion
  Reset();

  // Finalmente, enviamos a la capa MAC la decisión tomada de Scheduling sobre los datos
  m_schedSapUser->SchedDlConfigInd (ret);


  return;
}

///// Metodo para asignar usuarios /////
void
schedulerDRANMac::AssignUe(uint16_t ue, uint16_t subband)
{
	// Asigna el usuario "Ue" a a la subbanda "subband"

	  std::map <uint16_t, std::vector <uint16_t> >::iterator itSubbBusc; // Mapa que almacena subbandas - ues
	  std::map <uint16_t,SbMeasResult_s>::iterator it; // Mapa que almacena los UEs - CQIs
	  std::map <uint16_t, std::vector <uint16_t> >::iterator itAllocMap; // Mapa en el que se tienen almacenados los UEs-subbandas asignadas
	  std::map <uint16_t, uint16_t>::iterator itMapContSubbands; // Mapa contador de subbandas

	  m_rbgMap.at (subband) = true; // Eliminamos subbanda de la lista de subbandas disponibles
	  itAllocMap = m_dlAllocationMap.find (ue); // Buscamos usuario j
	  itSubbBusc = m_subbandasUeMap.find(subband); // Buscamos subbanda K
	  itMapContSubbands = m_subbandsAsigUEs.find(ue); // Buscamos UE j
	  if (itAllocMap == m_dlAllocationMap.end ()) // si no encuentra a este usuario, lo añade
		{
		  // insert new element
		  std::vector <uint16_t> tempMap;
		  tempMap.push_back (subband); // Añade el RBG a un mapa temporal para posteriormente añadirlo al mapa de asignaciones RBGs-UEs
		  m_dlAllocationMap.insert (std::pair <uint16_t, std::vector <uint16_t> > (ue, tempMap)); // Asignamos UE-subbanda asignada
		}
	  else
		{
		  (*itAllocMap).second.push_back (subband);
		}
	  if (itSubbBusc == m_subbandasUeMap.end()) // Añadimos la subbanda y el usuario
	  {
		  // insert new element
		  std::vector <uint16_t> temp2Map;
		  temp2Map.push_back (ue);
		  m_subbandasUeMap.insert (std::pair <uint16_t, std::vector <uint16_t> > (subband, temp2Map));
	  }
	  else
	  {
		  (*itSubbBusc).second.push_back (ue);

	  }
	  // Actualizamos contador de numero de subbandas asignadas a cada UE
	  if(itMapContSubbands != m_subbandsAsigUEs.end())
	  {
		  (*itMapContSubbands).second ++;
	  }

}

void
schedulerDRANMac::Reset()
{
	m_dlAllocationMap.erase(m_dlAllocationMap.begin(), m_dlAllocationMap.end());
	m_subbandsAsigUEs.erase(m_subbandsAsigUEs.begin(), m_subbandsAsigUEs.end());
	m_subbandasUeMap.erase(m_subbandasUeMap.begin(), m_subbandasUeMap.end());
}



///// Metodo para activar la bearer de datos para el ue que se va a añadir en la nueva celda
void
schedulerDRANMac::ActivateDataRadioBearer(uint16_t rnti, Ptr<LteEnbRrc> enbRrc)
{
	// Definimos el tipo de data radio bearer
    enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VIDEO;
    EpsBearer bearer (q);

    EpcEnbS1SapUser::DataRadioBearerSetupRequestParameters params;
    params.rnti = rnti;
    params.bearer = bearer;
    params.bearerId = 0;
    params.gtpTeid = 0; // don't care
    enbRrc->GetS1SapUser ()->DataRadioBearerSetupRequest (params);
}

void
schedulerDRANMac::DoSchedDlRachInfoReq (const struct FfMacSchedSapProvider::SchedDlRachInfoReqParameters& params)
{
	  std::cout << "------> DL Rach Info Req" << std::endl;

  NS_LOG_FUNCTION (this);

  m_rachList = params.m_rachList;

  return;
} // LteEnbMac will request to the scheduler the allocation of resources for the RAR using this primitive

void
schedulerDRANMac::DoSchedDlCqiInfoReq (const struct FfMacSchedSapProvider::SchedDlCqiInfoReqParameters& params)
{
	  std::cout << "------> DL CQI Info Req" << std::endl;

  NS_LOG_FUNCTION (this);
  m_ffrSapProvider->ReportDlCqiInfo (params);

  for (unsigned int i = 0; i < params.m_cqiList.size (); i++)
    {
      if ( params.m_cqiList.at (i).m_cqiType == CqiListElement_s::P10 )
        {
          NS_LOG_LOGIC ("wideband CQI " <<  (uint32_t) params.m_cqiList.at (i).m_wbCqi.at (0) << " reported");
          std::map <uint16_t,uint8_t>::iterator it;
          uint16_t rnti = params.m_cqiList.at (i).m_rnti;
          it = m_p10CqiRxed.find (rnti);
          if (it == m_p10CqiRxed.end ())
            {
              // create the new entry
              m_p10CqiRxed.insert ( std::pair<uint16_t, uint8_t > (rnti, params.m_cqiList.at (i).m_wbCqi.at (0)) ); // only codeword 0 at this stage (SISO)
              // generate correspondent timer
              m_p10CqiTimers.insert ( std::pair<uint16_t, uint32_t > (rnti, m_cqiTimersThreshold));
            }
          else
            {
              // update the CQI value and refresh correspondent timer
              (*it).second = params.m_cqiList.at (i).m_wbCqi.at (0);
              // update correspondent timer
              std::map <uint16_t,uint32_t>::iterator itTimers;
              itTimers = m_p10CqiTimers.find (rnti);
              (*itTimers).second = m_cqiTimersThreshold;
            }
        }
      else if ( params.m_cqiList.at (i).m_cqiType == CqiListElement_s::A30 )
        {
          // subband CQI reporting high layer configured
          std::map <uint16_t,SbMeasResult_s>::iterator it;
          uint16_t rnti = params.m_cqiList.at (i).m_rnti;
          it = m_a30CqiRxed.find (rnti);
          if (it == m_a30CqiRxed.end ())
            {
              // create the new entry
              m_a30CqiRxed.insert ( std::pair<uint16_t, SbMeasResult_s > (rnti, params.m_cqiList.at (i).m_sbMeasResult) );
              m_a30CqiTimers.insert ( std::pair<uint16_t, uint32_t > (rnti, m_cqiTimersThreshold));
            }
          else
            {
              // update the CQI value and refresh correspondent timer
              (*it).second = params.m_cqiList.at (i).m_sbMeasResult;
              std::map <uint16_t,uint32_t>::iterator itTimers;
              itTimers = m_a30CqiTimers.find (rnti);
              (*itTimers).second = m_cqiTimersThreshold;
            }
        }
      else
        {
          NS_LOG_ERROR (this << " CQI type unknown");
        }
    }
  return;
}


double
schedulerDRANMac::EstimateUlSinr (uint16_t rnti, uint16_t rb)
// Estima la SINR en aquellos casos en los que no existe (SINR = -5000) como la media de los valores de SINR que estan disponibles
{
	  std::cout << "------> Estimate UL SINR" << std::endl;

  std::map <uint16_t, std::vector <double> >::iterator itCqi = m_ueCqi.find (rnti); // Obtenemos el iterator que se corresponde la clave con el rnti
  if (itCqi == m_ueCqi.end ())
    {
      // no cqi info about this UE
      return (NO_SINR);

    }
  else
    {
      // take the average SINR value among the available
      double sinrSum = 0;
      unsigned int sinrNum = 0;
      for (uint32_t i = 0; i < m_cschedCellConfig.m_ulBandwidth; i++)
        {
          double sinr = (*itCqi).second.at (i);
          if (sinr != NO_SINR)
            {
              sinrSum += sinr;
              sinrNum++;
            }
        }
      double estimatedSinr = (sinrNum > 0) ? (sinrSum / sinrNum) : DBL_MAX;
      // store the value
      (*itCqi).second.at (rb) = estimatedSinr;
      return (estimatedSinr);
    }
}

void
schedulerDRANMac::DoSchedUlTriggerReq (const struct FfMacSchedSapProvider::SchedUlTriggerReqParameters& params)
{
	  std::cout << "------> LANZAMOS SCHEDULER UL" << std::endl;

  NS_LOG_FUNCTION (this << " UL - Frame no. " << (params.m_sfnSf >> 4) << " subframe no. " << (0xF & params.m_sfnSf) << " size " << params.m_ulInfoList.size ());
  std::cout << " UL - Frame no. " << (params.m_sfnSf >> 4) << " subframe no. " << (0xF & params.m_sfnSf) << " size " << params.m_ulInfoList.size () << std::endl;
  RefreshUlCqiMaps ();
  m_ffrSapProvider->ReportUlCqiInfo (m_ueCqi);

  // Generate RBs map
  FfMacSchedSapUser::SchedUlConfigIndParameters ret; // Objeto en el que se almacenaran las decisiones del scheduler UL y se tx a la capa MAC
  std::vector <bool> rbMap;
  uint16_t rbAllocatedNum = 0;
  std::set <uint16_t> rntiAllocated; // Usuarios ya asignados
  std::vector <uint16_t> rbgAllocationMap; // Vector de long el numero de RBs que hayan sido asignados mediante RACH y que almacenara el RNTI asignado a esos RBs
  // update with RACH allocation map
  rbgAllocationMap = m_rachAllocationMap; // Actualizamos el vector de RNTIs/RBs con lo obtenido tras la asignacion RACH en el DL
  //rbgm_dlAllocationMap.resize (m_cschedCellConfig.m_ulBandwidth, 0);
  m_rachAllocationMap.clear ();
  m_rachAllocationMap.resize (m_cschedCellConfig.m_ulBandwidth, 0);


  // Obtenemos el numero de RBs ya asignados
  rbMap.resize (m_cschedCellConfig.m_ulBandwidth, false);
  rbMap = m_ffrSapProvider->GetAvailableUlRbg (); // Obtiene el vector de los RBs disponibles en UL para esta celda (a diferencia del DL que obtiene los RBGS=subbandas disponibles)

  for (std::vector<bool>::iterator it = rbMap.begin (); it != rbMap.end (); it++)
    {
      if ((*it) == true )
        {
          rbAllocatedNum++; // Numero de RBs ya asignados al principio del scheduler
        }
    }

  uint8_t minContinuousUlBandwidth = m_ffrSapProvider->GetMinContinuousUlBandwidth ();
  uint8_t ffrUlBandwidth = m_cschedCellConfig.m_ulBandwidth - rbAllocatedNum; // BW disponible en RB teniendo en cuenta los RBs que ya han sido asignados

  // remove RACH allocation
  for (uint16_t i = 0; i < m_cschedCellConfig.m_ulBandwidth; i++)
    {
      if (rbgAllocationMap.at (i) != 0)
        {
          rbMap.at (i) = true; // Actualizamos vector de booleans con los RBs que han sido asignados
          NS_LOG_DEBUG (this << " Allocated for RACH " << i);
        }
    }


  // HARQ Process
  if (m_harqOn == true)
    {
      //   Process UL HARQ feedback

      for (uint16_t i = 0; i < params.m_ulInfoList.size (); i++)
        {
          if (params.m_ulInfoList.at (i).m_receptionStatus == UlInfoListElement_s::NotOk)
            {
              // retx correspondent block: retrieve the UL-DCI
              uint16_t rnti = params.m_ulInfoList.at (i).m_rnti;
              std::map <uint16_t, uint8_t>::iterator itProcId = m_ulHarqCurrentProcessId.find (rnti);
              if (itProcId == m_ulHarqCurrentProcessId.end ())
                {
                  NS_LOG_ERROR ("No info find in HARQ buffer for UE (might change eNB) " << rnti);
                }
              uint8_t harqId = (uint8_t)((*itProcId).second - HARQ_PERIOD) % HARQ_PROC_NUM;
              NS_LOG_INFO (this << " UL-HARQ retx RNTI " << rnti << " harqId " << (uint16_t)harqId << " i " << i << " size "  << params.m_ulInfoList.size ());
              std::map <uint16_t, UlHarqProcessesDciBuffer_t>::iterator itHarq = m_ulHarqProcessesDciBuffer.find (rnti);
              if (itHarq == m_ulHarqProcessesDciBuffer.end ())
                {
                  NS_LOG_ERROR ("No info find in HARQ buffer for UE (might change eNB) " << rnti);
                  continue;
                }
              UlDciListElement_s dci = (*itHarq).second.at (harqId);
              std::map <uint16_t, UlHarqProcessesStatus_t>::iterator itStat = m_ulHarqProcessesStatus.find (rnti);
              if (itStat == m_ulHarqProcessesStatus.end ())
                {
                  NS_LOG_ERROR ("No info find in HARQ buffer for UE (might change eNB) " << rnti);
                }
              if ((*itStat).second.at (harqId) >= 3)
                {
                  NS_LOG_INFO ("Max number of retransmissions reached (UL)-> drop process");
                  continue;
                }
              bool free = true;
              for (int j = dci.m_rbStart; j < dci.m_rbStart + dci.m_rbLen; j++)
                {
                  if (rbMap.at (j) == true)
                    {
                      free = false;
                      NS_LOG_INFO (this << " BUSY " << j);
                    }
                }
              if (free)
                {
                  // retx on the same RBs
                  for (int j = dci.m_rbStart; j < dci.m_rbStart + dci.m_rbLen; j++)
                    {
                      rbMap.at (j) = true;
                      rbgAllocationMap.at (j) = dci.m_rnti;
                      NS_LOG_INFO ("\tRB " << j);
                      rbAllocatedNum++;
                    }
                  NS_LOG_INFO (this << " Send retx in the same RBs " << (uint16_t)dci.m_rbStart << " to " << dci.m_rbStart + dci.m_rbLen << " RV " << (*itStat).second.at (harqId) + 1);
                }
              else
                {
                  NS_LOG_INFO ("Cannot allocate retx due to RACH allocations for UE " << rnti);
                  continue;
                }
              dci.m_ndi = 0;
              // Update HARQ buffers with new HarqId
              (*itStat).second.at ((*itProcId).second) = (*itStat).second.at (harqId) + 1;
              (*itStat).second.at (harqId) = 0;
              (*itHarq).second.at ((*itProcId).second) = dci;
              ret.m_dciList.push_back (dci);
              rntiAllocated.insert (dci.m_rnti);
            }
          else
            {
              NS_LOG_INFO (this << " HARQ-ACK feedback from RNTI " << params.m_ulInfoList.at (i).m_rnti);
            }
        }
    }


  std::map <uint16_t,uint32_t>::iterator it; // Iterator para recorrer los usuarios que han reportado informe de estado
  int nflows = 0;

  for (it = m_ceBsrRxed.begin (); it != m_ceBsrRxed.end (); it++)
    {
      std::set <uint16_t>::iterator itRnti = rntiAllocated.find ((*it).first);
      // select UEs with queues not empty and not yet allocated for HARQ
      if (((*it).second > 0)&&(itRnti == rntiAllocated.end ()))
        {
          nflows++; // Obtenemos el numero de flujos totales a tx por todos los RNTIs que no han sido asignado por HARQ y tienen datos a tx
        }
    } // De esta forma, evaluamos el numero de usuarios activos

  if (nflows == 0) // Si no hay usuarios activos bien porque no tengan datos a tx o porque hayan sido asignados por HARQ, se pasa la decision al scheduler de la asigancion HARQ y salimos porque no hay flujos activos a ser asignados
    {
      if (ret.m_dciList.size () > 0) // Pasamos la decision del scheduler de los usuarios que hayan sido asignados por HARQ (m_dciList > 0)
        {
          m_allocationMaps.insert (std::pair <uint16_t, std::vector <uint16_t> > (params.m_sfnSf, rbgAllocationMap)); // Introduce en el mapa m_m_dlAllocationMaps, la trama-subtrama y los RNTIs ya scheduled
          m_schedSapUser->SchedUlConfigInd (ret); // Pasa la decisión de scheduler a la capa MAC
        }

      return;  // no flows to be scheduled
    }


  // SCHEDULER DE DATOS (Si hay flujos activos)

  //----> ESTA PARTE POSIBLEMENTE CAMBIAR, EL NUMERO DE RBS POR FLUJO SERA UN DATO DE ENTRADA (NUMERO DE SUBBANDAS CONTIGUAS * RB/SUBBANDA)
  // Divide the remaining resources equally among the active users starting from the subsequent one served last scheduling trigger
  uint16_t tempRbPerFlow = (ffrUlBandwidth) / (nflows + rntiAllocated.size ()); // Obtenemos el numero de RBs disponibles por flujo activo tras la posible asignacion HARQ
  // ffrUlBandwidth es el BW disponible (total-RB's ocupados) ; nflows es el numero de usuarios ativos y rnitAllocated es el vector con los usuarios ya asignados.

  // DUDAAA! PORQUE DIVIDE ENTRE LOS USUARIOS ASIGNADOS TB? DE ESTA MANERA NO OBTIENES EL NUMERO DE RB'S DISPONIBLES POR FLUJO SI NO EL Nº DE RBS DISPONIBLES PARA LOS USUARIOS ACTIVOS Y LOS QUE YA HAN SIDO ASIGNADOS NO??

  // Comprobamos si el numero de RBs por flujo es mayor o menor que el minimo UL BW contiguo, en el primero de los casos el RB disponible por flujo sera el UL BW
  uint16_t rbPerFlow = (minContinuousUlBandwidth < tempRbPerFlow) ? minContinuousUlBandwidth : tempRbPerFlow;

  // Obtenemos el tamaño de la subbanda en RB y el numero de RB disponibles por usuario de acuerdo con el numero de subbandas-usuario han sido definidas
//  int rbgSize = GetRbgSize (m_cschedCellConfig.m_ulBandwidth);
//  uint16_t rbPerFlow = Nsubbands_user*rbgSize;

  // rbPerFlow -> maximo numero de RBs por usuario
  if (rbPerFlow < 3)
    {
      rbPerFlow = 3;  // at least 3 rbg per flow (till available resource) to ensure TxOpportunity >= 7 bytes
    }

  int rbAllocated = 0; // RB de comienzo (start), inicialmente 0

  // Una vez obtenido el numero de RBs disponibles por flujo, obtenemos el RNTI sobre el que tenemos que hacer el scheduler

  std::map <uint16_t, schedulerDRANFlowPerf_t>::iterator itStats;
  if (m_nextRntiUl != 0) // Comprobamos si hay un nuevo RNTI al que servir en el proximo scheduling en UL
    {
	  // Buscamos el nuevo RNTI  a servir entre el mapa de UE's status report recibido, para obtener asi la informacion del usuario
      for (it = m_ceBsrRxed.begin (); it != m_ceBsrRxed.end (); it++)
        {
          if ((*it).first == m_nextRntiUl)
            {
              break; // Salimos del bucle -> do
            }
        }
      if (it == m_ceBsrRxed.end ())
        {
          NS_LOG_ERROR (this << " no user found");
        }
    }
  else // Si no hay un nuevo RNTI al que servir en el proximo scheduling lo seleccionamos del mapa de informe de estado de UE's recibido
    {
      it = m_ceBsrRxed.begin ();
      m_nextRntiUl = (*it).first;
    }

  // -- COMIENZA EL SCHEDULER
  do // WHILE (haya un RNTI a ser asignado diferente y los RBs disponbiles para ese RNTI sean mayores que 0)
    {
      std::set <uint16_t>::iterator itRnti = rntiAllocated.find ((*it).first); // Buscamos si el RNTI  a ser servido/asignado esta dentro del mapa de RNTIs asignados
      if ((itRnti != rntiAllocated.end ())||((*it).second == 0)) // Si esta dentro de los usuarios ya asignados o este no tiene datos para tx, lo descartamos y buscamos otro usuario
        {
          // UE already allocated for UL-HARQ -> skip it
          NS_LOG_DEBUG (this << " UE already allocated in HARQ -> discared, RNTI " << (*it).first);
          it++;
          if (it == m_ceBsrRxed.end ())
            {
              // Si hemos finalizado la lista de usuarios disponibles empezamos desde el principio
              it = m_ceBsrRxed.begin ();
            }
          continue;
        }
      // Comprobamos si el RB de comienzo + los RBs disponibles a asignar estan dentro del ancho de banda total disponible del UL, en caso contrario limitamos
      // los RBs disponibles para el usuario a lo maximo que sea posible conceder teniendo en cuenta el inicio de los RBs que van a ser asignados.
      if (rbAllocated + rbPerFlow - 1 > m_cschedCellConfig.m_ulBandwidth)
        {
          // limit to physical resources last resource assignment
          rbPerFlow = m_cschedCellConfig.m_ulBandwidth - rbAllocated;
          // at least 3 rbg per flow to ensure TxOpportunity >= 7 bytes
          if (rbPerFlow < 3) // IMPORTANTE! Necesarios al menos 3 RB por usuario para que se pueda generar las TX Opportunity y TX los datos de este usuario
            {
              // terminate allocation
              rbPerFlow = 0; // si no pueden tener 3 RBs se termina la asignacion --> acaba scheduler
            }
        } // De esta manera, cuando un UE tenga maximo disponible 1 Subbanda que se le pueda asignar se quitara la restriccion de 2 subbandas adyacentes ya que solo hay 1 disponible.

      rbAllocated = 0;
      UlDciListElement_s uldci; // creamos objeto en el que van a ser almacenadas las decisiones del scheduler de DATOS (no de retx)
      uldci.m_rnti = (*it).first;
      uldci.m_rbLen = rbPerFlow;
      bool allocated = false;

      // Seleccionamos aleatoriamente la subbanda


      while ((!allocated)&&((rbAllocated + rbPerFlow - m_cschedCellConfig.m_ulBandwidth) < 1) && (rbPerFlow != 0))
        {
          // Comprobamos la disponibilidad de los recursos que van a ser asignados
          bool free = true;
          for (uint16_t j = rbAllocated; j < rbAllocated + rbPerFlow; j++)
            {
              if (rbMap.at (j) == true)
                {
                  free = false;
                  break;
                }
              if ((m_ffrSapProvider->IsUlRbgAvailableForUe (j, (*it).first)) == false)
                {
                  free = false;
                  break;
                }
            }
          if (free) // Si dichos recursos estan disponibles (no han sido previamente asignados y estan disponbiles para el UE considerado)
            {
              NS_LOG_INFO (this << "RNTI: "<< (*it).first<< " RB Allocated " << rbAllocated << " rbPerFlow " << rbPerFlow << " flows " << nflows);
              uldci.m_rbStart = rbAllocated; // Especificamos el inicio de esos RBs
              for (uint16_t j = rbAllocated; j < rbAllocated + rbPerFlow; j++)
                {
                  rbMap.at (j) = true; // Marcamos en el mapa de boolean los RBs que son asignados como true
                  // store info on allocation for managing ul-cqi interpretation
                  rbgAllocationMap.at (j) = (*it).first; // almacenamos el RNTI que es asignado
                }
              rbAllocated += rbPerFlow; // siguiente inicio de RBs tiene en cuenta los RBs que han sido asignados al UE anterior
              allocated = true;
              break;
            }
          rbAllocated++;
          if (rbAllocated + rbPerFlow - 1 > m_cschedCellConfig.m_ulBandwidth)
            {
              // limit to physical resources last resource assignment
              rbPerFlow = m_cschedCellConfig.m_ulBandwidth - rbAllocated;
              // at least 3 rbg per flow to ensure TxOpportunity >= 7 bytes
              if (rbPerFlow < 3)
                {
                  // terminate allocation
                  rbPerFlow = 0;
                }
            }
        }
      if (!allocated)
        {
          // unable to allocate new resource: finish scheduling
          m_nextRntiUl = (*it).first;
//          if (ret.m_dciList.size () > 0)
//            {
//              m_schedSapUser->SchedUlConfigInd (ret);
//            }
//          m_m_dlAllocationMaps.insert (std::pair <uint16_t, std::vector <uint16_t> > (params.m_sfnSf, rbgm_dlAllocationMap));
//          return;
          break;
        }


      // Calculo de CQI, MCS de los RBs asignados al usuario. Se escoge el CQI mas bajo de los RBs asignados

      std::map <uint16_t, std::vector <double> >::iterator itCqi = m_ueCqi.find ((*it).first);
      int cqi = 0;
      if (itCqi == m_ueCqi.end ())
        {
          // no cqi info about this UE
          uldci.m_mcs = 0; // MCS 0 -> UL-AMC TBD
        }
      else
        {
          // En primer lugar cogemos la SINR peor --> peor RB
          double minSinr = (*itCqi).second.at (uldci.m_rbStart);
          if (minSinr == NO_SINR)
            {
              minSinr = EstimateUlSinr ((*it).first, uldci.m_rbStart);
            }
          for (uint16_t i = uldci.m_rbStart; i < uldci.m_rbStart + uldci.m_rbLen; i++)
            {
              double sinr = (*itCqi).second.at (i);
              if (sinr == NO_SINR)
                {
                  sinr = EstimateUlSinr ((*it).first, i);
                }
              if ((*itCqi).second.at (i) < minSinr)
                {
                  minSinr = (*itCqi).second.at (i);
                }
            }

          // Traducimos la SINR a CQI -> WILD ACK: same as DL
          double s = log2 ( 1 + (
                              std::pow (10, minSinr / 10 )  /
                              ( (-std::log (5.0 * 0.00005 )) / 1.5) )); // NOTA!!! SI EN EL MODELO LTEAMC CAMBIAMOS LA BER DE REFERENCIA 0.00005 POR OTRO VALOR, CAMBIARLO AQUI TAMBIEN!
          cqi = m_amc->GetCqiFromSpectralEfficiency (s);
          if (cqi == 0) // Si el CQI = 0, cogemos otro usuario
            {
              it++;
              if (it == m_ceBsrRxed.end ())
                {
                  // restart from the first
                  it = m_ceBsrRxed.begin ();
                }
              NS_LOG_DEBUG (this << " UE discared for CQI=0, RNTI " << uldci.m_rnti);
              // remove UE from allocation map
              for (uint16_t i = uldci.m_rbStart; i < uldci.m_rbStart + uldci.m_rbLen; i++)
                {
                  rbgAllocationMap.at (i) = 0;
                }
              continue; // CQI == 0 means "out of range" (see table 7.2.3-1 of 36.213)
            }
          uldci.m_mcs = m_amc->GetMcsFromCqi (cqi);
        }

      uldci.m_tbSize = (m_amc->GetTbSizeFromMcs (uldci.m_mcs, rbPerFlow) / 8);
      UpdateUlRlcBufferInfo (uldci.m_rnti, uldci.m_tbSize);
      uldci.m_ndi = 1;
      uldci.m_cceIndex = 0;
      uldci.m_aggrLevel = 1;
      uldci.m_ueTxAntennaSelection = 3; // antenna selection OFF
      uldci.m_hopping = false;
      uldci.m_n2Dmrs = 0;
      uldci.m_tpc = 0; // no power control
      uldci.m_cqiRequest = false; // only period CQI at this stage
      uldci.m_ulIndex = 0; // TDD parameter
      uldci.m_dai = 1; // TDD parameter
      uldci.m_freqHopping = 0;
      uldci.m_pdcchPowerOffset = 0; // not used
      ret.m_dciList.push_back (uldci);
      // store DCI for HARQ_PERIOD
      uint8_t harqId = 0;
      if (m_harqOn == true)
        {
          std::map <uint16_t, uint8_t>::iterator itProcId;
          itProcId = m_ulHarqCurrentProcessId.find (uldci.m_rnti);
          if (itProcId == m_ulHarqCurrentProcessId.end ())
            {
              NS_FATAL_ERROR ("No info find in HARQ buffer for UE " << uldci.m_rnti);
            }
          harqId = (*itProcId).second;
          std::map <uint16_t, UlHarqProcessesDciBuffer_t>::iterator itDci = m_ulHarqProcessesDciBuffer.find (uldci.m_rnti);
          if (itDci == m_ulHarqProcessesDciBuffer.end ())
            {
              NS_FATAL_ERROR ("Unable to find RNTI entry in UL DCI HARQ buffer for RNTI " << uldci.m_rnti);
            }
          (*itDci).second.at (harqId) = uldci;
          // Update HARQ process status (RV 0)
          std::map <uint16_t, UlHarqProcessesStatus_t>::iterator itStat = m_ulHarqProcessesStatus.find (uldci.m_rnti);
          if (itStat == m_ulHarqProcessesStatus.end ())
            {
              NS_LOG_ERROR ("No info find in HARQ buffer for UE (might change eNB) " << uldci.m_rnti);
            }
          (*itStat).second.at (harqId) = 0;
        }

      NS_LOG_INFO (this << " UE Allocation RNTI " << (*it).first << " startPRB " << (uint32_t)uldci.m_rbStart << " nPRB " << (uint32_t)uldci.m_rbLen << " CQI " << cqi << " MCS " << (uint32_t)uldci.m_mcs << " TBsize " << uldci.m_tbSize << " RbAlloc " << rbAllocated << " harqId " << (uint16_t)harqId);
      std::cout << " UE Allocation RNTI " << (*it).first << " startPRB " << (uint32_t)uldci.m_rbStart << " nPRB " << (uint32_t)uldci.m_rbLen << " CQI " << cqi << " MCS " << (uint32_t)uldci.m_mcs << " TBsize " << uldci.m_tbSize << " RbAlloc " << rbAllocated << " harqId " << (uint16_t)harqId << std::endl;

      // update TTI  UE stats
      itStats = m_flowStatsUl.find ((*it).first);
      if (itStats != m_flowStatsUl.end ())
        {
          (*itStats).second.lastTtiBytesTrasmitted =  uldci.m_tbSize;
        }
      else
        {
          NS_LOG_DEBUG (this << " No Stats for this allocated UE");
        }


      it++;
      if (it == m_ceBsrRxed.end ())
        {
          // restart from the first
          it = m_ceBsrRxed.begin ();
        }
      if ((rbAllocated == m_cschedCellConfig.m_ulBandwidth) || (rbPerFlow == 0))
        {
          // Stop allocation: no more PRBs
          m_nextRntiUl = (*it).first;
          break;
        }
    }
  while (((*it).first != m_nextRntiUl)&&(rbPerFlow!=0));


  // Update global UE stats
  // update UEs stats
  for (itStats = m_flowStatsUl.begin (); itStats != m_flowStatsUl.end (); itStats++)
    {
      (*itStats).second.totalBytesTransmitted += (*itStats).second.lastTtiBytesTrasmitted;
      // update average throughput (see eq. 12.3 of Sec 12.3.1.2 of LTE – The UMTS Long Term Evolution, Ed Wiley)
		(*itStats).second.lastAveragedThroughput = ((1.0 - (1.0 / m_timeWindow)) * (*itStats).second.lastAveragedThroughput) + ((1.0 / m_timeWindow) * (double)((*itStats).second.lastTtiBytesTrasmitted / 0.001));
      NS_LOG_INFO (this << " UE total bytes " << (*itStats).second.totalBytesTransmitted);
      NS_LOG_INFO (this << " UE average throughput " << (*itStats).second.lastAveragedThroughput);
      (*itStats).second.lastTtiBytesTrasmitted = 0;
    }
  m_allocationMaps.insert (std::pair <uint16_t, std::vector <uint16_t> > (params.m_sfnSf, rbgAllocationMap));
  m_schedSapUser->SchedUlConfigInd (ret);

  return;
}

void
schedulerDRANMac::DoSchedUlNoiseInterferenceReq (const struct FfMacSchedSapProvider::SchedUlNoiseInterferenceReqParameters& params)
{
  NS_LOG_FUNCTION (this);
  return;
}

void
schedulerDRANMac::DoSchedUlSrInfoReq (const struct FfMacSchedSapProvider::SchedUlSrInfoReqParameters& params)
{
  NS_LOG_FUNCTION (this);
  return;
}

void
schedulerDRANMac::DoSchedUlMacCtrlInfoReq (const struct FfMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters& params)
{
	  std::cout << "------> MAC CTRL Info Req" << std::endl;

  NS_LOG_FUNCTION (this);

  std::map <uint16_t,uint32_t>::iterator it;

  for (unsigned int i = 0; i < params.m_macCeList.size (); i++)
    {
      if ( params.m_macCeList.at (i).m_macCeType == MacCeListElement_s::BSR )
        {
          // buffer status report
          // note that this scheduler does not differentiate the
          // allocation according to which LCGs have more/less bytes
          // to send.
          // Hence the BSR of different LCGs are just summed up to get
          // a total queue size that is used for allocation purposes.

          uint32_t buffer = 0;
          for (uint8_t lcg = 0; lcg < 4; ++lcg)
            {
              uint8_t bsrId = params.m_macCeList.at (i).m_macCeValue.m_bufferStatus.at (lcg);
              buffer += BufferSizeLevelBsr::BsrId2BufferSize (bsrId);
            }

          uint16_t rnti = params.m_macCeList.at (i).m_rnti;
          NS_LOG_LOGIC (this << "RNTI=" << rnti << " buffer=" << buffer);
          it = m_ceBsrRxed.find (rnti);
          if (it == m_ceBsrRxed.end ())
            {
              // create the new entry
              m_ceBsrRxed.insert ( std::pair<uint16_t, uint32_t > (rnti, buffer));
            }
          else
            {
              // update the buffer size value
              (*it).second = buffer;
            }
        }
    }

  return;
}

void
schedulerDRANMac::DoSchedUlCqiInfoReq (const struct FfMacSchedSapProvider::SchedUlCqiInfoReqParameters& params)
{
	  std::cout << "------> UL CQI Info Req" << std::endl;

  NS_LOG_FUNCTION (this);
  m_ffrSapProvider->ReportUlCqiInfo (params);

// retrieve the allocation for this subframe
  switch (m_ulCqiFilter)
    {
    case FfMacScheduler::SRS_UL_CQI:
      {
        // filter all the CQIs that are not SRS based
        if (params.m_ulCqi.m_type != UlCqi_s::SRS)
          {
            return;
          }
      }
      break;
    case FfMacScheduler::PUSCH_UL_CQI:
      {
        // filter all the CQIs that are not SRS based
        if (params.m_ulCqi.m_type != UlCqi_s::PUSCH)
          {
            return;
          }
      }
    case FfMacScheduler::ALL_UL_CQI:
      break;

    default:
      NS_FATAL_ERROR ("Unknown UL CQI type");
    }

  switch (params.m_ulCqi.m_type)
    {
    case UlCqi_s::PUSCH:
      {
        std::map <uint16_t, std::vector <uint16_t> >::iterator itMap;
        std::map <uint16_t, std::vector <double> >::iterator itCqi;
        NS_LOG_DEBUG (this << " Collect PUSCH CQIs of Frame no. " << (params.m_sfnSf >> 4) << " subframe no. " << (0xF & params.m_sfnSf));
        itMap = m_allocationMaps.find (params.m_sfnSf);
        if (itMap == m_allocationMaps.end ())
          {
            return;
          }
        for (uint32_t i = 0; i < (*itMap).second.size (); i++)
          {
            // convert from fixed point notation Sxxxxxxxxxxx.xxx to double
            double sinr = LteFfConverter::fpS11dot3toDouble (params.m_ulCqi.m_sinr.at (i));
            itCqi = m_ueCqi.find ((*itMap).second.at (i)); // Buscamos el UE en el mapa de SINR
            if (itCqi == m_ueCqi.end ())
              {
                // create a new entry
                std::vector <double> newCqi;
                for (uint32_t j = 0; j < m_cschedCellConfig.m_ulBandwidth; j++)
                  {
                    if (i == j)
                      {
                        newCqi.push_back (sinr);
                      }
                    else
                      {
                        // initialize with NO_SINR value.
                        newCqi.push_back (NO_SINR);
                      }

                  }
                m_ueCqi.insert (std::pair <uint16_t, std::vector <double> > ((*itMap).second.at (i), newCqi));
                // generate correspondent timer
                m_ueCqiTimers.insert (std::pair <uint16_t, uint32_t > ((*itMap).second.at (i), m_cqiTimersThreshold));
              }
            else
              {
                // update the value
                (*itCqi).second.at (i) = sinr;
                NS_LOG_DEBUG (this << " RNTI " << (*itMap).second.at (i) << " RB " << i << " SINR " << sinr);
                // update correspondent timer
                std::map <uint16_t, uint32_t>::iterator itTimers;
                itTimers = m_ueCqiTimers.find ((*itMap).second.at (i));
                (*itTimers).second = m_cqiTimersThreshold;

              }

          }
        // remove obsolete info on allocation
        m_allocationMaps.erase (itMap);
      }
      break;
    case UlCqi_s::SRS:
      {
    	 NS_LOG_DEBUG (this << " Collect SRS CQIs of Frame no. " << (params.m_sfnSf >> 4) << " subframe no. " << (0xF & params.m_sfnSf));
        // get the RNTI from vendor specific parameters
        uint16_t rnti = 0;
        NS_ASSERT (params.m_vendorSpecificList.size () > 0);
        for (uint16_t i = 0; i < params.m_vendorSpecificList.size (); i++)
          {
            if (params.m_vendorSpecificList.at (i).m_type == SRS_CQI_RNTI_VSP)
              {
                Ptr<SrsCqiRntiVsp> vsp = DynamicCast<SrsCqiRntiVsp> (params.m_vendorSpecificList.at (i).m_value);
                rnti = vsp->GetRnti ();
              }
          }
        std::map <uint16_t, std::vector <double> >::iterator itCqi;
        itCqi = m_ueCqi.find (rnti);
        if (itCqi == m_ueCqi.end ())
          {
            // create a new entry
            std::vector <double> newCqi;
            for (uint32_t j = 0; j < m_cschedCellConfig.m_ulBandwidth; j++)
              {
                double sinr = LteFfConverter::fpS11dot3toDouble (params.m_ulCqi.m_sinr.at (j));
                newCqi.push_back (sinr);
                NS_LOG_INFO (this << " RNTI " << rnti << " new SRS-CQI for RB  " << j << " value " << sinr);

              }
            m_ueCqi.insert (std::pair <uint16_t, std::vector <double> > (rnti, newCqi));
            // generate correspondent timer
            m_ueCqiTimers.insert (std::pair <uint16_t, uint32_t > (rnti, m_cqiTimersThreshold));
          }
        else
          {
            // update the values
            for (uint32_t j = 0; j < m_cschedCellConfig.m_ulBandwidth; j++)
              {
                double sinr = LteFfConverter::fpS11dot3toDouble (params.m_ulCqi.m_sinr.at (j));
                (*itCqi).second.at (j) = sinr;
                NS_LOG_INFO (this << " RNTI " << rnti << " update SRS-CQI for RB  " << j << " value " << sinr);
              }
            // update correspondent timer
            std::map <uint16_t, uint32_t>::iterator itTimers;
            itTimers = m_ueCqiTimers.find (rnti);
            (*itTimers).second = m_cqiTimersThreshold;

          }


      }
      break;
    case UlCqi_s::PUCCH_1:
    case UlCqi_s::PUCCH_2:
    case UlCqi_s::PRACH:
      {
        NS_FATAL_ERROR ("schedulerDRANMac supports only PUSCH and SRS UL-CQIs");
      }
      break;
    default:
      NS_FATAL_ERROR ("Unknown type of UL-CQI");
    }
  return;
}

void
schedulerDRANMac::RefreshDlCqiMaps (void)
{
	  std::cout << "------> Refresh DL CQI Maps" << std::endl;

  // refresh DL CQI P10 Map
  std::map <uint16_t,uint32_t>::iterator itP10 = m_p10CqiTimers.begin ();
  while (itP10 != m_p10CqiTimers.end ())
    {
      NS_LOG_INFO (this << " P10-CQI for user " << (*itP10).first << " is " << (uint32_t)(*itP10).second << " thr " << (uint32_t)m_cqiTimersThreshold);
      if ((*itP10).second == 0)
        {
          // delete correspondent entries
          std::map <uint16_t,uint8_t>::iterator itMap = m_p10CqiRxed.find ((*itP10).first);
          NS_ASSERT_MSG (itMap != m_p10CqiRxed.end (), " Does not find CQI report for user " << (*itP10).first);
          NS_LOG_INFO (this << " P10-CQI expired for user " << (*itP10).first);
          m_p10CqiRxed.erase (itMap);
          std::map <uint16_t,uint32_t>::iterator temp = itP10;
          itP10++;
          m_p10CqiTimers.erase (temp);
        }
      else
        {
          (*itP10).second--;
          itP10++;
        }
    }

  // refresh DL CQI A30 Map
  std::map <uint16_t,uint32_t>::iterator itA30 = m_a30CqiTimers.begin ();
  while (itA30 != m_a30CqiTimers.end ())
    {
      NS_LOG_INFO (this << " A30-CQI for user " << (*itA30).first << " is " << (uint32_t)(*itA30).second << " thr " << (uint32_t)m_cqiTimersThreshold);
      if ((*itA30).second == 0)
        {
          // delete correspondent entries
          std::map <uint16_t,SbMeasResult_s>::iterator itMap = m_a30CqiRxed.find ((*itA30).first);
          NS_ASSERT_MSG (itMap != m_a30CqiRxed.end (), " Does not find CQI report for user " << (*itA30).first);
          NS_LOG_INFO (this << " A30-CQI expired for user " << (*itA30).first);
          m_a30CqiRxed.erase (itMap);
          std::map <uint16_t,uint32_t>::iterator temp = itA30;
          itA30++;
          m_a30CqiTimers.erase (temp);
        }
      else
        {
          (*itA30).second--;
          itA30++;
        }
    }

  return;
} // Decrementa el mapa de timers de UE en DL CQI P10 y A30 RX, cuando llega a 0 elimina el CQI del mapa de UEs-CQIs


void
schedulerDRANMac::RefreshUlCqiMaps (void)
{
	  std::cout << "------> Refresh UL CQI Maps" << std::endl;

  // refresh UL CQI  Map
  std::map <uint16_t,uint32_t>::iterator itUl = m_ueCqiTimers.begin ();
  while (itUl != m_ueCqiTimers.end ())
    {
      NS_LOG_INFO (this << " UL-CQI for user " << (*itUl).first << " is " << (uint32_t)(*itUl).second << " thr " << (uint32_t)m_cqiTimersThreshold);
      if ((*itUl).second == 0)
        {
          // delete correspondent entries
          std::map <uint16_t, std::vector <double> >::iterator itMap = m_ueCqi.find ((*itUl).first);
          NS_ASSERT_MSG (itMap != m_ueCqi.end (), " Does not find CQI report for user " << (*itUl).first);
          NS_LOG_INFO (this << " UL-CQI exired for user " << (*itUl).first);
          (*itMap).second.clear ();
          m_ueCqi.erase (itMap);
          std::map <uint16_t,uint32_t>::iterator temp = itUl;
          itUl++;
          m_ueCqiTimers.erase (temp);
        }
      else
        {
          (*itUl).second--;
          itUl++;
        }
    }

  return;
} // Idem al anterior pero con el UL

void
schedulerDRANMac::UpdateDlRlcBufferInfo (uint16_t rnti, uint8_t lcid, uint16_t size)
{
	  std::cout << "------> Update DL RLC Buffer Info" << std::endl;

  std::map<LteFlowId_t, FfMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it;
  LteFlowId_t flow (rnti, lcid);
  it = m_rlcBufferReq.find (flow);
  if (it != m_rlcBufferReq.end ())
    {
      NS_LOG_INFO (this << " UE " << rnti << " LC " << (uint16_t)lcid << " txqueue " << (*it).second.m_rlcTransmissionQueueSize << " retxqueue " << (*it).second.m_rlcRetransmissionQueueSize << " status " << (*it).second.m_rlcStatusPduSize << " decrease " << size);
      // Update queues: RLC tx order Status, ReTx, Tx
      // Update status queue
      if (((*it).second.m_rlcStatusPduSize > 0) && (size >= (*it).second.m_rlcStatusPduSize))
        {
          (*it).second.m_rlcStatusPduSize = 0;
        }
      else if (((*it).second.m_rlcRetransmissionQueueSize > 0) && (size >= (*it).second.m_rlcRetransmissionQueueSize))
        {
          (*it).second.m_rlcRetransmissionQueueSize = 0;
        }
      else if ((*it).second.m_rlcTransmissionQueueSize > 0)
        {
          uint32_t rlcOverhead;
          if (lcid == 1)
            {
              // for SRB1 (using RLC AM) it's better to
              // overestimate RLC overhead rather than
              // underestimate it and risk unneeded
              // segmentation which increases delay 
              rlcOverhead = 4;
            }
          else
            {
              // minimum RLC overhead due to header
              rlcOverhead = 2;
            }
          // update transmission queue
          if ((*it).second.m_rlcTransmissionQueueSize <= size - rlcOverhead)
            {
              (*it).second.m_rlcTransmissionQueueSize = 0;
            }
          else
            {
              (*it).second.m_rlcTransmissionQueueSize -= size - rlcOverhead;
            }
        }
    }
  else
    {
      NS_LOG_ERROR (this << " Does not find DL RLC Buffer Report of UE " << rnti);
    }
}

void
schedulerDRANMac::UpdateUlRlcBufferInfo (uint16_t rnti, uint16_t size)
{
	  std::cout << "------> Update UL RLC Buffer Info" << std::endl;

  size = size - 2; // remove the minimum RLC overhead
  std::map <uint16_t,uint32_t>::iterator it = m_ceBsrRxed.find (rnti);
  if (it != m_ceBsrRxed.end ())
    {
      NS_LOG_INFO (this << " UE " << rnti << " size " << size << " BSR " << (*it).second);
      if ((*it).second >= size)
        {
          (*it).second -= size;
        }
      else
        {
          (*it).second = 0;
        }
    }
  else
    {
      NS_LOG_ERROR (this << " Does not find BSR report info of UE " << rnti);
    }

}

void
schedulerDRANMac::TransmissionModeConfigurationUpdate (uint16_t rnti, uint8_t txMode)
{
	  std::cout << "------> TX Mode Conf Update" << std::endl;

  NS_LOG_FUNCTION (this << " RNTI " << rnti << " txMode " << (uint16_t)txMode);
  FfMacCschedSapUser::CschedUeConfigUpdateIndParameters params;
  params.m_rnti = rnti;
  params.m_transmissionMode = txMode;
  m_cschedSapUser->CschedUeConfigUpdateInd (params);
}


}
