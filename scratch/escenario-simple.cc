#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/lte-module.h>
#include "ns3/config-store.h"
#include <ns3/lte-enb-rrc.h>
//#include <ns3/flow-monitor.h>
//#include <ns3/flow-monitor-helper.h>

using namespace ns3;

void Hola()
{
	std::cout<<"hola"<<std::endl;
}

//void
//GetLtePhy(NetDeviceContainer ueDevs)
//{
//	Ptr<LtePhy> ue1Phy = ueDevs.Get(0)->GetObject<LteUeNetDevice>()->GetPhy()->GetObject<LtePhy>();
//	Ptr<LteSpectrumPhy> downLinkSpectrum1 = ue1Phy->GetDownlinkSpectrumPhy();
//	SpectrumValue sinr1 = downLinkSpectrum1->GetSinrPerceived();
//	Ptr<LteSpectrumPhy> UpLinkSpectrum1 = ue1Phy->GetUplinkSpectrumPhy();
//	SpectrumValue sinr2 = UpLinkSpectrum1->GetSinrPerceived();
//	Ptr<LtePhy> ue2Phy = ueDevs.Get(1)->GetObject<LteUeNetDevice>()->GetPhy()->GetObject<LtePhy>();
//	Ptr<LteSpectrumPhy> downLinkSpectrum2 = ue2Phy->GetDownlinkSpectrumPhy();
//	SpectrumValue sinr3 = downLinkSpectrum2->GetSinrPerceived();
//	Ptr<LteSpectrumPhy> upLinkSpectrum2 = ue2Phy->GetUplinkSpectrumPhy();
//	SpectrumValue sinr4 = upLinkSpectrum2->GetSinrPerceived();
//    std::cout<<Simulator::Now()<<std::endl;
//}


static ns3::GlobalValue g_NsubbandsUser ("NsubbandsUser",
                                   	     "Número subbandas por usuario",
									     ns3::IntegerValue (1),
									     ns3::MakeIntegerChecker<int16_t> ());

static ns3::GlobalValue g_ULDL ("ULDL",
                                "UL/DL seleccion para ejecucion de Scheduler",
								ns3::IntegerValue (0),
								ns3::MakeIntegerChecker<int16_t> ());

int main (int argc, char *argv[])
{

//	//Parámetros de entrada que queremos cargar a la simulación
	  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("input-escenario-simple.txt"));
	  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Load"));
	  ConfigStore inputConfig;
	  inputConfig.ConfigureDefaults ();

	  // Configuracion parametros por defecto
