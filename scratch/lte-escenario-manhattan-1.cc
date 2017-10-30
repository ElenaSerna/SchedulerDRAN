#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/lte-module.h>
#include "ns3/config-store.h"
#include "ns3/netanim-module.h"
#include <ns3/building-allocator.h>
#include <ns3/mobility-building-info.h>
#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/buildings-module.h>
#include <ns3/buildings-helper.h>
#include <ns3/math.h>

/* Distribucion del ecenario manhattan (cuadricula de edificios) con nGrid² edificios y (nGrid+1)² sites.
Hay un site colocado en cada una de las intersecciones de las calles
Los usuarios se distribullen de forma aleatoria en cada calle tanto las horizontales como las verticales
Para enlazar cada usuario con su enb se hace con la función de enlazar a la más cercana.
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

NS_LOG_COMPONENT_DEFINE ("LteEscenarioManhattan");

//Funcion para generar un archivo de texto para graficar la posicion de los edificios en el gnuplot
void
PrintGnuplottableBuildingListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
  {
    NS_LOG_ERROR ("Can't open file " << filename);
    return;
  }
  uint32_t index = 0;
  for (BuildingList::Iterator it = BuildingList::Begin (); it != BuildingList::End (); ++it)
  {
    ++index;
    Box box = (*it)->GetBoundaries ();
    outFile << "set object " << index
            << " rect from " << box.xMin << "," << box.yMin
            << " to "  << box.xMax << "," << box.yMax
            << " front fs empty "
            << std::endl;
  }
}

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
//	  		  std::cout<<"RB "<<k<<"----> "<<SNR[i][j][k]<<std::endl;
	  	  }
	  }
  }

 }

void evento()
{
	std::cout << "hola evento" << std::endl;
}


static ns3::GlobalValue g_distanceBuilding ("distanceBuilding",
                                            "Distance of 1 building",
                                            ns3::DoubleValue (60),
                                            ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_distanceStreet ("distanceStreet",
                                          "Distance of 1 street",
                                          ns3::DoubleValue (30),
                                          ns3::MakeDoubleChecker<double> ());

static ns3::GlobalValue g_henb ("henb",
                                "enb heigh",
                                ns3::DoubleValue (30),
                                ns3::MakeDoubleChecker<double> ());

static ns3::GlobalValue g_nGrid ("nGrid",
                                 "number of buildings in 1 axis of the grid (debe de ser un numero par)",
                                 ns3::DoubleValue (8),
                                 ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_simTime ("simTime",
                                   "Total duration of the simulation (in seconds)",
                                   ns3::DoubleValue (5.000),
                                   ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_numberUesStreet ("numberUesStreet",
                                           "Number of UEs in each Street",
                                           ns3::UintegerValue (5),
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


int main (int argc, char *argv[])
{
// Command line arguments
  CommandLine cmd;
  cmd.Parse (argc, argv);

//Parámetros de entrada que queremos cargar a la simulación
  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("input-lte-escenario-manhattan.txt"));
  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Load"));
  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

//Declaración de las variables globales introducidas por input-lte.txt
  UintegerValue uintegerValue;
  IntegerValue integerValue;
  DoubleValue doubleValue;
  BooleanValue booleanValue;
  StringValue stringValue;

  GlobalValue::GetValueByName ("numberUesStreet", uintegerValue);
  uint16_t numberUesStreet = uintegerValue.Get ();
  GlobalValue::GetValueByName ("nGrid", doubleValue);
  double nGrid = doubleValue.Get ();
  GlobalValue::GetValueByName ("simTime", doubleValue);
  double simTime = doubleValue.Get ();
  GlobalValue::GetValueByName ("distanceBuilding", doubleValue);
  double distanceBuilding = doubleValue.Get ();
  GlobalValue::GetValueByName ("distanceStreet", doubleValue);
  double distanceStreet = doubleValue.Get ();
  GlobalValue::GetValueByName ("henb", doubleValue);
  double henb = doubleValue.Get ();
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

//  double c=1.5;
//  for (int i=0; i<2; i++)
//  {
//
//	  std::vector <int> eNBs;
//
//	  for (int j=0; j<3; j++)
//	  {
//		  std::vector <int> RBs;
//
//		  for (int k=0; k<4; k++)
//		  {
//			  RBs.push_back(c);
//			  c++;
//		  }
//		  eNBs.push_back(j);
//	  }
//	  var.push_back(eNBs,RBs);
//  }
//  std::cout << var[1][2] << std::endl;
//  std::cout << var[0].size() << std::endl;
//  std::cout << var.size() << std::endl;
//  std::cout << var[0][0].size() << std::endl;
//  std::cout << "Tamaño " << var.size() << std::endl;
  // Pasamos el numero de subbandasUser al scheduler (para el UL)
//  schedulerDRANMac::Nsubbands_user=NsubbandsUser;
  Config::SetDefault ("ns3::schedulerDRANMac::MaxSubbandsUE", UintegerValue (NsubbandsUser));
  LteEnbMac::ULDLselection=ULDL;


  // Configuracion parametros por defecto
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbRrc::EpsBearerToRlcMapping", EnumValue(LteEnbRrc::RLC_SM_ALWAYS)); // Configuracion full-buffer


//Creamos el contenedor de los usuarios
  Box macroUeBox = Box (-(nGrid/2)*(distanceBuilding+distanceStreet) - (distanceStreet/2), (nGrid/2)*(distanceBuilding+distanceStreet) + (distanceStreet/2), -(nGrid/2)*(distanceBuilding+distanceStreet) - (distanceStreet/2), (nGrid/2)*(distanceBuilding+distanceStreet) + (distanceStreet/2), 1.5, 1.5);

//Crea un objeto lteHelper
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
//  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::HybridBuildingsPropagationLossModel"));

//  lteHelper->SetPathlossModelAttribute ("ShadowSigmaExtWalls", DoubleValue (0));
//  lteHelper->SetPathlossModelAttribute ("ShadowSigmaOutdoor", DoubleValue (1));
//  lteHelper->SetPathlossModelAttribute ("ShadowSigmaIndoor", DoubleValue (1.5));
  //use always LOS  model
//  lteHelper->SetPathlossModelAttribute ("Los2NlosThr", DoubleValue (1e6));
//  lteHelper->SetSpectrumChannelType ("ns3::MultiModelSpectrumChannel");

  // Configuracion del Fading
  lteHelper->SetFadingModel("ns3::TraceFadingLossModel");
  lteHelper->SetFadingModelAttribute ("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_ampliado13''EPA_3kmph.fad"));
  lteHelper->SetFadingModelAttribute ("TraceLength", TimeValue (Seconds (13.0)));
  lteHelper->SetFadingModelAttribute ("SamplesNum", UintegerValue (10000));
  lteHelper->SetFadingModelAttribute ("WindowSize", TimeValue (Seconds (0.5)));
  lteHelper->SetFadingModelAttribute ("RbNum", UintegerValue (100));

  // Seleccion del tipo de scheduler
//  lteHelper->SetSchedulerType("ns3::schedulerDRANMac"); // Distribuido
//  lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");
//  lteHelper->SetSchedulerType("ns3::RrFfMacSchedulerComparative"); // Round robin


//Crea los nodos para las eNBs
  NodeContainer enbNodes;
  enbNodes.Create (pow(nGrid+1,2)); // 2^buildings_per_street sites

//Configuramos la distribucion de los edificios
  Ptr<GridBuildingAllocator> gridBuildingAllocator = CreateObject<GridBuildingAllocator> ();
  gridBuildingAllocator->SetAttribute ("GridWidth", UintegerValue (nGrid)); // numero de edificios por linea
  gridBuildingAllocator->SetAttribute ("LengthX", DoubleValue (distanceBuilding)); // longitud del edificio en x
  gridBuildingAllocator->SetAttribute ("LengthY", DoubleValue (distanceBuilding)); // longitud del edificio en y
  gridBuildingAllocator->SetAttribute ("DeltaX", DoubleValue (distanceStreet)); // longitud de la calle en x
  gridBuildingAllocator->SetAttribute ("DeltaY", DoubleValue (distanceStreet)); // longitud de la calle en y
  gridBuildingAllocator->SetAttribute ("Height", DoubleValue (30)); // altura del edificio
  gridBuildingAllocator->SetAttribute ("MinX", DoubleValue (macroUeBox.xMin + distanceStreet)); // coordenada x del primer edificio (abajo izquierda)
  gridBuildingAllocator->SetAttribute ("MinY", DoubleValue (macroUeBox.yMin + distanceStreet)); // coordenada y del primer edificio (abajo izquierda)
  gridBuildingAllocator->Create (pow(nGrid,2));

//Configuramos la distribución de las eNBs
  Vector pos = {0,0,0};
  uint32_t contador = 0;
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (enbNodes);
  BuildingsHelper::Install (enbNodes);
  lteHelper->SetEnbAntennaModelType ("ns3::IsotropicAntennaModel"); // Antena omnidireccional
  NetDeviceContainer enbDevs;

  for (int ii=-nGrid/2;ii<=nGrid/2;ii++)
  {
    for (int jj=-nGrid/2;jj<=nGrid/2;jj++)
    {
      pos = Vector(ii*(distanceBuilding+distanceStreet),jj*(distanceBuilding+distanceStreet),henb); // Altura de las eNBs de 3 m acorde con el escenario micro que definimos
      Ptr<Node> node = enbNodes.Get(contador);
      Ptr<MobilityModel> mm = node->GetObject<MobilityModel> ();
      mm->SetPosition (Vector(pos));
      enbDevs.Add (lteHelper->InstallEnbDevice (node));
      contador++;
    }
  }

//Configuramos las UEs.
  NodeContainer ues;

for(int ii=-nGrid/2;ii<=nGrid/2;ii++)
{
  NodeContainer ueNodes;
  ueNodes.Create (numberUesStreet);
    
  MobilityHelper mobilityUes;

  Ptr<RandomBoxPositionAllocator> randomUePositionAlloc = CreateObject<RandomBoxPositionAllocator> ();
  Ptr<UniformRandomVariable> xVal = CreateObject<UniformRandomVariable> ();
  xVal->SetAttribute ("Min", DoubleValue ((-distanceStreet/2)+(ii*(distanceBuilding + distanceStreet))));
  xVal->SetAttribute ("Max", DoubleValue ((distanceStreet/2)+(ii*(distanceBuilding + distanceStreet))));
  randomUePositionAlloc->SetAttribute ("X", PointerValue (xVal));
  Ptr<UniformRandomVariable> yVal = CreateObject<UniformRandomVariable> ();
  yVal->SetAttribute ("Min", DoubleValue ((-distanceStreet/2)-((nGrid/2)*(distanceBuilding+distanceStreet))));
  yVal->SetAttribute ("Max", DoubleValue ((distanceStreet/2)+((nGrid/2)*(distanceBuilding+distanceStreet))));
  randomUePositionAlloc->SetAttribute ("Y", PointerValue (yVal));
  Ptr<UniformRandomVariable> zVal = CreateObject<UniformRandomVariable> ();
  zVal->SetAttribute ("Min", DoubleValue (1.5));
  zVal->SetAttribute ("Max", DoubleValue (1.5));
  randomUePositionAlloc->SetAttribute ("Z", PointerValue (zVal));
  mobilityUes.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
/*  mobilityUes.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
			     "Speed", StringValue ("ns3::UniformRandomVariable[Min=99.0|Max=100]"),
			     "Bounds", RectangleValue (Rectangle((-distanceStreet/2)+(ii*(distanceBuilding + distanceStreet)), (distanceStreet/2)+(ii*(distanceBuilding + distanceStreet)), (-distanceStreet/2)-((nGrid/2)*(distanceBuilding+distanceStreet)), (distanceStreet/2)+((nGrid/2)*(distanceBuilding+distanceStreet))))); */

  mobilityUes.SetPositionAllocator (randomUePositionAlloc);
  mobilityUes.Install (ueNodes);
  BuildingsHelper::Install (ueNodes);
  ues.Add(ueNodes);
