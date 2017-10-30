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
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <ns3/lte-enb-rrc.h>
#include <ns3/packet.h>
//#include <packet-scheduler.h>

//#include <ns3/scheduler-DRAN-Mac.h>
//#include <ns3/CSchedException.h>



//#include <ns3/cclasePrueba.h>


#include <ns3/TiposScheduling.h>
#include <ns3/CCalculaScheduling.h>
#include <ns3/CSchedException.h>


//#include <ns3/phy-stats-calculator.h>

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

// Función para generar un archivo de texto para graficar la posición de los eNBs en el gnuplot 
void PrintGnuplottableEnbListToFile (std::string filename)
 {
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
  {
	NS_LOG_ERROR ("Can't open file " << filename);
	return;
  }
  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
  {
	Ptr<Node> node = *it;
	int nDevs = node->GetNDevices ();
	for (int j = 0; j < nDevs; j++)
	{
		Ptr<LteEnbNetDevice> enbdev = node->GetDevice (j)->GetObject <LteEnbNetDevice> ();
		if (enbdev)
		{
			Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
			outFile << "set label \"" << enbdev->GetCellId ()
			<< "\" at "<< pos.x << "," << pos.y
			<< " left font \"Helvetica,4\" textcolor rgb \"white\" front  point pt 2 ps 0.3 lc rgb \"white\" offset 0,0"
			<< std::endl;
		}
	}
  }
 }


 void 
AnimationInterface::WriteNodeColors ()
{
  for (NodeList::Iterator i = NodeList::Begin (); i != NodeList::End (); ++i)
 {
  Ptr<Node> n = *i;
 Rgb rgb = {255, 0, 0};
 if (m_nodeColors.find (n->GetId ()) == m_nodeColors.end ())
 {
  m_nodeColors[n->GetId ()] = rgb;
 }
  UpdateNodeColor (n, rgb.r, rgb.g, rgb.b);
 }
}

 void muestra_ganancias(double **matriz, int filas, int cols)
 {
 	std::cout << "Matriz ganancias: " << std::endl;
 	for (int i=0; i<filas ; i++){
 		std::cout << "[" ;
 		for (int j=0; j<cols; j++)
 			std::cout << matriz[i][j]<<' ';
 		std::cout << "]"<< std::endl;
 	}

 }


 void prueba(Ptr<LteHelper> lteHelper)
 {
	 std::cout<<lteHelper->GetSchedulerType();
 }


//void genRandom()
//{
	//double valor=rand() % 10;
	//std::cout << "valor random" << valor << std::endl;
//}

