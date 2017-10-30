#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/lte-module.h>
#include "ns3/config-store.h"
#include "ns3/netanim-module.h"
#include <ns3/mobility-building-info.h>
#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/buildings-module.h>
#include <ns3/buildings-helper.h>
#include <ns3/hybrid-buildings-propagation-loss-model.h>


/* Distribucion del ecenario canónico (3 anillos) con 27 sites y 3 sectores por site.
Los usuarios se distribullen de forma aleatoria en un unico circulo que engloba todos los sectores.
Para enlazar cada usuario con su sector se calcula en primer lugar el site más cercano y posteriormente se ve a que sector (orientacion de antena) corresponde.
*/

//#include <locale>
//#include <sstream>
//#include <string>
//#include <math.h>
//#include <cmath> 
//#include <ns3/double.h>
//#include <ns3/log.h>
//#include <ns3/abort.h>
//#include <ns3/pointer.h>
//#include <ns3/epc-helper.h>
//#include <iostream>
using namespace ns3;


//Funcion para generar un archivo de texto para graficar la posicion de los usuarios en el gnuplot
void 
PrintGnuplottableUeListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  /*if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }*/
  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
    {
      Ptr<Node> node = *it;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; j++)
        {
          Ptr<LteUeNetDevice> uedev = node->GetDevice (j)->GetObject <LteUeNetDevice> ();
          if (uedev)
            {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "set label \"" << uedev->GetImsi ()
                      << "\" at "<< pos.x << "," << pos.y << " left font \"Helvetica,4\" textcolor rgb \"grey\" front point pt 1 ps 0.3 lc rgb \"grey\" offset 0,0"
                      << std::endl;
            }
        }
    }
}


// Funcion para calcular la matriz de SNR global de todos los enlaces cruzados
// Dicha matriz se actualizara en cada subtrama, cada 1ms
void
DlSNRmatrix(Ptr<LteHelper> lteHelper, NodeContainer ueNodes, NodeContainer enbNodes)
{
//Calculo de la matriz de SNR para el downlink

  Ptr<HybridBuildingsPropagationLossModel> HybridBuildings = lteHelper->GetHybridBuildings();
  Ptr<SpectrumPropagationLossModel> FadingModule = lteHelper->GetFadingModule ();
  Ptr<TraceFadingLossModel> Fading = FadingModule->GetObject<TraceFadingLossModel> ();

  //Creo el spectrum model y el espectrum value para el downlink
  Ptr<SpectrumModel> sm;

  Bands bands;
  BandInfo bi;

  double freqLowDl = 2111000000;
  double freqHighDl = 2129000000;

  for (int freq = freqLowDl; freq<freqHighDl; freq = freq + 180000)
  {
	  bi.fl = freq;
	  bi.fc = freq+90000;
	  bi.fh = freq+180000;
	  bands.push_back (bi);
  }
  sm = Create<SpectrumModel> (bands);

  SNR=new double **[ueNodes.GetN()];
  for (uint32_t i = 0; i<ueNodes.GetN();i++)
  {
	  SNR[i]=new double *[enbNodes.GetN()];
	  for (uint32_t j = 0; j<enbNodes.GetN(); j++)
	  {
		  SNR[i][j]=new double [100];
	  }
  }

  double powerShadowing;
  for (uint32_t i=0; i<ueNodes.GetN(); i++)
  {
  	  Ptr<Node> nodeUe = ueNodes.Get(i);
  	  Ptr<MobilityModel> mmUe = nodeUe->GetObject<MobilityModel> ();
	  for (uint32_t j=0; j<enbNodes.GetN(); j++)
	  {
	  	  Ptr<Node> nodeEnb = enbNodes.Get(j);
	  	  Ptr<MobilityModel> mmEnb = nodeEnb->GetObject<MobilityModel> ();

	  	  //Obtengo el valor del shadowing y el pathlos y lo convierto a densidad espectral de potencia para cada RB
	  	  powerShadowing=HybridBuildings->DoCalcRxPower(46,mmEnb,mmUe); //en dbm
	  	  powerShadowing=std::pow (10,(powerShadowing-30)/10); //en W
	  	  powerShadowing=powerShadowing/(100 * 180000); //en W/Hz para cada RB

	  	  Ptr<SpectrumValue> rxPsdShadowing = Create<SpectrumValue> (sm);
	  	  for (uint32_t k=0; k<100; k++)
	  	  {
	  		(*rxPsdShadowing)[k] = powerShadowing;
	  	  }

	  	  Ptr<SpectrumValue> rxPsd = Fading->DoCalcRxPowerSpectralDensity(rxPsdShadowing, mmEnb, mmUe);

	  	  std::cout<<std::endl<<std::endl<<"USUARIO "<<i<<", ENB "<<j<<std::endl;
	  	  for (uint32_t k=0; k<100; k++)
	  	  {
	  		  SNR[i][j][k]=10*std::log10((*rxPsd)[k]*180000)-(10*std::log10((1.38064852e-23)*330*10*180000));
	  		  std::cout<<"RB "<<k<<"----> "<<SNR[i][j][k]<<std::endl;
	  	  }
	  }
  }
  }