//
//  //Instala el protocolo LTE en las UEs
//  NetDeviceContainer ueDevs;
//  ueDevs = lteHelper->InstallUeDevice (ueNodes);
//
//  //Vincula las UEs con las eNBs. Esto configurará cada UE de acuerdo a la configuración de las eNBs, y crea una conexión RRC entre ellos
//  lteHelper->AttachToClosestEnb(ueDevs, enbDevs);
//
//  //Activa la portadora de radio entra la UE y la eNB vinculada
//  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
//  EpsBearer bearer (q);
//  lteHelper->ActivateDataRadioBearer (ueDevs, bearer);
}
for(int jj=-nGrid/2;jj<=nGrid/2;jj++)
{
  NodeContainer ueNodes;
  ueNodes.Create (numberUesStreet);
    
  MobilityHelper mobilityUes;
  Ptr<RandomBoxPositionAllocator> randomUePositionAlloc = CreateObject<RandomBoxPositionAllocator> ();
  Ptr<UniformRandomVariable> xVal = CreateObject<UniformRandomVariable> ();
  xVal->SetAttribute ("Min", DoubleValue ((-distanceStreet/2)-((nGrid/2)*(distanceBuilding+distanceStreet))));
  xVal->SetAttribute ("Max", DoubleValue ((distanceStreet/2)+((nGrid/2)*(distanceBuilding+distanceStreet))));
  randomUePositionAlloc->SetAttribute ("X", PointerValue (xVal));
  Ptr<UniformRandomVariable> yVal = CreateObject<UniformRandomVariable> ();
  yVal->SetAttribute ("Min", DoubleValue ((-distanceStreet/2)+(jj*(distanceBuilding + distanceStreet))));
  yVal->SetAttribute ("Max", DoubleValue ((distanceStreet/2)+(jj*(distanceBuilding + distanceStreet))));
  randomUePositionAlloc->SetAttribute ("Y", PointerValue (yVal));
  Ptr<UniformRandomVariable> zVal = CreateObject<UniformRandomVariable> ();
  zVal->SetAttribute ("Min", DoubleValue (1.5));
  zVal->SetAttribute ("Max", DoubleValue (1.5));
  randomUePositionAlloc->SetAttribute ("Z", PointerValue (zVal));
  mobilityUes.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
/*  mobilityUes.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
			     "Speed", StringValue ("ns3::UniformRandomVariable[Min=99.0|Max=100]"),
			     "Bounds", RectangleValue (Rectangle((-distanceStreet/2)-((nGrid/2)*(distanceBuilding+distanceStreet)), (distanceStreet/2)+((nGrid/2)*(distanceBuilding+distanceStreet)), (-distanceStreet/2)+(jj*(distanceBuilding + distanceStreet)), (distanceStreet/2)+(jj*(distanceBuilding + distanceStreet)))));*/

  mobilityUes.SetPositionAllocator (randomUePositionAlloc);
  mobilityUes.Install (ueNodes);
  BuildingsHelper::Install (ueNodes);
  ues.Add(ueNodes);
//
//  //Instala el protocolo LTE en las UEs
//  NetDeviceContainer ueDevs;
//  ueDevs = lteHelper->InstallUeDevice (ueNodes);
//
//  //Vincula las UEs con las eNBs. Esto configurará cada UE de acuerdo a la configuración de las eNBs, y crea una conexión RRC entre ellos
//  lteHelper->AttachToClosestEnb(ueDevs, enbDevs);
//
//  //Activa la portadora de radio entra la UE y la eNB vinculada
//  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
//  EpsBearer bearer (q);
//  lteHelper->ActivateDataRadioBearer (ueDevs, bearer);
}

