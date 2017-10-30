/* 
 * File:   CUsuario.cpp
 * Author: jlh
 * 
 * Created on May 25, 2010, 5:37 PM
 */

#include <stdlib.h>
#include <memory.h>
#include <float.h>
#include <math.h>
#include "complejo.h"

#include "CUsuario.h"
#include "CSchedException.h"


#include <iostream>

using namespace std;

// maximo numero de subframes admitido para cada usuario
const int CUsuario::m_iNsubframes_max = 100000;
// throughput ficticio inicial para poder calcular throughput relativos
const double CUsuario::m_dThroughput_inicial = 100;
// numero de subbandas ficticio inicial para poder calcular throughput relativos a recursos
const int CUsuario::m_iResources_inicial = 1;


//CUsuario::CUsuario(int Nsubbandas = 8, int Nsubbandas_CQI = 8, int Nsubframes_promedia = 10, int truncCQI = 0, int NRB_DL = 100)
CUsuario::CUsuario(int Nsubbandas, int Nsubbandas_CQI, int Nsubframes_promedia, int truncCQI, int NRB_DL)
{
    m_iNsubbandas_tot = Nsubbandas;
    m_iNsubbandas_CQI = Nsubbandas_CQI;
    m_iNsubframes_promedia = Nsubframes_promedia;
    m_itruncCQI = truncCQI;
    m_iNRB_DL = NRB_DL;
    m_dSINR = 0;
    m_pGanancias = NULL;
    m_pGanancias_ord = NULL;
    m_pGanancias_trunc = NULL;
    m_pThroughput_subbanda = NULL;
    m_pThroughput_subbanda_trunc = NULL;
    m_pVentanaThroughput = NULL;
    m_pVentanaResources = NULL;
    m_iPosVentana = 0;
    m_iVentanaLlena = false;
    m_dThroughput_medio = 0;
    m_dThroughput_wideband = 0;
    m_pRelative_throughput = NULL;
    m_pRelative_throughput_trunc = NULL;
    m_dRelativeThroughput_wideband = 0;
    m_dResources_medio = 0;
    m_pResourceRelative_throughput = NULL;
    m_pResourceRelative_throughput_trunc = NULL;
    m_dResourceRelativeThroughput_wideband = 0;
    m_iNsubframes = 0; // número de subframes transcurridas
    //m_pBestSubbandas = NULL;
    m_iSubbandSize = 0;

    // calcula tamano de subbanda en RBs
    m_iSubbandSize = m_iNRB_DL / m_iNsubbandas_tot;

    ReservaMemoria();
}

CUsuario::~CUsuario() // Destructor
{
    delete []m_pGanancias;
    delete []m_pGanancias_ord;
    delete []m_pGanancias_trunc;
    delete []m_pThroughput_subbanda;
    delete []m_pThroughput_subbanda_trunc;
    delete []m_pVentanaThroughput;
    delete []m_pVentanaResources;
    delete []m_pRelative_throughput;
    delete []m_pRelative_throughput_trunc;
    delete []m_pResourceRelative_throughput;
    delete []m_pResourceRelative_throughput_trunc;
    //delete []m_pBestSubbandas;
}


void CUsuario::ReservaMemoria()
{
//    int i;

    // reserva memoria
    try
    {
        m_pGanancias = new double[m_iNsubbandas_tot];
        m_pGanancias_ord = new double[m_iNsubbandas_tot];
        m_pGanancias_trunc = new double[m_iNsubbandas_tot];
        m_pThroughput_subbanda = new double[m_iNsubbandas_tot];
        m_pThroughput_subbanda_trunc = new double[m_iNsubbandas_tot];
        m_pVentanaThroughput = new double[m_iNsubframes_promedia];
        m_pVentanaResources = new int[m_iNsubframes_promedia];
        m_pRelative_throughput = new double[m_iNsubbandas_tot];
        m_pRelative_throughput_trunc = new double[m_iNsubbandas_tot];
        m_pResourceRelative_throughput = new double[m_iNsubbandas_tot];
        m_pResourceRelative_throughput_trunc = new double[m_iNsubbandas_tot];
//        m_pBestSubbandas = new int[m_iNsubbandas_CQI];
    }
    catch(...)
    {
        CSchedException excepcion(ERROR_MEMORIA);
        std::cout << "exception occured in Scheduler"<< std::endl;
        throw(excepcion);
    }

    // inicializa
    memset(m_pGanancias, 0, m_iNsubbandas_tot * sizeof(double));
    memset(m_pGanancias_ord, 0, m_iNsubbandas_tot * sizeof(double));
    memset(m_pGanancias_trunc, 0, m_iNsubbandas_tot * sizeof(double));
    memset(m_pThroughput_subbanda, 0, m_iNsubbandas_tot * sizeof(double));
    memset(m_pThroughput_subbanda_trunc, 0, m_iNsubbandas_tot * sizeof(double));
    memset(m_pVentanaThroughput, 0, m_iNsubframes_promedia * sizeof(double));
    memset(m_pVentanaResources, 0, m_iNsubframes_promedia * sizeof(int));
    memset(m_pRelative_throughput, 0, m_iNsubbandas_tot * sizeof(double));
    memset(m_pRelative_throughput_trunc, 0, m_iNsubbandas_tot * sizeof(double));
    memset(m_pResourceRelative_throughput, 0, m_iNsubbandas_tot * sizeof(double));
    memset(m_pResourceRelative_throughput_trunc, 0, m_iNsubbandas_tot * sizeof(double));
    //memset(m_pBestSubbandas, 0, m_iNsubbandas_CQI * sizeof(int));
    m_dThroughput_medio = m_dThroughput_inicial;        // comienza con un throughput medio inicial (ficticio)
    m_dResources_medio = (double)m_iResources_inicial;
}