static ns3::GlobalValue g_distanceBuilding ("distanceBuilding",
                                            "Distance of 1 building",
                                            ns3::DoubleValue (60),
                                            ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_distanceStreet ("distanceStreet",
                                          "Distance of 1 street",
                                          ns3::DoubleValue (30),
                                          ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_nGrid ("nGrid",
                                 "number of buildings in 1 axis of the grid (debe de ser un numero par)",
                                 ns3::DoubleValue (8),
                                 ns3::MakeDoubleChecker<double> ());
//static ns3::GlobalValue g_simTime ("simTime",
                                   //"Total duration of the simulation (in seconds)",
                                   //ns3::DoubleValue (5.000),
                                   //ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_numberUesStreet ("numberUesStreet",
                                           "Number of UEs in each Street",
                                           ns3::UintegerValue (5),
                                           ns3::MakeUintegerChecker<uint16_t> ());
static ns3::GlobalValue g_remRbId ("remRbId",
                                   "Resource Block Id of Data Channel, for which REM will be generated;"
                                   "default value is -1, what means REM will be averaged from all RBs of "
                                   "Control Channel",
                                   ns3::IntegerValue (-1),
                                   MakeIntegerChecker<int32_t> ()); // para generar el Radio Environment Map
                                   
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

static ns3::GlobalValue g_schedulingPathResults ("schedulingPathResults",
                                				 "set the name of the file where the scheduling results will be stored",
												 ns3::StringValue (" "),
												 ns3::MakeStringChecker ());

static ns3::GlobalValue g_ULDL ("ULDL",
                                "especifica si el scheduling es UL o DL",
								ns3::IntegerValue (1),
								ns3::MakeIntegerChecker<int16_t> ()); // Será un integer para poder hacer un cast a T_UL_DL

static ns3::GlobalValue g_Nsubframes ("Nsubframes",
                                   	  "Número máximo de subframes a considerar",
									  ns3::IntegerValue (2000),
									  ns3::MakeIntegerChecker<int32_t> ());

static ns3::GlobalValue g_Nsubf_avg ("Nsubf_avg",
                                   	 "Número subframes a promediar",
									 ns3::IntegerValue (1000),
									 ns3::MakeIntegerChecker<int32_t> ());

static ns3::GlobalValue g_Nsubbands ("Nsubbands",
                                   	 "Número subbandas",
									 ns3::IntegerValue (12),
									 ns3::MakeIntegerChecker<int16_t> ());

static ns3::GlobalValue g_Nsubb_user ("Nsubb_user",
                                   	 "Número subbandas por usuario",
									 ns3::IntegerValue (1),
									 ns3::MakeIntegerChecker<int16_t> ());

static ns3::GlobalValue g_Nsubb_CQI ("Nsubb_CQI",
                                   	 "Número subbandas con información de CQI",
									 ns3::IntegerValue (12),
									 ns3::MakeIntegerChecker<int16_t> ());


static ns3::GlobalValue g_NRB ("NRB",
                               "Número de RBs",
							   ns3::IntegerValue (12),
							   ns3::MakeIntegerChecker<int16_t> ());

static ns3::GlobalValue g_throughput_curve ("throughput_curve",
											"curva tp vs sinr",
											ns3::StringValue (""),
											ns3::MakeStringChecker ());

static ns3::GlobalValue g_units_curve ("units_curve",
										"unidades de la curva",
										ns3::IntegerValue (1),
										ns3::MakeIntegerChecker<int16_t> ()); // Será un integer para poder hacer un cast a TRATE_UNITS

static ns3::GlobalValue g_tuncCQI ("truncCQI",
                                   "Truncar el CQI",
                                   ns3::BooleanValue (false),
                                   ns3::MakeBooleanChecker ());

static ns3::GlobalValue g_debug ("debug",
                                 "Mostar información",
                                 ns3::BooleanValue (false),
                                 ns3::MakeBooleanChecker ());

static ns3::GlobalValue g_seed_scheduling ("seed_scheduling",
										   "Semilla del generador",
										   ns3::UintegerValue (60),
										   ns3::MakeUintegerChecker<uint16_t> ());

static ns3::GlobalValue g_Scheduling ("Scheduling",
									  "Tipo de Scheduling",
									  ns3::IntegerValue (4),
									  ns3::MakeIntegerChecker<int16_t> ());


static ns3::GlobalValue g_Schedgain_sinr_min ("Schedgain_sinr_min",
											  "Mínima ganancia para compara con RR",
											  ns3::DoubleValue (-40),
											  ns3::MakeDoubleChecker<double> ());

static ns3::GlobalValue g_Schedgain_sinr_max ("Schedgain_sinr_max",
											  "Máxima ganancia para compara con RR",
											  ns3::DoubleValue (40),
											  ns3::MakeDoubleChecker<double> ());

static ns3::GlobalValue g_Schedgain_npoints ("Schedgain_npoints",
												  "Número de puntos a considerar",
												  ns3::IntegerValue (80),
												  ns3::MakeIntegerChecker<int16_t> ());
												  
static ns3::GlobalValue g_Schedgain_sector_file ("Schedgain_sector_file",
												 " ",
												 ns3::DoubleValue (0),
												 ns3::MakeDoubleChecker<double> ());



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
  //GlobalValue::GetValueByName ("simTime", doubleValue);
  //double simTime = doubleValue.Get ();
  GlobalValue::GetValueByName ("distanceBuilding", doubleValue);
  double distanceBuilding = doubleValue.Get ();
  GlobalValue::GetValueByName ("distanceStreet", doubleValue);
  double distanceStreet = doubleValue.Get ();
  GlobalValue::GetValueByName ("remRbId", integerValue);
  int32_t remRbId = integerValue.Get ();
  GlobalValue::GetValueByName ("generateRem", booleanValue);
  bool generateRem = booleanValue.Get ();
  GlobalValue::GetValueByName ("generateTraces", booleanValue);
  bool generateTraces = booleanValue.Get ();
  GlobalValue::GetValueByName ("schedulingPathResults", stringValue);
  std::string schedulingPathResults = stringValue.Get ();
  GlobalValue::GetValueByName ("ULDL", integerValue);
  int16_t ULDL = integerValue.Get ();
  GlobalValue::GetValueByName ("Nsubframes", integerValue);
  int32_t Nsubframes = integerValue.Get ();
  GlobalValue::GetValueByName ("Nsubf_avg", integerValue);
  int32_t Nsubf_avg = integerValue.Get ();
  GlobalValue::GetValueByName ("Nsubbands", integerValue);
  int16_t Nsubbands = integerValue.Get ();
  GlobalValue::GetValueByName ("Nsubb_user", integerValue);
  int16_t Nsubb_user = integerValue.Get ();
  GlobalValue::GetValueByName ("Nsubb_CQI", integerValue);
  int16_t Nsubb_CQI = integerValue.Get ();
  GlobalValue::GetValueByName ("NRB", integerValue);
  int16_t NRB = integerValue.Get ();
  //GlobalValue::GetValueByName ("throughput_curve", stringValue);
  //std::string throughput_curve = stringValue.Get ();
  //GlobalValue::GetValueByName ("units_curve", integerValue);
  //int16_t units_curve = integerValue.Get ();
  GlobalValue::GetValueByName ("truncCQI", booleanValue);
  bool truncCQI = booleanValue.Get ();
  GlobalValue::GetValueByName ("debug", booleanValue);
  bool debug = booleanValue.Get ();
  GlobalValue::GetValueByName ("seed_scheduling", uintegerValue);
  uint16_t seed_scheduling = uintegerValue.Get ();
  GlobalValue::GetValueByName ("Scheduling", integerValue);
  int16_t Scheduling = integerValue.Get ();
  GlobalValue::GetValueByName ("Schedgain_sinr_min", doubleValue);
  double Schedgain_sinr_min = doubleValue.Get ();
  GlobalValue::GetValueByName ("Schedgain_sinr_max", doubleValue);
  double Schedgain_sinr_max = doubleValue.Get ();
  GlobalValue::GetValueByName ("Schedgain_npoints", integerValue);
  int16_t Schedgain_npoints = integerValue.Get ();
  GlobalValue::GetValueByName ("Schedgain_sector_file", doubleValue);
  double Schedgain_sector_file = doubleValue.Get ();

  //const char *ULDL_type=ULDL.c_str();
  const char *schedPathResults=schedulingPathResults.c_str(); // Obtenemos una cadena de caracteres del string definido en SchedulingPathResults

  //std::cout << "variable " << ULDL_type << std::endl;
  std::cout << "variable2 " << schedPathResults << std::endl;

//Creamos el contenedor de los usuarios // altura sólo 1.5m  porque son móviles
  Box macroUeBox = Box (-(nGrid/2)*(distanceBuilding+distanceStreet) - (distanceStreet/2), (nGrid/2)*(distanceBuilding+distanceStreet) + (distanceStreet/2), -(nGrid/2)*(distanceBuilding+distanceStreet) - (distanceStreet/2), (nGrid/2)*(distanceBuilding+distanceStreet) + (distanceStreet/2), 1.5, 1.5);

//Crea un objeto lteHelper
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  //lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::HybridBuildingsPropagationLossModel"));

  lteHelper->SetPathlossModelAttribute ("ShadowSigmaExtWalls", DoubleValue (0));
  lteHelper->SetPathlossModelAttribute ("ShadowSigmaOutdoor", DoubleValue (1));
  lteHelper->SetPathlossModelAttribute ("ShadowSigmaIndoor", DoubleValue (1.5));
  //use always LOS  model
  lteHelper->SetPathlossModelAttribute ("Los2NlosThr", DoubleValue (1e6));
  lteHelper->SetSpectrumChannelType ("ns3::MultiModelSpectrumChannel");

//Crea los nodos para las eNBs
  NodeContainer enbNodes;
  int numENB=pow(nGrid+1,2); // pow es elevar, en este caso al cuadrado
  std::cout << "Número de eNBS " << numENB << std::endl;
  enbNodes.Create (numENB);

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
  NetDeviceContainer enbDevs;
//  NodeContainer ENBs; // lo utilizaremos para poder obtener parámetros como el ID de cada sector
  for (int ii=-nGrid/2;ii<=nGrid/2;ii++)
  {
    for (int jj=-nGrid/2;jj<=nGrid/2;jj++)
    {
      pos = Vector(ii*(distanceBuilding+distanceStreet),jj*(distanceBuilding+distanceStreet),15);
      Ptr<Node> node = enbNodes.Get(contador);
      Ptr<MobilityModel> mm = node->GetObject<MobilityModel> ();
      mm->SetPosition (Vector(pos));
//      ENBs.Add(enbNodes);
      enbDevs.Add (lteHelper->InstallEnbDevice (node)); // Llama a Cell config req del Scheduler a través de los objetos LteEnbNetDevice (func Initialize) y LteEnbRRC (func ConfigureMac)
      contador++;
    }
  }
	//printf("valor modif %d", subframes_modif);

//Configuramos las UEs.
NodeContainer UEs;
for(int ii=-nGrid/2;ii<=nGrid/2;ii++)
{
  NodeContainer ueNodes;
  ueNodes.Create (numberUesStreet);
    
  MobilityHelper mobilityUes;
  
  // Rejilla sobre la que se va a distribuir a los usuarios
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

  mobilityUes.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
			     "Speed", StringValue ("ns3::UniformRandomVariable[Min=99.0|Max=100.0]"),
			     "Bounds", RectangleValue (Rectangle((-distanceStreet/2)+(ii*(distanceBuilding + distanceStreet)), (distanceStreet/2)+(ii*(distanceBuilding + distanceStreet)), (-distanceStreet/2)-((nGrid/2)*(distanceBuilding+distanceStreet)), (distanceStreet/2)+((nGrid/2)*(distanceBuilding+distanceStreet))))); 

  mobilityUes.SetPositionAllocator (randomUePositionAlloc);
  mobilityUes.Install (ueNodes);
  BuildingsHelper::Install (ueNodes);
  UEs.Add(ueNodes);

  //Instala el protocolo LTE en las UEs    
  NetDeviceContainer ueDevs;
  ueDevs = lteHelper->InstallUeDevice (ueNodes);

  //Vincula las UEs con las eNBs. Esto configurará cada UE de acuerdo a la configuración de las eNBs, y crea una conexión RRC entre ellos
  lteHelper->AttachToClosestEnb(ueDevs, enbDevs);

  //Activa la portadora de radio entra la UE y la eNB vinculada
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VIDEO; // Flujo de datos de video
  EpsBearer bearer (q);
  lteHelper->ActivateDataRadioBearer (ueDevs, bearer);

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

  mobilityUes.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
			     "Speed", StringValue ("ns3::UniformRandomVariable[Min=99.0|Max=100]"),
			     "Bounds", RectangleValue (Rectangle((-distanceStreet/2)-((nGrid/2)*(distanceBuilding+distanceStreet)), (distanceStreet/2)+((nGrid/2)*(distanceBuilding+distanceStreet)), (-distanceStreet/2)+(jj*(distanceBuilding + distanceStreet)), (distanceStreet/2)+(jj*(distanceBuilding + distanceStreet)))));


  mobilityUes.SetPositionAllocator (randomUePositionAlloc);
  mobilityUes.Install (ueNodes);
  BuildingsHelper::Install (ueNodes);
  UEs.Add(ueNodes);

  //Instala el protocolo LTE en las UEs    
  NetDeviceContainer ueDevs;
  ueDevs = lteHelper->InstallUeDevice (ueNodes);

  //Vincula las UEs con las eNBs. Esto configurará cada UE de acuerdo a la configuración de las eNBs, y crea una conexión RRC entre ellos
  lteHelper->AttachToClosestEnb(ueDevs, enbDevs);

  //Activa la portadora de radio entra la UE y la eNB vinculada
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VIDEO; // Flujo de datos de Video
  EpsBearer bearer (q);
  lteHelper->ActivateDataRadioBearer (ueDevs, bearer); // Activa una bearer por cada UE

}