// Antena UEs
lteHelper->SetUeAntennaModelType ("ns3::IsotropicAntennaModel");
//Instala el protocolo LTE en las UEs
NetDeviceContainer ueDevs;
ueDevs = lteHelper->InstallUeDevice (ues);

//Calculo las perdidas entre base y usuario y attach.
	double loss;
	double BestLoss=1000000;
	uint32_t best;

	//Bucle para recorrer cada uno de los usuarios y obtener su potencia
	for(uint32_t jj=0;jj<ues.GetN();jj++)
	{
  	  Ptr<Node> nodeUe = ues.Get(jj);
  	  Ptr<MobilityModel> mmUe = nodeUe->GetObject<MobilityModel> ();
	  //vamos variando la celda para un usuario fijo para ver cual es la mejor servidora y la sinr que tendria conectado con cada una de ellas
	  for(uint32_t ii=0;ii<enbNodes.GetN();ii++)
	  {
  	    Ptr<Node> nodeEnb = enbNodes.Get(ii);
  	    Ptr<MobilityModel> mmEnb = nodeEnb->GetObject<MobilityModel> ();
  	    loss = lteHelper->GetLossDl(mmEnb,mmUe);
	    //Comprobamos si las perdidas de la nuevas celda son menores que las encontradas hasta el momento
  	    std::cout<<"usuario "<<jj<<"  celda "<<ii<<"  loss--->"<<loss<<"  BestLoss-->"<<BestLoss<<std::endl;
	    if (loss<BestLoss)
	    {
	      BestLoss=loss;
	      best = ii;
        }
	  }
	  BestLoss=1000000;

	  std::cout<<std::endl<<std::endl<<std::endl<<BestLoss<<"     "<<"mejor--->"<<best<<std::endl<<std::endl;
	  //Vincula las UEs con la eNBs de la que tiene menos perdidas. Esto configurará cada UE de acuerdo a la configuración de las eNBs, y crea una conexión RRC entre ellos
          lteHelper->Attach(ueDevs.Get(jj), enbDevs.Get(best));

  	  //Activa la portadora de radio entra la UE y la eNB vinculada
  	  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VIDEO;
  	  EpsBearer bearer (q);
  	  lteHelper->ActivateDataRadioBearer (ueDevs.Get(jj), bearer);
	}

  BuildingsHelper::MakeMobilityModelConsistent ();


  // Escribimos el numero de UEs y eNBs que hay en el escenario
  numUEs = ues.GetN();
  numeNBs = enbNodes.GetN();