//// funcion de comparacion para qsort()
//int compara_double_qsort(const void* a, const void* b)
//{
//    if (*(double *)a < *(double *)b)
//        return -1; // a va delante de b
//    else if (*(double *)a == *(double *)b)
//        return 0;
//    else
//        return 1; // a va después que b
//}
//void CUsuario::SetXY(double x, double y)
//{
//    m_dX = x;
//    m_dY = y;
//    m_iNsubframes = 0;
//}

void CUsuario::SetSINR(double sinr) // valor medio
{
    m_dSINR = sinr;
    m_iNsubframes = 0;
}


// establece las ganancias en m_pGanancias, calcula sus valores truncados en m_pGanancias_trunc
// y en caso UE-selected subband feedback, las mejores subbandas en m_pBestSubbandas.
void CUsuario::SetGanancias(double* ganancias)
{
//    double gan_med = 0;
//    int j;

    if (ganancias != NULL)
    {
        memcpy(m_pGanancias, ganancias, m_iNsubbandas_tot * sizeof(double));

//        // valores truncados
//        if (m_itruncCQI)
//        {
//
//                // higher-layer configured subband feedback: trunca las ganancias a -1,0,1,2
//                for (int j = 0; j < m_iNsubbandas_tot; j++)
//                {
//                    if (m_pGanancias[j] < -0.5)
//                        m_pGanancias_trunc[j] = -1;
//                    else if (m_pGanancias[j] < 0.5)
//                        m_pGanancias_trunc[j] = 0;
//                    else if (m_pGanancias[j] < 1.5)
//                        m_pGanancias_trunc[j] = 1;
//                    else
//                        m_pGanancias_trunc[j] = 2;
//                }
//
//        }

    }
    m_iNsubframes = 0; // no ha transcurrido ninguna subframe
}



// asigna los valores de throughput de subbanda sin truncar
void CUsuario::SetThroughputSubbandas(double* throughput)
{
    if (throughput != NULL)
        memcpy(m_pThroughput_subbanda, throughput, m_iNsubbandas_tot * sizeof(double));
    m_iNsubframes = 0;

    // inicializa valores relativos de throughput
    for (int i = 0; i < m_iNsubbandas_tot; i++)
    {
        m_pRelative_throughput[i] = m_pThroughput_subbanda[i] / m_dThroughput_inicial;
        m_pResourceRelative_throughput[i] = m_pThroughput_subbanda[i] / m_iResources_inicial;
    }
}

// asigna el throughput wideband
void CUsuario::SetThroughputWideband(double throughput_wideband)
{
    m_dThroughput_wideband = throughput_wideband;
}

// asigna los valores de throughput de subbanda truncados
//void CUsuario::SetThroughputSubbandasTrunc(double* throughput_trunc)
//{
//    if (throughput_trunc != NULL)
//        memcpy(m_pThroughput_subbanda_trunc, throughput_trunc, m_iNsubbandas_tot * sizeof(double));
//    m_iNsubframes = 0;
//
//    // inicializa valores relativos de throughput
//    for (int i = 0; i < m_iNsubbandas_tot; i++)
//    {
//        m_pRelative_throughput_trunc[i] = m_pThroughput_subbanda_trunc[i] / m_dThroughput_inicial;
//        m_pResourceRelative_throughput_trunc[i] = m_pThroughput_subbanda_trunc[i] / m_iResources_inicial;
//    }
//}