int numUEs=UEs.GetN();
std::cout << "Número de UES " << numUEs <<"\n" << std::endl;



BuildingsHelper::MakeMobilityModelConsistent (); // Da consistencia al modelo de mobilidad

// A partir de aquí llamamos a las funciones del Scheduler CRAN
// Los valores de SINR inst y medios serán recogidos en matrices: 
// - SINR inst:  una matriz tridimensional en la que para cada celda (RI) tendremos una matriz de N_usuarios x Nsubbandas con los valores de SINR.
// - SINR media: una matriz de N_usuarios x N_celdas en la que para cada para celda tendremos el valor de SINR medio del usuario en todo el ancho de banda.
// El tp inst and medio lo obtendremos con una fórmulao o una gráfica (por determinar)

// El tiempo de simulación está determinado por el número de subframes sobre las que se quiera realizar la simulación.
// 1 subframe = 1 ms --> x subframes = simTime (simulador entiende de seg)



std::cout << "*******PARÁMETROS DE CONFIGURACIÓN*********" << std::endl;
std::cout << "Fichero de resultados scheduler: " << schedulingPathResults << std::endl;
std::cout << "Scheduling UL/DL: " << ULDL << std::endl;
std::cout << "Número de subtramas máximas a considerar: " << Nsubframes << std::endl;
std::cout << "Longitud de la ventana (en nº de subtramas) sobre la que promediaremos el tp: " << Nsubf_avg << std::endl; // Lo vamos a considerar???
std::cout << "Número de subbandas: " << Nsubbands << std::endl;
std::cout << "Número de subbandas máximas por usuario: " << Nsubb_user << std::endl;
std::cout << "Número de subbandas con información de CQI: " << Nsubb_CQI << std::endl;
std::cout << "Número de RBs: " << NRB << std::endl; // ¿Si un RB son 12 portadoras y lo máximo que puede haber activo son 1200 porqué puede ser 110 RB??
std::cout << "Truncamiento de CQI: " << truncCQI << std::endl;
std::cout << "Debug Scheduler: " << debug << std::endl;
std::cout << "Semilla generador: " << seed_scheduling << std::endl;
std::cout << "Tipo de Scheduling: " << Scheduling << std::endl;
std::cout << "Rango de SINR sobre el que se calcula la ganancia de este Scheduler respecto al caso tradicional de RR: " << std::endl;
std::cout << "Ganancia SINR min: " << Schedgain_sinr_min << std::endl;
std::cout << "Ganancia SINR max: " << Schedgain_sinr_max << std::endl;
std::cout << "Número de puntos para la curva de ganancia: " << Schedgain_npoints << std::endl;
std::cout << "Schedgain_sector_file: " << Schedgain_sector_file << std::endl;