//  // Configuramos el evento que calcula la SNR de todos los enlaces
//  double time = 0.001;
//  while (time<simTime)
//  {
//    Time t = Seconds (time);
//    Simulator::Schedule(t, &DlSNRmatrix, lteHelper, ues, enbNodes);
//    time += 0.001;
//  }
//  double SNR2 [ues.GetN()][enbNodes.GetN()][100]; // Esta es la "copia" de la matriz SNR nuestra no visible
//
//std::cout << "Tamaño matriz " << sizeof(*SNR2)/sizeof(**SNR2) << std::endl;

    double time = 0.022;
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
  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("output-lte-escenario-manhattan.txt"));
  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Save"));
  ConfigStore outputConfig2;
  outputConfig2.ConfigureDefaults ();
  outputConfig2.ConfigureAttributes ();

//Generamos el archivo que usara el netAnim
  AnimationInterface anim ("anim-lte-manhattan.xml");

//Generamos la salida para el gnuPlot
  Ptr<RadioEnvironmentMapHelper> remHelper;
  if (generateRem)
    {
      PrintGnuplottableUeListToFile ("ues.txt");
      PrintGnuplottableBuildingListToFile ("buildings.txt");

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


//  // Fichero propio estadisticas - DL
//  FILE * pTpFile;
//  pTpFile = fopen ("Manhattan-Stats-Tp.txt","a");
//
//  int m_users=ues.GetN();
//  for (int i = 0; i < m_users; i++)
//    {
//      // get the imsi
//      int imsi = ueDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetImsi ();
//      uint8_t lcId = 3; // Canal logico asociado a la dedicated bearer que hemos definido de video.
//
//      // Obtenemos el Cell Id
//      int cellId=rlcStats->GetDlCellId(imsi,lcId);
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
//	  fprintf(pTpFile,"Imsi %d Cell Id %d DOWNLINK Paquetes tx %f Paquetes rx %f BLER %f Datos tx %f Datos rx %f\n" , imsi , cellId, dlTxPackets , dlRxPackets, dlBler ,dlTxData , dlRxData);
//
//
//    }
//
//  fclose(pTpFile);

  std::cout << " ********** FIN *********" << std::endl;

//Terminamos la simulación
  Simulator::Destroy ();

  return 0;
}
