/* 
 * File:   CScheduler.cpp
 * Author: jlh
 * 
 * Created on May 25, 2010, 5:36 PM
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include "CScheduler.h"
#include "CUsuario.h"
#include "CSchedException.h"
//#include "Random.h"
#include "TiposScheduling.h"

#include <iostream>

using namespace std;


const int CScheduler::m_iNSubframes_max = 100000;           // numero maximo de subframes donde calcula el throughput medio de cada usuario


CScheduler::CScheduler(T_SCHEDULER Scheduler, T_UL_DL uldl, int Nusuarios, int Nsubbandas, int Nsubbandas_usuario,
                       int idSector, CUsuario** pusuarios, int debug, double no_data, FILE *fichlog, FILE *fichusers,
                       double sinrmin_schedgain, double sinrmax_schedgain, int npuntos_schedgain, TRATE_UNITS units_curve)
//CScheduler::CScheduler(T_SCHEDULER Scheduler, T_UL_DL uldl, int Nusuarios, int Nsubbandas, int Nsubbandas_usuario,
                       //int idSector, int debug, double no_data,
                       //double sinrmin_schedgain, double sinrmax_schedgain, int npuntos_schedgain, TRATE_UNITS units_curve)

{
    int i;

    m_iScheduler = Scheduler;
    m_iULDL = uldl;
    m_iNUsuarios = Nusuarios;
    m_iNsubbandas = Nsubbandas;
    m_iNsubbandas_usuario = Nsubbandas_usuario;
//    m_pNsector = pNsector;
    m_idSector=idSector;
    m_pUsuarios = pusuarios;
    m_iDebug = debug;
    m_dNo_data = no_data;
    m_pFichLog = fichlog;
    m_pFichUsers = fichusers;
    m_dSinrMejoraUsuario_min = sinrmin_schedgain;
    m_dSinrMejoraUsuario_max = sinrmax_schedgain;
    m_iNumIntervalosMejoraUsuario = npuntos_schedgain;
    m_iUnits_curve = units_curve;

    m_iNsubframes_tot = 0;
    m_dThroughput_subframe = 0;
    m_dThroughput_global = 0;
    m_pMetricas = NULL;
    m_pUsuarioSubbanda = NULL;
    m_pSubbandasUsuario = NULL;
    m_pThroughputUsuarios = NULL;
    m_pThroughput_medio = NULL;
    m_pThroughput_RR = NULL;
    m_pNsubframes_asign = NULL;
    m_dMejora_media = 0;
    m_pMejoraUsuarioVsSinr = NULL;
    m_pNumOcurrenciasMejora_Sinr = NULL;
    m_dDeltasinr_mejora = 0;

    try
    {
        m_pMetricas = new double*[m_iNUsuarios];
        for (i = 0; i < m_iNUsuarios; i++)
            m_pMetricas[i] = NULL;
        for (i = 0; i < m_iNUsuarios; i++)
            m_pMetricas[i] = new double[m_iNsubbandas];
        m_pUsuarioSubbanda = new int[m_iNsubbandas];
        m_pSubbandasUsuario = new int[m_iNsubbandas];
        m_pThroughputUsuarios = new double*[m_iNUsuarios];
        for (i = 0; i < m_iNUsuarios; i++)
            m_pThroughputUsuarios[i] = NULL;
        for (i = 0; i < m_iNUsuarios; i++)
            m_pThroughputUsuarios[i] = new double[m_iNSubframes_max]; // Throughput de cada usuario y subframe
        m_pThroughput_medio = new double[m_iNUsuarios];
        m_pThroughput_RR = new double[m_iNUsuarios];
        m_pNsubframes_asign = new int[m_iNUsuarios];
        m_pMejoraUsuarioVsSinr = new double[m_iNumIntervalosMejoraUsuario];
        m_pNumOcurrenciasMejora_Sinr = new int[m_iNumIntervalosMejoraUsuario];
    }
    catch(...)
    {
        CSchedException excepcion(ERROR_MEMORIA);
        std::cout << "exception occured in Scheduler"<< std::endl;
        throw(excepcion);
    }

    // inicializa
    for (int i = 0; i < m_iNsubbandas; i++)
        m_pUsuarioSubbanda[i] = -1; // ningún usuario asignado
    for (int i = 0; i < m_iNUsuarios; i++)
        memset(m_pThroughputUsuarios[i], 0, m_iNSubframes_max * sizeof(double)); 

    m_dDeltasinr_mejora = (m_dSinrMejoraUsuario_max - m_dSinrMejoraUsuario_min) / m_iNumIntervalosMejoraUsuario;

}


CScheduler::~CScheduler()
{
    for (int i = 0; i < m_iNUsuarios; i++)
        delete []m_pMetricas[i];
    delete []m_pMetricas;
    delete []m_pUsuarioSubbanda;
    delete []m_pSubbandasUsuario;
    for (int i = 0; i < m_iNUsuarios; i++)
        delete []m_pThroughputUsuarios[i];
    delete []m_pThroughputUsuarios;
    delete []m_pThroughput_medio;
    delete []m_pThroughput_RR;
    delete []m_pNsubframes_asign;
    delete []m_pMejoraUsuarioVsSinr;
    delete []m_pNumOcurrenciasMejora_Sinr;
}


void CScheduler::SetMetricas(int fila,double a,double b,double c,double d,double e,double f,double g,double h)
{
//		double Metricas[8][8]={190, 100, 20, 88, 20, 72, 130, 120, 600, 10, 60, 24, 40, 210, 110, 10,
//		20, 180, 310, 400, 180, 40, 220, 305, 10, 200, 20, 500, 410, 160, 130, 80,
//		100, 30, 20, 299, 15, 40, 50, 250, 310, 150, 330, 10, 60, 10, 500, 800,
//		160, 10, 20, 10, 245, 5, 90, 200, 30, 9, 310, 301, 160, 110, 550, 10};
//		return Metricas;
		m_pMetricas[fila][0]=a;
		m_pMetricas[fila][1]=b;
		m_pMetricas[fila][2]=c;
		m_pMetricas[fila][3]=d;
		m_pMetricas[fila][4]=e;
		m_pMetricas[fila][5]=f;
		m_pMetricas[fila][6]=g;
		m_pMetricas[fila][7]=h;


//		m_pMetricas[1]={600, 10, 60, 24, 40, 210, 110, 10};
//		m_pMetricas[2]={20, 180, 310, 400, 180, 40, 220, 305};
//		m_pMetricas[3]={10, 200, 20, 500, 410, 160,	130, 80};
//		m_pMetricas[4]={100, 30, 20, 299, 15, 40, 50, 250};
//		m_pMetricas[5]={310, 150, 330, 10, 60, 10, 500, 800};
//		m_pMetricas[6]={160, 10, 20, 10, 245, 5, 90, 200};
//		m_pMetricas[7]={30,	9, 310,	301, 160, 110, 550, 10};
}



double * CScheduler::GetValorUsuario(double **m_matrix,int row, int cols)
{	double *fila= new double[cols];
	for (int j=0; j<cols; j++)
		fila[j]=m_matrix[row][j];
	return fila;
}



// considera usuario asignado cuando tiene nsubbandas asignaciones de subbanda.
bool CScheduler::EsUsuarioAsignado(int usuario, int nsubbandas) const
{
    int nsubbandas_asign = 0;
    for (int i = 0; i < m_iNsubbandas; i++)
    {
        if (m_pUsuarioSubbanda[i] == usuario)
        {
            if ((++nsubbandas_asign) == nsubbandas)
                return true;
        }
    }
    return false;
}

// indica si estan asignadas las columnas: subbanda, subbanda + 1, ..., subbanda + nsubbandas - 1
// devuelve false si alguna subbanda esta asignada (no importa a que usuario); en caso contrario true
//bool CScheduler::SubbandasLibres(int subbanda, int nsubbandas) const
//{
//    for (int i = 0; i < nsubbandas; i++)
//    {
//        if (m_pUsuarioSubbanda[subbanda + i] != -1)
//            return false;
//    }
//    return true;
//}

// Encuentra el usuario con metrica maxima en la columna dada, descontando
// los usuarios de la lista m_pUsuarioSubbanda con m_iNsubbandas_usuario asignaciones de subbanda.
// Cuando hay varios usuarios con metrica maxima, aleatoriza el elegido.
// Admite exceptuar el usuario "usuario_excepcion" de la busqueda.
// Si todos los usuarios han sido ya asignados devuelve -1
int CScheduler::MaxColumna(int col, int usuario_excepcion = -1) const
{
    double tpmax = -1;
    int imax = -1;
    for (int i = 0; i < m_iNUsuarios; i++)
    {
        if ((i == usuario_excepcion) || EsUsuarioAsignado(i, m_iNsubbandas_usuario)) continue;
        if (m_pMetricas[i][col] > tpmax)
        {
            tpmax = m_pMetricas[i][col];
            imax = i;
        }
        else if (m_pMetricas[i][col] == tpmax)
        {
            // evita coger siempre el mismo usuario cuando hay varios con metrica maxima
        	int p=rand()% 2;
//            if (p < 0.5)
        	 if (p < 1)
                imax = i;
        }
    }
    return imax;
}

// Devuelve el valor de K_col elegido para esa columna, que sera el mayor
// posible sin solapar con columnas ya asignadas.
// Si la columna ya esta asignada devuelve 0
inline int CScheduler::CalculaK_col(int col) const
{
    int K_col = 0;
    for (int i = 0; i < m_iNsubbandas_usuario; i++)
    {
        if (m_pUsuarioSubbanda[col + i] == -1)
            K_col++;
        else
            break;
    }
    return K_col;
}

// devuelve la funcion sum* definida por:
//   sum*(j, k, K_k) = T_{jk} + T_{j,k+1} + ... + T_{j,k + K_k - 1}
inline double CScheduler::Summ(int j, int k, int K_k) const
{
    double summ = 0;
    for (int i = 0; i < K_k; i++)
        summ += m_pMetricas[j][k + i]; // si llega al final no vuelve a empezar por la primera columna porque no serían contiguas
    return summ;
}

// encuentra el usuario con valor maximo de sum* en la columna dada, descontando
// los usuarios de la lista m_pUsuarioSubbanda con alguna asignacion de subbanda.
// La funcion sum* se define como:
//      sum*(j, k, K_col) = T_{jk} + T_{j,k+1} + ... + T_{j,k + K_col - 1}
// Cuando hay varios usuarios con metrica maxima, aleatoriza el elegido.
// Admite exceptuar el usuario "usuario_excepcion" de la busqueda.
// Si todos los usuarios han sido ya asignados devuelve -1
int CScheduler::MaxSummColumna(int col, int K_col, int usuario_excepcion = -1) const
{
    int i;

    // encuentra argmax de sum*(j, col, K_col)
    double tpmax = -1;
    int imax = -1;
    double tp;
    for (i = 0; i < m_iNUsuarios; i++)
    {
        if ((i == usuario_excepcion) || EsUsuarioAsignado(i, 1)) continue; // Evaluamos es UsuarioAsignado en una columna porque queremos saber si ese usuario ha sido ya asignado en otra subbanda, para lo cual tendrá ya (muy probablemente) K_col asignaciones realizadas
        if ((tp = Summ(i, col, K_col)) > tpmax)
        {
            tpmax = tp;
            imax = i;
        }
        else if (tp == tpmax)
        {
            // evita coger siempre el mismo usuario cuando hay varios con metrica maxima
            if (rand() < 0.5)
                imax = i;
        }
    }
    return imax;
}

// asigna al usuario las subbandas: subbanda, subbanda + 1, ..., subbanda + nsubbandas - 1
inline void CScheduler::AsignaSubbandas(int subbanda, int nsubbandas, int usuario)
{
    for (int i = 0; i < nsubbandas; i++)
        m_pUsuarioSubbanda[subbanda + i] = usuario; // Son subbandas contiguas, la asignación para UL
}


// realiza las asignaciones de usuario y actualiza el throughput global y de subframe
// caso sin informacion de subbandas: solo se asigna el usuario user
void CScheduler::ScheduleUsers_nofreq(int user, int subframe)
{
    m_dThroughput_subframe = m_pUsuarios[user]->GetThroughputWideband();
    m_dThroughput_global += m_dThroughput_subframe;

    for (int j = 0; j < m_iNUsuarios; j++)
    {
        if (j == user)
            m_pUsuarios[j]->ScheduleUser(true, 0, NULL);
        else
            m_pUsuarios[j]->NoScheduleUser(true);

        // guarda throughput de usuarios
        if (subframe < m_iNSubframes_max) 
            m_pThroughputUsuarios[j][subframe] = m_pUsuarios[j]->GetLastThroughput(); // Obtenemos el tp de cada usuario-subframe (tp medio de todas las subbandas)
    }

    if (m_iDebug)
        fprintf(m_pFichLog, "\tSubframe %d: user %d, throughput %.1f\n", subframe, user, m_dThroughput_subframe);
}

// realiza las asignaciones de usuario y actualiza el throughput global y de subframe
// caso con asignacion de subbandas
void CScheduler::ScheduleUsers_freq(int subframe)
{
    int j, k;

    m_dThroughput_subframe = 0;

    if (m_iDebug)
        fprintf(m_pFichLog, "\nSubframe %d:\n", subframe);

    for (j = 0; j < m_iNUsuarios; j++)
    {
        memset(m_pSubbandasUsuario, 0, m_iNsubbandas * sizeof(int));
        int Nsubbandas_asign = 0;
        for (k = 0; k < m_iNsubbandas; k++)
        {
            if (m_pUsuarioSubbanda[k] == j)
                m_pSubbandasUsuario[Nsubbandas_asign++] = k; // Nsubbandas_asign++ -> post-incremento
        } // Coloca correlativamente las subbandas que han sido asignadas para el usuario considerado j. Cada vez que cambia de usuario se machaca la asignación realizada anterior
        if (Nsubbandas_asign)
            m_pUsuarios[j]->ScheduleUser(false, Nsubbandas_asign, m_pSubbandasUsuario);
        else
            m_pUsuarios[j]->NoScheduleUser(false);

        m_dThroughput_subframe += m_pUsuarios[j]->GetLastThroughput();

        // guarda throughput de usuarios
        if (subframe < m_iNSubframes_max)
            m_pThroughputUsuarios[j][subframe] = m_pUsuarios[j]->GetLastThroughput();

        // muestra informacion de subbandas asignadas
        if (m_iDebug)
            fprintf(m_pFichLog, "\tUser %d: %d subbands assigned", j, Nsubbandas_asign);
        if (Nsubbandas_asign)
        {
            if (m_iDebug)
                fprintf(m_pFichLog, " (%d", m_pSubbandasUsuario[0]);
            for (k = 1; k < Nsubbandas_asign; k++)
            {
                if (m_iDebug)
                    fprintf(m_pFichLog, ",%d", m_pSubbandasUsuario[k]);
            }
            if (m_iDebug)
                fprintf(m_pFichLog, ")");
        }
        if (m_iDebug)
            fprintf(m_pFichLog, ", throughput %.1f\n", m_pUsuarios[j]->GetLastThroughput()); // throughput usuario
    }

    m_dThroughput_global += m_dThroughput_subframe;

    if (m_iDebug)
        fprintf(m_pFichLog, "Throughput subframe %d: %.1f\n", subframe, m_dThroughput_subframe);
}

// Calcula los resultados finales y los escribe en los ficheros de log y usuarios
void CScheduler::EscribeFicherosLogUsuarios()
{
    int i, j;
//    double x, y;
    double *ganancias_subbanda_aux = NULL;
    const char *str;
//
    //// fichero de log

    // -- throughput medio de celda
    switch (m_iUnits_curve)
    {
        case BPS:
            str = "bps";
            break;
        case KBPS:
            str = "Kbps";
            break;
        case MBPS:
            str = "Mbps";
            break;
        case GBPS:
            str = "Gbps";
            break;
    }
    printf("\nAverage cell throughput: %.2f %s\n\n", m_dThroughput_global, str);
    std::cout << "Average cell throughput: " << m_dThroughput_global << " " << str << std::endl;
    fprintf(m_pFichLog, "\nAverage cell throughput: %.2f %s\n\n", m_dThroughput_global, str);

    // -- throughput medio de cada usuario, numero de subframes asignadas a cada usuario e
    //    incremento de throughput con respecto a RR
    int nsubframes = (m_iNsubframes_tot > m_iNSubframes_max)? m_iNSubframes_max : m_iNsubframes_tot;
    int nusuarios_tpnulo = 0;
    m_dMejora_media = 0;
    for (j = 0; j < m_iNUsuarios; j++)
    {
        m_pThroughput_medio[j] = 0;
        m_pNsubframes_asign[j] = 0;
        for (int subframe = 0; subframe < nsubframes; subframe++)
        {
            if (m_pThroughputUsuarios[j][subframe])
            {
                m_pThroughput_medio[j] += m_pThroughputUsuarios[j][subframe];
                (m_pNsubframes_asign[j])++;
            }
        }
        m_pThroughput_medio[j] /= nsubframes;
        m_pThroughput_RR[j] = m_pUsuarios[j]->GetThroughputWideband() / m_iNUsuarios;
        if (m_pThroughput_RR[j])
        {
            m_dMejora_media += (m_pThroughput_medio[j]/m_pThroughput_RR[j] - 1); // Porqué -1?
            printf("User %d average throughput: %.2f --> %.1f %% better than RR (%d subframes)\n", j, m_pThroughput_medio[j],
                    (m_pThroughput_medio[j]/m_pThroughput_RR[j] - 1)*100, m_pNsubframes_asign[j]);
            std::cout << "User " << j << " average throughput: " << m_pThroughput_medio[j] << " ---> " << (m_pThroughput_medio[j]/m_pThroughput_RR[j] - 1)*100 << "% better than RR (" << m_pNsubframes_asign[j] << std::endl;
            fprintf(m_pFichLog, "User %d average throughput: %.2f --> %.1f %% better than RR (%d subframes)\n", j, m_pThroughput_medio[j],
                    (m_pThroughput_medio[j]/m_pThroughput_RR[j] - 1)*100, m_pNsubframes_asign[j]);
        }
        else
        {
            // si un usuario tiene throughput wideband nulo (y por tanto resultante), se descuenta del promedio
            printf("User %d average throughput: 0 (%d subframes)\n", j, m_pNsubframes_asign[j]);
            std::cout << "User " << j << "average throughput: 0 (" << m_pNsubframes_asign[j] << ")";
            fprintf(m_pFichLog, "User %d average throughput: 0 (%d subframes)\n", j, m_pNsubframes_asign[j]);
            nusuarios_tpnulo++;
        }
    }
    m_dMejora_media /= (m_iNUsuarios - nusuarios_tpnulo); // No consideramos en la mejora los usuarios que tienen tp nulo
    printf("Average per-user throughput increase: %.1f %%", m_dMejora_media * 100);
    std::cout << "Average per-user throughput increase: " << m_dMejora_media * 100 << std::endl;
    fprintf(m_pFichLog, "Average per-user throughput increase: %.1f %%", m_dMejora_media * 100);


    //// fichero de usuarios: situacion inicial de los usuarios ( sinr, throughput wideband, ganancias de subbanda)
    //   y final ( subbandas asociadas, throughput promedio final)
    try
    {
        ganancias_subbanda_aux = new double[m_iNsubbandas];
    }
    catch(...)
    {
        CSchedException excepcion(ERROR_MEMORIA);
        throw(excepcion);
    }

    for (i = 0; i < m_iNUsuarios; i++)
    {
//        m_pUsuarios[i]->GetXY(&x, &y);
        m_pUsuarios[i]->GetGanancias(ganancias_subbanda_aux);
        fprintf(m_pFichUsers, "%d\t%d\t%.2f\t%.1f\t%.2f", m_idSector, i,
                m_pUsuarios[i]->GetSINR(), m_pUsuarios[i]->GetThroughputWideband(), ganancias_subbanda_aux[0]);
        for (j = 1; j < m_iNsubbandas; j++)
        {
            fprintf(m_pFichUsers, "\t%.2f", ganancias_subbanda_aux[j]);
        }

//        fprintf(m_pFichUsers, "\t0\t0\t0");

        if (m_pThroughput_RR[i])
            fprintf(m_pFichUsers, "\t%.1f\t%.2f\t%d\n", m_pThroughput_medio[i], m_pThroughput_medio[i] / m_pThroughput_RR[i], m_pNsubframes_asign[i]);
        else
            fprintf(m_pFichUsers, "\t%.1f\t%.2f\t%d\n", m_pThroughput_medio[i], m_dNo_data, m_pNsubframes_asign[i]);
    }

    delete []ganancias_subbanda_aux;
}

// punto de entrada principal de ejecucion
void CScheduler::RunScheduler(int Nsubframes_tot)
{
	std::cout << "Dentro de CSchdeuler -> RunScheduler" << std::endl;
    switch (m_iScheduler)
    {
        case ROUND_ROBIN:
        case MAXIMUM_RATE:
        case PROPORTIONAL_FAIR:
        case PROPORTIONAL_FAIR_RES:
            RunScheduler_nofreq(Nsubframes_tot);
            break;

        case FREQ_MAXIMUM_RATE:
        case FREQ_PROPORTIONAL_FAIR:
        case FREQ_PROPORTIONAL_FAIR_RES:
            if (m_iULDL == UL)
                RunScheduler_freq_UL(Nsubframes_tot);
            else
            	std::cout << "Lanzamos scheduler de DL" << std::endl;
                RunScheduler_freq_DL(Nsubframes_tot);
            break;
    }

    EscribeFicherosLogUsuarios();

    CalculaMejoraUsuario();
}


// Scheduler en tiempo, sin informacion de banda ancha:
// ROUND_ROBIN, MAXIMUM_RATE, PROPORTIONAL_FAIR, PROPORTIONAL_FAIR_RES
void CScheduler::RunScheduler_nofreq(int Nsubframes_tot)
{
    int j, user;
    int subframe;

    m_iNsubframes_tot = Nsubframes_tot;

    m_dThroughput_global = 0;

    for (subframe = 0; subframe < m_iNsubframes_tot; subframe++)
    {

        if (m_iScheduler == ROUND_ROBIN)
        {
            //// elige el usuario por orden consecutivo

            user = (subframe % m_iNUsuarios); // Resto de la división
        }
        else
        {
            //// construye la tabla de metricas: valores de throughput relativo a su valor medio acumulado

            switch(m_iScheduler) // Construye solo las métricas para una subbanda porque no hay información del resto de subbandas (caso wideband)
            {
        	    // Añadimos estos case para que compile
            	case FREQ_MAXIMUM_RATE:
                case FREQ_PROPORTIONAL_FAIR:
                case FREQ_PROPORTIONAL_FAIR_RES:
                case ROUND_ROBIN:

                case MAXIMUM_RATE:
                    for (j = 0; j < m_iNUsuarios; j++)
                        m_pMetricas[j][0] = m_pUsuarios[j]->GetThroughputWideband();
                    break;

                case PROPORTIONAL_FAIR:
                    for (j = 0; j < m_iNUsuarios; j++)
                        m_pMetricas[j][0] = m_pUsuarios[j]->GetRelativeThroughputWideband();
                    break;

                case PROPORTIONAL_FAIR_RES:
                    for (j = 0; j < m_iNUsuarios; j++)
                        m_pMetricas[j][0] = m_pUsuarios[j]->GetResourceRelativeThroughputWideband();
                    break;
            }

            //// elige el usuario con mayor metrica m_pMetricas[j][0]
            //// cuando hay varios aleatoriza el usuario elegido

            double max_metrica = -1;
            user = -1;
            for (j = 0; j < m_iNUsuarios; j++)
            {
                if (m_pMetricas[j][0] > max_metrica)
                {
                    max_metrica = m_pMetricas[j][0];
                    user = j;
                }
                else if (m_pMetricas[j][0] == max_metrica)
                {
                    // evita coger siempre el mismo usuario cuando hay varios con metrica maxima
                    if (rand() < 0.5)
                        user = j;
                }
            }
        }

        //// Scheduling de usuarios asignados

        ScheduleUsers_nofreq(user, subframe);

        //// Progreso
        if ((100*subframe)%m_iNsubframes_tot == 0)
        {
            printf("\rScheduler in progress... %d %% complete", (100*subframe)/m_iNsubframes_tot);
            fflush(stdout);
            std::cout << "\rScheduler in progress..." << (100*subframe)/m_iNsubframes_tot << "% complete";
        }
    }

    printf("\rScheduler in progress... 100 %% complete\n");
    std::cout << "\rScheduler in progress...100 %% complete\n" << std::endl;

    m_dThroughput_global /= m_iNsubframes_tot;

}

// Scheduler en tiempo-frecuencia para DL, aprovechando la informacion de banda ancha:
// FREQ_MAXIMUM_RATE, FREQ_PROPORTIONAL_FAIR, FREQ_PROPORTIONAL_FAIR_RES
void CScheduler::RunScheduler_freq_DL(int Nsubframes_tot)
{
    int j, k, l;
    int subframe;
    int nsubbandas_asign;

    m_iNsubframes_tot = Nsubframes_tot;

    m_dThroughput_global = 0;

    std::cout << "Dentro de RunScheduler_freq_DL" << std::endl;

    for (subframe = 0; subframe < m_iNsubframes_tot; subframe++)
    {
    	std::cout << "Dentro de for Nsubframes" << std::endl;
        //// construye la tabla de metricas: valores de throughput relativo a su valor medio acumulado

        switch(m_iScheduler)
        {
        	// Añadimos estos case para que compile
        	case MAXIMUM_RATE:
            case PROPORTIONAL_FAIR:
            case PROPORTIONAL_FAIR_RES:
            case ROUND_ROBIN:

            case FREQ_MAXIMUM_RATE:
                for (j = 0; j < m_iNUsuarios; j++)
//                    m_pUsuarios[j]->GetThroughputSubbandas(m_pMetricas[j]);
                	m_pUsuarios[j]->GetThroughputSubbandas(GetValorUsuario(m_pMetricas, j, m_iNsubbandas));
                break;

            case FREQ_PROPORTIONAL_FAIR:
                for (j = 0; j < m_iNUsuarios; j++)
//                    m_pUsuarios[j]->GetRelativeThroughputSubbandas(m_pMetricas[j]); // Obtenemos las métricas de cada usuario
                	m_pUsuarios[j]->GetRelativeThroughputSubbandas(GetValorUsuario(m_pMetricas, j, m_iNsubbandas));
                break;

            case FREQ_PROPORTIONAL_FAIR_RES:
                for (j = 0; j < m_iNUsuarios; j++)
//                    m_pUsuarios[j]->GetResourceRelativeThroughputSubbandas(m_pMetricas[j]);
                	m_pUsuarios[j]->GetResourceRelativeThroughputSubbandas(GetValorUsuario(m_pMetricas, j, m_iNsubbandas));
                break;
        }

//        SetMetricas(0,190,100,20,88,20,72,130,120);
//        SetMetricas(1,600, 10, 60, 24, 40, 210, 110, 10);
//        SetMetricas(2,20, 180, 310, 400, 180, 40, 220, 305);
//        SetMetricas(3,10, 200, 20, 500, 410, 160,	130, 80);
//        SetMetricas(4,100, 30, 20, 299, 15, 40, 50, 250);
//        SetMetricas(5,310, 150, 330, 10, 60, 10, 500, 800);
//        SetMetricas(6,160, 10, 20, 10, 245, 5, 90, 200);
//        SetMetricas(7,30,	9, 310,	301, 160, 110, 550, 10);
//
//        std::cout<< "Primera metrica "<< m_pMetricas[0][0] << std::endl;
//        std::cout<< "Ultima metrica "<< m_pMetricas[7][7] << std::endl;

        //// Bucle de asignaciones de subbandas

        for (k = 0; k < m_iNsubbandas; k++)
            m_pUsuarioSubbanda[k] = -1;
        nsubbandas_asign = 0;
//        k=-1;
        while (nsubbandas_asign < m_iNsubbandas)
        {
            // elige la proxima subbanda aleatoriamente
//            k = (int)floor(genrand() * m_iNsubbandas);
        	k = rand() % m_iNsubbandas;
//        	k++;
            if (k == m_iNsubbandas || m_pUsuarioSubbanda[k] != -1) continue;  // subbanda ya asignada, porqué saltas si k es la última banda??

            j = MaxColumna(k);		// MaxColumna devolverá -1 cuando todos los usuarios sobre los que se realiza la búsqueda están asignados
            if (j == -1) break;         // ya han sido asignados todos los usuarios
            double Tjk = m_pMetricas[j][k];
            bool OtraSubbandaMejor = false;

            // vemos si el usuario j puede ser mejor candidato para otra subbanda
            for (l = 0; l < m_iNsubbandas; l++)
            {
                if (l == k) continue;
                double Tjl = m_pMetricas[j][l];
                if ( (m_pUsuarioSubbanda[l] == -1) &&
                     (j == MaxColumna(l)) &&
                     (Tjl > Tjk) )
                {
                    // buscamos los segundos maximos en las columnas k y l
                    int j1 = MaxColumna(k, j);
                    int j2 = MaxColumna(l, j);
                    if (j1 == -1 || j2 == -1) break;    // el usuario j es el unico que queda sin asignar
                    double Tj1k = m_pMetricas[j1][k];
                    double Tj2l = m_pMetricas[j2][l];
                    if (Tj1k + Tjl > Tjk + Tj2l)
                    {
                        // asignamos el usuario j1 a la subbanda k, y el usuario j a la subbanda l
                        m_pUsuarioSubbanda[k] = j1;
                        m_pUsuarioSubbanda[l] = j;
                    }
                    else
                    {
                        // asignamos el usuario j a la subbanda k, y el usuario j2 a la subbanda l
                        m_pUsuarioSubbanda[k] = j;
                        m_pUsuarioSubbanda[l] = j2;
                    }
                    nsubbandas_asign += 2; // Almacena el número de asignaciones realizadas para todas las subbandas
                    OtraSubbandaMejor = true; // Otra subbanda mejor?? en realidad es otra subanda que también da un máximo en el usuario
                    break;
                }
            }
            if (!OtraSubbandaMejor) // no hay otra subbanda en la que el usuario j sea mejor candidato
            {
                // asignamos el usuario j a la subbanda k
                m_pUsuarioSubbanda[k] = j;
                nsubbandas_asign++;
            }
        }

        //// Scheduling de usuarios asignados

        ScheduleUsers_freq(subframe);

        //// Progreso
        if ((100*subframe)%m_iNsubframes_tot == 0)
        {
            printf("\rScheduler in progress... %d %% complete", (100*subframe)/m_iNsubframes_tot);
            fflush(stdout);
            std::cout << "Scheduler in progress..." << (100*subframe)/m_iNsubframes_tot << "% complete" << std::endl;

        }
    }

    for (int i=0; i<m_iNsubbandas; i++)
    	printf("Usuario-subbanda %d\n", m_pUsuarioSubbanda[i]);

    printf("\rScheduler in progress... 100 %% complete\n");
//    std::cout << "Scheduler in progress...100 %% complete" << std::endl;
    m_dThroughput_global /= m_iNsubframes_tot;

}


// Scheduler en tiempo-frecuencia para UL, aprovechando la informacion de banda ancha:
// FREQ_MAXIMUM_RATE, FREQ_PROPORTIONAL_FAIR, FREQ_PROPORTIONAL_FAIR_RES
void CScheduler::RunScheduler_freq_UL(int Nsubframes_tot)
{
    int j, k, l;
    int subframe;
    int nsubbandas_asign;
    int Kk, Kl;

    m_iNsubframes_tot = Nsubframes_tot;

    m_dThroughput_global = 0;

    for (subframe = 0; subframe < m_iNsubframes_tot; subframe++)
    {
        //// construye la tabla de metricas: valores de throughput relativo a su valor medio acumulado

        switch(m_iScheduler)
        {
			// Añadimos estos case para que compile
			case MAXIMUM_RATE:
			case PROPORTIONAL_FAIR:
			case PROPORTIONAL_FAIR_RES:
            case ROUND_ROBIN:

            case FREQ_MAXIMUM_RATE:
                for (j = 0; j < m_iNUsuarios; j++)
                    m_pUsuarios[j]->GetThroughputSubbandas(m_pMetricas[j]);
                break;

            case FREQ_PROPORTIONAL_FAIR:
                for (j = 0; j < m_iNUsuarios; j++)
                    m_pUsuarios[j]->GetRelativeThroughputSubbandas(m_pMetricas[j]);
                break;

            case FREQ_PROPORTIONAL_FAIR_RES:
                for (j = 0; j < m_iNUsuarios; j++)
                    m_pUsuarios[j]->GetResourceRelativeThroughputSubbandas(m_pMetricas[j]);
                break;
        }

        //// Bucle de asignaciones de subbandas

        for (k = 0; k < m_iNsubbandas; k++)
            m_pUsuarioSubbanda[k] = -1;
        nsubbandas_asign = 0;
        while (nsubbandas_asign < m_iNsubbandas)
        {
            // elige la proxima subbanda aleatoriamente
//            k = (int)floor(genrand() * m_iNsubbandas);
        	k = rand() % m_iNsubbandas ; // genera un número aleatorio entre 0 y Numsectores
            if (k == m_iNsubbandas || m_pUsuarioSubbanda[k] != -1) continue;  // subbanda ya asignada

            Kk = CalculaK_col(k); // Devuelve el número de columnas a considerar como contiguas para evaluar la suma de métricas y obtener el usuario que máximiza esa suma --> Porqué es variable?? debería de ser un valor fijo para todos los usuarios
            j = MaxSummColumna(k, Kk); // Devuelve el usuario que maximiza la suma de métricas desde la columna k hasta la columna máxima kk
            if (j == -1) break;     // ya han sido asignados todos los usuarios
            double sumjk = Summ(j, k, Kk);
            bool OtraSubbandaMejor = false;

            // vemos si el usuario j puede ser mejor candidato para otra subbanda
            for (l = 0; l < m_iNsubbandas; l++)
            {
                if ((l >= k) && (l < k + Kk)) continue; // NO considera las bandas comprendidas en el rango k .. k+Kk-1
                Kl = CalculaK_col(l);
                double sumjl = Summ(j, l, Kl);
                if ( (Kl != 0) &&
                     (j == MaxSummColumna(l, Kl)) &&
                     (sumjl > sumjk) )
                {
                    // buscamos los segundos maximos en las columnas k..k+Kk-1 y l..l+Ll-1
                    int j1 = MaxSummColumna(k, Kk, j);
                    int j2 = MaxSummColumna(l, Kl, j);
                    if (j1 == -1 || j2 == -1) break;    // el usuario j es el unico que queda sin asignar
                    double sumj1k = Summ(j1, k, Kk);
                    double sumj2l = Summ(j2, l, Kl);
                    if (sumj1k + sumjl > sumjk + sumj2l)
                    {
                        // asignamos el usuario j1 a las subbandas k..k+Kk-1, y el usuario j a las subbandas l..l+Kl-1
                        AsignaSubbandas(k, Kk, j1);
                        AsignaSubbandas(l, Kl, j);
                    }
                    else
                    {
                        // asignamos el usuario j a las subbandas k..k+Kk-1, y el usuario j2 a las subbandas l..l+Kl-1
                        AsignaSubbandas(k, Kk, j);
                        AsignaSubbandas(l, Kl, j2);
                    }
                    nsubbandas_asign += (Kk + Kl);
                    OtraSubbandaMejor = true;
                    break;
                }
            }
            if (!OtraSubbandaMejor)
            {
                // asignamos el usuario j a las subbandas k..k+Kk-1
                AsignaSubbandas(k, Kk, j);
                nsubbandas_asign += Kk;
            }
        }

        //// Scheduling de usuarios asignados

        ScheduleUsers_freq(subframe);

        //// Progreso
        if ((100*subframe)%m_iNsubframes_tot == 0)
        {
            printf("\rScheduler in progress... %d %% complete", (100*subframe)/m_iNsubframes_tot);
            fflush(stdout);
        }
    }

    printf("\rScheduler in progress... 100 %% complete\n");

    m_dThroughput_global /= m_iNsubframes_tot;

}


// calcula la ganancia de scheduling de usuario en funcion de la sinr, definida como:
// m_pThroughput_medio[i] / m_pThroughput_RR[i]
void CScheduler::CalculaMejoraUsuario()
{
    int i;

    memset(m_pMejoraUsuarioVsSinr, 0, m_iNumIntervalosMejoraUsuario * sizeof(double));
    memset(m_pNumOcurrenciasMejora_Sinr, 0, m_iNumIntervalosMejoraUsuario * sizeof(int));

    for (i = 0; i < m_iNUsuarios; i++)
    {
        double mejora = (m_pThroughput_RR[i])? m_pThroughput_medio[i] / m_pThroughput_RR[i] : m_dNo_data;
        double sinr = m_pUsuarios[i]->GetSINR();
        int indice = (int)( (sinr - m_dSinrMejoraUsuario_min) / m_dDeltasinr_mejora );
        // truncamos los valores de sinr a [m_dSinrMejoraUsuario_min, m_dSinrMejoraUsuario_max)
        if (indice < 0)
            indice = 0;
        else if (indice > m_iNumIntervalosMejoraUsuario - 1)
            indice = m_iNumIntervalosMejoraUsuario - 1;
        if (mejora != m_dNo_data)
        {
            m_pMejoraUsuarioVsSinr[indice] += mejora;
            (m_pNumOcurrenciasMejora_Sinr[indice])++;
        }
    }

    for (i = 0; i < m_iNumIntervalosMejoraUsuario; i++)
    {
        if (m_pNumOcurrenciasMejora_Sinr[i])
            m_pMejoraUsuarioVsSinr[i] /= m_pNumOcurrenciasMejora_Sinr[i];
    }
}

// devuelve el histograma de mejora
void CScheduler::GetMejoraUsuario(double* mejora) const
{
    if (mejora != NULL)
        memcpy(mejora, m_pMejoraUsuarioVsSinr, m_iNumIntervalosMejoraUsuario * sizeof(double));
}




// devuelve el throughput de celda
double CScheduler::GetCellThroughput() const
{
    return m_dThroughput_global;
}

