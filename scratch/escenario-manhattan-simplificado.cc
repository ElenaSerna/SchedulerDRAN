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
//  lteHelper->SetSchedulerType("ns3::RrFfMacSchedulerComparative"); // Round robin
//  Config::SetDefault ("ns3::RrFfMacScheduler::HarqEnabled", BooleanValue (false));


  NodeContainer enbNodes;
  enbNodes.Create (25);
  NodeContainer ueNodes;
  ueNodes.Create (50);

  MobilityHelper mobility;
  // Posicion ENBs

  Ptr<RandomBoxPositionAllocator> randomENBPositionAlloc = CreateObject<RandomBoxPositionAllocator> ();
  Ptr<UniformRandomVariable> xValenb = CreateObject<UniformRandomVariable> ();
  xValenb->SetAttribute ("Min", DoubleValue (-200));
  xValenb->SetAttribute ("Max", DoubleValue (200));
  randomENBPositionAlloc->SetAttribute ("X", PointerValue (xValenb));
  Ptr<UniformRandomVariable> yValenb = CreateObject<UniformRandomVariable> ();
  yValenb->SetAttribute ("Min", DoubleValue (-200));
  yValenb->SetAttribute ("Max", DoubleValue (200));
  randomENBPositionAlloc->SetAttribute ("Y", PointerValue (yValenb));
  Ptr<UniformRandomVariable> zValenb = CreateObject<UniformRandomVariable> ();
  zValenb->SetAttribute ("Min", DoubleValue (3));
  zValenb->SetAttribute ("Max", DoubleValue (3));
  randomENBPositionAlloc->SetAttribute ("Z", PointerValue (zValenb));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
/*  mobilityUes.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
			     "Speed", StringValue ("ns3::UniformRandomVariable[Min=99.0|Max=100]"),
			     "Bounds", RectangleValue (Rectangle((-distanceStreet/2)+(ii*(distanceBuilding + distanceStreet)), (distanceStreet/2)+(ii*(distanceBuilding + distanceStreet)), (-distanceStreet/2)-((nGrid/2)*(distanceBuilding+distanceStreet)), (distanceStreet/2)+((nGrid/2)*(distanceBuilding+distanceStreet))))); */

  mobility.SetPositionAllocator (randomENBPositionAlloc);
  mobility.Install (enbNodes);



  // Posicion UEs

    mobility.Install (ueNodes);
    mobility.Install (enbNodes);

    Ptr<RandomBoxPositionAllocator> randomUePositionAlloc = CreateObject<RandomBoxPositionAllocator> ();
    Ptr<UniformRandomVariable> xVal = CreateObject<UniformRandomVariable> ();
    xVal->SetAttribute ("Min", DoubleValue (-500));
    xVal->SetAttribute ("Max", DoubleValue (500));
    randomUePositionAlloc->SetAttribute ("X", PointerValue (xVal));
    Ptr<UniformRandomVariable> yVal = CreateObject<UniformRandomVariable> ();
    yVal->SetAttribute ("Min", DoubleValue (-500));
    yVal->SetAttribute ("Max", DoubleValue (500));
    randomUePositionAlloc->SetAttribute ("Y", PointerValue (yVal));
    Ptr<UniformRandomVariable> zVal = CreateObject<UniformRandomVariable> ();
    zVal->SetAttribute ("Min", DoubleValue (1.5));
    zVal->SetAttribute ("Max", DoubleValue (1.5));
    randomUePositionAlloc->SetAttribute ("Z", PointerValue (zVal));
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  /*  mobilityUes.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
  			     "Speed", StringValue ("ns3::UniformRandomVariable[Min=99.0|Max=100]"),
  			     "Bounds", RectangleValue (Rectangle((-distanceStreet/2)+(ii*(distanceBuilding + distanceStreet)), (distanceStreet/2)+(ii*(distanceBuilding + distanceStreet)), (-distanceStreet/2)-((nGrid/2)*(distanceBuilding+distanceStreet)), (distanceStreet/2)+((nGrid/2)*(distanceBuilding+distanceStreet))))); */

    mobility.SetPositionAllocator (randomUePositionAlloc);
    mobility.Install (ueNodes);







    // Attach a la mejor servidora

    //Instala el protocolo LTE en las UEs y eNBs
    NetDeviceContainer ueDevs;
//    ueDevs = lteHelper->InstallUeDevice (ueNode  NetDeviceContainer ueDevs;
    ueDevs = lteHelper->InstallUeDevice (ueNodes);
    NetDeviceContainer enbDevs;
    enbDevs = lteHelper->InstallEnbDevice (enbNodes);



  // Attach a la mas cercana
  //Vincula las UEs con las eNBs. Esto configurará cada UE de acuerdo a la configuración de las eNBs, y crea una conexión RRC entre ellos
  lteHelper->AttachToClosestEnb(ueDevs, enbDevs);

  //Activa la portadora de radio entra la UE y la eNB vinculada
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  lteHelper->ActivateDataRadioBearer (ueDevs, bearer);


//    //Calculo las perdidas entre base y usuario y attach.
//    	double loss;
//    	double BestLoss=1000000;
//    	uint32_t best;
//
//    	//Bucle para recorrer cada uno de los usuarios y obtener su potencia
//    	for(uint32_t jj=0;jj<ueNodes.GetN();jj++)
//    	{
//      	  Ptr<Node> nodeUe = ueNodes.Get(jj);
//      	  Ptr<MobilityModel> mmUe = nodeUe->GetObject<MobilityModel> ();
//    	  //vamos variando la celda para un usuario fijo para ver cual es la mejor servidora y la sinr que tendria conectado con cada una de ellas
//    	  for(uint32_t ii=0;ii<enbNodes.GetN();ii++)
//    	  {
//      	    Ptr<Node> nodeEnb = enbNodes.Get(ii);
//      	    Ptr<MobilityModel> mmEnb = nodeEnb->GetObject<MobilityModel> ();
//      	    loss = lteHelper->GetLossDl(mmEnb,mmUe);
//    	    //Comprobamos si las perdidas de la nuevas celda son menores que las encontradas hasta el momento
////      	    std::cout<<"usuario "<<jj<<"  celda "<<ii<<"  loss--->"<<loss<<"  BestLoss-->"<<BestLoss<<std::endl;
//    	    if (loss<BestLoss)
//    	    {
//    	      BestLoss=loss;
//    	      best = ii;
//            }
//    	  }
//    	  BestLoss=1000000;
//
////    	  std::cout<<std::endl<<std::endl<<std::endl<<BestLoss<<"     "<<"mejor--->"<<best<<std::endl<<std::endl;
//    	  //Vincula las UEs con la eNBs de la que tiene menos perdidas. Esto configurará cada UE de acuerdo a la configuración de las eNBs, y crea una conexión RRC entre ellos
//              lteHelper->Attach(ueDevs.Get(jj), enbDevs.Get(best));
//
//      	  //Activa la portadora de radio entra la UE y la eNB vinculada
//      	  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VIDEO;
//      	  EpsBearer bearer (q);
//      	  lteHelper->ActivateDataRadioBearer (ueDevs.Get(jj), bearer);
//    	}


  double time = 0.020;
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

    int simTime=5; // 1000 subtramas
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