int CUsuario::GetNSubbandas()
{
    return m_iNsubbandas_tot;
}

//void CUsuario::GetXY(double* x, double* y)
//{
//    if (x != NULL && y != NULL)
//    {
//        *x = m_dX;
//        *y = m_dY;
//    }
//}

double CUsuario::GetSINR()
{
    return m_dSINR;
}

// devuelve las ganancias de subbanda sin truncar
void CUsuario::GetGanancias(double* ganancias)
{
    if (ganancias != NULL)
        memcpy(ganancias, m_pGanancias, m_iNsubbandas_tot * sizeof(double));
}

//// devuelve las ganancias de subbanda truncadas
//void CUsuario::GetGananciasTrunc(double* ganancias_trunc)
//{
//    if (ganancias_trunc != NULL)
//        memcpy(ganancias_trunc, m_pGanancias_trunc, m_iNsubbandas_tot * sizeof(double));
//}

int CUsuario::GetNSubframesPromedia()
{
    return m_iNsubframes_promedia;
}

// devuelve el throughput medio experimentado por el usuario a lo largo de m_iNsubframes_throughput subframes
double CUsuario::GetThroughputMedio()
{
    return m_dThroughput_medio;
}

// devuelve el throughput de cada subbanda, truncados o sin truncar (dependiendo de m_itruncCQI).
void CUsuario::GetThroughputSubbandas(double* throughput)
{

        if (m_itruncCQI) // throughput truncado
        {
        	if (throughput != NULL)                    // higher-layer configured subband feedback
                memcpy(throughput, m_pThroughput_subbanda_trunc, m_iNsubbandas_tot * sizeof(double));
        }
        else
        {
            if (throughput != NULL)
                memcpy(throughput, m_pThroughput_subbanda, m_iNsubbandas_tot * sizeof(double));
        }
 
}

// devuelve los valores de throughput de subbanda relativos al throughput medio experimentado, truncados o sin truncar
void CUsuario::GetRelativeThroughputSubbandas(double* relative_throughput)
{
        if (m_itruncCQI)
        {
            if (relative_throughput != NULL)
                memcpy(relative_throughput, m_pRelative_throughput_trunc, m_iNsubbandas_tot * sizeof(double));
        }
        else
        {
           if (relative_throughput != NULL)
                memcpy(relative_throughput, m_pRelative_throughput, m_iNsubbandas_tot * sizeof(double));
        }

}

// devuelve los valores de throughput de subbanda relativos a los recursos medios
void CUsuario::GetResourceRelativeThroughputSubbandas(double* resource_relative_throughput)
{
        if (m_itruncCQI)
        {
            if (resource_relative_throughput != NULL)
                memcpy(resource_relative_throughput, m_pResourceRelative_throughput_trunc, m_iNsubbandas_tot * sizeof(double));
        }
        else
        {
            if (resource_relative_throughput != NULL)
                memcpy(resource_relative_throughput, m_pResourceRelative_throughput, m_iNsubbandas_tot * sizeof(double));
        }

}

// devuelve el ultimo throughput experimentado por el usuario
double CUsuario::GetLastThroughput()
{
    if (m_iPosVentana == 0)
        return m_pVentanaThroughput[m_iNsubframes_promedia - 1];
    else
        return m_pVentanaThroughput[m_iPosVentana - 1];
}

double CUsuario::GetThroughputWideband()
{
    return m_dThroughput_wideband;
}

double CUsuario::GetRelativeThroughputWideband()
{
    return m_dRelativeThroughput_wideband;
}

double CUsuario::GetResourceRelativeThroughputWideband()
{
    return m_dResourceRelativeThroughput_wideband;
}


