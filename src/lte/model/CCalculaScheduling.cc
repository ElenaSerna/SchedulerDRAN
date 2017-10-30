/* 
 * File:   CCalculaScheduling.cpp
 * Author: urano
 * 
 * Created on October 19, 2010, 9:54 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <libgen.h>
#include <math.h>
#include <time.h>
#include "ns3/assert.h"
#include <iostream>
#include <new>
#include <exception>

#include "CCalculaScheduling.h"
#include "CUsuario.h"
#include "CScheduler.h"
#include "CSchedException.h"
//#include "Random.h"
#include "TiposScheduling.h"



using namespace std;


// Fuente
CCalculaScheduling::CCalculaScheduling(const char *scheduling_results, T_UL_DL ULDL, int Nsubf, int Nsubf_promedia, int Nsubbands, int Nsubbands_user,
		int Nsubbands_CQI, int NRB, int TruncCQI, int Debug, unsigned int Semilla, int Scheduler_type, double Sinr_gain_min,
		double Sinr_gain_max, int Sinr_gain_points, int Schedgain_sector_file, double *SINR_avg, double **Ganancias, int NSectores, int Nusers)
{
	// Asociación de parámetros
    m_iULDL = ULDL; // UL=0
    m_iNsubframes_tot = Nsubf;
    m_iNsubframes_promedia = Nsubf_promedia;
    m_iNsubframes=0; 						// subframes transcurridas
    m_iNsubbandas = Nsubbands;
    m_iNsubbandas_usuario = Nsubbands_user;
    m_iNsubbandas_CQI = Nsubbands_CQI;
    m_iNSectores = NSectores;
    m_iNRB = NRB;
    //m_iNRB_throughputcurve = 0;
    m_iNusuarios = Nusers;
    m_iScheduler = Scheduler_type;
    m_iSemilla = Semilla;
    m_iDebug = Debug;
    m_iTruncCQI = TruncCQI;
    m_pPath_results = scheduling_results; // Cadena de caracteres con el path en el que almacenar los resultados del scheduling
    m_dSinrMejoraUsuario_min = Sinr_gain_min;
    m_dSinrMejoraUsuario_max = Sinr_gain_max;
    m_iNumIntervalosMejoraUsuario = Sinr_gain_points;
    m_iFlagSectorFile = Schedgain_sector_file; // indica si se generan curvas de scheduling gain por cada sector
    m_pSINR_avg = SINR_avg;
    m_pGanancias_subbanda=Ganancias;
    m_pGanancias_subbanda_trunc = NULL;
    m_pThroughput_subbanda = NULL; // Inicialmente a NULL
    m_pThroughput_subbanda_trunc = NULL;
    m_pGanancias_subbanda_usuario = NULL;
    m_pGanancias_subbanda_usuario_trunc = NULL;
    m_pThroughput_subbanda_usuario_trunc = NULL;


    // ¿¿ A QUÉ INICIALIZAMOS ESTOS VALORES ??
    m_iUnits_curve = (TRATE_UNITS)0; // Unidades de la curva TP cargada
    m_pFichCurvaTp = NULL; // Fichero curva TP vs SINR --> para sacar los valores de TP correspondientes a la SINR de cada user/subbanda/sector
    m_pCurva_sinr = NULL; // valores de SINR de la curva tp=f(sinr) cargada anterior
    m_pCurva_throughput = NULL; // valores de TP de la curva cargada anterior
    m_pUsuarios = NULL; // usuarios activos

    m_pMejoraUsuariosSector = NULL;
    m_pMejoraUsuariosGlobal = NULL;
    m_pNumOcurrenciasMejoraGlobal = NULL;
    //m_pFichMejora = NULL;
    m_pFichThroughputGlobal = NULL;
    //m_iNpuntos_curva_sinr = 0;

    // inicializa la semilla a la hora del sistema o a un valor específico (m_iSemilla)
    if (m_iSemilla == 0)
    {
        struct timeval tv;
        if (gettimeofday(&tv, NULL) == -1)
        {
            CSchedException excepcion(ERROR_SEMILLA);
            throw(excepcion);
        }
        if ( ( (unsigned int)(tv.tv_sec * ((double)tv.tv_usec/1000)) ) % MODSEMILLA != 0 )
            srand( ( (unsigned int)(tv.tv_sec * ((double)tv.tv_usec/1000)) ) % MODSEMILLA );
        else
            srand((unsigned int)(tv.tv_sec));
            //srand(TIME(NULL));
    }
    else
        srand(m_iSemilla);

    // Reserva memoria
    ReservaMemoria();

    // abre ficheros
//    if (AbreFicheros() == -1)
//    {
//        CSchedException excepcion(ERROR_FICHEROS);
//        throw(excepcion);
//    }
    AbreFicheros();

    // Calculamos Matriz de Throughput
    CalculaThroughput();

}



CCalculaScheduling::~CCalculaScheduling() // Destructor
{
    //CierraFicheros();
    LiberaMemoria();
}



//bool CCalculaScheduling::LeeCurvaThroughput(FILE *fich, int *Nelem_curva, double *sinr, double *tp) const // MODIFICAR ESTA PARTE
//{
    //char str[MAX_LINE];

    //do
    //{
        //if (fgets(&str[0], MAX_LINE, fich) == NULL)
            //printf("cannot read throughput vs sinr curve\n");
            //return false;
    //}
    //while (str[0] != 'c');

    //*Nelem_curva = 0;
    //if (sscanf(str, "%*c %lf %lf\n", sinr++, tp++) != 2)
        //return false;
    //(*Nelem_curva)++;

    //while (fscanf(fich, "%*c %lf %lf\n", sinr++, tp++) == 2)
        //(*Nelem_curva)++;

    //return (*Nelem_curva != 0);
//}


// interpola la curva de throughput 
//double CCalculaScheduling::DevuelveThroughput(double *curva_sinr, double *curva_throughput, int npuntos_curva, double sinr) const
//{
    //// encuentra los dos puntos que encierran sinr
    //for (int i = 0; i < npuntos_curva - 1; i++)
    //{
        //if (curva_sinr[i] <= sinr && sinr < curva_sinr[i+1])
        //{
            //double m = (curva_throughput[i+1] - curva_throughput[i]) / (curva_sinr[i+1] - curva_sinr[i]);
            //return (curva_throughput[i] + m*(sinr - curva_sinr[i]));
        //}
    //}

    //// fuera de margen
    //if (sinr < curva_sinr[0])
        //return curva_throughput[0];
    //else if (sinr >= curva_sinr[npuntos_curva - 1])
        //return curva_throughput[npuntos_curva - 1];
    //else
        //return 0;
//}


void CCalculaScheduling::ReservaMemoria()
{
    int i;

    try
    {
        m_pCurva_sinr = new double[MAX_ELEM_CURVA_TP];
        m_pCurva_throughput = new double[MAX_ELEM_CURVA_TP];
        m_pUsuarios = new CUsuario*[m_iNusuarios];
        for (i = 0; i < m_iNusuarios; i++)
            m_pUsuarios[i] = NULL;
        for (i = 0; i < m_iNusuarios; i++)
            m_pUsuarios[i] = new CUsuario(m_iNsubbandas, m_iNsubbandas_CQI, m_iNsubframes_promedia,
                                       m_iTruncCQI, m_iNRB);
        m_pGanancias_subbanda_trunc = new double*[m_iNusuarios];
        for (i = 0; i < m_iNusuarios; i++)
        	m_pGanancias_subbanda_trunc[i] = NULL;
        for (i = 0; i < m_iNusuarios; i++)
        	m_pGanancias_subbanda_trunc[i] = new double[m_iNsubbandas];

        m_pThroughput_subbanda = new double*[m_iNusuarios];
        for (i = 0; i < m_iNusuarios; i++)
        	m_pThroughput_subbanda[i] = NULL;
        for (i = 0; i < m_iNusuarios; i++)
        	m_pThroughput_subbanda[i] = new double[m_iNsubbandas];

        m_pThroughput_subbanda_trunc = new double*[m_iNusuarios];
        for (i = 0; i < m_iNusuarios; i++)
        	m_pThroughput_subbanda_trunc[i] = NULL;
        for (i = 0; i < m_iNusuarios; i++)
        	m_pThroughput_subbanda_trunc[i] = new double[m_iNsubbandas];

        //m_pId_sectores = new char*[MAX_SECTORES];
        //for (i = 0; i < MAX_SECTORES; i++)
            //m_pId_sectores[i] = NULL;
        m_pMejoraUsuariosSector = new double[m_iNumIntervalosMejoraUsuario];
        m_pMejoraUsuariosGlobal = new double[m_iNumIntervalosMejoraUsuario];
        m_pNumOcurrenciasMejoraGlobal = new int[m_iNumIntervalosMejoraUsuario];
    }
//    catch(const char *e)
    catch(...)
    {
        CSchedException excepcion(ERROR_MEMORIA);
        std::cout << "exception occured in CalculaScheduling"<< std::endl;
//        std::cout << e << std::endl;
        throw(excepcion);
    }
}


void CCalculaScheduling::LiberaMemoria()
{
    int i;

    delete []m_pCurva_sinr;
    delete []m_pCurva_throughput;
    for (i = 0; i < m_iNusuarios; i++)
        delete m_pUsuarios[i];
    delete []m_pUsuarios;
    for (i = 0; i < m_iNusuarios; i++)
        delete m_pGanancias_subbanda_trunc[i];
    delete m_pGanancias_subbanda_trunc;
    for (i = 0; i < m_iNusuarios; i++)
        delete m_pUsuarios[i];
    delete []m_pUsuarios;
    for (i = 0; i < m_iNusuarios; i++)
        delete m_pThroughput_subbanda[i];
    delete []m_pThroughput_subbanda;
    for (i = 0; i < m_iNusuarios; i++)
        delete m_pThroughput_subbanda_trunc[i];
    delete []m_pThroughput_subbanda_trunc;

    //for (i = 0; i < m_iNsectores; i++)
    //{
        //if (m_pId_sectores[i] != NULL)
            //free(m_pId_sectores[i]);
    //}
    //delete []m_pId_sectores;
    delete []m_pMejoraUsuariosSector;
    delete []m_pMejoraUsuariosGlobal;
    delete []m_pNumOcurrenciasMejoraGlobal;
}


void CCalculaScheduling::AbreFicheros()
{
    char str[MAX_LINE];
    //char strfich[MAX_LINE];

    // -- fichero curva SINR - throughput
    //sprintf(str, "%s.crvf", argv[12]);
//    m_pFichCurvaTp = fopen(m_FichCurvaTp, "rt"); // puntero al fichero de curva tp
//    if (m_pFichCurvaTp == NULL)
//    {
//        printf("cannot open throughput vs sinr curve\n");
//        return (-1);
//    }

    // -- Creamos fichero de ganancias de scheduling global, de throughput de cada sector
//    if (strcmp(m_pNsector, "all") == 0)
//    {
        sprintf(str, "%s/sinr_schedgains_mejora.txt", m_pPath_results);
        m_pFichMejora = fopen(str, "wt");
        if (m_pFichMejora == NULL)
        {
        	std::cout << "Cannot create Mejora File"<< std::endl;
        	CSchedException excepcion(ERROR_FICHEROS);
        	throw(excepcion);
        }
        sprintf(str, "%s/throughput_sectors.txt", m_pPath_results);
        m_pFichThroughputGlobal = fopen(str, "wt");
        if (m_pFichThroughputGlobal==NULL)
        {
        	std::cout << "Cannot create Throughput Sectors File"<< std::endl;
        	CSchedException excepcion(ERROR_FICHEROS);
        	throw(excepcion);

        }

//    }

}


//void CCalculaScheduling::CierraFicheros()
//{
//	    fclose(m_FichCurvaTp);
//}



void CCalculaScheduling::CalculaThroughput()
{
	for (int i=0; i<m_iNusuarios; i++)
	{
		m_pGanancias_subbanda_usuario=GetValorUsuario(m_pGanancias_subbanda,i,m_iNsubbandas); // Obtenemos para cada usuario el valor de SINR y TP para todas las subbandas
		for (int j=0; j<m_iNsubbandas; j++)
		{
			double sinr_subbanda = m_pSINR_avg[j] + m_pGanancias_subbanda_usuario[j];
	//		m_pThroughput_subbanda[j] = DevuelveThroughput(m_pCurva_sinr, m_pCurva_throughput, m_iNpuntos_curva_sinr, sinr_subbanda)/* *
	//                                                                  ((double)m_iNRB / m_iNRB_throughputcurve)*/ / m_iNsubbandas;
			m_pThroughput_subbanda[i][j]=sinr_subbanda; // Matriz de tp instantáneo usuario-subbanda
		}
	}
}