// Configuramos la capa RLC en modo UM (sin retransmisiones)
Ptr<LteEnbRrc> lteRrc = CreateObject<LteEnbRrc> ();
lteRrc -> SetAttribute("EpsBearerToRlcMapping", EnumValue(LteEnbRrc::RLC_UM_ALWAYS));

//enum LteEpsBearerToRlcMapping_t rlc;
//const char *rlcmode = NULL;
EnumValue rlc;
lteRrc -> GetAttribute("EpsBearerToRlcMapping", rlc);
//lteRrc -> Get("EpsBearerToRlcMapping");
std::cout << rlc.Get() << std::endl;
// Añadir (cuándo esté casi terminado) --> Si argv[x] == null -> error parametro no present

// Calculamos el tiempo de simulación en función del número de subtramas
double simTime=Nsubframes/1000;



// Variables a introducir en función CalculaScheduling

//double *p_idSectores=new double[numENB];
//for (int i=0; i<numENB; i++)
//	p_idSectores[i]=ENBs.GetId();

//FILE *pINPUT;
//pINPUT=fopen("input-lte-escenario-manhattan.txt", "r"); // CAMBIAR ESTE FICHERO POR EL DE TP !!!!!

double **Ganancias; // Matriz con los valores de SINR inst para cada usuario y subbanda
Ganancias=new double*[numUEs];
for (int i=0; i<numUEs; i++)
	Ganancias[i]=new double[Nsubbands];