//	  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (100));
//	  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (100));
	  Config::SetDefault ("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(LteEnbRrc::RLC_SM_ALWAYS)); // Configuracion full-buffer

	  // Recogemos las variables del fichero de entrada que no sean variables por defecto
	  IntegerValue integerValue;
	  GlobalValue::GetValueByName ("NsubbandsUser", integerValue);
	  int16_t NsubbandsUser = integerValue.Get ();
	  GlobalValue::GetValueByName ("ULDL", integerValue);
	  int16_t ULDL = integerValue.Get ();

//	    schedulerDRANMac::Nsubbands_user=NsubbandsUser;
	  Config::SetDefault ("ns3::schedulerDRANMac::MaxSubbandsUE", UintegerValue (NsubbandsUser));
	    LteEnbMac::ULDLselection=ULDL; // DL - 0 ; UL - 1

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  lteHelper->SetFadingModel("ns3::TraceFadingLossModel");
  lteHelper->SetFadingModelAttribute ("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
  lteHelper->SetFadingModelAttribute ("TraceLength", TimeValue (Seconds (10.0)));
  lteHelper->SetFadingModelAttribute ("SamplesNum", UintegerValue (10000));
  lteHelper->SetFadingModelAttribute ("WindowSize", TimeValue (Seconds (0.5)));
  lteHelper->SetFadingModelAttribute ("RbNum", UintegerValue (100));

  // Seleccionamos el tipo de scheduler
  lteHelper->SetSchedulerType("ns3::schedulerDRANMac"); // Distribuido
//  lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");
//  Config::SetDefault ("ns3::PfFfMacScheduler::HarqEnabled", BooleanValue (false));
//  lteHelper->SetSchedulerType("ns3::RrFfMacSchedulerComparative"); // Round robin
//  Config::SetDefault ("ns3::RrFfMacSchedulerComparative::HarqEnabled", BooleanValue (false));


  ////////// PRUEBA CLUSTERS
  uint16_t numberOfClusters = 2;
  uint16_t nodesPerCluster = 2;
  uint16_t numberOfUeNodes = 8;

//  NodeContainer enbNodes;
//  enbNodes.Create (3);
//  NodeContainer ueNodes;
//  ueNodes.Create (8);

  NodeContainer ueNodes;
  NodeContainer enbNodes;
  //plus one control station per cluster
  enbNodes.Create((nodesPerCluster + 1) * numberOfClusters);
  ueNodes.Create(numberOfUeNodes);

//  MobilityHelper mobility;
//  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
//  mobility.Install (enbNodes);
//  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
//  mobility.Install (ueNodes);
//
//  Ptr<Node> enb = enbNodes.Get(0);
//  Ptr<MobilityModel> mm = enb->GetObject<MobilityModel>();
//  mm->SetPosition (Vector(-250,0,30));
//  Ptr<Node> enb2 = ueNodes.Get(1);
//  mm = enb2->GetObject<MobilityModel>();
//  mm->SetPosition (Vector(250,0,30));
//  Ptr<Node> enb3 = ueNodes.Get(2);
//  mm = enb3->GetObject<MobilityModel>();
//  mm->SetPosition (Vector(0,433,30));

  // Install Mobility Model
  double distance=60.0;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < enbNodes.GetN(); i++)
    {
      positionAlloc->Add (Vector(distance * i, 0, 0));
    }
  MobilityHelper mobility;
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(enbNodes);
  mobility.Install(ueNodes);
//
//  //Las colocamos para que caigan aleatoriamente dentro del circulo de radio la distancia maxima entre el centro y un site
//   Ptr<UniformDiscPositionAllocator> randomUePositionAlloc = CreateObject<UniformDiscPositionAllocator> ();
//   Ptr<UniformRandomVariable> theta = CreateObject<UniformRandomVariable> ();
//   randomUePositionAlloc->SetAttribute ("rho", DoubleValue (300));
//   randomUePositionAlloc->SetAttribute ("X", DoubleValue (0));
//   randomUePositionAlloc->SetAttribute ("Y", DoubleValue (433/2));
//
//   mobility.SetPositionAllocator (randomUePositionAlloc);
//   mobility.Install (ueNodes);
//
//   // Fijamos las posicion de los UEs a 1.5m
//   for (uint16_t i=0; i<ueNodes.GetN(); i++)
//   {
// 	  Ptr<Node> ue = ueNodes.Get(i);
// 	  Ptr<MobilityModel> mm = ue->GetObject<MobilityModel>();
// 	  Vector3D pos = mm->GetPosition ();
// 	  mm->SetPosition (Vector(pos.x,pos.y,1.5));
// //	  pos.z=1.5;
//   }

  // Install LTE Devices to the nodes
  NetDeviceContainer enbDevs = lteHelper->InstallClusterEnbDevice (enbNodes, numberOfClusters, nodesPerCluster);
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);


//
//  NetDeviceContainer enbDevs;
//  enbDevs = lteHelper->InstallEnbDevice (enbNodes);
//
//  NetDeviceContainer ueDevs;
//  ueDevs = lteHelper->InstallUeDevice (ueNodes);
//
//  lteHelper->Attach (ueDevs.Get(0), enbDevs.Get(0));
//  lteHelper->Attach (ueDevs.Get(4), enbDevs.Get(0));
//  lteHelper->Attach (ueDevs.Get(5), enbDevs.Get(0));
//
//  lteHelper->Attach (ueDevs.Get(1), enbDevs.Get(1));
//  lteHelper->Attach (ueDevs.Get(2), enbDevs.Get(1));
//  lteHelper->Attach (ueDevs.Get(7), enbDevs.Get(1));
//
//  lteHelper->Attach (ueDevs.Get(6), enbDevs.Get(0));
//  lteHelper->Attach (ueDevs.Get(3), enbDevs.Get(0));

//  lteHelper->Attach(ueDevs,enbDevs);

    enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VIDEO;
    EpsBearer bearer (q);
    lteHelper->ActivateDataRadioBearer (ueDevs, bearer);


  double time = 0.044;
  while (time<1)
  {
    Time t = Seconds (time);
    Simulator::Schedule(t, &Hola);
    time += 0.001;
  }
  

  //Guardamos la configuracion de la simulación en un fichero de texto
    Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("output-escenario-simple.txt"));
    Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Save"));
    ConfigStore outputConfig2;
    outputConfig2.ConfigureDefaults ();
    outputConfig2.ConfigureAttributes ();

    int simTime=1; // 1000 subtramas
//    double statsStartTime = 0; // need to allow for RRC connection establishment + SRS
    double statsDuration = 0.001;