// actualiza throughput medio, numero medio de recursos y valores de throughput relativo.
void CUsuario::ActualizaThroughput(bool wideband)
{
    int i;

//    // actualiza throughput medio y numero medio de recursos con filtro autorregresivo:
//    //   T[n] = T[n-1] * (1-1/t_a) + 1/t_a * R[n]
//    // (Sorensen y Pons, "Performance Evaluation of Proportional Fair Scheduling Algorithm with Measured Channels")
//    m_dThroughput_medio = (1 - (double)1 / m_iNsubframes_promedia) * m_dThroughput_medio +
//                          (double)1 / m_iNsubframes_promedia * m_pVentanaThroughput[m_iPosVentana];
//    m_dResources_medio = (1 - (double)1 / m_iNsubframes_promedia) * m_dResources_medio +
//                         (double)1 / m_iNsubframes_promedia * m_pVentanaResources[m_iPosVentana];

    // actualiza posicion de las ventanas
    if (m_iPosVentana == (m_iNsubframes_promedia - 1)) // ha llegado a la última posición de la ventana de Tp --> ventana llena
    {
        if (!m_iVentanaLlena) // si el indicador no de llenado no está puesto a true lo ponemos e inicializamos el índice de la posición de la ventana
            m_iVentanaLlena = true;
        m_iPosVentana = 0;
    }
    else
        m_iPosVentana++;

    // actualiza throughput medio y numero medio de recursos
    if (m_iVentanaLlena) // ventana llena --> podemos calcular tp medio de usuario en las "m_iVentanaTp" subframes
    {
        m_dThroughput_medio = 0;
        m_dResources_medio = 0;
        for (i = 0; i < m_iNsubframes_promedia; i++)
        {
            m_dThroughput_medio += m_pVentanaThroughput[i];
            m_dResources_medio += (double)m_pVentanaResources[i];
        }
        m_dThroughput_medio /= m_iNsubframes_promedia;
        m_dResources_medio /= m_iNsubframes_promedia;
    }
    else
    {
        m_dThroughput_medio = m_dThroughput_inicial;            // throughput ficticio instante -1
        m_dResources_medio = (double)m_iResources_inicial;      // numero de recursos ficticio instante -1
        for (i = 0; i < m_iPosVentana; i++)
        {
            m_dThroughput_medio += m_pVentanaThroughput[i];
            m_dResources_medio += (double)m_pVentanaResources[i];
        }
        m_dThroughput_medio /= (m_iPosVentana + 1);
        m_dResources_medio /= (m_iPosVentana + 1);
    }

    // actualiza los valores de throughput relativo
    if (wideband)
    {
        m_dRelativeThroughput_wideband = (m_dThroughput_medio == 0)? FLT_MAX : m_dThroughput_wideband / m_dThroughput_medio;
        m_dResourceRelativeThroughput_wideband = (m_dResources_medio == 0)? FLT_MAX : m_dThroughput_wideband / m_dResources_medio;
    }
    else
    {
        for (i = 0; i < m_iNsubbandas_tot; i++)
        {
 
          m_pRelative_throughput[i] = (m_dThroughput_medio == 0)? FLT_MAX : m_pThroughput_subbanda[i] / m_dThroughput_medio;
          m_pResourceRelative_throughput[i] = (m_dResources_medio == 0)? FLT_MAX : m_pThroughput_subbanda[i] / m_dResources_medio;
          if (m_itruncCQI)
          {
            m_pRelative_throughput_trunc[i] = (m_dThroughput_medio == 0)? FLT_MAX : m_pThroughput_subbanda_trunc[i] / m_dThroughput_medio; // El tp medio se calcula para cada usuario para todas las subbandas que tiene asignadas
            m_pResourceRelative_throughput_trunc[i] = (m_dResources_medio == 0)? FLT_MAX : m_pThroughput_subbanda_trunc[i] / m_dResources_medio;
          }

        }
    }
}

// asignacion de usuario, con el numero de subbandas indicado en Nsubbandas_asign
// si wideband == true, se asigna al usuario toda la banda de frecuencias; Nsubbandas_asign y Subbandas_asign no aplican
// si wideband == false aplican Nsubbandas_asign y Subbandas_asign
void CUsuario::ScheduleUser(bool wideband, int Nsubbandas_asign, int* Subbandas_asign)
{
    int i;
    double throughput;

    // actualiza ventanas
    if (wideband)
    {
        // se asigna toda la banda al usuario
        m_pVentanaThroughput[m_iPosVentana] = m_dThroughput_wideband;
        m_pVentanaResources[m_iPosVentana] = 1;     // cada subtrama asignada se indica con 1, en caso contrario con 0 (si wideband, si no se asigna el número de subbandas asig)
    }
    else
    {
        throughput = 0;
        for (i = 0; i < Nsubbandas_asign; i++)
            throughput += (m_itruncCQI)? m_pThroughput_subbanda_trunc[ Subbandas_asign[i] ] :
                                         m_pThroughput_subbanda[ Subbandas_asign[i] ];
        m_pVentanaThroughput[m_iPosVentana] = throughput;
        m_pVentanaResources[m_iPosVentana] = Nsubbandas_asign;
    }

    ActualizaThroughput(wideband);

    m_iNsubframes++; // actualizamos el numero de subframes transcurridas
}