//for (int i = 0; i < numUEs; i++) // Inicializar matriz --> sustituir por copiar "memcpy" cuando tengamos matriz de SINR values
//    memset(Ganancias[i], 0, Nsubbands * sizeof(double));


for (int i=0; i<numUEs; i++)
	for (int j=0; j<Nsubbands; j++)
		Ganancias[i][j]=20;

double *SINR=new double[numUEs];
for (int i=0; i < numUEs; i++)
	SINR[i]=5;


//muestra_ganancias(Ganancias, numUEs, Nsubbands);


//CCalculaScheduling CalculaScheduling(schedPathResults, (T_UL_DL)ULDL, Nsubframes, Nsubf_avg, Nsubbands, Nsubb_user, Nsubb_CQI, NRB,
//									 truncCQI, debug, seed_scheduling, Scheduling, Schedgain_sinr_min, Schedgain_sinr_max, Schedgain_npoints, Schedgain_sector_file,
//									 SINR, Ganancias, numENB, numUEs); // Pasarle parámetros
//CalculaScheduling.EjecutaScheduling();


for (int i=0;i<numUEs;i++) // Liberamos el espacio asignado en memoria a la matriz de ganancias una vez hemos trabajado con ellas
{
  delete [] Ganancias[i];
}

delete [] Ganancias;