// Calcula los valores de granancias truncados en m_pGanancias_trunc
void CCalculaScheduling::SetGananciasTruncadas(double **m_pGanancias)
{
//    int j;
    if (m_pGanancias != NULL)
    {
    	for (int i=0; i<m_iNusuarios; i++)
    	{
    		 // higher-layer configured subband feedback: trunca las ganancias a -1,0,1,2
    		         for (int j = 0; j < m_iNsubbandas; j++)
    		         {
    		            if (m_pGanancias[i][j] < -0.5)
    		                m_pGanancias_subbanda_trunc[i][j] = -1;
    		            else if (m_pGanancias[i][j] < 0.5)
    		                m_pGanancias_subbanda_trunc[i][j] = 0;
    		            else if (m_pGanancias[i][j] < 1.5)
    		                m_pGanancias_subbanda_trunc[i][j] = 1;
    		            else
    		                m_pGanancias_subbanda_trunc[i][j] = 2;
    		          }
    	}


    }
    m_iNsubframes = 0; // No ha transcurrido aún ninguna subtrama
}

void CCalculaScheduling::SetThroughputTruncado(double **m_pThroughput)
{
//	int j;
	if (m_pThroughput != NULL)
	{
		for (int i=0; i<m_iNusuarios; i++)
		{
			m_pGanancias_subbanda_usuario_trunc=GetValorUsuario(m_pGanancias_subbanda_trunc,i,m_iNsubbandas); // Obtenemos para cada usuario el valor de SINR y TP para todas las subbandas
			for (int j=0; j<m_iNsubbandas; j++)
			{
				double sinr_subbanda = m_pSINR_avg[j] + m_pGanancias_subbanda_usuario_trunc[j];
		//		m_pThroughput_subbanda[j] = DevuelveThroughput(m_pCurva_sinr, m_pCurva_throughput, m_iNpuntos_curva_sinr, sinr_subbanda)/* *
		//                                                                  ((double)m_iNRB / m_iNRB_throughputcurve)*/ / m_iNsubbandas;
				m_pThroughput_subbanda_trunc[i][j]=sinr_subbanda; // Matriz de tp instantáneo usuario-subbanda
			}
		}
	}

}