void evento()
{
	std::cout << "hola evento" << std::endl;
}


static ns3::GlobalValue g_distance ("distance",
                                   "Distance between two enbs",
                                   ns3::DoubleValue (500),
                                   ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_nMacroEnbSites ("nMacroEnbSites",
                                           "27 para este escenario. NO CAMBIAR!",
                                           ns3::DoubleValue (27),
                                           ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_simTime ("simTime",
                                   "Total duration of the simulation (in seconds)",
                                   ns3::DoubleValue (5.000),
                                   ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_numberOfRandomUes ("numberOfRandomUes",
                                             "Number of UEs in each section",
                                              ns3::UintegerValue (0),
                                              ns3::MakeUintegerChecker<uint16_t> ());
static ns3::GlobalValue g_remRbId ("remRbId",
                                   "Resource Block Id of Data Channel, for which REM will be generated;"
                                   "default value is -1, what means REM will be averaged from all RBs of "
                                   "Control Channel",
                                   ns3::IntegerValue (-1),
                                   MakeIntegerChecker<int32_t> ());
static ns3::GlobalValue g_NsubbandsUser ("NsubbandsUser",
                                   	     "Número subbandas por usuario",
									     ns3::IntegerValue (25),
									     ns3::MakeIntegerChecker<int16_t> ());
static ns3::GlobalValue g_ULDL ("ULDL",
                                "UL/DL seleccion para ejecucion de Scheduler",
								ns3::IntegerValue (0),
								ns3::MakeIntegerChecker<int16_t> ());
static ns3::GlobalValue g_generateRem ("generateRem",
                                       "if true, will generate a REM and then abort the simulation;"
                                       "if false, will run the simulation normally (without generating any REM)",
                                       ns3::BooleanValue (true),
                                       ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_generateTraces ("generateTraces",
                                       "if true, will generate a .txt file with the traces"
                                       "if false, will run the simulation normally (without generating any .txt)",
                                       ns3::BooleanValue (false),
                                       ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_generateSchedFiles ("generateSchedFiles",
                                          "if true, will generate several .txt files with the scheduler results"
                                          "if false, will run the simulation normally (without generating any .txt)",
                                          ns3::BooleanValue (false),
                                          ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_anillos ("anillos",
                                   "Número de anillos de la distribucion",
									ns3::IntegerValue (2),
									ns3::MakeIntegerChecker<int16_t> ());

/*static ns3::GlobalValue g_macroEnbTxPowerDbm ("macroEnbTxPowerDbm",
                                              "TX power [dBm] used by macro eNBs",
                                              ns3::DoubleValue (46.0),
                                              ns3::MakeDoubleChecker<double> ());*/

int main (int argc, char *argv[])
{
// Command line arguments
  CommandLine cmd;
  cmd.Parse (argc, argv);

//Parámetros de entrada que queremos cargar a la simulación
  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("input-lte-escenario-canonico.txt"));
  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Load"));
  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

//Declaración de las variables globales introducidas por input-lte.txt
  UintegerValue uintegerValue;
  IntegerValue integerValue;
  DoubleValue doubleValue;
  BooleanValue booleanValue;
  StringValue stringValue;

  GlobalValue::GetValueByName ("numberOfRandomUes", uintegerValue);
  uint16_t numberOfRandomUes = uintegerValue.Get ();
  GlobalValue::GetValueByName ("nMacroEnbSites", doubleValue);
//  double nMacroEnbSites = doubleValue.Get ();
//  GlobalValue::GetValueByName ("simTime", doubleValue);
  double simTime = doubleValue.Get ();
  GlobalValue::GetValueByName ("distance", doubleValue);
  double distance = doubleValue.Get ();
  GlobalValue::GetValueByName ("remRbId", integerValue);
  int32_t remRbId = integerValue.Get ();
  GlobalValue::GetValueByName ("NsubbandsUser", integerValue);
  int16_t NsubbandsUser = integerValue.Get ();
  GlobalValue::GetValueByName ("ULDL", integerValue);
  int16_t ULDL = integerValue.Get ();
  GlobalValue::GetValueByName ("generateRem", booleanValue);
  bool generateRem = booleanValue.Get ();
  GlobalValue::GetValueByName ("generateTraces", booleanValue);
  bool generateTraces = booleanValue.Get ();
  GlobalValue::GetValueByName ("generateSchedFiles", booleanValue);
  bool generateSchedFiles = booleanValue.Get ();
  GlobalValue::GetValueByName ("anillos", integerValue);
  int16_t anillos = integerValue.Get ();
//  GlobalValue::GetValueByName ("macroEnbTxPowerDbm", doubleValue);
//  double macroEnbTxPowerDbm = doubleValue.Get ();


  // Configuramos Scheduler y pasamos el numero de subbandasUser al scheduler (para el UL)
  Config::SetDefault ("ns3::schedulerDRANMac::FilesEnabled", BooleanValue (generateSchedFiles));
  Config::SetDefault ("ns3::RrFfMacSchedulerComparative::FilesEnabled", BooleanValue (generateSchedFiles));
  Config::SetDefault ("ns3::PfFfMacSchedulerComparative::FilesEnabled", BooleanValue (generateSchedFiles));
//  schedulerDRANMac::Nsubbands_user=NsubbandsUser;
  Config::SetDefault ("ns3::schedulerDRANMac::MaxSubbandsUE", UintegerValue (NsubbandsUser));
  LteEnbMac::ULDLselection=ULDL;


//Crea un objeto lteHelper
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  //  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::HybridBuildingsPropagationLossModel"));

  //  lteHelper->SetPathlossModelAttribute ("ShadowSigmaExtWalls", DoubleValue (0));
  //  lteHelper->SetPathlossModelAttribute ("ShadowSigmaOutdoor", DoubleValue (1));
  //  lteHelper->SetPathlossModelAttribute ("ShadowSigmaIndoor", DoubleValue (1.5));
  //  lteHelper->SetPathlossModelAttribute ("InternalWallLoss", DoubleValue (3));
    //use always LOS  model
  //  lteHelper->SetPathlossModelAttribute ("Los2NlosThr", DoubleValue (1e6));
  //  lteHelper->SetSpectrumChannelType ("ns3::MultiModelSpectrumChannel");

    // Configuracion del Fading
    lteHelper->SetFadingModel("ns3::TraceFadingLossModel");
    lteHelper->SetFadingModelAttribute ("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
    lteHelper->SetFadingModelAttribute ("TraceLength", TimeValue (Seconds (10.0)));
    lteHelper->SetFadingModelAttribute ("SamplesNum", UintegerValue (10000));
    lteHelper->SetFadingModelAttribute ("WindowSize", TimeValue (Seconds (0.5)));
    lteHelper->SetFadingModelAttribute ("RbNum", UintegerValue (100));

    //Creamos el contenedor de los usuarios
      Box macroUeBox;
      double RadioMax;

      MobilityHelper mobility;
      mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

      //Crea los nodos para las eNBs
      NodeContainer enbNodes;
      NetDeviceContainer enbDevs;

      //Variables auxiliares para crear las enbs
      double dy = (sqrt(3)/6)*distance; //distancia entre el origen y la primera linea de eNBs
      double num_x = 1;
      uint32_t contador=0;
      double auxiliar = 0.5;
      double antennaOrientation = 0;
      double enbHeight = 30;
      double x = 0;
      double y = 0;
      Vector pos = {0,0,0};

switch (anillos)
{
  case 1:
    enbNodes.Create (12*3); // Consideramos un anillo

  //Configuramos la distribución de las eNBs
    mobility.Install (enbNodes);
    BuildingsHelper::Install (enbNodes);
    lteHelper->SetEnbAntennaModelType ("ns3::ParabolicAntennaModel");

  num_x=0.5;
  for (int fila=-1;fila<3;fila++)
  {
    if(fila<=0)
    {
      auxiliar = 0.5;
    }
    else
    {
      auxiliar = -0.5;
    }
    num_x=num_x+auxiliar;
    for (x=-num_x*distance;x<=num_x*distance;x=x+distance)
    {
      antennaOrientation = -90;
      for (uint32_t j=1;j<=3;j++)
      {
  	y=-dy+fila*(dy+((distance/sqrt(3))));
          pos =Vector(x, y, enbHeight);
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
  RadioMax=3*distance/2;
  macroUeBox = Box (-distance * 3, (distance * 3) , -(2*3*distance/sqrt(3)) , (2*3*distance/sqrt(3)), 1.5, 1.5);

  break;

  case 2:
    enbNodes.Create (27*3); // Sectores = 3*EnbSites = 3*27 = 81

  //Configuramos la distribución de las eNBs
    mobility.Install (enbNodes);
    BuildingsHelper::Install (enbNodes);
    lteHelper->SetEnbAntennaModelType ("ns3::ParabolicAntennaModel");

  for (int fila=-2;fila<4;fila++)
  {
    if(fila<=0)
    {
      auxiliar = 0.5;
    }
    else
    {
      auxiliar = -0.5;
    }
    num_x=num_x+auxiliar;
    for (x=-num_x*distance;x<=num_x*distance;x=x+distance)
    {
      antennaOrientation = -90;
      for (uint32_t j=1;j<=3;j++)
      {
  	      y=-dy+fila*(dy+((distance/sqrt(3))));
          pos =Vector(x, y, enbHeight);
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
  RadioMax=5*distance/2;
  macroUeBox = Box (-distance * 1.5, (distance * 1.5) , -(2*2*distance/sqrt(3)) , (2*2*distance/sqrt(3)), 1.5, 1.5);
  break;
}

  
//configuramos las UEs
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  NodeContainer ueNodes;
  ueNodes.Create (numberOfRandomUes);
  
  //Las colocamos para que caigan aleatoriamente dentro del circulo de radio la distancia maxima entre el centro y un site
  Ptr<UniformDiscPositionAllocator> randomUePositionAlloc = CreateObject<UniformDiscPositionAllocator> ();
  Ptr<UniformRandomVariable> theta = CreateObject<UniformRandomVariable> ();
  randomUePositionAlloc->SetAttribute ("rho", DoubleValue (RadioMax));
  randomUePositionAlloc->SetAttribute ("X", DoubleValue (0));
  randomUePositionAlloc->SetAttribute ("Y", DoubleValue (0));

  mobility.SetPositionAllocator (randomUePositionAlloc);
  mobility.Install (ueNodes);

  // Fijamos las posicion de los UEs a 1.5m
  for (uint16_t i=0; i<ueNodes.GetN(); i++)
  {
	  Ptr<Node> ue = ueNodes.Get(i);
	  Ptr<MobilityModel> mm = ue->GetObject<MobilityModel>();
	  Vector3D pos = mm->GetPosition ();
	  mm->SetPosition (Vector(pos.x,pos.y,1.5));
//	  pos.z=1.5;
  }

  BuildingsHelper::Install (ueNodes);

  // Antena UEs
  lteHelper->SetUeAntennaModelType ("ns3::IsotropicAntennaModel");

//Instala el protocolo LTE en las UEs
  NetDeviceContainer ueDevs;
  ueDevs = lteHelper->InstallUeDevice (ueNodes);

////Vincula las UEs con las eNBs más favorables calculando la distancia a la estacion base más cercana y su sector.
////El sector al que pertenece el usuario se elige sabiendo que en cada site se han generado 3 enb con la orientacion de las antenas siempre en el mismo orden
//  Vector positionEnb = Vector(0,0,0);
//  Vector positionUe = Vector(0,0,0);
//  double distanciaUeEnb = 0;
//  double aux = distance;
//  double angle = 0;
//  uint32_t posAttach = 0;
//
//  for (uint32_t ue=0;ue<ueNodes.GetN();ue++)
//  {
//    for (uint32_t enb=0;enb<enbNodes.GetN();enb++)
//    {
//      Ptr<Node> nodePosEnb = enbNodes.Get(enb);
//      Ptr<MobilityModel> mmEnb = nodePosEnb->GetObject<MobilityModel> ();
//      positionEnb = mmEnb->GetPosition ();
//      Ptr<Node> nodePosUe = ueNodes.Get(ue);
//      Ptr<MobilityModel> mmUe = nodePosUe->GetObject<MobilityModel> ();
//      positionUe = mmUe->GetPosition ();
//
//      distanciaUeEnb = sqrt(pow((positionEnb.x-positionUe.x),2)+pow((positionEnb.y-positionUe.y),2));
//      if (distanciaUeEnb<aux)
//      {
//        aux=distanciaUeEnb;
//        posAttach=enb;
//      }
//    }
//    Ptr<Node> nodeSel = enbNodes.Get(posAttach);
//    Ptr<MobilityModel> mmEnbSel = nodeSel->GetObject<MobilityModel> ();
//    positionEnb = mmEnbSel->GetPosition ();
//    angle=std::atan2((positionUe.y-positionEnb.y),(positionUe.x-positionEnb.x))/(3.1415/180);
//    if((angle>-150)&&(angle<=-30))
//    {
//      lteHelper->Attach(ueDevs.Get(ue), enbDevs.Get(posAttach));
//    }
//    else if (((angle>90)&&(angle<=180))||((angle>=-180)&&(angle<=-150)))
//    {
//      lteHelper->Attach(ueDevs.Get(ue), enbDevs.Get(posAttach+2));
//    }
//    else
//    {
//      lteHelper->Attach(ueDevs.Get(ue), enbDevs.Get(posAttach+1));
//    }
//    aux=distance;
//  }

  //Calculo las perdidas entre base y usuario y attach.
  	double loss;
  	double BestLoss=1000000;
  	uint32_t best;

  	//Bucle para recorrer cada uno de los usuarios y obtener su potencia
  	for(uint32_t jj=0;jj<ueNodes.GetN();jj++)
  	{
    	  Ptr<Node> nodeUe = ueNodes.Get(jj);
    	  Ptr<MobilityModel> mmUe = nodeUe->GetObject<MobilityModel> ();
  	  //vamos variando la celda para un usuario fijo para ver cual es la mejor servidora y la sinr que tendria conectado con cada una de ellas
  	  for(uint32_t ii=0;ii<enbNodes.GetN();ii++)
  	  {
    	    Ptr<Node> nodeEnb = enbNodes.Get(ii);
    	    Ptr<MobilityModel> mmEnb = nodeEnb->GetObject<MobilityModel> ();
    	    loss = lteHelper->GetLossDl(mmEnb,mmUe);
  	    //Comprobamos si las perdidas de la nuevas celda son menores que las encontradas hasta el momento
//    	    std::cout<<"usuario "<<jj<<"  celda "<<ii<<"  loss--->"<<loss<<"  BestLoss-->"<<BestLoss<<std::endl;
  	    if (loss<BestLoss)
  	    {
  	      BestLoss=loss;
  	      best = ii;
          }
  	  }
  	  BestLoss=1000000;

//  	  std::cout<<std::endl<<std::endl<<std::endl<<BestLoss<<"     "<<"mejor--->"<<best<<std::endl<<std::endl;
  	  //Vincula las UEs con la eNBs de la que tiene menos perdidas. Esto configurará cada UE de acuerdo a la configuración de las eNBs, y crea una conexión RRC entre ellos
            lteHelper->Attach(ueDevs.Get(jj), enbDevs.Get(best));

    	  //Activa la portadora de radio entra la UE y la eNB vinculada
    	  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VIDEO;
    	  EpsBearer bearer (q);
    	  lteHelper->ActivateDataRadioBearer (ueDevs.Get(jj), bearer);
  	}

    BuildingsHelper::MakeMobilityModelConsistent ();


////Activa la portadora de radio entra la UE y la eNB vinculada
//  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VIDEO;
//  EpsBearer bearer (q);
//  lteHelper->ActivateDataRadioBearer (ueDevs, bearer);


//    // Configuramos el evento que calcula la SNR de todos los enlaces
//    double time = 0.001;
//    while (time<simTime)
//    {
//      Time t = Seconds (time);
//      Simulator::Schedule(t, &DlSNRmatrix, lteHelper, ueNodes, enbNodes);
//      time += 0.001;
//    }


    double time = 0.87;
    while (time<simTime)
    {
      Time t = Seconds (time);
      Simulator::Schedule(t, &evento);
      time += 0.001;
    }



//Habilitamos la salida de datos
  if(generateTraces)
  {
    lteHelper->EnablePhyTraces ();
    lteHelper->EnableMacTraces ();
    lteHelper->EnableRlcTraces ();
    lteHelper->EnablePdcpTraces ();
  }


  // Obtenemos estadisticas de Tp a nivel RLC
//  double statsStartTime = 0; // need to allow for RRC connection establishment + SRS
  double statsDuration = 0.001;
//
  Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();
//  rlcStats->SetAttribute ("StartTime", TimeValue (Seconds (statsStartTime)));
  rlcStats->SetAttribute ("EpochDuration", TimeValue (Seconds (statsDuration)));



//Guardamos la configuracion de la simulación en un fichero de texto
  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("output-lte-escenario-canonico.txt"));
  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Save"));
  ConfigStore outputConfig2;
  outputConfig2.ConfigureDefaults ();
  outputConfig2.ConfigureAttributes ();

//Generamos el archivo que usara el netAnim
  AnimationInterface anim ("anim-lte.xml");

//Generamos la salida para el gnuPlot
  Ptr<RadioEnvironmentMapHelper> remHelper;
  if (generateRem)
    {
      PrintGnuplottableUeListToFile ("ues.txt");

      remHelper = CreateObject<RadioEnvironmentMapHelper> ();
      remHelper->SetAttribute ("ChannelPath", StringValue ("/ChannelList/0"));
      remHelper->SetAttribute ("OutputFile", StringValue ("lte.rem"));
      remHelper->SetAttribute ("XMin", DoubleValue (macroUeBox.xMin));
      remHelper->SetAttribute ("XMax", DoubleValue (macroUeBox.xMax));
      remHelper->SetAttribute ("YMin", DoubleValue (macroUeBox.yMin));
      remHelper->SetAttribute ("YMax", DoubleValue (macroUeBox.yMax));
      remHelper->SetAttribute ("Z", DoubleValue (1.5));

      if (remRbId >= 0)
        {
          remHelper->SetAttribute ("UseDataChannel", BooleanValue (true));
          remHelper->SetAttribute ("RbId", IntegerValue (remRbId));
        }

      remHelper->Install ();
      // simulation will stop right after the REM has been generated
    }
  else
    {
      //Paramos la simulación
      Simulator::Stop (Seconds (simTime));
    }

//Lanzamos la simulación
  Simulator::Run ();

//  // Fichero propio estadisticas - DL ---> SOLO GENERA LAS ESTADISTICAS DE LA ULTIMA TRAMA-SUBTRAMA
//  FILE * pTpFile;
//  pTpFile = fopen ("Canonico-Stats-Tp.txt","a");
//
//  int m_users=ueNodes.GetN();
//  for (int i = 0; i < m_users; i++)
//    {
//      // get the imsi
//      int imsi = ueDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetImsi ();
//      uint8_t lcId = 3; // Canal logico asociado a la dedicated bearer que hemos definido de video.
//
//      double dlRxPackets = rlcStats->GetDlRxPackets (imsi, lcId);
//      double dlTxPackets = rlcStats->GetDlTxPackets (imsi, lcId);
//
//      double dlRxData = rlcStats->GetDlRxData (imsi, lcId);
//      double dlTxData = rlcStats->GetDlTxData (imsi, lcId);
//
//      // bler
//      double dlBler =  1.0 - (dlRxPackets/dlTxPackets);
//      std::cout << "\tUser " << i << " imsi " << imsi << " DOWNLINK"
//                   << " Paquetes tx " << dlTxPackets << " Paquetes rx " << dlRxPackets
//                   << " BLER " << dlBler << " Datos tx" << dlTxData << " Datos rx " << dlRxData
//					<< std::endl;
//
//	  fprintf(pTpFile,"User %d imsi %d DOWNLINK Paquetes tx %f Paquetes rx %f BLER %f Datos tx %f Datos rx %f\n" , i ,imsi , dlTxPackets , dlRxPackets, dlBler ,dlTxData , dlRxData);
//
//    }
//
//  fclose(pTpFile);


  std::cout << " ********** FIN *********" << std::endl;
//Terminamos la simulación
  Simulator::Destroy ();

  return 0;
}