// PASOS A SEGUIR

// Creamos RadioBearerInstance con los parámetros que queramos. Si necesitan actualizarse, creamos una función
// que actualiz los parámetros introducidos dentro de la clase. Esta función se llamará en la entidad RLC después
// de que cree una PDU para la capa MAC.

// Crear la clase ProportionalFairPackeScheduler (CCalculaScheduling) e insertarla en la clase virtual PacketScheduler
// (ver SimplePlacketScheduler class para entender los métodos que debemos implementar)

// Implementar PF en el método DoRunPacketScheduler




//To implement PF algorithms, the basically idea is this:
//
//the packet scheduler compute for each UE (activo??) and for all the available sub channel a metric
//
//m = instantaneous rate/ average rate.
//
//Then it assign a sub channel to that flow which has the highest metric.
//
//Then, for each scheduled flow, the packet scheduler should compute the quota of data that the flow can transmit using the assigned sub channels --> and communicate to RLC layer to resize the transmission buffer in order to avoid segmentation
//
//The quota of data depends from CQI values sent by the UE for the assigned sub channels (that will be mapped into the MCS value using the Amc Module) --> implement amc module
//
//As soon the scheduler has computed the quota of data that each scheduled flow can transmit, it should the RLC entities associated to these flows for dequeuing packet form the queue.
//
//After that, the packet scheduler should create the packet burst and send it to the PHY/Channel.



std::cout<<"Tipo de Scheduler: " << lteHelper->GetSchedulerType()<< std::endl;


//Habilitamos la salida de datos
  if(generateTraces)
  {
    lteHelper->EnablePhyTraces ();
    lteHelper->EnableMacTraces ();
    lteHelper->EnableRlcTraces ();
    lteHelper->EnablePdcpTraces ();
  }

//Guardamos la configuracion de la simulación en un fichero de texto
  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("output-lte-escenario-manhattan.txt"));
  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Save"));
  ConfigStore outputConfig2;
  outputConfig2.ConfigureDefaults ();
  outputConfig2.ConfigureAttributes ();

//Generamos el archivo que usara el netAnim
  AnimationInterface anim ("anim-lte-manhattan.xml");
  for (int i=0; i<numENB; ++i)
	anim.UpdateNodeColor(enbNodes.Get(i),0,255,0);
   // Create the animation object and configure for specified output
//pAnim = new AnimationInterface (animFile); 
// Provide the absolute path to the resource
//resourceId1 = pAnim->AddResource ("/Users/john/ns3/netanim-3.105/ns-3-logo1.png"); // añadir logos a los nodos
//resourceId2 = pAnim->AddResource ("/Users/john/ns3/netanim-3.105/ns-3-logo2.png");