double * CCalculaScheduling::GetValorUsuario(double **m_matrix,int row, int cols)
{	double *fila= new double[cols];
	for (int j=0; j<cols; j++)
		fila[j]=m_matrix[row][j];
	return fila;
}

// ejecucion de scheduling en un sector
void CCalculaScheduling::EjecutaSchedulingSector(int idSector, FILE *fichlog, FILE *fichusers, FILE *fichhist)
//void CCalculaScheduling::EjecutaSchedulingSector(int idSector)
{
	int valor_temporal=40; // ELIMINAR CUANDO SEPAMOS QUE HACER CON EL TP!!
//    int indice_flt;
	std::cout << "Valor primero " << m_pThroughput_subbanda[0][1] << std::endl;

	for (int i=0; i<m_iNusuarios; i++)
	{
		m_pGanancias_subbanda_usuario=GetValorUsuario(m_pGanancias_subbanda,i,m_iNsubbandas); // Obtenemos para cada usuario el valor de SINR y TP para todas las subbandas
		m_pThroughput_subbanda_usuario=GetValorUsuario(m_pThroughput_subbanda,i,m_iNsubbandas);
		m_pUsuarios[i]->SetSINR(m_pSINR_avg[i]);
        m_pUsuarios[i]->SetGanancias(m_pGanancias_subbanda_usuario);
        m_pUsuarios[i]->SetThroughputSubbandas(m_pThroughput_subbanda_usuario);
//        double throughput_wideband = DevuelveThroughput(m_pCurva_sinr, m_pCurva_throughput, m_iNpuntos_curva_sinr, m_pMap_fichSinr_flt[indice_flt])/* * ((double)m_iNRB / m_iNRB_throughputcurve)*/;
//        m_pUsuarios[i]->SetThroughputWideband(throughput_wideband);
        m_pUsuarios[i]->SetThroughputWideband(valor_temporal); // MODIFICAR CUANDO TENGAMOS CURVA/ECUACIÓN THROUGHPUT!!!
//        if (m_iTruncCQI)
//        {
//        	m_pGanancias_subbanda_usuario_trunc=GetValorUsuario(m_pGanancias_subbanda_trunc,i,m_iNsubbandas);
//        	m_pThroughput_subbanda_usuario_trunc=GetValorUsuario(m_pThroughput_subbanda_trunc,i,m_iNsubbandas);
//            m_pUsuarios[i]->SetThroughputSubbandasTrunc(m_pThroughput_subbanda_usuario_trunc);
//        }
	}

	std::cout << "Dentro de EjecutaSchedulingSector" << std::endl;

     //// lanza el scheduler --> COMPROBAR PARÁMETROS DE ENTRADA ESTÁN BIEN
     CScheduler Scheduler((T_SCHEDULER)m_iScheduler, m_iULDL, m_iNusuarios, m_iNsubbandas, m_iNsubbandas_usuario,
        			   idSector, m_pUsuarios, m_iDebug, m_dNo_data, fichlog, fichusers,
					   m_dSinrMejoraUsuario_min, m_dSinrMejoraUsuario_max, m_iNumIntervalosMejoraUsuario, m_iUnits_curve);
     Scheduler.RunScheduler(m_iNsubframes_tot);

    // escribe el histograma de mejora de usuarios del sector en el fichero
    Scheduler.GetMejoraUsuario(m_pMejoraUsuariosSector);

    if (fichhist != NULL)
    {
        fprintf(fichhist, "sinr\tsched_gain\n");
        for (int i = 0; i < m_iNumIntervalosMejoraUsuario; i++)
            fprintf(fichhist, "%.1f\t%.2f\n", (m_dSinrMejoraUsuario_min + i*m_dDeltasinr_mejora), m_pMejoraUsuariosSector[i]);
    }

    //// escribe los ficheros de throughput global
    if (m_pFichThroughputGlobal != NULL)
    {
        fprintf(m_pFichThroughputGlobal, "%.2f\n", Scheduler.GetCellThroughput());
        fflush(m_pFichThroughputGlobal);
    }

}