void CUsuario::NoScheduleUser(bool wideband)
{
//    int i;

    // actualiza ventanas
    m_pVentanaThroughput[m_iPosVentana] = 0;
    m_pVentanaResources[m_iPosVentana] = 0;

    ActualizaThroughput(wideband);

    m_iNsubframes++;
}

//// calcula las mejores m_iNsubbandas_CQI subbandas en m_pBestSubbandas
//void CUsuario::CalcBestSubbandas()
//{
//    int i, j;
//
//    memcpy(m_pGanancias_ord, m_pGanancias, m_iNsubbandas_tot * sizeof(double));
//    qsort((void *)m_pGanancias_ord, (size_t)m_iNsubbandas_tot, sizeof(double), compara_double_qsort);
//    for (i = 0; i < m_iNsubbandas_CQI; i++)
//    {
//        for (j = 0; j < m_iNsubbandas_tot; j++)
//        {
//            if (m_pGanancias[j] == m_pGanancias_ord[m_iNsubbandas_tot-1 - i])
//            {
//                m_pBestSubbandas[i] = j;
//                break;
//            }
//        }
//    }
//}

//// devuelve el throughput de las m_iNsubbandas_CQI mejores subbandas, y 0 en el resto (UE-selected subband feedback)
//void CUsuario::GetThroughputBestSubbandas(double* throughput)
//{
//    if (throughput != NULL)
//    {
//        memset(throughput, 0, m_iNsubbandas_tot * sizeof(double));
//        for (int i = 0; i < m_iNsubbandas_CQI; i++)
//            throughput[m_pBestSubbandas[i]] = m_pThroughput_subbanda[m_pBestSubbandas[i]];
//    }
//}
//
//// devuelve el throughput relativo al throughput medio, de las m_iNsubbandas_CQI mejores subbandas, y 0 en el resto (UE-selected subband feedback)
//void CUsuario::GetRelativeThroughputBestSubbandas(double* relative_throughput)
//{
//    if (relative_throughput != NULL)
//    {
//        memset(relative_throughput, 0, m_iNsubbandas_tot * sizeof(double));
//        for (int i = 0; i < m_iNsubbandas_CQI; i++)
//            relative_throughput[m_pBestSubbandas[i]] = m_pRelative_throughput[m_pBestSubbandas[i]];
//    }
//}
//
//// devuelve los valores de throughput de subbanda relativos a los recursos medios, de las m_iNsubbandas_CQI mejores subbandas, y 0 en el resto (UE-selected subband feedback)
//void CUsuario::GetResourceRelativeThroughputBestSubbandas(double* resource_relative_throughput)
//{
//    if (resource_relative_throughput != NULL)
//    {
//        memset(resource_relative_throughput, 0, m_iNsubbandas_tot * sizeof(double));
//        for (int i = 0; i < m_iNsubbandas_CQI; i++)
//            resource_relative_throughput[m_pBestSubbandas[i]] = m_pResourceRelative_throughput[m_pBestSubbandas[i]];
//    }
//}
//
//// idem, truncado
//void CUsuario::GetThroughputBestSubbandasTrunc(double* throughput_trunc)
//{
//    if (throughput_trunc != NULL)
//    {
//        memset(throughput_trunc, 0, m_iNsubbandas_tot * sizeof(double));
//        for (int i = 0; i < m_iNsubbandas_CQI; i++)
//            throughput_trunc[m_pBestSubbandas[i]] = m_pThroughput_subbanda_trunc[m_pBestSubbandas[i]];
//    }
//}
//
//// idem, truncado
//void CUsuario::GetRelativeThroughputBestSubbandasTrunc(double* relative_throughput_trunc)
//{
//    if (relative_throughput_trunc != NULL)
//    {
//        memset(relative_throughput_trunc, 0, m_iNsubbandas_tot * sizeof(double));
//        for (int i = 0; i < m_iNsubbandas_CQI; i++)
//            relative_throughput_trunc[m_pBestSubbandas[i]] = m_pRelative_throughput_trunc[m_pBestSubbandas[i]];
//    }
//}
//
//// idem, truncado
//void CUsuario::GetResourceRelativeThroughputBestSubbandasTrunc(double* resource_relative_throughput_trunc)
//{
//    if (resource_relative_throughput_trunc != NULL)
//    {
//        memset(resource_relative_throughput_trunc, 0, m_iNsubbandas_tot * sizeof(double));
//        for (int i = 0; i < m_iNsubbandas_CQI; i++)
//            resource_relative_throughput_trunc[m_pBestSubbandas[i]] = m_pResourceRelative_throughput_trunc[m_pBestSubbandas[i]];
//    }
//}
