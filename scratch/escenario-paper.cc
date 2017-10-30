#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/lte-module.h>
#include "ns3/config-store.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
  // Command line arguments
  CommandLine cmd;
  cmd.Parse (argc, argv);

  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue(43));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(LteEnbRrc::RLC_SM_ALWAYS)); // Configuracion full-buffer


  //Crea un objeto lteHelper
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

    lteHelper->SetFadingModel("ns3::TraceFadingLossModel");
    lteHelper->SetFadingModelAttribute ("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
    lteHelper->SetFadingModelAttribute ("TraceLength", TimeValue (Seconds (10.0)));
    lteHelper->SetFadingModelAttribute ("SamplesNum", UintegerValue (10000));
    lteHelper->SetFadingModelAttribute ("WindowSize", TimeValue (Seconds (0.5)));
    lteHelper->SetFadingModelAttribute ("RbNum", UintegerValue (100));

//    schedulerDRANMac::Nsubbands_user=25;
    LteEnbMac::ULDLselection=0; // DL - 0 ; UL - 1

    // Seleccionamos el tipo de scheduler
    lteHelper->SetSchedulerType("ns3::schedulerDRANMac"); // Distribuido
  //  lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");
  //  Config::SetDefault ("ns3::PfFfMacScheduler::HarqEnabled", BooleanValue (false));
//    lteHelper->SetSchedulerType("ns3::RrFfMacSchedulerComparative"); // Round robin
//    Config::SetDefault ("ns3::RrFfMacSchedulerComparative::HarqEnabled", BooleanValue (false));


  //Crea los nodos para las eNBs
    NodeContainer enbNodes;
    enbNodes.Create (7*3);

  //Configuramos la distribución de las eNBs
    double dy = std::sqrt((500*500)-(250*250)); //distancia entre el origen y la primera linea de eNBs
    uint32_t contador=0;
    double antennaOrientation = 0;
    double x = 0;
    double y = 0;
    Vector pos = {0,0,0};
    MobilityHelper mobility;
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (enbNodes);
    lteHelper->SetEnbAntennaModelType ("ns3::ParabolicAntennaModel");
    NetDeviceContainer enbDevs;

    for (int fila=-1;fila<2;fila++)
    {
      if(fila==0) //Fila central 3 enbs
      {
        for (x=-1;x<2;x=x+1)
        {
          antennaOrientation = -180;
          for (uint32_t j=1;j<=3;j++)
          {
      	      y=0;
              pos =Vector(x*500, y, 30);
      	      Ptr<Node> node = enbNodes.Get(contador);
              Ptr<MobilityModel> mm = node->GetObject<MobilityModel> ();
              mm->SetPosition (Vector (pos));
              lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (antennaOrientation));
              enbDevs.Add (lteHelper->InstallEnbDevice (node));
              antennaOrientation = antennaOrientation + 120;
              contador++;
          }
        }

      }
      else //fila superior e inferior 2 enbs
    	  for (double x = -0.5;x<=0.5;x=x+1)
    	  {
            antennaOrientation = -180;
            for (uint32_t j=1;j<=3;j++)
            {
         	  y=fila*dy;
              pos =Vector(x*500, y, 30);
    	      Ptr<Node> node = enbNodes.Get(contador);
              Ptr<MobilityModel> mm = node->GetObject<MobilityModel> ();
              mm->SetPosition (Vector (pos));
              lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (antennaOrientation));
              enbDevs.Add (lteHelper->InstallEnbDevice (node));
              antennaOrientation = antennaOrientation + 120;
              contador++;
            }
    	  }
      }

  //Configuramos los UEs
    for(uint32_t ii=0;ii<enbNodes.GetN();ii++)
    {
    	Ptr<RandomDiscPositionAllocator> randomUePositionAlloc = CreateObject<RandomDiscPositionAllocator> ();
    	Ptr<UniformRandomVariable> theta = CreateObject<UniformRandomVariable> ();
    	Ptr<UniformRandomVariable> rho = CreateObject<UniformRandomVariable> ();
    	rho->SetAttribute ("Min", DoubleValue (0));
    	rho->SetAttribute ("Max", DoubleValue (250));
    	randomUePositionAlloc->SetAttribute ("Rho", PointerValue (rho));
    	switch(ii%3)
    	{
    	case 0:
    	  theta->SetAttribute ("Min", DoubleValue (120*3.1415/180));
    	  theta->SetAttribute ("Max", DoubleValue (240*3.1415/180));
    	  randomUePositionAlloc->SetAttribute ("Theta", PointerValue (theta));
    	  break;

    	case 1:
      	  theta->SetAttribute ("Min", DoubleValue (0*3.1415/180));
      	  theta->SetAttribute ("Max", DoubleValue (120*3.1415/180));
      	  randomUePositionAlloc->SetAttribute ("Theta", PointerValue (theta));
      	  break;

    	case 2:
    	  theta->SetAttribute ("Min", DoubleValue (240*3.1415/180));
    	  theta->SetAttribute ("Max", DoubleValue (360*3.1415/180));
    	  randomUePositionAlloc->SetAttribute ("Theta", PointerValue (theta));
    	  break;
    	}
    Ptr<MobilityModel> mob = enbNodes.Get(ii)->GetObject<MobilityModel>();
    Vector m_position = mob->GetPosition();
    randomUePositionAlloc->SetAttribute("X", DoubleValue (m_position.x));
    randomUePositionAlloc->SetAttribute("Y", DoubleValue (m_position.y));
    mobility.SetPositionAllocator (randomUePositionAlloc);
    NodeContainer ueNodes;
    ueNodes.Create (20);
    mobility.Install (ueNodes);
	if(ii==11)
	{
		//Nodo 1
		Ptr<Node> nodo1=ueNodes.Get(0);
		Ptr<MobilityModel> mm1 = nodo1->GetObject<MobilityModel>();
		Vector position1 = Vector(30,20,30);
		mm1->SetPosition(position1);

		//Nodo2
		Ptr<Node> nodo2=ueNodes.Get(1);
		Ptr<MobilityModel> mm2 = nodo2->GetObject<MobilityModel>();
		Vector position2 = Vector(80,90,30);
		mm2->SetPosition(position2);

		//Nodo3
		Ptr<Node> nodo3=ueNodes.Get(2);
		Ptr<MobilityModel> mm3 = nodo3->GetObject<MobilityModel>();
		Vector position3 = Vector(150,120,30);
		mm3->SetPosition(position3);

		//Nodo4
		Ptr<Node> nodo4=ueNodes.Get(3);
		Ptr<MobilityModel> mm4 = nodo4->GetObject<MobilityModel>();
		Vector position4 = Vector(270,220,30);
		mm4->SetPosition(position4);
	}
    //Instala el protocolo LTE en las UEs
    NetDeviceContainer ueDevs;
    ueDevs = lteHelper->InstallUeDevice(ueNodes);
    //Vincula las UEs con las eNBs
    lteHelper->Attach(ueDevs, enbDevs.Get(ii));

    //Activa la portadora de radio entre la UE y la eNB vinculada
    enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VIDEO;
    EpsBearer bearer (q);
    lteHelper->ActivateDataRadioBearer(ueDevs, bearer);
    }



  AnimationInterface anim ("anim-lte-paper.xml");

  double statsDuration = 0.001;
//    double tolerance = 0.1;

  lteHelper->EnableMacTraces ();
  lteHelper->EnableRlcTraces ();
  lteHelper->EnablePhyTraces ();
  lteHelper->EnablePdcpTraces();

  Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();
//    rlcStats->SetAttribute ("StartTime", TimeValue (Seconds (statsStartTime)));
  rlcStats->SetAttribute ("EpochDuration", TimeValue (Seconds (statsDuration)));


  Simulator::Stop(Seconds (1.092));

  //Lanzamos la simulacion
  Simulator::Run();

  std::cout << " ********** FIN *********" << std::endl;

  //Terminamos la simulacion
  Simulator::Destroy ();

  return 0;
}