// metodo principal de ejecucion. Ejecuta el scheduler en el/los sector(es)
void CCalculaScheduling::EjecutaScheduling()
{
//    int i, j;
    char strfich[MAX_LINE];
//    int m_iNSectores;
//    int Numsectores;
    const char *str2 = NULL;
    FILE *fichlog = NULL;
    FILE *fichusers = NULL;
    FILE *fichhist = NULL;
    //int sector = 0;
//	printf("Dentro de Scheduling");
	std::cout << "Dentro de Ejecuta Scheduling" << std::endl;

	memset(m_pMejoraUsuariosGlobal, 0, m_iNumIntervalosMejoraUsuario * sizeof(double));
    memset(m_pNumOcurrenciasMejoraGlobal, 0, m_iNumIntervalosMejoraUsuario * sizeof(int));

    m_dDeltasinr_mejora = (m_dSinrMejoraUsuario_max - m_dSinrMejoraUsuario_min) / m_iNumIntervalosMejoraUsuario;

    switch(m_iScheduler)
    {
        case ROUND_ROBIN:
            str2 = "RR";
            break;
        case MAXIMUM_RATE:
            str2 = "MR";
            break;
        case PROPORTIONAL_FAIR:
            str2 = "PF";
            break;
        case FREQ_MAXIMUM_RATE:
            str2 = "FMR";
            break;
        case FREQ_PROPORTIONAL_FAIR:
            str2 = "FPF";
            break;
        case PROPORTIONAL_FAIR_RES:
            str2 = "PFRES";
            break;
        case FREQ_PROPORTIONAL_FAIR_RES:
            str2 = "FPFRES";
            break;
    }

    ////if (strcmp(m_pNsector, "all") == 0)
    ////{
        ////pNsector = m_pId_sectores;
        ////Numsectores = m_iNsectores;
    ////}
    ////else
    ////{
        ////pNsector = &m_pNsector;
        ////Numsectores = 1;
    ////}
	
	int Numsectores = m_iNSectores;

    // Elegir aleatoriamente el sector sobre el que aplicar el scheduling
//	int sector = rand() % Numsectores ; // genera un número aleatorio entre 0 y Numsectores
	int sector=1;
	std::cout << "Sector inicial: " << sector << std::endl;
        

    ////// Bucle principal de ejecucion de scheduling por sectores
    for (int i = 0; i < Numsectores; i++)
    {
        // crea ficheros de log, users e histograma (este ultimo solo si m_iFlagSectorFile = 1)
        //   el fichero de users es unico si se simula el escenario completo, agregando la informacion de cada sector
        // log
        sprintf(strfich, "%s/logscheduler__%s_%d_%s_%dsubf_%dusers_%dsubb_%dsubbuser_%dCQI_%dsubfavg_seed%d%s.txt", m_pPath_results,
                (m_iULDL == UL)? "UL" : "DL", sector, str2, m_iNsubframes_tot, m_iNusuarios, m_iNsubbandas, m_iNsubbandas_usuario, m_iNsubbandas_CQI,
                m_iNsubframes_promedia, m_iSemilla, (m_iTruncCQI)? "_trunc" : "");
        fichlog = fopen(strfich, "wt");
        if (fichlog == NULL)
        {
            std::cout <<"cannot create log file"<< std::endl;
            CSchedException excepcion(ERROR_FICHEROS);
            throw(excepcion);
        }

        // users
        if (Numsectores == 1)
        {
            sprintf(strfich, "%s/users__%s_%d_%s_%dsubf_%dusers_%dsubb_%dsubbuser_%dCQI_%dsubfavg_seed%d%s.txt", m_pPath_results,
                    (m_iULDL == UL)? "UL" : "DL", sector, str2, m_iNsubframes_tot, m_iNusuarios, m_iNsubbandas, m_iNsubbandas_usuario, m_iNsubbandas_CQI,
                    m_iNsubframes_promedia, m_iSemilla, (m_iTruncCQI)? "_trunc" : "");
        }
        else
        {
            sprintf(strfich, "%s/users__%s_allsectors_%s_%dsubf_%dusers_%dsubb_%dsubbuser_%dCQI_%dsubfavg_seed%d%s.txt", m_pPath_results,
                    (m_iULDL == UL)? "UL" : "DL", str2, m_iNsubframes_tot, m_iNusuarios, m_iNsubbandas, m_iNsubbandas_usuario, m_iNsubbandas_CQI,
                    m_iNsubframes_promedia, m_iSemilla, (m_iTruncCQI)? "_trunc" : "");
        }
        fichusers = fopen(strfich, (Numsectores == 1 || i == 0)? "wt" : "at");
        if (fichusers == NULL)
        {
//            printf("cannot create users file\n");
            std::cout <<"cannot create users file"<< std::endl;
            CSchedException excepcion(ERROR_FICHEROS);
            throw(excepcion);
        }
        if (i == 0)
        {
            // cabecera con la leyenda de los campos
            fprintf(fichusers, "name_sector\tuser\tsinr\tthroughput_wideband");
            for (int j = 0; j < m_iNsubbandas; j++)
                fprintf(fichusers, "\tgain_%d", j+1);
            fprintf(fichusers, "\tthroughput_avg\tsched_gain\tNsubf_sched\n");
        }

        // schedgains
        sprintf(strfich, "%s/schedgains__%s_%d_%s_%dsubf_%dusers_%dsubb_%dsubbuser_%dCQI_%dsubfavg_seed%d%s.txt", m_pPath_results,
                (m_iULDL == UL)? "UL" : "DL", sector, str2, m_iNsubframes_tot, m_iNusuarios, m_iNsubbandas, m_iNsubbandas_usuario, m_iNsubbandas_CQI,
                m_iNsubframes_promedia, m_iSemilla, (m_iTruncCQI)? "_trunc" : "");
        if (Numsectores == 1 || (Numsectores > 1 && m_iFlagSectorFile))
        {
            sprintf(strfich, "%s/sinr_schedgains__%d.txt", m_pPath_results, sector);
            fichhist = fopen(strfich, "wt");
            if (fichhist == NULL)
            {
//                printf("cannot create scheduling gains file\n");
                std::cout <<"cannot create scheduling gains"<< std::endl;
                CSchedException excepcion(ERROR_FICHEROS);
                throw(excepcion);
            }
        }


        // Scheduling del sector
        std::cout << "Scheduling on Sector "<< sector << " of " << Numsectores << "\n" ;

        // Truncamos ganancias y throughput si está indicado
        if (m_iTruncCQI)
        {
        	SetGananciasTruncadas(m_pGanancias_subbanda);
        	SetThroughputTruncado(m_pThroughput_subbanda);
        }

//        EjecutaSchedulingSector(sector);
        EjecutaSchedulingSector(sector, fichlog, fichusers, fichhist);

     
		// Acumula puntos de mejora por scheduling --> NO LO ENTIENDO MUY BIEN!!
            for (int j = 0; j < m_iNumIntervalosMejoraUsuario; j++)
            {
                if (m_pMejoraUsuariosSector[j])
                {
                    m_pMejoraUsuariosGlobal[j] += m_pMejoraUsuariosSector[j];
                    (m_pNumOcurrenciasMejoraGlobal[j])++;
                }
            }
        
        
        // cierra ficheros
        fclose(fichlog);
        fclose(fichusers);
        if (fichhist != NULL)
            fclose(fichhist);

        sector++;
    }


    //// Calcula el histograma global para el caso de escenario completo
    //if (strcmp(m_pNsector, "all") == 0)
    //{
		int caract=fprintf(m_pFichMejora, "sinr\tsched_gain\n");
		std::cout << caract << std::endl;
		for (int i = 0; i < m_iNumIntervalosMejoraUsuario; i++)
		{
			if (m_pNumOcurrenciasMejoraGlobal[i])
				m_pMejoraUsuariosGlobal[i] /= m_pNumOcurrenciasMejoraGlobal[i];

			fprintf(m_pFichMejora, "%.1f\t%.2f\n", m_dSinrMejoraUsuario_min + i*m_dDeltasinr_mejora, m_pMejoraUsuariosGlobal[i]);
		}
		fclose(m_pFichMejora);
    //}
}