//Generamos la salida para el gnuPlot
  Ptr<RadioEnvironmentMapHelper> remHelper;
  if (generateRem)
    {
      PrintGnuplottableUeListToFile ("ues.txt");
      PrintGnuplottableBuildingListToFile ("buildings.txt");
	  PrintGnuplottableEnbListToFile ("eNBs.txt");
		
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
 // PhyStatsCalculator::SetCurrentCellRsrpSinrFilename(	file	);


//  struct params_scheduler{
//	  const char *scheduling_results;
//	  T_UL_DL ULDL;
//	  int Nsubf;
//	  int Nsubf_promedia;
//	  int Nsubbands;
//	  int Nsubbands_user;
//	  int Nsubbands_CQI;
//	  int NRB;
//	  int TruncCQI;
//	  int Debug;
//	  unsigned int Semilla;
//	  int Scheduler_type;
//	  double Sinr_gain_min;
//	  double Sinr_gain_max;
//	  int Sinr_gain_points;
//	  int Schedgain_sector_file;
//	  double *SINR_avg;
//	  double **Ganancias;
//	  int NSectores;
//	  int Nusers;
//  };


  // Evento
//	Simulator::Schedule(Time(0.001),&prueba,lteHelper);


//  schedulerDRANMac::numPeras=8;
//  schedulerDRANMac::params_scheduler.scheduling_results=schedulingPathResults;
//  schedulerDRANMac::scheduling_path_results = schedPathResults;
//  schedulerDRANMac::ULDL = (T_UL_DL)ULDL;
//  schedulerDRANMac::Nsubf = Nsubframes;
//  schedulerDRANMac::Nsubf_promedia = Nsubf_avg;
//  schedulerDRANMac::Nsubbands = Nsubbands;
//  schedulerDRANMac::Nsubbands_user = Nsubb_user;
//  schedulerDRANMac::Nsubbands_CQI = Nsubb_CQI;
//  schedulerDRANMac::NRB = NRB;
//  schedulerDRANMac::TruncCQI = truncCQI;
//  schedulerDRANMac::Debug = debug;
//  schedulerDRANMac::Semilla = seed_scheduling;
//  schedulerDRANMac::Scheduler_type = Scheduling;
//  schedulerDRANMac::Sinr_gain_min = Schedgain_sinr_min;
//  schedulerDRANMac::Sinr_gain_max = Schedgain_sinr_max;
//  schedulerDRANMac::Sinr_gain_points = Schedgain_npoints;
//  schedulerDRANMac::Schedgain_sector_file = Schedgain_sector_file;
//  schedulerDRANMac::SINR_avg = SINR;
//  schedulerDRANMac::Ganancias = Ganancias;
//  schedulerDRANMac::NSectores = numENB;
//  schedulerDRANMac::Nusers = numUEs;


//  std::cout << "Scheduling UL/DL: " << ULDL << std::endl;
//  std::cout << "Número de subtramas máximas a considerar: " << Nsubframes << std::endl;
//  std::cout << "Longitud de la ventana (en nº de subtramas) sobre la que promediaremos el tp: " << Nsubf_avg << std::endl; // Lo vamos a considerar???
//  std::cout << "Número de subbandas: " << Nsubbands << std::endl;
//  std::cout << "Número de subbandas máximas por usuario: " << Nsubb_user << std::endl;
//  std::cout << "Número de subbandas con información de CQI: " << Nsubb_CQI << std::endl;
//  std::cout << "Número de RBs: " << NRB << std::endl; // ¿Si un RB son 12 portadoras y lo máximo que puede haber activo son 1200 porqué puede ser 110 RB??
//  std::cout << "Truncamiento de CQI: " << truncCQI << std::endl;
//  std::cout << "Debug Scheduler: " << debug << std::endl;
//  std::cout << "Semilla generador: " << seed_scheduling << std::endl;
//  std::cout << "Tipo de Scheduling: " << Scheduling << std::endl;
//  std::cout << "Rango de SINR sobre el que se calcula la ganancia de este Scheduler respecto al caso tradicional de RR: " << std::endl;
//  std::cout << "Ganancia SINR min: " << Schedgain_sinr_min << std::endl;
//  std::cout << "Ganancia SINR max: " << Schedgain_sinr_max << std::endl;
//  std::cout << "Número de puntos para la curva de ganancia: " << Schedgain_npoints << std::endl;
//  std::cout << "Schedgain_sector_file: " << Schedgain_sector_file << std::endl;
//Lanzamos la simulación
  Simulator::Run ();


//Terminamos la simulación
  Simulator::Destroy ();
  std::cout << "FIN." << std::endl;


  return 0;
}