//    double tolerance = 0.1;

    lteHelper->EnableMacTraces ();
    lteHelper->EnableRlcTraces ();
    lteHelper->EnablePhyTraces ();
    lteHelper->EnablePdcpTraces();

    Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();
//    rlcStats->SetAttribute ("StartTime", TimeValue (Seconds (statsStartTime)));
    rlcStats->SetAttribute ("EpochDuration", TimeValue (Seconds (statsDuration)));



    std::cout << Simulator::Now() << std::endl;

//  double simTime=0.6;

//  Simulator::Stop (Seconds (simTime));


//  Simulator::Stop (Seconds (statsStartTime + statsDuration - 0.000001));

    Simulator::Stop (Seconds(simTime));


  Simulator::Run ();

//  NS_LOG_INFO ("DL - Test with " << m_nUser << " user(s)");
//  std::vector <uint64_t> dlDataRxed;
//  double totalData = 0;
//  double totalEstThrPf = 0;
//  for (int i = 0; i < m_nUser; i++)
//  {
//	   // get the imsi
//	   uint64_t imsi = ueDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetImsi ();
//	   // get the lcId
//	  uint8_t lcId = 3;
//	  dlDataRxed.push_back (rlcStats->GetDlRxData (imsi, lcId));
//	  totalData += (double)dlDataRxed.at (i);
//	  NS_LOG_INFO ("\tUser " << i << " dist " << m_dist.at (i) << " imsi " << imsi << " bytes rxed " << (double)dlDataRxed.at (i) << "  thr " << (double)dlDataRxed.at (i) / statsDuration);
//	  totalEstThrPf += m_estThrPfDl.at (i);
//  }
//
//  NS_LOG_INFO ("UL - Test with " << m_nUser);
//  std::vector <uint64_t> ulDataRxed;
//  for (int i = 0; i < m_nUser; i++)
//  {
//	  // get the imsi
//	   uint64_t imsi = ueDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetImsi ();
//	  // get the lcId
//	   uint8_t lcId = 3;
//	  ulDataRxed.push_back (rlcStats->GetUlRxData (imsi, lcId));
//	  NS_LOG_INFO ("\tUser " << i << " dist " << m_dist.at (i) << " bytes rxed " << (double)ulDataRxed.at (i) << "  thr " << (double)ulDataRxed.at (i) / statsDuration << " ref " << (double)m_estThrPfUl.at (i));
//	  NS_TEST_ASSERT_MSG_EQ_TOL ((double)ulDataRxed.at (i) / statsDuration, (double)m_estThrPfUl.at (i), (double)m_estThrPfUl.at (i) * tolerance, " Unfair Throughput!");
//  }

//   int m_users=ueNodes.GetN();
//   for (int i = 0; i < m_users; i++)
//     {
//       // get the imsi
//       uint64_t imsi = ueDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetImsi ();
//       uint8_t lcId = 3; // Canal logico asociado a la dedicated bearer que hemos definido de video.
//
//       double dlRxPackets = rlcStats->GetDlRxPackets (imsi, lcId);
//       double dlTxPackets = rlcStats->GetDlTxPackets (imsi, lcId);
//       double dlBler =  1.0 - (dlRxPackets/dlTxPackets);
////       double expectedDlRxPackets = dlTxPackets -dlTxPackets*m_blerRef;
//       std::cout << "\tUser " << i << " imsi " << imsi << " DOWNLINK"
//                    << " pkts rx " << dlRxPackets << " tx " << dlTxPackets
//                    << " BLER " << dlBler // << " Err " << std::fabs (m_blerRef - dlBler)
////                    << " expected rx " << expectedDlRxPackets
////                    << " difference " << std::abs (expectedDlRxPackets - dlRxPackets)
////                    << " tolerance " << m_toleranceRxPackets
//					<< std::endl;

       // sanity check for whether the tx packets reported by the stats are correct
       // we expect one packet per TTI
//       double expectedDlTxPackets = statsDuration.GetMilliSeconds ();
//       NS_TEST_ASSERT_MSG_EQ_TOL (dlTxPackets, expectedDlTxPackets, expectedDlTxPackets * 0.005,
//                      airs            " too different DL TX packets reported");

       // this is the main test condition: check that the RX packets are within the expected range
//       NS_TEST_ASSERT_MSG_EQ_TOL (dlRxPackets, expectedDlRxPackets, m_toleranceRxPackets,
//                                  " too different DL RX packets reported");
//     }



   std::cout << " ********** FIN *********" << std::endl;

  Simulator::Destroy ();
  return 0;
}

